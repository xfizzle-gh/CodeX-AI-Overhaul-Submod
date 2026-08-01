from __future__ import annotations

import re
import subprocess
import sys
from pathlib import Path

BRANCH = "experiment/attack-mate-slot-proof"
BASE_LOCAL = "7091c264b6233fd33186a551fa30794a5b216f19"
REMOTE_PARENT = "8dd2f03b678e093be127aa40c14bdb066aff6656"
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
        return text.replace(old, new)
    if count == 0 and new in text:
        print(f"{label}: already corrected")
        return text
    raise RuntimeError(f"{label}: expected one old marker, found {count}")


def replace_function(text: str, name: str, new_body: str) -> str:
    pattern = re.compile(
        rf"^def {re.escape(name)}\(\) -> None:\n.*?(?=^def |\Z)",
        re.M | re.S,
    )
    matches = list(pattern.finditer(text))
    if len(matches) != 1:
        raise RuntimeError(f"{name}: expected one function, found {len(matches)}")
    return text[:matches[0].start()] + new_body.rstrip() + "\n\n" + text[matches[0].end():]


def find_balanced(text: str, marker: str, opener: str, closer: str) -> tuple[int, int]:
    start = text.index(marker)
    start = text.index(opener, start)
    depth = 0
    quoted = False
    escaped = False
    in_comment = False
    for pos in range(start, len(text)):
        ch = text[pos]
        if in_comment:
            if ch == "\n":
                in_comment = False
            continue
        if quoted:
            if escaped:
                escaped = False
            elif ch == "\\":
                escaped = True
            elif ch == '"':
                quoted = False
            continue
        if ch == ";":
            in_comment = True
        elif ch == '"':
            quoted = True
        elif ch == opener:
            depth += 1
        elif ch == closer:
            depth -= 1
            if depth == 0:
                return start, pos + 1
    raise RuntimeError(f"Unbalanced block for {marker}")


def patch_trigger_count(path: Path, trigger_name: str, old: int, new: int) -> None:
    text = path.read_text(encoding="utf-8-sig")
    marker = '{"' + trigger_name + '"'
    start, end = find_balanced(text, marker, "{", "}")
    block = text[start:end]
    old_count = f'{{count {{op ">="}} {{value {old}}}}}'
    new_count = f'{{count {{op ">="}} {{value {new}}}}}'
    old_amount = f"{{amount {old}}}"
    new_amount = f"{{amount {new}}}"
    block = replace_once(block, old_count, new_count, f"{trigger_name} gate")
    block = replace_once(block, old_amount, new_amount, f"{trigger_name} claim")
    path.write_text(text[:start] + block + text[end:], encoding="utf-8")


def patch_motor_tests(root: Path) -> None:
    path = root / "tests/test_motor_runtime_isolation.py"
    text = path.read_text(encoding="utf-8-sig")
    head = "\n".join(text.splitlines()[:8])
    if "import re" not in head:
        text = replace_once(
            text,
            "from __future__ import annotations\n\nfrom pathlib import Path\n",
            "from __future__ import annotations\n\nimport re\nfrom pathlib import Path\n",
            "motor test re import",
        )

    old_crew = '''        crew_actor = [
            command for command in body[:drive].split('{"actor_state"')[1:]
            if f'{{tag {crew}}}' in command[:800]
        ]
        assert not crew_actor
'''
    new_crew = '''        # A linked driver must not receive a separate actor-state activation before
        # the hull moves. The previous split-based check bled into the following
        # commands and falsely classified the hull actor-state block as a crew block.
        assert (
            f'{{selector {{ignore_captured_by_user 0}} {{tag {crew}}}}}'
            not in prefix
        )
'''
    text = replace_once(text, old_crew, new_crew, "crew actor-state assertion")

    new_standard = r'''def test_standard_infantry_arrivals_are_five_or_six() -> None:
    configs = (
        ("resource/map/multi/attack_support_waves.inc", "attack_support"),
        ("resource/map/multi/defense_support_waves.inc", "defense_support"),
        ("resource/map/multi/enemy_attack_support.inc", "enemy_attack"),
        ("resource/map/multi/enemy_defense_support.inc", "enemy_defense"),
    )
    for path, prefix in configs:
        text = (ROOT / path).read_text(encoding="utf-8-sig")
        pattern = re.compile(
            rf'\{{"{re.escape(prefix)}/(?:ally_)?(?:rusa|ukr|nato|prc)_'
            r'(?P<role>line|wpn|recon|assault|eng|light)"'
        )
        matches = list(pattern.finditer(text))
        assert matches, path
        for match in matches:
            name = match.group("name")
            role = match.group("role")
            target = 6 if role == "line" else 5
            trigger = block(text, '{"' + name + '"')
            assert f'{{count {{op ">="}} {{value {target}}}}}' in trigger, name
            assert f'{{amount {target}}}' in trigger, name
'''
    text = replace_function(text, "test_standard_infantry_arrivals_are_five_or_six", new_standard)
    path.write_text(text, encoding="utf-8")


