from __future__ import annotations

from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
WOODLAND = ROOT / "resource/map/multi/dcg_[cwa71]_woodland/campaign_capture_the_flag.mi"
MAP_MULTI = ROOT / "resource/map/multi"
OUT = ROOT / "docs/cwa_support_probe_details.txt"
SELF = ROOT / "tools/audit_cwa_support_details.py"
WORKFLOW = ROOT / ".github/workflows/audit-cwa-support-details.yml"


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def read(path: Path) -> str:
    return path.read_text(encoding="utf-8", errors="replace")


def excerpt(lines: list[str], center: int, before: int = 20, after: int = 40) -> str:
    start = max(0, center - before)
    end = min(len(lines), center + after + 1)
    return "\n".join(f"{i + 1}: {lines[i]}" for i in range(start, end))


def find_contexts(path: Path, pattern: str, limit: int = 20, before: int = 15, after: int = 30) -> list[str]:
    lines = read(path).splitlines()
    rx = re.compile(pattern, re.IGNORECASE)
    out: list[str] = []
    for i, line in enumerate(lines):
        if rx.search(line):
            out.append(f"FILE {rel(path)}\n{excerpt(lines, i, before, after)}")
            if len(out) >= limit:
                break
    return out

wood_lines = read(WOODLAND).splitlines()
wood_sections: list[str] = [
    f"line_count={len(wood_lines)}",
    f"dcg_script_occurrences={sum('dcg_script.inc' in line for line in wood_lines)}",
    f"waypoint_keyword_occurrences={sum('waypoint' in line.lower() for line in wood_lines)}",
    f"zone_keyword_occurrences={sum('zone' in line.lower() for line in wood_lines)}",
]
for pattern in (r"dcg_script\.inc", r"dcg_vars\.inc", r"dcg_functions", r"\{waypoints?\b", r"\{zones?\b", r"\{triggers?\b", r"\{mission\b"):
    wood_sections.extend(find_contexts(WOODLAND, pattern, limit=8, before=12, after=25))
wood_sections.append("WOODLAND LAST 180 LINES\n" + "\n".join(
    f"{i + 1}: {wood_lines[i]}" for i in range(max(0, len(wood_lines) - 180), len(wood_lines))
))

operation_contexts: list[str] = []
id_contexts: list[str] = []
clone_contexts: list[str] = []
defense_contexts: list[str] = []
for path in sorted(MAP_MULTI.rglob("*")):
    if not path.is_file() or path.suffix.lower() not in {".mi", ".inc"}:
        continue
    text = read(path)
    lines = text.splitlines()
    for i, line in enumerate(lines):
        low = line.lower()
        if "operation set" in low and len(operation_contexts) < 40:
            operation_contexts.append(f"FILE {rel(path)}\n{excerpt(lines, i, 25, 45)}")
        if "id_defenderbot$" in line and len(id_contexts) < 20:
            id_contexts.append(f"FILE {rel(path)}\n{excerpt(lines, i, 25, 60)}")
        if ("{clone}" in low or "{clone " in low) and len(clone_contexts) < 30:
            clone_contexts.append(f"FILE {rel(path)}\n{excerpt(lines, i, 30, 45)}")
        if "defense_level$" in line and ("set_i" in "\n".join(lines[max(0, i-5):i+6]) or "set_defense_level" in "\n".join(lines[max(0, i-20):i+20])) and len(defense_contexts) < 20:
            defense_contexts.append(f"FILE {rel(path)}\n{excerpt(lines, i, 35, 70)}")

parts = [
    "CWA SUPPORT PROBE DETAILS",
    "",
    "=== WOODLAND STRUCTURE ===",
    *wood_sections,
    "",
    "=== EXACT PLAYER OPERATION SET CONTEXTS ===",
    *(operation_contexts or ["(none found)"]),
    "",
    "=== ID_DEFENDERBOT CONTEXTS ===",
    *(id_contexts or ["(none found)"]),
    "",
    "=== CLONE CONTEXTS ===",
    *(clone_contexts or ["(none found)"]),
    "",
    "=== DEFENSE LEVEL INITIALIZATION CONTEXTS ===",
    *(defense_contexts or ["(none found)"]),
]
OUT.write_text("\n".join(parts), encoding="utf-8")
SELF.unlink()
WORKFLOW.unlink()
