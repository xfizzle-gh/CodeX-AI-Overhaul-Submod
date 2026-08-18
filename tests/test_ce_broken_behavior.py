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
        apply = HUMAN.read_text(encoding="utf-8").split('{on "aio_morale_surrender"', 1)[1]
        self.assertIn("{if rand", apply)
        self.assertLess(apply.find("aio_steadfast"), apply.find("aio_morale_low"))
        self.assertLess(apply.find("aio_cmd_independent"), apply.find("aio_morale_low"))
        self.assertIn("aio_morale_surrender_cand", text)
        self.assertIn('{"for selector" aio_morale_surrender_cand}', text)
        self.assertIn("{time 0.2}", surr)
        self.assertNotIn("{delete}", text)
        self.assertNotIn('{player "0"}', text)
        self.assertNotIn("{control AI}", text)

    def test_one_surrender_authority(self) -> None:
        text = BEH.read_text(encoding="utf-8")
        self.assertEqual(text.count('{"conquest_enhanced_mechanics/broken/surrender"'), 1)
        self.assertEqual(text.count('{name "conquest_enhanced_mechanics/broken/surrender"}'), 1)

    def test_surrender_presentation_and_cleanup(self) -> None:
        beh = BEH.read_text(encoding="utf-8")
        human = HUMAN.read_text(encoding="utf-8")
        lua = (ROOT / "resource/script/multiplayer/modes/utility_ce.lua").read_text(encoding="utf-8")
        self.assertIn("{collage walk_giveup_1}", beh)
        self.assertIn("{collage stand_giveup_2}", beh)
        self.assertIn("{action drop}", beh)
        self.assertIn('{"delete"', beh)
        self.assertIn('{on "start_white_flag"', human)
        self.assertIn("{delay 35", human)
        self.assertIn('{tags add "aio_morale_surrender_expire"}', human)
        self.assertNotIn('{call "delete"}', human)
        apply = human.split('{on "aio_morale_surrender_apply"', 1)[1]
        self.assertLess(apply.find('{call "start_white_flag"}'), apply.find("{delay 35"))
        self.assertEqual(human.count('{call "start_white_flag"}'), 1)
        self.assertNotIn("{effect start_white_flag}", beh)
        present = beh.split("broken/surrender_present", 1)[1].split("broken/surrender_expire", 1)[0]
        self.assertIn("{tag_add aio_morale_surrender_presenting}", present)
        self.assertLess(present.find("{tag_add aio_morale_surrender_presenting}"), present.find("{action drop}"))
        self.assertLess(present.find("{collage walk_giveup_1}"), present.find("{collage stand_giveup_2}"))
        self.assertLess(present.find("{collage stand_giveup_2}"), present.find("{tag_add aio_morale_surrender_fx}"))
        self.assertIn("{tag_remove aio_morale_surrender_presenting}", present)
        self.assertGreaterEqual(present.count("{tag aio_morale_surrender_presenting}"), 5)
        self.assertNotIn('{player "0"}', beh)
        self.assertNotIn("{control AI}", beh)
        self.assertIn("enable_ce_morale_autodemo", lua.split("function StartCeMoraleProbeLog()", 1)[1][:400])
        self.assertTrue((ROOT / "resource/entity/fx/human_markers_fx/white_flag.def").is_file())

    def test_surrender_evacuates_to_own_entry(self) -> None:
        beh = BEH.read_text(encoding="utf-8")
        lua = CONQ.read_text(encoding="utf-8")
        evac = beh.split("broken/surrender_evacuate", 1)[1].split("broken/surrender_arrive", 1)[0]
        self.assertIn("{tag aio_morale_surrender_evacuating}", evac)
        self.assertIn("{tag aio_morale_surrendering}", evac)
        self.assertIn("{state dead}", evac)
        self.assertIn("{state inactive}", evac)
        self.assertIn("{state user_control}", evac)
        self.assertIn("{tag player}", evac)
        self.assertIn("{action move}", evac)
        self.assertIn('{waypoint "attack_support_entry_a"}', evac)
        self.assertIn('{waypoint "attack_support_entry_b"}', evac)
        self.assertIn("enemy_spawnside$", evac)
        self.assertNotIn("{able", evac)
        self.assertNotIn("fight", evac)
        self.assertIn("{weapon_prepare off}", evac)
        arrive = beh.split("broken/surrender_arrive", 1)[1].split("broken/surrender_expire", 1)[0]
        self.assertIn('{"delete"', arrive)
        self.assertIn("{tag spawn_a}", arrive)
        self.assertIn("{tag spawn_b}", arrive)
        self.assertIn("aio_morale_surrendering", lua)
        self.assertIn("aio_morale_surrender_evacuating", lua)

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