def patch_attack_slot_tests(root: Path) -> None:
    path = root / "tests/test_attack_support_slot_proof.py"
    text = path.read_text(encoding="utf-8-sig")

    text = replace_once(
        text,
        '''                    # A pool must field at least four consecutive draws of its own
                    # comp; beyond that the gate declines and the pick falls back to
                    # the faction line pool rather than deploying a partial team.
                    self.assertGreaterEqual(depth // take, 4, tag)
''',
        '''                    # Larger 5-6 body arrivals intentionally consume specialist
                    # pools faster. Every pool must still field at least two complete
                    # squads; after that the command falls back to the deep line pool
                    # rather than deploying a partial team.
                    self.assertGreaterEqual(depth // take, 2, tag)
''',
        "faction pool draw floor",
    )

    text = replace_once(
        text,
        '''                self.assertEqual(
                    code.count('{"set_i" {var "%s$"} {op "="} {value 0}}' % var), 1
                )
''',
        '''                # One reset is initialization; the second releases the completed
                # or failed lifecycle so another truck package may be dispatched.
                self.assertEqual(
                    code.count('{"set_i" {var "%s$"} {op "="} {value 0}}' % var), 2
                )
''',
        "motor stage reset count",
    )

    text = replace_once(
        text,
        '''                self.assertLess(stage3, emit)
                self.assertLess(emit, stage4)
                self.assertLess(stage4, pax_source)
''',
        '''                # Stage 4 now means movement was proved and unload was released.
                # It is written immediately before emit; passengers become normal
                # infantry only after the emit command completes.
                self.assertLess(stage3, stage4)
                self.assertLess(stage4, emit)
                self.assertLess(emit, pax_source)
''',
        "motor emit ordering",
    )

    path.write_text(text, encoding="utf-8")


def patch_enemy_defense_tests(root: Path) -> None:
    path = root / "tests/test_enemy_defense_support.py"
    text = path.read_text(encoding="utf-8-sig")
    text = replace_once(
        text,
        '''        # Every draw is a 3-4 body fireteam.
        for _suffix, _cmd, _role, take, _stage in DRAWS:
            self.assertGreaterEqual(take, 3)
            self.assertLessEqual(take, 4)
''',
        '''        # Standard arrivals are now five or six bodies. They remain small
        # enough to maneuver independently and still use the same no-retreat policy.
        for _suffix, _cmd, _role, take, _stage in DRAWS:
            self.assertGreaterEqual(take, 5)
            self.assertLessEqual(take, 6)
''',
        "enemy defense squad-size assertion",
    )
    path.write_text(text, encoding="utf-8")


