from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
ATTACK = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
BIRTH = ROOT / "resource/map/multi/native_support_birth.inc"
INERT = ROOT / "resource/map/multi/support_templates_inert.inc"
VARS = ROOT / "resource/map/multi/dcg_vars.inc"
WAVES = ROOT / "resource/map/multi/attack_support_waves.inc"
TEMPLATES = ROOT / "resource/map/multi/attack_support_templates.inc"


class NativeSupportBirthTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.attack = ATTACK.read_text(encoding="utf-8")
        cls.birth = BIRTH.read_text(encoding="utf-8")
        cls.inert = INERT.read_text(encoding="utf-8")
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.templates = TEMPLATES.read_text(encoding="utf-8")

    def test_uses_proven_human_then_mate_identity(self):
        self.assertIn('local function resolveHumanId', self.attack)
        self.assertIn('setVar("id_attack_support_human", humanId)', self.attack)
        self.assertIn('setVar("id_attack_support_mate", mateId)', self.attack)
        self.assertIn('setVar("id_attack_support", mateId)', self.attack)

    def test_legacy_parked_wave_engine_is_not_armed(self):
        self.assertIn('setVar("attack_support_native_birth", 1)', self.attack)
        self.assertIn('setVar("attack_support_use_mi", 0)', self.attack)
        self.assertNotIn('setVar("attack_support_use_mi", 1)', self.attack)
        self.assertIn('{"attack_support/init"', self.waves)

    def test_runtime_birth_is_human_owned_clone_not_bot_spawn(self):
        self.assertIn('{clone}', self.birth)
        self.assertIn('("native_support_set_human_owner")', self.birth)
        self.assertIn('{control user}', self.birth)
        self.assertIn('{tag_add native_support_source}', self.birth)
        self.assertIn('{zone {zone "gamezone"}}', self.birth)
        forbidden = (
            "SpawnAt", "BotApi.Commands:Spawn", "GameSpawn", "IsUnitAvailable",
            "QueryScene", "unitset", "research_stage",
        )
        for token in forbidden:
            self.assertNotIn(token, self.birth)

    def test_clone_detection_does_not_depend_on_inherited_source_tag(self):
        # Production already documents why it moves originals instead of trying
        # to find a clone by inherited provenance. #111 v1 ignored that warning
        # and native testing produced a false stage-3 success with no actor.
        self.assertIn("a cloned entity's\n; provenance is invisible", self.templates)
        start = self.birth.index('(define "native_support_mark_runtime_clone"')
        end = self.birth.index('\n\t\t\t)', start) + len('\n\t\t\t)')
        body = self.birth[start:end]
        self.assertIn('{zone {zone "gamezone"}}', body)
        self.assertIn('{state {state operatable}}', body)
        self.assertIn('{state {state user_control}}', body)
        self.assertIn('{prop {prop human}}', body)
        self.assertIn('{amount 5}', body)
        self.assertIn('{tag_add native_support_new}', body)
        self.assertNotIn('{tag {tag native_support_source}}', body)

    def test_diagnostic_stage_exposes_claim_clone_and_detection_boundaries(self):
        self.assertIn('{"set_i" {var "native_support_stage$"} {op "="} {value 10}}', self.birth)
        self.assertIn('{"set_i" {var "native_support_stage$"} {op "="} {value 11}}', self.birth)
        self.assertIn('{"set_i" {var "native_support_stage$"} {op "="} {value 12}}', self.birth)
        finish = self.birth.index('(define "native_support_finish_clone"')
        self.assertLess(
            self.birth.index('{"set_i" {var "native_support_stage$"} {op "="} {value 11}}'),
            self.birth.index('("native_support_clone_to_entry")'),
        )
        self.assertLess(
            self.birth.index('("native_support_clone_to_entry")'),
            self.birth.index('("native_support_mark_runtime_clone")', finish),
        )

    def test_source_prototype_is_restored_not_consumed(self):
        self.assertIn('{player "0"}', self.birth)
        self.assertIn('{inactive on}', self.birth)
        self.assertIn('{tag_remove native_support_source}', self.birth)
        self.assertNotIn('{"delete"', self.birth)

    def test_handoff_matches_accepted_110_boundary(self):
        self.assertIn('("native_support_set_mate_owner")', self.birth)
        self.assertIn('{control AI}', self.birth)
        self.assertIn('{ai_move {mode enable}}', self.birth)
        self.assertIn('{remove select}', self.birth)
        self.assertIn('{"delay" {time 3}}', self.birth)
        self.assertIn('{action move}', self.birth)

    def test_initial_acceptance_is_safe_infantry_only(self):
        for faction in ("rusa", "ukr", "prc", "nato"):
            self.assertIn(f'("native_support_claim_line" args {faction})', self.birth)
        self.assertIn('{prop {prop human}}', self.birth)
        self.assertIn('{state {state linked}}', self.birth)
        for vehicle_token in ("ally_sup_rusa_ifv", "ally_sup_ukr_ifv", "ally_sup_prc_ifv", "ally_sup_nato_ifv"):
            self.assertNotIn(vehicle_token, self.birth)

    def test_bridge_is_loaded_and_vars_declared(self):
        self.assertIn('(include "/map/multi/native_support_birth.inc")', self.inert)
        for name in (
            "id_attack_support_human", "id_attack_support_mate",
            "attack_support_native_birth", "native_support_armed",
            "native_support_stage", "native_support_wave_num",
            "native_support_waves_left", "native_support_busy",
        ):
            self.assertIn(f'{{"{name}"}}', self.vars)

    def test_first_native_test_is_guaranteed_and_single_wave(self):
        self.assertIn('{"set_i" {var "native_support_waves_left$"} {op "="} {value 1}}', self.birth)
        self.assertIn('{"delay" {time 5}}', self.birth)
        self.assertNotIn('support_mission_enabled$', self.birth)

    def test_balanced_delimiters(self):
        for text in (self.birth, self.inert, self.vars):
            self.assertEqual(text.count("{"), text.count("}"))
            self.assertEqual(text.count("("), text.count(")"))


if __name__ == "__main__":
    unittest.main()
