from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
PROBE = ROOT / "resource/map/multi/native_human_auto_handoff_probe.inc"
VARS = ROOT / "resource/map/multi/dcg_vars.inc"
INERT = ROOT / "resource/map/multi/support_templates_inert.inc"
ATTACK = ROOT / "resource/script/multiplayer/modes/attack_support.lua"


class NativeHumanAutoHandoffProbeTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.probe = PROBE.read_text(encoding="utf-8")
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.inert = INERT.read_text(encoding="utf-8")
        cls.attack = ATTACK.read_text(encoding="utf-8")

    def test_probe_is_loaded_by_common_campaign_include(self):
        self.assertIn('(include "/map/multi/native_human_auto_handoff_probe.inc")', self.inert)
        self.assertIn('{"native_handoff_probe_stage"}', self.vars)

    def test_source_is_one_native_user_controlled_infantry_actor(self):
        self.assertIn('{amount 1}', self.probe)
        self.assertNotIn('{tag player}', self.probe)
        self.assertIn('{prop {prop human}}', self.probe)
        self.assertIn('{state {state operatable}}', self.probe)
        self.assertIn('{state {state user_control}}', self.probe)
        self.assertIn('{state {state inactive}}', self.probe)
        self.assertIn('{state {state linked}}', self.probe)
        self.assertNotIn('{tag attack_support_tpl}', self.probe)
        self.assertNotIn('{tag attack_support_src}', self.probe)

    def test_handoff_targets_runtime_mate_id(self):
        self.assertIn('{var "id_attack_support$"}', self.probe)
        for player_id in range(1, 17):
            self.assertIn(f'{{player "{player_id}"}}', self.probe)
        self.assertIn('{operation set}', self.probe)

    def test_handoff_matches_tmai_settle_and_ai_control(self):
        self.assertIn('{control user}', self.probe)
        self.assertIn('{ai_move {mode disable}}', self.probe)
        self.assertIn('{control AI}', self.probe)
        self.assertIn('{ai_move {mode enable}}', self.probe)
        self.assertIn('{remove select}', self.probe)
        self.assertIn('{"delay" {time 3}}', self.probe)
        self.assertIn('{action move}', self.probe)

    def test_probe_does_not_spawn_or_modify_catalogs(self):
        forbidden = (
            'SpawnAt', 'BotApi.Commands:Spawn', 'GameSpawn', 'IsUnitAvailable',
            'unitset', 'research_stage', 'attack_support_tpl', 'def_sup_',
        )
        for token in forbidden:
            self.assertNotIn(token, self.probe)

    def test_probe_is_single_shot(self):
        self.assertIn('{var "native_handoff_probe_stage$"} {op "=="} {value 0}', self.probe)
        self.assertIn('{var "native_handoff_probe_stage$"} {op "="} {value 1}', self.probe)
        self.assertIn('{var "native_handoff_probe_stage$"} {op "="} {value 2}', self.probe)
        self.assertIn('{var "native_handoff_probe_stage$"} {op "="} {value 3}', self.probe)

    def test_pr110_restores_normal_ready_controller_not_stage0_probe(self):
        self.assertNotIn('CODEX_NATIVE_STAGE0', self.attack)
        self.assertIn('sc:SetVar("id_attack_support", ownerId)', self.attack)
        self.assertIn('sc:SetVar("attack_support_ready", 1)', self.attack)
        self.assertNotIn('setVar("attack_support_ready", 0)', self.attack)

    def test_runtime_mirror_exposes_handoff_gate_state(self):
        self.assertIn('"handoff_probe"', self.attack)
        self.assertIn('readVar("native_handoff_probe_stage")', self.attack)
        self.assertIn('readVar("attack_support_ready")', self.attack)
        self.assertIn('readVar("id_attack_support")', self.attack)

    def test_braces_are_balanced(self):
        self.assertEqual(self.probe.count('{'), self.probe.count('}'))
        self.assertEqual(self.probe.count('('), self.probe.count(')'))


if __name__ == "__main__":
    unittest.main()
