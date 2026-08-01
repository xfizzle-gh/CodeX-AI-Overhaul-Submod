import re
import unittest
from pathlib import Path

from test_attack_support_slot_proof import strip_comments

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

    # The two airborne chassis FAMILIES use two different state vocabularies, and the
    # canonical form for each is a census of every {Chassis} block written by the
    # editor across the installed stack:
    #   airborne     1523 blocks, every one {AirborneMode 1} + {Altitude N}; not one
    #                carries {Airborne} or {EngineStarted}.
    #   helicopter     58 blocks, every one {Airborne} + {EngineStarted} (+{Altitude});
    #                not one carries {AirborneMode}.
    # Both are paired with an explicit {ChassisManager} sibling and {DisableObstacles}
    # on a parked hull. Mixing the two vocabularies is what the previous pass shipped
    # on the three fixed-wing hulls.
    HELO_SNAPSHOT = (
        '{Chassis "helicopter"\n\t\t\t{Airborne}\n\t\t\t{EngineStarted}\n\t\t\t{Altitude 22}\n\t\t}'
        '\n\t\t{ChassisManager\n\t\t\t{Current "helicopter"}\n\t\t}'
    )
    PLANE_SNAPSHOT = (
        '{Chassis "airborne"\n\t\t\t{AirborneMode 1}\n\t\t\t{Altitude 65}\n\t\t}'
        '\n\t\t{ChassisManager\n\t\t\t{Current "airborne"}\n\t\t}'
    )

    def test_exact_aircraft_and_crews(self) -> None:
        for asset in ("mi17_b8_rus", "mi17_b8_ukr", "il-76td_para", "c130_para"):
            self.assertIn(f'{{Entity "{asset}"', self.tpl)
        for breed in ("mp/rusa/2022s/rus_pliot", "mp/ukr/2022s/ukr_pilot", "mp/nato/2022s/nato_pilot"):
            self.assertIn(f'{{Human "{breed}"', self.tpl)
        self.assertEqual(self.tpl.count(self.HELO_SNAPSHOT), 4)

    def test_nato_helo_flies_the_proven_hull_and_the_blackhawk_is_out_of_every_pool(self) -> None:
        """NATO's E2 hull is the Code:X mi17_b8_rus; uh-60m_blackhawk_mg is blocked.

        Verdict recorded 2026-07-30. The Blackhawk's West-81 def IS
        helicopter-chassis-compatible - it includes /properties/helicopter.ext, declares
        {PatherID "helicopter"}, {targetClass "helicopter"}, {ObstacleId "helicopter"
        "in_air"} and its own {Chassis "helicopter"} - so the park snapshot was valid for
        it, and the def demonstrably loaded at map parse. What was never proved is that
        the parked ACTOR instantiated: it emitted no [0x0 actor] line where every other
        E2 hull did, and the live NATO claim came back empty (combo_helo_fail 9) even
        though the trigger's own pool counts passed. NATO therefore moves to the one
        airframe with an instantiation proof, which also makes the NATO package
        structurally identical to the rusa/ukr ones. Fail code 13 decides the Blackhawk.
        """
        self.assertIn('{Entity "mi17_b8_rus" 0xb40f', self.tpl)
        for text in (self.tpl, self.waves):
            self.assertNotIn('{Entity "uh-60m_blackhawk_mg"', text)
            for line in text.splitlines():
                if "uh-60m_blackhawk_mg" in line:
                    self.assertTrue(line.lstrip().startswith(";"), line)
        # The commented record is allowed to stay; a live pool entry is not.
        self.assertNotIn("uh-60m_blackhawk_mg", strip_comments(self.tpl))
        self.assertNotIn("uh-60m_blackhawk_mg", strip_comments(self.waves))
        # Pilots, team, ids, MIDs, Links and Tags around the swapped hull are unchanged.
        for eid, breed in (
            ("0xb410", "mp/nato/2022s/nato_pilot"),
            ("0xb411", "mp/nato/2022s/nato_pilot"),
            ("0xb412", "mp/nato/2022s/82nd_squadlead"),
            ("0xb413", "mp/nato/2022s/82nd_mg"),
            ("0xb414", "mp/nato/2022s/82nd_rifleman"),
            ("0xb415", "mp/nato/2022s/82nd_rifleman"),
        ):
            self.assertIn(f'{{Human "{breed}" {eid}', self.tpl)
        self.assertIn('{MID 9814}', self.tpl)
        self.assertIn('{Link 0xb410 {0xb40f "driver"}}', self.tpl)
        self.assertIn('{Link 0xb411 {0xb40f "commander"}}', self.tpl)
        self.assertIn(
            '{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_helo" '
            '"support_e2_aircraft" "hidden" 0xb40f}',
            self.tpl,
        )
        # Three live E2 helo hulls plus the PRC Mi-171 adaptation, all one airframe now.
        self.assertEqual(self.tpl.count('{Entity "mi17_b8_rus"'), 3)
        self.assertEqual(self.tpl.count('{Entity "mi17_b8_ukr"'), 1)

    def test_every_e2_aircraft_park_block_is_in_its_families_canonical_form(self) -> None:
        """A parked aircraft is a STATE SNAPSHOT, and the two families differ.

        The previous pass wrote the three fixed-wing hulls in the HELICOPTER family's
        vocabulary - {Chassis "airborne" {Airborne}{EngineStarted}} - a form that
        appears on zero of the 1523 airborne-chassis blocks the editor has written
        across this stack, and it also omitted the {ChassisManager} override that
        Airborne_M.ext (default {chassisManager {current "wheel"}}) makes load-bearing.
        The helicopter hulls were already correct and are unchanged apart from gaining
        the explicit sibling and {DisableObstacles} every parked hull carries.
        """
        e2 = block(self.tpl, "; ===== E2 AIR PACKAGE POOLS", "; ===== TAGS =====")
        self.assertEqual(e2.count(self.HELO_SNAPSHOT), 3)
        self.assertEqual(e2.count(self.PLANE_SNAPSHOT), 3)
        entities = re.findall(r'\{Entity "([^"]+)" (0xb4[0-9a-f]{2})', e2)
        self.assertEqual(
            entities,
            [
                ("mi17_b8_rus", "0xb401"),
                ("mi17_b8_ukr", "0xb408"),
                ("mi17_b8_rus", "0xb40f"),
                ("il-76td_para", "0xb416"),
                ("c130_para", "0xb420"),
                ("c130_para", "0xb428"),
            ],
        )
        for name, eid in entities:
            with self.subTest(hull=eid):
                body = mi_block(e2, f'{{Entity "{name}" {eid}')
                self.assertIn("{DisableObstacles}", body)
                self.assertIn("{ChassisManager", body)
                self.assertIn("{Altitude ", body)
                # Positions stay 2D: altitude is the chassis snapshot's job. Two
                # coordinates only, never three.
                position = re.search(r"\{Position ([^}]*)\}", body).group(1)
                self.assertEqual(len(position.split()), 2, position)
                if name.startswith("mi17"):
                    self.assertIn('{Chassis "helicopter"', body)
                    self.assertIn('{Current "helicopter"}', body)
                    self.assertIn("{Airborne}", body)
                    self.assertIn("{EngineStarted}", body)
                    self.assertNotIn("AirborneMode", body)
                else:
                    self.assertIn('{Chassis "airborne"', body)
                    self.assertIn('{Current "airborne"}', body)
                    self.assertIn("{AirborneMode 1}", body)
                    # The invented crossover form, gone for good.
                    self.assertNotIn("{Airborne}", body)
                    self.assertNotIn("{EngineStarted}", body)
        # The PRC Mi-171 adaptation sits outside the 0xb4 band and carries the same one.
        prc = mi_block(self.tpl, '{Entity "mi17_b8_rus" 0xc200')
        self.assertIn(self.HELO_SNAPSHOT, prc)
        self.assertIn("{DisableObstacles}", prc)
        self.assertEqual(self.tpl.count(self.PLANE_SNAPSHOT), 3)
        # No airborne-chassis block anywhere in the pool may carry helicopter state,
        # and no helicopter-chassis block may carry airborne state.
        live = strip_comments(self.tpl)
        self.assertIsNone(
            re.search(r'(?s)\{Chassis "airborne"[^}]*\{(Airborne|EngineStarted)\}', live)
        )
        self.assertIsNone(re.search(r'(?s)\{Chassis "helicopter"[^}]*\{AirborneMode', live))
        # Fixed-wing altitude remains snapshot state. The only runtime air_state
        # commands are the helicopter's explicit ground-and-lift pair.
        live_waves = strip_comments(self.waves)
        self.assertNotIn("{altitude 65}", live_waves)
        self.assertEqual(live_waves.count('{"air_state"'), 2)
        self.assertIn('{altitude 0}', mi_define(live_waves, "e2_descend_helo"))
        self.assertIn('{altitude 22}', mi_define(live_waves, "e2_lift_helo"))
        self.assertIn("{Altitude 65}", self.tpl)
        # The wheel chassis is only ever named in the explanatory comment, never parked.
        self.assertNotIn('{Chassis "wheel"', live)

    def test_verified_link_place_tables_for_every_e2_airframe(self) -> None:
        """Transcribed from each airframe's own crew.ext - never invented.

        A {Link} to a place the def does not declare is dropped silently and the body is
        left standing at the park position, so the whole table is pinned by value.
        """
        links = block(
            self.tpl,
            "; ----- E2 CREW + PARATROOPER LINKS -----",
            "; sup_linked marks every body",
        )
        expected = [
            # mi17_b8_rus / mi17_b8_ukr / mi17_b8_rus (NATO): crew + seat1..4
            ("0xb402", "0xb401", "driver"),
            ("0xb403", "0xb401", "commander"),
            ("0xb409", "0xb408", "driver"),
            ("0xb40a", "0xb408", "commander"),
            ("0xb410", "0xb40f", "driver"),
            ("0xb411", "0xb40f", "commander"),
            ("0xb404", "0xb401", "seat1"),
            ("0xb405", "0xb401", "seat2"),
            ("0xb406", "0xb401", "seat3"),
            ("0xb407", "0xb401", "seat4"),
            ("0xb40b", "0xb408", "seat1"),
            ("0xb40c", "0xb408", "seat2"),
            ("0xb40d", "0xb408", "seat3"),
            ("0xb40e", "0xb408", "seat4"),
            ("0xb412", "0xb40f", "seat1"),
            ("0xb413", "0xb40f", "seat2"),
            ("0xb414", "0xb40f", "seat3"),
            ("0xb415", "0xb40f", "seat4"),
            # il-76td_para: driver, driver1, driver2, commander, commander1, seat00+
            ("0xb417", "0xb416", "driver"),
            ("0xb418", "0xb416", "driver1"),
            ("0xb419", "0xb416", "driver2"),
            ("0xb41a", "0xb416", "commander"),
            ("0xb41b", "0xb416", "commander1"),
            ("0xb41c", "0xb416", "seat01"),
            ("0xb41d", "0xb416", "seat02"),
            ("0xb41e", "0xb416", "seat03"),
            ("0xb41f", "0xb416", "seat04"),
            # c130_para: driver, driver2, commander; passengers start at seat02
            ("0xb421", "0xb420", "driver"),
            ("0xb422", "0xb420", "driver2"),
            ("0xb423", "0xb420", "commander"),
            ("0xb424", "0xb420", "seat02"),
            ("0xb425", "0xb420", "seat03"),
            ("0xb426", "0xb420", "seat04"),
            ("0xb427", "0xb420", "seat05"),
            ("0xb429", "0xb428", "driver"),
            ("0xb42a", "0xb428", "driver2"),
            ("0xb42b", "0xb428", "commander"),
            ("0xb42c", "0xb428", "seat02"),
            ("0xb42d", "0xb428", "seat03"),
            ("0xb42e", "0xb428", "seat04"),
            ("0xb42f", "0xb428", "seat05"),
        ]
        found = re.findall(r"\{Link (0x[0-9a-f]+) \{(0x[0-9a-f]+) \"([^\"]+)\"\}\}", links)
        self.assertEqual(found, expected)
        # PRC Mi-171 adaptation, same crew and passenger place table.
        self.assertIn('{Link 0xc201 {0xc200 "driver"}}', self.tpl)
        self.assertIn('{Link 0xc202 {0xc200 "commander"}}', self.tpl)
        for eid, place in zip(("0xc203", "0xc204", "0xc205", "0xc206"),
                              ("seat1", "seat2", "seat3", "seat4")):
            self.assertIn(f'{{Link {eid} {{0xc200 "{place}"}}}}', self.tpl)
        self.assertEqual(self.tpl.count('"support_e2_helo_pax"'), 16)
        # c130_para declares no seat01, and the Blackhawk-only places are nobody's.
        self.assertNotIn('{0xb420 "seat01"}', self.tpl)
        self.assertNotIn('{0xb428 "seat01"}', self.tpl)
        # gunner1/gunner2 are Blackhawk-only places (the humvee packages elsewhere in
        # this pool have their own) and no E2 airframe declares them.
        for place in ("gunner1", "gunner2"):
            self.assertNotIn(f'"{place}"}}', links)

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
        for marker in ("must park 634 prototypes", "support_e2_test", "support_e2_para_pax", "ce_ai_logic_triggers.inc"):
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

    def test_helicopter_uses_the_clone_teleport_dispatch_and_existing_pads(self) -> None:
        """The base game's own aircraft call-in recipe, adopted whole.

        The superseded mechanism placed the pool original on a GROUND entry pad and
        then tried to push it into the air with an air_state altitude command. Nothing
        ever appeared in the sky. The replacement is what the base game does: clone the
        hidden parked template to a numeric waypoint, teleport it there, and let that
        waypoint's own {commands} block re-tag and order the arrival.
        """
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== E2 PARADROP")
        self.assertNotIn("{altitude 30}", e2)
        for faction in ("rusa", "prc", "ukr", "nato"):
            self.assertNotIn('{"air_state"', mi_block(e2, f'{{"attack_support/e2_helo_{faction}"'))
        self.assertIn('{altitude 0}', mi_define(e2, "e2_descend_helo"))
        self.assertIn('{altitude 22}', mi_define(e2, "e2_lift_helo"))
        self.assertNotIn('("e2_place_aircraft_entry")', e2)
        self.assertEqual(e2.count("{drop sensor}"), 4)
        self.assertGreaterEqual(e2.count("{control AI}"), 4)
        self.assertGreaterEqual(e2.count("{action move}"), 4)
        self.assertEqual(e2.count('("e2_clone_aircraft")'), 4)
        # Ground pads are still the infantry delivery points; only the aircraft moved
        # to the numeric band.
        for side in "ab":
            for n in (1, 2):
                self.assertIn(f'{{waypoint "attack_support_air_{side}{n}"}}', e2)
        self.assertNotRegex(e2, r"support_e2_lz_fpc|e2_lz_fpc")

    def test_clone_dispatch_is_verbatim_the_base_game_recipe(self) -> None:
        clone = mi_define(self.waves, "e2_clone_aircraft")
        # One case per spawn side plus the fail-safe default, all three identical apart
        # from the destination node.
        self.assertEqual(clone.count('{"actor_to_waypoint"'), 3)
        self.assertEqual(clone.count("{clone}"), 3)
        self.assertEqual(clone.count('{approach "safe teleport & rotate"}'), 3)
        self.assertEqual(clone.count("{source advanced}"), 3)
        self.assertEqual(clone.count("{amount 1}"), 3)
        # Without this clause the selector cannot reach a hidden parked template at all.
        self.assertEqual(clone.count("{include {tag {tag hidden}}}"), 3)
        # NUMERIC destinations only: actor_to_waypoint accepts nothing else.
        self.assertEqual(clone.count('{waypoint "21"}'), 1)
        self.assertEqual(clone.count('{waypoint "22"}'), 2)
        self.assertNotRegex(clone, r'\{waypoint "[a-z_]')
        # The source marker is not a claim: the parked hull keeps its pool tag, which
        # is what makes an aircraft call-in repeatable and pool-free.
        self.assertIn("{select {tag {tag support_e2_src}}}", clone)
        self.assertIn(
            '{"entity_state" {selector {tag support_e2_src}} {tag_remove support_e2_src}}', clone
        )

    def test_arriving_clone_is_retagged_before_it_is_ordered(self) -> None:
        """A clone carries none of the parked template's tags.

        That is why the base game puts the re-tag in the destination waypoint's own
        {commands} block, which runs on the arriving actor. Every engine step that
        follows addresses support_e2_arrival, and the engine never assumes the clone
        inherited a pool tag.
        """
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== MOTORIZED INSERT")
        for helper in (
            "e2_prove_arrival",
            "e2_promote_arrival",
            "e2_own_arrival",
            "e2_arrival_is_helo",
            "e2_arrival_is_plane",
        ):
            with self.subTest(helper=helper):
                self.assertIn(f'(define "{helper}"', e2)
        # Every leg proves the arrival exists before it promotes or orders anything.
        for leg in ("e2_arrival_is_helo", "e2_arrival_is_plane"):
            self.assertLess(
                e2.index('("e2_prove_arrival")'), e2.index(f'("{leg}")'), leg
            )
        prove = mi_define(self.waves, "e2_prove_arrival")
        self.assertIn('{tag support_e2_arrival}', prove)
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 14}}', prove)
        # The promote step never re-adds a pool tag and never leaves the arrival hidden.
        promote = mi_define(self.waves, "e2_promote_arrival")
        self.assertIn("{tag_remove hidden}", promote)
        self.assertIn("{tag_add support_e2_claim}", promote)
        self.assertIn("{tag_add support_e2_aircraft}", promote)
        self.assertIn("{discovered on}", promote)
        # Ownership of the arrival is a literal 1-16 switch that fails closed.
        own = mi_define(self.waves, "e2_own_arrival")
        for player in range(1, 17):
            self.assertIn(f'{{player "{player}"}}', own)
        default = own.split('{"default"', 1)[1]
        self.assertNotIn('{player "', default)
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 8}}', default)

    def test_engine_never_touches_the_base_airstrike_namespace(self) -> None:
        """airstrike_*, enemy_air and ai_air_target are LIVE in this stack.

        The base game's ai_air chain survives into the shared dcg_script.inc and into
        two of the fourteen managed maps, complete with its own numeric waypoints and
        {commands} blocks. Every tag this engine writes is in the support_e2_* space.
        """
        live = strip_comments(self.waves)
        for foreign in ("airstrike_", "enemy_air", "ai_air_target"):
            with self.subTest(tag=foreign):
                self.assertNotIn(foreign, live)

    def test_helicopter_places_four_independent_troops_at_half_second_cadence(self) -> None:
        """The real insert carries four linked passengers and emits them only after landing."""
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== E2 PARADROP")
        emit = mi_define(self.waves, "e2_emit_helo_pax")
        promote = mi_define(self.waves, "e2_promote_helo_pax")
        grounded = mi_block(self.waves, '{"attack_support/e2_helo_grounded"')

        # Every side/LZ branch uses the shipped passenger-filter form. The crew tag is
        # a selector for occupants already linked into seat1..seat4, never a tag writer.
        self.assertEqual(emit.count('{crew {tag support_e2_helo_pax}}'), 5)
        self.assertEqual(emit.count('{emit {mode passengers}}'), 5)
        for side in "ab":
            for pad in (1, 2):
                self.assertIn(f'{{waypoint "attack_support_air_{side}{pad}"}}', emit)
        self.assertNotIn('("e2_place_one")', grounded)
        self.assertNotIn('("e2_place_one_entry")', grounded)

        # The emitted copies are promoted through one common passenger tag and only
        # after the grounded receiver has been proven on the arriving helicopter.
        self.assertIn('{select {tag {tag support_e2_helo_pax}}}', promote)
        self.assertIn('{exclude {tag {tag hidden}}}', promote)
        self.assertIn('{tag_add support_e2_pax}', promote)
        self.assertLess(
            grounded.index('("e2_emit_helo_pax")'),
            grounded.index('("e2_promote_helo_pax")'),
        )
        self.assertLess(
            grounded.index('("e2_promote_helo_pax")'),
            grounded.index('("e2_finish_team_or_fail")'),
        )

        # Four real seat links exist per faction package, and the old fake placement
        # chain is absent from every helicopter launch leg.
        templates = TEMPLATES.read_text(encoding="utf-8")
        expected_links = {
            'rusa': ('0xb401', range(0xb404, 0xb408)),
            'ukr': ('0xb408', range(0xb40b, 0xb40f)),
            'nato': ('0xb40f', range(0xb412, 0xb416)),
            'prc': ('0xc200', range(0xc203, 0xc207)),
        }
        for faction, (hull, riders) in expected_links.items():
            for seat, rider in enumerate(riders, start=1):
                self.assertIn(f'{{Link 0x{rider:x} {{{hull} "seat{seat}"}}}}', templates)
            launch = mi_block(self.waves, f'{{"attack_support/e2_helo_{faction}"')
            self.assertNotIn('("e2_place_one")', launch)
            self.assertNotIn('("e2_place_one_entry")', launch)

    def test_helicopter_has_fail_closed_faction_and_bounded_delete(self) -> None:
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== E2 PARADROP")
        for faction in ("rusa", "ukr", "nato", "prc"):
            self.assertIn(f'{{"attack_support/e2_helo_{faction}"', e2)
        # PRC flies the Mi-171 adaptation; only the fixed-wing paradrop stays excluded.
        self.assertNotIn("attack_support/e2_para_prc", e2)
        self.assertIn('{value 1}', e2)
        self.assertIn('(define "e2_delete_aircraft"', e2)
        self.assertIn('{"delete"', e2)
        self.assertRegex(e2, r'\{"delay" \{time (?:45|60|75|90)\}\}')

    def test_ownership_switch_lists_1_through_16_and_default_has_no_player(self) -> None:
        """Both ownership switches, each read as its own balanced define.

        It used to read the span between e2_own_current and e2_place_one, which was the
        same thing only for as long as nothing else lived in that gap. e2_own_pax does.
        """
        for name in ("e2_own_current", "e2_own_pax"):
            switch = mi_define(self.waves, name)
            with self.subTest(define=name):
                for player in range(1, 17):
                    self.assertIn(f'{{player "{player}"}}', switch)
                    self.assertIn(f'{{op "=="}} {{value {player}}}', switch)
                default = switch.split('{"default"', 1)[1]
                self.assertNotIn('{player "', default)
                # Fail-closed: an unresolved id transfers nothing and records 8.
                self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 8}}', default)

    def test_cleanup_targets_only_the_claimed_aircraft(self) -> None:
        delete = block(self.waves, '(define "e2_delete_aircraft"', '(define "e2_fail_and_cleanup"')
        self.assertIn('{tag support_e2_aircraft}', delete)
        self.assertIn('{tag support_e2_claim}', delete)
        self.assertNotIn('{selector {ignore_captured_by_user 0} {tag support_e2_aircraft}}', delete)

    def test_supported_pools_are_exact_and_task4_is_not_implemented_here(self) -> None:
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== E2 PARADROP")
        for faction in ("rusa", "ukr", "nato", "prc"):
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
        for faction in ("rusa", "ukr", "nato", "prc"):
            helo = mi_block(self.waves, f'{{"attack_support/e2_helo_{faction}"')
            condition = helo.split('{actions', 1)[0]
            self.assertIn('{var "support_e2_stage$"} {op "=="} {value 10}', condition)
            self.assertNotIn('{var "support_e2_stage$"} {op "=="} {value 0}', condition)
            selected = helo.index('("e2_choose_flag")')
            accepted = helo.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 20}}')
            flight = helo.index('("e2_clone_aircraft")')
            self.assertLess(selected, accepted)
            self.assertLess(accepted, flight)

    def test_pad_safety_uses_one_claimed_marker_and_two_independent_near_queries(self) -> None:
        claim = mi_define(self.waves, "e2_claim_lz_marker")
        self.assertIn('{tag support_e2_marker_tpl}', claim)
        self.assertIn('{tag_add support_e2_lz_marker}', claim)
        self.assertIn('{amount 1}', claim)
        self.assertNotIn('{tag_remove support_e2_marker_tpl}', claim)

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
        """Arrival starts descent; grounded evidence starts the actual unload."""
        lz = mi_block(self.waves, '{"attack_support/e2_helo_lz"')
        condition, actions = lz.split("{actions", 1)
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', condition)
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 30}', condition)
        self.assertIn('{tag support_e2_arrival}', condition)
        self.assertIn('{near_to', condition)
        self.assertIn('{tag support_e2_lz_marker}', condition)
        self.assertIn('{distance 120}', condition)
        self.assertNotIn('{state operatable}', condition)
        self.assertIn('{tag {tag support_e2_lz_done}}', condition)
        self.assertLess(
            actions.index('{tag_add support_e2_lz_done}'),
            actions.index('("e2_announce_helo")'),
        )
        self.assertLess(
            actions.index('("e2_announce_helo")'),
            actions.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 35}}'),
        )
        self.assertLess(
            actions.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 35}}'),
            actions.index('("e2_descend_helo")'),
        )
        self.assertNotIn('("e2_emit_helo_pax")', actions)
        self.assertNotIn('("e2_order_aircraft_exit")', actions)

        grounded = mi_block(self.waves, '{"attack_support/e2_helo_grounded"')
        g_condition, g_actions = grounded.split("{actions", 1)
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 35}', g_condition)
        self.assertIn('{tag support_e2_arrival} {tag w81_landing}', g_condition)
        self.assertIn('{near_to', g_condition)
        self.assertIn('{tag support_e2_lz_marker}', g_condition)
        self.assertIn('{distance 120}', g_condition)
        stage40 = g_actions.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}')
        emit = g_actions.index('("e2_emit_helo_pax")')
        promote = g_actions.index('("e2_promote_helo_pax")')
        finish = g_actions.index('("e2_finish_team_or_fail")')
        lift = g_actions.index('("e2_lift_helo")')
        exit_at = g_actions.index('("e2_order_aircraft_exit")')
        stage60 = g_actions.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 60}}')
        self.assertLess(stage40, emit)
        self.assertLess(emit, promote)
        self.assertLess(promote, finish)
        self.assertLess(finish, lift)
        self.assertLess(lift, exit_at)
        self.assertLess(exit_at, stage60)
        self.assertIn('("e2_complete_cleanup")', g_actions)

        timeout = mi_block(self.waves, '{"attack_support/e2_helo_timeout"')
        t_condition, t_actions = timeout.split("{actions", 1)
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 30}', t_condition)
        self.assertEqual(t_actions.count('{var "support_e2_fail$"} {op "="} {value 5}'), 2)
        self.assertEqual(t_actions.count('("e2_order_aircraft_exit")'), 2)
        self.assertEqual(t_actions.count('("e2_fail_and_cleanup")'), 2)
        self.assertIn('("e2_lift_helo")', t_actions)
        self.assertNotIn('("e2_place_one")', t_actions)
        self.assertNotIn('("e2_place_one_entry")', t_actions)

        for faction in ("rusa", "ukr", "nato", "prc"):
            helo = mi_block(self.waves, f'{{"attack_support/e2_helo_{faction}"')
            with self.subTest(helo=faction):
                self.assertNotIn('{"delay" {time 40}}', helo)
                refused = helo.split('("e2_fly_helo_or_fail")', 1)[1]
                self.assertIn('("e2_order_aircraft_exit")', refused)
                self.assertIn('("e2_fail_and_cleanup")', refused)
                self.assertNotIn('("e2_place_one")', refused)

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

        for faction in ("rusa", "ukr", "nato", "prc"):
            helo = mi_block(self.waves, f'{{"attack_support/e2_helo_{faction}"')
            fail4_at = helo.index('{var "support_e2_fail$"} {op "=="} {value 4}')
            # The whole {"case"} that fail 4 opens, not "up to the next default": the
            # stage-40 write inside it is now itself evidence-gated and carries its own.
            fail4 = mi_block(helo[helo.rindex('{"case"', 0, fail4_at):], '{"case"')
            self.assertNotIn('("e2_place_one_entry")', fail4)
            self.assertEqual(fail4.count('("e2_complete_cleanup")'), 1)
            self.assertNotIn('("e2_delete_aircraft")', fail4)
            self.assertNotIn('("e2_fail_and_cleanup")', fail4)
            self.assertNotIn('{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}', fail4)

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
        self.assertNotIn("if ($E2TestMode -ne 0)", self.deploy)
        self.assertIn("$expectedLegacyMode = 0", self.deploy)
        self.assertNotIn("$sourceLegacyInit =", self.deploy)
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
        for faction in ("rusa", "ukr", "nato", "prc"):
            child = mi_block(self.e2, f'{{"attack_support/e2_helo_{faction}"')
            condition = child.split("{actions", 1)[0]
            self.assertIn('{var "support_e2_test$"} {op "=="} {value 1}', condition)
            self.assertIn('{var "support_e2_test$"} {op "=="} {value 3}', condition)
            self.assertNotIn('{var "support_e2_test$"} {op ">"}', condition)

    def test_combo_transition_is_claim_free_and_ordered(self) -> None:
        transition = mi_block(self.e2, '{"attack_support/e2_combo_transition"')
        condition, actions = transition.split("{actions", 1)
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', condition)
        self.assertIn('{var "support_e2_test$"} {op "=="} {value 3}', condition)
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 70}', condition)
        self.assertIn('{tag support_e2_claim}', condition)
        self.assertIn("!3", condition)
        # SetVar takes integers only. The var-to-var copy that used to stand here
        # never landed: the live run failed the helo leg with fail 1 and the combo
        # still reported combo_helo_fail 0 - a fabricated success. The fold is
        # literal cases, one per code, and still runs before the reset for leg two.
        self.assertNotIn('{var "support_e2_combo_helo_fail$"} {op "="} {var ', actions)
        copy_at = actions.index('{var "support_e2_combo_helo_fail$"} {op "="} {value ')
        # Append-only: 0-12 are the codes the fold already carried, 13 is the new
        # "parked airframe absent at dispatch" code. None of them may be renumbered.
        for code in range(0, 14):
            self.assertIn(
                '{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value %d}} '
                '{"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value %d}}}' % (code, code),
                actions,
            )
        clear_at = actions.index('{"set_i" {var "support_e2_fail$"} {op "="} {value 0}}')
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
            flight = launch.index('("e2_clone_aircraft")')
            self.assertLess(selected, accepted)
            self.assertLess(accepted, owned)
            self.assertLess(owned, flight)
        self.assertNotIn('attack_support/e2_para_prc', self.e2)

    def test_para_dispatch_consumes_nothing_from_the_pool(self) -> None:
        """Cloning makes the para leg repeatable and pool-free.

        The pool counts stay in the trigger CONDITION as a park-presence proof, but
        the actions no longer remove a single pool tag: the parked plane keeps its
        crew and its seated jumpers, and the clone carries copies of both.
        """
        crew_counts = {"rusa": 5, "ukr": 3, "nato": 3}
        for faction, crew_count in crew_counts.items():
            launch = mi_block(self.waves, f'{{"attack_support/e2_para_{faction}"')
            condition, actions = launch.split("{actions", 1)
            for pool in (
                f"support_e2_{faction}_para",
                f"support_e2_{faction}_para_crew",
                f"support_e2_{faction}_para_pax",
            ):
                with self.subTest(pool=pool):
                    self.assertIn(f'{{tag {pool}}}', condition)
                    self.assertNotIn(f'{{tag_remove {pool}}}', actions)
            self.assertIn(f'{{value {crew_count}}}', condition)
            # Exactly one action marks the hull to copy, and it is not a claim.
            self.assertEqual(actions.count("{tag_add support_e2_src}"), 1)
            self.assertNotIn("{tag_add support_e2_claim}", actions)
            self.assertIn('("e2_arrival_is_plane")', actions)

    def test_launch_flight_order_is_attested_and_targets_selected_flag(self) -> None:
        """Altitude, then AI control, then the run-in - and the run-in is gated.

        The run-in used to sit inline and fire unconditionally. When the claim had
        produced no aircraft the {"action"} had nothing to order, the 90s window
        expired and the leg blamed the flight. It now lives behind
        e2_fly_para_or_fail, which orders only against a live claimed transport and
        otherwise records fail 10 without ever reaching stage 30.
        """
        gate = mi_define(self.waves, "e2_fly_para_or_fail")
        proof = gate.index("{type entities}")
        move = gate.index('{"action"', proof)
        stage30 = gate.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 30}}')
        self.assertLess(proof, move)
        self.assertLess(move, stage30)
        self.assertIn("{tag support_e2_plane}", gate[proof:move])
        self.assertIn("{tag support_e2_claim}", gate[proof:move])
        # Bare tags only. The simple-selector {state operatable} decoration that used to
        # sit here matched nothing on a live, flying aircraft (2026-07-30), so the leg
        # recorded fail 10 at dispatch and never opened the run-in at all.
        self.assertNotIn("{state operatable}", gate)
        self.assertIn("{action move}", gate[move:stage30])
        self.assertIn("{tag support_e2_flag_target}", gate[move:stage30])
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 10}}', gate)
        for faction in ("rusa", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_para_{faction}"')
            clone = launch.index('("e2_clone_aircraft")')
            promote = launch.index('("e2_promote_arrival")', clone)
            actor = launch.index('{"actor_state"', promote)
            fly = launch.index('("e2_fly_para_or_fail")', actor)
            self.assertLess(clone, promote)
            self.assertLess(promote, actor)
            self.assertLess(actor, fly)
            self.assertIn("{tag support_e2_arrival}", launch[actor:fly])
            self.assertIn("{drop sensor}", launch[actor:fly])
            self.assertIn("{control AI}", launch[actor:fly])
            self.assertIn("{movement {speed fast}}", launch[actor:fly])
            # Altitude comes from the parked chassis snapshot, and the aircraft is
            # never placed onto a ground pad any more.
            self.assertNotIn('{"air_state"', launch)
            self.assertNotIn('("e2_place_aircraft_entry")', launch)
            # No ungated run-in may remain in the trigger itself.
            self.assertNotIn('{"action"', launch)

    def test_release_is_target_anchored_banded_and_one_shot(self) -> None:
        for faction in ("rusa", "ukr", "nato"):
            release = mi_block(self.waves, f'{{"attack_support/e2_para_release_{faction}"')
            condition = release.split("{actions", 1)[0]
            self.assertIn('{var "support_e2_test$"} {op "=="} {value 2}', condition)
            self.assertIn('{var "support_e2_stage$"} {op "=="} {value 30}', condition)
            # ===== RE-KEYED ONTO THE PROVENANCE TAG (2026-07-31) =====
            # Every selector in this monitor used to name support_e2_plane +
            # support_e2_claim, plus a {state "not dead"} decoration. The live run of
            # 2026-07-31 proved that triple unreadable on the para leg from the other
            # side: e2_fly_para_or_fail's claim-keyed case matched nothing on a C-130
            # the player watched fly the whole map, so the leg never even left stage 20.
            # support_e2_arrival is written by the numeric waypoint's own {commands}
            # block, exists on the dispatched clone and on nothing else in the mission,
            # and is what attack_support/e2_helo_lz's near check fired on when the
            # helicopter reached its LZ - the one near-check shape with a live proof.
            self.assertNotIn('{tag support_e2_plane}', condition)
            self.assertNotIn('{tag support_e2_claim}', condition)
            self.assertNotIn('{state "not dead"}', condition)
            self.assertNotIn('{state operatable}', condition)
            self.assertIn('{tag support_e2_arrival}', condition)
            self.assertIn('{near_to', condition)
            self.assertIn('{tag support_e2_flag_target}', condition)
            # Live proof: the 4000-unit annulus fired at the backline. Release now
            # requires the aircraft to reach 1500 units (150 m) from the selected flag.
            self.assertEqual(condition.count('{distance 1500}'), 1)
            self.assertNotIn('{distance 4000}', condition)
            self.assertNotIn('{distance 600}', condition)
            self.assertNotIn('{distance 2500}', condition)
            self.assertNotIn('{"6.near"', condition)
            self.assertNotIn('{"8.cmp_i"', condition)
            expression = re.search(r'\{expression "([^"]+)"\}', condition).group(1)
            self.assertEqual(expression, "1 & 2 & 3 & 4 & 5 & 7")
            self.assertIn('{tag support_e2_released}', condition)
            actions = release.split("{actions", 1)[1]
            tagged = actions.index('{tag_add support_e2_released}')
            effect = actions.index('{effect drop_paratrooper}')
            stage40 = actions.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}')
            self.assertLess(tagged, effect)
            self.assertLess(effect, stage40)
            # ===== ONE VERB, EXACTLY AS THE SHIPPED CALL-IN FIRES IT (2026-07-31) =====
            # This block used to pair the effect with an {"emit" {mode passengers}}, on
            # the belief that drop_paratrooper only opens the cargo bay and that some
            # other verb therefore had to unlink the seats. The cargo-bay half of that
            # reading is right; the conclusion was wrong. Code:X's own conquest paradrop
            # ("[ordos] paratrooper detector script for dcg",
            # forest_/campaign_capture_the_flag.mi:31963-32127 and
            # bakhmut_1/campaign_capture_the_flag.mi:17293-17384) fires
            # {effect drop_paratrooper} and NOTHING ELSE - no emit, no second effect -
            # because open_cargo plays "cargo_open" with the `callback` keyword
            # (Airborne_M.inc:3645), and {on animation_end "cargo_open"}
            # (Airborne_M.inc:3765-3793) is what calls drop_desant1/2/3, which do the
            # {with_linked_entity "seatNN" {unlink}}. Both our airframes reach that
            # fall-through branch because both remove "place1_busy" on spawn.
            # The emit was actively harmful: it force-unloaded the seats one second
            # after the effect, before the staggered cascade could run.
            self.assertNotIn('{"emit"', actions)
            self.assertNotIn("{mode passengers}", actions)
            # The effect is addressed at the claimed arrival, never at the parked
            # original which must survive for the next call-in - and in the advanced
            # {group {select ...}} form the shipped trigger uses.
            effect_block = mi_block(actions, '{"effect"')
            self.assertIn("{tag support_e2_arrival}", effect_block)
            self.assertNotIn("{tag support_e2_plane}", effect_block)
            self.assertNotIn("{tag support_e2_claim}", effect_block)
            self.assertNotIn("{state inhabited}", effect_block)
            self.assertIn("{source advanced}", effect_block)
            # The shipped 0.1s beat after the effect.
            self.assertIn('{"delay" {time 0.1}}', actions)
            # ...and the plane may not be sent away on the same tick as the release:
            # eject_troopers staggers its unlinks out to 11s after drop_paratroopers.
            wait = actions.index('{"delay" {time 15}}')
            self.assertLess(effect, wait)
            self.assertLess(wait, stage40)
            self.assertLess(stage40, actions.index('("e2_order_aircraft_exit")'))
            # The pax-tagging route is gone with the claim: a clone's jumpers carry
            # none of this engine's tags, so nothing may pretend to address them.
            self.assertNotIn("support_e2_para_pax", actions)
        # drop_paratrooper (singular) is the receiver Code:X declares for both
        # airframes; drop_paratroopers (plural) belongs to a different prop group.
        live_e2 = strip_comments(self.e2)
        self.assertEqual(live_e2.count('{effect drop_paratrooper}'), 3)
        self.assertNotIn('{effect drop_paratroopers}', live_e2)
        # No emit verb survives anywhere in the E2 paradrop.
        self.assertEqual(live_e2.count("{mode passengers}"), 0)
        self.assertNotIn('{"emit"', live_e2)

    def test_missed_release_is_fail6_and_cannot_place_passengers(self) -> None:
        """Both exit cases go through the evidence gate, and both write a code.

        Fail 6 used to be spelled out inline on the stage-30 case only, and the stage-20
        case wrote no code at all - which is how the 2026-07-31 run walked 20 -> 60 -> 70
        with band 0, pass 0 and fail 0. The two cases are now identical and both route
        through e2_para_require_release_or_fail, which is where the literal 6 lives.
        """
        for faction in ("rusa", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_para_{faction}"')
            timeout = launch.split('("e2_fly_para_or_fail")', 1)[1]
            self.assertRegex(timeout, r'\{"delay" \{time (?:60|75|90)\}\}')
            self.assertIn('{var "support_e2_stage$"} {op "=="} {value 30}', timeout)
            self.assertIn('("e2_order_aircraft_exit")', timeout)
            self.assertIn('("e2_fail_and_cleanup")', timeout)
            # And the leg cannot hang at stage 20 when the gate refused the run-in:
            # that branch exits the plane and cleans up, preserving fail 10.
            self.assertIn('{var "support_e2_stage$"} {op "=="} {value 20}', timeout)
            # Two exit cases, two gate calls, and neither writes stage 60 unguarded.
            self.assertEqual(timeout.count('("e2_para_require_release_or_fail")'), 2)
            for case in re.findall(
                r'\{"case" \{condition \{type cmp_i\} \{var "support_e2_stage\$"\} '
                r'\{op "=="\} \{value \d+\}\}[^\n]*',
                timeout,
            ):
                with self.subTest(faction=faction, case=case[:64]):
                    gate = case.index('("e2_para_require_release_or_fail")')
                    self.assertLess(gate, case.index('{value 60}'))
                    # Off the map BEFORE the delete, with the helo leg's transit time.
                    exit_at = case.index('("e2_order_aircraft_exit")')
                    self.assertLess(exit_at, case.index('("e2_fail_and_cleanup")'))
                    self.assertIn('{"delay" {time 60}}', case)
                    self.assertNotIn('{"delay" {time 10}}', case)
        gate = mi_define(self.waves, "e2_para_require_release_or_fail")
        self.assertIn('{var "support_e2_fail$"} {op "="} {value 6}', gate)
        self.assertNotIn('("e2_place_one")', self.e2)
        self.assertNotIn('("e2_place_one_entry")', self.e2)
        self.assertNotRegex(self.e2, r'\{"placement"[^}]*support_e2_para_pax')

    def test_jumper_ordering_is_retired_and_never_reintroduced(self) -> None:
        """A clone's jumpers carry none of this engine's tags, so it cannot order them.

        attack_support/e2_para_landed selected landed jumpers by support_e2_para_pax +
        support_e2_claim. Under the clone dispatch those tags exist only on the parked
        originals, and no selector form can distinguish one aircraft's untagged jumpers
        from anyone else's. The trigger is retired with a recorded reason; ordering is
        left to CE, which already owns everything it tags paratrooper_need_orders. This
        engine only PROVES a jumper landed, so the ordering conflict the plan warns
        about cannot arise.
        """
        self.assertNotIn('{"attack_support/e2_para_landed"', self.waves)
        self.assertNotIn("support_e2_landed", self.waves)
        self.assertIn("attack_support/e2_para_landed RETIRED", self.waves)
        # And this engine still never issues an order to a CE-tagged paratrooper.
        live = strip_comments(self.e2)
        for chunk in re.findall(r'(?s)\{"(?:action|actor_state)"\{?.{0,400}', live):
            self.assertNotIn("paratrooper_need_orders", chunk)
        for wp in (5004, 5005, 5006):
            self.assertNotIn(f'waypoint "{wp}"', self.e2)

    def test_plane_delete_and_survivor_deadline_preserve_honest_failure(self) -> None:
        settle = mi_define(self.waves, "e2_para_settle")
        self.assertIn('{"delay" {time 90}}', settle)
        self.assertIn('("e2_delete_aircraft")', settle)
        self.assertIn('{"delay" {time 29}}', settle)
        # The landing proof is a live CE-tagged paratrooper near the chosen flag.
        self.assertEqual(settle.count("{tag paratrooper_need_orders}"), 2)
        self.assertEqual(settle.count("{tag support_e2_flag_target}"), 2)
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
            '{effect drop_paratrooper}',
            '{mode passengers}',
            'paratrooper_need_orders',
            '{"attack_support/e2_para_range"',
            '{"attack_support/e2_para_alive"',
            '(define "e2_para_range_poll"',
            '{distance 600}',
            '{distance 4000}',
            'support_e2_para_range_band$',
            'support_e2_para_pass$',
            # The retired band values are now forbidden markers, not required ones.
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


class E2EvidenceGateTests(unittest.TestCase):
    """Stages advance on entity proof, never on a timer alone.

    Live run 2026-07-30 (game.log, 72 identical heartbeats over two missions):
    `e2_test 2 e2_stage 70 e2_fail 1 e2_combo_helo_fail 0` from the first sample
    onward, while `faction_support_army` read 0 for the first ~28 seconds and the
    player never saw an aircraft. Both legs died in the by-army switch before a
    hull was ever claimed, and the combo still reported the helo leg clean.
    """

    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")

    def test_dispatch_waits_for_the_published_faction(self) -> None:
        dispatch = mi_block(self.waves, '{"attack_support/e2_dispatch"')
        condition = dispatch.split("{actions", 1)[0]
        self.assertIn('{var "faction_support_army$"} {op ">"} {value 0}', condition)
        term = re.search(
            r'\{"(\d+)\.cmp_i" \{var "faction_support_army\$"\} \{op ">"\} \{value 0\}\}',
            condition,
        )
        self.assertIsNotNone(term, condition)
        expression = re.search(r'\{expression "([^"]+)"\}', condition).group(1)
        # The term has to be ANDed in, not merely present in the term list.
        self.assertIn(term.group(1), re.findall(r"\d+", expression))
        self.assertNotIn("|", expression)

    def test_unresolved_army_has_its_own_code(self) -> None:
        for name in ("e2_trigger_helo_by_army", "e2_trigger_para_by_army"):
            body = mi_define(self.waves, name)
            with self.subTest(define=name):
                # 12 = army not published yet; 1 stays "this army has no such package"
                # (PRC has no fixed-wing call-in), so the two are never confused.
                self.assertIn(
                    '{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 0}} '
                    '{"set_i" {var "support_e2_fail$"} {op "="} {value 12}}',
                    body,
                )
                self.assertIn('{"default" {"set_i" {var "support_e2_fail$"} {op "="} {value 1}}', body)
                self.assertLess(body.index("{value 12}"), body.index('{"default"'))

    def test_arrival_proof_runs_before_the_lifecycle(self) -> None:
        """The claim proofs are superseded by the arrival proof.

        Nothing is claimed from the pool any more, so "the claim produced no aircraft"
        (9) has no dispatch-time meaning: the dispatch-time question is whether the
        clone reached its waypoint (14). Fail 9 stays wired, on the stage-30 liveness
        monitor, where it now means "the aircraft that was flying has gone".
        """
        for family, factions in (("helo", ("rusa", "prc", "ukr", "nato")),
                                 ("para", ("rusa", "ukr", "nato"))):
            for faction in factions:
                launch = mi_block(self.waves, f'{{"attack_support/e2_{family}_{faction}"')
                with self.subTest(leg=f"{family}_{faction}"):
                    self.assertLess(
                        launch.index('("e2_clone_aircraft")'),
                        launch.index('("e2_prove_arrival")'),
                    )
                    self.assertLess(
                        launch.index('("e2_prove_arrival")'),
                        launch.index('("e2_promote_arrival")'),
                    )
        for name in ("e2_prove_helo_claim", "e2_prove_para_claim"):
            with self.subTest(retired=name):
                self.assertNotIn(f'(define "{name}"', self.waves)
                self.assertNotIn(f'("{name}")', self.waves)
        body = mi_define(self.waves, "e2_prove_arrival")
        self.assertIn("{type entities}", body)
        self.assertIn("{tag support_e2_arrival}", body)
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 14}}', body)
        alive = mi_block(self.waves, '{"attack_support/e2_para_alive"')
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 9}}', alive)

    def test_flight_and_landing_stages_need_a_live_entity(self) -> None:
        fly = mi_define(self.waves, "e2_fly_helo_or_fail")
        self.assertIn("{type entities}", fly)
        self.assertNotIn("{state operatable}", fly)
        self.assertIn("{tag support_e2_helo}", fly)
        self.assertIn("{tag support_e2_aircraft}", fly)
        self.assertIn("{tag support_e2_claim}", fly)
        self.assertIn('("e2_order_aircraft_lz")', fly)
        self.assertIn('{"set_i" {var "support_e2_stage$"} {op "="} {value 30}}', fly)
        default = fly.split('{"default"', 1)[1]
        self.assertIn('{var "support_e2_fail$"} {op "=="} {value 0}', default)
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 10}}', default)

        # Stage 35 is backed by physical arrival at the selected LZ. Stage 40 is
        # backed by the aircraft's own altitude_checker receiver adding w81_landing.
        lz = mi_block(self.waves, '{"attack_support/e2_helo_lz"')
        lz_condition, lz_actions = lz.split("{actions", 1)
        self.assertIn('{tag support_e2_arrival}', lz_condition)
        self.assertIn('{tag support_e2_lz_marker}', lz_condition)
        self.assertIn('{distance 120}', lz_condition)
        self.assertIn('{"set_i" {var "support_e2_stage$"} {op "="} {value 35}}', lz_actions)

        grounded = mi_block(self.waves, '{"attack_support/e2_helo_grounded"')
        grounded_condition, grounded_actions = grounded.split("{actions", 1)
        self.assertIn('{tag support_e2_arrival} {tag w81_landing}', grounded_condition)
        self.assertIn('{tag support_e2_lz_marker}', grounded_condition)
        self.assertIn('{distance 120}', grounded_condition)
        self.assertIn('{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}', grounded_actions)

        team = mi_define(self.waves, "e2_finish_team_or_fail")
        self.assertNotIn("{state operatable}", team)
        self.assertNotIn("{state {state dead}}", team)
        self.assertNotIn("{state {state inactive}}", team)
        self.assertIn('{selector {ignore_captured_by_user 0} {tag support_e2_pax}}', team)
        self.assertLess(team.index('("e2_own_pax")'), team.index("{type entities}"))
        self.assertIn('("e2_order_team")', team)
        self.assertIn('{"set_i" {var "support_e2_stage$"} {op "="} {value 50}}', team)
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 11}}', team)

        for faction in ("rusa", "prc", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_helo_{faction}"')
            with self.subTest(helo=faction):
                self.assertIn('("e2_fly_helo_or_fail")', launch)
                self.assertNotIn('("e2_finish_team_or_fail")', launch)
                for stage in (30, 35, 40, 50):
                    needle = '{"set_i" {var "support_e2_stage$"} {op "="} {value ' + str(stage) + '}}'
                    self.assertNotIn(needle, launch)

    def test_an_earlier_failure_is_never_relabelled_as_a_flight_failure(self) -> None:
        """fail 5 ("never reached the LZ") must not overwrite fail 9/10.

        The arrival window closes on every abort path, so an unguarded `fail = 5`
        there would bury the real reason - the exact class of lie this task is about.
        """
        needle = '{"set_i" {var "support_e2_fail$"} {op "="} {value 5}}'
        # Fail 5 now lives in one place for the whole helo family: the arrival-window
        # timeout. No faction leg may name it at all.
        for faction in ("rusa", "prc", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_helo_{faction}"')
            with self.subTest(helo=faction):
                self.assertNotIn(needle, launch)
        timeout = mi_block(self.waves, '{"attack_support/e2_helo_timeout"')
        self.assertEqual(timeout.count(needle), 2)
        cursor = 0
        for _ in range(2):
            at = timeout.index(needle, cursor)
            guard = timeout[:at]
            guard = guard[guard.rindex('{"case"'):]
            self.assertIn(
                '{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}',
                guard,
            )
            cursor = at + len(needle)


class E2ParkedAirframeProofTests(unittest.TestCase):
    """Fail code 13 = the parked airframe was absent at dispatch.

    Live run 2026-07-30, NATO human-attack, -E2TestMode 3: the helo leg reported
    e2_combo_helo_fail 9 - e2_prove_helo_claim found no entity carrying
    support_e2_helo + support_e2_aircraft + support_e2_claim - even though the
    trigger attack_support/e2_helo_nato had fired, meaning its own condition terms
    8/9/10 had counted the parked hull, its 2 crew and its 4-man team. Code 9 cannot
    tell "the entity was never there" from "the entity was there and the claim
    selector did not take it". 13 is set BEFORE any claim, which is the only point at
    which the two are separable, and it is what makes the next run decidable.
    """

    E2_POOLS = (
        ("rusa", "helo"),
        ("ukr", "helo"),
        ("nato", "helo"),
        ("prc", "helo"),
        ("rusa", "para"),
        ("ukr", "para"),
        ("nato", "para"),
    )

    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.deploy = DEPLOY.read_text(encoding="utf-8")

    def test_fail_code_table_is_append_only_and_documents_13_and_14(self) -> None:
        table = block(
            self.waves,
            "; Fail codes (stable - append only, never renumber):",
            '(define "e2_prove_park_rusa_helo"',
        )
        for code, phrase in (
            (1, "faction/mode switch fell through"),
            (2, "child trigger claimed nothing"),
            (3, "no active flag to fly at"),
            (4, "both deep LZ pads unsafe"),
            (5, "helo never reached the LZ"),
            (6, "para plane never released its jumpers"),
            (7, "no paratrooper survived the landing"),
            (8, "owner id unresolved"),
            (9, "the claim produced no aircraft at all"),
            (10, "the claimed aircraft was not operatable"),
            (11, "the insert landed no live body"),
            (12, "faction army still unresolved at dispatch"),
            (13, "the parked airframe was absent at dispatch"),
            # Appended by the clone restructure, never renumbering 1-13.
            (14, "the clone never arrived"),
        ):
            with self.subTest(code=code):
                self.assertRegex(table, rf";\s+{code}\s+{re.escape(phrase)}")
        self.assertNotRegex(table, r";\s+15\s")
        # 13, 9 and 14 stay three separate facts: nothing was parked / something was
        # parked but the leg selector took nothing / the parked hull was copied but no
        # copy ever reached the destination waypoint.
        self.assertIn(
            '{"set_i" {var "support_e2_fail$"} {op "="} {value 13}}', self.waves
        )
        self.assertIn(
            '{"set_i" {var "support_e2_fail$"} {op "="} {value 14}}', self.waves
        )
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 9}}', self.waves)
        # And the combo fold carries 14 as itself, not as the 99 catch-all.
        transition = mi_block(self.waves, '{"attack_support/e2_combo_transition"')
        self.assertIn(
            '{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 14}} '
            '{"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 14}}}',
            transition,
        )

    def test_every_faction_dispatch_probes_the_parked_hull_before_any_claim(self) -> None:
        for faction, kind in self.E2_POOLS:
            probe = f"e2_prove_park_{faction}_{kind}"
            body = mi_define(self.waves, probe)
            with self.subTest(probe=probe):
                # The intersection, not just the pool tag: the pool tag alone is what
                # the trigger condition already counted, and it counted true in the run
                # whose claim came back empty.
                self.assertIn(
                    f'{{selector {{ignore_captured_by_user 0}} {{tag support_e2_{faction}_{kind}}} '
                    '{tag support_e2_aircraft}}',
                    body,
                )
                self.assertIn("{type entities}", body)
                self.assertIn('{count {op ">="} {value 1}}', body)
                self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 13}}', body)
                self.assertIn('("e2_fail_and_cleanup")', body)

            trigger = mi_block(self.waves, f'{{"attack_support/e2_{kind}_{faction}"')
            actions = trigger.split("{actions", 1)[1]
            with self.subTest(trigger=f"e2_{kind}_{faction}"):
                self.assertEqual(actions.count(f'("{probe}")'), 1)
                # Nothing may be marked, proved or flown before the probe, and the rest
                # of the leg is fenced behind fail$ == 0 so 13 is never overwritten.
                # The helo leg still claims its four troops; the para leg claims
                # nothing at all now, so the shared marker is the src tag.
                probe_at = actions.index(f'("{probe}")')
                self.assertLess(probe_at, actions.index("{tag_add support_e2_src}"))
                self.assertLess(probe_at, actions.index('("e2_choose_flag")'))
                gate = actions[probe_at:]
                gate = gate[gate.index('{"switch"') :]
                self.assertLess(
                    gate.index(
                        '{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}'
                    ),
                    gate.index("{tag_add support_e2_src}"),
                )

    def test_thirteen_is_carried_by_the_combo_fold_and_the_deploy_guards(self) -> None:
        transition = mi_block(self.waves, '{"attack_support/e2_combo_transition"')
        self.assertIn(
            '{"case" {condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 13}} '
            '{"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 13}}}',
            transition,
        )
        self.assertIn('{"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 99}}', transition)
        for marker in (
            '{"set_i" {var "support_e2_fail$"} {op "="} {value 13}}',
            '{"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 13}}',
        ):
            self.assertIn(marker, self.deploy)
        for faction, kind in self.E2_POOLS:
            self.assertIn(f'(define "e2_prove_park_{faction}_{kind}"', self.deploy)
        self.assertIn(
            "Source faction pool parks the unproven uh-60m_blackhawk_mg airframe", self.deploy
        )
        self.assertIn(
            "Workshop faction pool parks the unproven uh-60m_blackhawk_mg airframe", self.deploy
        )


