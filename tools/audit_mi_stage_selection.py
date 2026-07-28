from __future__ import annotations

from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
MAP = ROOT / "resource/map"
OUT = ROOT / "docs/mi_stage_selection_audit.txt"
SELF = ROOT / "tools/audit_mi_stage_selection.py"
WORKFLOW = ROOT / ".github/workflows/audit-mi-stage-selection.yml"


def rel(p: Path) -> str:
    return p.relative_to(ROOT).as_posix()


def read(p: Path) -> str:
    return p.read_text(encoding="utf-8", errors="replace")


def ctx(lines: list[str], i: int, before: int = 18, after: int = 32) -> str:
    s, e = max(0, i-before), min(len(lines), i+after+1)
    return "\n".join(f"{n+1}: {lines[n]}" for n in range(s, e))

sections: dict[str, list[str]] = {
    "TOP LEVEL ZONES BLOCKS": [],
    "WAYPOINT SELECTOR OR NEAR_TO USES": [],
    "TARGET_WAYPOINT PLACEMENT USES": [],
    "TEMP TAG THEN CLONE PATTERNS": [],
    "PLAYER OPERATION SET IDS": [],
}

player_ids: set[int] = set()
for path in sorted(MAP.rglob("*")):
    if not path.is_file() or path.suffix.lower() not in {".mi", ".inc"}:
        continue
    lines = read(path).splitlines()
    for i, line in enumerate(lines):
        low = line.lower()
        if re.search(r"^\s*\{zones\b", line, re.IGNORECASE) and len(sections["TOP LEVEL ZONES BLOCKS"]) < 30:
            sections["TOP LEVEL ZONES BLOCKS"].append(f"FILE {rel(path)}\n{ctx(lines, i)}")
        if "waypoint" in low and ("selector" in "\n".join(lines[max(0,i-15):i+2]).lower() or "near_to" in "\n".join(lines[max(0,i-12):i+2]).lower()) and len(sections["WAYPOINT SELECTOR OR NEAR_TO USES"]) < 40:
            sections["WAYPOINT SELECTOR OR NEAR_TO USES"].append(f"FILE {rel(path)}\n{ctx(lines, i)}")
        if "target_waypoint" in low and len(sections["TARGET_WAYPOINT PLACEMENT USES"]) < 25:
            sections["TARGET_WAYPOINT PLACEMENT USES"].append(f"FILE {rel(path)}\n{ctx(lines, i, 28, 25)}")
        if "{clone}" in low or "{clone " in low:
            neighborhood = "\n".join(lines[max(0,i-70):i+20]).lower()
            if "tag_add" in neighborhood and len(sections["TEMP TAG THEN CLONE PATTERNS"]) < 25:
                sections["TEMP TAG THEN CLONE PATTERNS"].append(f"FILE {rel(path)}\n{ctx(lines, i, 70, 35)}")
        if "{operation set}" in low:
            neighborhood = "\n".join(lines[i:i+8])
            m = re.search(r'\{player\s+"(\d+)"\}', neighborhood)
            if m:
                player_ids.add(int(m.group(1)))
                if len(sections["PLAYER OPERATION SET IDS"]) < 40:
                    sections["PLAYER OPERATION SET IDS"].append(f"FILE {rel(path)}\n{ctx(lines, i, 24, 20)}")

parts = ["MI STAGE SELECTION AUDIT", f"player_operation_set_ids={sorted(player_ids)}", ""]
for title, rows in sections.items():
    parts.extend([f"=== {title} ===", *(rows or ["(none found)"]), ""])
OUT.write_text("\n".join(parts), encoding="utf-8")
SELF.unlink()
WORKFLOW.unlink()
