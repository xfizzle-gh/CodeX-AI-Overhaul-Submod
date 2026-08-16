from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
GAME_SET = ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"


class MateTransferSeedProbeTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.script = SCRIPT.read_text(encoding="utf-8")
        cls.game = GAME_SET.read_text(encoding="utf-8")

    def test_probe_is_explicit_and_mate_slot_is_unchanged(self):
        self.assertIn('PREFIX = "CODEX_MATE_SEED_PROBE"', self.script)
        self.assertIn('{aiTeamPlayers 1}', self.game)
        self.assertIn('{value "conquest"}', self.game)

    def test_zero_native_spawn_calls(self):
        self.assertNotIn(':SpawnAt(', self.script)
        self.assertNotIn(':Spawn(', self.script)
        self.assertNotIn('GameSpawn', self.script)
        self.assertIn('"native_spawn_calls", "disabled"', self.script)

    def test_no_unsafe_context_or_global_provisioning(self):
        self.assertNotIn(':QueryScene(', self.script)
        self.assertNotIn('spawnPointName', self.script)
        self.assertNotIn('PlayerSpawnPoint', self.script)
        self.assertNotIn('require(', self.script)
        self.assertNotIn('{value "2022s"}', self.game)

    def test_availability_is_measured_before_and_after_transfer(self):
        self.assertIn('state.before = safeAvailability("before_transfer")', self.script)
        self.assertIn('local after = safeAvailability("after_transfer")', self.script)
        self.assertIn('"false_to_true", flipped', self.script)
        self.assertIn('TRANSFER_SEEDED_NATIVE_CATALOG', self.script)
        self.assertIn('TRANSFER_DID_NOT_SEED_NATIVE_CATALOG', self.script)

    def test_requires_real_manually_transferred_squad(self):
        self.assertIn('source", "manual_native_transfer"', self.script)
        self.assertIn('SETTLE_MS = 3000', self.script)
        self.assertIn('type(sc.Squads) ~= "table"', self.script)
        self.assertNotIn('SetVar("id_attack_support"', self.script)
        self.assertNotIn('SetVar("attack_support_ready"', self.script)

    def test_attack_rusa_gate_is_fail_closed(self):
        self.assertIn('if c.Attacking ~= true then', self.script)
        self.assertIn('if tostring(i.team or "") ~= "a" or tostring(i.army or "") ~= "rusa" then', self.script)


if __name__ == "__main__":
    unittest.main()