def patch_enemy_attack_finish(root: Path) -> None:
    path = root / "resource/map/multi/enemy_attack_support.inc"
    text = path.read_text(encoding="utf-8-sig")
    start, end = find_balanced(text, '(define "ea_finish"', "(", ")")
    body = text[start:end]

    body = replace_once(
        body,
        '''\t\t\t\t; Split the deploy into two staggered fireteams so a wave does not laser
\t\t\t\t; in as one blob. Every draw here is four bodies, so two pairs is the
\t\t\t\t; whole wave: a third group would only ever be an order on an empty
\t\t\t\t; selector.
''',
        '''\t\t\t\t; Split the six-body arrival into three staggered pairs so the wave does
\t\t\t\t; not laser in as one blob. A smaller fallback draw leaves the final
\t\t\t\t; selector empty, which is safe.
''',
        "enemy attack split comment",
    )

    old_g2 = '''\t\t\t\t{"entity_state"
\t\t\t\t\t{selector
\t\t\t\t\t\t{source advanced}
\t\t\t\t\t\t{group
\t\t\t\t\t\t\t{select {tag {tag ea_deploy}}}
\t\t\t\t\t\t\t{exclude {tag {tag ea_g1}}}
\t\t\t\t\t\t}
\t\t\t\t\t}
\t\t\t\t\t{tag_add ea_g2}
\t\t\t\t}
'''
    new_g2_g3 = '''\t\t\t\t{"entity_state"
\t\t\t\t\t{selector
\t\t\t\t\t\t{source advanced}
\t\t\t\t\t\t{group
\t\t\t\t\t\t\t{select {tag {tag ea_deploy}}}
\t\t\t\t\t\t\t{exclude {tag {tag ea_g1}}}
\t\t\t\t\t\t}
\t\t\t\t\t\t{amount 2}
\t\t\t\t\t}
\t\t\t\t\t{tag_add ea_g2}
\t\t\t\t}
\t\t\t\t{"delay" {time 0.05}}
\t\t\t\t{"entity_state"
\t\t\t\t\t{selector
\t\t\t\t\t\t{source advanced}
\t\t\t\t\t\t{group
\t\t\t\t\t\t\t{select {tag {tag ea_deploy}}}
\t\t\t\t\t\t\t{exclude
\t\t\t\t\t\t\t\t{tag {tag ea_g1}}
\t\t\t\t\t\t\t\t{tag {tag ea_g2}}
\t\t\t\t\t\t\t}
\t\t\t\t\t\t}
\t\t\t\t\t}
\t\t\t\t\t{tag_add ea_g3}
\t\t\t\t}
'''
    body = replace_once(body, old_g2, new_g2_g3, "enemy attack third group")

    old_tail = '''\t\t\t\t}
\t\t\t\t{"delay" {time 0.2}}
\t\t\t\t{"entity_state"
'''
    new_tail = '''\t\t\t\t}
\t\t\t\t{"delay" {time 0.35}}
\t\t\t\t{"action"
\t\t\t\t\t{selector {ignore_captured_by_user 0} {tag ea_g3}}
\t\t\t\t\t{drop orders}
\t\t\t\t\t{action advance}
\t\t\t\t\t{target {ignore_captured_by_user 0} {tag ea_flag3}}
\t\t\t\t}
\t\t\t\t{"delay" {time 0.2}}
\t\t\t\t{"entity_state"
'''
    idx = body.rfind(old_tail)
    if idx < 0:
        if "{tag ea_g3}" not in body:
            raise RuntimeError("enemy attack third-group order marker not found")
    else:
        body = body[:idx] + new_tail + body[idx + len(old_tail):]

    body = replace_once(
        body,
        '''\t\t\t\t\t{tag_remove ea_g1}
\t\t\t\t\t{tag_remove ea_g2}
''',
        '''\t\t\t\t\t{tag_remove ea_g1}
\t\t\t\t\t{tag_remove ea_g2}
\t\t\t\t\t{tag_remove ea_g3}
''',
        "enemy attack third-group cleanup",
    )
    path.write_text(text[:start] + body + text[end:], encoding="utf-8")


