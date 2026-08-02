from __future__ import annotations

import importlib.util
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MODULE_PATH = ROOT / "tools" / "apply_transport_flag_perimeter_waypoints_fixed.py"
spec = importlib.util.spec_from_file_location("actual_map_perimeters", MODULE_PATH)
assert spec and spec.loader
perimeters = importlib.util.module_from_spec(spec)
sys.modules[spec.name] = perimeters
spec.loader.exec_module(perimeters)


class ActualMapTransportPerimeterTests(unittest.TestCase):
    def test_all_fourteen_real_maps_generate_valid_safe_routes(self) -> None:
        paths = perimeters.map_files(ROOT)
        self.assertEqual(len(paths), 14)
        for path in paths:
            original = path.read_text(encoding="utf-8-sig")
            patched = perimeters.patch_text(original, str(path))
            perimeters.validate_text(patched, str(path))
            flags = perimeters.extract_flags(patched, str(path))
            points = perimeters.parse_waypoints(patched, str(path))
            self.assertEqual(len(points), 5)
            for slot, point in enumerate(points):
                source = flags[slot % len(flags)]
                centre_distance = (
                    (point.x - source.x) ** 2 + (point.y - source.y) ** 2
                ) ** 0.5
                self.assertGreaterEqual(
                    centre_distance - perimeters.RADIUS,
                    perimeters.CLOSEST_TO_FLAG - 0.1,
                    f"route slot {slot + 1} can reach the flag post in {path}",
                )


if __name__ == "__main__":
    unittest.main()
