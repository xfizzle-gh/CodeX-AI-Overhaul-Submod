from pathlib import Path

root = Path("resource/map/multi")
paths = sorted(
    path for path in root.glob("dcg_*/campaign_capture_the_flag.mi")
    if path.parent.name.startswith("dcg_[cwa71]_")
)
for path in paths:
    lines = path.read_text(encoding="utf-8").splitlines()
    print(f"## {path.parent.name} lines={len(lines)}")
    for number, line in enumerate(lines, 1):
        stripped = line.strip()
        if (
            "dcg_script" in line
            or "allied_support_ownership_probe" in line
            or stripped.startswith("(include")
            or stripped == "{waypoints"
        ):
            print(f"{number}: {line}")
    print()
