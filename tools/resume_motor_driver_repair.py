from __future__ import annotations

import os
import shutil
import subprocess
import sys
from pathlib import Path

BRANCH = "experiment/attack-mate-slot-proof"
WORKSHOP = r"E:\Steam\steamapps\workshop\content\400750\3636883799"

ALLOWED_DIRTY_PATHS = {
    "resource/map/multi/attack_support_waves.inc",
    "resource/map/multi/defense_support_waves.inc",
    "resource/map/multi/enemy_attack_support.inc",
    "resource/map/multi/enemy_defense_support.inc",
    "resource/map/multi/dcg_vars.inc",
    "tests/test_motor_runtime_isolation.py",
    "tests/test_motor_package1_restore.py",
    "tests/test_attack_support_slot_proof.py",
    "tests/test_enemy_defense_support.py",
    "tests/test_defense_mission_support.py",
    "tools/deploy_attack_support_probe.ps1",
    "tools/apply_motor_driver_repair.py",
    "tools/resume_motor_driver_repair.py",
}

FOCUSED_TESTS = [
    "tests/test_motor_runtime_isolation.py",
    "tests/test_motor_package1_restore.py",
    "tests/test_attack_support_slot_proof.py",
    "tests/test_enemy_defense_support.py",
    "tests/test_defense_mission_support.py",
]


def run(args: list[str], *, cwd: Path, capture: bool = False) -> str:
    print("+", subprocess.list2cmdline(args), flush=True)
    completed = subprocess.run(
        args,
        cwd=cwd,
        check=True,
        text=True,
        stdout=subprocess.PIPE if capture else None,
        stderr=subprocess.STDOUT if capture else None,
    )
    return completed.stdout.strip() if capture and completed.stdout else ""


def changed_paths(root: Path) -> set[str]:
    paths: set[str] = set()
    for args in (
        ["git", "diff", "--name-only"],
        ["git", "diff", "--cached", "--name-only"],
        ["git", "ls-files", "--others", "--exclude-standard"],
    ):
        output = run(args, cwd=root, capture=True)
        paths.update(line.strip().replace("\\", "/") for line in output.splitlines() if line.strip())
    return paths


def patch_generated_test(root: Path) -> None:
    path = root / "tests/test_motor_runtime_isolation.py"
    text = path.read_text(encoding="utf-8-sig")
    bad = "        assert f'{{count {{op \">=\"}} {{value {target}}}}' in trigger, name"
    good = "        assert f'{{count {{op \">=\"}} {{value {target}}}}}' in trigger, name"
    bad_count = text.count(bad)
    good_count = text.count(good)
    if bad_count == 1:
        path.write_text(text.replace(bad, good), encoding="utf-8")
        print("Corrected the malformed motor-count assertion.")
    elif bad_count == 0 and good_count == 1:
        print("Motor-count assertion was already corrected.")
    else:
        raise RuntimeError(
            f"Expected exactly one malformed or corrected assertion; found bad={bad_count}, good={good_count}."
        )


def remove_if_present(path: Path) -> None:
    try:
        path.unlink()
    except FileNotFoundError:
        pass


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    branch = run(["git", "branch", "--show-current"], cwd=root, capture=True)
    if branch != BRANCH:
        raise RuntimeError(f"Wrong branch: {branch!r}. Expected {BRANCH!r}.")

    patch_generated_test(root)
    run([sys.executable, "-m", "py_compile", "tests/test_motor_runtime_isolation.py"], cwd=root)
    run([sys.executable, "-m", "pytest", *FOCUSED_TESTS, "-q"], cwd=root)
    run([sys.executable, "-m", "pytest", "tests/", "-q"], cwd=root)
    run(["git", "diff", "--check"], cwd=root)

    dirty = changed_paths(root)
    unexpected = dirty - ALLOWED_DIRTY_PATHS
    if unexpected:
        raise RuntimeError("Refusing to commit unexpected paths: " + ", ".join(sorted(unexpected)))
    required = {
        "resource/map/multi/attack_support_waves.inc",
        "resource/map/multi/defense_support_waves.inc",
        "resource/map/multi/enemy_attack_support.inc",
        "resource/map/multi/enemy_defense_support.inc",
        "tests/test_motor_runtime_isolation.py",
    }
    missing = required - dirty
    if missing:
        raise RuntimeError("Generated repair is missing expected paths: " + ", ".join(sorted(missing)))

    helper = root / "tools/resume_motor_driver_repair.py"
    old_runner = root / "tools/apply_motor_driver_repair.py"
    remove_if_present(old_runner)
    remove_if_present(helper)

    run(["git", "add", "-A"], cwd=root)
    run(["git", "commit", "-m", "Repair motor transport driver lifecycle"], cwd=root)

    run(["git", "fetch", "origin"], cwd=root)
    run(["git", "rebase", f"origin/{BRANCH}"], cwd=root)
    if (root / "tools/resume_motor_driver_repair.py").exists():
        run(["git", "rm", "--force", "tools/resume_motor_driver_repair.py"], cwd=root)
        run(["git", "commit", "--amend", "--no-edit"], cwd=root)

    run([sys.executable, "-m", "pytest", "tests/", "-q"], cwd=root)
    run(["git", "diff", "--check", "HEAD^", "HEAD"], cwd=root)
    status = run(["git", "status", "--porcelain=v1"], cwd=root, capture=True)
    if status:
        raise RuntimeError("Working tree is not clean after repair:\n" + status)

    run(["git", "push", "origin", f"HEAD:{BRANCH}"], cwd=root)
    commit = run(["git", "rev-parse", "HEAD"], cwd=root, capture=True)

    shell = shutil.which("powershell") or shutil.which("pwsh")
    if not shell:
        raise RuntimeError("PowerShell was not found; cannot run the guarded workshop deployment.")
    deploy = root / "tools/deploy_attack_support_probe.ps1"
    deploy_args = [
        shell,
        "-NoProfile",
        "-ExecutionPolicy",
        "Bypass",
        "-File",
        str(deploy),
        "-RepoRoot",
        str(root),
        "-WorkshopRoot",
        WORKSHOP,
    ]
    for run_number in (1, 2):
        print(f"\n=== GUARDED DEPLOYMENT RUN {run_number}/2 ===", flush=True)
        run(deploy_args, cwd=root)

    print("\nVERIFIED: motor repair committed, pushed, and deployed twice.")
    print(f"Commit for live test: {commit}")


if __name__ == "__main__":
    try:
        main()
    except subprocess.CalledProcessError as exc:
        print(f"\nFAILED: command exited with status {exc.returncode}.", file=sys.stderr)
        raise
