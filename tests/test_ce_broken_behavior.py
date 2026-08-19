from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BEH = ROOT / "resource/map/multi/ce/ce_broken_behavior_triggers.inc"
MACHINE = ROOT / "resource/map/multi/ce/ce_morale_machine_triggers.inc"
HUMAN = ROOT / "resource/set/interaction_entity/human_ce.inc"
DCG = ROOT / "resource/map/multi/dcg_script.inc"
CONQ = ROOT / "resource/script/multiplayer/modes/conquest.lua"
WAVES = ROOT / "resource/map/multi/attack_support_waves.inc"


class CeBrokenBehaviorTests(unittest.TestCase):
    def test_stack_and_lua_yield(self) -> None:
        self.assertIn("ce_broken_behavior_triggers.inc", DCG.read_text(encoding="utf-8"))
        self.assertIn("aio_morale_owned", CONQ.read_text(encoding="utf-8"))
        self.assertIn('{advance_ratio "0.1"}', BEH.read_text(encoding="utf-8"))
        self.assertNotIn("{advance_ratio 0.1}", BEH.read_text(encoding="utf-8"))
        lua = (ROOT / "resource/script/multiplayer/modes/utility_ce.lua").read_text(encoding="utf-8")
        self.assertIn("retreat=", lua)
        self.assertIn("CE_MORALE_EVENT retreat", lua)
        self.assertIn("CE_MORALE_EVENT surrender", lua)
        self.assertIn("observe_surrender", BEH.read_text(encoding="utf-8"))
        die = HUMAN.read_text(encoding="utf-8").split('{on "die"', 1)[1].split("{on ", 1)[0]
        self.assertIn('{tags remove "aio_morale_broken"}', die)
        hold = BEH.read_text(encoding="utf-8").split("{drop \"orders sensor senseless\"}", 1)[0]
        hold = hold[hold.rfind('{"actor_state"'):]
        self.assertIn("{state dead}", hold)
        self.assertIn("{state inactive}", hold)
        self.assertIn("aio_morale_owned", WAVES.read_text(encoding="utf-8"))
        dcg = (ROOT / "resource/map/multi/dcg_script.inc").read_text(encoding="utf-8")
        tune = dcg.split("cmp_def_1_tune", 1)[1].split("cmp_def_2_tune", 1)[0]
        self.assertIn("aio_morale_owned", tune)

    def test_player_excluded_from_broken_and_surrender(self) -> None:
        text = BEH.read_text(encoding="utf-8") + MACHINE.read_text(encoding="utf-8")
        self.assertIn("{state user_control}", text)
        self.assertIn("{tag player}", MACHINE.read_text(encoding="utf-8").split("escalate_broken", 1)[1])
        human = HUMAN.read_text(encoding="utf-8")
        surr = human.split('{on "aio_morale_surrender"', 1)[1]
        self.assertIn("not user_control", surr)

    def test_broken_recovers_to_panic(self) -> None:
        human = HUMAN.read_text(encoding="utf-8")
        done = human.split('{on "recovered_from_broken"', 1)[1].split('{on "', 1)[0]
        self.assertIn('{tags add "aio_morale_panic"}', done)
        self.assertIn("aio_cmd_linked", done)
        self.assertNotIn('{tags remove "aio_morale_owned"}', done)
        self.assertIn('{call "recovering_from_panic"}', done)
        steady = human.split('{on "recovered_from_shaken"', 1)[1].split('{on "', 1)[0]
        self.assertIn('{tags remove "aio_morale_owned"}', steady)
        self.assertIn("aio_morale_watch_regroup", human)
        beh = BEH.read_text(encoding="utf-8")
        self.assertIn("{relation ally}", beh)
        self.assertIn("{sort", beh)
        self.assertIn("{mode nearest}", beh.split("{sort", 1)[1].split("{amount", 1)[0])
        owned_refresh = beh.split("{tag_add aio_morale_regrouping}", 1)[1]
        owned_refresh = owned_refresh.split("broken/rally", 1)[0]
        self.assertIn("{tag aio_morale_owned}", owned_refresh)
        self.assertIn("{drop orders}", beh)
        first = beh.split("{drop orders}", 1)[0]
        self.assertIn("{tag aio_morale_owned}", first[-400:])

    def test_surrender_requires_broken_and_failed_regroup(self) -> None:
        text = BEH.read_text(encoding="utf-8")
        surr = text.split("broken/surrender", 1)[1]
        self.assertIn("aio_morale_broken", surr)
        self.assertIn("aio_morale_regroup_failed", surr)
        self.assertNotIn("aio_morale_panic", surr.split("{actions", 1)[0])
        apply = HUMAN.read_text(encoding="utf-8").split('{on "aio_morale_surrender"', 1)[1].split('{on "aio_morale_surrender_apply"', 1)[0]
        self.assertIn('{if tagged "aio_cmd_linked"', apply)
        self.assertLess(apply.find('{if tagged "aio_cmd_linked"'), apply.find("{if rand"))
        self.assertLess(apply.find("aio_steadfast"), apply.find("aio_morale_low"))
        self.assertLess(apply.find("aio_cmd_independent"), apply.find("aio_morale_low"))
        cond = text.split("broken/surrender", 1)[1].split("{actions", 1)[0]
        self.assertIn("aio_cmd_linked", cond)
        self.assertIn("aio_morale_surrender_cand", text)
        self.assertIn('{"for selector" aio_morale_surrender_cand}', text)
        self.assertIn("{time 0.2}", surr)
        self.assertNotIn("{delete}", text)
        self.assertNotIn('{player "0"}', text)
        self.assertNotIn("{control AI}", text)

    def test_command_reacquire_clears_failed_regroup(self) -> None:
        beh = BEH.read_text(encoding="utf-8")
        human = HUMAN.read_text(encoding="utf-8")
        rec = beh.split("broken/reacquire", 1)[1].split("broken/surrender", 1)[0]
        self.assertIn("aio_morale_broken", rec)
        self.assertIn("aio_cmd_linked", rec)
        self.assertIn("aio_morale_regroup_failed", rec)
        self.assertIn("tag_remove aio_morale_regroup_failed", rec)
        self.assertIn("tag_remove aio_morale_surrender_cand", rec)
        self.assertIn("aio_morale_surrendering", rec)
        self.assertIn("{state user_control}", rec)
        self.assertIn("{tag player}", rec)
        self.assertNotIn("{effect recovering_from_broken}", rec)
        recover = beh.split("broken/recover", 1)[1].split("broken/reacquire", 1)[0]
        self.assertIn("{effect recovering_from_broken}", recover)
        self.assertIn("aio_morale_recovering_from_broken", recover)
        self.assertIn("aio_morale_surrendering", recover)
        self.assertEqual(beh.count("{effect recovering_from_broken}"), 1)
        self.assertEqual(human.count('{on "recovering_from_broken"'), 1)
        self.assertEqual(human.count('{on "aio_morale_surrender_apply"'), 1)
        self.assertNotIn('{player "0"}', rec)
        self.assertNotIn("{control AI}", rec)

    def _enemy_before_distance(self, text: str) -> str:
        start = text.find("{enemy")
        self.assertGreater(start, -1, "missing {enemy")
        end = text.find("{distance", start)
        self.assertGreater(end, start, "missing {distance after {enemy")
        return text[start:end]

    def test_scripted_attack_excludes_surrendering(self) -> None:
        ce = (ROOT / "resource/map/multi/ce/ai_logic/ce_lua_triggers.inc").read_text(encoding="utf-8")
        unhold = ce.split("ai/ai_unhold", 1)[1].split("ai/tank_alt_fight", 1)[0]
        first = unhold.split('{"1.see_actors"', 1)[1].split('{"2.see_actors"', 1)[0]
        second = unhold.split('{"2.see_actors"', 1)[1]
        self.assertIn("aio_morale_surrendering", self._enemy_before_distance(first))
        self.assertIn("aio_morale_surrendering", self._enemy_before_distance(second))
        self.assertIn("_user_ally", first)
        td = ce.split("ai/td_attack_see_actors", 1)[1].split("{distance", 1)[0]
        self.assertIn("aio_morale_surrendering", td.split("{enemy", 1)[1])
        codex = (ROOT / "resource/map/multi/codex_ai_combat.inc").read_text(encoding="utf-8")
        c100 = codex.split("{meters 100}", 1)[0].rsplit("{enemy", 1)[1]
        c125 = codex.split("{meters 125}", 1)[0].rsplit("{enemy", 1)[1]
        self.assertIn("aio_morale_surrendering", c100)
        self.assertIn("aio_morale_surrendering", c125)
        dcg = DCG.read_text(encoding="utf-8")
        for grenade in ("m67", "m26", "rgd5"):
            block = dcg.split("dcg/betterai/grenade/inf/" + grenade, 1)[1].split("{distance", 1)[0]
            self.assertIn("aio_morale_surrendering", block.split("{enemy", 1)[1])
        d55 = dcg.split("{meters 55}", 1)[0].rsplit("{enemy", 1)[1]
        d66 = dcg.split("{meters 66}", 1)[0].rsplit("{enemy", 1)[1]
        self.assertIn("aio_morale_surrendering", d55)
        self.assertIn("aio_morale_surrendering", d66)
        lua = (ROOT / "resource/script/multiplayer/modes/utility_ce.lua").read_text(encoding="utf-8")
        self.assertIn("CE_POW alive=1 surrendering=", lua)
        self.assertIn("CE_POW alive=0 surrendering=0", lua)
        self.assertNotIn("targeted=", lua)
        self.assertIn("Diagnostic-only 2s watcher", lua)
        for rel in (
            "resource/map/multi/ce/ai_logic/ce_lua_triggers.inc",
            "resource/map/multi/codex_ai_combat.inc",
            "resource/map/multi/dcg_script.inc",
        ):
            text = (ROOT / rel).read_text(encoding="utf-8")
            self.assertEqual(text.count("{"), text.count("}"), rel)

    def test_one_surrender_authority(self) -> None:
        text = BEH.read_text(encoding="utf-8")
        self.assertEqual(text.count('{"conquest_enhanced_mechanics/broken/surrender"'), 1)
        self.assertEqual(text.count('{name "conquest_enhanced_mechanics/broken/surrender"}'), 1)

    def test_surrender_presentation_and_cleanup(self) -> None:
        beh = BEH.read_text(encoding="utf-8")
        human = HUMAN.read_text(encoding="utf-8")
        lua = (ROOT / "resource/script/multiplayer/modes/utility_ce.lua").read_text(encoding="utf-8")
        self.assertIn("{collage walk_giveup_1}", beh)
        self.assertNotIn("{collage stand_giveup_2}", beh)
        self.assertIn("{action drop}", beh)
        self.assertIn('{"delete"', beh)
        self.assertIn('{on "start_white_flag"', human)
        self.assertIn("{delay 75", human)
        apply = human.split('{on "aio_morale_surrender_apply"', 1)[1]
        self.assertNotIn('{able "neutral" 1}', apply)
        self.assertNotIn('{player "0"}', apply)
        self.assertNotIn("{delay 80", apply)
        self.assertNotIn("{delay 60", apply)
        self.assertIn('{tags add "aio_morale_surrender_expire"}', human)
        self.assertNotIn('{call "delete"}', human)
        self.assertLess(apply.find('{call "aio_morale_refresh_icons"}'), apply.find("{delay 75"))
        self.assertEqual(human.count('{call "start_white_flag"}'), 1)
        self.assertNotIn("{effect start_white_flag}", beh)
        present = beh.split("broken/surrender_present", 1)[1].split("broken/surrender_evacuate", 1)[0]
        self.assertIn("{tag_add aio_morale_surrender_presenting}", present)
        self.assertLess(present.find("{tag_add aio_morale_surrender_presenting}"), present.find("{action drop}"))
        self.assertLess(present.find("{action drop}"), present.find("{collage walk_giveup_1}"))
        self.assertLess(present.find("{collage walk_giveup_1}"), present.find("{tag_add aio_morale_surrender_fx}"))
        self.assertIn("{totalTime 99999}", present)
        self.assertIn("{flags loop}", present)
        self.assertIn("{drop orders}", present)
        self.assertIn("{tag_remove aio_morale_surrender_presenting}", present)
        self.assertGreaterEqual(present.count("{tag aio_morale_surrender_presenting}"), 4)
        self.assertNotIn('{player "0"}', beh)
        self.assertNotIn("{control AI}", beh)
        self.assertIn("enable_ce_morale_autodemo", lua.split("function StartCeMoraleProbeLog()", 1)[1][:400])
        self.assertTrue((ROOT / "resource/entity/fx/human_markers_fx/white_flag.def").is_file())
        self.assertTrue((ROOT / "resource/entity/fx/human_markers_fx/no_comd.def").is_file())
        self.assertTrue((ROOT / "resource/entity/fx/human_markers_fx/aio_cmd_junior.def").is_file())
        self.assertTrue((ROOT / "resource/entity/fx/human_markers_fx/aio_morale_panic.def").is_file())
        self.assertTrue((ROOT / "resource/entity/fx/human_markers_fx/aio_morale_broken.def").is_file())
        self.assertIn('{on "aio_morale_refresh_icons"', human)
        self.assertIn('{add_view "aio_cmd_lost" "aio_cmd_lost" "head"}', human)
        self.assertTrue((ROOT / "resource/entity/fx/human_markers_fx/aio_cmd_lost.def").is_file())
        rank = (ROOT / "resource/entity/fx/human_markers_fx/aio_cmd_junior.def").read_text(encoding="utf-8")
        self.assertIn("{min 0.0675}", rank)
        self.assertIn("{offset 0 0 0}", rank)
        self.assertNotIn("{halo}", rank)
        lost = (ROOT / "resource/entity/fx/human_markers_fx/aio_cmd_lost.def").read_text(encoding="utf-8")
        self.assertIn("{min 0.20}", lost)
        flag = (ROOT / "resource/entity/fx/human_markers_fx/white_flag.def").read_text(encoding="utf-8")
        self.assertIn("{min 0.14}", flag)
        self.assertNotIn("{min 0.3}", flag)
        self.assertIn('{add_view "aio_cmd_junior"', human)

    def test_surrender_evacuates_to_captor_entry(self) -> None:
        beh = BEH.read_text(encoding="utf-8")
        lua = CONQ.read_text(encoding="utf-8")
        ctf = (ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set").read_text(encoding="utf-8")
        self.assertRegex(ctf, r"\{scoreFinal\s+8500\}")
        self.assertNotRegex(ctf, r"\{scoreFinal\s+9000\}")
        self.assertIn("points_table_player=0/0.000,0.33/3.750,0.50/4.500,0.66/5.600,1.00/7.500", ctf)
        self.assertIn("points_table_ai=0/0.000,0.33/3.750,0.50/4.500,0.66/5.600,1.00/7.500", ctf)
        self.assertIn("kill_score_multiplier=6.00", ctf)
        self.assertNotIn("surrender_hold", beh)
        evac = beh.split("broken/surrender_evacuate", 1)[1].split("broken/surrender_arrive_a", 1)[0]
        self.assertIn("{tag aio_morale_surrender_evacuating}", evac)
        self.assertIn("{tag aio_morale_surrendering}", evac)
        self.assertIn("{state dead}", evac)
        self.assertIn("{state inactive}", evac)
        self.assertIn("{state user_control}", evac)
        self.assertIn("{tag player}", evac)
        self.assertIn("{move_mode free}", evac)
        self.assertIn("{mode enable}", evac)
        self.assertIn("{action move}", evac)
        self.assertIn('{waypoint "attack_support_entry_a"}', evac)
        self.assertIn('{waypoint "attack_support_entry_b"}', evac)
        self.assertIn("enemy_spawnside$", evac)
        self.assertIn("{tag _user_ally}", evac)
        self.assertIn("{tag def_sup_src}", evac)
        self.assertGreaterEqual(evac.count("{action move}"), 4)
        self.assertEqual(evac.count('{"actor_state"'), 1)
        self.assertIn("{time 5}", evac)
        self.assertNotIn('{var "enemy_spawnside$"} {op "=="} {value 0}', evac)
        s1 = evac.split("{value 1}", 1)[1].split("{value 2}", 1)[0]
        s1_wp = [line for line in s1.splitlines() if "attack_support_entry" in line]
        self.assertEqual(len(s1_wp), 2)
        self.assertIn("entry_a", s1_wp[0])
        self.assertIn("entry_b", s1_wp[1])
        self.assertIn("aio_morale_surrender_to_a", s1)
        self.assertIn("aio_morale_surrender_to_b", s1)
        s2 = evac.split("{value 2}", 1)[1]
        s2_wp = [line for line in s2.splitlines() if "attack_support_entry" in line]
        self.assertEqual(len(s2_wp), 2)
        self.assertIn("entry_b", s2_wp[0])
        self.assertIn("entry_a", s2_wp[1])
        arrive_a = beh.split("broken/surrender_arrive_a", 1)[1].split("broken/surrender_arrive_b", 1)[0]
        arrive_b = beh.split("broken/surrender_arrive_b", 1)[1].split("broken/surrender_expire", 1)[0]
        self.assertIn('{"delete"', arrive_a)
        self.assertIn('{"delete"', arrive_b)
        self.assertIn("aio_morale_surrender_at_egress", arrive_a)
        self.assertIn("{tag spawn_a}", arrive_a)
        self.assertNotIn("{tag spawn_b}", arrive_a)
        self.assertIn("{tag aio_morale_surrender_to_a}", arrive_a)
        self.assertIn("{tag spawn_b}", arrive_b)
        self.assertNotIn("{tag spawn_a}", arrive_b)
        self.assertIn("{tag aio_morale_surrender_to_b}", arrive_b)
        self.assertIn("aio_morale_surrendering", lua)
        self.assertIn("aio_morale_surrender_evacuating", lua)
        self.assertNotIn("{stat_notify", beh)
        self.assertEqual(HUMAN.read_text(encoding="utf-8").count("{stat_notify"), 0)

    def test_effect_selectors_exclude_dead_inactive(self) -> None:
        parts = BEH.read_text(encoding="utf-8").split('{"effect"')
        self.assertGreater(len(parts), 1)
        for part in parts[1:]:
            block = part.split("{effect ", 1)[0]
            self.assertIn("{state dead}", block)
            self.assertIn("{state inactive}", block)

    def test_cleanup_strips_inactive_broken_tags(self) -> None:
        cleanup = BEH.read_text(encoding="utf-8").split("broken/cleanup_dead", 1)[1]
        self.assertNotIn('{state "dead inactive"}', cleanup)
        self.assertIn("{state dead}", cleanup)
        self.assertIn("{state inactive}", cleanup)
        for tag in (
            "aio_morale_owned",
            "aio_morale_broken",
            "aio_morale_regrouping",
            "aio_morale_regroup_failed",
            "aio_morale_moving_to_rally",
            "aio_morale_surrender_cand",
            "aio_morale_surrendering",
            "aio_morale_watching_regroup",
            "aio_morale_surrender_fx",
            "aio_morale_surrender_presenting",
            "aio_morale_surrender_expire",
            "aio_morale_surrender_evacuating",
            "aio_morale_surrender_at_egress",
            "aio_morale_surrender_to_a",
            "aio_morale_surrender_to_b",
        ):
            self.assertIn("tag_remove " + tag, cleanup)

    def test_actor_state_selectors_exclude_dead_and_inactive(self) -> None:
        text = BEH.read_text(encoding="utf-8")
        self.assertNotIn('{state "dead inactive"}', text)
        parts = text.split('{"actor_state"')
        self.assertGreater(len(parts), 1)
        for part in parts[1:]:
            block = part.split('{"', 1)[0]
            self.assertIn("{state dead}", block)
            self.assertIn("{state inactive}", block)


if __name__ == "__main__":
    unittest.main()
