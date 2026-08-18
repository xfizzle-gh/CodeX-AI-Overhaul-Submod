from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MAP_NAMES = [
    "dcg_[cwa71]_airbase",
    "dcg_[cwa71]_border",
    "dcg_[cwa71]_europe",
    "dcg_[cwa71]_factory",
    "dcg_[cwa71]_fields",
    "dcg_[cwa71]_fulda",
    "dcg_[cwa71]_grassland",
    "dcg_[cwa71]_industrial",
    "dcg_[cwa71]_monastery",
    "dcg_[cwa71]_outback",
    "dcg_[cwa71]_stasis",
    "dcg_[cwa71]_train_station",
    "dcg_[cwa71]_winds_valley",
    "dcg_[cwa71]_woodland",
]
TRIGGERS = ROOT / "resource/map/multi/ce/ce_morale_classification_triggers.inc"
MOD = ROOT / "resource/map/multi/ce/morale_system.mod"
HELPERS = ROOT / "resource/map/multi/ce/ce_morale_helpers.inc"
DCG = ROOT / "resource/map/multi/dcg_script.inc"
VARS = ROOT / "resource/map/multi/ce/ce_vars.inc"
LUA = ROOT / "resource/script/multiplayer/modes/utility_ce.lua"
CFG = ROOT / "resource/conquest_configuration/bot.conquest_configuration.lua"


class CeMoraleRuntimeGateTests(unittest.TestCase):
    def test_live_ce_stack_includes_classification_module(self) -> None:
        dcg = DCG.read_text(encoding="utf-8")
        self.assertIn('ce_morale_classification_triggers.inc', dcg)
        self.assertIn('ce_morale_marker_apply_triggers.inc', dcg)
        self.assertIn('{"enable_ce_morale_mechanic"}', VARS.read_text(encoding="utf-8"))
        self.assertIn("enableCeMoraleMechanic", CFG.read_text(encoding="utf-8"))
        self.assertIn("enable_ce_morale_mechanic", LUA.read_text(encoding="utf-8"))
        self.assertIn("CE_MORALE_PROBE", LUA.read_text(encoding="utf-8"))
        self.assertIn("ce_morale_probe_report.inc", dcg)
        self.assertIn("ce_morale_diag_triggers.inc", dcg)
        self.assertIn("CE_MORALE_DIAG", LUA.read_text(encoding="utf-8"))
        diag = (ROOT / "resource/map/multi/ce/ce_morale_diag_triggers.inc").read_text(encoding="utf-8")
        self.assertIn("ce_morale_diag_mi_alive$", diag)
        self.assertIn("ce_morale_diag_human$", diag)
        self.assertIn("aio_morale_diag_roundtrip", diag)
        self.assertIn("CE_MORALE_ARCH", LUA.read_text(encoding="utf-8"))
        self.assertIn("ce_morale_diag_add_action_ran$", diag)
        self.assertIn("ce_morale_diag_added_tag_read$", diag)
        self.assertNotIn("ignore_captured_by_user", diag)
        self.assertNotIn("{tag soldier}", diag)
        self.assertNotIn("aio_morale_broken", diag)

    def test_modifier_file_matches_locked_penalties(self) -> None:
        text = MOD.read_text(encoding="utf-8")
        self.assertIn("{name aio_morale_shaken}", text)
        self.assertIn("{name aio_morale_panic}", text)
        self.assertIn("{tag aio_morale_shaken}", text)
        self.assertIn("{tag aio_morale_panic}", text)
        self.assertIn("{scale 0.75}", text)
        self.assertIn("{scale 0.5}", text)
        self.assertIn("{scale 0.8}", text)
        self.assertNotIn("{tag shaken}", text)
        self.assertNotIn("{tag panic}", text)

    def test_pr_b_has_no_automatic_escalation_or_broken(self) -> None:
        text = TRIGGERS.read_text(encoding="utf-8")
        self.assertIn("classify_existing", text)
        self.assertIn("classify_default_regular", text)
        self.assertIn("cleanup_dead", text)
        self.assertIn("force_shaken", text)
        self.assertIn("force_panic", text)
        self.assertIn("observe_source_profile", text)
        self.assertIn("observe_source_quality", text)
        self.assertIn("observe_late", text)
        self.assertIn("autodemo_start", text)
        self.assertIn("ce_morale_probe_source_fail", text)
        self.assertNotIn("aio_morale_broken", text)
        self.assertNotIn("aio_morale_owned", text)
        self.assertNotIn("aio_morale_surrendering", text)
        self.assertNotIn("suppressed", text)

    def test_autodemo_probe_is_fail_closed(self) -> None:
        text = TRIGGERS.read_text(encoding="utf-8")
        default = text.split("classify_default_regular", 1)[1].split("cleanup_dead", 1)[0]
        self.assertIn('enable_ce_morale_autodemo$', default)
        self.assertIn("{value 0}", default)
        force = text.split("morale/force_shaken", 1)[1].split("morale/force_panic", 1)[0]
        self.assertIn("{tag aio_morale_trained}", force)
        self.assertIn("{tag aio_morale_elite}", force)
        self.assertIn("{tag aio_morale_fallback}", force)
        self.assertNotIn("{tag aio_morale_classified}", force)
        start = text.split("morale/autodemo_start", 1)[1]
        self.assertIn("ce_morale_source_quality_seen$", start)
        self.assertIn("ce_morale_probe_source_fail", start)

    def test_cwa_maps_load_morale_modifiers(self) -> None:
        self.assertTrue(HELPERS.is_file())
        for name in MAP_NAMES:
            mission = ROOT / "resource/map/multi" / name / "campaign_capture_the_flag.mi"
            text = mission.read_text(encoding="utf-8", errors="replace")
            self.assertIn("/map/multi/ce/ce_morale_helpers.inc", text, name)


if __name__ == "__main__":
    unittest.main()
