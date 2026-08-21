from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BEH = ROOT / "resource/map/multi/ce/ce_broken_behavior_triggers.inc"
CAMP = ROOT / "resource/map/multi/ce/ce_pow_camp_triggers.inc"
TRIG = ROOT / "resource/map/multi/ce/ce_triggers.inc"
VARS = ROOT / "resource/map/multi/ce/ce_vars.inc"
HUMAN = ROOT / "resource/set/interaction_entity/human_ce.inc"


class CePowCampTests(unittest.TestCase):
    def test_camp_is_wired(self) -> None:
        self.assertTrue(CAMP.is_file())
        self.assertIn("ce_pow_camp_triggers.inc", TRIG.read_text(encoding="utf-8"))
        self.assertIn('{"aio_pow_camp_ready"}', VARS.read_text(encoding="utf-8"))
        camp = CAMP.read_text(encoding="utf-8")
        self.assertIn("Interim collection", camp)
        self.assertIn("P0+harmless lifecycle stays in surrender_present", camp)

    def test_camp_marks_captor_rear_spawn(self) -> None:
        camp = CAMP.read_text(encoding="utf-8")
        s1 = camp.split("spawn_pow_camp_s1", 1)[1].split("spawn_pow_camp_s2", 1)[0]
        s2 = camp.split("spawn_pow_camp_s2", 1)[1].split("surrender_arrive_camp", 1)[0]
        self.assertIn("{tag spawn_b}", s1)
        self.assertNotIn("{tag spawn_a}", s1)
        self.assertIn("{tag spawn_a}", s2)
        self.assertNotIn("{tag spawn_b}", s2)
        self.assertIn("{tag_add aio_pow_camp}", s1)
        self.assertIn("{tag_add aio_pow_camp}", s2)
        self.assertIn("aio_pow_camp_ready$", s1)
        self.assertIn("enemy_spawnside$", s1)
        self.assertIn("{op \"<=\"}", s1)
        self.assertIn("{value 1}", s1)
        self.assertIn("{value 2}", s2)
        self.assertNotIn("{op \"==\"}", s1.split("enemy_spawnside$", 1)[1].split("aio_pow_camp_ready$", 1)[0])
        self.assertNotIn('{"delete"', camp)
        self.assertNotIn('{player "0"}', camp)
        self.assertNotIn("{control AI}", camp)

    def test_evac_branches_once_on_live_camp(self) -> None:
        evac = BEH.read_text(encoding="utf-8").split("broken/surrender_evacuate", 1)[1].split(
            "broken/surrender_arrive_a", 1
        )[0]
        self.assertNotIn("aio_pow_camp_ready$", evac)
        self.assertNotIn("tag_remove aio_morale_surrender_to_a", evac)
        self.assertNotIn("tag_remove aio_morale_surrender_to_b", evac)
        self.assertEqual(evac.count("{tag_add aio_morale_surrender_to_camp}"), 2)
        self.assertIn("{tag aio_pow_camp}", evac)
        self.assertIn("{op \">=\"}", evac)
        self.assertIn("{tag prisoner_in_camp}", evac)
        self.assertIn("{tag aio_pow_captor_player}", evac)
        self.assertIn("{tag aio_pow_captor_enemy}", evac)
        self.assertNotIn("{tag _user_ally}", evac)
        self.assertNotIn("{tag def_sup_src}", evac)
        self.assertIn("{tag aio_pow_camp_enemy}", evac)
        self.assertEqual(evac.count("{type entities}"), 4)
        self.assertEqual(evac.count("{tag_add aio_morale_surrender_to_enemy_camp}"), 2)
        self.assertIn('{waypoint "attack_support_entry_a"}', evac)
        self.assertIn('{waypoint "attack_support_entry_b"}', evac)
        self.assertIn("{time 3}", evac)
        self.assertNotIn("{time 5}", evac)
        self.assertNotIn('{"actor_state"', evac)
        self.assertNotIn('{drop "orders sensor senseless"}', evac)
        self.assertNotIn("{fire_mode hold}", evac)
        s1 = evac.split("{value 1}", 1)[1].split("{value 2}", 1)[0]
        self.assertIn("{tag_add aio_morale_surrender_to_enemy_camp}", s1)
        self.assertIn("{tag aio_pow_camp_enemy}", s1)
        self.assertIn("{tag_add aio_morale_surrender_to_camp}", s1)
        self.assertIn("{tag aio_pow_camp}", s1)
        self.assertIn("entry_a", s1)
        self.assertIn("entry_b", s1)
        self.assertIn("{tag aio_pow_captor_enemy}", s1)
        self.assertIn("{tag aio_pow_captor_player}", s1)
        s2 = evac.split("{value 2}", 1)[1]
        self.assertIn("{tag_add aio_morale_surrender_to_enemy_camp}", s2)
        self.assertIn("{tag_add aio_morale_surrender_to_camp}", s2)
        self.assertIn("entry_b", s2)
        self.assertIn("entry_a", s2)

    def test_camp_arrival_holds_without_delete(self) -> None:
        arrive = CAMP.read_text(encoding="utf-8").split("surrender_arrive_camp", 1)[1]
        self.assertIn("{tag_add prisoner_in_camp}", arrive)
        self.assertIn('{var "ce_morale_diag_held$"}', arrive)
        self.assertIn("{tag aio_pow_camp}", arrive)
        self.assertIn("{meters 25}", arrive)
        self.assertNotIn("{meters 80}", arrive)
        self.assertNotIn("aio_pow_camp_ready$", arrive)
        self.assertIn("{move_mode hold}", arrive)
        self.assertIn("{mode disable}", arrive)
        self.assertNotIn('{"delete"', arrive)
        self.assertIn("tag_remove aio_morale_surrender_evacuating", arrive)
        self.assertIn("tag_remove aio_morale_surrender_expire", arrive)
        self.assertIn("{state dead}", arrive)
        self.assertIn("{state inactive}", arrive)
        self.assertIn("{state user_control}", arrive)
        self.assertIn("{tag player}", arrive)

    def test_expire_and_apply_spare_camped_pows(self) -> None:
        human = HUMAN.read_text(encoding="utf-8")
        apply = human.split('{on "aio_morale_surrender_apply"', 1)[1].split("{on ", 1)[0]
        self.assertIn("{delay 500", apply)
        self.assertNotIn("{delay 100", apply)
        self.assertIn('not tagged "prisoner_in_camp"', apply)
        self.assertIn('not tagged "prisoner_in_enemy_camp"', apply)
        self.assertIn('not tagged "aio_morale_surrender_evacuating"', apply)
        self.assertIn('not tagged "aio_morale_surrender_presenting"', apply)
        self.assertIn('not tagged "aio_morale_surrender_to_camp"', apply)
        self.assertIn('not tagged "aio_morale_surrender_to_enemy_camp"', apply)
        self.assertNotIn('{able "select" 0}', apply)
        self.assertNotIn('{able "fight" 0}', apply)
        self.assertNotIn('{player "0"}', apply)
        expire = BEH.read_text(encoding="utf-8").split("broken/surrender_expire", 1)[1].split(
            "broken/observe_surrender", 1
        )[0]
        self.assertIn("{tag prisoner_in_camp}", expire)
        self.assertIn("{tag aio_morale_surrender_evacuating}", expire)
        self.assertIn("{tag aio_morale_surrender_presenting}", expire)
        self.assertIn("{tag aio_morale_surrender_to_camp}", expire)
        self.assertIn("{tag aio_morale_surrender_to_enemy_camp}", expire)
        die = human.split('{on "die"', 1)[1].split("{on ", 1)[0]
        self.assertIn('{tags remove "prisoner_in_camp"}', die)
        self.assertIn('{tags remove "aio_morale_surrender_to_camp"}', die)
        self.assertIn('{tags remove "aio_pow_captor_player"}', die)
        self.assertIn('{tags remove "aio_pow_captor_enemy"}', die)

    def test_camp_files_do_not_override_p0_lifecycle(self) -> None:
        camp = CAMP.read_text(encoding="utf-8")
        present = BEH.read_text(encoding="utf-8").split("broken/surrender_present", 1)[1].split(
            "broken/surrender_evacuate", 1
        )[0]
        self.assertNotIn('{player "0"}', camp)
        self.assertNotIn("{control AI}", camp)
        self.assertNotIn('{able "select" 0}', camp)
        self.assertNotIn('{able "fight" 0}', camp)
        self.assertIn('{player "0"}', present)
        self.assertIn("{impregnability harmless}", present)
        self.assertIn("{effect start_white_flag}", present)
        self.assertIn("{collage stand_giveup_1}", present)


if __name__ == "__main__":
    unittest.main()
