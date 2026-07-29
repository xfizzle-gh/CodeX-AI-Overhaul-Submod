from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GAME_SET = ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"
BOT_MAIN = ROOT / "resource/script/multiplayer/bot.main.lua"
ATTACK_MATE = ROOT / "resource/script/multiplayer/modes/attacker_mate.lua"
DEPLOY = ROOT / "tools/deploy_attack_mate_probe.ps1"


class AttackMateSlotProofTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.game_set = GAME_SET.read_text(encoding="utf-8")
        cls.bot_main = BOT_MAIN.read_text(encoding="utf-8")
        cls.attack_mate = ATTACK_MATE.read_text(encoding="utf-8")
        cls.deploy = DEPLOY.read_text(encoding="utf-8")

    def test_team_a_requests_exactly_one_ai_mate_slot(self) -> None:
        self.assertEqual(self.game_set.count("{aiTeamPlayers 1}"), 1)
        team_a = self.game_set.index('{"a"')
        team_b = self.game_set.index('{"b"', team_a)
        self.assertIn("{aiTeamPlayers 1}", self.game_set[team_a:team_b])
        self.assertIn("{minTeamSlots 7}", self.game_set[team_a:team_b])

    def test_router_matches_proven_manual_transfer_checkpoint(self) -> None:
        for marker in (
            'local ROUTER_PREFIX = "CODEX_ATTACK_MATE_ROUTER"',
            "local function isAttackMateCandidate(identity)",
            "identity.firstPlayerId > 0 and identity.playerId == identity.firstPlayerId",
            'routerLog("route_skip", "first_player_slot", "playerId", identity.playerId)',
            "local function safeRequire(path)",
            'safeRequire("resource/script/multiplayer/modes/attacker_mate")',
            'local gameModeScriptPath = "resource/script/multiplayer/modes/" .. mode',
            "pcall(initialize)",
        ):
            self.assertIn(marker, self.bot_main)

        # Live proof: the transferable Team A AI process reported the same ID as
        # FirstPlayerId. Running a Lua controller on it crashed; skipping it left
        # the slot alive and allowed engine-level ownership transfer.
        skip_gate = self.bot_main.index(
            "identity.firstPlayerId > 0 and identity.playerId == identity.firstPlayerId"
        )
        mate_route = self.bot_main.index("if isAttackMateCandidate(identity) then")
        self.assertLess(skip_gate, mate_route)
        self.assertNotIn("team_a_attack_safe_route", self.bot_main)

    def test_probe_remains_read_only(self) -> None:
        for marker in (
            'local PREFIX = "CODEX_ATTACK_MATE_PROBE"',
            'sc:SetVar("id_attacker_mate", id.playerId)',
            'sc:SetVar("attacker_mate_ready", 1)',
            '"scene_squads"',
            '"scene_flags"',
            '"diagnostics_only"',
            '"orders", "disabled"',
        ):
            self.assertIn(marker, self.attack_mate)

        forbidden = (
            "CaptureFlag(",
            "SeekAndDestroy(",
            ":Spawn(",
            ":SpawnAt(",
            ":Purchase(",
            "GameModeSpawnUnit(",
        )
        for marker in forbidden:
            self.assertNotIn(marker, self.attack_mate)

    def test_deployment_uses_active_checkout_and_requires_checkpoint_router(self) -> None:
        for marker in (
            "$MyInvocation.MyCommand.Path",
            'Join-Path $ScriptDirectory ".."',
            '$ExpectedBranch = "experiment/attack-mate-slot-proof"',
            "git -C $RepoRoot branch --show-current",
            'route_skip", "first_player_slot',
            "identity.firstPlayerId > 0 and identity.playerId == identity.firstPlayerId",
            "safeRequire",
            "diagnostics_only",
        ):
            self.assertIn(marker, self.deploy)

        self.assertNotIn(
            "CodeX AI Overhaul Submod",
            self.deploy,
            "deployment must not read from the obsolete space-named checkout",
        )
        self.assertNotIn("team_a_attack_safe_route", self.deploy)

    def test_lua_delimiters_are_reasonably_balanced(self) -> None:
        for text in (self.bot_main, self.attack_mate):
            self.assertEqual(text.count("("), text.count(")"))


if __name__ == "__main__":
    unittest.main()
