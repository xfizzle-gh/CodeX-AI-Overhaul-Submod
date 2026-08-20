from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DIAG = ROOT / "resource/map/multi/ce/ce_morale_diag_triggers.inc"
DCG = ROOT / "resource/map/multi/dcg_script.inc"
LUA = ROOT / "resource/script/multiplayer/modes/utility_ce.lua"
VARS = ROOT / "resource/map/multi/ce/ce_vars.inc"
MARKER = ROOT / "resource/set/stuff/special/aio_morale_marker"
CANARIES = [
    ROOT / "resource/set/breed/mp/rusa/2022s/lud_rifleman.set",
    ROOT / "resource/set/breed/mp/rusa/era2022/lud_rifleman.set",
    ROOT / "resource/set/breed/mp/ukr/era2022/ukr_rifleman.set",
    ROOT / "resource/set/breed/mp/ukr/2022s/ter_rifleman.set",
    ROOT / "resource/set/breed/mp/ukr/era2022/ter_rifleman.set",
    ROOT / "resource/set/breed/mp/ukr/era1960/ter_rifleman.set",
    ROOT / "resource/set/breed/mp/rusa/2022s/rus90_recon.set",
    ROOT / "resource/set/breed/mp/rusa/era2022/rus90_recon.set",
]


class CeMoraleArchDiagTests(unittest.TestCase):
    def test_live_stack_loads_arch_diag(self) -> None:
        dcg = DCG.read_text(encoding="utf-8")
        self.assertIn("ce_morale_diag_triggers.inc", dcg)
        self.assertIn("ce_morale_marker_apply_triggers.inc", dcg)
        lua = LUA.read_text(encoding="utf-8")
        self.assertIn("CE_MORALE_ARCH", lua)
        self.assertIn("TAG_ADD_FAIL", lua)
        self.assertIn("TAG_READ_FAIL", lua)
        self.assertIn("PR_A_SOURCE_FAIL", lua)
        self.assertIn("INVENTORY_CANARY_FAIL", lua)
        self.assertIn("CANARY_ABSENT", lua)
        self.assertIn("canary_present=", lua)
        self.assertIn("SHAKEN_APPLY_FAIL", lua)
        self.assertIn("PANIC_APPLY_FAIL", lua)
        vars_text = VARS.read_text(encoding="utf-8")
        for name in (
            "ce_morale_diag_known_tag",
            "ce_morale_diag_pr_a_source",
            "ce_morale_diag_canary_present",
            "ce_morale_diag_inventory_canary",
            "ce_morale_diag_shaken",
            "ce_morale_diag_panic",
            "ce_morale_diag_player_hit",
        ):
            self.assertIn(name, vars_text)

    def test_diag_covers_independent_architecture_bits(self) -> None:
        text = DIAG.read_text(encoding="utf-8")
        self.assertIn("diag_mi_alive", text)
        self.assertIn("{prop human}", text)
        self.assertIn("aio_morale_diag_roundtrip", text)
        self.assertIn("{state user_control}", text)
        self.assertIn("{tag aio_morale_low}", text)
        self.assertIn("{tag aio_morale_regular}", text)
        self.assertIn("{tag aio_morale_trained}", text)
        self.assertIn("{tag aio_morale_elite}", text)
        self.assertIn("{tag aio_morale_fallback}", text)
        self.assertIn('{item "aio_morale_marker"}', text)
        self.assertIn('{item "secret_doc_bag2"}', text)
        self.assertIn("diag_canary_present", text)
        self.assertIn("{type existance}", text)
        self.assertIn("{tag_add aio_morale_shaken}", text)
        self.assertIn("{tag_add aio_morale_panic}", text)
        self.assertNotIn("ignore_captured_by_user", text)
        self.assertNotIn("{tag soldier}", text)
        self.assertNotIn("aio_morale_broken", text)
        apply = (ROOT / "resource/map/multi/ce/ce_morale_marker_apply_triggers.inc").read_text(encoding="utf-8")
        self.assertIn('{item "aio_marker_morale_low"}', apply)
        self.assertIn("{tag_add aio_morale_low}", apply)
        self.assertIn("{action remove}", apply)
        shaken = text.split("diag_shaken_apply", 1)[1].split("diag_panic_apply", 1)[0]
        self.assertIn("{state user_control}", shaken)
        self.assertIn("{tag player}", shaken)
        source = text.split("diag_pr_a_source", 1)[1].split("diag_inventory_canary", 1)[0]
        self.assertIn("aio_morale_fallback", source)

    def test_inventory_marker_copies_fe_hidden_item(self) -> None:
        text = MARKER.read_text(encoding="utf-8")
        self.assertIn("{entity \"secret_doc_bag2\"}", text)
        self.assertIn("{fsm \"stuff\"}", text)
        self.assertIn("{size 1 1}", text)

    def test_only_canary_breeds_carry_hidden_marker(self) -> None:
        for path in CANARIES:
            self.assertTrue(path.is_file(), path)
            text = path.read_text(encoding="utf-8")
            self.assertIn('{item "aio_morale_marker"}', text)
            self.assertIn('{item "secret_doc_bag2"}', text)
        marked = [
            path
            for path in (ROOT / "resource/set/breed").rglob("*.set")
            if "isolation_test" not in path.parts and "generated_pow" not in path.parts
            if '{item "aio_morale_marker"}' in path.read_text(encoding="utf-8")
        ]
        self.assertEqual(sorted(marked), sorted(CANARIES))


if __name__ == "__main__":
    unittest.main()
