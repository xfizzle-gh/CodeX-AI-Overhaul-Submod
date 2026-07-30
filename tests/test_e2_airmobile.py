import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TEMPLATES = ROOT / "resource/map/multi/faction_support_templates.inc"
VARS = ROOT / "resource/map/multi/dcg_vars.inc"
WAVES = ROOT / "resource/map/multi/attack_support_waves.inc"
LUA = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
POT = ROOT / "localizations/default/interface/text/mission/multi/support_events.pot"
DEPLOY = ROOT / "tools/deploy_attack_support_probe.ps1"
CE_MAP = ROOT / "resource/map/multi/ce/ai_logic/ce_ai_logic_triggers.inc"
CE_SCRIPT = ROOT / "resource/map_scripts/ai_logic/ce_ai_logic_triggers.inc"
PLAN = ROOT / "docs/superpowers/plans/2026-07-30-e2-airmobile-flight-paradrop.md"


def block(text: str, start: str, end: str) -> str:
    return text.split(start, 1)[1].split(end, 1)[0]


def mi_block(text: str, marker: str) -> str:
    """Return the balanced MI {...} form beginning at marker."""
    start = text.index(marker)
    open_at = text.index("{", start)
    depth = 0
    for pos in range(open_at, len(text)):
        if text[pos] == "{":
            depth += 1
        elif text[pos] == "}":
            depth -= 1
            if depth == 0:
                return text[open_at : pos + 1]
    raise AssertionError(f"unbalanced MI block: {marker}")


def mi_define(text: str, name: str) -> str:
    """Return the balanced (define ...) form, including nested calls."""
    marker = f'(define "{name}"'
    start = text.index(marker)
    depth = 0
    for pos in range(start, len(text)):
        if text[pos] == "(":
            depth += 1
        elif text[pos] == ")":
            depth -= 1
            if depth == 0:
                return text[start : pos + 1]
    raise AssertionError(f"unbalanced MI define: {name}")


class E2PoolAndStateTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.tpl = TEMPLATES.read_text(encoding="utf-8")
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.lua = LUA.read_text(encoding="utf-8")
        cls.pot = POT.read_text(encoding="utf-8")
        cls.deploy = DEPLOY.read_text(encoding="utf-8")

    def test_exact_e2_id_mid_and_parking_bands(self) -> None:
        expected_ids = [f"0x{n:x}" for n in range(0xB401, 0xB430)]
        expected_mids = list(range(9800, 9847))
        e2 = block(self.tpl, "; ===== E2 AIR PACKAGE POOLS", "; ===== TAGS =====")
        ids = re.findall(r'\{(?:Entity|Human) "[^"]+" (0xb4[0-9a-f]{2})', e2)
        mids = [int(v) for v in re.findall(r"\{MID (98\d\d)\}", e2)]
        self.assertEqual(ids, expected_ids)
        self.assertEqual(mids, expected_mids)
        for entity_id in expected_ids:
            self.assertRegex(self.tpl, rf'\{{Tags "ally_sup_tpl" "support_e2_tpl"[^\n]* {entity_id}\}}')
        self.assertEqual(self.tpl.count(" -36800}"), 47)
        self.assertNotIn("0xb400", self.tpl)

    def test_exact_aircraft_and_crews(self) -> None:
        for asset in ("mi17_b8_rus", "mi17_b8_ukr", "uh-60m_blackhawk_mg", "il-76td_para", "c130_para"):
            self.assertIn(f'{{Entity "{asset}"', self.tpl)
        for breed in ("mp/rusa/2022s/rus_pliot", "mp/ukr/2022s/ukr_pilot", "mp/nato/2022s/nato_pilot"):
            self.assertIn(f'{{Human "{breed}"', self.tpl)
        self.assertEqual(self.tpl.count('{Chassis "helicopter"\n\t\t\t{Airborne}\n\t\t\t{EngineStarted}\n\t\t\t{Altitude 22}\n\t\t}'), 3)

    def test_payloads_and_ejectable_links_are_pinned(self) -> None:
        for breed in ("106vdv_squadlead", "106vdv_mg", "106vdv_rifleman", "ukr13_squadlead", "ukr13_lmg", "ukr13_rifleman", "82nd_squadlead", "82nd_mg", "82nd_rifleman"):
            self.assertIn(breed, self.tpl)
        for place in ("seat01", "seat02", "seat03", "seat04", "seat03", "seat04", "seat05"):
            self.assertIn(f'"{place}"', self.tpl)
        self.assertNotIn('"seat00"', self.tpl)
        for n in range(21, 49):
            self.assertNotRegex(self.tpl, rf'"seat0?{n}"')
        self.assertEqual(self.tpl.count('"support_e2_para_pax"'), 12)

    def test_default_off_integer_state_and_lua_mirror(self) -> None:
        for name in ("support_e2_test", "support_e2_stage", "support_e2_fail", "support_e2_lz", "support_e2_flag"):
            self.assertIn(f'{{"{name}"}}', self.vars)
            self.assertIn(f'{{var "{name}$"}} {{op "="}} {{value 0}}', self.waves)
            self.assertIn(f'readVar("{name}")', self.lua)
        init = block(self.waves, '{"attack_support/init"', '{"attack_support/clock"')
        self.assertNotIn('{var "support_e2_test$"} {op "="} {value 1}', init)
        self.assertNotIn('{var "support_e2_test$"} {op "="} {value 2}', init)

    def test_dependency_localization_and_deploy_guards(self) -> None:
        self.assertIn("West-81", self.tpl)
        for key in ("e2_helo_inbound", "e2_para_inbound", "e2_insert_failed"):
            self.assertIn(f'msgctxt "mission/multi/support/{key}"', self.pot)
        for marker in ("must park 626 prototypes", "support_e2_test", "support_e2_para_pax", "ce_ai_logic_triggers.inc"):
            self.assertIn(marker, self.deploy)

