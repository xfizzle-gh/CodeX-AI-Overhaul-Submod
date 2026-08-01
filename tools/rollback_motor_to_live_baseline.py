from __future__ import annotations

import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
BASELINE = "13a5861488a29640c0da7b4098f1b5e893662bfc"
MARKER = "; LAST LIVE MOVING MOTOR BASELINE (2026-07-31). Restored after the stage-9/no-drive regression."

MOTORS = (
    ("resource/map/multi/attack_support_waves.inc", "as_finish_motor", "attack_support_motor_stage", "attack_support_motor_hull", "attack_support_motor_pax", "attack_support_motor_crew"),
    ("resource/map/multi/defense_support_waves.inc", "ds_finish_motor", "defense_support_motor_stage", "def_sup_motor_hull", "def_sup_motor_pax", "def_sup_motor_crew"),
    ("resource/map/multi/enemy_attack_support.inc", "ea_finish_motor", "enemy_attack_motor_stage", "ea_motor_hull", "ea_motor_pax", "ea_motor_crew"),
    ("resource/map/multi/enemy_defense_support.inc", "ed_finish_motor", "enemy_defense_motor_stage", "enemy_def_motor_hull", "enemy_def_motor_pax", "enemy_def_motor_crew"),
)


def run(*args: str) -> str:
    result = subprocess.run(
        args,
        cwd=ROOT,
        check=True,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
    )
    return result.stdout


def balanced_define(text: str, name: str) -> tuple[int, int, str]:
    token = f'(define "{name}"'
    start = text.index(token)
    depth = 0
    quoted = False
    escaped = False
    comment = False
    for index in range(start, len(text)):
        char = text[index]
        if comment:
            if char == "\n":
                comment = False
            continue
        if quoted:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == '"':
                quoted = False
            continue
        if char == ";":
            comment = True
        elif char == '"':
            quoted = True
        elif char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
            if depth == 0:
                return start, index + 1, text[start:index + 1]
    raise RuntimeError(f"Unbalanced define: {name}")


def replace_python_function(text: str, name: str, replacement: str) -> str:
    pattern = re.compile(rf"^def {re.escape(name)}\(.*?(?=^def |\Z)", re.M | re.S)
    matches = list(pattern.finditer(text))
    if len(matches) != 1:
        raise RuntimeError(f"Expected one Python function {name}, found {len(matches)}")
    match = matches[0]
    return text[:match.start()] + replacement.rstrip() + "\n\n" + text[match.end():]


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    return text.replace(old, new)


def restore_motor_defines() -> None:
    for relative, name, *_ in MOTORS:
        path = ROOT / relative
        current = path.read_text(encoding="utf-8-sig")
        baseline = run("git", "show", f"{BASELINE}:{relative}")
        _, _, baseline_body = balanced_define(baseline, name)
        start, end, _ = balanced_define(current, name)
        restored = MARKER + "\n" + baseline_body
        updated = current[:start] + restored + current[end:]
        path.write_text(updated, encoding="utf-8", newline="\n")


def update_deploy_guard() -> None:
    path = ROOT / "tools/deploy_attack_support_probe.ps1"
    text = path.read_text(encoding="utf-8-sig")
    old = "foreach ($marker in @('Per-body activation spacing', 'Atomic linked-package activation', 'INVALID MOTOR PACKAGE - REHIDE', 'Fail closed before any drive or emit')) {"
    new = "foreach ($marker in @('Per-body activation spacing', 'LAST LIVE MOVING MOTOR BASELINE', 'Promote the three roles independently', 'Vehicles use MOVE')) {"
    text = replace_once(text, old, new, "deploy motor marker list")
    path.write_text(text, encoding="utf-8", newline="\n")


