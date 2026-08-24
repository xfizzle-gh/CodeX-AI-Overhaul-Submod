from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BEH = ROOT / "resource/map/multi/ce/ce_broken_behavior_triggers.inc"
CAMP = ROOT / "resource/map/multi/ce/ce_pow_camp_triggers.inc"
MANAGE = ROOT / "resource/map/multi/ce/ce_pow_camp_manage_triggers.inc"
LIB = ROOT / "resource/map/multi/ce/ce_pow_liberate_triggers.inc"
TRIG = ROOT / "resource/map/multi/ce/ce_triggers.inc"
HUMAN = ROOT / "resource/set/interaction_entity/human_ce.inc"
DUMMY = ROOT / "resource/set/interaction_entity/dummy_ce.inc"
CONQ = ROOT / "resource/script/multiplayer/modes/conquest.lua"


class CePowFollowupTests(unittest.TestCase):
    def test_camp_stays_invisible_anchor(self) -> None:
        dummy = DUMMY.read_text(encoding="utf-8")
        camp = CAMP.read_text(encoding="utf-8")
        manage = MANAGE.read_text(encoding="utf-8")
        self.assertNotIn('{spawn "sandbag_ring"', dummy)
        self.assertNotIn("{effect aio_pow_show_camp}", camp)
        self.assertNotIn("{effect aio_pow_show_camp}", manage)
        self.assertNotIn('{"map_point_conquest"', dummy)
        self.assertIn("{tag_add aio_pow_camp}", camp)
        self.assertIn("{tag_add aio_pow_camp_enemy}", manage)
        self.assertIn("invisible", camp)
        self.assertNotIn("radio_prison_camp", camp)
        self.assertNotIn("prison_cell_beacon", camp)
        self.assertNotIn("{effect set_prison_camp}", camp)

    def test_surrender_detector_matches_accepted_main(self) -> None:
        surr = BEH.read_text(encoding="utf-8").split(
            '{"conquest_enhanced_mechanics/broken/surrender"', 1
        )[1].split("broken/surrender_diag_assign", 1)[0]
        cond = surr.split("{actions", 1)[0]
        self.assertIn("{enemy\n", cond)
        self.assertNotIn("{enemy}", cond)
        self.assertIn("{source advanced}", cond.split("{enemy\n", 1)[1])
        self.assertIn("{meters 30}", cond)
        self.assertNotIn("{meters 20}", cond)
        self.assertIn("{detection located}", cond)
        self.assertIn("aio_cmd_linked", cond)
        self.assertNotIn("aio_pow_liberated", cond)
        self.assertNotIn("aio_pow_withdraw", cond)

    def test_orig_owner_is_stamped_before_p0(self) -> None:
        beh = BEH.read_text(encoding="utf-8")
        present = beh.split("broken/surrender_present", 1)[1].split(
            '{"conquest_enhanced_mechanics/broken/surrender_evacuate"', 1
        )[0]
        self.assertIn("aio_pow_orig_p%slot", beh)
        self.assertIn('("pow_stamp_orig" slot(1))', present)
        self.assertIn('("pow_stamp_orig" slot(16))', present)
        self.assertLess(present.find('("pow_stamp_orig" slot(1))'), present.find('{player "0"}'))
        self.assertLess(present.find("{tag_add aio_pow_captor_enemy}"), present.find('("pow_stamp_orig" slot(1))'))
        self.assertLess(present.find("{effect aio_pow_clear_orig}"), present.find('("pow_stamp_orig" slot(1))'))
        self.assertIn('{on "aio_pow_clear_orig"', HUMAN.read_text(encoding="utf-8"))
        stamp = present.split("{effect start_white_flag}", 1)[1].split('{"player"', 1)[0]
        self.assertNotIn("{tag _user_ally}", stamp)
        self.assertNotIn("{tag def_sup_src}", stamp)

    def test_evac_action_move_is_oneshot(self) -> None:
        beh = BEH.read_text(encoding="utf-8")
        evac = beh.split('{"conquest_enhanced_mechanics/broken/surrender_evacuate"', 1)[1].split(
            '\n\t\t\t{"conquest_enhanced_mechanics/broken/surrender_arrive_a"', 1
        )[0]
        present = beh.split("broken/surrender_present", 1)[1].split(
            '{"conquest_enhanced_mechanics/broken/surrender_evacuate"', 1
        )[0]
        lib = LIB.read_text(encoding="utf-8")
        human = HUMAN.read_text(encoding="utf-8")
        self.assertNotIn("surrender_evac_recover", evac)
        self.assertNotIn("aio_pow_recovery_used", evac)
        self.assertNotIn("aio_pow_recovery_used", present)
        self.assertNotIn("aio_pow_recovery_used", lib)
        self.assertNotIn("aio_pow_recovery_used", human)
        self.assertIn("{time 2}", evac.split("{actions", 1)[1].split("{action move}", 1)[0])
        self.assertIn("{time 0.25}", evac.split('{"actor_state"', 1)[1].split("{action move}", 1)[0])
        self.assertNotIn("{time 3}", evac)
        self.assertNotIn("pow_using_drops", evac)
        self.assertNotIn("pow_using_drops", beh)
        self.assertNotIn("{speed assault}", evac)
        self.assertNotIn("{kind fast}", evac)

        def _close(src: str, open_idx: int) -> int:
            depth = 0
            i = open_idx
            while i < len(src):
                if src[i] == "{":
                    depth += 1
                elif src[i] == "}":
                    depth -= 1
                    if depth == 0:
                        return i
                i += 1
            self.fail("unbalanced block")

        dest_tags = (
            "aio_morale_surrender_to_camp",
            "aio_morale_surrender_to_enemy_camp",
            "aio_morale_surrender_to_a",
            "aio_morale_surrender_to_b",
        )
        pop_keys = dest_tags + ("aio_pow_captor_enemy", "aio_pow_captor_player")
        moves = 0
        pos = 0
        while True:
            act = evac.find('{"action"', pos)
            if act == -1:
                break
            act_end = _close(evac, act)
            block = evac[act : act_end + 1]
            pos = act_end + 1
            if "{action move}" not in block:
                continue
            moves += 1
            selector = block.split("{action move}", 1)[0]
            self.assertIn("{tag aio_pow_move_issued}", selector)
            self.assertNotIn("{drop orders}", block)
            after = evac[act_end + 1 :].lstrip()
            self.assertTrue(after.startswith('{"entity_state"'), msg=block[-80:])
            stamp_end = _close(after, 0)
            stamp = after[: stamp_end + 1]
            self.assertIn("{tag_add aio_pow_move_issued}", stamp)
            for key in pop_keys:
                token = "{tag %s}" % key
                if token in selector:
                    self.assertIn(token, stamp)
            self.assertFalse(all(("{tag %s}" % tag) in stamp for tag in dest_tags))
        self.assertEqual(moves, evac.count("{action move}"))
        self.assertEqual(moves, 8)
        self.assertEqual(evac.count("{tag_add aio_pow_move_issued}"), moves)
        self.assertEqual(evac.count('{"actor_state"'), 1)
        ast = evac.find('{"actor_state"')
        ast_end = _close(evac, ast)
        actor = evac[ast : ast_end + 1]
        self.assertIn("{drop orders}", actor)
        self.assertIn("{move_mode free}", actor)
        self.assertIn("{mode enable}", actor)
        self.assertIn("{speed fast}", actor)
        self.assertNotIn("{kind fast}", actor)
        self.assertIn("{tag aio_pow_move_issued}", actor)
        between = evac[ast_end + 1 : evac.find("{action move}", ast_end)]
        self.assertIn("{time 0.25}", between)
        self.assertNotEqual(evac[ast_end + 1 : evac.find('{"action"', ast_end)].strip(), "")
        self.assertNotIn("{drop orders}", evac[evac.find("{action move}") :])
        self.assertNotIn("{time 5}", evac)
        self.assertNotIn("{time 10}", evac)
        apply = human.split('{on "aio_morale_surrender_apply"', 1)[1].split("{on ", 1)[0]
        self.assertIn('{tags remove "aio_pow_move_issued"}', apply)
        self.assertNotIn("aio_pow_recovery_used", apply)
        self.assertIn("{tag_remove aio_pow_move_issued}", present)
        self.assertIn("{tag_remove aio_pow_move_issued}", lib)

    def test_evac_has_no_recovery(self) -> None:
        beh = BEH.read_text(encoding="utf-8")
        self.assertNotIn("surrender_evac_recover", beh)
        self.assertNotIn("aio_pow_recovery_used", beh)
        self.assertNotIn("aio_pow_recovery_used", HUMAN.read_text(encoding="utf-8"))
        self.assertNotIn("aio_pow_recovery_used", LIB.read_text(encoding="utf-8"))

    def test_evac_fast_release_uses_human_resupply_gap(self) -> None:
        lua = (ROOT / "resource/map/multi/ce/ai_logic/ce_lua_triggers.inc").read_text(encoding="utf-8")
        resupply = (ROOT / "resource/map/multi/ce/ce_functions.inc").read_text(encoding="utf-8").split(
            '{"send_auto_resupply"', 1
        )[1].split(";============", 1)[0]
        unhold = lua.split('{"for selector" unhold_iq}', 1)[1]
        human_state = unhold.split("{prop human}", 1)[1]
        human_state = human_state[human_state.find("{drop orders}") :]
        human_state = human_state[: human_state.find('{"default"}')]
        self.assertIn("{drop orders}", human_state)
        self.assertIn("{move_mode free}", human_state)
        self.assertIn("{mode enable}", human_state)
        self.assertNotIn("{speed fast}", human_state.split("{action move}", 1)[0] if "{action move}" in human_state else human_state)
        gap = resupply.split('{"actor_state"', 1)[1].split("{action move}", 1)[0]
        self.assertIn("{speed fast}", gap)
        self.assertIn("{time 0.25}", gap)
        self.assertIn("{type human}", resupply.split('{"actor_state"', 1)[1].split("{action move}", 1)[0])
        evac = BEH.read_text(encoding="utf-8").split(
            '{"conquest_enhanced_mechanics/broken/surrender_evacuate"', 1
        )[1].split('\n\t\t\t{"conquest_enhanced_mechanics/broken/surrender_arrive_a"', 1)[0]
        release = evac.split("{actions", 1)[1].split("{action move}", 1)[0]
        self.assertLess(release.find("{time 2}"), release.find('{"actor_state"'))
        self.assertLess(release.find('{"actor_state"'), release.find("{time 0.25}"))
        self.assertGreater(release.find("{time 0.25}"), release.rfind("{speed fast}"))
        self.assertNotIn("{kind fast}", evac)
        self.assertNotIn("{speed assault}", evac)

    def test_using_drop_has_no_guessed_rocketlauncher(self) -> None:
        present = BEH.read_text(encoding="utf-8").split("broken/surrender_present", 1)[1].split(
            '{"conquest_enhanced_mechanics/broken/surrender_evacuate"', 1
        )[0]
        self.assertEqual(present.count('{"inventory"'), 3)
        self.assertEqual(present.count('{item "weapon"}'), 2)
        self.assertGreaterEqual(present.count("{type using}"), 3)
        self.assertNotIn("rocketlauncher", present)
        self.assertNotIn("{volume in_hands}", present)
        self.assertNotIn("{action take}", present)

    def test_liberation_uses_pre_p0_provenance_and_withdraws(self) -> None:
        self.assertTrue(LIB.is_file())
        self.assertIn("ce_pow_liberate_triggers.inc", TRIG.read_text(encoding="utf-8"))
        lib = "\n".join(
            line for line in LIB.read_text(encoding="utf-8").splitlines() if not line.lstrip().startswith(";")
        )
        human = HUMAN.read_text(encoding="utf-8")
        self.assertIn("{tag aio_pow_captor_enemy}", lib)
        self.assertIn("{relation ally}", lib)
        self.assertIn("id_1st_player$", lib)
        self.assertIn("{meters 2}", lib)
        self.assertIn("{detection located}", lib)
        self.assertIn("{operation set}", lib)
        self.assertIn('{player "%slot"}', lib)
        self.assertIn('("pow_restore_orig" slot(1))', lib)
        self.assertIn('("pow_restore_orig" slot(16))', lib)
        self.assertIn("{impregnability disabled}", lib)
        self.assertIn("{effect stop_white_flag}", lib)
        self.assertIn("{effect aio_pow_liberate_guard}", lib)
        self.assertIn('{waypoint "attack_support_entry_a"}', lib)
        self.assertIn('{waypoint "attack_support_entry_b"}', lib)
        self.assertIn("{tag_add aio_pow_liberated}", lib)
        self.assertIn("{tag_add aio_pow_withdraw}", lib)
        self.assertIn("{tag_add aio_pow_withdraw_a}", lib)
        self.assertIn("{tag_add aio_pow_withdraw_b}", lib)
        self.assertIn("pow_withdraw_arrive_a", lib)
        self.assertIn("pow_withdraw_arrive_b", lib)
        self.assertIn("{tag spawn_a}", lib)
        self.assertIn("{tag spawn_b}", lib)
        self.assertIn("{meters 200}", lib)
        self.assertIn("{effect aio_pow_clear_orig}", lib)
        self.assertLess(lib.find('("pow_restore_orig" slot(16))'), lib.find("{effect aio_pow_clear_orig}"))
        self.assertIn("{tag_remove aio_morale_surrendering}", lib)
        self.assertNotIn("{tag _user_ally}", lib)
        self.assertNotIn("{tag def_sup_src}", lib)
        self.assertNotIn("{control user}", lib)
        self.assertNotIn("{control AI}", lib)
        self.assertNotIn("{weapon_prepare on}", lib)
        self.assertNotIn("{fire_mode open}", lib)
        self.assertNotIn('{player "0"}', lib)
        self.assertIn('{on "aio_pow_liberate_guard"', human)
        guard = human.split('{on "aio_pow_liberate_guard"', 1)[1].split("{on ", 1)[0]
        self.assertIn("{delay 20", guard)
        self.assertIn('{tags remove "aio_pow_liberated"}', guard)
        self.assertNotIn('{tags remove "aio_pow_withdraw"}', guard)
        self.assertIn("aio_pow_liberated", CONQ.read_text(encoding="utf-8"))
        self.assertIn("aio_pow_withdraw", CONQ.read_text(encoding="utf-8"))

    def test_no_naive_friendly_proximity_surrender_rule(self) -> None:
        beh = BEH.read_text(encoding="utf-8")
        surr = beh.split('{"conquest_enhanced_mechanics/broken/surrender"', 1)[1].split(
            "broken/surrender_diag_assign", 1
        )[0]
        cond = surr.split("{actions", 1)[0]
        self.assertIn("aio_cmd_linked", cond)
        self.assertNotIn("aio_pow_liberated", cond)
        self.assertNotIn("aio_pow_withdraw", cond)
        self.assertNotIn("{meters 5}", cond)
        self.assertNotIn("{meters 10}", cond)
        self.assertNotIn("{meters 15}", cond)
        self.assertIn("Do not add a naive friendly-proximity rule", beh)
        apply = HUMAN.read_text(encoding="utf-8").split('{on "aio_morale_surrender"', 1)[1].split(
            '{on "aio_morale_surrender_apply"', 1
        )[0]
        self.assertLess(apply.find('{if tagged "aio_cmd_linked"'), apply.find('{if rand'))
        self.assertLess(apply.find('{if tagged "aio_pow_liberated"'), apply.find("{if rand"))

    def test_management_still_deferred(self) -> None:
        manage = MANAGE.read_text(encoding="utf-8")
        lib = LIB.read_text(encoding="utf-8")
        human = HUMAN.read_text(encoding="utf-8")
        self.assertNotIn("aio_pow_convert", manage + lib + human)
        self.assertNotIn("follow_me", manage + lib)
        self.assertNotIn("prison_truck", manage + lib)
        self.assertNotIn("{on \"aio_pow_convert\"", human)


if __name__ == "__main__":
    unittest.main()
