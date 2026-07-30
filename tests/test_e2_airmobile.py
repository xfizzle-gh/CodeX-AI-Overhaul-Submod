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


def block(text: str, start: str, end: str) -> str:
    return text.split(start, 1)[1].split(end, 1)[0]


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
        ids = re.findall(r'\{(?:Entity|Human) "[^"]+" (0xb4[0-9a-f]{2})', self.tpl)
        mids = [int(v) for v in re.findall(r"\{MID (98\d\d)\}", self.tpl)]
        self.assertEqual(ids, [f"0x{n:x}" for n in range(0xB401, 0xB430)])
        self.assertEqual(mids, list(range(9800, 9847)))
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
        for marker in ("must park 502 prototypes", "support_e2_test", "support_e2_para_pax", "ce_ai_logic_triggers.inc"):
            self.assertIn(marker, self.deploy)

class E2CeIsolationTests(unittest.TestCase):
    def test_ce_mirrors_are_byte_identical(self) -> None:
        self.assertEqual(CE_MAP.read_bytes(), CE_SCRIPT.read_bytes())

    def test_paratrooper_order_selector_excludes_e2_at_selection_time(self) -> None:
        text = CE_MAP.read_text(encoding="utf-8")
        order_block = block(text, '{"ai_logic/paratrooper_orders"', '{"ai_logic/')
        selector = block(order_block, '{selector', '{sort')
        exclude = selector.split('{exclude', 1)[1]
        self.assertIn('{tag paratrooper_need_orders}', selector)
        self.assertRegex(exclude, r"\{tag\s+\{tag support_e2_para_pax\}")
        self.assertEqual(order_block.count("support_e2_para_pax"), 1)
