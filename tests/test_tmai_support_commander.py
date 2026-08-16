from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SUPPORT = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
HANDOFF = ROOT / "resource/map/multi/attack_support_tmai_handoff.inc"
BOT_MAIN = ROOT / "resource/script/multiplayer/bot.main.lua"
NATIVE_FOW = ROOT / "resource/script/multiplayer/modes/native_support_fow_test.lua"


class TmaiReferencedSupportCommanderTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.support = SUPPORT.read_text(encoding="utf-8")
        cls.handoff = HANDOFF.read_text(encoding="utf-8")
        cls.bot_main = BOT_MAIN.read_text(encoding="utf-8")
        cls.native_fow = NATIVE_FOW.read_text(encoding="utf-8")

    def test_tmai_handoff_is_explicit_and_observable(self) -> None:
        self.assertIn("TMAI v0.17", self.support)
        self.assertIn('local HANDOFF_PREFIX = "CODEX_TMAI_HANDOFF"', self.support)
        self.assertIn('"flow", "human_origin_to_mate_to_MI_action_move"', self.support)
        self.assertIn('"order_transport", "MI_action_move"', self.support)

    def test_controller_uses_identity_bus_not_botapi_combat_orders(self) -> None:
        self.assertIn('setVar("id_attack_support_human", humanId)', self.support)
        self.assertIn('setVar("id_attack_support_mate", mateId)', self.support)
        self.assertIn('setVar("tmai_handoff_enabled", 1)', self.support)
        for forbidden_call in (
            ":CaptureFlag(",
            ".CaptureFlag(",
            ":SeekAndDestroy(",
            ".SeekAndDestroy(",
            "BotApi.Commands:",
            "BotApi.Commands.",
        ):
            with self.subTest(forbidden_call=forbidden_call):
                self.assertNotIn(forbidden_call, self.support)

    def test_fragile_mate_slot_uses_no_utility_or_native_scene_polling(self) -> None:
        forbidden = (
            r"require\(\[\[/script/multiplayer/modes/utility\]\]\)",
            r'require\(["\']resource/script/multiplayer/modes/utility["\']\)',
            r"require\(\[\[/script/multiplayer/logic/main\]\]\)",
        )
        for pattern in forbidden:
            with self.subTest(pattern=pattern):
                self.assertIsNone(re.search(pattern, self.support))
        self.assertNotIn("QueryScene", self.support.replace("QueryScene polling", "scene polling"))
        self.assertIn('return humanId, "campaign_four_slot_complement"', self.support)
        self.assertIn("for playerId = 1, 4 do", self.support)

    def test_human_origin_is_established_before_attack_support_ready(self) -> None:
        arm = self.support[self.support.index("local function armHumanOriginHandoff") : self.support.index("local function mirrorState")]
        human = arm.index('setVar("id_attack_support_human", humanId)')
        first_owner = arm.index('setVar("id_attack_support", humanId)')
        prepare = arm.index('setVar("tmai_handoff_prepare", 1)')
        prepared_gate = arm.index('readVarNumber("tmai_handoff_prepared") ~= 1')
        ready = arm.index('setVar("attack_support_ready", 1)')
        self.assertLess(human, first_owner)
        self.assertLess(first_owner, prepare)
        self.assertLess(prepare, prepared_gate)
        self.assertLess(prepared_gate, ready)

    def test_bridge_mimics_manual_transfer_lifecycle_in_order(self) -> None:
        human_seed = self.handoff.index('("tmai_set_human_owner" args attack_support_tpl)')
        control_user = self.handoff.index("{control user}", human_seed)
        first_dwell = self.handoff.index('{"delay" {time 3}}', control_user)
        mate = self.handoff.index('("tmai_set_mate_owner" args tmai_handoff_pending)', first_dwell)
        control_ai = self.handoff.index("{control AI}", mate)
        settle = self.handoff.index('{"delay" {time 3}}', control_ai)
        first_move = self.handoff.index("{action move}", settle)
        self.assertLess(human_seed, control_user)
        self.assertLess(control_user, first_dwell)
        self.assertLess(first_dwell, mate)
        self.assertLess(mate, control_ai)
        self.assertLess(control_ai, settle)
        self.assertLess(settle, first_move)

    def test_handoff_only_processes_newly_deployed_support_once(self) -> None:
        self.assertIn('{select {tag {tag attack_support_src}}}', self.handoff)
        self.assertIn('{exclude {tag {tag tmai_handoff_done}}}', self.handoff)
        self.assertIn('{exclude {tag {tag tmai_handoff_pending}}}', self.handoff)
        self.assertIn('{tag_add tmai_handoff_pending}', self.handoff)
        self.assertIn('{tag_add tmai_handoff_done}', self.handoff)
        self.assertIn('{tag_remove tmai_handoff_pending}', self.handoff)
        self.assertIn('{var "tmai_handoff_seq$"} {op "+"} {value 1}', self.handoff)

    def test_tmai_small_infantry_groups_exclude_linked_vehicle_crews(self) -> None:
        self.assertIn('{include {prop {prop human}}}', self.handoff)
        self.assertIn('{exclude {state {state linked}}}', self.handoff)
        self.assertIn('{amount 4}', self.handoff)
        self.assertIn('{tag_add tmai_move_g1}', self.handoff)
        self.assertIn('{tag_add tmai_move_g2}', self.handoff)
        self.assertIn('{tag tmai_move_g1}', self.handoff)
        self.assertIn('{tag tmai_move_g2}', self.handoff)

    def test_each_production_humvee_is_tasked_individually(self) -> None:
        for index in range(1, 5):
            marker = f'{{tag attack_support_hmmwv{index}}} {{type vehicle}}'
            with self.subTest(index=index):
                self.assertIn(marker, self.handoff)
        self.assertNotIn("include {type", self.handoff)

    def test_mi_action_move_spreads_groups_across_distinct_objectives(self) -> None:
        for flag in ("tmai_support_flag1", "tmai_support_flag2", "tmai_support_flag3"):
            self.assertIn(flag, self.handoff)
        self.assertGreaterEqual(self.handoff.count("{action move}"), 6)
        self.assertIn('{target {ignore_captured_by_user 0} {tag tmai_support_flag1}}', self.handoff)
        self.assertIn('{target {ignore_captured_by_user 0} {tag tmai_support_flag2}}', self.handoff)
        self.assertIn('{target {ignore_captured_by_user 0} {tag tmai_support_flag3}}', self.handoff)
        self.assertNotIn("{action advance}", self.handoff)

    def test_human_and_mate_switches_cover_all_runtime_player_ids(self) -> None:
        for player_id in range(1, 17):
            human = f'{{var "id_attack_support_human$"}} {{op "=="}} {{value {player_id}}}'
            mate = f'{{var "id_attack_support_mate$"}} {{op "=="}} {{value {player_id}}}'
            with self.subTest(player_id=player_id):
                self.assertIn(human, self.handoff)
                self.assertIn(mate, self.handoff)

    def test_commander_only_runs_for_routed_human_attack_mate_slot(self) -> None:
        self.assertIn('if identity.team ~= "a" then return false end', self.bot_main)
        self.assertIn("if identity.isHuman then return false end", self.bot_main)
        self.assertIn("identity.playerId == identity.defenderBotId", self.bot_main)
        self.assertIn('safeRequire("resource/script/multiplayer/modes/attack_support")', self.bot_main)
        self.assertIn("if id.attacking == false then", self.support)

    def test_pr99_native_fow_probe_remains_separate(self) -> None:
        attack_route = self.bot_main.index("if isAttackSupportCandidate(identity) then")
        attack_return = self.bot_main.index("return", attack_route)
        native_require = self.bot_main.index("native_support_fow_test")
        self.assertLess(attack_return, native_require)
        self.assertIn("CODEX_NATIVE_SUPPORT_TEST", self.native_fow)
        self.assertNotIn("CODEX_NATIVE_SUPPORT_TEST", self.support)
        self.assertNotIn("CODEX_NATIVE_SUPPORT_TEST", self.handoff)


if __name__ == "__main__":
    unittest.main()
