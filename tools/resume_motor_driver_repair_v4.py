from __future__ import annotations

import subprocess
import sys
from pathlib import Path

BRANCH = "experiment/attack-mate-slot-proof"
BASE_LOCAL = "7091c264b6233fd33186a551fa30794a5b216f19"
REMOTE_PARENT = "6e5f8b600068e1e4e7ac8e07c260c78c8f50637c"


def run(args: list[str], cwd: Path, *, capture: bool = False) -> str:
    print("+", subprocess.list2cmdline(args))
    completed = subprocess.run(
        args,
        cwd=cwd,
        check=True,
        text=True,
        stdout=subprocess.PIPE if capture else None,
        stderr=subprocess.STDOUT if capture else None,
    )
    return completed.stdout or ""


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count == 1:
        print(f"Correcting {label}.")
        return text.replace(old, new)
    if count == 0 and new in text:
        print(f"{label} was already corrected.")
        return text
    raise RuntimeError(f"{label}: expected one old marker, found {count}")


def main() -> None:
    root = Path(__file__).resolve().parents[1]

    branch = run(["git", "branch", "--show-current"], root, capture=True).strip()
    if branch != BRANCH:
        raise RuntimeError(f"Wrong branch: {branch}")

    local_head = run(["git", "rev-parse", "HEAD"], root, capture=True).strip()
    if local_head != BASE_LOCAL:
        raise RuntimeError(f"Unexpected local HEAD: {local_head}; expected {BASE_LOCAL}")

    remote_head = run(["git", "rev-parse", f"origin/{BRANCH}"], root, capture=True).strip()
    run(["git", "merge-base", "--is-ancestor", REMOTE_PARENT, remote_head], root)
    remote_v4 = run(
        ["git", "show", f"{remote_head}:tools/resume_motor_driver_repair_v4.py"],
        root,
        capture=True,
    )
    if "per-group advance/drop assertion" not in remote_v4:
        raise RuntimeError("Remote branch does not contain the expected v4 recovery runner")

    test_path = root / "tests/test_defense_mission_support.py"
    test_text = test_path.read_text(encoding="utf-8-sig")
    old_assertion = '''        # All three groups advance rather than beelining and drop prior orders.
        self.assertEqual(finish.count("{action advance}"), 3)
        self.assertEqual(finish.count("{drop orders}"), 3)
'''
    new_assertion = '''        # Check the three named fireteams directly. ea_g2 has two alternative
        # advance branches, so a global count is intentionally greater than three.
        for group in ("ea_g1", "ea_g2", "ea_g3"):
            group_orders = []
            for match in re.finditer(r'\\{"action"', finish):
                order = block_at(finish, match.start())
                if f"{{tag {group}}}" in order:
                    group_orders.append(order)
            self.assertTrue(
                any("{action advance}" in order for order in group_orders), group
            )
            self.assertTrue(
                any("{drop orders}" in order for order in group_orders), group
            )
'''
    test_text = replace_once(
        test_text,
        old_assertion,
        new_assertion,
        "per-group advance/drop assertion",
    )
    test_path.write_text(test_text, encoding="utf-8")

    # Refresh the v3 runner from the remote branch, then teach its allowlist and
    # cleanup phase about this final v4 helper before executing it.
    v3_text = run(
        ["git", "show", f"{remote_head}:tools/resume_motor_driver_repair_v3.py"],
        root,
        capture=True,
    )
    old_helpers = '''    "tools/resume_motor_driver_repair_v3.py",
]
'''
    new_helpers = '''    "tools/resume_motor_driver_repair_v3.py",
    "tools/resume_motor_driver_repair_v4.py",
]
'''
    v3_text = replace_once(
        v3_text,
        old_helpers,
        new_helpers,
        "v4 helper cleanup registration",
    )
    v3_path = root / "tools/resume_motor_driver_repair_v3.py"
    v3_path.write_text(v3_text, encoding="utf-8")

    run([sys.executable, "-m", "py_compile", str(v3_path), str(test_path)], root)
    run([sys.executable, str(v3_path)], root)


if __name__ == "__main__":
    main()
