from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ATTACK_SUPPORT_LUA = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
ATTACK_SUPPORT_WAVES_INC = ROOT / "resource/map/multi/attack_support_waves.inc"
DEFENSE_SUPPORT_WAVES_INC = ROOT / "resource/map/multi/defense_support_waves.inc"
ENEMY_DEFENSE_SUPPORT_INC = ROOT / "resource/map/multi/enemy_defense_support.inc"
ENEMY_ATTACK_SUPPORT_INC = ROOT / "resource/map/multi/enemy_attack_support.inc"


class AlliedSupportSharedFowAndGateTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.attack_support_lua = ATTACK_SUPPORT_LUA.read_text(encoding="utf-8")
        cls.attack_waves_inc = ATTACK_SUPPORT_WAVES_INC.read_text(encoding="utf-8")
        cls.defense_waves_inc = DEFENSE_SUPPORT_WAVES_INC.read_text(encoding="utf-8")
        cls.enemy_defense_inc = ENEMY_DEFENSE_SUPPORT_INC.read_text(encoding="utf-8")
        cls.enemy_attack_inc = ENEMY_ATTACK_SUPPORT_INC.read_text(encoding="utf-8")

    def test_attack_support_assigns_real_allied_ai_owner_not_controller(self) -> None:
        # 1. Must resolve ownerId from defenderBotId (with fallback to controller playerId)
        self.assertIn(
            "local ownerId = positiveId(id.defenderBotId, id.playerId)",
            self.attack_support_lua,
        )
        self.assertIn(
            'sc:SetVar("id_attack_support", ownerId)',
            self.attack_support_lua,
        )

        # 2. Must NEVER assign phantom controller playerId or human player directly
        self.assertNotIn(
            'sc:SetVar("id_attack_support", id.playerId)',
            self.attack_support_lua,
        )
        self.assertNotIn(
            'sc:SetVar("id_attack_support", id.firstPlayerId)',
            self.attack_support_lua,
        )

        # 3. publishIdentity must preserve readiness and MI wave flags
        publish_start = self.attack_support_lua.find("local function publishIdentity(id)")
        publish_end = self.attack_support_lua.find("\nend\n", publish_start)
        self.assertGreater(publish_start, 0)
        self.assertGreater(publish_end, publish_start)
        publish_body = self.attack_support_lua[publish_start:publish_end]

        self.assertIn('sc:SetVar("attack_support_ready", 1)', publish_body)
        self.assertIn('sc:SetVar("attack_support_use_mi", 1)', publish_body)
        self.assertNotIn("id.firstPlayerId", publish_body)
        self.assertIn("if ownerId <= 0 then", publish_body)

    def test_all_four_support_quadrants_have_correct_ai_ownership(self) -> None:
        # Quadrant 1: Friendly Attack Support -> id_attack_support$
        self.assertIn('{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}}', self.attack_waves_inc)
        self.assertIn('{var "id_attack_support$"}', self.attack_waves_inc)

        # Quadrant 2: Friendly Defense Support -> id_defenderbot$ (real allied defender bot)
        self.assertIn('{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 1}}', self.defense_waves_inc)
        self.assertIn('{var "id_defenderbot$"}', self.defense_waves_inc)

        # Quadrant 3: Enemy Defense Support -> id_1st_enemy$ (real enemy bot)
        self.assertIn('{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 1}}', self.enemy_defense_inc)
        self.assertIn('{var "id_1st_enemy$"}', self.enemy_defense_inc)

        # Quadrant 4: Enemy Attack Support -> id_1st_enemy$ (real enemy bot)
        self.assertIn('{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 1}}', self.enemy_attack_inc)
        self.assertIn('{var "id_1st_enemy$"}', self.enemy_attack_inc)

        # No support quadrant transfers units to the human player
        for name, text in (
            ("attack_waves", self.attack_waves_inc),
            ("defense_waves", self.defense_waves_inc),
            ("enemy_defense", self.enemy_defense_inc),
            ("enemy_attack", self.enemy_attack_inc),
        ):
            with self.subTest(quadrant=name):
                self.assertNotIn('{player "0"}', text)
                self.assertNotIn('control user', text)

    def test_canonical_mission_support_gate_is_100_percent(self) -> None:
        # Find support_mission/roll block
        roll_start = self.attack_waves_inc.find('{"support_mission/roll"')
        self.assertGreater(roll_start, 0)
        roll_end = self.attack_waves_inc.find('{"attack_support/init"', roll_start)
        self.assertGreater(roll_end, roll_start)
        roll_block = self.attack_waves_inc[roll_start:roll_end]

        # Must roll once per mission
        self.assertIn('{var "support_mission_roll_done$"} {op "=="} {value 0}', roll_block)
        self.assertIn('{var "support_mission_roll_done$"} {op "="} {value 1}', roll_block)

        # Must use 1.0 (100% chance) for testing
        self.assertIn('{condition {type rand} {value 1.0}}', roll_block)
        self.assertIn('{var "support_mission_enabled$"} {op "="} {value 1}', roll_block)

    def test_both_appropriate_support_systems_arm_on_human_attack(self) -> None:
        # Friendly Attack Support arms when user_is_defender$ == 0 and support_mission_enabled$ == 1
        attack_init_start = self.attack_waves_inc.find('{"attack_support/init"')
        self.assertGreater(attack_init_start, 0)
        attack_init_end = self.attack_waves_inc.find('{"attack_support/clock"', attack_init_start)
        self.assertGreater(attack_init_end, attack_init_start)
        attack_init = self.attack_waves_inc[attack_init_start:attack_init_end]

        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', attack_init)
        self.assertIn('{var "support_mission_enabled$"} {op "=="} {value 1}', attack_init)
        self.assertIn('{var "id_attack_support$"} {op ">"} {value 0}', attack_init)
        self.assertIn('{var "attack_support_armed$"} {op "="} {value 1}', attack_init)

        # Enemy Defense Support arms when user_is_defender$ == 0 and support_mission_enabled$ == 1
        enemy_def_init_start = self.enemy_defense_inc.find('{"enemy_defense/init"')
        self.assertGreater(enemy_def_init_start, 0)
        enemy_def_init_end = self.enemy_defense_inc.find('{"enemy_defense/trickle"', enemy_def_init_start)
        self.assertGreater(enemy_def_init_end, enemy_def_init_start)
        enemy_def_init = self.enemy_defense_inc[enemy_def_init_start:enemy_def_init_end]

        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', enemy_def_init)
        self.assertIn('{var "support_mission_enabled$"} {op "=="} {value 1}', enemy_def_init)
        self.assertIn('{var "id_1st_enemy$"} {op ">"} {value 0}', enemy_def_init)
        self.assertIn('{var "enemy_defense_armed$"} {op "="} {value 1}', enemy_def_init)

    def test_both_appropriate_support_systems_arm_on_human_defense(self) -> None:
        # Friendly Defense Support arms when user_is_defender$ == 1 and support_mission_enabled$ == 1
        def_init_start = self.defense_waves_inc.find('{"defense_support/init"')
        self.assertGreater(def_init_start, 0)
        def_init_end = self.defense_waves_inc.find('{"defense_support/clock"', def_init_start)
        self.assertGreater(def_init_end, def_init_start)
        def_init = self.defense_waves_inc[def_init_start:def_init_end]

        self.assertIn('{var "user_is_defender$"} {op "=="} {value 1}', def_init)
        self.assertIn('{var "support_mission_enabled$"} {op "=="} {value 1}', def_init)
        self.assertIn('{var "id_defenderbot$"} {op ">"} {value 0}', def_init)
        self.assertIn('{var "defense_support_armed$"} {op "="} {value 1}', def_init)

        # Enemy Attack Support arms when user_is_defender$ == 1 and support_mission_enabled$ == 1
        enemy_att_init_start = self.enemy_attack_inc.find('{"enemy_attack/init"')
        self.assertGreater(enemy_att_init_start, 0)
        enemy_att_init_end = self.enemy_attack_inc.find('{"enemy_attack/clock"', enemy_att_init_start)
        self.assertGreater(enemy_att_init_end, enemy_att_init_start)
        enemy_att_init = self.enemy_attack_inc[enemy_att_init_start:enemy_att_init_end]

        self.assertIn('{var "user_is_defender$"} {op "=="} {value 1}', enemy_att_init)
        self.assertIn('{var "support_mission_enabled$"} {op "=="} {value 1}', enemy_att_init)
        self.assertIn('{var "id_1st_enemy$"} {op ">"} {value 0}', enemy_att_init)
        self.assertIn('{var "enemy_attack_armed$"} {op "="} {value 1}', enemy_att_init)


if __name__ == "__main__":
    unittest.main()
