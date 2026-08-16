from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SUPPORT = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
HANDOFF = ROOT / "resource/map/multi/attack_support_tmai_handoff.inc"
VARS = ROOT / "resource/map/multi/dcg_vars.inc"
INERT = ROOT / "resource/map/multi/support_templates_inert.inc"
TEMPLATES = ROOT / "resource/map/multi/attack_support_templates.inc"
WAVES = ROOT / "resource/map/multi/attack_support_waves.inc"
PURCHASE_ROOT = ROOT / "resource/script/multiplayer/units"


class TmaiHumanOriginHandoffTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.support = SUPPORT.read_text(encoding="utf-8")
        cls.handoff = HANDOFF.read_text(encoding="utf-8")
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.inert = INERT.read_text(encoding="utf-8")
        cls.templates = TEMPLATES.read_text(encoding="utf-8")
        cls.waves = WAVES.read_text(encoding="utf-8")

    def test_mission_bus_declares_every_handoff_authority(self) -> None:
        for name in (
            "id_attack_support_human",
            "id_attack_support_mate",
            "tmai_handoff_prepare",
            "tmai_handoff_prepared",
            "tmai_handoff_enabled",
            "tmai_handoff_busy",
            "tmai_handoff_seq",
        ):
            with self.subTest(name=name):
                self.assertIn('{"' + name + '"}', self.vars)

    def test_bridge_is_loaded_in_shared_trigger_scope_before_wave_runtime(self) -> None:
        self.assertIn('(include "/map/multi/attack_support_tmai_handoff.inc")', self.inert)
        self.assertIn('{"support_templates/inert"', self.inert)
        self.assertIn('{"support_templates/cull"', self.inert)
        self.assertIn('{"attack_support/tmai_human_seed"', self.handoff)
        self.assertIn('{"attack_support/tmai_handoff"', self.handoff)

    def test_extra_support_pool_remains_separate_from_conquest_purchase_rosters(self) -> None:
        self.assertIn("attack_support_tpl", self.templates)
        self.assertIn("attack_support_inf_usmc", self.templates)
        self.assertIn("attack_support_inf_1ad", self.templates)
        self.assertIn("attack_support_inf_arf", self.templates)
        self.assertIn("attack_support_inf_pzgd", self.templates)
        offenders = []
        for path in PURCHASE_ROOT.rglob("*.lua"):
            text = path.read_text(encoding="utf-8")
            if "attack_support_tpl" in text or "tmai_handoff" in text:
                offenders.append(str(path.relative_to(ROOT)))
        self.assertEqual(offenders, [])

    def test_legacy_templates_are_not_rewritten_to_a_guessed_static_human_id(self) -> None:
        self.assertIn('{Player 0}', self.templates)
        self.assertNotIn('{Player 1}', self.templates)
        self.assertNotIn('{Player 2}', self.templates)
        self.assertIn('("tmai_set_human_owner" args attack_support_tpl)', self.handoff)

    def test_existing_wave_first_owner_switch_is_reused_with_human_id(self) -> None:
        self.assertIn('(define "am_own_to_support"', self.waves)
        self.assertIn('{var "id_attack_support$"}', self.waves)
        self.assertIn('setVar("id_attack_support", humanId)', self.support)
        self.assertNotIn('setVar("id_attack_support", mateId)', self.support)

    def test_pool_seed_is_a_hard_precondition_for_wave_readiness(self) -> None:
        arm = self.support[self.support.index("local function armHumanOriginHandoff") : self.support.index("local function mirrorState")]
        prepare = arm.index('setVar("tmai_handoff_prepare", 1)')
        gate = arm.index('readVarNumber("tmai_handoff_prepared") ~= 1')
        enabled = arm.index('setVar("tmai_handoff_enabled", 1)')
        ready = arm.index('setVar("attack_support_ready", 1)')
        self.assertLess(prepare, gate)
        self.assertLess(gate, enabled)
        self.assertLess(enabled, ready)

    def test_handoff_waits_until_legacy_wave_finalizer_releases_deploy_tag(self) -> None:
        selection_start = self.handoff.index('{select {tag {tag attack_support_src}}}')
        pending = self.handoff.index('{tag_add tmai_handoff_pending}', selection_start)
        selection = self.handoff[selection_start:pending]
        self.assertIn('{exclude {tag {tag attack_support_deploy}}}', selection)
        self.assertIn('{tag_remove attack_support_deploy}', self.waves)

    def test_deployed_units_get_manual_transfer_equivalent_state_sequence(self) -> None:
        pending = self.handoff.index('{tag_add tmai_handoff_pending}')
        user = self.handoff.index('{control user}', pending)
        first_three = self.handoff.index('{"delay" {time 3}}', user)
        mate = self.handoff.index('("tmai_set_mate_owner" args tmai_handoff_pending)', first_three)
        ai = self.handoff.index('{control AI}', mate)
        second_three = self.handoff.index('{"delay" {time 3}}', ai)
        move = self.handoff.index('{action move}', second_three)
        self.assertLess(pending, user)
        self.assertLess(user, first_three)
        self.assertLess(first_three, mate)
        self.assertLess(mate, ai)
        self.assertLess(ai, second_three)
        self.assertLess(second_three, move)

    def test_handoff_source_is_structurally_balanced(self) -> None:
        self.assertEqual(self.handoff.count("{"), self.handoff.count("}"))
        self.assertEqual(self.handoff.count("("), self.handoff.count(")"))

    def test_handoff_does_not_modify_enemy_support_ownership(self) -> None:
        self.assertNotIn("id_1st_enemy", self.handoff)
        self.assertNotIn("enemy_defense", self.handoff)
        self.assertNotIn("enemy_attack", self.handoff)
        self.assertNotIn("id_defenderbot", self.handoff)

    def test_runtime_logging_exposes_resolved_ids_and_completed_handoffs(self) -> None:
        self.assertIn('local HANDOFF_PREFIX = "CODEX_TMAI_HANDOFF"', self.support)
        self.assertIn('handoffLog("armed"', self.support)
        self.assertIn('"human", humanId', self.support)
        self.assertIn('"mate", mateId', self.support)
        self.assertIn('handoffLog("completed"', self.support)
        self.assertIn('"order_transport", "MI_action_move"', self.support)

    def test_human_resolution_uses_proven_four_slot_complement_and_fails_closed(self) -> None:
        resolver = self.support[self.support.index("local function resolveHumanId") : self.support.index("local function logWait")]
        self.assertIn("for playerId = 1, 4 do", resolver)
        self.assertIn('return humanId, "campaign_four_slot_complement"', resolver)
        self.assertIn('return 0, "four_slot_" .. item.name .. "_out_of_range="', resolver)
        self.assertIn('return 0, "four_slot_duplicate_id="', resolver)
        self.assertIn('return 0, "four_slot_no_candidate"', resolver)
        self.assertNotIn(":QueryScene(", self.support)
        self.assertIn('logWait("human_id_unresolved:"', self.support)
        self.assertIn('if humanId <= 0 then', self.support)
        self.assertIn('if humanId == mateId then', self.support)


if __name__ == "__main__":
    unittest.main()
