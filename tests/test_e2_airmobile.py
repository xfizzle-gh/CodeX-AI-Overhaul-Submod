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
        # Altitude is snapshot state now; the engine no longer issues an air_state
        # altitude command at all.
        self.assertNotIn("{altitude 65}", self.waves)
        self.assertNotIn('{"air_state"', self.waves)
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
            # mi17_b8_rus / mi17_b8_ukr / mi17_b8_rus (NATO): driver, commander
            ("0xb402", "0xb401", "driver"),
            ("0xb403", "0xb401", "commander"),
            ("0xb409", "0xb408", "driver"),
            ("0xb40a", "0xb408", "commander"),
            ("0xb410", "0xb40f", "driver"),
            ("0xb411", "0xb40f", "commander"),
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
        # PRC Mi-171 adaptation, same two crew places.
        self.assertIn('{Link 0xc201 {0xc200 "driver"}}', self.tpl)
        self.assertIn('{Link 0xc202 {0xc200 "commander"}}', self.tpl)
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
        for marker in ("must park 633 prototypes", "support_e2_test", "support_e2_para_pax", "ce_ai_logic_triggers.inc"):
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
        self.assertNotIn('{"air_state"', e2)
        self.assertNotIn("{altitude 30}", e2)
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
        self.assertEqual(clone.count('{waypoint "9101"}'), 1)
        self.assertEqual(clone.count('{waypoint "9102"}'), 2)
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
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== E2 PARADROP")
        self.assertIn('(define "e2_place_one"', e2)
        self.assertIn('{"delay" {time 0.5}}', e2)
        self.assertIn('{action advance}', e2)
        self.assertIn('{tag support_e2_flag_target}', e2)
        self.assertIn('{amount 1}', e2)
        # The LZ unload is now ONE shared trigger armed off the aircraft's arrival rather
        # than four inline copies, so the four placements live there and nowhere else.
        # Each faction leg still keeps its own four entry-standoff placements for fail 4.
        lz = mi_block(self.waves, '{"attack_support/e2_helo_lz"')
        self.assertEqual(lz.count('("e2_place_one")'), 4)
        self.assertEqual(e2.count('("e2_place_one")'), 4)
        self.assertEqual(e2.count('("e2_place_one_entry")'), 16)

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
        """The unload chain hangs off the AIRCRAFT, not off the stage machine.

        Live 2026-07-30: the dispatch-time evidence gate false-failed (fail 10) on a
        helicopter the player watched fly across the map. The chain used to hang off a
        {"delay" {time 40}} inside the same action list, so the leg abandoned itself -
        and the physical clone kept the move order the waypoint {commands} block had
        given it and hovered over the objective for the rest of the mission, with no
        unload, no departure and no delete. The near check is now a trigger CONDITION
        on the arrived, re-tagged aircraft.
        """
        lz = mi_block(self.waves, '{"attack_support/e2_helo_lz"')
        condition, actions = lz.split("{actions", 1)
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', condition)
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 30}', condition)
        self.assertIn('{units', condition)
        self.assertIn('{tag support_e2_helo}', condition)
        self.assertIn('{near_to', condition)
        self.assertIn('{tag support_e2_lz_marker}', condition)
        self.assertIn('{distance 120}', condition)
        # The gate that zeroed the match on a live aircraft is gone from the near check.
        self.assertNotIn('{state operatable}', condition)
        # One-shot per leg: the marker is added before anything else happens.
        self.assertIn('{tag {tag support_e2_lz_done}}', condition)
        self.assertLess(
            actions.index('{tag_add support_e2_lz_done}'),
            actions.index('("e2_announce_helo")'),
        )
        stage40 = actions.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}')
        place = actions.index('("e2_place_one")')
        exit_at = actions.index('("e2_order_aircraft_exit")')
        stage60 = actions.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 60}}')
        self.assertLess(stage40, place)
        self.assertLess(place, exit_at)
        self.assertLess(exit_at, stage60)
        self.assertIn('("e2_complete_cleanup")', actions)

        # The timeout owns fail 5, and it too always departs and deletes.
        timeout = mi_block(self.waves, '{"attack_support/e2_helo_timeout"')
        t_condition, t_actions = timeout.split("{actions", 1)
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 30}', t_condition)
        self.assertIn('{var "support_e2_fail$"} {op "="} {value 5}', t_actions)
        self.assertIn('("e2_order_aircraft_exit")', t_actions)
        self.assertEqual(t_actions.count('("e2_fail_and_cleanup")'), 1)
        self.assertNotIn('("e2_place_one")', t_actions)
        self.assertNotIn('("e2_place_one_entry")', t_actions)

        # And the refusal branch in each faction leg departs and deletes rather than
        # walking away from a hull that is physically in the air.
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
            self.assertEqual(fail4.count('("e2_place_one_entry")'), 4)
            self.assertEqual(fail4.count('("e2_complete_cleanup")'), 1)
            self.assertNotIn('("e2_delete_aircraft")', fail4)
            self.assertNotIn('("e2_fail_and_cleanup")', fail4)
            # Stage 40 claims "bodies are being delivered" and needs a claimed team.
            stage40 = fail4.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}')
            gate = fail4[:stage40]
            self.assertIn('{type entities}', gate)
            self.assertIn('{tag support_e2_team}', gate)

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
            self.assertIn('{tag support_e2_plane}', condition)
            self.assertIn('{state "not dead"}', condition)
            self.assertNotIn('{state operatable}', condition)
            self.assertIn('{near_to', condition)
            self.assertIn('{tag support_e2_flag_target}', condition)
            # Widened band: 600-4000, a 3400-unit annulus. The old 1500-2500 shell was
            # 150-250m out and the airborne chassis crosses it in under 4 seconds - or
            # misses it entirely on an off-axis run-in. Both old values are gone.
            self.assertEqual(condition.count('{distance 4000}'), 1)
            self.assertEqual(condition.count('{distance 600}'), 1)
            self.assertNotIn('{distance 2500}', condition)
            self.assertNotIn('{distance 1500}', condition)
            self.assertRegex(condition, r'\{expression "[^"]*!\d+[^"]*"\}')
            # Term 8 is the bounded closest-approach fallback, ORed against the inner
            # exclusion so a run-in that steps into the hole or parks overhead still
            # drops. It never bypasses the outer ring, which is ANDed in on its own.
            self.assertIn(
                '{"8.cmp_i" {var "support_e2_para_pass$"} {op "=="} {value 1}}', condition
            )
            expression = re.search(r'\{expression "([^"]+)"\}', condition).group(1)
            self.assertEqual(expression, "1 & 2 & 3 & 4 & 5 & 7 & (8 | !6)")
            self.assertIn('{tag support_e2_released}', condition)
            actions = release.split("{actions", 1)[1]
            tagged = actions.index('{tag_add support_e2_released}')
            effect = actions.index('{effect drop_paratrooper}')
            emit = actions.index('{"emit"', effect)
            stage40 = actions.index('{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}')
            self.assertLess(tagged, effect)
            self.assertLess(effect, emit)
            self.assertLess(emit, stage40)
            # The effect only opens the cargo bay; the emit verb is what unlinks the
            # seats. Both are addressed at the claimed arrival, never at the parked
            # original, which must survive for the next call-in.
            emit_block = mi_block(actions[emit:], '{"emit"')
            self.assertIn("{tag support_e2_plane}", emit_block)
            self.assertIn("{tag support_e2_claim}", emit_block)
            self.assertIn("{type vehicle}", emit_block)
            self.assertIn("{state inhabited}", emit_block)
            self.assertIn("{mode passengers}", emit_block)
            # The pax-tagging route is gone with the claim: a clone's jumpers carry
            # none of this engine's tags, so nothing may pretend to address them.
            self.assertNotIn("support_e2_para_pax", actions)
        # drop_paratrooper (singular) is the receiver Code:X declares for both
        # airframes; drop_paratroopers (plural) belongs to a different prop group.
        self.assertEqual(self.e2.count('{effect drop_paratrooper}'), 3)
        self.assertNotIn('{effect drop_paratroopers}', self.e2)
        self.assertEqual(strip_comments(self.e2).count("{mode passengers}"), 3)

    def test_missed_release_is_fail6_and_cannot_place_passengers(self) -> None:
        for faction in ("rusa", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_para_{faction}"')
            timeout = launch.split('("e2_fly_para_or_fail")', 1)[1]
            self.assertRegex(timeout, r'\{"delay" \{time (?:60|75|90)\}\}')
            self.assertIn('{var "support_e2_stage$"} {op "=="} {value 30}', timeout)
            self.assertIn('{var "support_e2_fail$"} {op "="} {value 6}', timeout)
            self.assertIn('("e2_order_aircraft_exit")', timeout)
            self.assertIn('("e2_fail_and_cleanup")', timeout)
            # And the leg cannot hang at stage 20 when the gate refused the run-in:
            # that branch exits the plane and cleans up, preserving fail 10.
            self.assertIn('{var "support_e2_stage$"} {op "=="} {value 20}', timeout)
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
        # An earlier code wins: 14 must never be relabelled as 10.
        default = fly.split('{"default"', 1)[1]
        self.assertIn('{var "support_e2_fail$"} {op "=="} {value 0}', default)
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 10}}', default)
        self.assertLess(
            default.index('{op "=="} {value 0}'),
            default.index('{"set_i" {var "support_e2_fail$"} {op "="} {value 10}}'),
        )

        team = mi_define(self.waves, "e2_finish_team_or_fail")
        self.assertIn("{tag support_e2_team}", team)
        # Corpses genuinely have to be excluded here, so this is the ADVANCED group form
        # the rest of the file already uses, never a simple-selector decoration.
        self.assertNotIn("{state operatable}", team)
        self.assertIn("{source advanced}", team)
        self.assertIn("{select {tag {tag support_e2_team}}}", team)
        self.assertIn("{exclude {state {state dead}} {state {state inactive}}}", team)
        self.assertIn('("e2_order_team")', team)
        self.assertIn('{"set_i" {var "support_e2_stage$"} {op "="} {value 50}}', team)
        self.assertIn('{"set_i" {var "support_e2_fail$"} {op "="} {value 11}}', team)

        # No helo trigger may set stage 30 or 50 outside those gates any more.
        for faction in ("rusa", "prc", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_helo_{faction}"')
            with self.subTest(helo=faction):
                self.assertIn('("e2_fly_helo_or_fail")', launch)
                # One left in the leg: the fail-4 entry standoff. The LZ unload's copy
                # moved to attack_support/e2_helo_lz with the rest of the chain.
                self.assertEqual(launch.count('("e2_finish_team_or_fail")'), 1)
                self.assertNotIn(
                    '{"set_i" {var "support_e2_stage$"} {op "="} {value 30}}', launch
                )
                self.assertNotIn(
                    '{"set_i" {var "support_e2_stage$"} {op "="} {value 50}}', launch
                )

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
        self.assertEqual(timeout.count(needle), 1)
        guard = timeout[: timeout.index(needle)]
        guard = guard[guard.rindex('{"case"') :]
        self.assertIn(
            '{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}',
            guard,
        )


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
        expression = re.search(r'\{expression "([^"]+)"\}', condition).group(1)
        self.assertEqual(expression, "1 & 2 & 3 & !4")
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
                self.assertIn('{var "support_e2_fail$"} {op "="} {value 6}', timeout)
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
    """The 9101-9104 air waypoint band: free, deploy-generated, and command-carrying.

    {"actor_to_waypoint"} and the {waypoint} order term accept NUMERIC names only, so
    the air nodes cannot use the attack_support_* naming the pads use. That makes a
    collision sweep mandatory rather than cosmetic: the base game's own airstrike
    geometry already occupies "0" on all fourteen managed maps and "1".."6" on two of
    them, complete with its own enemy_air {commands} blocks.
    """

    BAND = ("9101", "9102", "9103", "9104")
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
        # "0" is the base-game roam/exit node the motorised insert already uses.
        self.assertEqual(numeric_targets, {"0"} | set(self.BAND))

    def test_deploy_generates_the_band_idempotently_and_self_healing(self) -> None:
        deploy = self.deploy
        # Strip-then-rebuild, with a brace matcher because the entry nodes carry a
        # nested {commands} block the flat pad regex cannot reach.
        self.assertIn("function Remove-NamedWaypointBlock", deploy)
        self.assertIn(
            "foreach ($num in @('9101', '9102', '9103', '9104')) {\n"
            "        $text = Remove-NamedWaypointBlock -Text $text -Name $num\n"
            "    }",
            deploy,
        )
        self.assertIn("$entryNum = if ($side -eq 'a') { '9101' } else { '9102' }", deploy)
        self.assertIn("$exitNum = if ($side -eq 'a') { '9103' } else { '9104' }", deploy)
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
        # The one gate that genuinely has to exclude corpses uses the advanced group.
        team = mi_define(code, "e2_finish_team_or_fail")
        self.assertIn("{exclude {state {state dead}} {state {state inactive}}}", team)
        # The near checks keep their corpse exclusion in the proven "not dead" spelling.
        for name in ("e2_para_range_poll", "e2_para_settle"):
            probe = mi_define(code, name)
            with self.subTest(probe=name):
                self.assertNotIn("{state operatable}", probe)
                self.assertIn('{state "not dead"}', probe)
        for faction in ("rusa", "ukr", "nato"):
            release = mi_block(code, f'{{"attack_support/e2_para_release_{faction}"')
            with self.subTest(release=faction):
                self.assertNotIn("{state operatable}", release)
                self.assertIn('{state "not dead"}', release)
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
        for waypoint in ('{waypoint "9103"}', '{waypoint "9104"}'):
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
        for name in ("e2_helo_lz", "e2_helo_timeout"):
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
        sweep = mi_block(self.waves, '{"attack_support/e2_orphan_sweep"')
        condition, actions = sweep.split("{actions", 1)
        self.assertIn('{var "user_is_defender$"} {op "=="} {value 0}', condition)
        # Armed once the leg is over, on the provenance tag rather than on a claim.
        self.assertIn('{var "support_e2_stage$"} {op ">="} {value 70}', condition)
        self.assertIn("{tag support_e2_arrival}", condition)
        self.assertNotIn("support_e2_claim", condition)
        self.assertLess(
            actions.index('("e2_order_aircraft_exit")'),
            actions.index('{"delete"'),
        )
        self.assertIn("{tag support_e2_arrival}", actions)
        # It re-arms, so it stands for the whole mission rather than firing once.
        self.assertIn('{"trigger" {name "attack_support/e2_orphan_sweep"}}', actions)

    def test_the_unload_chain_no_longer_hangs_off_the_stage_machine(self) -> None:
        live = strip_comments(self.waves)
        # The fixed arrival window inside the dispatch action list is gone for good,
        # and the deploy refuses to ship it back.
        self.assertNotIn('{"delay" {time 40}}', live)
        self.assertIn('{"delay" {time 40}}', DEPLOY.read_text(encoding="utf-8"))
        # The unload is one trigger, whose CONDITION is the near check on the arrival.
        lz = mi_block(self.waves, '{"attack_support/e2_helo_lz"')
        condition = lz.split("{actions", 1)[0]
        self.assertIn('{"6.near"', condition)
        self.assertIn("{tag support_e2_lz_marker}", condition)
        self.assertIn("{distance 120}", condition)
        # No faction leg may place at the LZ any more; they only keep the fail-4
        # entry standoff, which never involves an aircraft.
        for faction in ("rusa", "prc", "ukr", "nato"):
            launch = mi_block(self.waves, f'{{"attack_support/e2_helo_{faction}"')
            with self.subTest(helo=faction):
                self.assertNotIn('("e2_place_one")', launch)
                self.assertEqual(launch.count('("e2_place_one_entry")'), 4)


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
        expected = {0: 2, 10: 1, 20: 7, 30: 2, 40: 8, 50: 1, 60: 11, 70: 2}
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
        for name in ("e2_fly_helo_or_fail", "e2_fly_para_or_fail"):
            gate = mi_define(self.live, name)
            with self.subTest(gate=name):
                self.assertEqual(gate.count(self.write(30)), 1)
                proof = gate.index("{type entities}")
                self.assertLess(proof, gate.index(self.write(30)))
        for family, factions in (("helo", ("rusa", "prc", "ukr", "nato")),
                                 ("para", ("rusa", "ukr", "nato"))):
            for faction in factions:
                launch = mi_block(self.live, f'{{"attack_support/e2_{family}_{faction}"')
                with self.subTest(leg=f"{family}_{faction}"):
                    self.assertNotIn(self.write(30), launch)

    def test_stage_40_always_has_something_to_deliver(self) -> None:
        # 4 entry standoffs, each gated on a claimed team; 1 LZ unload, gated on the
        # aircraft actually being at the LZ; 3 paradrop releases, gated on the band.
        lz = mi_block(self.live, '{"attack_support/e2_helo_lz"')
        self.assertEqual(lz.count(self.write(40)), 1)
        self.assertIn("{tag support_e2_lz_marker}", lz.split("{actions", 1)[0])
        for faction in ("rusa", "prc", "ukr", "nato"):
            launch = mi_block(self.live, f'{{"attack_support/e2_helo_{faction}"')
            with self.subTest(helo=faction):
                self.assertEqual(launch.count(self.write(40)), 1)
                head = launch[: launch.index(self.write(40))]
                gate = head[head.rindex('{"case"'):]
                self.assertIn("{type entities}", gate)
                self.assertIn("{tag support_e2_team}", gate)
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
        # after a mode that has already been turned off - but it can only ever match an
        # entity the clone dispatch itself created, and stage 0 is not >= 70, so it is
        # inert on a shipped build where nothing was ever dispatched.
        sweep = mi_block(self.waves, '{"attack_support/e2_orphan_sweep"')
        condition = sweep.split("{actions", 1)[0]
        self.assertIn('{var "support_e2_stage$"} {op ">="} {value 70}', condition)
        self.assertIn("{tag support_e2_arrival}", condition)

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
