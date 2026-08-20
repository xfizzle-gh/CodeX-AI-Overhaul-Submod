from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BREED = ROOT / "resource/set/breed/test"
ROSTER = ROOT / "resource/set/multiplayer/units/only_roster_conquest.set"
TRIG = ROOT / "resource/map/multi/ce/ce_triggers.inc"


class AioIsoCivilianBehaviourTests(unittest.TestCase):
    def test_three_breeds_exist(self) -> None:
        self.assertTrue((BREED / "aio_iso_hostile_soldier.set").is_file())
        self.assertTrue((BREED / "aio_iso_hostile_civ.set").is_file())
        self.assertTrue((BREED / "aio_iso_hostile_civ_rifle.set").is_file())

    def test_only_behaviour_and_inventory_differ(self) -> None:
        soldier = (BREED / "aio_iso_hostile_soldier.set").read_text(encoding="utf-8")
        civ = (BREED / "aio_iso_hostile_civ.set").read_text(encoding="utf-8")
        armed = (BREED / "aio_iso_hostile_civ_rifle.set").read_text(encoding="utf-8")
        self.assertIn("{behaviour soldier}", soldier)
        self.assertIn("{behaviour civilian}", civ)
        self.assertIn("{behaviour civilian}", armed)
        self.assertIn('{skin "nrf_1"}', soldier)
        self.assertIn('{skin "nrf_1"}', civ)
        self.assertIn('{skin "nrf_1"}', armed)
        self.assertIn("{item \"mars_l\" filled}", soldier)
        self.assertIn("{item \"mars_l\" filled}", armed)
        self.assertNotIn("mars_l", civ)
        self.assertNotIn("aio_marker_morale", soldier)
        self.assertNotIn("aio_marker_morale", civ)
        self.assertNotIn("aio_marker_morale", armed)
        self.assertNotIn('{player "0"}', soldier + civ + armed)
        self.assertNotIn("{control AI}", soldier + civ + armed)
        self.assertNotIn("ai_ignore", soldier + civ + armed)
        self.assertNotIn("aio_morale", soldier + civ + armed)

    def test_not_wired_into_conquest_or_ce(self) -> None:
        if ROSTER.is_file():
            self.assertNotIn("aio_iso_hostile", ROSTER.read_text(encoding="utf-8"))
        if TRIG.is_file():
            self.assertNotIn("aio_iso_hostile", TRIG.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