class E2ParaRunInLivenessTests(unittest.TestCase):
    """Stage 30 must be unsustainable without a live aircraft.

    Live run 2026-07-30: the para leg reached stage 30 - so e2_prove_para_claim AND
    e2_fly_para_or_fail both passed, i.e. a claimed, operatable plane existed at that
    instant - then sat there for ~92 seconds with no aircraft visible anywhere in the
    sky and no minimap contact, and finally reported fail 6 ("never released"). Nothing
    between the one-shot proof and the timeout ever re-proved the aircraft, so fail 6
    was blaming a release band for an aircraft that may already have been gone.
    """

    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.lua = LUA.read_text(encoding="utf-8")
        cls.e2 = block(cls.waves, "; ===== E2 PARADROP", "; ===== MOTORIZED INSERT")

    def test_liveness_monitor_is_armed_for_all_of_stage_30(self) -> None:
        alive = mi_block(self.waves, '{"attack_support/e2_para_alive"')
        condition, actions = alive.split("{actions", 1)
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', condition)
        self.assertIn('{var "support_e2_test$"} {op "=="} {value 2}', condition)
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 30}', condition)
        # It fires on the ABSENCE of the proof, so it is a standing re-proof rather
        # than a one-shot: while the aircraft is alive the condition is simply false.
        self.assertIn(
            '{"4.entities" {selector {ignore_captured_by_user 0} {tag support_e2_plane} '
            '{tag support_e2_aircraft} {tag support_e2_claim}} '
            '{count {op ">="} {value 1}}}',
            condition,
        )
        self.assertNotIn('{state operatable}', condition)
        # THE LIVE-ARRIVAL INTERLOCK, as a condition term. Term 4 asks a bookkeeping
        # question and those tags provably stop being co-resident on an aircraft that is
        # still flying; term 5 asks the question about the world, on the provenance tag,
        # in the advanced exclude form. While a live arrival exists this monitor cannot
        # fire at all, so neither fail 9 nor fail 10 is reachable under a healthy hull.
        self.assertIn(
            '{"5.entities" {selector {source advanced} {group '
            '{select {tag {tag support_e2_arrival}}} '
            '{exclude {state {state dead}} {state {state inactive}}}}} '
            '{count {op ">="} {value 1}}}',
            condition,
        )
        expression = re.search(r'\{expression "([^"]+)"\}', condition).group(1)
        self.assertEqual(expression, "1 & 2 & 3 & !4 & !5")
        # Hull still present but not operatable is 10; nothing claimed at all is 9.
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 10}}', actions)
        self.assertIn(
            '{"default" {"set_i" {var "support_e2_fail$"} {op "="} {value 9}}}', actions
        )
        self.assertLess(
            actions.index('{value 10}}'), actions.index('{"default" {"set_i"')
        )
        self.assertIn('{"set_i" {var "support_e2_stage$"} {op "="} {value 60}}', actions)
        self.assertIn('("e2_fail_and_cleanup")', actions)
        self.assertNotIn('{value 6}', actions)

    def test_closest_approach_tracker_is_integer_only_and_self_bounded(self) -> None:
        poll = mi_define(self.waves, "e2_para_range_poll")
        # Coarse bands, tightest first, so the first matching case is the true band.
        for distance in (1000, 2000, 3000, 4000):
            self.assertIn(f'{{distance {distance}}}', poll)
        self.assertEqual(poll.count("{type near}"), 4)
        for band in (1, 2, 3, 4):
            self.assertIn(
                f'{{"set_i" {{var "support_e2_para_range_band$"}} {{op "="}} {{value {band}}}}}',
                poll,
            )
        # SetVar integers only: the trend is literal case folds, never a var-to-var copy.
        self.assertNotRegex(poll, r'\{op "="\} \{var ')
        self.assertEqual(
            poll.count('{"set_i" {var "support_e2_para_pass$"} {op "="} {value 1}}'), 7
        )
        self.assertNotIn('{"set_i" {var "support_e2_para_pass$"} {op "="} {value 0}}', poll)

        tracker = mi_block(self.waves, '{"attack_support/e2_para_range"')
        condition, actions = tracker.split("{actions", 1)
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', condition)
        self.assertIn('{var "support_e2_test$"} {op "=="} {value 2}', condition)
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 30}', condition)
        self.assertIn('("e2_para_range_poll")', actions)
        # Bounded: the poll re-arms itself only while stage 30 is still open.
        rearm = '{"trigger" {name "attack_support/e2_para_range"}}'
        self.assertEqual(actions.count(rearm), 1)
        guard = actions[: actions.index(rearm)]
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 30}', guard)
        self.assertRegex(actions, r'\{"delay" \{time 0\.5\}\}')

    def test_tracker_state_is_declared_reset_and_mirrored(self) -> None:
        for name in ("support_e2_para_range_band", "support_e2_para_pass"):
            with self.subTest(var=name):
                self.assertIn(f'{{"{name}"}}', self.vars)
                self.assertIn(f'{{var "{name}$"}} {{op "="}} {{value 0}}', self.waves)
                self.assertIn(f'readVar("{name}")', self.lua)
        init = block(self.waves, '{"attack_support/init"', '{"attack_support/clock"')
        for name in ("support_e2_para_range_band", "support_e2_para_pass"):
            self.assertIn(f'{{"set_i" {{var "{name}$"}} {{op "="}} {{value 0}}}}', init)
        # And cleared per leg, so a second leg cannot inherit leg one's release grant.
        reset = mi_define(self.waves, "e2_reset_target")
        self.assertIn(
            '{"set_i" {var "support_e2_para_range_band$"} {op "="} {value 0}}', reset
        )
        self.assertIn('{"set_i" {var "support_e2_para_pass$"} {op "="} {value 0}}', reset)

    def test_fail6_is_still_reachable_for_a_genuine_never_approached_run(self) -> None:
        """The fallback must not mask "the plane never came near the flag"."""
        for faction in ("rusa", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_para_{faction}"')
            with self.subTest(para=faction):
                timeout = launch.split('("e2_fly_para_or_fail")', 1)[1]
                self.assertIn('{var "support_e2_stage$"} {op "=="} {value 30}', timeout)
                # Fail 6 moved into e2_para_require_release_or_fail, which both exit
                # cases call - so it is reachable from strictly more paths than before,
                # never fewer.
                self.assertIn('("e2_para_require_release_or_fail")', timeout)
        gate = mi_define(self.waves, "e2_para_require_release_or_fail")
        self.assertIn('{var "support_e2_fail$"} {op "="} {value 6}', gate)
        # Nothing outside the tracker may grant the release pass, and the tracker only
        # grants it from inside the outer ring - so a run-in that never gets within 4000
        # leaves band 0, pass 0, no release, and the 90s timeout still reports 6.
        self.assertEqual(
            self.waves.count('{"set_i" {var "support_e2_para_pass$"} {op "="} {value 1}}'), 7
        )
        poll = mi_define(self.waves, "e2_para_range_poll")
        self.assertEqual(
            poll.count('{"set_i" {var "support_e2_para_pass$"} {op "="} {value 1}}'), 7
        )
        # The outer ring is ANDed into every release monitor on its own, so term 8 can
        # never release a plane that is outside it.
        for faction in ("rusa", "ukr", "nato"):
            release = mi_block(self.waves, f'{{"attack_support/e2_para_release_{faction}"')
            expression = re.search(
                r'\{expression "([^"]+)"\}', release.split("{actions", 1)[0]
            ).group(1)
            self.assertIn("5 & 7", expression)
            self.assertNotIn("(5", expression)


class E2NumericWaypointBandTests(unittest.TestCase):
    """The 21-24 air waypoint band: free, deploy-generated, and command-carrying.

    {"actor_to_waypoint"} and the {waypoint} order term accept NUMERIC names only, so
    the air nodes cannot use the attack_support_* naming the pads use. That makes a
    collision sweep mandatory rather than cosmetic: the base game's own airstrike
    geometry already occupies "0" on all fourteen managed maps and "1".."6" on two of
    them, complete with its own enemy_air {commands} blocks.

    The band was 9101-9104 until 2026-07-31, when every map in the family failed to
    load: "Can't use waypoint id, it already used (eHelperWaypoint.cpp:55)", raised on
    the first of the four even though each id appeared exactly once in the file. A
    numeric waypoint name is an engine-side id, not a label, and the usable space is
    small - no shipped .mi anywhere on disk (vanilla or workshop) names a waypoint
    above 1000. RETIRED_BAND stays asserted-against so the crash cannot come back.
    """

    BAND = ("21", "22", "23", "24")
    RETIRED_BAND = ("9101", "9102", "9103", "9104")
    MAPS = sorted((ROOT / "resource/map/multi").glob("dcg_[[]cwa71[]]_*/campaign_capture_the_flag.mi"))

    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.deploy = DEPLOY.read_text(encoding="utf-8")

    def test_the_family_is_the_fourteen_managed_maps(self) -> None:
        self.assertEqual(len(self.MAPS), 14)

    def test_band_does_not_collide_with_any_existing_waypoint_name(self) -> None:
        """Swept against every managed map and every .inc under resource/map."""
        for path in self.MAPS:
            text = path.read_text(encoding="utf-8")
            waypoints = mi_block(text, "{waypoints")
            names = set(re.findall(r'\{"([^"]+)"', waypoints))
            with self.subTest(map=path.parent.name):
                # The base geometry we must never disturb.
                self.assertIn("0", names)
                for numeric in self.BAND:
                    self.assertNotIn(numeric, names)
        for inc in sorted((ROOT / "resource/map").rglob("*.inc")):
            text = inc.read_text(encoding="utf-8", errors="ignore")
            for numeric in self.BAND:
                with self.subTest(inc=inc.name, name=numeric):
                    # Our own engine's move orders are the only thing allowed to name
                    # the band, and it names it as an order target, never as a block.
                    self.assertNotIn(f'{{"{numeric}"', text)

    def test_engine_addresses_the_band_and_nothing_else_numeric(self) -> None:
        live = strip_comments(self.waves)
        numeric_targets = set(re.findall(r'\{waypoint "(\d+)"\}', live))
        # Motor cleanup now routes through named map-edge entry waypoints.
        self.assertEqual(numeric_targets, set(self.BAND))
        self.assertNotIn('{waypoint "0"}', mi_define(live, "as_finish_motor"))

    def test_deploy_generates_the_band_idempotently_and_self_healing(self) -> None:
        deploy = self.deploy
        # Strip-then-rebuild, with a brace matcher because the entry nodes carry a
        # nested {commands} block the flat pad regex cannot reach.
        self.assertIn("function Remove-NamedWaypointBlock", deploy)
        self.assertIn(
            "foreach ($num in @('21', '22', '23', '24', '9101', '9102', '9103', '9104')) {\n"
            "        $text = Remove-NamedWaypointBlock -Text $text -Name $num\n"
            "    }",
            deploy,
        )
        self.assertIn("$entryNum = if ($side -eq 'a') { '21' } else { '22' }", deploy)
        self.assertIn("$exitNum = if ($side -eq 'a') { '23' } else { '24' }", deploy)
        # Entry nodes: base-game shape - radius, z 0, and a {commands} block.
        self.assertIn("$AirEntryRadius = 800", deploy)
        self.assertIn('{radius $AirEntryRadius}', deploy)
        self.assertIn('{0:F2} {1:F2} 0.00', deploy)
        # Exit nodes: the altitude-carrying node shape.
        self.assertIn("$AirCruiseZ = 170.0", deploy)
        self.assertIn("$nx, $ny, $AirCruiseZ", deploy)
        self.assertIn("$AirEntryFactor = 1.15", deploy)
        # And it verifies its own output, exactly once per node per map.
        self.assertIn("Expected exactly one air waypoint $num in", deploy)
        self.assertIn("Expected exactly two air-entry command blocks in", deploy)
        # A map deployed before the renumber still carries the retired band, and one
        # surviving block is a load-time crash - so the deploy both strips it and
        # refuses to sign off on a map that still has it.
        self.assertIn("Map still carries the retired air waypoint band $num", deploy)

    def test_band_stays_inside_the_engine_usable_id_range(self) -> None:
        """A numeric waypoint name is an engine id, and the id space is small.

        9101-9104 crashed every map at load with eHelperWaypoint's "id already used"
        even though each id was written exactly once. Nothing shipped - vanilla or any
        installed workshop mod - names a waypoint above 1000, so the band has to stay
        well inside that. Two digits keeps it inside the range vanilla exercises
        routinely rather than at the edge of what happens to be observed.
        """
        for numeric in self.BAND:
            with self.subTest(name=numeric):
                self.assertLess(int(numeric), 1000)
        for numeric in self.RETIRED_BAND:
            with self.subTest(retired=numeric):
                self.assertNotIn(f'{{waypoint "{numeric}"}}', self.waves)
                self.assertNotIn("'" + numeric + "' } else", self.deploy)

    def test_waypoint_command_block_retags_before_it_orders(self) -> None:
        """A clone carries no tags, so the re-tag has to come first.

        This is the whole reason the base game puts a {commands} block on the
        destination waypoint instead of ordering from the dispatching trigger.
        """
        deploy = self.deploy
        retag = deploy.index("{tag_add support_e2_arrival}")
        actor = deploy.index('{`"actor_state`"', retag)
        action = deploy.index('{`"action`"', actor)
        effect = deploy.index("{effect takeoff_load}", action)
        self.assertLess(retag, actor)
        self.assertLess(actor, action)
        self.assertLess(action, effect)
        ordered = deploy[action:effect]
        self.assertIn("{action move}", ordered)
        self.assertIn("{tag support_e2_flag_target}", ordered)
        self.assertIn("{tag support_e2_arrival}", ordered)
        # The generated block never touches the base airstrike namespace.
        generated = deploy[retag:effect]
        for foreign in ("enemy_air", "airstrike_", "ai_air_target"):
            with self.subTest(tag=foreign):
                self.assertNotIn(foreign, generated)
        self.assertIn("Air-entry command block collides with the base airstrike chain", deploy)


class E2ScopedCloneExceptionTests(unittest.TestCase):
    """{clone} is authorised for E2 aircraft dispatch and for nothing else.

    The general ban exists because a freshly created entity's provenance is invisible
    to every selector this stack can express. The base game solves that for aircraft
    only, by re-tagging the arrival from the destination waypoint's {commands} block.
    Outside that one recipe the ban stands, so it is pinned across all of resource/.
    """

    # Pre-existing {clone} users we ship but did not author: the CE mirror pair, the
    # HF trigger set, and one vendored base map. Frozen as an exact list so a new
    # {clone} anywhere - including inside these - has to be a deliberate edit.
    VENDORED = {
        "resource/map/multi/ce/ce_functions.inc",
        "resource/map_scripts/ce_functions.inc",
        "resource/map/multi/HF-dcg_script.inc",
        "resource/map/multi/dcg_[cwa71]_border/campaign_capture_the_flag.mi",
    }

    def test_clone_appears_only_in_the_e2_aircraft_dispatch(self) -> None:
        offenders = []
        for path in sorted((ROOT / "resource").rglob("*")):
            if not path.is_file() or path.suffix not in (".inc", ".mi", ".set", ".lua"):
                continue
            text = strip_comments(path.read_text(encoding="utf-8", errors="ignore"))
            if "{clone}" not in text:
                continue
            if path == WAVES:
                dispatch = mi_define(text, "e2_clone_aircraft")
                self.assertEqual(text.count("{clone}"), 3)
                self.assertEqual(dispatch.count("{clone}"), 3)
                continue
            offenders.append(path.relative_to(ROOT).as_posix())
        self.assertEqual(sorted(offenders), sorted(self.VENDORED))
        # Nothing this project authors under resource/map/multi may clone.
        for name in (
            "attack_support_waves.inc",
            "defense_support_waves.inc",
            "enemy_attack_support.inc",
            "enemy_defense_support.inc",
            "faction_support_templates.inc",
            "attack_support_templates.inc",
            "enemy_defense_templates.inc",
            "flag_props_templates.inc",
        ):
            live = strip_comments((ROOT / "resource/map/multi" / name).read_text(encoding="utf-8"))
            with self.subTest(engine=name):
                if name == "attack_support_waves.inc":
                    continue
                self.assertNotIn("{clone}", live)

    def test_no_engine_borrows_the_live_base_airstrike_namespace(self) -> None:
        """airstrike_*, enemy_air and ai_air_target are LIVE in the shared script."""
        engines = (
            "attack_support_waves.inc",
            "defense_support_waves.inc",
            "enemy_attack_support.inc",
            "enemy_defense_support.inc",
            "faction_support_templates.inc",
        )
        for name in engines:
            live = strip_comments((ROOT / "resource/map/multi" / name).read_text(encoding="utf-8"))
            for foreign in ("airstrike_", "enemy_air", "ai_air_target"):
                with self.subTest(engine=name, tag=foreign):
                    self.assertNotIn(foreign, live)


class E2FileHygieneTests(unittest.TestCase):
    """MI delimiter balance and the default-off gate, for every file this task touched."""

    TOUCHED = (
        "resource/map/multi/faction_support_templates.inc",
        "resource/map/multi/attack_support_waves.inc",
        "resource/map/multi/dcg_vars.inc",
    )

    def test_delimiters_are_balanced(self) -> None:
        for path in self.TOUCHED:
            code = strip_comments((ROOT / path).read_text(encoding="utf-8"))
            with self.subTest(file=path):
                self.assertEqual(code.count("{"), code.count("}"))
                self.assertEqual(code.count("("), code.count(")"))
        lua = LUA.read_text(encoding="utf-8")
        self.assertEqual(lua.count("("), lua.count(")"))

    def test_everything_new_stays_inside_the_default_off_gate(self) -> None:
        waves = WAVES.read_text(encoding="utf-8")
        init = block(waves, '{"attack_support/init"', '{"attack_support/clock"')
        self.assertIn('{var "support_e2_test$"} {op "="} {value 0}', init)
        self.assertNotIn('{var "support_e2_test$"} {op "="} {value 1}', init)
        self.assertNotIn('{var "support_e2_test$"} {op "="} {value 2}', init)
        self.assertNotIn('{var "support_e2_test$"} {op "="} {value 3}', init)
        for name in ('e2_para_range', 'e2_para_alive'):
            trigger = mi_block(waves, f'{{"attack_support/{name}"')
            condition = trigger.split("{actions", 1)[0]
            with self.subTest(trigger=name):
                self.assertIn('{var "support_e2_test$"} {op "=="} {value 2}', condition)
                self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', condition)
                self.assertNotIn('{var "support_e2_test$"} {op ">"}', condition)
        # The new probes add no budget spend and no new wave command.
        e2 = block(waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== MOTORIZED INSERT")
        for budget in (
            "attack_support_waves_left$",
            "attack_support_air_left$",
            "attack_support_motor_left$",
            "attack_support_ifv_left$",
        ):
            self.assertNotIn(f'{{var "{budget}"}} {{op "-"}}', e2)


class E2SelectorDecorationTests(unittest.TestCase):
    """A {state operatable} decoration on a selector zeroes the match on these units.

    SECOND LIVE PROOF, 2026-07-30. The first proof retired the ADVANCED spelling from
    pool selectors and the deploy has banned it since. The SIMPLE spelling was never
    banned, and it cost a whole run: e2_fly_helo_or_fail carried it, matched nothing on
    a helicopter the player watched fly the full route and hover over the objective, and
    recorded fail 10 one second after the clone arrived. e2_fly_para_or_fail reproduced
    the identical fail 10 from the identical gate in the same run, which is why the para
    leg looked like it had inherited the helo leg's code when it had not.

    The ban is narrow by design: {state operatable} is forbidden only where the same
    selector is also addressing this system's own support_e2_* entities. It stays legal
    where it addresses ordinary live map units, and that remaining scope is pinned here.
    """

    ENGINES = (
        "resource/map/multi/attack_support_waves.inc",
        "resource/map/multi/defense_support_waves.inc",
        "resource/map/multi/enemy_attack_support.inc",
        "resource/map/multi/enemy_defense_support.inc",
    )

    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.deploy = DEPLOY.read_text(encoding="utf-8")

    def test_no_support_e2_selector_carries_the_decoration(self) -> None:
        for path in self.ENGINES:
            code = strip_comments((ROOT / path).read_text(encoding="utf-8"))
            for number, line in enumerate(code.splitlines(), 1):
                at = line.find("{state operatable}")
                if at < 0:
                    continue
                with self.subTest(file=path, line=number):
                    self.assertNotIn("support_e2_", line[:at])

    def test_the_remaining_scope_is_exactly_the_enemy_proximity_guard(self) -> None:
        code = strip_comments(self.waves)
        self.assertEqual(code.count("{state operatable}"), 2)
        choose = mi_define(code, "e2_choose_lz")
        # Both survivors are the LZ safety guard, and both address live bot humans on
        # the map rather than anything this engine claimed or cloned.
        self.assertEqual(choose.count("{state operatable}"), 2)
        for line in choose.splitlines():
            if "{state operatable}" in line:
                self.assertIn("{tag _bot}", line)
                self.assertNotIn("support_e2_", line[: line.index("{state operatable}")])
        for path in self.ENGINES[1:]:
            other = strip_comments((ROOT / path).read_text(encoding="utf-8"))
            with self.subTest(file=path):
                self.assertNotIn("{state operatable}", other)

    def test_every_e2_lifecycle_gate_is_bare_tags_or_the_advanced_exclude(self) -> None:
        code = strip_comments(self.waves)
        for name in ("e2_fly_helo_or_fail", "e2_fly_para_or_fail", "e2_finish_team_or_fail"):
            gate = mi_define(code, name)
            with self.subTest(gate=name):
                self.assertNotIn("{state operatable}", gate)
                self.assertIn("{type entities}", gate)
                self.assertIn('{count {op ">="} {value 1}}', gate)
        # The landed check is a BARE TAG count on support_e2_pax - see
        # test_flight_and_landing_stages_need_a_live_entity for why the corpse filter
        # had to go: it returned zero on four troops the player could see.
        team = mi_define(code, "e2_finish_team_or_fail")
        self.assertIn('{selector {ignore_captured_by_user 0} {tag support_e2_pax}}', team)
        self.assertNotIn("{state ", team)
        # NO STATE DECORATION SURVIVES ON A support_e2_ NEAR CHECK (2026-07-31). The
        # band tracker carried {state "not dead"} on top of an unreadable tag pair and
        # read 0 for an entire overflight. Its units selector is now the bare provenance
        # tag, the one shape attack_support/e2_helo_lz proved live.
        poll = mi_define(code, "e2_para_range_poll")
        self.assertNotIn("{state ", poll)
        self.assertEqual(poll.count("{tag support_e2_arrival}"), 4)
        self.assertNotIn("{tag support_e2_plane}", poll)
        for faction in ("rusa", "ukr", "nato"):
            release = mi_block(code, f'{{"attack_support/e2_para_release_{faction}"')
            with self.subTest(release=faction):
                self.assertNotIn("{state ", release)
                self.assertNotIn("{tag support_e2_plane}", release)
        # e2_para_settle keeps "not dead": its near check addresses CE's own
        # paratrooper_need_orders jumpers, ordinary live map units, not a support_e2_
        # entity - the same carve-out as the {tag _bot} proximity guard.
        settle = mi_define(code, "e2_para_settle")
        self.assertNotIn("{state operatable}", settle)
        self.assertEqual(settle.count('{state "not dead"}'), 2)
        for line in settle.splitlines():
            if '{state "not dead"}' in line:
                self.assertIn("{tag paratrooper_need_orders}", line)
        alive = mi_block(code, '{"attack_support/e2_para_alive"')
        self.assertNotIn("{state operatable}", alive)

    def test_the_deploy_bans_the_simple_form_on_both_sides(self) -> None:
        self.assertIn("SECOND LIVE PROOF (2026-07-30)", self.deploy)
        self.assertIn(
            "decorates a support_e2_ selector with {{state operatable}} on line {1}",
            self.deploy,
        )
        self.assertIn(
            "decorates a support_e2_ selector with {{state operatable}} on line {0}",
            self.deploy,
        )
        # The advanced form stays banned exactly as it was: narrowed, never loosened.
        self.assertIn("'{state {state operatable}}'", self.deploy)


class E2NoOrphanAircraftTests(unittest.TestCase):
    """No evidence gate may leave a dispatched clone flying.

    Live 2026-07-30: the helicopter leg false-failed at dispatch, the action list walked
    away, and the physical clone kept the move order the numeric waypoint's {commands}
    block had given it. It hovered over the objective for the rest of the mission with
    no unload, no departure and no delete - because the departure order and the delete
    both keyed on a claim tag while the whole chain hung off the stage machine.

    Three things make that unreachable now: the departure and the delete each carry a
    provenance-keyed backstop (support_e2_arrival is written only by the waypoint
    {commands} block, so no parked template can ever carry it), every failure path
    issues both, and a standing sweep catches anything that still slips through.
    """

    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")

    def test_delete_and_departure_both_carry_a_provenance_backstop(self) -> None:
        delete = mi_define(self.waves, "e2_delete_aircraft")
        # The claim-keyed sweep is unchanged and still requires BOTH tags, so a parked
        # template can never be deleted by another package's cleanup.
        self.assertIn(
            '{"delete" {selector {ignore_captured_by_user 0} {tag support_e2_aircraft} '
            '{tag support_e2_claim}}}',
            delete,
        )
        self.assertIn(
            '{"delete" {selector {ignore_captured_by_user 0} {tag support_e2_arrival}}}',
            delete,
        )
        self.assertEqual(delete.count('{"delete"'), 2)

        exit_order = mi_define(self.waves, "e2_order_aircraft_exit")
        # One claim-keyed and one arrival-keyed order per side case, all three cases.
        self.assertEqual(exit_order.count('{"action"'), 6)
        self.assertEqual(exit_order.count("{tag support_e2_arrival}"), 3)
        self.assertEqual(exit_order.count("{tag support_e2_claim}"), 3)
        for waypoint in ('{waypoint "23"}', '{waypoint "24"}'):
            self.assertIn(waypoint, exit_order)

    def test_the_arrival_tag_is_only_ever_written_by_the_waypoint(self) -> None:
        """The backstop is only safe because nothing in the pool can carry it."""
        live = strip_comments(self.waves)
        self.assertNotIn("{tag_add support_e2_arrival}", live)
        templates = TEMPLATES.read_text(encoding="utf-8")
        self.assertNotIn("support_e2_arrival", templates)
        deploy = DEPLOY.read_text(encoding="utf-8")
        self.assertIn("{tag_add support_e2_arrival}", deploy)

    def test_every_helo_failure_path_departs_and_deletes(self) -> None:
        for faction in ("rusa", "prc", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_helo_{faction}"')
            refused = launch.split('("e2_fly_helo_or_fail")', 1)[1]
            with self.subTest(helo=faction):
                # The flight leg was refused: the hull is still real and in the air.
                self.assertLess(
                    refused.index('("e2_order_aircraft_exit")'),
                    refused.index('("e2_fail_and_cleanup")'),
                )
        lz = mi_block(self.waves, '{"attack_support/e2_helo_lz"')
        self.assertIn('("e2_descend_helo")', lz)
        self.assertNotIn('("e2_order_aircraft_exit")', lz)
        for name in ("e2_helo_grounded", "e2_helo_timeout"):
            trigger = mi_block(self.waves, f'{{"attack_support/{name}"')
            with self.subTest(trigger=name):
                self.assertIn('("e2_order_aircraft_exit")', trigger)
                self.assertRegex(trigger, r'\("e2_(?:fail_and|complete)_cleanup"\)')
        # And the para legs, both timeout branches each.
        for faction in ("rusa", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_para_{faction}"')
            timeout = launch.split('("e2_fly_para_or_fail")', 1)[1]
            with self.subTest(para=faction):
                self.assertEqual(timeout.count('("e2_order_aircraft_exit")'), 2)
                # Two timeout branches of its own, plus the two enclosing aborts the
                # leg already carried for an unresolved target or an unresolved owner.
                self.assertEqual(timeout.count('("e2_fail_and_cleanup")'), 4)

    def test_a_standing_sweep_catches_anything_left_flying(self) -> None:
        """And ONLY what its own leg has finished with.

        Live 2026-07-30: the sweep fired on support_e2_stage$ >= 70 plus the presence of
        any arrival. Stage reaches 70 on the combo transition, so it deleted a healthy
        in-flight helicopter mid-mission and the player watched it be replaced by the
        paratrooper spawn. A stage number is a statement about the ENGINE; the terminal
        marker is a statement about THIS HULL. The sweep now reads the latter.
        """
        sweep = mi_block(self.waves, '{"attack_support/e2_orphan_sweep"')
        condition, actions = sweep.split("{actions", 1)
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', condition)
        # The global stage variable may never gate this again, in any comparison.
        self.assertNotIn('{var "support_e2_stage$"}', condition)
        # Provenance AND terminality, both required.
        self.assertIn(
            '{"2.entities" {selector {tag support_e2_arrival} {tag support_e2_leg_done}} '
            '{count {op ">="} {value 1}}}',
            condition,
        )
        self.assertNotIn("support_e2_claim", condition)
        self.assertLess(
            actions.index('("e2_order_aircraft_exit")'),
            actions.index('{"delete"'),
        )
        # The delete is as narrow as the condition: never a bare arrival sweep.
        self.assertIn(
            '{"delete" {selector {ignore_captured_by_user 0} {tag support_e2_arrival} '
            '{tag support_e2_leg_done}}}',
            actions,
        )
        # It re-arms, so it stands for the whole mission rather than firing once.
        self.assertIn('{"trigger" {name "attack_support/e2_orphan_sweep"}}', actions)

    def test_the_unload_chain_no_longer_hangs_off_the_stage_machine(self) -> None:
        live = strip_comments(self.waves)
        self.assertNotIn('{"delay" {time 40}}', live)
        self.assertIn('{"delay" {time 40}}', DEPLOY.read_text(encoding="utf-8"))

        lz = mi_block(self.waves, '{"attack_support/e2_helo_lz"')
        condition, actions = lz.split("{actions", 1)
        self.assertIn('{"6.near"', condition)
        self.assertIn("{tag support_e2_lz_marker}", condition)
        self.assertIn("{distance 120}", condition)
        self.assertIn('("e2_descend_helo")', actions)
        self.assertNotIn('("e2_emit_helo_pax")', actions)

        grounded = mi_block(self.waves, '{"attack_support/e2_helo_grounded"')
        g_condition, g_actions = grounded.split("{actions", 1)
        self.assertIn('{tag support_e2_arrival} {tag w81_landing}', g_condition)
        self.assertLess(
            g_actions.index('("e2_emit_helo_pax")'),
            g_actions.index('("e2_promote_helo_pax")'),
        )
        self.assertLess(
            g_actions.index('("e2_promote_helo_pax")'),
            g_actions.index('("e2_finish_team_or_fail")'),
        )
        self.assertLess(
            g_actions.index('("e2_lift_helo")'),
            g_actions.index('("e2_order_aircraft_exit")'),
        )

        for faction in ("rusa", "prc", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_helo_{faction}"')
            with self.subTest(helo=faction):
                self.assertNotIn('("e2_place_one")', launch)
                self.assertNotIn('("e2_place_one_entry")', launch)
                self.assertNotIn('("e2_emit_helo_pax")', launch)


class E2StageEvidenceLedgerTests(unittest.TestCase):
    """Every support_e2_stage$ write, enumerated and justified.

    Live 2026-07-30: the helo leg walked 20 -> 60 -> 70 with fail 10 already set. Stages
    60 and 70 were supposed to be unreachable on that path. A stage number is a claim
    about the world; this is the ledger that says which evidence backs each one.
    """

    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.live = strip_comments(cls.waves)

    def write(self, value: int) -> str:
        return '{"set_i" {var "support_e2_stage$"} {op "="} {value %d}}' % value

    def test_the_ledger_totals_are_exact(self) -> None:
        # 30 is written four times now: the claim-keyed case of each flight gate, the
        # bounded helo leg poll's exhausted-bound branch, and (added 2026-07-31) the
        # LIVE-ARRIVAL branch of e2_fly_para_or_fail, which used to order the plane at
        # the objective and leave the stage at 20 - so the range tracker, all three
        # release monitors and the stage-30 liveness monitor never armed and the C-130
        # flew the whole map with band 0 and pass 0. All four sit behind an entity
        # proof; test_stage_30_is_only_written_behind_aircraft_evidence pins that.
        expected = {0: 2, 10: 1, 20: 7, 30: 4, 35: 1, 40: 4, 50: 1, 60: 11, 70: 2}
        for value, count in sorted(expected.items()):
            with self.subTest(stage=value):
                self.assertEqual(self.live.count(self.write(value)), count)

    def test_stage_20_requires_the_target_entity(self) -> None:
        """Not merely "nothing has failed yet" - the target itself."""
        gate = ('{condition {type entities} {selector {tag support_e2_flag_target}} '
                '{count {op ">="} {value 1}}}')
        self.assertEqual(self.live.count(gate), 7)
        at = 0
        while True:
            at = self.live.find(gate, at)
            if at < 0:
                break
            # The write is the first thing the gate opens onto, every time.
            self.assertIn(self.write(20), self.live[at : at + 200])
            at += 1
        for family, factions in (("helo", ("rusa", "prc", "ukr", "nato")),
                                 ("para", ("rusa", "ukr", "nato"))):
            for faction in factions:
                launch = mi_block(self.waves, f'{{"attack_support/e2_{family}_{faction}"')
                with self.subTest(leg=f"{family}_{faction}"):
                    self.assertLess(
                        launch.index('("e2_choose_flag")'), launch.index(self.write(20))
                    )
                    head = launch[: launch.index(self.write(20))]
                    self.assertIn("{tag support_e2_flag_target}", head)

    def test_stage_30_is_only_written_by_the_two_flight_gates(self) -> None:
        # The helo gate writes 30 once (its claim-keyed case); the para gate writes it
        # twice - the claim-keyed case AND the live-arrival interlock branch, which
        # orders the plane off the provenance tag and must open the leg for the same
        # reason the helo poll's exhausted bound does: a healthy flying aircraft that
        # cannot progress is worse than one that fails.
        for name, writes in (("e2_fly_helo_or_fail", 1), ("e2_fly_para_or_fail", 2)):
            gate = mi_define(self.live, name)
            with self.subTest(gate=name):
                self.assertEqual(gate.count(self.write(30)), writes)
                proof = gate.index("{type entities}")
                self.assertLess(proof, gate.index(self.write(30)))
                # Every write is preceded by an entity proof AND by the order that
                # makes the stage true; neither is a bare timer.
                at = 0
                for _ in range(writes):
                    at = gate.index(self.write(30), at)
                    head = gate[:at]
                    self.assertIn("{type entities}", head)
                    self.assertRegex(head, r'\{"action"|\("e2_order_aircraft_lz"\)')
                    at += 1
        for family, factions in (("helo", ("rusa", "prc", "ukr", "nato")),
                                 ("para", ("rusa", "ukr", "nato"))):
            for faction in factions:
                launch = mi_block(self.live, f'{{"attack_support/e2_{family}_{faction}"')
                with self.subTest(leg=f"{family}_{faction}"):
                    self.assertNotIn(self.write(30), launch)

    def test_stage_30_is_only_written_behind_aircraft_evidence(self) -> None:
        """The third writer is the poll's exhausted-bound branch, and it is EVIDENCE.

        It does not open the leg because the bound ran out; it opens the leg because the
        aircraft is demonstrably alive - the advanced exclude form, on the provenance tag
        the numeric waypoint wrote. Opening on that is the whole point: a healthy flying
        helicopter must be un-failable, so the exhausted bound resolves to "fly", not
        "fail", whenever there is something in the sky to fly.
        """
        poll = mi_block(self.live, '{"attack_support/e2_helo_leg_poll"')
        actions = poll.split("{actions", 1)[1]
        self.assertEqual(actions.count(self.write(30)), 1)
        head = actions[: actions.index(self.write(30))]
        self.assertIn("{select {tag {tag support_e2_arrival}}}", head)
        self.assertIn("{exclude {state {state dead}} {state {state inactive}}}", head)
        self.assertNotIn("{state operatable}", head)
        # And the order goes out before the stage claims the leg is open.
        self.assertIn('("e2_order_aircraft_lz")', head)

    def test_stage_35_and_40_are_grounded_helicopter_evidence(self) -> None:
        lz = mi_block(self.live, '{"attack_support/e2_helo_lz"')
        self.assertEqual(lz.count(self.write(35)), 1)
        self.assertLess(lz.index('{"6.near"'), lz.index(self.write(35)))
        self.assertIn('("e2_descend_helo")', lz)

        grounded = mi_block(self.live, '{"attack_support/e2_helo_grounded"')
        condition = grounded.split("{actions", 1)[0]
        self.assertIn('{tag support_e2_arrival} {tag w81_landing}', condition)
        self.assertIn('{tag support_e2_lz_marker}', condition)
        self.assertEqual(grounded.count(self.write(40)), 1)
        self.assertLess(condition.index('w81_landing'), grounded.index(self.write(40)))
        self.assertLess(grounded.index(self.write(40)), grounded.index('("e2_emit_helo_pax")'))

        # The other three stage-40 writes are the faction-specific para releases.
        for faction in ("rusa", "ukr", "nato"):
            release = mi_block(self.live, f'{{"attack_support/e2_para_release_{faction}"')
            with self.subTest(release=faction):
                self.assertEqual(release.count(self.write(40)), 1)
                self.assertIn('{"5.near"', release.split("{actions", 1)[0])

    def test_stage_50_is_only_written_on_a_live_body(self) -> None:
        team = mi_define(self.live, "e2_finish_team_or_fail")
        self.assertEqual(team.count(self.write(50)), 1)
        self.assertLess(team.index("{type entities}"), team.index(self.write(50)))
        self.assertEqual(self.live.count(self.write(50)), 1)

    def test_stage_60_and_70_are_terminal_and_always_clean_up(self) -> None:
        """60 and 70 are "the leg is over", so each one must end in a cleanup."""
        for value in (60, 70):
            start = 0
            while True:
                at = self.live.find(self.write(value), start)
                if at < 0:
                    break
                start = at + 1
                tail = self.live[at : at + 1200]
                with self.subTest(stage=value, at=at):
                    if value == 70:
                        # Only the two cleanups themselves write 70.
                        self.assertIn("support_e2_stage$", tail)
                    else:
                        self.assertRegex(
                            tail, r'\("e2_(?:fail_and|complete)_cleanup"\)'
                        )
        for name in ("e2_fail_and_cleanup", "e2_complete_cleanup"):
            body = mi_define(self.live, name)
            with self.subTest(cleanup=name):
                self.assertEqual(body.count(self.write(70)), 1)
                self.assertIn('("e2_delete_aircraft")', body)
                # Both clear the one-shot arrival marker, so a second leg can arm.
                self.assertIn("{tag_remove support_e2_lz_done}", body)


class E2ComboFailResetTests(unittest.TestCase):
    """The para leg always starts from support_e2_fail$ == 0.

    Live 2026-07-30: e2_fail stayed 10 for the whole para leg, which made every para
    diagnostic unreadable. The transition DOES clear the code, and the dispatcher clears
    it again - but the transition also requires that no claimed entity remain, and the
    helo leg had orphaned a claimed aircraft. Rather than relax that requirement, the
    leftover is retired first and the transition is re-armed.
    """

    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")

    def test_the_transition_keeps_its_claim_free_requirement(self) -> None:
        transition = mi_block(self.waves, '{"attack_support/e2_combo_transition"')
        condition = transition.split("{actions", 1)[0]
        self.assertIn('{"3.entities" {selector {tag support_e2_claim}}}', condition)
        self.assertIn("!3", condition)

    def test_a_leftover_claim_is_retired_rather_than_ignored(self) -> None:
        clear = mi_block(self.waves, '{"attack_support/e2_combo_clear"')
        condition, actions = clear.split("{actions", 1)
        # The exact complement of the transition's condition: the two can never both
        # be true, so there is no race between them.
        self.assertIn('{var "support_e2_test$"} {op "=="} {value 3}', condition)
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 70}', condition)
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', condition)
        self.assertIn(
            '{"4.entities" {selector {tag support_e2_claim}} {count {op ">="} {value 1}}}',
            condition,
        )
        self.assertNotIn("!", re.search(r'\{expression "([^"]+)"\}', condition).group(1))
        self.assertLess(
            actions.index('("e2_order_aircraft_exit")'),
            actions.index('("e2_delete_aircraft")'),
        )
        self.assertIn("{tag_remove support_e2_claim}", actions)
        self.assertIn("{tag_remove support_e2_arrival}", actions)
        self.assertIn("{tag_remove support_e2_lz_done}", actions)
        self.assertIn('{"trigger" {name "attack_support/e2_combo_transition"}}', actions)

    def test_the_para_leg_cannot_start_with_another_legs_code(self) -> None:
        clear = '{"set_i" {var "support_e2_fail$"} {op "="} {value 0}}'
        dispatch = mi_block(self.waves, '{"attack_support/e2_dispatch"')
        actions = dispatch.split("{actions", 1)[1]
        # The dispatcher clears it before it routes to any child.
        self.assertLess(actions.index(clear), actions.index('{"switch"'))
        # The transition clears it on the way through, before mode 2 is entered.
        transition = mi_block(self.waves, '{"attack_support/e2_combo_transition"')
        t_actions = transition.split("{actions", 1)[1]
        self.assertLess(
            t_actions.index(clear),
            t_actions.index('{var "support_e2_test$"} {op "="} {value 2}'),
        )
        # And the para by-army router asserts it once more, at the last point before a
        # faction child can run.
        router = mi_define(self.waves, "e2_trigger_para_by_army")
        self.assertLess(router.index(clear), router.index('{"switch"'))

    def test_the_new_triggers_stay_inert_on_the_default_off_gate(self) -> None:
        for name, modes in (
            ("e2_helo_lz", ("1", "3")),
            ("e2_helo_timeout", ("1", "3")),
            ("e2_combo_clear", ("3",)),
        ):
            trigger = mi_block(self.waves, f'{{"attack_support/{name}"')
            condition = trigger.split("{actions", 1)[0]
            with self.subTest(trigger=name):
                self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', condition)
                self.assertNotIn('{var "support_e2_test$"} {op ">"}', condition)
                for mode in modes:
                    self.assertIn(
                        '{var "support_e2_test$"} {op "=="} {value %s}' % mode, condition
                    )
        # The orphan sweep carries no test-mode gate on purpose - it exists to clean up
        # after a mode that has already been turned off - but it is now DOUBLY inert on a
        # shipped build: support_e2_arrival is written only by the numeric waypoint's
        # {commands} block, so nothing carries it unless a clone was dispatched, and
        # support_e2_leg_done is written only by e2_mark_leg_done, so nothing carries it
        # unless a leg both ran and ended.
        sweep = mi_block(self.waves, '{"attack_support/e2_orphan_sweep"')
        condition = sweep.split("{actions", 1)[0]
        self.assertIn("{tag support_e2_arrival}", condition)
        self.assertIn("{tag support_e2_leg_done}", condition)
        # Same for the long-stop and the interlock: no test-mode gate, but nothing to
        # match until a clone exists.
        for name in ("e2_arrival_longstop", "e2_fail_interlock"):
            trigger = mi_block(self.waves, f'{{"attack_support/{name}"')
            with self.subTest(trigger=name):
                head = trigger.split("{actions", 1)[0]
                self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', head)
                self.assertIn("{tag support_e2_arrival}", head)

    def test_delimiters_are_balanced_in_every_touched_file(self) -> None:
        for path in (
            "resource/map/multi/attack_support_waves.inc",
            "resource/map/multi/defense_support_waves.inc",
            "resource/map/multi/enemy_attack_support.inc",
            "resource/map/multi/enemy_defense_support.inc",
            "resource/map/multi/dcg_vars.inc",
        ):
            code = strip_comments((ROOT / path).read_text(encoding="utf-8"))
            with self.subTest(file=path):
                self.assertEqual(code.count("{"), code.count("}"))
                self.assertEqual(code.count("("), code.count(")"))
        lua = LUA.read_text(encoding="utf-8")
        self.assertEqual(lua.count("("), lua.count(")"))


class E2HealthyAircraftIsUnfailableTests(unittest.TestCase):
    """A HEALTHY FLYING HELICOPTER MUST BE UN-FAILABLE.

    THIRD LIVE PROOF, 2026-07-30. The player watched the clone spawn and fly - "it was
    on its way" - and the mirror still reported e2_fail 10 and e2_combo_helo_fail 10.
    e2_prove_arrival had already passed, so the arrival existed; what stopped holding was
    the CO-RESIDENCE of support_e2_helo + support_e2_aircraft + support_e2_claim on it,
    across the ("e2_own_arrival") {"player"} {operation set} transfer and the
    {"actor_state"} step that follow the promote. The same file records the same loss
    independently on e2_order_aircraft_exit: the claim-keyed order matched nothing while
    the arrival-keyed order did - and the arrival tag is the one the numeric waypoint's
    own {commands} block writes, before any of this engine's steps run.

    Three defences, all pinned here:
      1. the lifecycle tags are RE-ASSERTED off support_e2_arrival AFTER the transfer;
      2. the gate is a bounded, self-re-arming POLL, not a one-shot sample;
      3. no code that asserts the aircraft's absence (9, 10, 14) may be written while an
         entity carrying support_e2_arrival exists and is not dead.
    """

    LIVE_ARRIVAL = (
        "{select {tag {tag support_e2_arrival}}}",
        "{exclude {state {state dead}} {state {state inactive}}}",
    )

    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.live = strip_comments(cls.waves)

    def test_lifecycle_tags_are_reasserted_after_the_ownership_transfer(self) -> None:
        for name, own in (
            ("e2_reassert_arrival_helo", "support_e2_helo"),
            ("e2_reassert_arrival_plane", "support_e2_plane"),
        ):
            body = mi_define(self.live, name)
            with self.subTest(helper=name):
                # Keyed on the ONE handle that survives the transfer.
                self.assertIn("{selector {tag support_e2_arrival}}", body)
                for tag in (own, "support_e2_aircraft", "support_e2_claim"):
                    self.assertIn("{tag_add %s}" % tag, body)
                # It re-asserts; it never re-adds a pool tag or re-hides the hull.
                self.assertIn("{tag_remove hidden}", body)
                self.assertNotIn("support_e2_tpl", body)
                self.assertNotIn("ally_sup_tpl", body)
        # Every leg re-asserts AFTER ("e2_own_arrival") and AFTER the {"actor_state"}
        # step, which is the window the tags did not survive.
        for family, factions, helper in (
            ("helo", ("rusa", "prc", "ukr", "nato"), "e2_reassert_arrival_helo"),
            ("para", ("rusa", "ukr", "nato"), "e2_reassert_arrival_plane"),
        ):
            for faction in factions:
                launch = mi_block(self.live, f'{{"attack_support/e2_{family}_{faction}"')
                with self.subTest(leg=f"{family}_{faction}"):
                    at = launch.index('("%s")' % helper)
                    self.assertLess(launch.index('("e2_own_arrival")'), at)
                    self.assertLess(launch.index('{"actor_state"'), at)
                    self.assertLess(at, launch.index('("e2_fly_%s_or_fail")' % family))

    def test_the_flight_gate_is_a_bounded_rearming_poll_not_one_sample(self) -> None:
        poll = mi_block(self.live, '{"attack_support/e2_helo_leg_poll"')
        condition, actions = poll.split("{actions", 1)
        # Armed for exactly the window between dispatch and an open flight leg.
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 20}', condition)
        self.assertIn("{tag support_e2_arrival}", condition)
        # Every attempt re-asserts before it re-reads: a sample of stale tags is not
        # evidence, it is the defect.
        self.assertLess(
            actions.index('("e2_reassert_arrival_helo")'),
            actions.index('("e2_fly_helo_or_fail")'),
        )
        # It re-arms itself, and the re-arm is bounded by a counted attempt.
        self.assertIn('{"trigger" {name "attack_support/e2_helo_leg_poll"}}', actions)
        self.assertIn('{"set_i" {var "support_e2_air_try$"} {op "+"} {value 1}}', actions)
        self.assertIn(
            '{condition {type cmp_i} {var "support_e2_air_try$"} {op "<"} {value 20}}',
            actions,
        )
        # Success resets the count, so the bound belongs to one arrival.
        self.assertIn('{"set_i" {var "support_e2_air_try$"} {op "="} {value 0}}', actions)
        # Every faction leg arms it, and none of them still treats the first read as
        # the last word.
        for faction in ("rusa", "prc", "ukr", "nato"):
            launch = mi_block(self.live, f'{{"attack_support/e2_helo_{faction}"')
            with self.subTest(helo=faction):
                self.assertLess(
                    launch.index('("e2_fly_helo_or_fail")'),
                    launch.index('("e2_arm_helo_leg")'),
                )
        arm = mi_define(self.live, "e2_arm_helo_leg")
        self.assertIn('{"trigger" {name "attack_support/e2_helo_leg_poll"}}', arm)

    def test_no_absence_code_is_writable_while_a_live_arrival_exists(self) -> None:
        """9, 10 and 14 all assert 'the aircraft is not there'. A live arrival is the
        counter-example, so every writer of those codes is behind the interlock."""
        for name in ("e2_fly_helo_or_fail", "e2_fly_para_or_fail"):
            gate = mi_define(self.live, name)
            with self.subTest(gate=name):
                write = gate.index('{"set_i" {var "support_e2_fail$"} {op "="} {value 10}}')
                head = gate[:write]
                for fragment in self.LIVE_ARRIVAL:
                    self.assertIn(fragment, head)
                # The liveness question is asked in the advanced exclude form only.
                self.assertNotIn("{state operatable}", gate)
        # The stage-30 liveness monitor cannot even fire while an arrival is alive.
        alive = mi_block(self.live, '{"attack_support/e2_para_alive"')
        condition = alive.split("{actions", 1)[0]
        for fragment in self.LIVE_ARRIVAL:
            self.assertIn(fragment, condition)
        self.assertIn("!5", condition)
        # The bounded poll's own writer resolves an exhausted bound to FLY, not FAIL,
        # whenever there is a live hull: the fail-10 branch is the {"default"} of the
        # liveness case, never its body.
        poll = mi_block(self.live, '{"attack_support/e2_helo_leg_poll"')
        actions = poll.split("{actions", 1)[1]
        live_at = actions.index("{select {tag {tag support_e2_arrival}}}")
        fail_at = actions.index('{"set_i" {var "support_e2_fail$"} {op "="} {value 10}}')
        self.assertLess(live_at, fail_at)
        self.assertLess(actions.index('("e2_order_aircraft_lz")'), fail_at)
        # And the standing interlock retires 9/10/14 wherever else they came from,
        # while the hull is alive and its own leg is not over.
        lock = mi_block(self.live, '{"attack_support/e2_fail_interlock"')
        condition, actions = lock.split("{actions", 1)
        for code in (9, 10, 14):
            self.assertIn(
                '{var "support_e2_fail$"} {op "=="} {value %d}' % code, condition
            )
        # The outcome codes are NOT scrubbed: erasing 4/5/6/7/11 would fabricate a
        # success, which is the exact class of lie the evidence ladder exists to stop.
        for code in (4, 5, 6, 7, 11):
            self.assertNotIn(
                '{var "support_e2_fail$"} {op "=="} {value %d}' % code, condition
            )
        for fragment in self.LIVE_ARRIVAL:
            self.assertIn(fragment, condition)
        self.assertIn("{tag support_e2_leg_done}", condition)
        self.assertIn("!4", condition)
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 0}}', actions)

    def test_every_terminal_path_sets_the_marker_and_only_those(self) -> None:
        marker = mi_define(self.live, "e2_mark_leg_done")
        # One definition, published as a tag on the hull and as a var for the engine.
        self.assertIn(
            "{\"entity_state\" {selector {tag support_e2_arrival}} "
            "{tag_add support_e2_leg_done}}",
            marker,
        )
        self.assertIn('{"set_i" {var "support_e2_leg_done$"} {op "="} {value 1}}', marker)
        # Exactly one writer of either half, anywhere in the engine.
        self.assertEqual(self.live.count("{tag_add support_e2_leg_done}"), 1)
        self.assertEqual(
            self.live.count('{"set_i" {var "support_e2_leg_done$"} {op "="} {value 1}}'), 1
        )
        # Every path that genuinely ends a leg calls it.
        for name in ("e2_fail_and_cleanup", "e2_complete_cleanup"):
            body = mi_define(self.live, name)
            with self.subTest(cleanup=name):
                self.assertIn('("e2_mark_leg_done")', body)
                # Marked BEFORE the delete, so a hull that survives the delete is left
                # carrying its own terminal marker for the sweep to find.
                self.assertLess(
                    body.index('("e2_mark_leg_done")'),
                    body.index('("e2_delete_aircraft")'),
                )
        for name in ("e2_combo_clear", "e2_arrival_longstop"):
            trigger = mi_block(self.live, f'{{"attack_support/{name}"')
            with self.subTest(terminal=name):
                self.assertIn('("e2_mark_leg_done")', trigger.split("{actions", 1)[1])
        # A transition is NOT a terminal path and may never mark one.
        transition = mi_block(self.live, '{"attack_support/e2_combo_transition"')
        self.assertNotIn('("e2_mark_leg_done")', transition)
        # And the marker is cleared for the next leg, in both places a leg can start.
        clear = '{"set_i" {var "support_e2_leg_done$"} {op "="} {value 0}}'
        self.assertIn(clear, mi_block(self.live, '{"attack_support/e2_dispatch"'))
        self.assertIn(clear, transition)
        self.assertIn(clear, block(self.live, '{"attack_support/init"', '{"attack_support/clock"'))

    def test_the_longstop_is_keyed_to_the_leg_not_to_the_stage_variable(self) -> None:
        stop = mi_block(self.live, '{"attack_support/e2_arrival_longstop"')
        condition, actions = stop.split("{actions", 1)
        # Never the global stage var - that is what deleted a healthy aircraft.
        self.assertNotIn('{var "support_e2_stage$"}', condition)
        self.assertNotIn('{var "support_e2_stage$"}', actions)
        # It watches an arrival whose own leg is NOT over.
        self.assertIn(
            '{"2.entities" {selector {tag support_e2_arrival}} '
            '{count {op ">="} {value 1}}}',
            condition,
        )
        self.assertIn(
            '{"3.entities" {selector {tag support_e2_arrival} {tag support_e2_leg_done}} '
            '{count {op ">="} {value 1}}}',
            condition,
        )
        self.assertIn("!3", condition)
        # Consecutive observations only: a leg that ends resets the count.
        self.assertIn('{"set_i" {var "support_e2_air_age$"} {op "+"} {value 1}}', actions)
        self.assertIn('{"set_i" {var "support_e2_air_age$"} {op "="} {value 0}}', actions)
        self.assertIn(
            '{condition {type cmp_i} {var "support_e2_air_age$"} {op ">="} {value 12}}',
            actions,
        )
        # Self-re-arming, so the guarantee stands for the whole mission.
        self.assertIn('{"trigger" {name "attack_support/e2_arrival_longstop"}}', actions)

    def test_no_new_simple_selector_state_decoration_on_an_e2_selector(self) -> None:
        """The ban that this whole round of false failures earned, re-asserted over the
        new code: liveness is the advanced exclude form and nothing else."""
        for number, line in enumerate(self.live.splitlines(), 1):
            at = line.find("{state operatable}")
            if at < 0:
                continue
            with self.subTest(line=number):
                self.assertNotIn("support_e2_", line[:at])
        # The two survivors are still the LZ enemy-proximity guard and nothing else.
        self.assertEqual(self.live.count("{state operatable}"), 2)
        for name in (
            "e2_helo_leg_poll",
            "e2_fail_interlock",
            "e2_arrival_longstop",
            "e2_orphan_sweep",
            "e2_clock",
        ):
            trigger = mi_block(self.live, f'{{"attack_support/{name}"')
            with self.subTest(trigger=name):
                self.assertNotIn("{state operatable}", trigger)
                self.assertNotIn('{state "not dead"}', trigger)
        for name in ("e2_reassert_arrival_helo", "e2_reassert_arrival_plane",
                     "e2_mark_leg_done", "e2_arm_helo_leg"):
            body = mi_define(self.live, name)
            with self.subTest(helper=name):
                self.assertNotIn("{state operatable}", body)

    def test_the_new_state_is_declared_and_mirrored(self) -> None:
        declared = VARS.read_text(encoding="utf-8")
        lua = LUA.read_text(encoding="utf-8")
        for var in (
            "support_e2_leg_done",
            "support_e2_air_try",
            "support_e2_air_age",
            "support_e2_clock_t",
        ):
            with self.subTest(var=var):
                self.assertIn('{"%s"}' % var, declared)
                self.assertIn('readVar("%s")' % var, lua)
                # SetVar takes integers only: no var-to-var copy, ever.
                self.assertNotIn('{var "%s$"} {op "="} {var ' % var, self.live)

    def test_delimiters_are_balanced_in_every_file_this_task_touched(self) -> None:
        for path in (
            "resource/map/multi/attack_support_waves.inc",
            "resource/map/multi/dcg_vars.inc",
        ):
            code = strip_comments((ROOT / path).read_text(encoding="utf-8"))
            with self.subTest(file=path):
                self.assertEqual(code.count("{"), code.count("}"))
                self.assertEqual(code.count("("), code.count(")"))
        lua = LUA.read_text(encoding="utf-8")
        self.assertEqual(lua.count("("), lua.count(")"))


class E2ParaDispatchFloorTests(unittest.TestCase):
    """The para leg dispatches at the 3-minute mark and never preempts a live helo leg.

    Two terms, both tightening. The helo leg must have reached a TERMINAL state - the
    same one definition of "over" the orphan sweep reads off the hull - and match time
    must have reached 180 seconds. The clock is a FLOOR on the para dispatch, never a
    ceiling on the helo leg: if the helo leg is still flying at 180s, the para leg waits.
    """

    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.live = strip_comments(cls.waves)

    def test_the_transition_requires_both_terminal_helo_and_the_180s_floor(self) -> None:
        transition = mi_block(self.live, '{"attack_support/e2_combo_transition"')
        condition = transition.split("{actions", 1)[0]
        expression = re.search(r'\{expression "([^"]+)"\}', condition).group(1)
        self.assertEqual(expression, "1 & 2 & !3 & 4 & 5 & 6")
        # The two pins that were already there are untouched, not relaxed.
        self.assertIn('{"3.entities" {selector {tag support_e2_claim}}}', condition)
        self.assertIn('{"4.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}', condition)
        # AND the helo leg is over, by the one definition of over.
        self.assertIn(
            '{"5.cmp_i" {var "support_e2_leg_done$"} {op "=="} {value 1}}', condition
        )
        # AND at least 180 seconds of match time have passed.
        self.assertIn(
            '{"6.cmp_i" {var "support_e2_clock_t$"} {op ">="} {value 18}}', condition
        )

    def test_the_clock_is_a_floor_and_never_a_ceiling(self) -> None:
        """Nothing keyed on the clock may interrupt, shorten or fail a running leg."""
        for name in (
            "e2_helo_lz",
            "e2_helo_timeout",
            "e2_helo_leg_poll",
            "e2_orphan_sweep",
            "e2_arrival_longstop",
            "e2_combo_clear",
        ):
            trigger = mi_block(self.live, f'{{"attack_support/{name}"')
            with self.subTest(trigger=name):
                self.assertNotIn("support_e2_clock_t$", trigger)
        for faction in ("rusa", "prc", "ukr", "nato"):
            launch = mi_block(self.live, f'{{"attack_support/e2_helo_{faction}"')
            with self.subTest(helo=faction):
                self.assertNotIn("support_e2_clock_t$", launch)
        # Exactly one reader in the whole engine: the transition's floor.
        self.assertEqual(self.live.count('{var "support_e2_clock_t$"} {op ">="}'), 1)

    def test_match_time_is_a_counted_integer_step_not_a_float_timer(self) -> None:
        clock = mi_block(self.live, '{"attack_support/e2_clock"')
        condition, actions = clock.split("{actions", 1)
        # Armed once, by the same latch that arms the wave engine, and inert on defence.
        self.assertIn('{var "attack_support_armed$"} {op "=="} {value 1}', condition)
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', condition)
        # One step per 10 real seconds, so 18 steps is 180 seconds. Integer only.
        self.assertIn('{"delay" {time 10}}', actions)
        self.assertIn('{"set_i" {var "support_e2_clock_t$"} {op "+"} {value 1}}', actions)
        self.assertNotIn('{var "support_e2_clock_t$"} {op "="} {var ', actions)
        # Self-re-arming, the same shape as attack_support/clock and e2_para_range.
        self.assertIn('{"trigger" {name "attack_support/e2_clock"}}', actions)
        # Armed and zeroed from init, so the clock measures the match and not a leg.
        init = block(self.live, '{"attack_support/init"', '{"attack_support/clock"')
        self.assertIn('{"set_i" {var "support_e2_clock_t$"} {op "="} {value 0}}', init)
        self.assertIn('{"trigger" {name "attack_support/e2_clock"}}', init)
        # No leg may reset it: that would make the floor measure the leg, not the match.
        for family, factions in (("helo", ("rusa", "prc", "ukr", "nato")),
                                 ("para", ("rusa", "ukr", "nato"))):
            for faction in factions:
                launch = mi_block(self.live, f'{{"attack_support/e2_{family}_{faction}"')
                with self.subTest(leg=f"{family}_{faction}"):
                    self.assertNotIn("support_e2_clock_t$", launch)
        self.assertNotIn(
            "support_e2_clock_t$", mi_block(self.live, '{"attack_support/e2_dispatch"')
        )

    def test_the_whole_e2_system_is_still_inert_at_test_zero(self) -> None:
        """support_e2_test$ defaults to 0 and nothing new changes that."""
        init = block(self.live, '{"attack_support/init"', '{"attack_support/clock"')
        self.assertIn('{var "support_e2_test$"} {op "="} {value 0}', init)
        for value in (1, 2, 3):
            self.assertNotIn(
                '{var "support_e2_test$"} {op "="} {value %d}' % value, init
            )
        # The two legs and their monitors stay mode-gated.
        for name in ("e2_helo_leg_poll",):
            condition = mi_block(self.live, f'{{"attack_support/{name}"').split("{actions", 1)[0]
            with self.subTest(trigger=name):
                self.assertIn('{var "support_e2_test$"} {op "=="} {value 1}', condition)
                self.assertIn('{var "support_e2_test$"} {op "=="} {value 3}', condition)
                self.assertNotIn('{var "support_e2_test$"} {op ">"}', condition)
        # The three ungated standing guards match only entities the clone dispatch
        # itself created, so at test 0 - where no clone is ever dispatched - none of
        # them can act. The match clock writes one var nothing reads until a leg runs.
        for name in ("e2_orphan_sweep", "e2_arrival_longstop", "e2_fail_interlock"):
            condition = mi_block(self.live, f'{{"attack_support/{name}"').split("{actions", 1)[0]
            with self.subTest(trigger=name):
                self.assertIn("{tag support_e2_arrival}", condition)
        clock = mi_block(self.live, '{"attack_support/e2_clock"')
        actions = clock.split("{actions", 1)[1]
        self.assertNotIn("support_e2_stage$", actions)
        self.assertNotIn("support_e2_fail$", actions)
        self.assertNotIn("{tag_add", actions)
        self.assertNotIn('{"delete"', actions)


class E2PassengerOwnershipTests(unittest.TestCase):
    """The four inserted troops belong to the support player, and are counted as such.

    LIVE 2026-07-31, fields, NATO attack, combo mode. The helicopter leg ran end to end
    - flew, arrived, emitted, exited, deleted, all watched - and left FOUR UNAFFILIATED
    TROOPS at the LZ: white minimap dots, nobody's units. The same leg walked stage
    30 -> 60 recording fail 11, "the insert landed no live body", about bodies the
    player was looking at.

    One defect, two faces. The insert's only ownership step was e2_own_current, which
    runs at stage 20 - when the four bodies are still HIDDEN, INACTIVE parked templates.
    Every delivery path in this engine that has never produced a neutral body does the
    opposite: am_finish_deploy and as_finish_motor both place, then strip
    {hidden}/{inactive}, and only THEN transfer. And the landed check counted
    support_e2_team through {exclude {state {state dead}} {state {state inactive}}} -
    a state decoration on a support_e2_ selector, the class that has already zeroed two
    E2 matches on units that were demonstrably alive.

    support_e2_pax closes both: each completed delivery path writes it only after a
    body exists outside the parked pool, it carries the literal 1-16 transfer, and it
    is what the landed check counts.
    """

    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.live = strip_comments(cls.waves)
        cls.deploy = DEPLOY.read_text(encoding="utf-8")

    def test_the_pax_tag_is_written_only_on_completed_delivery_paths(self) -> None:
        self.assertEqual(self.live.count("{tag_add support_e2_pax}"), 4)
        for name in ("e2_place_one", "e2_place_one_entry"):
            place = mi_define(self.live, name)
            with self.subTest(place=name):
                self.assertEqual(place.count("{tag_add support_e2_pax}"), 1)
                promote = mi_block(
                    place[place.index("{tag_add support_e2_pax}") - 400 :], '{"entity_state"'
                )
                self.assertIn("{tag_remove hidden}", promote)
                self.assertIn("{inactive off}", promote)
                self.assertLess(place.index('{"placement"'), place.index("{tag_add support_e2_pax}"))

        helo = mi_define(self.live, "e2_promote_helo_pax")
        self.assertEqual(helo.count("{tag_add support_e2_pax}"), 1)
        self.assertIn("{select {tag {tag support_e2_helo_pax}}}", helo)
        self.assertIn("{exclude {tag {tag hidden}}}", helo)
        self.assertIn("{tag_remove support_e2_helo_pax}", helo)

        para_start = self.live.index('{"attack_support/e2_para_takeover"')
        para_end = self.live.index('{"attack_support/e2_paradrop_link_0"', para_start)
        para = self.live[para_start:para_end]
        self.assertEqual(para.count("{tag_add support_e2_pax}"), 1)
        self.assertIn("{tag paratrooper_need_orders}", para)
        self.assertIn("{state {state linked}}", para)
        self.assertIn("{state {state inactive}}", para)
        self.assertIn("{state {state dead}}", para)
        self.assertIn('("e2_own_pax")', para)
        self.assertIn('("e2_order_team")', para)

    def test_the_pax_tag_carries_the_literal_1_to_16_switch_and_fails_closed(self) -> None:
        own = mi_define(self.live, "e2_own_pax")
        for player in range(1, 17):
            with self.subTest(player=player):
                self.assertIn(
                    '{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} '
                    '{value %d}} {"player" {selector {ignore_captured_by_user 0} '
                    '{tag support_e2_pax}} {operation set} {player "%d"}}}' % (player, player),
                    own,
                )
        # No range comparison, no var-to-var fold, no computed player id.
        self.assertEqual(own.count('{op "=="}'), 16)
        self.assertNotIn('{op ">"}', own)
        self.assertNotIn('{op ">="}', own)
        # Fail-closed: the default transfers nothing and records the unresolved id.
        default = own.split('{"default"', 1)[1]
        self.assertNotIn('{player "', default)
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 8}}', default)
        # SetVar integers only.
        self.assertNotRegex(own, r'\{op "="\} \{var ')

    def test_no_e2_body_is_delivered_without_an_ownership_transfer(self) -> None:
        finish = mi_define(self.live, "e2_finish_team_or_fail")
        self.assertEqual(finish.split("\n")[1].strip(), '("e2_own_pax")')
        self.assertLess(finish.index('("e2_own_pax")'), finish.index('("e2_order_team")'))

        grounded = mi_block(self.live, '{"attack_support/e2_helo_grounded"')
        for marker in ('("e2_emit_helo_pax")', '("e2_promote_helo_pax")',
                       '("e2_finish_team_or_fail")', '("e2_lift_helo")'):
            self.assertIn(marker, grounded)
        self.assertLess(grounded.index('("e2_emit_helo_pax")'),
                        grounded.index('("e2_promote_helo_pax")'))
        self.assertLess(grounded.index('("e2_promote_helo_pax")'),
                        grounded.index('("e2_finish_team_or_fail")'))
        self.assertLess(grounded.index('("e2_finish_team_or_fail")'),
                        grounded.index('("e2_lift_helo")'))

        # Unsafe LZs fail closed; linked riders are never detached into a fake insert.
        for faction in ("rusa", "prc", "ukr", "nato"):
            launch = mi_block(self.live, f'{{"attack_support/e2_helo_{faction}"')
            with self.subTest(helo=faction):
                self.assertNotIn('("e2_place_one")', launch)
                self.assertNotIn('("e2_place_one_entry")', launch)
                self.assertNotIn('("e2_finish_team_or_fail")', launch)

    def test_the_landed_check_counts_the_same_tag_and_fail_11_stays_reachable(self) -> None:
        finish = mi_define(self.live, "e2_finish_team_or_fail")
        self.assertIn('{selector {ignore_captured_by_user 0} {tag support_e2_pax}}', finish)
        self.assertIn('{count {op ">="} {value 1}}', finish)
        # BARE TAG. No state decoration in any spelling: the advanced exclude form is
        # what returned zero on four visible troops.
        self.assertNotIn("{state ", finish)
        self.assertNotIn("{source advanced}", finish)
        # Fail 11 is still the outcome when nothing landed, and an earlier code wins.
        default = finish.split('{"default"', 1)[1]
        self.assertIn('{var "support_e2_fail$"} {op "=="} {value 0}', default)
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 11}}', default)
        self.assertLess(
            default.index('{op "=="} {value 0}'),
            default.index('{"set_i" {var "support_e2_fail$"} {op "="} {value 11}}'),
        )
        # Stage 50 is behind the count, never beside it.
        self.assertLess(
            finish.index('{count {op ">="} {value 1}}'),
            finish.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 50}}'),
        )

    def test_the_pax_tag_is_ordered_and_retired_with_the_leg(self) -> None:
        order = mi_define(self.live, "e2_order_team")
        # AI control and the advance order both address the delivered bodies.
        self.assertEqual(order.count("{tag support_e2_pax}"), 2)
        self.assertNotIn("{tag support_e2_team}", order)
        self.assertIn("{control AI}", order)
        self.assertIn("{action advance}", order)
        self.assertIn("{tag support_e2_flag_target}", order)
        # Both cleanups strip it, so a second leg starts from an empty pax set.
        for name in ("e2_fail_and_cleanup", "e2_complete_cleanup"):
            body = mi_define(self.live, name)
            with self.subTest(cleanup=name):
                self.assertIn(
                    '{"entity_state" {selector {tag support_e2_pax}} '
                    '{tag_remove support_e2_pax}}',
                    body,
                )
        self.assertEqual(self.live.count("{tag_remove support_e2_pax}"), 2)
        # The pool tag support_e2_<faction>_para_pax is a different string and none of
        # this touches it.
        self.assertNotIn("{tag_add support_e2_para_pax}", self.live)

    def test_the_deploy_pins_the_pax_machinery(self) -> None:
        for marker in ("'{tag_add support_e2_pax}'", '\'(define "e2_own_pax"\'',
                       "'(\"e2_own_pax\")'"):
            self.assertIn(marker, self.deploy)


