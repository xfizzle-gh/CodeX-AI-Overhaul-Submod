#!/usr/bin/env python3
"""Static safety checks for the curated 2022s skirmish rosters."""

from __future__ import annotations

from collections import Counter
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
SET_DIR = ROOT / "resource/set/multiplayer/units/2022s"
CONQUEST_DIR = ROOT / "resource/set/multiplayer/units/conquest"
ROSTER_FILE = ROOT / "resource/set/multiplayer/units/roster_2022s.set"

DOCTRINE_FILES = {
    "nato": SET_DIR / "doctrine_units_nato.set",
    "ukr": SET_DIR / "doctrine_units_ukr.set",
    "rusa": SET_DIR / "doctrine_units_rusa.set",
    "prc": SET_DIR / "doctrine_units_prc.set",
}

BREED_CATALOGS = {
    "nato": CONQUEST_DIR / "inf_nato.set",
    "ukr": CONQUEST_DIR / "inf_ukr.set",
    "rusa": CONQUEST_DIR / "inf_rusa.set",
    "prc": CONQUEST_DIR / "inf_prc_era1960.set",
}

ACTIVE_SET_FILES = [
    SET_DIR / "minimal_units.set",
    SET_DIR / "doctrines.set",
    *DOCTRINE_FILES.values(),
]

LUA_FILES = {
    "nato": ROOT / "resource/script/multiplayer/units/nato/2022s.nato.lua",
    "ukr": ROOT / "resource/script/multiplayer/units/ukr/2022s.ukr.lua",
    "rusa": ROOT / "resource/script/multiplayer/units/rusa/2022s.rusa.lua",
    "prc": ROOT / "resource/script/multiplayer/units/prc/2022s.prc.lua",
}

EXPECTED_DP_COUNTS = {
    "modern_nato_usa": 5,
    "modern_nato_eu": 5,
    "modern_ukr_home": 5,
    "modern_ukr_western": 5,
    "modern_rusa_ground": 5,
    "modern_rusa_vdv": 5,
    "modern_prc_112": 5,
    "modern_prc_139": 5,
}

EXPECTED_ROSTER_INCLUDES = {
    '(include "2022s/doctrine_units_nato.set")',
    '(include "2022s/doctrine_units_ukr.set")',
    '(include "2022s/doctrine_units_rusa.set")',
    '(include "2022s/doctrine_units_prc.set")',
}

FORBIDDEN_BLOCK_PATTERNS = {
    "MLRS label": re.compile(r"\bmlrs\b", re.IGNORECASE),
    "M270": re.compile(r"\bm[-_]?270\b", re.IGNORECASE),
    "HIMARS": re.compile(r"\bhimars\b|\bm[-_]?142\b", re.IGNORECASE),
    "BM-21": re.compile(r"\bbm[-_]?21\b", re.IGNORECASE),
    "PHL-03": re.compile(r"\bphl[-_]?03\b", re.IGNORECASE),
    "TOS-1/Solntsepek": re.compile(r"\btos[-_]?1\b|solntsepek", re.IGNORECASE),
    "air-support action template": re.compile(
        r"mp_support_action|dp_action_vehicle|airsupport_trigger", re.IGNORECASE
    ),
}

BLOCK_START_RE = re.compile(r'^\s*\{"([^"]+)"')
DIRECT_ID_RE = re.compile(r'^\s*\{"([^"]+)"', re.MULTILINE)
PURCHASE_ID_RE = re.compile(r'\bunit\s*=\s*"([^"]+)"')
TEMPLATE_RE = re.compile(r'\("([^"]+)"')
DOCTRINE_COST_RE = re.compile(
    r'\("doctrine_t1"[^\n]*\bd\(([^)]+)\)[^\n]*\bcost\(([-+]?\d+(?:\.\d+)?)\)'
)
ZERO_COUNT_RE = re.compile(r":\s*0(?=[)\s])")
SIDE_RE = re.compile(r"\bside\(([^)]+)\)")
MEMBER_RE = re.compile(r"\b(?:c\d+|crew\d*)\(([^:()]+):([0-9]+)\)")
BREED_COST_RE = re.compile(
    r'^\s*\{"mp/([^/]+)/2022s/([^"]+)".*?\{cost\s+([-+]?\d+(?:\.\d+)?)\}'
)


def fail(errors: list[str], message: str) -> None:
    errors.append(message)


