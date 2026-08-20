from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
EDITOR = ROOT / "resource/map/multi/ce/ce_pow_oldboy_captive_editor.inc"
MIRROR = ROOT / "resource/map/multi/ce/ce_pow_replace_editor.inc"
TRIG = ROOT / "resource/map/multi/ce/ce_triggers.inc"
DCG = ROOT / "resource/map/multi/dcg_script.inc"
HUMAN = ROOT / "resource/set/interaction_entity/human_ce.inc"
BEH = ROOT / "resource/map/multi/ce/ce_broken_behavior_triggers.inc"
TRANSFER = ROOT / "docs/pow_mirror_transfer.md"


class OldBoyCaptiveEditorTests(unittest.TestCase):
    def test_opt_in_sequence_is_exactly_old_boy_captive(self) -> None:
        self.assertTrue(EDITOR.is_file())
        raw = EDITOR.read_text(encoding="utf-8")
        body = "\n".join(line for line in raw.splitlines() if not line.lstrip().startswith(";"))
        self.assertNotIn("ce_pow_oldboy_captive_editor", TRIG.read_text(encoding="utf-8"))
        self.assertNotIn("ce_pow_oldboy_captive_editor", DCG.read_text(encoding="utf-8"))
        self.assertIn("{effect start_white_flag}", body)
        self.assertIn("{operation set}", body)
        self.assertIn('{player "0"}', body)
        self.assertIn("{tag_remove enemy}", body)
        self.assertIn("{action drop}", body)
        self.assertIn("{item \"weapon\"}", body)
        self.assertIn("{type using}", body)
        self.assertIn("{collage stand_giveup_1}", body)
        self.assertLess(body.find("{effect start_white_flag}"), body.find('{player "0"}'))
        self.assertLess(body.find('{player "0"}'), body.find("{tag_remove enemy}"))
        self.assertLess(body.find("{tag_remove enemy}"), body.find("{action drop}"))
        self.assertLess(body.find("{action drop}"), body.find("{collage stand_giveup_1}"))
        self.assertNotIn("{control AI}", body)
        self.assertNotIn("{able \"neutral\"}", body)
        self.assertNotIn("{able \"select\" 0}", body)
        self.assertNotIn("{able \"fight\" 0}", body)
        self.assertNotIn("aio_morale_surrender", body)
        self.assertNotIn("aio_pow_need_replace", body)
        self.assertNotIn("aio_pow_civ", body)
        self.assertNotIn("generated_pow", body)
        self.assertNotIn("{clone}", body)
        apply = HUMAN.read_text(encoding="utf-8").split(
            '{on "aio_morale_surrender_apply"', 1
        )[1].split('{on "', 1)[0]
        self.assertNotIn('{player "0"}', apply)
        self.assertNotIn("{control AI}", apply)
        beh = BEH.read_text(encoding="utf-8")
        self.assertNotIn('{player "0"}', beh)
        self.assertNotIn("{control AI}", beh)
        mirror_body = "\n".join(
            line
            for line in MIRROR.read_text(encoding="utf-8").splitlines()
            if not line.lstrip().startswith(";")
        )
        self.assertNotIn('{player "0"}', mirror_body)
        transfer = TRANSFER.read_text(encoding="utf-8")
        self.assertIn("P1->P0 alone PASS", transfer)
        self.assertIn("parked", transfer)
        self.assertIn("stand_giveup_1", transfer)
        self.assertNotIn("{control AI}", apply)
