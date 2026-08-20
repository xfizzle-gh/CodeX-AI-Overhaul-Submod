from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BREED = ROOT / "resource/set/breed/isolation_test"
ROSTER = ROOT / "resource/set/multiplayer/units/only_roster_conquest.set"
TRIG = ROOT / "resource/map/multi/ce/ce_triggers.inc"


class AioIsoCivilianBehaviourTests(unittest.TestCase):
    def test_three_breeds_exist(self) -> None:
        self.assertTrue((BREED / "aio_iso_hostile_soldier.set").is_file())
        self.assertTrue((BREED / "aio_iso_hostile_civ.set").is_file())
        self.assertTrue((BREED / "aio_iso_hostile_civ_rifle.set").is_file())

    def test_soldier_and_civ_rifle_differ_only_in_behaviour(self) -> None:
        soldier = (BREED / "aio_iso_hostile_soldier.set").read_text(encoding="utf-8")
        armed = (BREED / "aio_iso_hostile_civ_rifle.set").read_text(encoding="utf-8")
        self.assertIn("{behaviour soldier}", soldier)
        self.assertIn("{behaviour civilian}", armed)
        self.assertEqual(
            soldier.replace("{behaviour soldier}", "{behaviour civilian}", 1),
            armed,
        )
        self.assertNotIn("{tags", soldier)
        self.assertNotIn("{tags", armed)

    def test_unarmed_civ_is_inventory_variant_only(self) -> None:
        civ = (BREED / "aio_iso_hostile_civ.set").read_text(encoding="utf-8")
        armed = (BREED / "aio_iso_hostile_civ_rifle.set").read_text(encoding="utf-8")
        self.assertIn("{behaviour civilian}", civ)
        self.assertNotIn("mars_l", civ)
        self.assertIn("{item \"mars_l\" filled}", armed)
        blob = civ + armed
        self.assertNotIn("aio_marker_morale", blob)
        self.assertNotIn('{player "0"}', blob)
        self.assertNotIn("{control AI}", blob)
        self.assertNotIn("ai_ignore", blob)
        self.assertNotIn("aio_morale", blob)

    def test_not_wired_into_conquest_or_ce(self) -> None:
        if ROSTER.is_file():
            self.assertNotIn("aio_iso_hostile", ROSTER.read_text(encoding="utf-8"))
        if TRIG.is_file():
            self.assertNotIn("aio_iso_hostile", TRIG.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
