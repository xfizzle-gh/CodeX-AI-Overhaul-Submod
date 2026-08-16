from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SUPPORT = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
BOT_MAIN = ROOT / "resource/script/multiplayer/bot.main.lua"
NATIVE_FOW = ROOT / "resource/script/multiplayer/modes/native_support_fow_test.lua"


class TmaiReferencedSupportCommanderTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.support = SUPPORT.read_text(encoding="utf-8")
        cls.bot_main = BOT_MAIN.read_text(encoding="utf-8")
        cls.native_fow = NATIVE_FOW.read_text(encoding="utf-8")

    def test_tmai_reference_is_explicit_and_observable(self) -> None:
        self.assertIn("TMAI v0.17", self.support)
        self.assertIn('local COMMANDER_PREFIX = "CODEX_TMAI_SUPPORT"', self.support)
        self.assertIn('"reference", "TMAI_v0.17"', self.support)

    def test_three_second_settle_uses_engine_quant_timer(self) -> None:
        self.assertIn("local TMAI_SETTLE_MS = 3000", self.support)
        self.assertIn("ev:SetQuantTimer(function() settleManaged(entry.key, generation) end, TMAI_SETTLE_MS)", self.support)
        self.assertIn('commanderLog("settled", key, "after_ms", TMAI_SETTLE_MS)', self.support)

    def test_commander_replaces_random_and_periodic_order_spam(self) -> None:
        self.assertNotIn("local function pickFlagName()", self.support)
        self.assertNotIn("math.random(#names)", self.support)
        self.assertNotIn("state.ordered", self.support)
        self.assertNotIn("state.quant % 400 == 0", self.support)
        self.assertIn("entry.lastRole == role and entry.lastTarget == flagName", self.support)
        self.assertIn("state.planDirty", self.support)

    def test_managed_groups_settle_and_prune_against_live_scene_squads(self) -> None:
        self.assertIn("managed = {}", self.support)
        self.assertIn("local function discoverAndPruneSquads()", self.support)
        self.assertIn("state.managed[key] = entry", self.support)
        self.assertIn("state.managed[key] = nil", self.support)
        self.assertIn('commanderLog("pruned", key, "not_in_scene_squads")', self.support)
        self.assertIn("entry.settled = true", self.support)

    def test_objectives_are_distinct_before_reinforcement(self) -> None:
        plan = self.support[self.support.index("local function buildPlan") : self.support.index("local function applyCommanderPlan")]
        unique_pass = plan.index("for _, flag in ipairs(attackFlags) do")
        reinforcement = plan.index('assignDesired(desired, entries[nextEntry], "reinforce", best.name)')
        self.assertLess(unique_pass, reinforcement)
        self.assertIn('state.recentlyLost[flag.name] and "counterattack" or "attack"', plan)
        self.assertIn("reinforceLoads", plan)

    def test_recently_lost_friendly_flag_becomes_counterattack_priority(self) -> None:
        self.assertIn('previous == "friendly" and relation ~= "friendly"', self.support)
        self.assertIn("state.recentlyLost[name] = true", self.support)
        self.assertIn("if al ~= bl then return al end", self.support)
        self.assertIn('commanderLog("flag_lost", name, "to", relation)', self.support)
        self.assertIn('"counterattack"', self.support)

    def test_newly_captured_ground_gets_bounded_hold_groups(self) -> None:
        self.assertIn("local MAX_CAPTURE_HOLD_GROUPS = 4", self.support)
        self.assertIn('previous ~= "friendly" and relation == "friendly"', self.support)
        self.assertIn("state.newlyCaptured[name] = true", self.support)
        self.assertIn('assignDesired(desired, entries[nextEntry], "hold", flag.name)', self.support)
        self.assertIn("holdCount < MAX_CAPTURE_HOLD_GROUPS", self.support)

    def test_commander_keeps_a_small_reserve(self) -> None:
        self.assertIn("local MAX_RESERVE_GROUPS = 1", self.support)
        self.assertIn('assignDesired(desired, entries[nextEntry], "reserve", nil)', self.support)
        self.assertIn('entry.lastRole = "reserve"', self.support)
        self.assertIn('commanderLog("reserve", entry.key, "hold_current_position")', self.support)

    def test_transport_remains_proven_captureflag_with_seek_fallback(self) -> None:
        self.assertIn("c:CaptureFlag(entry.squad, flagName)", self.support)
        self.assertIn("c:SeekAndDestroy(entry.squad)", self.support)
        self.assertIn('"SeekAndDestroy_fallback"', self.support)
        self.assertNotIn("action move", self.support)

    def test_fragile_support_slot_still_never_loads_utility(self) -> None:
        forbidden = (
            r"require\(\[\[/script/multiplayer/modes/utility\]\]\)",
            r'require\(["\']resource/script/multiplayer/modes/utility["\']\)',
            r"require\(\[\[/script/multiplayer/logic/main\]\]\)",
        )
        for pattern in forbidden:
            with self.subTest(pattern=pattern):
                self.assertIsNone(re.search(pattern, self.support))

    def test_commander_only_runs_for_routed_human_attack_support_slot(self) -> None:
        self.assertIn('if identity.team ~= "a" then return false end', self.bot_main)
        self.assertIn("if identity.isHuman then return false end", self.bot_main)
        self.assertIn("identity.playerId == identity.defenderBotId", self.bot_main)
        self.assertIn('safeRequire("resource/script/multiplayer/modes/attack_support")', self.bot_main)
        self.assertIn("if state.attackMission == true", self.support)
        self.assertIn("if id.attacking == false then", self.support)

    def test_pr99_native_fow_diagnostic_remains_separate(self) -> None:
        attack_route = self.bot_main.index("if isAttackSupportCandidate(identity) then")
        attack_return = self.bot_main.index("return", attack_route)
        native_require = self.bot_main.index("native_support_fow_test")
        self.assertLess(attack_return, native_require)
        self.assertIn("CODEX_NATIVE_SUPPORT_TEST", self.native_fow)
        self.assertNotIn("CODEX_NATIVE_SUPPORT_TEST", self.support)


if __name__ == "__main__":
    unittest.main()
