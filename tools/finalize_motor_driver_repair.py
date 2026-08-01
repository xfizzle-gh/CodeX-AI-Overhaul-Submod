from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

BRANCH = "experiment/attack-mate-slot-proof"
BASE_LOCAL = "7091c264b6233fd33186a551fa30794a5b216f19"
REMOTE_PARENT = "1546545f60cbf31d84c23bd8566f37623f429c64"
WORKSHOP = r"E:\Steam\steamapps\workshop\content\400750\3636883799"

FOCUSED = [
    "tests/test_motor_runtime_isolation.py",
    "tests/test_motor_package1_restore.py",
    "tests/test_attack_support_slot_proof.py",
    "tests/test_enemy_defense_support.py",
    "tests/test_defense_mission_support.py",
    "tests/test_e2_airmobile.py",
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
    "tests/test_e2_airmobile.py",
    "tools/deploy_attack_support_probe.ps1",
]

HELPERS = [
    "tools/apply_motor_driver_repair.py",
    "tools/resume_motor_driver_repair.py",
    "tools/resume_motor_driver_repair_v2.py",
    "tools/resume_motor_driver_repair_v3.py",
    "tools/resume_motor_driver_repair_v4.py",
    "tools/finalize_motor_driver_repair.py",
]


def run(args: list[str], cwd: Path, *, capture: bool = False) -> str:
    print("+", subprocess.list2cmdline(args), flush=True)
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
        print(f"Correcting {label}.", flush=True)
        return text.replace(old, new)
    if count == 0 and new in text:
        print(f"{label} was already corrected.", flush=True)
        return text
    raise RuntimeError(f"{label}: expected one old marker, found {count}")


def normalize_generated_whitespace(path: Path) -> None:
    """Remove trailing horizontal whitespace and enforce one final newline.

    This preserves the file's existing LF/CRLF convention and changes no non-whitespace
    content. The original generator emitted eight whitespace-only lines plus one extra
    blank line at EOF; git diff --check correctly rejected them after all tests passed.
    """
    data = path.read_bytes()
    newline = b"\r\n" if b"\r\n" in data else b"\n"
    cleaned = re.sub(rb"[ \t]+(?=\r?\n)", b"", data)
    cleaned = re.sub(rb"[ \t]+\Z", b"", cleaned)
    cleaned = re.sub(rb"(?:\r?\n)+\Z", b"", cleaned) + newline
    if cleaned != data:
        path.write_bytes(cleaned)
        print(f"Normalized generated whitespace: {path}", flush=True)


def normalize_status_path(line: str) -> str:
    path = line[3:].strip().replace("\\", "/")
    if " -> " in path:
        path = path.split(" -> ", 1)[1]
    return path.strip('"')


