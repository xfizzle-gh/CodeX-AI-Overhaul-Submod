from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GAME_SET = ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"
BOT_MAIN = ROOT / "resource/script/multiplayer/bot.main.lua"
ATTACK_MATE = ROOT / "resource/script/multiplayer/modes/attacker_mate.lua"


class AttackMateSlotProofTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.game_set = GAME_SET.read_text(encoding="utf-8")
        cls.bot_main = BOT_MAIN.read_text(encoding="utf-8")
        cls.attack_mate = ATTACK_MATE.read_text(encoding="utf-8")

    def test_team_a_requests_exactly_one_ai_mate_slot(self) -> None:
        self.assertEqual(self.game_set.count("{aiTeamPlayers 1}"), 1)
        team_a = self.game_set.index('{"a"')
        team_b = self.game_set.index('{"b"', team_a)
        self.assertIn("{aiTeamPlayers 1}", self.game_set[team_a:team_b])
        self.assertIn("{minTeamSlots 7}", self.game_set[team_a:team_b])

    def test_router_never_loads_conquest_for_team_a_attack_bots(self) -> None:
        for marker in (
            'local ROUTER_PREFIX = "CODEX_ATTACK_MATE_ROUTER"',
            'local function isCampaignTeamABot(identity)',
            'if identity.attacking == true then return true end',
            'identity.playerId == identity.defenderBotId',
            'require("resource/script/multiplayer/modes/attacker_mate")',
            '"team_a_attack_safe_route"',
            'local gameModeScriptPath = "resource/script/multiplayer/modes/" .. mode',
        ):
            self.assertIn(marker, self.bot_main)

        # FirstPlayerId is not a human-only identity; the first live test proved
        # it can equal the Team A AI player ID.
        self.assertNotIn(
            "identity.playerId == identity.firstPlayerId",
            self.bot_main,
        )

        attack_gate = self.bot_main.index("if identity.attacking == true then return true end")
        defender_gate = self.bot_main.index("identity.playerId == identity.defenderBotId")
        self.assertLess(attack_gate, defender_gate)

    def test_probe_is_read_only_and_publishes_only_primary_candidate(self) -> None:
        for marker in (
            'local PREFIX = "CODEX_ATTACK_MATE_PROBE"',
            'local function isPrimaryAttackMate(id)',
            'not isDefenderBot(id)',
            '"primary_attack_mate_candidate"',
            '"attack_defenderbot_shadow"',
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

    def test_lua_delimiters_are_reasonably_balanced(self) -> None:
        for text in (self.bot_main, self.attack_mate):
            self.assertEqual(text.count("("), text.count(")"))


if __name__ == "__main__":
    unittest.main()
