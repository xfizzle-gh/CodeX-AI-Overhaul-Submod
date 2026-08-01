from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TEMPLATE = ROOT / "resource/map/multi/faction_support_templates.inc"
ENGINES = {
    "resource/map/multi/attack_support_waves.inc": {
        "define": "as",
        "hull": "attack_support_motor_hull",
        "crew": "attack_support_motor_crew",
        "pax": "attack_support_motor_pax",
        "flag": "attack_support_motor_flag",
        "stage": "attack_support_motor_stage$",
    },
    "resource/map/multi/defense_support_waves.inc": {
        "define": "ds",
        "hull": "def_sup_motor_hull",
        "crew": "def_sup_motor_crew",
        "pax": "def_sup_motor_pax",
        "flag": "def_sup_motor_flag",
        "stage": "defense_support_motor_stage$",
    },
    "resource/map/multi/enemy_attack_support.inc": {
        "define": "ea",
        "hull": "ea_motor_hull",
        "crew": "ea_motor_crew",
        "pax": "ea_motor_pax",
        "flag": "ea_motor_flag",
        "stage": "enemy_attack_motor_stage$",
    },
    "resource/map/multi/enemy_defense_support.inc": {
        "define": "ed",
        "hull": "enemy_def_motor_hull",
        "crew": "enemy_def_motor_crew",
        "pax": "enemy_def_motor_pax",
        "flag": "enemy_def_motor_flag",
        "stage": "enemy_defense_motor_stage$",
    },
}


def _define_block(text: str, name: str) -> str:
    start = text.index(f'(define "{name}"')
    depth = 0
    opened = False
    for index in range(start, len(text)):
        char = text[index]
        if char == "(":
            depth += 1
            opened = True
        elif char == ")":
            depth -= 1
            if opened and depth == 0:
                return text[start:index + 1]
    raise AssertionError(f"Unbalanced define: {name}")


def _motor_links(text: str) -> list[tuple[int, int, str]]:
    link_re = re.compile(
        r'\{Link\s+(0x[0-9a-fA-F]+)\s+\{(0x[0-9a-fA-F]+)\s+"([^"]+)"\}\}'
    )
    result: list[tuple[int, int, str]] = []
    for body_raw, hull_raw, slot in link_re.findall(text):
        body = int(body_raw, 16)
        hull = int(hull_raw, 16)
        base = 0xB3A0 <= hull <= 0xB3CB and 0xB3A0 <= body <= 0xB3CB
        topup = 0xC100 <= hull <= 0xC153 and 0xC100 <= body <= 0xC153
        if base or topup:
            result.append((body, hull, slot))
    return result


class MotorRuntimeIsolationTests(unittest.TestCase):
    def test_all_four_engines_use_decoupled_drop_helpers(self) -> None:
        for relative, config in ENGINES.items():
            define = config["define"]
            with self.subTest(relative=relative):
                text = (ROOT / relative).read_text(encoding="utf-8")
                finisher = _define_block(text, f"{define}_finish_motor")
                batch_helper = _define_block(text, f"{define}_drop_motor_pax")
                body_helper = _define_block(text, f"{define}_motor_drop_one")
                self.assertIn(f'("{define}_drop_motor_pax")', finisher)
                self.assertEqual(
                    batch_helper.count(f'("{define}_motor_drop_one")'),
                    8,
                )
                self.assertIn('{"placement"', body_helper)
                self.assertIn("attack_support_air_", body_helper)
                self.assertIn(config["pax"], body_helper)

    def test_release_has_no_occupancy_or_emit_dependency(self) -> None:
        for relative, config in ENGINES.items():
            define = config["define"]
            with self.subTest(relative=relative):
                text = (ROOT / relative).read_text(encoding="utf-8")
                finisher = _define_block(text, f"{define}_finish_motor")
                batch_helper = _define_block(text, f"{define}_drop_motor_pax")
                body_helper = _define_block(text, f"{define}_motor_drop_one")
                combined = finisher + batch_helper + body_helper
                self.assertNotIn("{state inhabited}", combined)
                self.assertNotIn("{emit", combined)
                self.assertNotIn("emit {mode passengers}", combined)
                self.assertGreaterEqual(finisher.count('{"delay" {time 7}}'), 4)

    def test_truck_moves_before_timed_infantry_release(self) -> None:
        for relative, config in ENGINES.items():
            define = config["define"]
            with self.subTest(relative=relative):
                text = (ROOT / relative).read_text(encoding="utf-8")
                finisher = _define_block(text, f"{define}_finish_motor")
                helper_call = finisher.index(f'("{define}_drop_motor_pax")')
                vehicle_move = finisher.index("{action move}")
                infantry_advance = finisher.index("{action advance}", helper_call)
                self.assertLess(vehicle_move, helper_call)
                self.assertLess(helper_call, infantry_advance)
                self.assertIn(config["hull"], finisher)
                self.assertIn(config["crew"], finisher)
                self.assertIn(config["pax"], finisher)
                self.assertIn(config["flag"], finisher)

    def test_motor_stage_completes_and_resets(self) -> None:
        for relative, config in ENGINES.items():
            define = config["define"]
            stage = config["stage"]
            with self.subTest(relative=relative):
                text = (ROOT / relative).read_text(encoding="utf-8")
                finisher = _define_block(text, f"{define}_finish_motor")
                self.assertIn(
                    f'{{"set_i" {{var "{stage}"}} {{op "="}} {{value 4}}}}',
                    finisher,
                )
                self.assertIn(
                    f'{{"set_i" {{var "{stage}"}} {{op "="}} {{value 0}}}}',
                    finisher,
                )

    def test_templates_link_only_one_driver_per_motor_package(self) -> None:
        text = TEMPLATE.read_text(encoding="utf-8")
        links = _motor_links(text)
        self.assertEqual(len(links), 16)
        self.assertEqual(sum(slot == "driver" for _, _, slot in links), 16)
        self.assertEqual(sum(slot == "commander" for _, _, slot in links), 0)
        self.assertEqual(sum(slot.startswith("seat") for _, _, slot in links), 0)
        self.assertEqual(len({hull for _, hull, _ in links}), 16)

    def test_motor_passengers_are_loose_hidden_packages(self) -> None:
        text = TEMPLATE.read_text(encoding="utf-8")
        tag_lines = [line for line in text.splitlines() if "{Tags " in line]
        pax_lines = [
            line
            for line in tag_lines
            if re.search(r'ally_sup_(?:rusa|ukr|nato|prc)_p[1-4]_pax', line)
        ]
        self.assertTrue(pax_lines)
        self.assertTrue(all("sup_linked" not in line for line in pax_lines))
        for faction in ("rusa", "ukr", "nato", "prc"):
            for package in range(1, 5):
                tag = f'ally_sup_{faction}_p{package}_crew'
                linked = [
                    line
                    for line in tag_lines
                    if tag in line and "sup_linked" in line
                ]
                self.assertEqual(
                    len(linked),
                    1,
                    f"{tag} must name exactly one linked driver",
                )


if __name__ == "__main__":
    unittest.main()