def main() -> None:
    root = Path(__file__).resolve().parents[1]

    branch = run(["git", "branch", "--show-current"], root, capture=True).strip()
    if branch != BRANCH:
        raise RuntimeError(f"Wrong branch: {branch}")

    local_head = run(["git", "rev-parse", "HEAD"], root, capture=True).strip()
    if local_head != BASE_LOCAL:
        raise RuntimeError(
            f"Unexpected local HEAD: {local_head}; expected {BASE_LOCAL}. "
            "No reset was performed."
        )

    run(["git", "fetch", "origin"], root)
    remote_head = run(
        ["git", "rev-parse", f"origin/{BRANCH}"], root, capture=True
    ).strip()
    run(["git", "merge-base", "--is-ancestor", REMOTE_PARENT, remote_head], root)

    remote_finalizer = run(
        ["git", "show", f"{remote_head}:tools/finalize_motor_driver_repair.py"],
        root,
        capture=True,
    )
    if "standalone motor repair finalizer v2" not in remote_finalizer.lower():
        raise RuntimeError("Remote branch does not contain the expected v2 finalizer")

    e2_path = root / "tests/test_e2_airmobile.py"
    e2_text = e2_path.read_text(encoding="utf-8-sig")
    old_e2 = '        self.assertIn("if ($E2TestMode -ne 0)", self.deploy)\n'
    new_e2 = '''        self.assertNotIn("if ($E2TestMode -ne 0)", self.deploy)
        self.assertIn("$expectedLegacyMode = 0", self.deploy)
        self.assertNotIn("$sourceLegacyInit =", self.deploy)
'''
    e2_text = replace_once(
        e2_text,
        old_e2,
        new_e2,
        "retired E1 deploy-contract assertion",
    )
    e2_path.write_text(e2_text, encoding="utf-8")

    required_markers = {
        "tests/test_motor_runtime_isolation.py": [
            "(?P<name>",
            "driver must not receive a separate actor-state activation",
            "test_standard_infantry_arrivals_are_five_or_six",
        ],
        "tests/test_attack_support_slot_proof.py": [
            "at least two complete",
            "the second releases the completed",
            "Stage 4 now means movement was proved",
        ],
        "tests/test_enemy_defense_support.py": [
            "Standard arrivals are now five or six bodies",
        ],
        "tests/test_defense_mission_support.py": [
            'by_group = {"ea_g1": set(), "ea_g2": set(), "ea_g3": set()}',
            "Check the three named fireteams directly",
            '6 if role == "line" else 5',
        ],
        "tools/deploy_attack_support_probe.ps1": [
            "$expectedLegacyMode = 0",
            "Legacy E1 is retired for every deployment mode",
        ],
    }
    for relative, markers in required_markers.items():
        text = (root / relative).read_text(encoding="utf-8-sig")
        for marker in markers:
            if marker not in text:
                raise RuntimeError(f"Missing required repaired marker in {relative}: {marker}")

    deploy_text = (root / "tools/deploy_attack_support_probe.ps1").read_text(
        encoding="utf-8-sig"
    )
    if "$sourceLegacyInit =" in deploy_text or "if ($E2TestMode -ne 0)" in deploy_text:
        raise RuntimeError("Obsolete legacy-E1 deployment override is still present")

    for relative in STAGE_FILES:
        normalize_generated_whitespace(root / relative)

    run(
        [
            sys.executable,
            "-m",
            "py_compile",
            "tests/test_motor_runtime_isolation.py",
            "tests/test_attack_support_slot_proof.py",
            "tests/test_enemy_defense_support.py",
            "tests/test_defense_mission_support.py",
            "tests/test_e2_airmobile.py",
        ],
        root,
    )

    print("\nRunning focused motor/support validation...", flush=True)
    run([sys.executable, "-m", "pytest", *FOCUSED, "-q"], root)

    print("\nRunning complete repository suite...", flush=True)
    run([sys.executable, "-m", "pytest", "tests", "-q"], root)
    run(["git", "diff", "--check"], root)

    status = run(["git", "status", "--short"], root, capture=True)
    print("\nValidated working-tree changes:\n" + status, flush=True)
    allowed = set(STAGE_FILES + HELPERS)
    seen = {normalize_status_path(line) for line in status.splitlines() if line.strip()}
    unexpected = seen - allowed
    if unexpected:
        raise RuntimeError(f"Unexpected changed files: {sorted(unexpected)}")

    required_changed = {
        "resource/map/multi/attack_support_waves.inc",
        "resource/map/multi/defense_support_waves.inc",
        "resource/map/multi/enemy_attack_support.inc",
        "resource/map/multi/enemy_defense_support.inc",
        "tests/test_motor_runtime_isolation.py",
        "tests/test_e2_airmobile.py",
        "tools/deploy_attack_support_probe.ps1",
    }
    missing = required_changed - seen
    if missing:
        raise RuntimeError(f"Expected repair files are not modified: {sorted(missing)}")

    run(["git", "add", "--", *STAGE_FILES], root)
    run(
        [
            "git",
            "commit",
            "-m",
            "Fix motor driver release and enlarge infantry arrivals",
        ],
        root,
    )
    runtime_commit_before_rebase = run(
        ["git", "rev-parse", "HEAD"], root, capture=True
    ).strip()

    # Remove only untracked helper copies before rebase. The original apply utility is
    # tracked at BASE_LOCAL and must remain present until the remote helper history is
    # replayed; deleting it here would make rebase refuse a dirty tracked deletion.
    for helper in HELPERS:
        helper_status = run(
            ["git", "status", "--porcelain", "--", helper], root, capture=True
        )
        if helper_status.startswith("?? "):
            (root / helper).unlink(missing_ok=True)

    run(["git", "rebase", f"origin/{BRANCH}"], root)

    for helper in HELPERS:
        path = root / helper
        if path.exists():
            path.unlink()
    run(["git", "add", "-A", "--", *HELPERS], root)
    run(["git", "commit", "-m", "Remove temporary motor repair utilities"], root)

    final_commit = run(["git", "rev-parse", "HEAD"], root, capture=True).strip()
    clean_before_push = run(["git", "status", "--porcelain"], root, capture=True)
    if clean_before_push.strip():
        raise RuntimeError("Repository is not clean before push:\n" + clean_before_push)

    run(["git", "push", "origin", f"HEAD:refs/heads/{BRANCH}"], root)
    remote_after_push = run(
        ["git", "ls-remote", "origin", f"refs/heads/{BRANCH}"],
        root,
        capture=True,
    ).split()[0]
    if remote_after_push != final_commit:
        raise RuntimeError(
            f"Remote verification failed: expected {final_commit}, got {remote_after_push}"
        )

    deploy = root / "tools/deploy_attack_support_probe.ps1"
    for number in (1, 2):
        print(f"\nDeployment run {number}...", flush=True)
        run(
            [
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
            ],
            root,
        )
        print(f"Deployment run {number} passed.", flush=True)

    run(["git", "diff", "--check"], root)
    final_status = run(["git", "status", "--porcelain"], root, capture=True)
    if final_status.strip():
        raise RuntimeError(
            "Deployment completed but changed tracked repository files:\n" + final_status
        )

    print("\nVERIFIED: full suite passed; motor repair committed, pushed, and deployed twice.")
    print(f"Runtime repair before rebase: {runtime_commit_before_rebase}")
    print(f"Commit for live test: {final_commit}")


if __name__ == "__main__":
    main()

# Standalone motor repair finalizer v2.
