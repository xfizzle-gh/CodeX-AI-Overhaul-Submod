from __future__ import annotations

import importlib.util
import math
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
    "fourq_visible",
    ROOT / "tools" / "apply_motor_visible_package_overlay.py",
)
timing = load_module(
    "fourq_timing",
    ROOT / "tools" / "apply_runtime_proven_motor_60s.py",
)
defender = load_module(
    "fourq_defender",
    ROOT / "tools" / "apply_friendly_defender_motor_one_shot.py",
)
quadrants = load_module(
    "fourq_transport_fixed",
    ROOT / "tools" / "apply_four_quadrant_transport_patrol_fixed.py",
)
perimeters = load_module(
    "fourq_perimeters_fixed",
    ROOT / "tools" / "apply_transport_flag_perimeter_waypoints_fixed.py",
)

FILES = (
    "attack_support_waves.inc",
    "enemy_defense_support.inc",
    "defense_support_waves.inc",
    "enemy_attack_support.inc",
    "faction_support_templates.inc",
    "dcg_vars.inc",
)


class FourQuadrantTransportPatrolTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        self.multi = self.root / "resource/map/multi"
        self.multi.mkdir(parents=True)
        for name in FILES:
            shutil.copy2(ROOT / "resource/map/multi" / name, self.multi / name)

        visible.patch_multi_root(self.multi)
        timing.patch_multi_root(self.multi)
        defender.apply(self.root)

    def tearDown(self) -> None:
        self.temp.cleanup()

    def text(self, name: str) -> str:
        return (self.multi / name).read_text(encoding="utf-8-sig")

    def test_all_four_engines_and_all_four_factions_are_present(self) -> None:
        quadrants.apply(self.root)
        quadrants.validate(self.root)
        for engine in quadrants.ENGINES:
            text = self.text(engine.filename)
            self.assertEqual(
                text.count(quadrants.ENGINE_BEGIN.format(key=engine.key.upper())), 1
            )
            for faction in quadrants.FACTIONS:
                self.assertEqual(
                    text.count(f'{{"{engine.trigger_ns}/normal_transport_{faction}"'),
                    1,
                )
            self.assertEqual(
                text.count(f'{{"{engine.trigger_ns}/normal_transport_patrol"'), 1
            )

    def test_normal_paths_have_no_scripted_dismount_or_withdrawal(self) -> None:
        quadrants.apply(self.root)
        for engine in quadrants.ENGINES:
            text = self.text(engine.filename)
            bounds = quadrants.marked_bounds(
                text,
                quadrants.ENGINE_BEGIN.format(key=engine.key.upper()),
                quadrants.ENGINE_END.format(key=engine.key.upper()),
            )
            self.assertIsNotNone(bounds)
            assert bounds is not None
            block = text[bounds[0] : bounds[1]]
            self.assertIn("{action move}", block)
            self.assertIn("{distance 80}", block)
            for step in range(1, 6):
                self.assertIn(f'{{waypoint "transport_patrol_flag_{step}"}}', block)
            for forbidden in (
                '{"emit"',
                "{mode passengers}",
                "exit_motor_to_origin",
                "motor_leaving",
                '{"delete"',
                '{"delay" {time 75}}',
            ):
                self.assertNotIn(forbidden, block)

    def test_old_timer_driven_motor_dispatches_are_disabled(self) -> None:
        quadrants.apply(self.root)
        attack = self.text("attack_support_waves.inc")
        defense = self.text("defense_support_waves.inc")
        enemy = self.text("enemy_attack_support.inc")
        self.assertNotIn('{"attack_support/motor_test"', attack)
        self.assertNotIn('{"defense_support/motor_test"', defense)
        self.assertNotIn('{"enemy_attack/motor_test"', enemy)
        for text, var in (
            (attack, "attack_support_motor_left"),
            (attack, "attack_support_motor_test"),
            (enemy, "enemy_attack_motor_left"),
            (enemy, "enemy_attack_motor_test"),
        ):
            self.assertEqual(
                text.count(f'{{"set_i" {{var "{var}$"}} {{op "="}} {{value 0}}}}'),
                1,
            )

    def test_enemy_faction_packages_are_linked_for_all_trucks(self) -> None:
        quadrants.apply(self.root)
        templates = self.text("faction_support_templates.inc")
        for faction, cfg in quadrants.FACTIONS.items():
            start = int(cfg["enemy_start"])
            self.assertIn(f"0x{start:x}", templates)
            self.assertIn(f'"transport_enemy_{faction}"', templates)
            self.assertIn(f'{{0x{start:x} "driver"}}', templates)
            self.assertIn(f'{{0x{start:x} "commander"}}', templates)
            for seat in range(1, 9):
                self.assertIn(f'{{0x{start:x} "seat{seat}"}}', templates)

    def test_vars_and_overlay_are_idempotent(self) -> None:
        before = {name: (self.multi / name).read_bytes() for name in FILES}
        predicted = quadrants.apply(self.root, check_only=True)
        self.assertEqual({path.name for path in predicted}, set(FILES))
        self.assertEqual(
            before, {name: (self.multi / name).read_bytes() for name in FILES}
        )
        quadrants.apply(self.root)
        first = {name: (self.multi / name).read_bytes() for name in FILES}
        self.assertEqual(quadrants.apply(self.root), [])
        self.assertEqual(
            first, {name: (self.multi / name).read_bytes() for name in FILES}
        )

    def make_maps(self, flag_count: int = 3) -> None:
        for index in range(14):
            directory = self.multi / f"dcg_[cwa71]_fixture_{index:02d}"
            directory.mkdir(parents=True)
            flags = []
            for flag in range(flag_count):
                x = 1000.0 + flag * 800.0
                y = -1200.0 + flag * 500.0
                flags.append(
                    f'\t{{Entity "flag_point_campaign_{flag + 1}" 0x{0x100 + flag:x}\n'
                    f'\t\t{{Position {x:.2f} {y:.2f} 0.00}}\n'
                    f'\t}}'
                )
            text = (
                "{mission\n"
                "\t{entities\n"
                + "\n".join(flags)
                + "\n\t}\n"
                "\t\t{waypoints\n"
                "\t\t}\n"
                "}\n"
            )
            (directory / "campaign_capture_the_flag.mi").write_text(text, encoding="utf-8")

    def test_perimeter_waypoints_stay_clear_of_flag_posts(self) -> None:
        self.make_maps(flag_count=2)
        changed = perimeters.apply(self.root)
        self.assertEqual(len(changed), 14)
        perimeters.validate(self.root)
        sample = next(iter(perimeters.map_files(self.root)))
        text = sample.read_text(encoding="utf-8-sig")
        flags = perimeters.extract_flags(text, str(sample))
        points = perimeters.parse_waypoints(text, str(sample))
        self.assertEqual(len(points), 5)
        for slot, point in enumerate(points):
            source = flags[slot % len(flags)]
            centre_distance = math.hypot(point.x - source.x, point.y - source.y)
            self.assertAlmostEqual(centre_distance, perimeters.OFFSET, delta=0.1)
            self.assertGreaterEqual(
                centre_distance - perimeters.RADIUS,
                perimeters.CLOSEST_TO_FLAG - 0.1,
            )
        first = {path: path.read_bytes() for path in perimeters.map_files(self.root)}
        self.assertEqual(perimeters.apply(self.root), [])
        self.assertEqual(
            first, {path: path.read_bytes() for path in perimeters.map_files(self.root)}
        )


if __name__ == "__main__":
    unittest.main()