def read_text(path: Path, errors: list[str]) -> str:
    if not path.is_file():
        fail(errors, f"missing required file: {path.relative_to(ROOT)}")
        return ""
    return path.read_text(encoding="utf-8")


def parse_unit_blocks(path: Path, errors: list[str]) -> list[tuple[str, str]]:
    text = read_text(path, errors)
    blocks: list[tuple[str, str]] = []
    current_id: str | None = None
    current_lines: list[str] = []

    for line in text.splitlines():
        if current_id is None:
            match = BLOCK_START_RE.match(line)
            if match:
                current_id = match.group(1)
                current_lines = [line]
            continue

        current_lines.append(line)
        if line.strip() == "}":
            blocks.append((current_id, "\n".join(current_lines)))
            current_id = None
            current_lines = []

    if current_id is not None:
        fail(
            errors,
            f"{path.relative_to(ROOT)}: unterminated unit block for {current_id}",
        )
    return blocks


def extract_defined_ids(errors: list[str]) -> set[str]:
    defined: set[str] = set()
    for path in sorted(SET_DIR.glob("*.set")):
        text = read_text(path, errors)
        defined.update(DIRECT_ID_RE.findall(text))

        for line in text.splitlines():
            if line.lstrip().startswith(";"):
                continue
            name_match = re.search(r"\bname\(([^)]+)\)", line)
            side_match = SIDE_RE.search(line)
            if name_match and side_match:
                defined.add(f"{name_match.group(1)}({side_match.group(1)})")
    return defined


def extract_breed_costs(errors: list[str]) -> dict[tuple[str, str], float]:
    costs: dict[tuple[str, str], float] = {}

    for expected_side, path in BREED_CATALOGS.items():
        text = read_text(path, errors)
        for line_number, line in enumerate(text.splitlines(), start=1):
            match = BREED_COST_RE.match(line)
            if not match:
                continue

            side, breed, raw_cost = match.groups()
            if side != expected_side:
                fail(
                    errors,
                    f"{path.relative_to(ROOT)}:{line_number}: expected side {expected_side}, found {side}",
                )

            # Code:X intentionally overrides some breed prices later in the same
            # catalog. Match the engine's final-definition-wins behavior.
            costs[(side, breed)] = float(raw_cost)

    return costs


def validate_unit_blocks(errors: list[str]) -> None:
    dp_counts: Counter[str] = Counter()

    for faction, path in DOCTRINE_FILES.items():
        for unit_id, body in parse_unit_blocks(path, errors):
            template_match = TEMPLATE_RE.search(body)
            if not template_match:
                fail(errors, f"{path.relative_to(ROOT)}: {unit_id} has no template")
                continue

            template = template_match.group(1)

            if ZERO_COUNT_RE.search(body):
                fail(errors, f"{path.relative_to(ROOT)}: {unit_id} contains a zero-count member")

            for label, pattern in FORBIDDEN_BLOCK_PATTERNS.items():
                if pattern.search(body):
                    fail(errors, f"{path.relative_to(ROOT)}: {unit_id} contains forbidden {label}")

            if template.startswith("mp_"):
                if not re.search(r"\bd\([^)]+\)", body):
                    fail(errors, f"{path.relative_to(ROOT)}: {unit_id} is MP but has no doctrine gate")

                if "vehicle(" in body:
                    if not re.search(r"\bcrew\d*\([^)]+:[1-9]\d*\)", body):
                        fail(errors, f"{path.relative_to(ROOT)}: {unit_id} vehicle has no positive crew")
                elif "c1(" not in body:
                    fail(errors, f"{path.relative_to(ROOT)}: {unit_id} infantry has no content")

            elif template.startswith("dp_"):
                cost_match = DOCTRINE_COST_RE.search(body)
                if not cost_match:
                    fail(errors, f"{path.relative_to(ROOT)}: {unit_id} has no explicit doctrine cost")
                    continue

                doctrine, raw_cost = cost_match.groups()
                cost = float(raw_cost)
                if cost <= 0:
                    fail(errors, f"{path.relative_to(ROOT)}: {unit_id} has nonpositive DP cost {raw_cost}")
                dp_counts[doctrine] += 1

                if "vehicle(" in body:
                    if not re.search(r"\bcrew\d*\([^)]+:[1-9]\d*\)", body):
                        fail(errors, f"{path.relative_to(ROOT)}: {unit_id} DP vehicle has no positive crew")
                elif "c1(" not in body:
                    fail(errors, f"{path.relative_to(ROOT)}: {unit_id} DP infantry has no content")
            else:
                fail(errors, f"{path.relative_to(ROOT)}: {unit_id} uses unsupported template {template}")

        if not path.is_file():
            fail(errors, f"missing doctrine file for {faction}")

    for doctrine, expected in EXPECTED_DP_COUNTS.items():
        actual = dp_counts[doctrine]
        if actual != expected:
            fail(errors, f"{doctrine}: expected {expected} DP choices, found {actual}")

    unexpected = sorted(set(dp_counts) - set(EXPECTED_DP_COUNTS))
    for doctrine in unexpected:
        fail(errors, f"unexpected doctrine ID in DP roster: {doctrine}")


