from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
GAME_SET = ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"
SCRIPT = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
MINIMAL = ROOT / "resource/set/multiplayer/units/2022s/minimal_units.set"


class MateUnitsetProvisionFowTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.game = GAME_SET.read_text(encoding="utf-8")
        cls.script = SCRIPT.read_text(encoding="utf-8")
        cls.minimal = MINIMAL.read_text(encoding="utf-8")

    def test_research_mode_is_explicit(self):
        self.assertIn("RESEARCH ONLY (#104)", self.game)
        self.assertIn('PREFIX = "CODEX_MATE_UNITSET_PROBE"', self.script)
        self.assertIn('"research_only", true', self.script)

    def test_extra_team_a_mate_remains_enabled(self):
        self.assertIn('{aiTeamPlayers 1}', self.game)

    def test_bot_state_uses_2022s_but_human_common_stays_conquest(self):
        define_end = self.game.index("\n\n{game")
        bot_state = self.game[:define_end]
        self.assertIn('{value "2022s"}', bot_state)
        self.assertNotIn('{value "conquest"}', bot_state)

        common_start = self.game.index("{common")
        common_end = self.game.index("{bots", common_start)
        common = self.game[common_start:common_end]
        self.assertIn('{value "conquest"}', common)

    def test_probe_candidates_exist_in_2022s_roster(self):
        self.assertIn('name(rus90_inf_rifle)', self.minimal)
        self.assertIn('name(lud_22_1)', self.minimal)
        self.assertIn('"rus90_inf_rifle(rusa)"', self.script)
        self.assertIn('"lud_22_1(rusa)"', self.script)

    def test_availability_is_observed(self):
        self.assertIn('local available = unitAvailability(cmd, unit)', self.script)
        self.assertIn('availableCount = availableCount + 1', self.script)
        self.assertIn('"provision_result", "PASSED"', self.script)
        self.assertIn('"provision_result", "FAILED"', self.script)

    def test_probe_logs_raw_spawn_context_without_using_it(self):
        self.assertIn('"spawnPointName", tostring(i.spawnPointName)', self.script)
        self.assertIn('"PlayerSpawnPoint", tostring(c.PlayerSpawnPoint)', self.script)
        self.assertIn('"SpawnAt_api", tostring(cmd and cmd.SpawnAt ~= nil)', self.script)
        self.assertIn('"Spawn_api", tostring(cmd and cmd.Spawn ~= nil)', self.script)
        self.assertIn('"native_spawn_calls", "disabled"', self.script)

    def test_no_native_spawn_or_game_spawn_path(self):
        self.assertNotIn(':SpawnAt(', self.script)
        self.assertNotIn(':Spawn(', self.script)
        self.assertNotIn('ev.GameSpawn', self.script)
        self.assertNotIn('native_birth_confirmed', self.script)
        self.assertNotIn('SeekAndDestroy', self.script)

    def test_no_parked_or_transfer_support_path(self):
        self.assertNotIn('SetVar("attack_support_ready"', self.script)
        self.assertNotIn('SetVar("id_attack_support"', self.script)
        self.assertNotIn('attack_support_tpl', self.script)
        self.assertNotIn('tmai_handoff', self.script)
        self.assertNotIn(':QueryScene(', self.script)


if __name__ == "__main__":
    unittest.main()
