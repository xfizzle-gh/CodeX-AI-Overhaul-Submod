from __future__ import annotations

import importlib.util
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PRODUCTION_SCRIPT = ROOT / "tools" / "apply_canonical_motor_production_overlay.py"
HOTFIX_SCRIPT = ROOT / "tools" / "apply_motor_linked_seat_hotfix.py"
FILES = (
    "resource/map/multi/faction_support_templates.inc",
    "resource/map/multi/dcg_vars.inc",
    "resource/map/multi/attack_support_waves.inc",
    "resource/map/multi/defense_support_waves.inc",
    "resource/map/multi/enemy_attack_support.inc",
    "resource/map/multi/enemy_defense_support.inc",
)


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    assert spec and spec.loader
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


production = load_module("canonical_motor_for_seat_test", PRODUCTION_SCRIPT)
hotfix = load_module("motor_linked_seat_hotfix", HOTFIX_SCRIPT)


class MotorLinkedSeatHotfixTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp = tempfile.TemporaryDirectory()
        self.root = Path(self.temp.name)
        for relative in FILES:
            source = ROOT / relative
            target = self.root / relative
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, target)

    def tearDown(self) -> None:
        self.temp.cleanup()

    @property
    def expected_changes(self) -> set[str]:
        return {
            hotfix.VARS_PATH.as_posix(),
            *(engine.relative_path for engine in hotfix.ENGINES),
        }

    def build_production_then_apply_hotfix(self) -> None:
        production.apply(self.root)
        changed = hotfix.apply(self.root)
        self.assertEqual(set(changed), self.expected_changes)

    def test_all_four_engines_use_isolated_motor_lane(self) -> None:
        self.build_production_then_apply_hotfix()

        templates = (self.root / hotfix.TEMPLATE_PATH).read_text(
            encoding="utf-8-sig"
        )
        hotfix.validate_cab_links(templates)

        variables = (self.root / hotfix.VARS_PATH).read_text(encoding="utf-8-sig")
        hotfix.validate_vars(variables)

        for engine in hotfix.ENGINES:
            text = (self.root / engine.relative_path).read_text(encoding="utf-8-sig")
            hotfix.validate_engine(text, engine)

            clock = hotfix.brace_block(
                text, f'{{"{engine.namespace}/motor_clock"'
            )[2]
            self.assertIn(f'{{var "{engine.dispatch_var}"}}', clock)
            self.assertNotIn(f'{{var "{engine.wave_var}"}}', clock)
            self.assertRegex(
                clock,
                r'\{"set_i"\s+\{var\s+"'
                + engine.dispatch_var.replace("$", r"\$")
                + r'"\}\s+\{op\s+"="\}\s+\{value\s+1\}\}',
            )

            placer = hotfix.paren_block(
                text, f'(define "{engine.placer_macro}"'
            )[2]
            self.assertEqual(placer.count(f"{{tag {engine.hull_tag}}}"), 3)
            self.assertNotIn(f"{{tag {engine.deploy_tag}}}", placer)

            for faction in hotfix.FACTIONS:
                trigger = hotfix.brace_block(
                    text,
                    f'{{"{engine.trigger_pattern.format(faction=faction)}"',
                )[2]
                self.assertIn(f'{{var "{engine.dispatch_var}"}}', trigger)
                self.assertNotIn(f'{{var "{engine.wave_var}"}}', trigger)
                self.assertNotIn(engine.deploy_tag, trigger)
                self.assertIn(engine.transfer_tag, trigger)

            finisher = hotfix.paren_block(
                text, f'(define "{engine.finisher}"'
            )[2]
            first_actor_at = finisher.index('{"actor_state"')
            first_actor = hotfix.brace_block(
                finisher, '{"actor_state"', search_from=first_actor_at
            )[2]
            self.assertIn(f"{{tag {engine.hull_tag}}}", first_actor)
            self.assertNotIn(f"{{tag {engine.deploy_tag}}}", first_actor)

            emit_at = finisher.index('{"emit"')
            post_emit = finisher[emit_at:]
            passenger_actor_at = post_emit.index('{"actor_state"')
            passenger_actor = hotfix.brace_block(
                post_emit, '{"actor_state"', search_from=passenger_actor_at
            )[2]
            self.assertIn(f"{{tag {engine.passenger_tag}}}", passenger_actor)

    def test_opening_infantry_command_cannot_block_first_motor(self) -> None:
        self.build_production_then_apply_hotfix()

        for engine in hotfix.ENGINES:
            text = (self.root / engine.relative_path).read_text(encoding="utf-8-sig")
            clock = hotfix.brace_block(
                text, f'{{"{engine.namespace}/motor_clock"'
            )[2]

            # The motor clock owns its own one-bit command lane. It must not test or
            # write the infantry wave command, so a simultaneous opening wave cannot
            # defer the first 30-second motor dispatch.
            self.assertNotIn(engine.wave_var, clock)
            self.assertIn(engine.dispatch_var, clock)

            for faction in hotfix.FACTIONS:
                trigger = hotfix.brace_block(
                    text,
                    f'{{"{engine.trigger_pattern.format(faction=faction)}"',
                )[2]
                self.assertNotIn(engine.wave_var, trigger)
                self.assertIn(engine.dispatch_var, trigger)

    def test_motor_packages_never_enter_generic_infantry_staging(self) -> None:
        self.build_production_then_apply_hotfix()

        for engine in hotfix.ENGINES:
            text = (self.root / engine.relative_path).read_text(encoding="utf-8-sig")
            for faction in hotfix.FACTIONS:
                trigger = hotfix.brace_block(
                    text,
                    f'{{"{engine.trigger_pattern.format(faction=faction)}"',
                )[2]
                self.assertNotIn(engine.deploy_tag, trigger)
                self.assertIn(engine.transfer_tag, trigger)

    def test_hotfix_is_idempotent(self) -> None:
        self.build_production_then_apply_hotfix()
        before = {relative: (self.root / relative).read_bytes() for relative in FILES}
        changed = hotfix.apply(self.root)
        after = {relative: (self.root / relative).read_bytes() for relative in FILES}
        self.assertEqual(changed, [])
        self.assertEqual(before, after)

    def test_check_mode_reports_without_writing(self) -> None:
        production.apply(self.root)
        before = {relative: (self.root / relative).read_bytes() for relative in FILES}
        changed = hotfix.apply(self.root, check_only=True)
        after = {relative: (self.root / relative).read_bytes() for relative in FILES}
        self.assertEqual(set(changed), self.expected_changes)
        self.assertEqual(before, after)


if __name__ == "__main__":
    unittest.main()
