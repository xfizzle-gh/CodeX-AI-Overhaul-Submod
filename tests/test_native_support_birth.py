from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
ATTACK = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
BIRTH = ROOT / "resource/map/multi/native_support_birth.inc"
INERT = ROOT / "resource/map/multi/support_templates_inert.inc"
VARS = ROOT / "resource/map/multi/dcg_vars.inc"
WAVES = ROOT / "resource/map/multi/attack_support_waves.inc"


class NativeSupportBirthTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.attack = ATTACK.read_text(encoding="utf-8")
        cls.birth = BIRTH.read_text(encoding="utf-8")
        cls.inert = INERT.read_text(encoding="utf-8")
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.waves = WAVES.read_text(encoding="utf-8")

    def _define_body(self, name, next_name):
        start = self.birth.index(f'(define "{name}"')
        end = self.birth.index(f'(define "{next_name}"', start)
        return self.birth[start:end]

    def test_uses_proven_human_then_mate_identity(self):
        self.assertIn('local function resolveHumanId', self.attack)
        self.assertIn('setVar("id_attack_support_human", humanId)', self.attack)
        self.assertIn('setVar("id_attack_support_mate", mateId)', self.attack)
        self.assertIn('setVar("id_attack_support", mateId)', self.attack)

    def test_legacy_wave_engine_is_not_armed(self):
        self.assertIn('setVar("attack_support_native_birth", 1)', self.attack)
        self.assertIn('setVar("attack_support_use_mi", 0)', self.attack)
        self.assertNotIn('setVar("attack_support_use_mi", 1)', self.attack)
        self.assertIn('{"attack_support/init"', self.waves)

    def test_v4_is_single_actor_delta_probe(self):
        claim = self._define_body("native_support_claim_line", "native_support_claim_current_line")
        clone = self._define_body("native_support_clone_to_entry", "native_support_restore_source")
        self.assertIn('{amount 1}', claim)
        self.assertIn('{amount 1}', clone)
        self.assertNotIn('{amount 5}', self.birth)
        self.assertIn('ONE-ACTOR provenance probe', self.birth)

    def test_source_stays_inactive_through_clone(self):
        prepare = self._define_body("native_support_prepare_source", "native_support_clone_to_entry")
        clone = self._define_body("native_support_clone_to_entry", "native_support_restore_source")
        self.assertIn('("native_support_set_human_owner")', prepare)
        self.assertIn('{inactive on}', prepare)
        self.assertNotIn('{inactive off}', prepare)
        self.assertIn('{state {state inactive}}', clone)
        self.assertIn('{clone}', clone)

    def test_preexisting_humans_are_snapshotted_before_clone(self):
        snapshot = self._define_body("native_support_mark_preexisting", "native_support_prepare_source")
        self.assertIn('{zone {zone "gamezone"}}', snapshot)
        self.assertIn('{prop {prop human}}', snapshot)
        self.assertIn('{tag_add native_support_preexisting}', snapshot)

        start = self.birth.index('(define "native_support_issue_line"')
        end = self.birth.index('{"native_support/init"', start)
        issue = self.birth[start:end]
        self.assertLess(
            issue.index('("native_support_mark_preexisting")'),
            issue.index('("native_support_prepare_source")'),
        )
        self.assertLess(
            issue.index('("native_support_prepare_source")'),
            issue.index('("native_support_clone_to_entry")'),
        )

    def test_runtime_candidate_is_new_inactive_human_not_generic_gamezone_human(self):
        mark = self._define_body("native_support_mark_runtime_clone", "native_support_clear_snapshot")
        self.assertIn('{zone {zone "gamezone"}}', mark)
        self.assertIn('{prop {prop human}}', mark)
        self.assertIn('{state {state inactive}}', mark)
        self.assertIn('{tag {tag native_support_preexisting}}', mark)
        self.assertIn('{tag {tag native_support_live}}', mark)
        self.assertIn('{amount 1}', mark)
        self.assertIn('{tag_add native_support_new}', mark)
        self.assertIn('{inactive off}', mark)

    def test_completion_is_fail_closed_until_delta_candidate_exists(self):
        start = self.birth.index('{"native_support/claim_clone"')
        claim = self.birth[start:]
        self.assertIn('{var "native_support_stage$"} {op "=="} {value 11}', claim)
        self.assertIn('{"2.entities"', claim)
        self.assertIn('{state {state inactive}}', claim)
        self.assertIn('{tag {tag native_support_preexisting}}', claim)
        self.assertIn('("native_support_complete_handoff")', claim)

        issue_start = self.birth.index('(define "native_support_issue_line"')
        init = self.birth.index('{"native_support/init"', issue_start)
        issue = self.birth[issue_start:init]
        self.assertIn('{"set_i" {var "native_support_stage$"} {op "="} {value 11}}', issue)
        self.assertNotIn('("native_support_complete_handoff")', issue)
        self.assertNotIn('{value 12}', issue)
        self.assertNotIn('{value 3}', issue)

    def test_source_prototype_is_restored_not_consumed(self):
        restore = self._define_body("native_support_restore_source", "native_support_mark_runtime_clone")
        self.assertIn('{player "0"}', restore)
        self.assertIn('{inactive on}', restore)
        self.assertIn('{tag_remove native_support_source}', restore)
        self.assertNotIn('{"delete"', self.birth)

    def test_handoff_matches_accepted_110_boundary(self):
        handoff = self._define_body("native_support_complete_handoff", "native_support_issue_line")
        self.assertIn('("native_support_set_mate_owner")', handoff)
        self.assertIn('{control user}', handoff)
        self.assertIn('{ai_move {mode disable}}', handoff)
        self.assertIn('{control AI}', handoff)
        self.assertIn('{ai_move {mode enable}}', handoff)
        self.assertIn('{remove select}', handoff)
        self.assertIn('{"delay" {time 3}}', handoff)
        self.assertIn('{action move}', handoff)
        self.assertIn('{value 3}', handoff)

    def test_snapshot_tag_is_cleared_after_success(self):
        clear = self._define_body("native_support_clear_snapshot", "native_support_complete_handoff")
        self.assertIn('{tag native_support_preexisting}', clear)
        self.assertIn('{tag_remove native_support_preexisting}', clear)
        handoff = self._define_body("native_support_complete_handoff", "native_support_issue_line")
        self.assertIn('("native_support_clear_snapshot")', handoff)

    def test_no_bot_spawn_or_catalog_mutation(self):
        forbidden = (
            "SpawnAt", "BotApi.Commands:Spawn", "GameSpawn", "IsUnitAvailable",
            "QueryScene", "unitset", "research_stage",
        )
        for token in forbidden:
            self.assertNotIn(token, self.birth)

    def test_initial_probe_is_infantry_only(self):
        for faction in ("rusa", "ukr", "prc", "nato"):
            self.assertIn(f'("native_support_claim_line" args {faction})', self.birth)
        self.assertIn('{prop {prop human}}', self.birth)
        self.assertIn('{state {state linked}}', self.birth)
        for vehicle_token in (
            "ally_sup_rusa_ifv", "ally_sup_ukr_ifv",
            "ally_sup_prc_ifv", "ally_sup_nato_ifv",
        ):
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

    def test_first_test_is_guaranteed_single_wave(self):
        self.assertIn('{"set_i" {var "native_support_waves_left$"} {op "="} {value 1}}', self.birth)
        self.assertIn('{"delay" {time 5}}', self.birth)
        self.assertNotIn('support_mission_enabled$', self.birth)

    def test_balanced_delimiters(self):
        for text in (self.birth, self.inert, self.vars):
            self.assertEqual(text.count("{"), text.count("}"))
            self.assertEqual(text.count("("), text.count(")"))


if __name__ == "__main__":
    unittest.main()