def patch_defense_mission_tests(root: Path) -> None:
    path = root / "tests/test_defense_mission_support.py"
    text = path.read_text(encoding="utf-8-sig")

    text = replace_once(
        text,
        '''        # Two staggered fireteams. Every draw is four bodies, so two pairs is the whole
        # wave and a third group would only ever order an empty selector.
        for n in (1, 2):
            self.assertIn("{tag_add ea_g%d}" % n, finish)
            self.assertIn("{tag_remove ea_g%d}" % n, finish)
        self.assertNotIn("ea_g3", finish)
        self.assertEqual(finish.count("{amount 2}"), 1)
        for _s, _c, _r, take, _st in EA_DRAWS:
            self.assertEqual(take, 4)
''',
        '''        # Six-body arrivals are split into three staggered pairs.
        for n in (1, 2, 3):
            self.assertIn("{tag_add ea_g%d}" % n, finish)
            self.assertIn("{tag_remove ea_g%d}" % n, finish)
        self.assertGreaterEqual(finish.count("{amount 2}"), 2)
        for _s, _c, _r, take, _st in EA_DRAWS:
            self.assertEqual(take, 6)
''',
        "enemy attack three-group test",
    )
    path.write_text(text, encoding="utf-8")


def main() -> None:
    root = Path(__file__).resolve().parents[1]
    branch = run(["git", "branch", "--show-current"], root, capture=True).strip()
    if branch != BRANCH:
        raise RuntimeError(f"Wrong branch: {branch}")

    head = run(["git", "rev-parse", "HEAD"], root, capture=True).strip()
    if head != BASE_LOCAL:
        raise RuntimeError(f"Unexpected local HEAD: {head}; expected {BASE_LOCAL}")

    remote = run(["git", "rev-parse", f"origin/{BRANCH}"], root, capture=True).strip()
    run(["git", "merge-base", "--is-ancestor", REMOTE_PARENT, remote], root)
    remote_runner = run(
        ["git", "show", f"{remote}:tools/resume_motor_driver_repair_v2.py"],
        root,
        capture=True,
    )
    if "patch_enemy_attack_finish" not in remote_runner:
        raise RuntimeError("Remote branch does not contain the expected v2 recovery runner")

    patch_motor_tests(root)
    patch_attack_slot_tests(root)
    patch_enemy_defense_tests(root)
    patch_enemy_attack_finish(root)
    patch_defense_mission_tests(root)

    for trigger in ("comp_usmc", "comp_1ad", "comp_arf"):
        patch_trigger_count(
            root / "resource/map/multi/defense_support_waves.inc",
            f"defense_support/{trigger}",
            4,
            5,
        )

    run([sys.executable, "-m", "py_compile",
         "tests/test_motor_runtime_isolation.py",
         "tests/test_attack_support_slot_proof.py",
         "tests/test_enemy_defense_support.py",
         "tests/test_defense_mission_support.py"], root)

    run([sys.executable, "-m", "pytest", *FOCUSED, "-q"], root)
    run([sys.executable, "-m", "pytest", "tests", "-q"], root)
    run(["git", "diff", "--check"], root)

    status = run(["git", "status", "--short"], root, capture=True)
    print(status)
    allowed = set(STAGE_FILES + HELPERS)
    seen = set()
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
    repair_commit = run(["git", "rev-parse", "HEAD"], root, capture=True).strip()

    for helper in ("tools/resume_motor_driver_repair.py", "tools/resume_motor_driver_repair_v2.py"):
        try:
            (root / helper).unlink()
        except FileNotFoundError:
            pass

    run(["git", "rebase", f"origin/{BRANCH}"], root)

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
            "powershell", "-NoProfile", "-ExecutionPolicy", "Bypass",
            "-File", str(deploy),
            "-RepoRoot", str(root),
            "-WorkshopRoot", WORKSHOP,
            "-E2TestMode", "0",
        ], root)
        print(f"Deployment run {number} passed.")

    print("\nVERIFIED: motor repair committed, pushed, and deployed twice.")
    print(f"Runtime repair before helper cleanup: {repair_commit}")
    print(f"Commit for live test: {final_commit}")


if __name__ == "__main__":
    main()
