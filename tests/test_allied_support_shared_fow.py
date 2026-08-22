from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BOT_MAIN_LUA = ROOT / "resource/script/multiplayer/bot.main.lua"
ATTACK_SUPPORT_LUA = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
ATTACK_SUPPORT_WAVES_INC = ROOT / "resource/map/multi/attack_support_waves.inc"
NATIVE_SUPPORT_BIRTH_INC = ROOT / "resource/map/multi/native_support_birth.inc"
DEFENSE_SUPPORT_WAVES_INC = ROOT / "resource/map/multi/defense_support_waves.inc"
ENEMY_DEFENSE_SUPPORT_INC = ROOT / "resource/map/multi/enemy_defense_support.inc"
ENEMY_ATTACK_SUPPORT_INC = ROOT / "resource/map/multi/enemy_attack_support.inc"


class AlliedSupportSharedFowAndGateTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.bot_main_lua = BOT_MAIN_LUA.read_text(encoding="utf-8")
        cls.attack_support_lua = ATTACK_SUPPORT_LUA.read_text(encoding="utf-8")
        cls.attack_waves_inc = ATTACK_SUPPORT_WAVES_INC.read_text(encoding="utf-8")
        cls.native_birth_inc = NATIVE_SUPPORT_BIRTH_INC.read_text(encoding="utf-8")
        cls.defense_waves_inc = DEFENSE_SUPPORT_WAVES_INC.read_text(encoding="utf-8")
        cls.enemy_defense_inc = ENEMY_DEFENSE_SUPPORT_INC.read_text(encoding="utf-8")
        cls.enemy_attack_inc = ENEMY_ATTACK_SUPPORT_INC.read_text(encoding="utf-8")

    def test_attack_support_routes_real_mate_and_real_human_separately(self) -> None:
        # bot.main.lua remains the authority that routes attack_support.lua only onto
        # the extra non-human Team-A Mate process, never the human or DefenderBot.
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

        self.assertIn("local mateId = positiveId(id.playerId, 0)", self.attack_support_lua)
        self.assertIn("local humanId = resolveHumanId(id)", self.attack_support_lua)
        self.assertIn('setVar("id_attack_support", mateId)', self.attack_support_lua)
        self.assertIn('setVar("id_attack_support_mate", mateId)', self.attack_support_lua)
        self.assertIn('setVar("id_attack_support_human", humanId)', self.attack_support_lua)
        self.assertNotIn('setVar("id_attack_support", id.defenderBotId)', self.attack_support_lua)
        self.assertNotIn('setVar("id_attack_support", id.firstPlayerId)', self.attack_support_lua)

    def test_attack_support_arms_native_birth_and_holds_legacy_mi_wave_engine_off(self) -> None:
        publish_start = self.attack_support_lua.find("local function publishIdentity(id, isRetry)")
        publish_end = self.attack_support_lua.find("\nend\n", publish_start)
        self.assertGreater(publish_start, 0)
        self.assertGreater(publish_end, publish_start)
        publish_body = self.attack_support_lua[publish_start:publish_end]

        self.assertIn('setVar("attack_support_ready", 1)', publish_body)
        self.assertIn('setVar("attack_support_native_birth", 1)', publish_body)
        self.assertIn('setVar("attack_support_use_mi", 0)', publish_body)
        self.assertNotIn('setVar("attack_support_use_mi", 1)', self.attack_support_lua)
        self.assertIn("state.identityPublished = true", publish_body)

    def test_attack_support_retries_late_identity_and_stops_retrying_on_defense(self) -> None:
        self.assertIn("identityPublished = false", self.attack_support_lua)
        self.assertIn("attackMission = nil", self.attack_support_lua)

        publish_start = self.attack_support_lua.find("local function publishIdentity(id, isRetry)")
        publish_end = self.attack_support_lua.find("\nend\n", publish_start)
        publish_body = self.attack_support_lua[publish_start:publish_end]
        self.assertIn("if id.attacking == false then", publish_body)
        self.assertIn("state.attackMission = false", publish_body)
        self.assertIn("state.attackMission = true", publish_body)
        self.assertIn("human_or_mate_unresolved", publish_body)

        quant_start = self.attack_support_lua.find("local function onQuant()")
        quant_end = self.attack_support_lua.find("\nend\n", quant_start)
        self.assertGreater(quant_start, 0)
        self.assertGreater(quant_end, quant_start)
        quant_body = self.attack_support_lua[quant_start:quant_end]
        self.assertIn(
            "if not state.identityPublished and state.attackMission ~= false and state.quant % 10 == 0 then",
            quant_body,
        )
        self.assertIn("publishIdentity(identity(), true)", quant_body)

    def test_game_start_resets_identity_and_native_birth_state(self) -> None:
        start = self.attack_support_lua.find("local function onGameStart()")
        end = self.attack_support_lua.find("\nend\n", start)
        self.assertGreater(start, 0)
        self.assertGreater(end, start)
        body = self.attack_support_lua[start:end]
        self.assertIn("state.quant = 0", body)
        self.assertIn("state.ordered = {}", body)
        self.assertIn("state.identityPublished = false", body)
        self.assertIn("state.attackMission = nil", body)
        self.assertIn('setVar("attack_support_native_birth", 0)', body)
        self.assertIn('setVar("native_support_stage", 0)', body)
        self.assertIn("publishIdentity(id, false)", body)

    def test_native_birth_bridge_uses_proven_110_handoff_boundary(self) -> None:
        # Birth is runtime MI cloning while the source is temporarily human-owned.
        self.assertIn('{clone}', self.native_birth_inc)
        self.assertIn('("native_support_set_human_owner")', self.native_birth_inc)
        self.assertIn('{control user}', self.native_birth_inc)

        # Then it uses the exact accepted ownership/control boundary from #110.
        self.assertIn('("native_support_set_mate_owner")', self.native_birth_inc)
        self.assertIn('{control AI}', self.native_birth_inc)
        self.assertIn('{ai_move {mode enable}}', self.native_birth_inc)
        self.assertIn('{remove select}', self.native_birth_inc)
        self.assertIn('{"delay" {time 3}}', self.native_birth_inc)
        self.assertIn('{action move}', self.native_birth_inc)

        # Rejected native-bot spawn/catalog paths must not return.
        for forbidden in (
            "SpawnAt", "BotApi.Commands:Spawn", "GameSpawn", "IsUnitAvailable",
            "QueryScene", "unitset", "research_stage",
        ):
            self.assertNotIn(forbidden, self.native_birth_inc)

    def test_legacy_attack_wave_source_stays_present_but_native_birth_does_not_use_it(self) -> None:
        # The user's current live wave/composition file is intentionally left physically
        # untouched. Its legacy initializer remains source-compatible, but this diagnostic
        # never arms it because attack_support_use_mi is held at zero.
        self.assertIn('{"attack_support/init"', self.attack_waves_inc)
        self.assertIn('{var "attack_support_use_mi$"} {op "=="} {value 1}', self.attack_waves_inc)
        self.assertIn('setVar("attack_support_use_mi", 0)', self.attack_support_lua)
        self.assertNotIn('support_mission_enabled$', self.native_birth_inc)
        self.assertIn('{"set_i" {var "native_support_waves_left$"} {op "="} {value 1}}', self.native_birth_inc)

    def test_other_three_support_quadrants_keep_existing_ai_ownership(self) -> None:
        self.assertIn('{"case" {condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 1}}', self.defense_waves_inc)
        self.assertIn('{var "id_defenderbot$"}', self.defense_waves_inc)

        self.assertIn('{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 1}}', self.enemy_defense_inc)
        self.assertIn('{var "id_1st_enemy$"}', self.enemy_defense_inc)

        self.assertIn('{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 1}}', self.enemy_attack_inc)
        self.assertIn('{var "id_1st_enemy$"}', self.enemy_attack_inc)

        for name, text in (
            ("defense_waves", self.defense_waves_inc),
            ("enemy_defense", self.enemy_defense_inc),
            ("enemy_attack", self.enemy_attack_inc),
        ):
            with self.subTest(quadrant=name):
                self.assertNotIn('{player "0"}', text)
                self.assertNotIn('control user', text)

    def test_enemy_defense_support_still_arms_on_human_attack(self) -> None:
        enemy_def_init_start = self.enemy_defense_inc.find('{"enemy_defense/init"')
        self.assertGreater(enemy_def_init_start, 0)
        enemy_def_init_end = self.enemy_defense_inc.find('{"enemy_defense/trickle"', enemy_def_init_start)
        self.assertGreater(enemy_def_init_end, enemy_def_init_start)
        enemy_def_init = self.enemy_defense_inc[enemy_def_init_start:enemy_def_init_end]

        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', enemy_def_init)
        self.assertIn('{var "support_mission_enabled$"} {op "=="} {value 1}', enemy_def_init)
        self.assertIn('{var "id_1st_enemy$"} {op ">"} {value 0}', enemy_def_init)
        self.assertIn('{var "enemy_defense_armed$"} {op "="} {value 1}', enemy_def_init)

    def test_human_defense_quadrants_still_arm_normally(self) -> None:
        def_init_start = self.defense_waves_inc.find('{"defense_support/init"')
        self.assertGreater(def_init_start, 0)
        def_init_end = self.defense_waves_inc.find('{"defense_support/clock"', def_init_start)
        self.assertGreater(def_init_end, def_init_start)
        def_init = self.defense_waves_inc[def_init_start:def_init_end]

        self.assertIn('{var "user_is_defender$"} {op "=="} {value 1}', def_init)
        self.assertIn('{var "support_mission_enabled$"} {op "=="} {value 1}', def_init)
        self.assertIn('{var "id_defenderbot$"} {op ">"} {value 0}', def_init)
        self.assertIn('{var "defense_support_armed$"} {op "="} {value 1}', def_init)

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
