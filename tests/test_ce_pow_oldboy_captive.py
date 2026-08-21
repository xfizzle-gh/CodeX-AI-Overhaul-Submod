from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
HUMAN = ROOT / "resource/set/interaction_entity/human_ce.inc"
BEH = ROOT / "resource/map/multi/ce/ce_broken_behavior_triggers.inc"
TRANSFER = ROOT / "docs/pow_mirror_transfer.md"
TRIG = ROOT / "resource/map/multi/ce/ce_triggers.inc"
DCG = ROOT / "resource/map/multi/dcg_script.inc"
DMG = ROOT / "resource/map/multi/ce/ce_pow_dmg_editor.inc"


def _uncommented(path: Path) -> str:
    return "\n".join(
        line for line in path.read_text(encoding="utf-8").splitlines() if not line.lstrip().startswith(";")
    )


class OldBoyProductionSurrenderTests(unittest.TestCase):
    def test_production_present_is_old_boy_five_step(self) -> None:
        beh = _uncommented(BEH)
        present = beh.split("broken/surrender_present", 1)[1].split("broken/surrender_evacuate", 1)[0]
        evac = beh.split("broken/surrender_evacuate", 1)[1].split("broken/surrender_arrive_a", 1)[0]
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
        self.assertNotIn("{ai_move", present)
        self.assertNotIn('{drop "orders sensor senseless"}', present)
        self.assertNotIn("{Player 0}", present)
        self.assertNotIn("{collage walk_giveup_1}", present)
        self.assertNotIn('{"inventory"', present)
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
        self.assertNotIn('{drop "orders sensor senseless"}', evac)
        self.assertIn("{action move}", evac)

    def test_parked_mirror_and_diagnostics_are_gone(self) -> None:
        human = HUMAN.read_text(encoding="utf-8")
        self.assertFalse((ROOT / "resource/map/multi/ce/ce_pow_oldboy_captive_editor.inc").exists())
        self.assertFalse((ROOT / "resource/map/multi/ce/ce_pow_replace_editor.inc").exists())
        self.assertFalse((ROOT / "resource/map/multi/ce/ce_pow_replace_editor_templates.inc").exists())
        self.assertFalse((ROOT / "tools/generate_pow_mirrors.py").exists())
        self.assertFalse((ROOT / "docs/pow_mirror_mapping.tsv").exists())
        self.assertFalse((ROOT / "resource/set/breed/generated_pow").exists())
        self.assertNotIn("aio_pow_ob_fight_off", human)
        self.assertNotIn("aio_iso_drop_rifle", human)
        self.assertNotIn("{delete}", human)
        self.assertNotIn("ce_pow_oldboy_captive_editor", TRIG.read_text(encoding="utf-8"))
        self.assertNotIn("ce_pow_replace_editor", TRIG.read_text(encoding="utf-8"))
        self.assertNotIn("ce_pow_oldboy_captive_editor", DCG.read_text(encoding="utf-8"))
        self.assertNotIn("ce_pow_replace_editor", DCG.read_text(encoding="utf-8"))
        transfer = TRANSFER.read_text(encoding="utf-8")
        self.assertIn("Run A PASS", transfer)
        self.assertIn("Run B PASS", transfer)
        self.assertIn("Run C PASS", transfer)
        self.assertIn("Run D PASS", transfer)
        self.assertIn("Run E PASS", transfer)
        self.assertIn("{volume in_hands}", transfer)
        self.assertIn("end-to-end", transfer)
        self.assertIn("Pruned", transfer)
        self.assertIn("d7fa808", transfer)
        self.assertIn("b95f3cdc", transfer)
        self.assertIn("withdrawn", transfer)
        self.assertIn("scene.quant.dmg", transfer)
        self.assertIn("Isolation F PASS", transfer)
        self.assertIn("Isolation G", transfer)
        self.assertIn("AI-owned", transfer)
        self.assertIn("ce_pow_dmg_editor.inc", transfer)
        self.assertIn("aio_pow_dmg_ctrl", transfer)
        self.assertIn("aio_pow_dmg_dummy_ctrl", transfer)
        self.assertIn("splash", transfer)


class CombinedDamageEditorTests(unittest.TestCase):
    def test_combined_fixture_is_opt_in_three_subject(self) -> None:
        self.assertTrue(DMG.is_file())
        body = _uncommented(DMG)
        self.assertNotIn("ce_pow_dmg_editor", TRIG.read_text(encoding="utf-8"))
        self.assertNotIn("ce_pow_dmg_editor", DCG.read_text(encoding="utf-8"))
        self.assertIn("aio_pow_dmg_ctrl", body)
        self.assertIn("aio_pow_dmg_p0", body)
        self.assertIn("aio_pow_dmg_ob", body)
        self.assertIn("aio_pow_dmg_ai", body)
        self.assertIn("aio_pow_dmg_dummy_ctrl", body)
        self.assertIn("aio_pow_dmg_dummy_p0", body)
        self.assertIn("aio_pow_dmg_dummy_ob", body)
        self.assertIn("{action attack}", body)
        self.assertEqual(body.count("{action attack}"), 3)
        attacks = body.split("{action attack}", 1)[1]
        self.assertLess(
            attacks.find("{tag aio_pow_dmg_dummy_ctrl}"),
            attacks.find("{tag aio_pow_dmg_dummy_p0}"),
        )
        self.assertLess(
            attacks.find("{tag aio_pow_dmg_dummy_p0}"),
            attacks.find("{tag aio_pow_dmg_dummy_ob}"),
        )
        self.assertNotIn("{tag aio_pow_dmg_ctrl}", attacks)
        self.assertNotIn("{tag aio_pow_dmg_p0}", attacks)
        self.assertNotIn("{tag aio_pow_dmg_ob}", attacks)
        self.assertIn("{state user_control}", body)
        self.assertNotIn("aio_morale_surrendering", body)
        self.assertNotIn("aio_morale_surrender_expire", body)
        self.assertNotIn("aio_morale_surrender_evacuating", body)
        self.assertNotIn('{"delete"', body)
        self.assertEqual(body.count('{player "0"}'), 2)
        self.assertEqual(body.count("{effect start_white_flag}"), 1)
        self.assertEqual(body.count("{tag_remove enemy}"), 1)
        self.assertEqual(body.count("{volume in_hands}"), 1)
        self.assertEqual(body.count("{collage stand_giveup_1}"), 1)
        self.assertLess(body.find('{player "0"}'), body.find("{effect start_white_flag}"))
        self.assertLess(body.find("{effect start_white_flag}"), body.find("{tag_remove enemy}"))
        self.assertLess(body.find("{tag_remove enemy}"), body.find("{volume in_hands}"))
        self.assertLess(body.find("{volume in_hands}"), body.find("{collage stand_giveup_1}"))
        extras = (
            "{control AI}",
            '{able "select" 0}',
            '{able "fight" 0}',
            "{fire_mode hold}",
            "{weapon_prepare off}",
            "{move_mode hold}",
            "{ai_move",
            '{drop "orders sensor senseless"}',
            "{Player 0}",
            "aio_pow_ob_fight_off",
            "generated_pow",
            "{delete}",
        )
        for extra in extras:
            self.assertNotIn(extra, body, extra)


if __name__ == "__main__":
    unittest.main()
