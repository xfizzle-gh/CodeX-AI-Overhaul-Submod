from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BEH = ROOT / "resource/map/multi/ce/ce_broken_behavior_triggers.inc"
CAMP = ROOT / "resource/map/multi/ce/ce_pow_camp_triggers.inc"
MANAGE = ROOT / "resource/map/multi/ce/ce_pow_camp_manage_triggers.inc"
TRIG = ROOT / "resource/map/multi/ce/ce_triggers.inc"
VARS = ROOT / "resource/map/multi/ce/ce_vars.inc"
HUMAN = ROOT / "resource/set/interaction_entity/human_ce.inc"
POT = ROOT / "localizations/default/interface/text/mission/multi/ce_mission_messages.pot"


class CePowCampManageTests(unittest.TestCase):
    def test_manage_is_wired(self) -> None:
        self.assertTrue(MANAGE.is_file())
        self.assertIn("ce_pow_camp_manage_triggers.inc", TRIG.read_text(encoding="utf-8"))
        vars_text = VARS.read_text(encoding="utf-8")
        self.assertIn('{"aio_pow_camp_enemy_ready"}', vars_text)
        self.assertIn('{"aio_pow_camp_announced"}', vars_text)
        self.assertNotIn('{"aio_pow_camp_count"}', vars_text)
        self.assertIn("ce_pow_camp_held", POT.read_text(encoding="utf-8"))

    def test_enemy_camp_is_opposite_spawn(self) -> None:
        manage = MANAGE.read_text(encoding="utf-8")
        s1 = manage.split("spawn_enemy_pow_camp_s1", 1)[1].split("spawn_enemy_pow_camp_s2", 1)[0]
        s2 = manage.split("spawn_enemy_pow_camp_s2", 1)[1].split("surrender_arrive_enemy_camp", 1)[0]
        self.assertIn("{tag spawn_a}", s1)
        self.assertNotIn("{tag spawn_b}", s1)
        self.assertIn("{tag spawn_b}", s2)
        self.assertNotIn("{tag spawn_a}", s2)
        self.assertIn("{tag_add aio_pow_camp_enemy}", s1)
        arrive = manage.split("surrender_arrive_enemy_camp", 1)[1].split("broken/pow_announce", 1)[0]
        self.assertIn("{tag_add prisoner_in_enemy_camp}", arrive)
        self.assertIn("{meters 25}", arrive)
        self.assertNotIn("{meters 80}", arrive)
        self.assertNotIn("aio_pow_camp_enemy_ready$", arrive)
        self.assertNotIn('{"delete"', arrive)

    def test_no_auto_convert_or_release(self) -> None:
        manage = MANAGE.read_text(encoding="utf-8")
        human = HUMAN.read_text(encoding="utf-8")
        self.assertNotIn("{effect aio_pow_convert}", manage)
        self.assertNotIn("{effect aio_pow_camped}", manage)
        self.assertNotIn("aio_pow_convert_ready", manage)
        self.assertNotIn("id_1st_player$", manage)
        self.assertNotIn('{player "1"}', manage)
        self.assertNotIn('{on "aio_pow_convert"', human)
        self.assertNotIn('{on "aio_pow_release"', human)
        self.assertNotIn('{on "aio_pow_camped"', human)
        self.assertIn("broken/pow_announce", manage)
        self.assertIn("ce_pow_camp_held", manage)
        evac = BEH.read_text(encoding="utf-8").split("broken/surrender_evacuate", 1)[1].split(
            "broken/surrender_arrive_a", 1
        )[0]
        self.assertIn("{tag aio_pow_camp_enemy}", evac)
        self.assertIn("{tag_add aio_morale_surrender_to_enemy_camp}", evac)
        self.assertNotIn("aio_pow_camp_enemy_ready$", evac)

    def test_camp_files_do_not_set_player_zero(self) -> None:
        text = MANAGE.read_text(encoding="utf-8") + CAMP.read_text(encoding="utf-8")
        self.assertNotIn('{player "0"}', text)
        self.assertNotIn("{control AI}", text)

    def test_captor_stamp_is_before_p0(self) -> None:
        present = BEH.read_text(encoding="utf-8").split("broken/surrender_present", 1)[1].split(
            "broken/surrender_evacuate", 1
        )[0]
        stamp = present.split("{effect start_white_flag}", 1)[1].split('{"player"', 1)[0]
        self.assertLess(present.find("{tag_add aio_pow_captor_enemy}"), present.find('{player "0"}'))
        self.assertLess(present.find("{tag_add aio_pow_captor_player}"), present.find('{player "0"}'))
        self.assertIn("{relation ally}", stamp)
        self.assertIn("{player\n", stamp)
        self.assertIn("id_1st_player$", stamp)
        self.assertIn("id_1st_enemy$", stamp)
        self.assertNotIn('{player "0"}', stamp)
        self.assertNotIn("{tag _user_ally}", stamp)
        self.assertNotIn("{tag def_sup_src}", stamp)


if __name__ == "__main__":
    unittest.main()
