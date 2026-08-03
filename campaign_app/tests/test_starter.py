from __future__ import annotations

import unittest
from pathlib import Path

from gates_of_codex.codex.catalog import CodeXCatalog, UnitDefinition
from gates_of_codex.scenario import load_scenario
from gates_of_codex.starter import populate_valid_rosters


SCENARIO = Path(__file__).resolve().parents[1] / "data/four_faction_test.json"


class StarterTests(unittest.TestCase):
    def test_replaces_invalid_rosters_for_all_factions(self) -> None:
        units = {}
        for side in ("nato", "ukr", "rusa", "prc"):
            units[f"rifle({side})"] = UnitDefinition(
                name=f"rifle({side})", side=side, category="infantry", manpower_cost=100
            )
            units[f"tank({side})"] = UnitDefinition(
                name=f"tank({side})", side=side, category="tank", manpower_cost=500
            )
        state = load_scenario(SCENARIO)
        populate_valid_rosters(state, CodeXCatalog(units=units))
        for battalion in state.battalions.values():
            self.assertEqual(2, len(battalion.roster))
            self.assertTrue(
                all(
                    entry.unit_name.endswith(f"({battalion.faction.value})")
                    for entry in battalion.roster
                )
            )


if __name__ == "__main__":
    unittest.main()
