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
H_INC = ROOT / "resource/map/multi/dcg_zeeland_sum/aio_p0_runtime_h.inc"
H_MI = ROOT / "resource/map/multi/dcg_zeeland_sum/aio_p0_runtime_h.mi"
H_INFO = ROOT / "resource/map/multi/dcg_zeeland_sum/aio_p0_runtime_h.info"


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
        self.assertIn("{tag_add aio_pow_evt_p0}", present)
        self.assertIn("{tag_add aio_pow_evt_present_done}", present)
        self.assertLess(present.find('{player "0"}'), present.find("{tag_add aio_pow_evt_p0}"))
        self.assertLess(present.find("{tag_add aio_pow_evt_p0}"), present.find("{tag_remove enemy}"))

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
        self.assertIn("Isolation H", transfer)
        self.assertIn("cancelled", transfer)
        self.assertIn("AI-owned", transfer)
        self.assertIn("ce_pow_dmg_editor.inc", transfer)
        self.assertIn("aio_p0_runtime_h", transfer)
        self.assertIn("aio_p0_h_dummy_1", transfer)
        self.assertIn("abandon Player 0", transfer)
        self.assertIn("not Conquest rematches", transfer)
        self.assertIn("CE_POW_DIAG", transfer)
        self.assertIn("eActorSensorDetect", transfer)
        self.assertIn("0xHHH", transfer)
        self.assertIn("Old Boy vs Conquest post-P0 cleanup delta", transfer)
        self.assertIn("sensor=unreadable", transfer)
        self.assertIn("one normal Conquest", transfer)


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


class IsolationHSplashMissionTests(unittest.TestCase):
    def test_three_editor_files_exist_and_are_opt_in(self) -> None:
        self.assertTrue(H_INC.is_file())
        self.assertTrue(H_MI.is_file())
        self.assertTrue(H_INFO.is_file())
        mi = H_MI.read_text(encoding="utf-8")
        inc = H_INC.read_text(encoding="utf-8")
        self.assertIn('(include "/map/multi/dcg_zeeland_sum/aio_p0_runtime_h.inc")', mi)
        self.assertNotIn("aio_p0_runtime_h", TRIG.read_text(encoding="utf-8"))
        self.assertNotIn("aio_p0_runtime_h", DCG.read_text(encoding="utf-8"))
        self.assertNotIn("aio_p0_runtime_h", BEH.read_text(encoding="utf-8"))
        self.assertNotIn("dcg_zeeland_sum", TRIG.read_text(encoding="utf-8"))
        self.assertIn("EDITOR ONLY", inc)

    def test_tank_attacks_dummies_left_to_right_never_bystanders(self) -> None:
        body = _uncommented(H_INC)
        mi = H_MI.read_text(encoding="utf-8")
        self.assertEqual(body.count("{action attack}"), 3)
        attacks = body.split("{action attack}", 1)[1]
        self.assertLess(attacks.find("aio_p0_h_dummy_1"), attacks.find("aio_p0_h_dummy_2"))
        self.assertLess(attacks.find("aio_p0_h_dummy_2"), attacks.find("aio_p0_h_dummy_3"))
        self.assertIn("{state operatable}", body)
        self.assertIn("{state user_control}", body)
        self.assertIn("aio_p0_h_dummy_1", attacks)
        self.assertIn("aio_p0_h_dummy_2", attacks)
        self.assertIn("aio_p0_h_dummy_3", attacks)
        self.assertIn("aio_p0_h_dummy_1", mi)
        self.assertIn("aio_p0_h_dummy_2", mi)
        self.assertIn("aio_p0_h_dummy_3", mi)
        self.assertIn("{Player 1}", mi)
        self.assertIn("{Player 2}", mi)
        self.assertNotIn("{Player 0}", mi)
        self.assertIn("{Position 200 0}", mi)
        self.assertIn("{Position 1400 0}", mi)
        self.assertIn("{Position 2600 0}", mi)
        self.assertIn("{Position 200 12}", mi)
        self.assertIn("{Position 1400 12}", mi)
        self.assertIn("{Position 2600 12}", mi)
        self.assertIn('{Link 0x8108 {0x8107 "driver"}}', mi)
        self.assertIn('{Link 0x8109 {0x8107 "gunner"}}', mi)
        self.assertIn('{Link 0x810a {0x8107 "commander"}}', mi)
        self.assertIn('{Link 0x810b {0x8107 "charger"}}', mi)

    def test_p0_apply_matches_g_and_forbids_extras(self) -> None:
        body = _uncommented(H_INC)
        self.assertEqual(body.count('{player "0"}'), 2)
        self.assertEqual(body.count("{effect start_white_flag}"), 1)
        self.assertEqual(body.count("{tag_remove enemy}"), 1)
        self.assertEqual(body.count("{volume in_hands}"), 1)
        self.assertEqual(body.count("{collage stand_giveup_1}"), 1)
        self.assertLess(body.find("aio_p0_h_bystander_p0"), body.find("{effect start_white_flag}"))
        self.assertLess(body.find("{effect start_white_flag}"), body.find("{tag_remove enemy}"))
        self.assertLess(body.find("{tag_remove enemy}"), body.find("{volume in_hands}"))
        self.assertLess(body.find("{volume in_hands}"), body.find("{collage stand_giveup_1}"))
        self.assertNotIn("aio_morale_surrendering", body)
        self.assertNotIn("aio_morale_surrender_expire", body)
        self.assertNotIn("aio_morale_surrender_evacuating", body)
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
            self.assertNotIn(extra, H_MI.read_text(encoding="utf-8"), extra)


if __name__ == "__main__":
    unittest.main()