def validate_purchase_tables(errors: list[str]) -> None:
    defined = extract_defined_ids(errors)

    for faction, path in LUA_FILES.items():
        text = read_text(path, errors)
        purchase_ids = PURCHASE_ID_RE.findall(text)
        if not purchase_ids:
            fail(errors, f"{path.relative_to(ROOT)}: no purchase IDs found")
            continue

        missing = sorted(set(purchase_ids) - defined)
        for unit_id in missing:
            fail(errors, f"{path.relative_to(ROOT)}: purchase ID is undefined: {unit_id}")

        duplicates = sorted(unit_id for unit_id, count in Counter(purchase_ids).items() if count > 1)
        for unit_id in duplicates:
            # Ukraine intentionally reuses its Western doctrine selector as a DP card.
            if faction == "ukr" and unit_id == "doctrine_squad_47th(ukr)":
                continue
            fail(errors, f"{path.relative_to(ROOT)}: duplicate purchase ID: {unit_id}")


def validate_inherited_member_costs(errors: list[str]) -> None:
    costs = extract_breed_costs(errors)

    for path in ACTIVE_SET_FILES:
        text = read_text(path, errors)
        for line_number, line in enumerate(text.splitlines(), start=1):
            if line.lstrip().startswith(";"):
                continue

            members = MEMBER_RE.findall(line)
            if not members:
                continue

            side_match = SIDE_RE.search(line)
            if not side_match:
                fail(
                    errors,
                    f"{path.relative_to(ROOT)}:{line_number}: member content has no side",
                )
                continue

            side = side_match.group(1)
            if side not in BREED_CATALOGS:
                fail(
                    errors,
                    f"{path.relative_to(ROOT)}:{line_number}: unsupported side {side}",
                )
                continue

            for breed, raw_count in members:
                count = int(raw_count)
                if count <= 0:
                    fail(
                        errors,
                        f"{path.relative_to(ROOT)}:{line_number}: {side}/{breed} has nonpositive count {count}",
                    )
                    continue

                cost = costs.get((side, breed))
                if cost is None:
                    fail(
                        errors,
                        f"{path.relative_to(ROOT)}:{line_number}: undefined inherited cost for {side}/{breed}",
                    )
                elif cost <= 0:
                    fail(
                        errors,
                        f"{path.relative_to(ROOT)}:{line_number}: {side}/{breed} has nonpositive inherited cost {cost}",
                    )


def validate_roster_includes(errors: list[str]) -> None:
    text = read_text(ROSTER_FILE, errors)
    for include in sorted(EXPECTED_ROSTER_INCLUDES):
        if include not in text:
            fail(errors, f"{ROSTER_FILE.relative_to(ROOT)}: missing {include}")


def validate_no_zero_counts_in_active_sets(errors: list[str]) -> None:
    for path in ACTIVE_SET_FILES:
        text = read_text(path, errors)
        for line_number, line in enumerate(text.splitlines(), start=1):
            if line.lstrip().startswith(";"):
                continue
            if ZERO_COUNT_RE.search(line):
                fail(errors, f"{path.relative_to(ROOT)}:{line_number}: zero-count content")


def main() -> int:
    errors: list[str] = []
    validate_unit_blocks(errors)
    validate_purchase_tables(errors)
    validate_inherited_member_costs(errors)
    validate_roster_includes(errors)
    validate_no_zero_counts_in_active_sets(errors)

    if errors:
        print("Skirmish roster validation failed:", file=sys.stderr)
        for error in errors:
            print(f"  - {error}", file=sys.stderr)
        return 1

    print("Skirmish roster validation passed.")
    print(
        "Validated four factions, eight doctrines, positive DP costs, "
        "purchase resolution, and positive inherited member costs."
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
