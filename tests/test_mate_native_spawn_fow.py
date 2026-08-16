from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "resource/script/multiplayer/modes/attack_support.lua"


class MateNativeSpawnFowTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.src = SCRIPT.read_text(encoding="utf-8")

    def test_fresh_native_probe_markers(self):
        self.assertIn('PREFIX = "CODEX_MATE_NATIVE_SPAWN"', self.src)
        self.assertIn('"native_mate_spawn"', self.src)
        self.assertIn('"parked_templates", "disabled"', self.src)
        self.assertIn('"mi_support_waves", "disabled"', self.src)
        self.assertIn('"ownership_transfer", "disabled"', self.src)

    def test_uses_existing_rusa_conquest_candidates(self):
        self.assertIn('"rus90_inf_rifle(rusa)"', self.src)
        self.assertIn('"rus4_inf_rifle_rpg27(rusa)"', self.src)
        self.assertIn('"lud_22_1(rusa)"', self.src)

    def test_one_shot_attempt(self):
        self.assertIn('state.attempted then return', self.src)
        self.assertIn('state.attempted = true', self.src)

    def test_native_spawn_at_then_spawn_fallback(self):
        self.assertIn('cmd:SpawnAt(unit, MAX_SQUAD_SIZE, 0)', self.src)
        self.assertIn('cmd:Spawn(unit, MAX_SQUAD_SIZE)', self.src)
        self.assertLess(self.src.index('cmd:SpawnAt(unit, MAX_SQUAD_SIZE, 0)'), self.src.index('cmd:Spawn(unit, MAX_SQUAD_SIZE)'))

    def test_game_spawn_is_runtime_authority(self):
        self.assertIn('ev:Subscribe(ev.GameSpawn', self.src)
        self.assertIn('args.squadId', self.src)
        self.assertIn('"native_birth_confirmed", true', self.src)

    def test_logs_availability_without_gating_on_it(self):
        self.assertIn('cmd:IsUnitAvailable(unit)', self.src)
        self.assertIn('unitAvailability(cmd, unit)', self.src)
        self.assertIn('if tryNativeSpawn(unit) then', self.src)

    def test_no_old_support_architecture(self):
        self.assertNotIn('SetVar("attack_support_ready"', self.src)
        self.assertNotIn('SetVar("id_attack_support"', self.src)
        self.assertNotIn('attack_support_tpl', self.src)
        self.assertNotIn('tmai_handoff', self.src)
        self.assertNotIn(':QueryScene(', self.src)

    def test_no_fragile_utility_stack(self):
        self.assertNotIn('require([[/script/multiplayer/modes/utility]])', self.src)
        self.assertNotIn('require([[/script/multiplayer/logic/main]])', self.src)

    def test_spawned_squad_gets_one_post_spawn_order(self):
        self.assertIn('cmd:SeekAndDestroy(squad)', self.src)
        self.assertIn('ORDER_DELAY_MS = 3000', self.src)


if __name__ == "__main__":
    unittest.main()
