from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ATTACK_SUPPORT_LUA = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
ATTACK_SUPPORT_WAVES_INC = ROOT / "resource/map/multi/attack_support_waves.inc"
DEFENSE_SUPPORT_WAVES_INC = ROOT / "resource/map/multi/defense_support_waves.inc"
ENEMY_DEFENSE_SUPPORT_INC = ROOT / "resource/map/multi/enemy_defense_support.inc"
ENEMY_ATTACK_SUPPORT_INC = ROOT / "resource/map/multi/enemy_attack_support.inc"


class SupportOwnershipAndGateTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.attack_support_lua = ATTACK_SUPPORT_LUA.read_text(encoding="utf-8")
        cls.attack_waves_inc = ATTACK_SUPPORT_WAVES_INC.read_text(encoding="utf-8")
        cls.defense_waves_inc = DEFENSE_SUPPORT_WAVES_INC.read_text(encoding="utf-8")
        cls.enemy_defense_inc = ENEMY_DEFENSE_SUPPORT_INC.read_text(encoding="utf-8")
        cls.enemy_attack_inc = ENEMY_ATTACK_SUPPORT_INC.read_text(encoding="utf-8")

    def test_attack_support_stays_owned_by_its_team_a_controller(self) -> None:
        """DefenderBotId is the opposing defender on human-attack missions.

        Native test on 2026-08-15 proved assigning attack support to DefenderBotId
        transferred the friendly package to the hostile "Defender AI" player.  The
        extra Team A attack-support controller is therefore the known-safe owner.
        """
        self.assertIn(
            'sc:SetVar("id_attack_support", id.playerId)',
            self.attack_support_lua,
        )
        self.assertNotIn(
            'sc:SetVar("id_attack_support", id.defenderBotId)',
            self.attack_support_lua,
        )
        self.assertNotIn(
            "local ownerId = positiveId(id.defenderBotId, 0)",
            self.attack_support_lua,
        )
        self.assertNotIn(
            'sc:SetVar("id_attack_support", id.firstEnemyId)',
            self.attack_support_lua,
        )

    def test_attack_support_only_arms_on_human_attack_role(self) -> None:
        start = self.attack_support_lua.index("local function publishIdentity(id)")
        end = self.attack_support_lua.index("\nend\n", start)
        body = self.attack_support_lua[start:end]
        self.assertIn("if id.attacking ~= true then return end", body)
        self.assertIn('sc:SetVar("attack_support_ready", 1)', body)
        self.assertIn('sc:SetVar("attack_support_use_mi", 1)', body)

    def test_all_four_support_quadrants_use_ai_owners(self) -> None:
        self.assertIn(
            '{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}}',
            self.attack_waves_inc,
        )
        self.assertIn('{var "id_attack_support$"}', self.attack_waves_inc)

        self.assertIn(
            '{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 1}}',
            self.defense_waves_inc,
        )
        self.assertIn('{var "id_defenderbot$"}', self.defense_waves_inc)

        self.assertIn(
            '{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 1}}',
            self.enemy_defense_inc,
        )
        self.assertIn('{var "id_1st_enemy$"}', self.enemy_defense_inc)

        self.assertIn(
            '{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 1}}',
            self.enemy_attack_inc,
        )
        self.assertIn('{var "id_1st_enemy$"}', self.enemy_attack_inc)

        for name, text in (
            ("attack_waves", self.attack_waves_inc),
            ("defense_waves", self.defense_waves_inc),
            ("enemy_defense", self.enemy_defense_inc),
            ("enemy_attack", self.enemy_attack_inc),
        ):
            with self.subTest(quadrant=name):
                self.assertNotIn('{player "0"}', text)
                self.assertNotIn("control user", text)

    def test_canonical_mission_support_gate_is_100_percent(self) -> None:
        roll_start = self.attack_waves_inc.find('{"support_mission/roll"')
        self.assertGreater(roll_start, 0)
        roll_end = self.attack_waves_inc.find('{"attack_support/init"', roll_start)
        self.assertGreater(roll_end, roll_start)
        roll_block = self.attack_waves_inc[roll_start:roll_end]

        self.assertIn('{var "support_mission_roll_done$"} {op "=="} {value 0}', roll_block)
        self.assertIn('{var "support_mission_roll_done$"} {op "="} {value 1}', roll_block)
        self.assertIn('{condition {type rand} {value 1.0}}', roll_block)
        self.assertIn('{var "support_mission_enabled$"} {op "="} {value 1}', roll_block)

    def test_both_appropriate_support_systems_arm_on_human_attack(self) -> None:
        attack_init_start = self.attack_waves_inc.find('{"attack_support/init"')
        attack_init_end = self.attack_waves_inc.find('{"attack_support/clock"', attack_init_start)
        attack_init = self.attack_waves_inc[attack_init_start:attack_init_end]
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', attack_init)
        self.assertIn('{var "support_mission_enabled$"} {op "=="} {value 1}', attack_init)
        self.assertIn('{var "id_attack_support$"} {op ">"} {value 0}', attack_init)
        self.assertIn('{var "attack_support_armed$"} {op "="} {value 1}', attack_init)

        enemy_def_init_start = self.enemy_defense_inc.find('{"enemy_defense/init"')
        enemy_def_init_end = self.enemy_defense_inc.find('{"enemy_defense/trickle"', enemy_def_init_start)
        enemy_def_init = self.enemy_defense_inc[enemy_def_init_start:enemy_def_init_end]
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', enemy_def_init)
        self.assertIn('{var "support_mission_enabled$"} {op "=="} {value 1}', enemy_def_init)
        self.assertIn('{var "id_1st_enemy$"} {op ">"} {value 0}', enemy_def_init)
        self.assertIn('{var "enemy_defense_armed$"} {op "="} {value 1}', enemy_def_init)

    def test_both_appropriate_support_systems_arm_on_human_defense(self) -> None:
        def_init_start = self.defense_waves_inc.find('{"defense_support/init"')
        def_init_end = self.defense_waves_inc.find('{"defense_support/clock"', def_init_start)
        def_init = self.defense_waves_inc[def_init_start:def_init_end]
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 1}', def_init)
        self.assertIn('{var "support_mission_enabled$"} {op "=="} {value 1}', def_init)
        self.assertIn('{var "id_defenderbot$"} {op ">"} {value 0}', def_init)
        self.assertIn('{var "defense_support_armed$"} {op "="} {value 1}', def_init)

        enemy_att_init_start = self.enemy_attack_inc.find('{"enemy_attack/init"')
        enemy_att_init_end = self.enemy_attack_inc.find('{"enemy_attack/clock"', enemy_att_init_start)
        enemy_att_init = self.enemy_attack_inc[enemy_att_init_start:enemy_att_init_end]
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 1}', enemy_att_init)
        self.assertIn('{var "support_mission_enabled$"} {op "=="} {value 1}', enemy_att_init)
        self.assertIn('{var "id_1st_enemy$"} {op ">"} {value 0}', enemy_att_init)
        self.assertIn('{var "enemy_attack_armed$"} {op "="} {value 1}', enemy_att_init)


if __name__ == "__main__":
    unittest.main()
