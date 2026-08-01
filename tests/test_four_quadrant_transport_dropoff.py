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
    "dropoff_visible",
    ROOT / "tools" / "apply_motor_visible_package_overlay.py",
)
timing = load_module(
    "dropoff_timing",
    ROOT / "tools" / "apply_runtime_proven_motor_60s.py",
)
defender = load_module(
    "dropoff_defender",
    ROOT / "tools" / "apply_friendly_defender_motor_one_shot.py",
)
dropoff = load_module(
    "dropoff_overlay",
    ROOT / "tools" / "apply_four_quadrant_transport_dropoff_fixed.py",
)

FILES = (
    "attack_support_waves.inc",
    "enemy_defense_support.inc",
    "defense_support_waves.inc",
    "enemy_attack_support.inc",
    "faction_support_templates.inc",
    "dcg_vars.inc",
)


class FourQuadrantTransportDropoffTests(unittest.TestCase):
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

    def engine_block(self, engine) -> str:
        text = self.text(engine.filename)
        bounds = dropoff.marked_bounds(
            text,
            dropoff.base.ENGINE_BEGIN.format(key=engine.key.upper()),
            dropoff.base.ENGINE_END.format(key=engine.key.upper()),
        )
        self.assertIsNotNone(bounds)
        assert bounds is not None
        return text[bounds[0] : bounds[1]]

    def test_all_four_engines_and_factions_use_single_dropoff(self) -> None:
        dropoff.apply(self.root)
        dropoff.validate(self.root)

        for engine in dropoff.ENGINES:
            block = self.engine_block(engine)
            for faction in dropoff.FACTIONS:
                self.assertEqual(
                    block.count(f'{{"{engine.trigger_ns}/normal_transport_{faction}"'),
                    1,
                )
            self.assertEqual(
                block.count(f'{{"{engine.trigger_ns}/normal_transport_dropoff"'),
                1,
            )
            self.assertNotIn("/normal_transport_patrol", block)

    def test_passengers_are_held_then_released_at_arrival(self) -> None:
        dropoff.apply(self.root)
        for engine in dropoff.ENGINES:
            block = self.engine_block(engine)
            self.assertEqual(block.count(dropoff.HOLD_MARKER), 4)
            self.assertEqual(block.count(dropoff.DROPOFF_MARKER), 1)
            self.assertIn("{ai_move {mode disable}}", block)
            self.assertIn("{movement {speed stop}", block)
            self.assertIn("{emit {mode passengers}}", block)
            self.assertIn("{ai_move {mode enable}}", block)
            self.assertIn("{action advance}", block)
            self.assertIn(f"{{distance {dropoff.ARRIVAL_DISTANCE}}}", block)
            self.assertIn(
                f'{{waypoint "transport_patrol_flag_{engine.start_step}"}}',
                block,
            )

    def test_truck_has_no_second_route_or_cleanup(self) -> None:
        dropoff.apply(self.root)
        for engine in dropoff.ENGINES:
            block = self.engine_block(engine)
            waypoint = f'{{waypoint "transport_patrol_flag_{engine.start_step}"}}'
            # One move at dispatch and one same-point order at arrival before speed stop.
            self.assertEqual(block.count(waypoint), 2)
            for forbidden in (
                "exit_motor_to_origin",
                "motor_leaving",
                '{"delete"',
                '{"delay" {time 75}}',
                "normal_transport_patrol",
            ):
                self.assertNotIn(forbidden, block)

    def test_enemy_packages_keep_all_eight_linked_passenger_seats(self) -> None:
        dropoff.apply(self.root)
        templates = self.text("faction_support_templates.inc")
        for faction, cfg in dropoff.FACTIONS.items():
            hull = int(cfg["enemy_start"])
            for seat in range(1, 9):
                self.assertIn(f'{{0x{hull:x} "seat{seat}"}}', templates)

    def test_overlay_is_idempotent_and_check_only_is_read_only(self) -> None:
        before = {name: (self.multi / name).read_bytes() for name in FILES}
        predicted = dropoff.apply(self.root, check_only=True)
        self.assertEqual({path.name for path in predicted}, set(FILES))
        self.assertEqual(
            before,
            {name: (self.multi / name).read_bytes() for name in FILES},
        )

        dropoff.apply(self.root)
        first = {name: (self.multi / name).read_bytes() for name in FILES}
        self.assertEqual(dropoff.apply(self.root), [])
        self.assertEqual(
            first,
            {name: (self.multi / name).read_bytes() for name in FILES},
        )


if __name__ == "__main__":
    unittest.main()
