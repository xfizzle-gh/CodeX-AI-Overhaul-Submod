from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TEMPLATE = ROOT / "resource/map/multi/faction_support_templates.inc"
ENGINES = {
    "resource/map/multi/attack_support_waves.inc": "as",
    "resource/map/multi/defense_support_waves.inc": "ds",
    "resource/map/multi/enemy_attack_support.inc": "ea",
    "resource/map/multi/enemy_defense_support.inc": "ed",
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
    def test_motor_core_runs_on_all_four_engines(self) -> None:
        for relative, prefix in ENGINES.items():
            with self.subTest(relative=relative):
                text = (ROOT / relative).read_text(encoding="utf-8")
                block = _define_block(text, f"{prefix}_finish_motor")
                self.assertIn("DECOUPLED MOTOR DELIVERY", block)
                self.assertIn("NO PASSENGER LINK OR EMIT DEPENDENCY", block)
                self.assertIn("BOUNDED INFANTRY RELEASE INDEPENDENT OF TRUCK", block)
                self.assertIn(f"{prefix}_motor_hull", block)
                self.assertIn(f"{prefix}_motor_crew", block)
                self.assertIn(f"{prefix}_motor_pax", block)
                self.assertIn(f"{prefix}_motor_flag", block)

    def test_release_is_independent_of_vehicle_occupancy_and_emit(self) -> None:
        for relative, prefix in ENGINES.items():
            with self.subTest(relative=relative):
                text = (ROOT / relative).read_text(encoding="utf-8")
                block = _define_block(text, f"{prefix}_finish_motor")
                self.assertNotIn("{state inhabited}", block)
                self.assertNotIn("{emit", block)
                self.assertNotIn("emit {mode passengers}", block)
                self.assertGreaterEqual(block.count('{"delay" {time 7}}'), 4)
                self.assertIn("attack_support_air_", block)
                placement = block.index('{"placement"')
                advance = block.index("{action advance}", placement)
                self.assertLess(placement, advance)

    def test_truck_uses_vehicle_move_and_infantry_uses_advance(self) -> None:
        for relative, prefix in ENGINES.items():
            with self.subTest(relative=relative):
                text = (ROOT / relative).read_text(encoding="utf-8")
                block = _define_block(text, f"{prefix}_finish_motor")
                move = block.index("{action move}")
                release = block.index("BOUNDED INFANTRY RELEASE INDEPENDENT OF TRUCK")
                advance = block.index("{action advance}", release)
                self.assertLess(move, release)
                self.assertLess(release, advance)
                self.assertIn(
                    '{"set_i" {var "' + prefix + '_motor_stage$"} {op "="} {value 5}}',
                    block,
                )
                self.assertIn(
                    '{"set_i" {var "' + prefix + '_motor_stage$"} {op "="} {value 0}}',
                    block,
                )

    def test_templates_link_only_one_driver_per_motor_package(self) -> None:
        text = TEMPLATE.read_text(encoding="utf-8")
        links = _motor_links(text)
        self.assertEqual(sum(slot == "driver" for _, _, slot in links), 16)
        self.assertEqual(sum(slot == "commander" for _, _, slot in links), 0)
        self.assertEqual(sum(slot.startswith("seat") for _, _, slot in links), 0)

    def test_motor_passengers_are_loose_hidden_packages(self) -> None:
        text = TEMPLATE.read_text(encoding="utf-8")
        lines = text.splitlines()
        pax_lines = [
            line
            for line in lines
            if re.search(r'ally_sup_(?:rusa|ukr|nato|prc)_p[1-4]_pax', line)
        ]
        self.assertTrue(pax_lines)
        self.assertTrue(all("sup_linked" not in line for line in pax_lines))
        for faction in ("rusa", "ukr", "nato", "prc"):
            for package in range(1, 5):
                tag = f'ally_sup_{faction}_p{package}_crew'
                linked = [line for line in lines if tag in line and "sup_linked" in line]
                self.assertEqual(len(linked), 1, f"{tag} must name exactly one linked driver")


if __name__ == "__main__":
    unittest.main()
