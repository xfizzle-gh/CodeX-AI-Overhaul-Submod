from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TEMPLATE = ROOT / "resource/map/multi/faction_support_templates.inc"
FACTIONS = ("rusa", "ukr", "nato", "prc")


class MotorPackageOneRestoreTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.text = TEMPLATE.read_text(encoding="utf-8")
        cls.tag_lines = [
            line for line in cls.text.splitlines() if "{Tags " in line
        ]

    def test_all_numbered_motor_packages_exist(self) -> None:
        for faction in FACTIONS:
            for package in range(1, 5):
                self.assertIn(f'ally_sup_{faction}_p{package}_hull', self.text)
                self.assertIn(f'ally_sup_{faction}_p{package}_crew', self.text)
                self.assertIn(f'ally_sup_{faction}_p{package}_pax', self.text)

    def test_each_package_has_one_linked_driver(self) -> None:
        for faction in FACTIONS:
            for package in range(1, 5):
                tag = f'ally_sup_{faction}_p{package}_crew'
                linked = [
                    line
                    for line in self.tag_lines
                    if tag in line and "sup_linked" in line
                ]
                self.assertEqual(len(linked), 1, f"{tag} must have one linked driver")

    def test_passenger_tags_are_never_linked(self) -> None:
        for faction in FACTIONS:
            for package in range(1, 5):
                tag = f'ally_sup_{faction}_p{package}_pax'
                lines = [line for line in self.tag_lines if tag in line]
                self.assertTrue(lines, tag)
                self.assertTrue(all("sup_linked" not in line for line in lines), tag)

    def test_motor_link_table_has_only_one_driver_per_hull(self) -> None:
        link_re = re.compile(
            r'\{Link\s+(0x[0-9a-fA-F]+)\s+\{(0x[0-9a-fA-F]+)\s+"([^"]+)"\}\}'
        )
        links: list[tuple[int, int, str]] = []
        for body_raw, hull_raw, slot in link_re.findall(self.text):
            body = int(body_raw, 16)
            hull = int(hull_raw, 16)
            base = 0xB3A0 <= hull <= 0xB3CB and 0xB3A0 <= body <= 0xB3CB
            topup = 0xC100 <= hull <= 0xC153 and 0xC100 <= body <= 0xC153
            if base or topup:
                links.append((body, hull, slot))
        self.assertEqual(len(links), 16)
        self.assertTrue(all(slot == "driver" for _, _, slot in links))
        self.assertEqual(len({hull for _, hull, _ in links}), 16)


if __name__ == "__main__":
    unittest.main()