class E2CeIsolationTests(unittest.TestCase):
    def test_ce_mirrors_are_byte_identical(self) -> None:
        self.assertEqual(CE_MAP.read_bytes(), CE_SCRIPT.read_bytes())

    def test_every_paratrooper_order_selector_excludes_e2(self) -> None:
        text = CE_MAP.read_text(encoding="utf-8")
        order_block = block(text, '{"ai_logic/paratrooper_orders"', '{"ai_logic/')
        selectors = []
        cursor = 0
        while True:
            start = order_block.find('{selector', cursor)
            if start < 0:
                break
            selector = mi_block(order_block[start:], '{selector')
            if '{tag paratrooper_need_orders}' in selector:
                selectors.append(selector)
            cursor = start + len(selector)

        self.assertEqual(len(selectors), 3)
        for selector in selectors:
            exclude = mi_block(selector, '{exclude')
            self.assertIn('{tag paratrooper_need_orders}', selector)
            self.assertRegex(exclude, r"\{tag\s+\{tag support_e2_para_pax\}")
            self.assertEqual(selector.count("support_e2_para_pax"), 1)
        self.assertEqual(order_block.count("support_e2_para_pax"), 3)


class E2HelicopterLifecycleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")

    def test_dispatch_is_strictly_default_off_and_budget_neutral(self) -> None:
        self.assertIn("; ===== E2 REAL AIR INSERT PROBES =====", self.waves)
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== MOTORIZED INSERT")
        self.assertIn('{var "support_e2_test$"} {op "=="} {value 1}', e2)
        self.assertIn('{var "support_e2_test$"} {op "=="} {value 2}', e2)
        for gate in (
            '{var "user_is_defender$"} {op "=="} {value 0}',
            '{var "attack_support_ready$"} {op "=="} {value 1}',
            '{var "attack_support_use_mi$"} {op "=="} {value 1}',
            '{var "id_attack_support$"} {op ">"} {value 0}',
            '{var "support_e2_stage$"} {op "=="} {value 0}',
            '{var "support_e2_test$"} {op ">"} {value 0}',
        ):
            self.assertIn(gate, e2)
        for budget in (
            "attack_support_waves_left$",
            "attack_support_air_left$",
            "attack_support_motor_left$",
            "attack_support_ifv_left$",
        ):
            self.assertNotIn(f'{{var "{budget}"}} {{op "-"}}', e2)
        self.assertNotIn('{var "attack_support_wave_cmd$"} {op "="}', e2)

    def test_helicopter_uses_attested_flight_sequence_and_existing_pads(self) -> None:
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== E2 PARADROP")
        self.assertEqual(e2.count('{"air_state"'), 3)
        self.assertEqual(e2.count('{altitude 30}'), 3)
        self.assertEqual(e2.count('{drop sensor}'), 3)
        self.assertGreaterEqual(e2.count('{control AI}'), 3)
        self.assertGreaterEqual(e2.count('{action move}'), 6)
        for side in "ab":
            self.assertIn(f'{{waypoint "attack_support_entry_{side}1"}}', e2)
            for n in (1, 2):
                self.assertIn(f'{{waypoint "attack_support_air_{side}{n}"}}', e2)
        self.assertNotRegex(e2, r"support_e2_lz_fpc|e2_lz_fpc")
        self.assertNotIn("{clone}", e2)

    def test_helicopter_places_four_independent_troops_at_half_second_cadence(self) -> None:
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== E2 PARADROP")
        self.assertIn('(define "e2_place_one"', e2)
        self.assertIn('{"delay" {time 0.5}}', e2)
        self.assertGreaterEqual(e2.count('("e2_place_one")'), 12)
        self.assertIn('{action advance}', e2)
        self.assertIn('{tag support_e2_flag_target}', e2)
        self.assertIn('{amount 1}', e2)

    def test_helicopter_has_fail_closed_faction_and_bounded_delete(self) -> None:
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== E2 PARADROP")
        for faction in ("rusa", "ukr", "nato"):
            self.assertIn(f'{{"attack_support/e2_helo_{faction}"', e2)
        self.assertNotIn("attack_support/e2_helo_prc", e2)
        self.assertIn('{value 1}', e2)
        self.assertIn('(define "e2_delete_aircraft"', e2)
        self.assertIn('{"delete"', e2)
        self.assertRegex(e2, r'\{"delay" \{time (?:45|60|75|90)\}\}')

    def test_ownership_switch_lists_1_through_16_and_default_has_no_player(self) -> None:
        e2 = block(self.waves, '(define "e2_own_current"', '(define "e2_place_one"')
        for player in range(1, 17):
            self.assertIn(f'{{player "{player}"}}', e2)
        default = e2.split('{"default"', 1)[1]
        self.assertNotIn('{player "', default)

    def test_cleanup_targets_only_the_claimed_aircraft(self) -> None:
        delete = block(self.waves, '(define "e2_delete_aircraft"', '(define "e2_fail_and_cleanup"')
        self.assertIn('{tag support_e2_aircraft}', delete)
        self.assertIn('{tag support_e2_claim}', delete)
        self.assertNotIn('{selector {ignore_captured_by_user 0} {tag support_e2_aircraft}}', delete)

    def test_supported_pools_are_exact_and_task4_is_not_implemented_here(self) -> None:
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== E2 PARADROP")
        for faction in ("rusa", "ukr", "nato"):
            self.assertIn(f'support_e2_{faction}_helo', e2)
            self.assertIn(f'support_e2_{faction}_helo_crew', e2)
            self.assertIn(f'support_e2_{faction}_helo_team', e2)
        self.assertNotIn('{effect drop_paratrooper}', e2)
        self.assertNotIn('{effect drop_paratroopers}', e2)

    def test_dispatch_reserves_stage_before_async_child_and_pool_short_is_atomic(self) -> None:
        dispatch = mi_block(self.waves, '{"attack_support/e2_dispatch"')
        reserve = '{"set_i" {var "support_e2_stage$"} {op "="} {value 10}}'
        first_child = '("e2_trigger_helo_by_army")'
        self.assertLess(dispatch.index(reserve), dispatch.index(first_child))
        timeout = dispatch.split('{"delay" {time 1}}', 1)[1]
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 10}', timeout)
        self.assertIn('{var "support_e2_fail$"} {op "="} {value 2}', timeout)
        for faction in ("rusa", "ukr", "nato"):
            helo = mi_block(self.waves, f'{{"attack_support/e2_helo_{faction}"')
            condition = helo.split('{actions', 1)[0]
            self.assertIn('{var "support_e2_stage$"} {op "=="} {value 10}', condition)
            self.assertNotIn('{var "support_e2_stage$"} {op "=="} {value 0}', condition)
            selected = helo.index('("e2_choose_flag")')
            accepted = helo.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 20}}')
            flight = helo.index('{"air_state"')
            self.assertLess(selected, accepted)
            self.assertLess(accepted, flight)

    def test_pad_safety_uses_one_claimed_marker_and_two_independent_near_queries(self) -> None:
        claim = mi_define(self.waves, "e2_claim_lz_marker")
        self.assertIn('{tag support_e2_team}', claim)
        self.assertIn('{tag_add support_e2_lz_marker}', claim)
        self.assertIn('{amount 1}', claim)
        self.assertNotIn('{tag_remove support_e2_team}', claim)

        place = mi_define(self.waves, "e2_place_lz_marker")
        for side in "ab":
            for pad in (1, 2):
                self.assertIn(
                    f'{{target_waypoint "attack_support_air_{side}{pad}"}}', place
                )
        self.assertNotIn('attack_support_entry_', place)

        choose = mi_define(self.waves, "e2_choose_lz")
        self.assertEqual(choose.count('("e2_claim_lz_marker")'), 1)
        self.assertEqual(choose.count('("e2_place_lz_marker")'), 2)
        self.assertEqual(choose.count('{type near}'), 2)
        self.assertEqual(choose.count('{distance 120}'), 2)
        self.assertEqual(choose.count('{tag support_e2_lz_marker}'), 2)
        self.assertGreaterEqual(choose.count('{tag _bot}'), 2)
        self.assertEqual(choose.count('{type human}'), 2)
        self.assertEqual(choose.count('{state operatable}'), 2)
        self.assertNotIn('{type entities}', choose)
        self.assertNotIn('{source advanced}', choose)

    def test_arrival_is_pad_anchored_before_any_troop_promotion(self) -> None:
        for faction in ("rusa", "ukr", "nato"):
            helo = mi_block(self.waves, f'{{"attack_support/e2_helo_{faction}"')
            arrival = helo.split('{"delay" {time 40}}', 1)[1]
            near = arrival.index('{type near}')
            announce = arrival.index('("e2_announce_helo")')
            place = arrival.index('("e2_place_one")')
            self.assertLess(near, announce)
            self.assertLess(near, place)
            self.assertIn('{units', arrival[:announce])
            self.assertIn('{tag support_e2_helo}', arrival[:announce])
            self.assertIn('{state operatable}', arrival[:announce])
            self.assertIn('{near_to', arrival[:announce])
            self.assertIn('{tag support_e2_lz_marker}', arrival[:announce])
            self.assertIn('{distance 120}', arrival[:announce])
            self.assertNotIn('target_waypoint', arrival[:announce])
            fail5 = arrival.index('{var "support_e2_fail$"} {op "="} {value 5}')
            failure_default = arrival.rfind('{"default"', 0, fail5)
            self.assertGreaterEqual(failure_default, 0)
            failed = mi_block(arrival[failure_default:], '{"default"')
            self.assertIn('("e2_order_aircraft_exit")', failed)
            self.assertEqual(failed.count('("e2_fail_and_cleanup")'), 1)
            self.assertNotIn('("e2_place_one")', failed)
            self.assertNotIn('("e2_place_one_entry")', failed)

    def test_fail4_uses_numbered_entry_helper_and_each_abort_cleans_once(self) -> None:
        choose_flag = mi_define(self.waves, "e2_choose_flag")
        self.assertIn('{var "support_e2_fail$"} {op "="} {value 3}', choose_flag)
        self.assertNotIn('e2_fail_and_cleanup', choose_flag)
        self.assertNotIn('e2_complete_cleanup', choose_flag)

        entry = mi_define(self.waves, "e2_place_one_entry")
        self.assertIn('{target_waypoint "attack_support_entry_a1"}', entry)
        self.assertIn('{target_waypoint "attack_support_entry_b1"}', entry)
        self.assertNotIn('{tag spawn_a}', entry)
        self.assertNotIn('{tag spawn_b}', entry)
        self.assertIn('{"delay" {time 0.5}}', entry)

        for faction in ("rusa", "ukr", "nato"):
            helo = mi_block(self.waves, f'{{"attack_support/e2_helo_{faction}"')
            fail4_at = helo.index('{var "support_e2_fail$"} {op "=="} {value 4}')
            fail4 = helo[fail4_at : helo.index('{"default"', fail4_at)]
            self.assertEqual(fail4.count('("e2_place_one_entry")'), 4)
            self.assertEqual(fail4.count('("e2_complete_cleanup")'), 1)
            self.assertNotIn('("e2_delete_aircraft")', fail4)
            self.assertNotIn('("e2_fail_and_cleanup")', fail4)

    def test_deployer_pins_task3_source_and_workshop_contracts(self) -> None:
        deploy = DEPLOY.read_text(encoding="utf-8")
        for array in (
            "$E2HeloWaveMarkers",
            "$E2HeloTemplateMarkers",
            "$E2HeloForbiddenMarkers",
        ):
            self.assertIn(array, deploy)
        for marker in (
            "; ===== E2 REAL AIR INSERT PROBES =====",
            '{"attack_support/e2_dispatch"',
            '{"attack_support/e2_helo_rusa"',
            '{"attack_support/e2_helo_ukr"',
            '{"attack_support/e2_helo_nato"',
            '{"air_state"',
            'support_e2_lz',
            '{"delete"',
            '{Altitude 22}',
        ):
            self.assertIn(marker, deploy)
        for marker in ('attack_support/e2_helo_prc', '{clone}', 'support_e2_lz_fpc'):
            self.assertIn(marker, deploy)
        for side in ("Source", "Workshop"):
            self.assertIn(f'{side} wave engine is missing E2 helicopter marker', deploy)
            self.assertIn(f'{side} E2 helicopter template is missing marker', deploy)
            self.assertIn(f'{side} wave engine contains forbidden E2 helicopter marker', deploy)

    def test_plan_records_portable_active_target_sentinel(self) -> None:
        plan = PLAN.read_text(encoding="utf-8")
        self.assertIn(
            '`support_e2_flag$`: `0=none`, `1=one portable active target selected`',
            plan,
        )
        self.assertNotIn('`1`-`5` for selected `fpc1`-`fpc5`', plan)
        task3 = block(plan, "### Task 3:", "### Task 4:")
        self.assertIn("set support_e2_flag$ to 1 as the portable active-target sentinel", task3)
        self.assertNotIn("testing the selected entity against fpc1..fpc5", task3)