def update_runtime_tests() -> None:
    path = ROOT / "tests/test_motor_runtime_isolation.py"
    text = path.read_text(encoding="utf-8-sig")

    replacement = '''def test_motor_finishers_match_last_live_moving_baseline() -> None:
    for path, finisher, _, _, _, hull, pax, crew, _, _ in CONFIGS:
        text = (ROOT / path).read_text(encoding="utf-8-sig")
        body = block(text, f'(define "{finisher}"')

        assert "LAST LIVE MOVING MOTOR BASELINE" in text
        assert "MOTOR RELEASE REQUIRES PROVEN MOVEMENT" not in body
        assert "_transfer_release" not in body
        for role in (hull, pax, crew):
            assert f"{role}_tx" not in body

        stage_var = {
            "as_finish_motor": "attack_support_motor_stage",
            "ds_finish_motor": "defense_support_motor_stage",
            "ea_finish_motor": "enemy_attack_motor_stage",
            "ed_finish_motor": "enemy_defense_motor_stage",
        }[finisher]
        stage2 = body.index(
            f'{{"set_i" {{var "{stage_var}$"}} {{op "="}} {{value 2}}}}'
        )
        drive = body.index("; Vehicles use MOVE")
        stage3 = body.index(
            f'{{"set_i" {{var "{stage_var}$"}} {{op "="}} {{value 3}}}}'
        )
        emit = body.index('{"emit"', stage3)
        stage4 = body.index(
            f'{{"set_i" {{var "{stage_var}$"}} {{op "="}} {{value 4}}}}', emit
        )
        assert stage2 < drive < stage3 < emit < stage4

        prefix = body[:drive]
        assert f'{{selector {{ignore_captured_by_user 0}} {{tag {crew}}}}}' in prefix
        emit_block = block(body, '{"emit"')
        assert f"{{tag {hull}}}" in emit_block
        assert "{type vehicle}" in emit_block
        assert "{state inhabited}" in emit_block
        assert "{emit {mode passengers}}" in emit_block
        assert '{value 9}' not in body
'''
    text = replace_python_function(
        text,
        "test_runtime_proof_requires_role_reassertion_and_movement_release",
        replacement,
    )

    deploy_replacement = '''def test_deploy_guard_pins_live_moving_motor_baseline() -> None:
    deploy = (ROOT / "tools/deploy_attack_support_probe.ps1").read_text(encoding="utf-8-sig")
    assert "still forces the legacy mid-map airmobile test" in deploy
    assert "Per-body activation spacing" in deploy
    assert "LAST LIVE MOVING MOTOR BASELINE" in deploy
    assert "Promote the three roles independently" in deploy
    assert "Vehicles use MOVE" in deploy
    assert "Atomic linked-package activation" not in deploy
    assert "INVALID MOTOR PACKAGE - REHIDE" not in deploy
    assert "Fail closed before any drive or emit" not in deploy
'''
    text = replace_python_function(
        text,
        "test_deploy_guard_pins_runtime_transport_repairs",
        deploy_replacement,
    )
    path.write_text(text.rstrip() + "\n", encoding="utf-8", newline="\n")


def update_slot_tests() -> None:
    path = ROOT / "tests/test_attack_support_slot_proof.py"
    text = path.read_text(encoding="utf-8-sig")

    old_reset = '''                # One reset is initialization; the second releases the completed
                # or failed lifecycle so another truck package may be dispatched.
                self.assertEqual(
                    code.count('{"set_i" {var "%s$"} {op "="} {value 0}}' % var), 2
                )
'''
    new_reset = '''                # The last live-moving baseline initializes the stage once. It does
                # not synthesize a stage-9 failure latch or a later reset.
                self.assertEqual(
                    code.count('{"set_i" {var "%s$"} {op "="} {value 0}}' % var), 1
                )
'''
    text = replace_once(text, old_reset, new_reset, "motor stage reset assertion")

    old_order = '''                # Stage 4 now means movement was proved and unload was released.
                # It is written immediately before emit; passengers become normal
                # infantry only after the emit command completes.
                self.assertLess(stage3, stage4)
                self.assertLess(stage4, emit)
                self.assertLess(emit, pax_source)
'''
    new_order = '''                # Last live-moving ordering: drive, timed transit, emit, then stage 4.
                self.assertLess(stage3, emit)
                self.assertLess(emit, stage4)
                self.assertLess(stage4, pax_source)
'''
    text = replace_once(text, old_order, new_order, "motor emit ordering assertion")
    path.write_text(text, encoding="utf-8", newline="\n")


def audit() -> None:
    for relative, name, stage, hull, pax, crew in MOTORS:
        text = (ROOT / relative).read_text(encoding="utf-8-sig")
        _, _, body = balanced_define(text, name)
        if MARKER not in text:
            raise RuntimeError(f"Missing rollback marker in {relative}")
        for forbidden in ("_tx", "_transfer_release", "MOTOR RELEASE REQUIRES PROVEN MOVEMENT"):
            if forbidden in body:
                raise RuntimeError(f"Failed to remove {forbidden} from {relative}")
        for required in (
            "Promote the three roles independently",
            "Vehicles use MOVE",
            f'{{tag {hull}}}',
            f'{{tag {pax}}}',
            f'{{tag {crew}}}',
            f'{{var "{stage}$"}}',
            "{state inhabited}",
            "{emit {mode passengers}}",
        ):
            if required not in body:
                raise RuntimeError(f"Missing baseline marker {required!r} in {relative}")
        if body.count('{"delay" {time 7}}') != 4:
            raise RuntimeError(f"Unexpected drive timing in {relative}")

    all_tests = "\n".join(
        path.read_text(encoding="utf-8-sig") for path in (ROOT / "tests").glob("test_*.py")
    )
    for stale in (
        "MOTOR RELEASE REQUIRES PROVEN MOVEMENT",
        "test_runtime_proof_requires_role_reassertion_and_movement_release",
        "Stage 4 now means movement was proved",
        "second releases the completed",
    ):
        if stale in all_tests:
            raise RuntimeError(f"Stale failed-design assertion remains: {stale}")


def main() -> None:
    restore_motor_defines()
    update_deploy_guard()
    update_runtime_tests()
    update_slot_tests()
    audit()
    print("Restored all four motor finishers to the last live-moving baseline.")


if __name__ == "__main__":
    main()
