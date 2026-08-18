from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "resource/script/multiplayer/modes/attack_support.lua"


class TmaiManualTransferParityTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.src = SCRIPT.read_text(encoding="utf-8")

    def test_manual_only_markers(self):
        self.assertIn('PREFIX = "CODEX_TMAI_MANUAL"', self.src)
        self.assertIn('"manual_transfer_only"', self.src)
        self.assertIn('"auto_spawn", "disabled"', self.src)
        self.assertIn('"auto_transfer", "disabled"', self.src)
        self.assertIn('"support_waves", "disabled"', self.src)

    def test_does_not_arm_automatic_support(self):
        self.assertNotIn('SetVar("attack_support_ready"', self.src)
        self.assertNotIn('SetVar("id_attack_support"', self.src)
        self.assertNotIn('SetVar("attack_support_use_mi"', self.src)

    def test_does_not_spawn_or_change_ownership(self):
        self.assertNotIn(':Spawn(', self.src)
        self.assertNotIn(':SpawnAt(', self.src)
        self.assertNotIn(':GameModeSpawnUnit(', self.src)
        self.assertNotIn('operation set', self.src)
        self.assertNotIn('tmai_set_mate_owner', self.src)

    def test_uses_live_mate_squads(self):
        self.assertIn('type(sc.Squads) ~= "table"', self.src)
        self.assertIn('for _, squad in pairs(sc.Squads)', self.src)
        self.assertIn('"discovered"', self.src)
        self.assertIn('"pruned"', self.src)

    def test_three_second_settle(self):
        self.assertIn('SETTLE_MS = 3000', self.src)
        self.assertIn('SetQuantTimer(function() settleEntry', self.src)
        self.assertIn('"settled"', self.src)

    def test_tmai_style_objective_policy(self):
        self.assertIn('recentlyLost', self.src)
        self.assertIn('"counterattack"', self.src)
        self.assertIn('"reinforce"', self.src)
        self.assertIn('distinct = math.min(#groups, #objectives)', self.src)

    def test_suppresses_identical_orders(self):
        self.assertIn('entry.lastRole == role and entry.lastTarget == target', self.src)

    def test_safe_existing_order_transport(self):
        self.assertIn('cmd:CaptureFlag(entry.squad, target)', self.src)
        self.assertIn('cmd:SeekAndDestroy(entry.squad)', self.src)

    def test_no_fragile_scene_query_or_utility_require(self):
        self.assertNotIn(':QueryScene(', self.src)
        self.assertNotIn('require([[/script/multiplayer/modes/utility]])', self.src)
        self.assertNotIn('require([[/script/multiplayer/logic/main]])', self.src)


if __name__ == "__main__":
    unittest.main()
