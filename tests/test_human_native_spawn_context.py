from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
ROUTER = ROOT / "resource/script/multiplayer/bot.main.lua"
PROBE = ROOT / "resource/script/multiplayer/modes/human_native_spawn_probe.lua"
GAME = ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"


class HumanNativeSpawnContextProbeTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.router = ROUTER.read_text(encoding="utf-8")
        cls.probe = PROBE.read_text(encoding="utf-8")
        cls.game = GAME.read_text(encoding="utf-8")

    def test_router_only_loads_probe_for_rusa_human_attack(self):
        self.assertIn('if identity.isHuman then', self.router)
        self.assertIn('identity.gameMode == "campaign_capture_the_flag"', self.router)
        self.assertIn('identity.team == "a"', self.router)
        self.assertIn('identity.army == "rusa"', self.router)
        self.assertIn('identity.attacking == true', self.router)
        self.assertIn(
            'safeRequire("resource/script/multiplayer/modes/human_native_spawn_probe")',
            self.router,
        )
        self.assertIn('routerLog("route_skip", "human_player"', self.router)

    def test_normal_attack_support_bot_routing_is_preserved(self):
        self.assertIn('if identity.isHuman then return false end', self.router)
        self.assertIn('if isAttackSupportCandidate(identity) then', self.router)
        self.assertIn(
            'safeRequire("resource/script/multiplayer/modes/attack_support")',
            self.router,
        )
        self.assertIn(
            'if identity.defenderBotId > 0 and identity.playerId == identity.defenderBotId then',
            self.router,
        )

    def test_probe_is_read_only_and_has_zero_native_spawn_calls(self):
        self.assertIn('PREFIX = "CODEX_HUMAN_NATIVE_CONTEXT"', self.probe)
        self.assertIn('commands:IsUnitAvailable(unit)', self.probe)
        self.assertIn('"native_spawn_calls", "disabled"', self.probe)
        self.assertNotIn(':SpawnAt(', self.probe)
        self.assertNotIn(':Spawn(', self.probe)
        self.assertNotIn('GameSpawn', self.probe)
        self.assertNotIn('SetVar(', self.probe)
        self.assertNotIn('QueryScene', self.probe)
        self.assertNotIn('Scene.Squads', self.probe)
        self.assertNotIn('Events', self.probe)
        self.assertNotIn('SetQuantTimer', self.probe)
        self.assertNotIn('control AI', self.probe)
        self.assertNotIn('control user', self.probe)

    def test_probe_reads_human_native_spawn_metadata(self):
        self.assertIn('instance.spawnPointName', self.probe)
        self.assertIn('conquest.PlayerSpawnPoint', self.probe)
        self.assertIn('conquest.EnemySpawnPoint', self.probe)
        self.assertIn('HUMAN_NATIVE_CATALOG_PRESENT', self.probe)
        self.assertIn('HUMAN_NATIVE_CATALOG_NOT_OBSERVED', self.probe)

    def test_probe_uses_known_rusa_candidates(self):
        self.assertIn('rus90_inf_rifle(rusa)', self.probe)
        self.assertIn('lud_22_1(rusa)', self.probe)

    def test_campaign_bot_unitset_is_untouched(self):
        self.assertIn('{aiTeamPlayers 1}', self.game)
        self.assertIn('{value "conquest"}', self.game)
        self.assertNotIn('{value "2022s"}', self.game)


if __name__ == "__main__":
    unittest.main()
