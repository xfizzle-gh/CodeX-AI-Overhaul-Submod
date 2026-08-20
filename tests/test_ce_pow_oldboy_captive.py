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


def _uncommented(path: Path) -> str:
    return "\n".join(
        line for line in path.read_text(encoding="utf-8").splitlines() if not line.lstrip().startswith(";")
    )


class OldBoyCaptiveEditorTests(unittest.TestCase):
    def test_production_present_is_old_boy_five_step(self) -> None:
        beh = _uncommented(BEH)
        present = beh.split("broken/surrender_present", 1)[1].split("broken/surrender_evacuate", 1)[0]
        human = HUMAN.read_text(encoding="utf-8")
        apply = human.split('{on "aio_morale_surrender_apply"', 1)[1].split('{on "', 1)[0]
        self.assertIn("{effect start_white_flag}", present)
        self.assertIn("{operation set}", present)
        self.assertIn('{player "0"}', present)
        self.assertIn("{tag_remove enemy}", present)
        self.assertIn('{"action"', present)
        self.assertIn("{action drop}", present)
        self.assertIn("{volume in_hands}", present)
        self.assertIn("{collage stand_giveup_1}", present)
        self.assertLess(present.find("{effect start_white_flag}"), present.find('{player "0"}'))
        self.assertLess(present.find('{player "0"}'), present.find("{tag_remove enemy}"))
        self.assertLess(present.find("{tag_remove enemy}"), present.find("{volume in_hands}"))
        self.assertLess(present.find("{volume in_hands}"), present.find("{collage stand_giveup_1}"))
        self.assertNotIn("{control AI}", present)
        self.assertNotIn('{able "select" 0}', present)
        self.assertNotIn('{able "fight" 0}', present)
        self.assertNotIn("{fire_mode hold}", present)
        self.assertNotIn("{move_mode hold}", present)
        self.assertNotIn("{weapon_prepare off}", present)
        self.assertNotIn('{call "weapon_prepare_off"}', present)
        self.assertNotIn("{ai_move", present)
        self.assertNotIn('{drop "orders sensor senseless"}', present)
        self.assertNotIn("{Player 0}", present)
        self.assertNotIn("{collage walk_giveup_1}", present)
        self.assertNotIn('{"inventory"', present)
        self.assertNotIn("{item \"weapon\"}", present)
        self.assertNotIn("{effect aio_pow_ob_fight_off}", present)
        self.assertNotIn("{control AI}", apply)
        self.assertNotIn('{able "select" 0}', apply)
        self.assertNotIn('{able "fight" 0}', apply)
        self.assertNotIn('{player "0"}', apply)
        self.assertIn('{tags add "aio_morale_surrendering"}', apply)
        self.assertNotIn("{control AI}", beh)
        self.assertNotIn("{fire_mode hold}", beh)
        self.assertNotIn("{weapon_prepare off}", beh)
        self.assertNotIn("{Player 0}", beh)

    def test_opt_in_diagnostic_stays_unwired(self) -> None:
        self.assertTrue(EDITOR.is_file())
        body = _uncommented(EDITOR)
        self.assertNotIn("ce_pow_oldboy_captive_editor", TRIG.read_text(encoding="utf-8"))
        self.assertNotIn("ce_pow_oldboy_captive_editor", DCG.read_text(encoding="utf-8"))
        self.assertIn("{effect start_white_flag}", body)
        self.assertIn('{player "0"}', body)
        self.assertIn("{tag_remove enemy}", body)
        self.assertIn("{collage stand_giveup_1}", body)
        self.assertNotIn("{control AI}", body)
        self.assertNotIn("{able \"neutral\"}", body)
        self.assertNotIn("{able \"select\" 0}", body)
        self.assertNotIn('{able "fight" 0}', body)
        self.assertNotIn("{fire_mode hold}", body)
        self.assertNotIn("{weapon_prepare off}", body)
        self.assertNotIn("aio_morale_surrender", body)
        self.assertNotIn("generated_pow", body)
        fight = HUMAN.read_text(encoding="utf-8").split(
            '{on "aio_pow_ob_fight_off"', 1
        )[1].split("{on ", 1)[0]
        self.assertIn('{able "fight" 0}', fight)
        self.assertNotIn("{delete}", fight)
        self.assertNotIn("{control AI}", fight)
        self.assertNotIn("{delete}", HUMAN.read_text(encoding="utf-8"))
        mirror_body = _uncommented(MIRROR)
        self.assertNotIn('{player "0"}', mirror_body)
        self.assertNotIn("{control AI}", mirror_body)
        transfer = TRANSFER.read_text(encoding="utf-8")
        self.assertIn("Run A PASS", transfer)
        self.assertIn("Run B PASS", transfer)
        self.assertIn("Run C/D/E PASS", transfer)
        self.assertIn("parked", transfer)
        self.assertIn("stand_giveup_1", transfer)
        self.assertIn("{volume in_hands}", transfer)
        self.assertIn("aio_morale_surrendering", transfer)
        self.assertNotIn("{control AI}", HUMAN.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