class E2ParaExitEvidenceTests(unittest.TestCase):
    """The para leg may not close without a release or a reason, and may not vanish.

    LIVE 2026-07-31, test 2. Stage 10 -> 20 at 04:53 (the C-130 dispatched and the
    player watched it fly), -> 60 at 06:25, -> 70, with e2_para_band 0 and
    e2_para_pass 0 for the WHOLE flight and fail 0 at the end. Nobody jumped, the plane
    disappeared in mid-map, and the ledger read like a clean run.

    Three faults, all closed here:
      1. e2_fly_para_or_fail's live-arrival branch ordered the plane at the objective
         and left the stage at 20. The range tracker, all three release monitors and the
         stage-30 liveness monitor are every one of them gated on stage 30, so none of
         them ever armed - which is exactly what band 0 / pass 0 for a whole overflight
         looks like from the log.
      2. The stage-20 exit case wrote stage 60 and no code at all.
      3. The exit allowed 10 seconds between "fly off the map" and the delete.
    """

    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.live = strip_comments(cls.waves)
        cls.deploy = DEPLOY.read_text(encoding="utf-8")
        # The section markers are comments, so the slice is taken on the raw text and
        # the comments are stripped afterwards.
        cls.para = strip_comments(
            block(cls.waves, "; ===== E2 PARADROP", "; ===== MOTORIZED INSERT")
        )

    def test_the_run_in_opens_on_a_live_arrival_instead_of_stalling_at_stage_20(self) -> None:
        gate = mi_define(self.live, "e2_fly_para_or_fail")
        interlock = gate.split('{"default"', 1)[1]
        # The live-arrival branch: advanced exclude form on the provenance tag, an order
        # off that same tag, and then the stage.
        self.assertIn("{select {tag {tag support_e2_arrival}}}", interlock)
        self.assertIn("{exclude {state {state dead}} {state {state inactive}}}", interlock)
        order = interlock.index('{"action"')
        stage = interlock.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 30}}')
        self.assertLess(interlock.index("{select {tag {tag support_e2_arrival}}}"), order)
        self.assertLess(order, stage)
        # Fail 10 stays reachable, and only from the branch with no live arrival.
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 10}}', interlock)
        self.assertLess(
            stage, interlock.index('{"set_i" {var "support_e2_fail$"} {op "="} {value 10}}')
        )

    def test_every_para_exit_is_evidence_gated(self) -> None:
        gate = mi_define(self.live, "e2_para_require_release_or_fail")
        # Either a recorded release...
        self.assertIn(
            '{condition {type entities} {selector {tag support_e2_released}} '
            '{count {op ">="} {value 1}}}',
            gate,
        )
        # ...or a code, and an earlier code still wins over 6.
        self.assertIn('{var "support_e2_fail$"} {op "=="} {value 0}', gate)
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 6}}', gate)
        self.assertLess(
            gate.index('{op "=="} {value 0}'),
            gate.index('{"set_i" {var "support_e2_fail$"} {op "="} {value 6}}'),
        )
        # It never writes a stage of its own: the caller keeps that, one term later.
        self.assertNotIn("support_e2_stage$", gate)
        # Called once per exit case: three factions, two cases each.
        self.assertEqual(self.para.count('("e2_para_require_release_or_fail")'), 6)

    def test_no_para_stage_60_is_written_without_evidence(self) -> None:
        """Enumerate every stage-60 write on the para path and name its witness."""
        write60 = '{"set_i" {var "support_e2_stage$"} {op "="} {value 60}}'
        sites = []
        at = 0
        while True:
            at = self.para.find(write60, at)
            if at < 0:
                break
            sites.append(at)
            at += 1
        # 6 dispatch exits (3 factions x 2 cases), 3 in e2_para_settle (its two
        # jumper-near proofs and its fail-7 branch), 1 in e2_para_alive.
        self.assertEqual(len(sites), 10)
        for at in sites:
            head = self.para[max(0, at - 900) : at]
            with self.subTest(at=at):
                self.assertRegex(
                    head,
                    r'\("e2_para_require_release_or_fail"\)'
                    r'|\{tag paratrooper_need_orders\}'
                    r'|\{"set_i" \{var "support_e2_fail\$"\} \{op "="\} \{value 7\}\}'
                    r'|\{"set_i" \{var "support_e2_fail\$"\} \{op "="\} \{value 9\}\}',
                )
        # And stage 70 is only ever the two cleanups, both of which are reached only
        # from a gated 60 or from a fail path.
        write70 = '{"set_i" {var "support_e2_stage$"} {op "="} {value 70}}'
        self.assertNotIn(write70, self.para)
        self.assertEqual(self.live.count(write70), 2)

    def test_fail_6_and_fail_7_both_stay_reachable(self) -> None:
        self.assertIn(
            '{"set_i" {var "support_e2_fail$"} {op "="} {value 6}}',
            mi_define(self.live, "e2_para_require_release_or_fail"),
        )
        settle = mi_define(self.live, "e2_para_settle")
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 7}}', settle)
        # 7 is the no-survivor branch and it routes to the failure cleanup, never the
        # completion one.
        tail = settle[settle.index('{"set_i" {var "support_e2_fail$"} {op "="} {value 7}}') :]
        self.assertIn('("e2_fail_and_cleanup")', tail)
        self.assertNotIn('("e2_complete_cleanup")', tail)

    def test_the_para_exit_routes_off_map_before_any_delete(self) -> None:
        """Order to the numeric exit waypoint first, then delete after the transit.

        The C-130 vanished in mid-map because the exit allowed 10 seconds. The transit
        allowance is now the helo leg's 60 - the one the player watched a helicopter
        complete on the same run.
        """
        exit_order = mi_define(self.live, "e2_order_aircraft_exit")
        for numeric in ('{waypoint "23"}', '{waypoint "24"}'):
            self.assertIn(numeric, exit_order)
        for faction in ("rusa", "ukr", "nato"):
            launch = mi_block(self.live, f'{{"attack_support/e2_para_{faction}"')
            timeout = launch.split('("e2_fly_para_or_fail")', 1)[1]
            with self.subTest(para=faction):
                self.assertEqual(timeout.count('("e2_order_aircraft_exit")'), 2)
                self.assertEqual(timeout.count('{"delay" {time 60}}'), 2)
                self.assertNotIn('{"delay" {time 10}}', timeout)
                for case in timeout.split('{"case"')[1:]:
                    if '("e2_order_aircraft_exit")' not in case:
                        continue
                    self.assertLess(
                        case.index('("e2_order_aircraft_exit")'),
                        case.index('{"delay" {time 60}}'),
                    )
                    self.assertLess(
                        case.index('{"delay" {time 60}}'), case.index('("e2_fail_and_cleanup")')
                    )
        # The release path already orders the exit before e2_para_settle's 90s delete.
        for faction in ("rusa", "ukr", "nato"):
            release = mi_block(self.live, f'{{"attack_support/e2_para_release_{faction}"')
            actions = release.split("{actions", 1)[1]
            with self.subTest(release=faction):
                self.assertLess(
                    actions.index('("e2_order_aircraft_exit")'),
                    actions.index('("e2_para_settle")'),
                )
        settle = mi_define(self.live, "e2_para_settle")
        self.assertLess(
            settle.index('{"delay" {time 90}}'), settle.index('("e2_delete_aircraft")')
        )
        # And the last-resort sweep gets the same transit allowance, not 20 seconds.
        sweep = mi_block(self.live, '{"attack_support/e2_orphan_sweep"')
        actions = sweep.split("{actions", 1)[1]
        self.assertLess(
            actions.index('("e2_order_aircraft_exit")'), actions.index('{"delay" {time 60}}')
        )
        self.assertLess(actions.index('{"delay" {time 60}}'), actions.index('{"delete"'))
        self.assertNotIn('{"delay" {time 20}}', actions)

    def test_the_band_metric_names_a_reference_that_exists_at_sample_time(self) -> None:
        """Provenance tag for the aircraft, flag-target tag for the reference.

        The old metric asked for support_e2_plane AND support_e2_claim on one entity -
        the co-residence this file already records as not surviving the ownership
        transfer, and which the 2026-07-31 run re-proved from the other side when
        e2_fly_para_or_fail's claim-keyed case matched nothing on a flying C-130. It
        also carried a {state "not dead"} decoration. Both are gone; the shape is now
        the one attack_support/e2_helo_lz term 6 fired on live.
        """
        poll = mi_define(self.live, "e2_para_range_poll")
        self.assertEqual(poll.count("{type near}"), 4)
        self.assertEqual(
            poll.count('{units {ignore_captured_by_user 0} {tag support_e2_arrival}}'), 4
        )
        self.assertEqual(
            poll.count('{near_to {ignore_captured_by_user 0} {tag support_e2_flag_target}}'), 4
        )
        self.assertNotIn("{tag support_e2_plane}", poll)
        self.assertNotIn("{tag support_e2_claim}", poll)
        self.assertNotIn("{state ", poll)
        # The reference is written by e2_choose_flag, is a precondition of stage 20 on
        # every para leg, and is only ever cleared by the reset and the two cleanups -
        # so it provably exists for the whole of stage 30, which is when the poll runs.
        self.assertIn(
            "{tag_add support_e2_flag_target}", mi_define(self.live, "e2_choose_flag")
        )
        self.assertEqual(self.live.count("{tag_remove support_e2_flag_target}"), 3)
        for name in ("e2_reset_target", "e2_fail_and_cleanup", "e2_complete_cleanup"):
            with self.subTest(clears=name):
                self.assertIn(
                    "{tag_remove support_e2_flag_target}", mi_define(self.live, name)
                )
        # And the same live shape drives the helo unload, which is where it was proven.
        lz = mi_block(self.live, '{"attack_support/e2_helo_lz"').split("{actions", 1)[0]
        self.assertIn('{units {ignore_captured_by_user 0} {tag support_e2_arrival}}', lz)

    def test_the_band_rings_are_in_map_decimetres(self) -> None:
        """1000/2000/3000/4000 units = 100/200/300/400 m on an ~11400 x 6200 map.

        Map coordinates are DECIMETRES - this repo's own live-tuned finding, the same
        one that corrected the motorised band. The outer ring is a fifth of the short
        axis of dcg_[cwa71]_fields, so any run-in that crosses the objective at all
        enters it; band 0 therefore means "never came near", which is what fail 6 says.
        """
        poll = mi_define(self.live, "e2_para_range_poll")
        rings = [int(v) for v in re.findall(r"\{distance (\d+)\}", poll)]
        self.assertEqual(rings, [1000, 2000, 3000, 4000])
        # Tightest first, so the first matching case is the true band.
        self.assertEqual(rings, sorted(rings))
        # The release monitors' own rings sit inside the tracker's outer ring.
        for faction in ("rusa", "ukr", "nato"):
            condition = mi_block(
                self.live, f'{{"attack_support/e2_para_release_{faction}"'
            ).split("{actions", 1)[0]
            with self.subTest(release=faction):
                self.assertEqual(
                    sorted(int(v) for v in re.findall(r"\{distance (\d+)\}", condition)),
                    [1500],
                )
        # The old 2500-unit shell remains retired. The telemetry poll still owns
        # 600/4000, while 1500 is now the release radius.
        self.assertNotIn("{distance 2500}", self.para)
        self.assertIn("'{distance 2500}'", self.deploy)
        self.assertNotIn("{distance 600}", self.para)
        self.assertIn("{distance 4000}", self.para)
        self.assertIn("{distance 1500}", self.para)

    def test_the_release_monitors_address_the_dispatched_clone(self) -> None:
        for faction in ("rusa", "ukr", "nato"):
            release = mi_block(self.live, f'{{"attack_support/e2_para_release_{faction}"')
            condition, actions = release.split("{actions", 1)
            with self.subTest(release=faction):
                # One-shot, on the provenance tag, excluded once released.
                self.assertIn(
                    '{selector {source advanced} {group {select {tag {tag support_e2_arrival}}} '
                    '{exclude {tag {tag support_e2_released}}}}}',
                    condition,
                )
                self.assertIn(
                    '{"entity_state" {selector {tag support_e2_arrival}} '
                    '{tag_add support_e2_released}}',
                    actions,
                )
                # The release is ONE effect on the provenance handle, never on the parked
                # original - which must survive for the next call-in. The {"emit"} that
                # used to follow it is gone: it force-unloaded the seats before
                # {on animation_end "cargo_open"} could run drop_desant1/2/3, and a
                # passenger who leaves by emit rather than by the seat unlink never
                # reaches the parachute path at all.
                self.assertIn(
                    '{"effect" {selector {source advanced} {group '
                    '{select {tag {tag support_e2_arrival}}}}} {effect drop_paratrooper}}',
                    actions,
                )
                self.assertNotIn('{"emit"', actions)
                self.assertNotIn("{mode passengers}", actions)
                self.assertNotIn("support_e2_tpl", actions)

    def test_the_deploy_pins_the_para_exit_gate(self) -> None:
        for marker in ('\'(define "e2_para_require_release_or_fail"\'',
                       "'(\"e2_para_require_release_or_fail\")'"):
            self.assertIn(marker, self.deploy)

    def test_delimiters_are_balanced_in_every_file_this_pass_touched(self) -> None:
        for path in (
            "resource/map/multi/attack_support_waves.inc",
            "resource/map/multi/dcg_vars.inc",
            "resource/map/multi/faction_support_templates.inc",
        ):
            code = strip_comments((ROOT / path).read_text(encoding="utf-8"))
            with self.subTest(file=path):
                self.assertEqual(code.count("{"), code.count("}"))
                self.assertEqual(code.count("("), code.count(")"))

    def test_the_whole_pass_stays_inert_at_test_mode_zero(self) -> None:
        """Nothing added here can act unless a leg is actually running."""
        init = block(self.live, '{"attack_support/init"', '{"attack_support/clock"')
        self.assertIn('{var "support_e2_test$"} {op "="} {value 0}', init)
        for value in (1, 2, 3):
            self.assertNotIn('{var "support_e2_test$"} {op "="} {value %d}' % value, init)
        # The two new defines are called only from inside gated triggers - they are
        # never armed as triggers of their own.
        for name in ("e2_own_pax", "e2_para_require_release_or_fail"):
            with self.subTest(define=name):
                self.assertNotIn('{"attack_support/%s"' % name, self.live)
                self.assertNotIn('{"trigger" {name "attack_support/%s"}}' % name, self.live)
        # e2_para_require_release_or_fail only reads state - it touches nothing.
        gate = mi_define(self.live, "e2_para_require_release_or_fail")
        self.assertNotIn("{tag_add", gate)
        self.assertNotIn("{tag_remove", gate)
        self.assertNotIn('{"delete"', gate)
        self.assertNotIn('{"placement"', gate)
        # Every para trigger is still mode-gated on test 2.
        for faction in ("rusa", "ukr", "nato"):
            for family in ("e2_para_%s", "e2_para_release_%s"):
                condition = mi_block(
                    self.live, '{"attack_support/%s"' % (family % faction)
                ).split("{actions", 1)[0]
                with self.subTest(trigger=family % faction):
                    self.assertIn('{var "support_e2_test$"} {op "=="} {value 2}', condition)
