from __future__ import annotations

import importlib.util
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec and spec.loader
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


visible = load_module(
    "motor_visible_overlay",
    ROOT / "tools" / "apply_motor_visible_package_overlay.py",
)
timing = load_module(
    "runtime_motor_timing",
    ROOT / "tools" / "apply_runtime_proven_motor_60s.py",
)
defender = load_module(
    "friendly_defender_motor",
    ROOT / "tools" / "apply_friendly_defender_motor_one_shot.py",
)

FILES = (
    "attack_support_waves.inc",
    "enemy_attack_support.inc",
    "defense_support_waves.inc",
    "dcg_vars.inc",
)


class FriendlyDefenderMotorOneShotTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        self.multi = self.root / "resource/map/multi"
        self.multi.mkdir(parents=True)
        for name in FILES:
            shutil.copy2(ROOT / "resource/map/multi" / name, self.multi / name)

        # Recreate the exact deployed runtime-proven baseline before adding stage 2.
        visible.patch_multi_root(self.multi)
        timing.patch_multi_root(self.multi)
        visible.validate_multi_root(self.multi)
        timing.validate_multi_root(self.multi)

    def tearDown(self) -> None:
        self.temp.cleanup()

    @staticmethod
    def reverse_common(block: str) -> str:
        replacements = (
            ("def_sup_motor_g4", "attack_support_g4"),
            ("def_sup_motor_g3", "attack_support_g3"),
            ("def_sup_motor_g2", "attack_support_g2"),
            ("def_sup_motor_g1", "attack_support_g1"),
            ("defense_support_transferred", "attack_support_transferred"),
            ("def_sup_motor_leaving", "am_motor_leaving"),
            ("def_sup_motor_pax", "attack_support_motor_pax"),
            ("def_sup_motor_hull", "attack_support_motor_hull"),
            ("def_sup_motor_flag", "attack_support_flag1"),
            ("def_sup_src", "attack_support_src"),
            ("def_sup_deploy", "attack_support_deploy"),
        )
        for old, new in replacements:
            block = block.replace(old, new)
        return block

    def test_existing_runtime_proven_engines_remain_byte_identical(self) -> None:
        attack_before = (self.multi / "attack_support_waves.inc").read_bytes()
        enemy_before = (self.multi / "enemy_attack_support.inc").read_bytes()

        defender.apply(self.root)
        defender.validate(self.root)

        self.assertEqual(
            attack_before,
            (self.multi / "attack_support_waves.inc").read_bytes(),
        )
        self.assertEqual(
            enemy_before,
            (self.multi / "enemy_attack_support.inc").read_bytes(),
        )

    def test_defender_lifecycle_is_exact_transformation_of_proven_path(self) -> None:
        defender.apply(self.root)
        attack = (self.multi / "attack_support_waves.inc").read_text(
            encoding="utf-8-sig"
        )
        defense = (self.multi / "defense_support_waves.inc").read_text(
            encoding="utf-8-sig"
        )

        source_placer = defender.paren_block(
            attack, '(define "as_place_motor_visible"'
        )[2]
        derived_placer = defender.paren_block(
            defense, '(define "ds_place_motor_visible"'
        )[2]
        restored_placer = self.reverse_common(derived_placer).replace(
            '(define "ds_place_motor_visible"',
            '(define "as_place_motor_visible"',
            1,
        )
        self.assertEqual(source_placer, restored_placer)

        source_finisher = defender.paren_block(attack, '(define "as_finish_motor"')[2]
        derived_finisher = defender.paren_block(defense, '(define "ds_finish_motor"')[2]
        restored_finisher = self.reverse_common(derived_finisher)
        restored_finisher = restored_finisher.replace(
            '(define "ds_finish_motor"', '(define "as_finish_motor"', 1
        ).replace(
            '("ds_own_to_defenderbot")', '("am_own_to_support")', 1
        ).replace("90s later", "45s later")
        self.assertEqual(source_finisher, restored_finisher)

        source_cleanup = defender.brace_block(
            attack, '{"attack_support/motor_cleanup"'
        )[2]
        derived_cleanup = defender.brace_block(
            defense, '{"defense_support/motor_cleanup"'
        )[2]
        restored_cleanup = self.reverse_common(derived_cleanup)
        restored_cleanup = restored_cleanup.replace(
            '{"defense_support/motor_cleanup"',
            '{"attack_support/motor_cleanup"',
            1,
        ).replace(
            '{var "user_is_defender$"} {op "=="} {value 1}',
            '{var "user_is_defender$"} {op "=="} {value 0}',
            1,
        )
        self.assertEqual(source_cleanup, restored_cleanup)

        for faction in defender.FACTIONS:
            source = defender.brace_block(
                attack, f'{{"attack_support/ally_{faction}_motor"'
            )[2]
            derived = defender.brace_block(
                defense, f'{{"defense_support/ally_{faction}_motor"'
            )[2]
            restored = self.reverse_common(derived)
            replacements = (
                (f'{{"defense_support/ally_{faction}_motor"', f'{{"attack_support/ally_{faction}_motor"'),
                ('{var "user_is_defender$"} {op "=="} {value 1}', '{var "user_is_defender$"} {op "=="} {value 0}'),
                ('defense_support_wave_cmd', 'attack_support_wave_cmd'),
                ('id_defenderbot', 'id_attack_support'),
                ('defense_support_motor_left', 'attack_support_motor_left'),
                ('("ds_announce_wave")', '("as_announce_motor")'),
                ('DEFENSE SUPPORT MOTORIZED INSERT', 'ATTACK SUPPORT MOTORIZED INSERT'),
                ('("ds_place_motor_visible")', '("as_place_motor_visible")'),
                ('("ds_finish_motor")', '("as_finish_motor")'),
            )
            for old, new in replacements:
                restored = restored.replace(old, new, 1)
            self.assertEqual(source, restored, faction)

    def test_one_shot_contract_and_variables(self) -> None:
        defender.apply(self.root)
        defense = (self.multi / "defense_support_waves.inc").read_text(
            encoding="utf-8-sig"
        )
        variables = (self.multi / "dcg_vars.inc").read_text(encoding="utf-8-sig")
        test = defender.brace_block(
            defense, '{"defense_support/motor_test"'
        )[2]
        self.assertEqual(test.count('{"delay" {time 30}}'), 1)
        self.assertEqual(test.count('("ds_poke_faction_motor")'), 1)
        self.assertIn(
            '{var "defense_support_motor_left$"} {op "="} {value 1}', test
        )
        self.assertNotIn("motor_clock", defense)
        self.assertNotIn("180", test)
        self.assertIn('{"defense_support_motor_left"}', variables)
        self.assertIn('{"defense_support_motor_test_done"}', variables)

    def test_overlay_is_idempotent(self) -> None:
        defender.apply(self.root)
        first = {name: (self.multi / name).read_bytes() for name in FILES}
        changed = defender.apply(self.root)
        second = {name: (self.multi / name).read_bytes() for name in FILES}
        self.assertEqual(changed, [])
        self.assertEqual(first, second)

    def test_check_only_does_not_write(self) -> None:
        before = {name: (self.multi / name).read_bytes() for name in FILES}
        changed = defender.apply(self.root, check_only=True)
        after = {name: (self.multi / name).read_bytes() for name in FILES}
        self.assertEqual(
            {path.name for path in changed},
            {"defense_support_waves.inc", "dcg_vars.inc"},
        )
        self.assertEqual(before, after)


if __name__ == "__main__":
    unittest.main()
