from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BOT_MAIN_LUA = ROOT / "resource/script/multiplayer/bot.main.lua"
ATTACK_SUPPORT_LUA = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
ATTACK_SUPPORT_WAVES_INC = ROOT / "resource/map/multi/attack_support_waves.inc"
TMAI_HANDOFF_INC = ROOT / "resource/map/multi/attack_support_tmai_handoff.inc"
DEFENSE_SUPPORT_WAVES_INC = ROOT / "resource/map/multi/defense_support_waves.inc"
ENEMY_DEFENSE_SUPPORT_INC = ROOT / "resource/map/multi/enemy_defense_support.inc"
ENEMY_ATTACK_SUPPORT_INC = ROOT / "resource/map/multi/enemy_attack_support.inc"


class AlliedSupportSharedFowAndGateTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.bot_main_lua = BOT_MAIN_LUA.read_text(encoding="utf-8")
        cls.attack_support_lua = ATTACK_SUPPORT_LUA.read_text(encoding="utf-8")
        cls.attack_waves_inc = ATTACK_SUPPORT_WAVES_INC.read_text(encoding="utf-8")
        cls.handoff_inc = TMAI_HANDOFF_INC.read_text(encoding="utf-8")
        cls.defense_waves_inc = DEFENSE_SUPPORT_WAVES_INC.read_text(encoding="utf-8")
        cls.enemy_defense_inc = ENEMY_DEFENSE_SUPPORT_INC.read_text(encoding="utf-8")
        cls.enemy_attack_inc = ENEMY_ATTACK_SUPPORT_INC.read_text(encoding="utf-8")

    def test_router_still_reserves_extra_team_a_bot_as_mate(self) -> None:
        self.assertIn('if identity.team ~= "a" then return false end', self.bot_main_lua)
        self.assertIn("if identity.isHuman then return false end", self.bot_main_lua)
        self.assertIn(
            "if identity.defenderBotId > 0 and identity.playerId == identity.defenderBotId then",
            self.bot_main_lua,
        )
        self.assertIn(
            'safeRequire("resource/script/multiplayer/modes/attack_support")',
            self.bot_main_lua,
        )

    def test_attack_support_separates_human_origin_from_mate_destination(self) -> None:
        required = (
            'setVar("id_attack_support_human", humanId)',
            'setVar("id_attack_support_mate", mateId)',
            'setVar("id_attack_support", humanId)',
            'setVar("tmai_handoff_prepare", 1)',
            'readVarNumber("tmai_handoff_prepared") ~= 1',
            'setVar("tmai_handoff_enabled", 1)',
            'setVar("attack_support_ready", 1)',
        )
        for marker in required:
            with self.subTest(marker=marker):
                self.assertIn(marker, self.attack_support_lua)
        self.assertNotIn('setVar("id_attack_support", mateId)', self.attack_support_lua)
        self.assertNotIn('setVar("id_attack_support", id.defenderBotId)', self.attack_support_lua)

    def test_human_identity_is_discovered_and_bot_ids_are_excluded(self) -> None:
        self.assertIn('sc:QueryScene({"soldier"}, 5)', self.attack_support_lua)
        self.assertIn("if playerId == id.playerId then return true end", self.attack_support_lua)
        self.assertIn("playerId == id.firstEnemyId", self.attack_support_lua)
        self.assertIn("playerId == id.defenderBotId", self.attack_support_lua)
        self.assertIn('return 0, "ambiguous_candidates="', self.attack_support_lua)
        self.assertIn('return candidates[1].id, "single_nonbot_soldier_owner"', self.attack_support_lua)
        self.assertNotIn("humanId = id.firstPlayerId", self.attack_support_lua)

    def test_handoff_bridge_performs_human_then_mate_ownership(self) -> None:
        self.assertIn('(define "tmai_set_human_owner"', self.handoff_inc)
        self.assertIn('(define "tmai_set_mate_owner"', self.handoff_inc)
        self.assertIn('("tmai_set_human_owner" args attack_support_tpl)', self.handoff_inc)
        self.assertIn('("tmai_set_mate_owner" args tmai_handoff_pending)', self.handoff_inc)
        human_pos = self.handoff_inc.index('("tmai_set_human_owner" args attack_support_tpl)')
        mate_pos = self.handoff_inc.index('("tmai_set_mate_owner" args tmai_handoff_pending)')
        self.assertLess(human_pos, mate_pos)

    def test_all_four_support_quadrants_keep_their_existing_owner_authorities(self) -> None:
        self.assertIn(
            '{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}}',
            self.attack_waves_inc,
        )
        self.assertIn('{var "id_defenderbot$"}', self.defense_waves_inc)
        self.assertIn('{var "id_1st_enemy$"}', self.enemy_defense_inc)
        self.assertIn('{var "id_1st_enemy$"}', self.enemy_attack_inc)
        for name, text in (
            ("attack_waves", self.attack_waves_inc),
            ("defense_waves", self.defense_waves_inc),
            ("enemy_defense", self.enemy_defense_inc),
            ("enemy_attack", self.enemy_attack_inc),
        ):
            with self.subTest(quadrant=name):
                self.assertNotIn('{player "0"}', text)

    def test_canonical_mission_support_gate_is_100_percent(self) -> None:
        roll_start = self.attack_waves_inc.find('{"support_mission/roll"')
        roll_end = self.attack_waves_inc.find('{"attack_support/init"', roll_start)
        self.assertGreater(roll_start, 0)
        self.assertGreater(roll_end, roll_start)
        roll_block = self.attack_waves_inc[roll_start:roll_end]
        self.assertIn('{condition {type rand} {value 1.0}}', roll_block)
        self.assertIn('{var "support_mission_enabled$"} {op "="} {value 1}', roll_block)

    def test_attack_and_defense_role_gates_remain_separate(self) -> None:
        attack_start = self.attack_waves_inc.find('{"attack_support/init"')
        attack_end = self.attack_waves_inc.find('{"attack_support/clock"', attack_start)
        attack_init = self.attack_waves_inc[attack_start:attack_end]
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', attack_init)
        self.assertIn('{var "attack_support_ready$"} {op "=="} {value 1}', attack_init)
        self.assertIn('{var "id_attack_support$"} {op ">"} {value 0}', attack_init)

        defense_start = self.defense_waves_inc.find('{"defense_support/init"')
        defense_end = self.defense_waves_inc.find('{"defense_support/clock"', defense_start)
        defense_init = self.defense_waves_inc[defense_start:defense_end]
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 1}', defense_init)
        self.assertIn('{var "id_defenderbot$"} {op ">"} {value 0}', defense_init)


if __name__ == "__main__":
    unittest.main()