class E2SequentialComboTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.lua = LUA.read_text(encoding="utf-8")
        cls.deploy = DEPLOY.read_text(encoding="utf-8")
        cls.e2 = block(cls.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== MOTORIZED INSERT")

    def test_deploy_mode_parameter_is_validated_and_defaults_off(self) -> None:
        self.assertRegex(self.deploy, r"\[ValidateSet\(0,\s*1,\s*2,\s*3\)\]\s*\[int\]\$E2TestMode\s*=\s*0")

    def test_deploy_override_is_exact_target_only_and_validated(self) -> None:
        self.assertIn("Set-ExactSingleReplacement", self.deploy)
        self.assertIn("$deployedWaveCode", self.deploy)
        self.assertIn("[System.IO.File]::WriteAllText($deployedWaves", self.deploy)
        self.assertIn("Requested E2 test mode was not written exactly once", self.deploy)
        self.assertIn("Legacy E1 air test value is incorrect", self.deploy)
        self.assertIn("if ($E2TestMode -ne 0)", self.deploy)
        self.assertNotIn("WriteAllText($wavesSource", self.deploy)

    def test_combo_result_is_declared_initialized_and_mirrored(self) -> None:
        name = "support_e2_combo_helo_fail"
        self.assertIn(f'{{"{name}"}}', self.vars)
        self.assertIn(f'{{var "{name}$"}} {{op "="}} {{value 0}}', self.waves)
        self.assertIn(f'readVar("{name}")', self.lua)
        init = block(self.waves, '{"attack_support/init"', '{"attack_support/clock"')
        self.assertIn('{var "support_e2_test$"} {op "="} {value 0}', init)

    def test_mode_three_enters_only_the_helicopter_dispatch_first(self) -> None:
        dispatch = mi_block(self.e2, '{"attack_support/e2_dispatch"')
        mode3 = dispatch.split('{var "support_e2_test$"} {op "=="} {value 3}', 1)[1]
        mode3 = mode3.split('{var "support_e2_test$"} {op "=="} {value 2}', 1)[0]
        self.assertIn('("e2_trigger_helo_by_army")', mode3)
        self.assertNotIn('("e2_trigger_para_by_army")', mode3)

    def test_helicopter_children_accept_exactly_modes_one_and_three(self) -> None:
        for faction in ("rusa", "ukr", "nato"):
            child = mi_block(self.e2, f'{{"attack_support/e2_helo_{faction}"')
            condition = child.split("{actions", 1)[0]
            self.assertIn('{var "support_e2_test$"} {op "=="} {value 1}', condition)
            self.assertIn('{var "support_e2_test$"} {op "=="} {value 3}', condition)
            self.assertNotIn('{var "support_e2_test$"} {op ">"}', condition)

    def test_combo_transition_is_claim_free_and_ordered(self) -> None:
        transition = mi_block(self.e2, '{"attack_support/e2_combo_transition"')
        condition, actions = transition.split("{actions", 1)
        self.assertIn('{var "support_e2_test$"} {op "=="} {value 3}', condition)
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 70}', condition)
        self.assertIn('{tag support_e2_claim}', condition)
        self.assertIn("!3", condition)
        copy_at = actions.index('{var "support_e2_combo_helo_fail$"} {op "="} {var "support_e2_fail$"}')
        clear_at = actions.index('{var "support_e2_fail$"} {op "="} {value 0}')
        mode2_at = actions.index('{var "support_e2_test$"} {op "="} {value 2}')
        stage0_at = actions.index('{var "support_e2_stage$"} {op "="} {value 0}')
        self.assertLess(copy_at, clear_at)
        self.assertLess(clear_at, mode2_at)
        self.assertLess(mode2_at, stage0_at)

    def test_combo_is_budget_neutral_and_mode_two_cannot_retrigger(self) -> None:
        transition = mi_block(self.e2, '{"attack_support/e2_combo_transition"')
        for budget in ("attack_support_waves_left$", "attack_support_air_left$", "attack_support_motor_left$", "attack_support_ifv_left$"):
            self.assertNotIn(budget, transition)
        self.assertNotIn('{var "support_e2_test$"} {op "=="} {value 2}', transition.split("{actions", 1)[0])


