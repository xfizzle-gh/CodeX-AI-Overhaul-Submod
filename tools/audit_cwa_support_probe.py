from __future__ import annotations

from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
MAP = ROOT / "resource/map"
OUT = ROOT / "docs/cwa_support_probe_audit.txt"
SELF = ROOT / "tools/audit_cwa_support_probe.py"
WORKFLOW = ROOT / ".github/workflows/audit-cwa-support-probe.yml"

TEXT_SUFFIXES = {".mi", ".inc", ".set", ".lua", ".txt"}
files = [p for p in MAP.rglob("*") if p.is_file()]
text_files = [p for p in files if p.suffix.lower() in TEXT_SUFFIXES]

def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()

def read(path: Path) -> str:
    try:
        return path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return ""

def context_hits(pattern: str, limit: int = 40) -> list[str]:
    rx = re.compile(pattern, re.IGNORECASE)
    results: list[str] = []
    for path in text_files:
        lines = read(path).splitlines()
        for index, line in enumerate(lines):
            if rx.search(line):
                start = max(0, index - 3)
                end = min(len(lines), index + 5)
                excerpt = "\n".join(f"  {i + 1}: {lines[i]}" for i in range(start, end))
                results.append(f"FILE {rel(path)}\n{excerpt}")
                if len(results) >= limit:
                    return results
    return results

cwa_paths = sorted(
    p for p in files
    if "cwa71" in rel(p).lower() or "[cwa" in rel(p).lower()
)
woodland_paths = sorted(p for p in files if "woodland" in rel(p).lower())

mission_like = [p for p in cwa_paths if p.suffix.lower() in {".mi", ".inc"}]
include_rows: list[str] = []
for path in mission_like:
    content = read(path)
    include_rows.append(
        f"{rel(path)} | dcg_script={('/map/multi/dcg_script.inc' in content or 'dcg_script.inc' in content)} "
        f"dcg_vars={'dcg_vars.inc' in content} dcg_functions={'dcg_functions' in content} "
        f"waypoints={content.lower().count('waypoint')} zones={content.lower().count('zone')}"
    )

sections: list[tuple[str, list[str]]] = [
    ("CWA FILE INVENTORY", [rel(p) for p in cwa_paths]),
    ("WOODLAND FILE CANDIDATES", [rel(p) for p in woodland_paths]),
    ("CWA MISSION INCLUDE SUMMARY", include_rows),
    ("DEFENSE LEVEL WRITES/READS", context_hits(r"defense_level\$?|SetVar\(\"defense_level")),
    ("DEFENDERBOT ID REFERENCES", context_hits(r"id_defenderbot\$?|DefenderBotId")),
    ("PLAYER OWNERSHIP SET OPERATIONS", context_hits(r"operation\s+set|\{\s*\"?player\"?")),
    ("CMP_DEF CLONE/PLACEMENT", context_hits(r"cmp_def|\{clone\}|\{placement")),
    ("WOODLAND WAYPOINT/ZONE DEFINITIONS", []),
]

woodland_details: list[str] = []
for path in woodland_paths:
    if path.suffix.lower() not in TEXT_SUFFIXES:
        continue
    lines = read(path).splitlines()
    hits = []
    for i, line in enumerate(lines):
        if re.search(r"waypoint|zone|dcg_script|dcg_vars|dcg_functions|cmp_def", line, re.IGNORECASE):
            hits.append(f"  {i + 1}: {line}")
            if len(hits) >= 160:
                hits.append("  ... truncated ...")
                break
    woodland_details.append(f"FILE {rel(path)}\n" + "\n".join(hits))
sections[-1] = (sections[-1][0], woodland_details)

lines_out = [
    "CWA ALLIED SUPPORT PROTOTYPE AUDIT",
    "Generated from the repository tree on the prototype branch.",
    "",
]
for title, rows in sections:
    lines_out.extend([f"=== {title} ===", *(rows or ["(none found)"]), ""])
OUT.parent.mkdir(parents=True, exist_ok=True)
OUT.write_text("\n".join(lines_out), encoding="utf-8")

SELF.unlink()
WORKFLOW.unlink()
