from __future__ import annotations

import subprocess
import sys
from pathlib import Path

BRANCH = "experiment/attack-mate-slot-proof"
BASE_LOCAL = "7091c264b6233fd33186a551fa30794a5b216f19"
REMOTE_PARENT = "9dfb5ed8e05171ccf0be0b1c31c42975a052c011"
WORKSHOP = r"E:\Steam\steamapps\workshop\content\400750\3636883799"

FOCUSED = [
    "tests/test_motor_runtime_isolation.py",
    "tests/test_motor_package1_restore.py",
    "tests/test_attack_support_slot_proof.py",
    "tests/test_enemy_defense_support.py",
    "tests/test_defense_mission_support.py",
]

STAGE_FILES = [
    "resource/map/multi/attack_support_waves.inc",
    "resource/map/multi/defense_support_waves.inc",
    "resource/map/multi/enemy_attack_support.inc",
    "resource/map/multi/enemy_defense_support.inc",
    "tests/test_motor_runtime_isolation.py",
    "tests/test_attack_support_slot_proof.py",
    "tests/test_enemy_defense_support.py",
    "tests/test_defense_mission_support.py",
    "tools/deploy_attack_support_probe.ps1",
]

HELPERS = [
    "tools/apply_motor_driver_repair.py",
    "tools/resume_motor_driver_repair.py",
    "tools/resume_motor_driver_repair_v2.py",
    "tools/resume_motor_driver_repair_v3.py",
]


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


def patch_remaining_tests(root: Path) -> None:
    motor_path = root / "tests/test_motor_runtime_isolation.py"
    motor = motor_path.read_text(encoding="utf-8-sig")
    old_pattern = '''        pattern = re.compile(
            rf'\\{{"{re.escape(prefix)}/(?:ally_)?(?:rusa|ukr|nato|prc)_'
            r'(?P<role>line|wpn|recon|assault|eng|light)"'
        )
'''
    new_pattern = '''        pattern = re.compile(
            rf'\\{{"(?P<name>{re.escape(prefix)}/(?:ally_)?(?:rusa|ukr|nato|prc)_'
            r'(?P<role>line|wpn|recon|assault|eng|light))"'
        )
'''
    motor = replace_once(motor, old_pattern, new_pattern, "named infantry-trigger regex group")
    motor_path.write_text(motor, encoding="utf-8")

    defense_path = root / "tests/test_defense_mission_support.py"
    defense = defense_path.read_text(encoding="utf-8-sig")
    old_mix = '''        for _s, _c, _r, take, _st in EA_DRAWS:
            self.assertEqual(take, 6)
'''
    new_mix = '''        for _s, _c, role, take, _st in EA_DRAWS:
            self.assertEqual(take, 6 if role == "line" else 5)
'''
    defense = replace_once(defense, old_mix, new_mix, "enemy-attack 5-6 body mix assertion")
    defense_path.write_text(defense, encoding="utf-8")


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
    remote_runner = run(
        ["git", "show", f"{remote_head}:tools/resume_motor_driver_repair_v3.py"],
        root,
        capture=True,
    )
    if "patch_remaining_tests" not in remote_runner:
        raise RuntimeError("Remote branch does not contain the expected v3 recovery runner")

    patch_remaining_tests(root)

    run([
        sys.executable,
        "-m",
        "py_compile",
        "tests/test_motor_runtime_isolation.py",
        "tests/test_attack_support_slot_proof.py",
        "tests/test_enemy_defense_support.py",
        "tests/test_defense_mission_support.py",
    ], root)

    run([sys.executable, "-m", "pytest", *FOCUSED, "-q"], root)
    run([sys.executable, "-m", "pytest", "tests", "-q"], root)
    run(["git", "diff", "--check"], root)

    status = run(["git", "status", "--short"], root, capture=True)
    print(status)
    allowed = set(STAGE_FILES + HELPERS)
    seen: set[str] = set()
    for line in status.splitlines():
        path = line[3:].strip().replace("\\", "/")
        if " -> " in path:
            path = path.split(" -> ", 1)[1]
        seen.add(path)
    unexpected = seen - allowed
    if unexpected:
        raise RuntimeError(f"Unexpected changed files: {sorted(unexpected)}")

    run(["git", "add", "--", *STAGE_FILES], root)
    run(["git", "commit", "-m", "Fix motor driver release and enlarge infantry arrivals"], root)
    runtime_commit = run(["git", "rev-parse", "HEAD"], root, capture=True).strip()

    # Remove untracked helper copies before rebasing so the tracked remote copies
    # cannot collide with them.
    for helper in HELPERS:
        try:
            (root / helper).unlink()
        except FileNotFoundError:
            pass

    run(["git", "rebase", f"origin/{BRANCH}"], root)

    # The remote helper commits now exist in history. Delete all repair machinery
    # from the final branch state and commit only that cleanup.
    for helper in HELPERS:
        path = root / helper
        if path.exists():
            path.unlink()
    run(["git", "add", "-A", "--", *HELPERS], root)
    run(["git", "commit", "-m", "Remove temporary motor repair runners"], root)
    final_commit = run(["git", "rev-parse", "HEAD"], root, capture=True).strip()

    run(["git", "push", "origin", f"HEAD:refs/heads/{BRANCH}"], root)

    deploy = root / "tools/deploy_attack_support_probe.ps1"
    for number in (1, 2):
        print(f"\nDeployment run {number}...")
        run([
            "powershell",
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(deploy),
            "-RepoRoot",
            str(root),
            "-WorkshopRoot",
            WORKSHOP,
            "-E2TestMode",
            "0",
        ], root)
        print(f"Deployment run {number} passed.")

    print("\nVERIFIED: motor repair committed, pushed, and deployed twice.")
    print(f"Runtime repair before helper cleanup: {runtime_commit}")
    print(f"Commit for live test: {final_commit}")


if __name__ == "__main__":
    main()
