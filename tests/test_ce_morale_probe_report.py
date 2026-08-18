from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
TRIGGER_INDEX = ROOT / "resource/map/multi/ce/ce_triggers.inc"
REPORT = ROOT / "resource/map/multi/ce/ce_morale_probe_report.inc"


class CeMoraleProbeReportTests(unittest.TestCase):
    def test_reporter_is_in_live_trigger_stack(self) -> None:
        index = TRIGGER_INDEX.read_text(encoding="utf-8")
        self.assertIn('ce_morale_probe_report.inc', index)
        self.assertTrue(REPORT.is_file())

    def test_reporter_has_mutually_exclusive_final_paths(self) -> None:
        text = REPORT.read_text(encoding="utf-8")
        for name in (
            "report_source_fail",
            "report_regular_only",
            "report_late_fail",
            "report_pass",
        ):
            self.assertIn(name, text)
        for var in (
            "ce_morale_autodemo_done$",
            "ce_morale_source_tag_seen$",
            "ce_morale_source_quality_seen$",
            "ce_morale_late_seen$",
        ):
            self.assertIn(var, text)

    def test_reporter_repeats_existing_visible_result_messages(self) -> None:
        text = REPORT.read_text(encoding="utf-8")
        self.assertIn("mission/multi/ce_morale_probe_source_fail", text)
        self.assertIn("mission/multi/ce_morale_probe_source_regular_only", text)
        self.assertIn("mission/multi/ce_morale_probe_late_fail", text)
        self.assertIn("mission/multi/ce_morale_probe_quality_ok", text)
        self.assertIn("mission/multi/ce_morale_force_shaken", text)
        self.assertIn("mission/multi/ce_morale_probe_late_ok", text)
        self.assertIn("mission/multi/ce_morale_force_panic", text)

    def test_reporter_is_observability_only(self) -> None:
        text = REPORT.read_text(encoding="utf-8")
        for forbidden in (
            '{"entity_state"',
            "tag_add",
            "tag_remove",
            '{"set_i"',
            '{"set_f"',
            '{"actor_state"',
        ):
            self.assertNotIn(forbidden, text)


if __name__ == "__main__":
    unittest.main()
