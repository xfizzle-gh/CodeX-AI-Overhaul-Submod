from __future__ import annotations

from .codex.catalog import CodeXCatalog, UnitDefinition
from .models import BattalionRosterEntry, CampaignState, Faction


GROUND_CATEGORIES = {
    "infantry",
    "recon",
    "vehicle",
    "ifv",
    "tank",
    "artillery",
    "air_defense",
}


def populate_valid_rosters(
    state: CampaignState,
    catalog: CodeXCatalog,
    *,
    replace_invalid: bool = True,
) -> CampaignState:
    for battalion in state.battalions.values():
        current_valid = bool(battalion.roster) and all(
            entry.unit_name in catalog.units
            and catalog.units[entry.unit_name].side == battalion.faction.value
            for entry in battalion.roster
        )
        if current_valid:
            for entry in battalion.roster:
                entry.category = catalog.units[entry.unit_name].category
            continue
        if not replace_invalid and battalion.roster:
            missing = [
                entry.unit_name
                for entry in battalion.roster
                if entry.unit_name not in catalog.units
            ]
            raise ValueError(
                f"Battalion {battalion.battalion_id} has unavailable Code:X units: {missing}"
            )
        battalion.roster = _starter_roster(catalog, battalion.faction)
    state.validate()
    return state


def _starter_roster(
    catalog: CodeXCatalog, faction: Faction
) -> list[BattalionRosterEntry]:
    candidates = [
        unit
        for unit in catalog.by_side(faction.value)
        if unit.category in GROUND_CATEGORIES and not unit.is_doctrine_unit
    ]
    if not candidates:
        candidates = [
            unit
            for unit in catalog.by_side(faction.value)
            if unit.category in GROUND_CATEGORIES
        ]
    if not candidates:
        raise ValueError(f"Code:X catalog contains no ground units for {faction.value}")

    infantry = _pick(candidates, ("infantry", "recon"))
    support = _pick(candidates, ("ifv", "vehicle", "tank", "air_defense", "artillery"))
    assert infantry is not None
    roster = [
        BattalionRosterEntry(
            unit_name=infantry.name,
            quantity=3,
            category=infantry.category,
        )
    ]
    if support and support.name != infantry.name:
        roster.append(
            BattalionRosterEntry(
                unit_name=support.name,
                quantity=1,
                category=support.category,
            )
        )
    return roster


def _pick(
    candidates: list[UnitDefinition], priorities: tuple[str, ...]
) -> UnitDefinition | None:
    for category in priorities:
        matches = sorted(
            (unit for unit in candidates if unit.category == category),
            key=lambda unit: (unit.manpower_cost, unit.name),
        )
        if matches:
            return matches[0]
    return sorted(candidates, key=lambda unit: unit.name)[0] if candidates else None
