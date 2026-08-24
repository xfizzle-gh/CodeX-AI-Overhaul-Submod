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
TRIG = ROOT / "resource/map/multi/ce/ce_triggers.inc"
LUA = ROOT / "resource/script/multiplayer/modes/utility_ce.lua"


class CeBrokenBehaviorTests(unittest.TestCase):
    def test_stack_and_lua_yield(self) -> None:
        self.assertIn("ce_broken_behavior_triggers.inc", DCG.read_text(encoding="utf-8"))
        self.assertIn("aio_morale_owned", CONQ.read_text(encoding="utf-8"))
        self.assertNotIn("aio_morale_surrendering", CONQ.read_text(encoding="utf-8"))
        self.assertIn('{advance_ratio "0.1"}', BEH.read_text(encoding="utf-8"))
        lua = LUA.read_text(encoding="utf-8")
        self.assertIn("retreat=", lua)
        self.assertIn("CE_MORALE_EVENT retreat", lua)
        self.assertNotIn("CE_MORALE_EVENT surrender", lua)
        self.assertNotIn("startPowDiagWatch", lua)
        die = HUMAN.read_text(encoding="utf-8").split('{on "die"', 1)[1].split("{on ", 1)[0]
        self.assertIn('{tags remove "aio_morale_broken"}', die)
        self.assertIn('{tags remove "aio_morale_retreat_issued"}', die)
        hold = BEH.read_text(encoding="utf-8").split("{drop \"orders sensor senseless\"}", 1)[0]
        hold = hold[hold.rfind('{"actor_state"'):]
        self.assertIn("{state dead}", hold)
        self.assertIn("{state inactive}", hold)
        self.assertIn("aio_morale_owned", WAVES.read_text(encoding="utf-8"))

    def test_direct_control_excluded_from_broken_escape(self) -> None:
        machine = MACHINE.read_text(encoding="utf-8")
        broken = machine.split("escalate_broken", 1)[1].split("start_recover", 1)[0]
        self.assertIn("{state user_control}", broken)
        self.assertNotIn("{tag player}", broken)
        beh = BEH.read_text(encoding="utf-8")
        acquire = beh.split("broken/acquire", 1)[1].split("broken/escape", 1)[0]
        escape = beh.split("broken/escape", 1)[1]
        self.assertIn("{state user_control}", acquire)
        self.assertIn("{state user_control}", escape)
        self.assertNotIn("{tag player}", acquire)
        self.assertNotIn("{tag player}", escape)
        self.assertEqual(acquire.count('{"actor_state"'), 1)
        actor = acquire.split('{"actor_state"', 1)[1]
        self.assertIn("{tag aio_morale_retreat_issued}", actor.split("{exclude", 1)[1])

    def test_pow_shipping_path_is_gone(self) -> None:
        beh = BEH.read_text(encoding="utf-8")
        human = HUMAN.read_text(encoding="utf-8")
        trig = TRIG.read_text(encoding="utf-8")
        self.assertNotIn("ce_pow_camp_triggers.inc", trig)
        self.assertNotIn("ce_pow_camp_manage_triggers.inc", trig)
        self.assertFalse((ROOT / "resource/map/multi/ce/ce_pow_camp_triggers.inc").exists())
        self.assertFalse((ROOT / "resource/map/multi/ce/ce_pow_camp_manage_triggers.inc").exists())
        for forbidden in (
            "broken/surrender",
            "start_white_flag",
            '{player "0"}',
            "impregnability harmless",
            "aio_pow_camp",
            "aio_pow_captor",
            "aio_morale_surrendering",
            "attack_support_entry_",
        ):
            self.assertNotIn(forbidden, beh)
        self.assertNotIn('{on "aio_morale_surrender"', human)
        self.assertNotIn('{on "start_white_flag"', human)
        self.assertNotIn("aio_morale_surrendering", human)

    def test_escape_is_oneshot_own_spawn_fast(self) -> None:
        beh = BEH.read_text(encoding="utf-8")
        escape = beh.split("broken/escape", 1)[1].split("broken/arrive", 1)[0]
        self.assertIn("{speed fast}", escape)
        self.assertNotIn("{kind fast}", escape)
        self.assertIn("{time 0.25}", escape)
        self.assertLess(escape.find('{"actor_state"'), escape.find("{tag_add aio_morale_retreat_issued}"))
        self.assertLess(escape.find("{tag_add aio_morale_retreat_issued}"), escape.find("{time 0.25}"))
        self.assertLess(escape.find("{time 0.25}"), escape.find("{action move}"))
        actor = escape.split('{"actor_state"', 1)[1].split("{tag_add aio_morale_retreat_issued}", 1)[0]
        self.assertIn("{tag aio_morale_retreat_issued}", actor.split("{exclude", 1)[1])
        self.assertNotIn("{tag aio_morale_retreat_issued}", actor.split("{exclude", 1)[0])
        self.assertIn("{tag spawn_a}", escape)
        self.assertIn("{tag spawn_b}", escape)
        self.assertIn("enemy_spawnside$", escape)
        self.assertIn("id_1st_player$", escape)
        self.assertIn("{relation ally}", escape)
        self.assertNotIn("broken/rally", beh)
        self.assertNotIn("{tag aio_cmd_junior}", escape)
        self.assertNotIn("{tag aio_cmd_primary}", escape)
        self.assertIn("{tag aio_morale_retreat_issued}", escape.split("{action move}", 1)[0])

    def test_spawnside_routes_player_and_enemy_to_own_rear(self) -> None:
        escape = BEH.read_text(encoding="utf-8").split("broken/escape", 1)[1].split("broken/arrive", 1)[0]
        self.assertIn('{var "enemy_spawnside$"}', escape)
        le = escape.split('{op "<="}', 1)[1]
        ally_after = le.split("{relation ally}", 1)[1][:1200]
        self.assertIn("{tag spawn_b}", ally_after)
        eq = escape.split('{op "=="}', 1)
        self.assertGreater(len(eq), 1)
        spawn2 = escape.split('{var "enemy_spawnside$"}', 2)[-1]
        ally2 = spawn2.split("{relation ally}", 1)[1][:1200]
        self.assertIn("{tag spawn_a}", ally2)

    def test_arrive_rallies_to_shaken_and_releases_ownership(self) -> None:
        beh = BEH.read_text(encoding="utf-8")
        arrive = beh.split("broken/arrive", 1)[1]
        self.assertIn("{tag spawn_a}", arrive)
        self.assertIn("{tag spawn_b}", arrive)
        self.assertIn("{effect aio_morale_rally}", arrive)
        self.assertIn("{meters 25}", arrive)
        self.assertNotIn("{meters 200}", beh)
        human = HUMAN.read_text(encoding="utf-8")
        rally = human.split('{on "aio_morale_rally"', 1)[1].split('{on "', 1)[0]
        self.assertIn('{tags remove "aio_morale_broken"}', rally)
        self.assertIn('{tags remove "aio_morale_owned"}', rally)
        self.assertIn('{tags remove "aio_morale_retreat_issued"}', rally)
        self.assertIn('{tags remove "aio_morale_retreat_wave"}', rally)
        self.assertIn('{tags add "aio_morale_shaken"}', rally)
        self.assertIn('{call "recovering_from_shaken"}', rally)
        self.assertNotIn('{tags add "aio_morale_panic"}', rally)
        self.assertNotIn('{player "0"}', rally)
        self.assertNotIn('{call "recovered_from_shaken"}', rally)
        self.assertNotIn('{tags remove "aio_morale_shaken"}', rally)
        self.assertNotIn('{tags add "aio_morale_did_recover"}', rally)

    def test_later_breaks_do_not_reissue_runners(self) -> None:
        beh = BEH.read_text(encoding="utf-8")
        escape = beh.split("broken/escape", 1)[1].split("broken/arrive", 1)[0]
        self.assertEqual(escape.count("{tag_add aio_morale_retreat_issued}"), 1)
        issued_on_moves = 0
        for part in escape.split('{"action"')[1:]:
            if "{action move}" not in part:
                continue
            selector = part.split("{action move}", 1)[0]
            if "aio_morale_retreat_issued" in selector:
                issued_on_moves += 1
        self.assertGreater(issued_on_moves, 0)

    def test_effect_selectors_exclude_dead_inactive(self) -> None:
        parts = BEH.read_text(encoding="utf-8").split('{"effect"')
        self.assertGreater(len(parts), 1)
        for part in parts[1:]:
            block = part.split("{effect ", 1)[0]
            self.assertIn("{state dead}", block)
            self.assertIn("{state inactive}", block)

    def test_cleanup_strips_inactive_broken_tags(self) -> None:
        cleanup = BEH.read_text(encoding="utf-8").split("broken/cleanup_dead", 1)[1]
        self.assertIn("{state dead}", cleanup)
        self.assertIn("{state inactive}", cleanup)
        for tag in (
            "aio_morale_owned",
            "aio_morale_broken",
            "aio_morale_retreat_issued",
        ):
            self.assertIn("tag_remove " + tag, cleanup)
        self.assertNotIn("aio_morale_surrendering", cleanup)
        self.assertNotIn("aio_pow_camp", cleanup)

    def test_actor_state_selectors_exclude_dead_and_inactive(self) -> None:
        text = BEH.read_text(encoding="utf-8")
        parts = text.split('{"actor_state"')
        self.assertGreater(len(parts), 1)
        for part in parts[1:]:
            block = part.split('{"', 1)[0]
            self.assertIn("{state dead}", block)
            self.assertIn("{state inactive}", block)


if __name__ == "__main__":
    unittest.main()
