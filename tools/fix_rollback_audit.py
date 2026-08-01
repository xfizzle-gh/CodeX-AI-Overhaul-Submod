from pathlib import Path

path = Path(__file__).resolve().with_name("rollback_motor_to_live_baseline.py")
text = path.read_text(encoding="utf-8")
old = '''    for stale in (
        "MOTOR RELEASE REQUIRES PROVEN MOVEMENT",
        "test_runtime_proof_requires_role_reassertion_and_movement_release",
'''
new = '''    for stale in (
        "test_runtime_proof_requires_role_reassertion_and_movement_release",
'''
if text.count(old) != 1:
    raise RuntimeError(f"Expected one stale-audit block, found {text.count(old)}")
path.write_text(text.replace(old, new), encoding="utf-8", newline="\n")
print("Removed the rollback audit false positive.")
