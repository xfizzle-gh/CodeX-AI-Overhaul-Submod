from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GAME_SET = ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"
BOT_MAIN = ROOT / "resource/script/multiplayer/bot.main.lua"
ATTACK_MATE = ROOT / "resource/script/multiplayer/modes/attacker_mate.lua"
RETASK = ROOT / "resource/map/multi/attack_mate_retask_probe.inc"
DEPLOY = ROOT / "tools/deploy_attack_mate_probe.ps1"


class AttackMateSlotProofTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.game_set = GAME_SET.read_text(encoding="utf-8")
        cls.bot_main = BOT_MAIN.read_text(encoding="utf-8")
        cls.attack_mate = ATTACK_MATE.read_text(encoding="utf-8")
        cls.retask = RETASK.read_text(encoding="utf-8")
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

        skip_gate = self.bot_main.index(
            "identity.firstPlayerId > 0 and identity.playerId == identity.firstPlayerId"
        )
        mate_route = self.bot_main.index("if isAttackMateCandidate(identity) then")
        self.assertLess(skip_gate, mate_route)
        self.assertNotIn("team_a_attack_safe_route", self.bot_main)

    def test_lua_probe_remains_read_only(self) -> None:
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

    def test_mission_probe_orders_transfers_then_retasks(self) -> None:
        for marker in (
            '"attack_mate/probe_init"',
            'ATTACK MATE PROBE ARMED FIXED DELAY',
            '{"delay" {time 40}}',
            '{tag_add attack_mate_probe}',
            'ATTACK MATE PROBE 2 LEG1 ORDERED',
            '{target {tag fpc1}}',
            '{player "3"}',
            '{tag_add attack_mate_owned}',
            '"attack_mate/probe_retask"',
            '{zone {zone "fpc1"}}',
            '{target {tag fpc2}}',
            'ATTACK MATE PROBE 4 RETASKED TO FPC2',
            'ATTACK MATE PROBE TIMEOUT BEFORE FPC1',
        ):
            self.assertIn(marker, self.retask)

        leg1 = self.retask.index('ATTACK MATE PROBE 2 LEG1 ORDERED')
        transfer = self.retask.index('{player "3"}')
        retask = self.retask.index('ATTACK MATE PROBE 4 RETASKED TO FPC2')
        self.assertLess(leg1, transfer)
        self.assertLess(transfer, retask)
        self.assertNotIn('user_is_defender$', self.retask)
        self.assertNotIn('prep_inform$', self.retask)

    def test_deployment_patches_exactly_the_cwa_map_family(self) -> None:
        for marker in (
            "$MyInvocation.MyCommand.Path",
            'Join-Path $ScriptDirectory ".."',
            '$ExpectedBranch = "experiment/attack-mate-slot-proof"',
            "git -C $RepoRoot branch --show-current",
            'route_skip", "first_player_slot',
            "identity.firstPlayerId > 0 and identity.playerId == identity.firstPlayerId",
            "safeRequire",
            "diagnostics_only",
            'resource\\map\\multi\\attack_mate_retask_probe.inc',
            "^dcg_\\[cwa71\\]_",
            "Expected 14 CWA campaign_capture_the_flag.mi files",
            '(include "../attack_mate_retask_probe.inc")',
            "_attack_mate_probe_backups",
        ):
            self.assertIn(marker, self.deploy)

        self.assertNotIn(
            "CodeX AI Overhaul Submod",
            self.deploy,
            "deployment must not read from the obsolete space-named checkout",
        )
        self.assertNotIn("team_a_attack_safe_route", self.deploy)

    def test_delimiters_are_balanced(self) -> None:
        for text in (self.bot_main, self.attack_mate):
            self.assertEqual(text.count("("), text.count(")"))

        code = "\n".join(line.split(";", 1)[0] for line in self.retask.splitlines())
        self.assertEqual(code.count("{"), code.count("}"))
        self.assertEqual(code.count("("), code.count(")"))


if __name__ == "__main__":
    unittest.main()