class E2ParadropLifecycleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.e2 = block(cls.waves, "; ===== E2 PARADROP", "; ===== MOTORIZED INSERT")

    def test_supported_launches_reserve_stage_and_accept_target_before_flight(self) -> None:
        for faction in ("rusa", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_para_{faction}"')
            condition = launch.split("{actions", 1)[0]
            self.assertIn('{var "support_e2_test$"} {op "=="} {value 2}', condition)
            self.assertIn('{var "support_e2_stage$"} {op "=="} {value 10}', condition)
            self.assertNotIn('{var "support_e2_stage$"} {op "=="} {value 0}', condition)
            selected = launch.index('("e2_choose_flag")')
            accepted = launch.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 20}}')
            owned = launch.index('("e2_own_current")')
            flight = launch.index('{"air_state"')
            self.assertLess(selected, accepted)
            self.assertLess(accepted, owned)
            self.assertLess(owned, flight)
        self.assertNotIn('attack_support/e2_para_prc', self.e2)

    def test_claims_exact_plane_crew_and_payload_pools(self) -> None:
        crew_counts = {"rusa": 5, "ukr": 3, "nato": 3}
        for faction, crew_count in crew_counts.items():
            launch = mi_block(self.waves, f'{{"attack_support/e2_para_{faction}"')
            for pool in (
                f"support_e2_{faction}_para",
                f"support_e2_{faction}_para_crew",
                f"support_e2_{faction}_para_pax",
            ):
                self.assertIn(f'{{tag {pool}}}', launch)
                self.assertIn(f'{{tag_remove {pool}}}', launch)
            self.assertIn(f'{{amount {crew_count}}}', launch)
            self.assertIn('{amount 4}', launch)
            self.assertGreaterEqual(launch.count('{tag_add support_e2_claim}'), 3)
            self.assertIn('{tag_add support_e2_plane}', launch)
            self.assertNotIn('{tag_remove support_e2_para_pax}', launch)

    def test_launch_flight_order_is_attested_and_targets_selected_flag(self) -> None:
        for faction in ("rusa", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_para_{faction}"')
            air = launch.index('{"air_state"')
            actor = launch.index('{"actor_state"', air)
            move = launch.index('{"action"', actor)
            stage30 = launch.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 30}}')
            self.assertLess(air, actor)
            self.assertLess(actor, move)
            self.assertLess(move, stage30)
            self.assertIn('{altitude 65}', launch[air:actor])
            self.assertIn('{drop sensor}', launch[actor:move])
            self.assertIn('{control AI}', launch[actor:move])
            self.assertIn('{movement {speed fast}}', launch[actor:move])
            self.assertIn('{action move}', launch[move:stage30])
            self.assertIn('{tag support_e2_flag_target}', launch[move:stage30])
            self.assertIn('("e2_place_aircraft_entry")', launch[:air])

    def test_release_is_target_anchored_banded_and_one_shot(self) -> None:
        for faction in ("rusa", "ukr", "nato"):
            release = mi_block(self.waves, f'{{"attack_support/e2_para_release_{faction}"')
            condition = release.split("{actions", 1)[0]
            self.assertIn('{var "support_e2_test$"} {op "=="} {value 2}', condition)
            self.assertIn('{var "support_e2_stage$"} {op "=="} {value 30}', condition)
            self.assertIn('{tag support_e2_plane}', condition)
            self.assertIn('{state operatable}', condition)
            self.assertIn('{near_to', condition)
            self.assertIn('{tag support_e2_flag_target}', condition)
            self.assertEqual(condition.count('{distance 2500}'), 1)
            self.assertEqual(condition.count('{distance 1500}'), 1)
            self.assertRegex(condition, r'\{expression "[^"]*!\d+[^"]*"\}')
            self.assertIn('{tag support_e2_released}', condition)
            actions = release.split("{actions", 1)[1]
            tagged = actions.index('{tag_add support_e2_released}')
            effect = actions.index('{effect drop_paratrooper}')
            stage40 = actions.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}')
            pax_route = actions[tagged:effect]
            self.assertIn(
                '{selector {ignore_captured_by_user 0} {tag support_e2_para_pax} '
                '{tag support_e2_claim} {type human} {state operatable}}',
                pax_route,
            )
            self.assertIn('{tag_add paratrooper}', pax_route)
            self.assertIn('{tag_add ignore_spawn_logic}', pax_route)
            self.assertLess(tagged, effect)
            self.assertLess(effect, stage40)
        self.assertEqual(self.e2.count('{effect drop_paratrooper}'), 3)
        self.assertNotIn('{effect drop_paratroopers}', self.e2)

    def test_missed_release_is_fail6_and_cannot_place_passengers(self) -> None:
        for faction in ("rusa", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_para_{faction}"')
            timeout = launch.split('{"set_i" {var "support_e2_stage$"} {op "="} {value 30}}', 1)[1]
            self.assertRegex(timeout, r'\{"delay" \{time (?:60|75|90)\}\}')
            self.assertIn('{var "support_e2_stage$"} {op "=="} {value 30}', timeout)
            self.assertIn('{var "support_e2_fail$"} {op "="} {value 6}', timeout)
            self.assertIn('("e2_order_para_exit")', timeout)
            self.assertIn('("e2_fail_and_cleanup")', timeout)
        self.assertNotIn('("e2_place_one")', self.e2)
        self.assertNotIn('("e2_place_one_entry")', self.e2)
        self.assertNotRegex(self.e2, r'\{"placement"[^}]*support_e2_para_pax')

    def test_survivors_leave_ce_tag_and_advance_on_e2_target(self) -> None:
        landed = mi_block(self.waves, '{"attack_support/e2_para_landed"')
        condition = landed.split("{actions", 1)[0]
        self.assertIn('{var "support_e2_test$"} {op "=="} {value 2}', condition)
        for tag in ("support_e2_claim", "support_e2_para_pax", "paratrooper_need_orders"):
            self.assertIn(f'{{tag {tag}}}', condition)
        actions = landed.split("{actions", 1)[1]
        self.assertIn('{source advanced}', actions)
        self.assertNotIn('{prop {prop human}}', actions)
        self.assertIn('{state {state dead}}', actions)
        self.assertIn('{state {state linked}}', actions)
        self.assertIn('{tag_add support_e2_landed}', actions)
        self.assertIn('{tag_remove paratrooper_need_orders}', actions)
        self.assertIn('{tag_remove ai_spawn}', actions)
        self.assertIn('{action advance}', actions)
        self.assertIn('{tag support_e2_flag_target}', actions)
        self.assertIn('{var "support_e2_stage$"} {op "="} {value 50}', actions)
        self.assertIn('{"trigger" {name "attack_support/e2_para_landed"}}', actions)
        for wp in (5004, 5005, 5006):
            self.assertNotIn(f'waypoint "{wp}"', self.e2)

    def test_stage50_requires_a_post_filter_operatable_survivor(self) -> None:
        landed = mi_block(self.waves, '{"attack_support/e2_para_landed"')
        actions = landed.split("{actions", 1)[1]
        self.assertIn('{"switch"', actions)
        survivor_switch = mi_block(actions, '{"switch"')
        survivor_case = mi_block(survivor_switch, '{"case"')
        survivor_default = mi_block(survivor_switch, '{"default"')
        self.assertIn(
            '{condition {type entities} {selector {ignore_captured_by_user 0} '
            '{tag support_e2_landed} {tag support_e2_claim} {type human} '
            '{state operatable}}}',
            survivor_case,
        )
        stage50 = '{"set_i" {var "support_e2_stage$"} {op "="} {value 50}}'
        self.assertEqual(actions.count(stage50), 1)
        self.assertIn(stage50, survivor_case)
        self.assertIn('{"actor_state"', survivor_case)
        self.assertIn('{"action"', survivor_case)
        self.assertIn('{"trigger" {name "attack_support/e2_para_landed"}}', survivor_case)
        self.assertNotIn(stage50, survivor_default)
        self.assertNotIn('{"actor_state"', survivor_default)
        self.assertNotIn('{"action"', survivor_default)
        self.assertNotIn('{"trigger" {name "attack_support/e2_para_landed"}}', survivor_default)

    def test_plane_delete_and_survivor_deadline_preserve_honest_failure(self) -> None:
        settle = mi_define(self.waves, "e2_para_settle")
        self.assertIn('{"delay" {time 90}}', settle)
        self.assertIn('("e2_delete_aircraft")', settle)
        self.assertIn('{"delay" {time 29}}', settle)
        self.assertIn('{tag support_e2_landed}', settle)
        self.assertIn('{var "support_e2_fail$"} {op "="} {value 7}', settle)
        fail7_at = settle.index('{var "support_e2_fail$"} {op "="} {value 7}')
        self.assertNotIn('("e2_complete_cleanup")', settle[fail7_at:])
        self.assertIn('("e2_fail_and_cleanup")', settle[fail7_at:])
        self.assertIn('{var "support_e2_stage$"} {op "="} {value 60}', settle)

    def test_deployer_guards_source_and_workshop_para_contracts(self) -> None:
        deploy = DEPLOY.read_text(encoding="utf-8")
        for array in ("$E2ParaWaveMarkers", "$E2ParaForbiddenMarkers"):
            self.assertIn(array, deploy)
        for marker in (
            '; ===== E2 PARADROP',
            '{"attack_support/e2_para_rusa"',
            '{"attack_support/e2_para_ukr"',
            '{"attack_support/e2_para_nato"',
            '{"attack_support/e2_para_landed"',
            '{effect drop_paratrooper}',
            '{tag_add paratrooper}',
            '{tag_add ignore_spawn_logic}',
            '{distance 1500}',
            '{distance 2500}',
            'support_e2_released',
            'support_e2_fail$"} {op "="} {value 6}',
            'support_e2_fail$"} {op "="} {value 7}',
        ):
            self.assertIn(marker, deploy)
        for marker in ('{effect drop_paratroopers}', 'waypoint "5004"', 'waypoint "5005"', 'waypoint "5006"'):
            self.assertIn(marker, deploy)
        self.assertIn('$sourceWaveCode = [System.IO.File]::ReadAllText($wavesSource)', deploy)
        self.assertIn('$workshopWaveCode = [System.IO.File]::ReadAllText($waves)', deploy)
        for side in ("Source", "Workshop"):
            self.assertIn(f'{side} wave engine is missing E2 paradrop marker', deploy)
            self.assertIn(f'{side} wave engine contains forbidden E2 paradrop marker', deploy)
