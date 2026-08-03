from __future__ import annotations

import shutil
import unittest
from pathlib import Path
from tempfile import TemporaryDirectory

from gates_of_codex.codex.catalog import CodeXCatalog, CodeXCatalogScanner
from gates_of_codex.codex.locator import CodeXLocator


FIXTURE = Path(__file__).resolve().parent / "fixtures" / "codex"


class CatalogTests(unittest.TestCase):
    def test_scans_set_and_lua_metadata(self) -> None:
        catalog = CodeXCatalogScanner().scan(FIXTURE)
        self.assertEqual(2, len(catalog.units))
        infantry = catalog.units["squad_inf2_rifle(nato)"]
        tank = catalog.units["squad_tank1_m1a2_sep(nato)"]
        self.assertEqual("nato", infantry.side)
        self.assertEqual("infantry", infantry.category)
        self.assertEqual(8, sum(infantry.members.values()))
        self.assertEqual("tank", tank.category)
        self.assertEqual(["m1a2_sep"], tank.vehicles)
        self.assertEqual(8.0, tank.doctrine_cost)
        self.assertTrue(tank.is_doctrine_unit)

    def test_catalog_round_trip(self) -> None:
        catalog = CodeXCatalogScanner().scan(FIXTURE)
        with TemporaryDirectory() as folder:
            target = Path(folder) / "catalog.json"
            catalog.save(target)
            loaded = CodeXCatalog.load(target)
            self.assertEqual(catalog.to_dict(), loaded.to_dict())

    def test_locator_recognizes_codex_mod(self) -> None:
        locator = CodeXLocator()
        with TemporaryDirectory() as folder:
            library = Path(folder)
            target = library / "steamapps/workshop/content/400750/123456"
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copytree(FIXTURE, target)
            self.assertEqual(target, locator.find_code_x_directory([library]))


if __name__ == "__main__":
    unittest.main()
