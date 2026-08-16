from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BOT_MAIN_LUA = ROOT / "resource/script/multiplayer/bot.main.lua"
HANDOFF_LUA = ROOT / "resource/script/multiplayer/modes/attack_support_handoff.lua"
GAME_SET = ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"
ATTACK_SUPPORT_WAVES_INC = ROOT / "resource/map/multi/attack_support_waves.inc"
DEFENSE_SUPPORT_WAVES_INC = ROOT / "resource/map/multi/defense_support_waves.inc"
ENEMY_DEFENSE_SUPPORT_INC = ROOT / "resource/map/multi/enemy_defense_support.inc"
ENEMY_ATTACK_SUPPORT_INC = ROOT / "resource/map/multi/enemy_attack_support.inc"


class AlliedSupportSharedFowAndGateTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.bot_main = BOT_MAIN_LUA.read_text(encoding="utf-8")
        cls.handoff = HANDOFF_LUA.read_text(encoding="utf-8")
        cls.game_set = GAME_SET.read_text(encoding="utf-8")
        cls.attack_waves = ATTACK_SUPPORT_WAVES_INC.read_text(encoding="utf-8")
        cls.defense_waves = DEFENSE_SUPPORT_WAVES_INC.read_text(encoding="utf-8")
        cls.enemy_defense = ENEMY_DEFENSE_SUPPORT_INC.read_text(encoding="utf-8")
        cls.enemy_attack = ENEMY_ATTACK_SUPPORT_INC.read_text(encoding="utf-8")

    def test_attack_support_does_not_create_a_custom_team_a_bot(self) -> None:
        self.assertNotIn("aiTeamPlayers", self.game_set)
        self.assertNotIn("isAttackSupportCandidate", self.bot_main)
        self.assertNotIn(
            'safeRequire("resource/script/multiplayer/modes/attack_support")',
            self.bot_main,
        )
        self.assertIn(
            'safeRequire("resource/script/multiplayer/modes/attack_support_handoff")',
            self.bot_main,
        )

    def test_handoff_uses_vanilla_first_player_identity_from_mission_authority(self) -> None:
        self.assertIn(
            "firstPlayerId = positiveId(c.FirstPlayerId, i.CampaignFirstPlayerId)",
            self.handoff,
        )
        self.assertIn(
            "firstEnemyId = positiveId(c.FirstEnemyId, i.CampaignFirstEnemyId)",
            self.handoff,
        )
        self.assertIn(
            "if id.firstEnemyId > 0 and id.playerId ~= id.firstEnemyId then",
            self.handoff,
        )
        self.assertIn("if id.attacking == true then", self.handoff)
        self.assertIn("if id.attacking ~= false then return false end", self.handoff)
        self.assertIn(
            'sc:SetVar("id_attack_support", id.firstPlayerId)',
            self.handoff,
        )
        self.assertIn('sc:SetVar("attack_support_ready", 1)', self.handoff)
        self.assertIn('sc:SetVar("attack_support_use_mi", 1)', self.handoff)
        self.assertNotIn("DefenderBotId", self.handoff)
        self.assertNotIn('sc:SetVar("id_attack_support", id.playerId)', self.handoff)

    def test_handoff_retries_late_conquest_identity(self) -> None:
        self.assertIn("published = false", self.handoff)
        self.assertIn("applicable = nil", self.handoff)
        self.assertIn("if id.firstEnemyId <= 0 or id.firstPlayerId <= 0 then", self.handoff)
        self.assertIn(
            "if not state.published and state.applicable ~= false then",
            self.handoff,
        )
        self.assertIn("publish(true)", self.handoff)

    def test_attack_support_stays_ai_controlled_and_unselectable(self) -> None:
        self.assertIn('(define "am_own_to_support"', self.attack_waves)
        self.assertIn('{var "id_attack_support$"}', self.attack_waves)
        self.assertIn('{control AI}', self.attack_waves)
        self.assertIn('{ai_move {mode enable}}', self.attack_waves)
        self.assertIn('{remove select}', self.attack_waves)
        self.assertNotIn("control user", self.attack_waves)

    def test_all_four_support_quadrants_keep_their_intended_owner_variables(self) -> None:
        self.assertIn('{var "id_attack_support$"}', self.attack_waves)
        self.assertIn('{var "id_defenderbot$"}', self.defense_waves)
        self.assertIn('{var "id_1st_enemy$"}', self.enemy_defense)
        self.assertIn('{var "id_1st_enemy$"}', self.enemy_attack)

        for name, text in (
            ("attack_waves", self.attack_waves),
            ("defense_waves", self.defense_waves),
            ("enemy_defense", self.enemy_defense),
            ("enemy_attack", self.enemy_attack),
        ):
            with self.subTest(quadrant=name):
                self.assertNotIn('{player "0"}', text)
                self.assertNotIn("control user", text)

    def test_canonical_mission_support_gate_remains_100_percent(self) -> None:
        roll_start = self.attack_waves.find('{"support_mission/roll"')
        self.assertGreater(roll_start, 0)
        roll_end = self.attack_waves.find('{"attack_support/init"', roll_start)
        self.assertGreater(roll_end, roll_start)
        roll_block = self.attack_waves[roll_start:roll_end]

        self.assertIn('{var "support_mission_roll_done$"} {op "=="} {value 0}', roll_block)
        self.assertIn('{var "support_mission_roll_done$"} {op "="} {value 1}', roll_block)
        self.assertIn('{condition {type rand} {value 1.0}}', roll_block)
        self.assertIn('{var "support_mission_enabled$"} {op "="} {value 1}', roll_block)

    def test_attack_support_still_arms_from_existing_mi_contract(self) -> None:
        start = self.attack_waves.find('{"attack_support/init"')
        end = self.attack_waves.find('{"attack_support/clock"', start)
        self.assertGreater(start, 0)
        self.assertGreater(end, start)
        block = self.attack_waves[start:end]

        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', block)
        self.assertIn('{var "attack_support_ready$"} {op "=="} {value 1}', block)
        self.assertIn('{var "attack_support_use_mi$"} {op "=="} {value 1}', block)
        self.assertIn('{var "id_attack_support$"} {op ">"} {value 0}', block)
        self.assertIn('{var "support_mission_enabled$"} {op "=="} {value 1}', block)
        self.assertIn('{var "attack_support_armed$"} {op "="} {value 1}', block)


if __name__ == "__main__":
    unittest.main()
