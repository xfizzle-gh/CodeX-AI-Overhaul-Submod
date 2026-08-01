from __future__ import annotations

from pathlib import Path

DEPLOY = Path("tools/deploy_attack_support_probe.ps1")
TESTS = Path("tests/test_motor_runtime_isolation.py")

OLD_GUARD = '''if (Select-String -Quiet -LiteralPath $wavesSource -SimpleMatch '{tag_remove attack_support_src}') {
    throw "Source wave engine removes attack_support_src, but the entire downstream chain selects on it"
}
'''

NEW_GUARD = '''# attack_support_src remains permanent on delivered infantry because the live-unit
# cap and downstream order flow select on it. The sole exception is the EMPTY motor
# hull after its passengers have emitted: it must leave the infantry namespace before
# returning to the map edge, or the generic support/patrol systems reclaim the truck.
$attackSourceRemovalToken = '{tag_remove attack_support_src}'
$wavesText = Get-Content -LiteralPath $wavesSource -Raw
$attackSourceRemovalCount = [regex]::Matches(
    $wavesText,
    [regex]::Escape($attackSourceRemovalToken)
).Count
if ($attackSourceRemovalCount -ne 1) {
    throw "Source wave engine must remove attack_support_src exactly once, from the empty departing motor hull; found $attackSourceRemovalCount removals"
}
$wavesCompact = [regex]::Replace($wavesText, '\\s+', ' ').Trim()
$allowedMotorRetirement = '{"entity_state" {selector {tag attack_support_motor_hull}} {tag_add am_motor_leaving} {tag_remove attack_support_src} {tag_remove attack_support_g1} {tag_remove attack_support_g2} {tag_remove attack_support_g3} {tag_remove attack_support_g4} }'
if (-not $wavesCompact.Contains($allowedMotorRetirement)) {
    throw "Source wave engine removes attack_support_src outside the exact empty-hull retirement block"
}
'''

TEST = '''\n\ndef test_deploy_guard_allows_only_the_empty_departing_hull_to_drop_attack_source() -> None:
    deploy = (ROOT / "tools/deploy_attack_support_probe.ps1").read_text(encoding="utf-8-sig")
    waves = (ROOT / "resource/map/multi/attack_support_waves.inc").read_text(encoding="utf-8-sig")

    token = "{tag_remove attack_support_src}"
    assert waves.count(token) == 1
    motor = block(waves, '(define "as_finish_motor"')
    leaving = motor.index("{tag_add am_motor_leaving}")
    removal = motor.index(token)
    assert leaving < removal
    retirement = motor[motor.rindex('{"entity_state"', 0, leaving):removal + len(token) + 200]
    assert "{selector {tag attack_support_motor_hull}}" in retirement
    assert "{tag_remove attack_support_g1}" in retirement
    assert "{tag_remove attack_support_g4}" in retirement

    assert "$attackSourceRemovalCount -ne 1" in deploy
    assert "$allowedMotorRetirement" in deploy
    assert "outside the exact empty-hull retirement block" in deploy
    assert "but the entire downstream chain selects on it" not in deploy
'''


def main() -> None:
    deploy = DEPLOY.read_text(encoding="utf-8-sig")
    if OLD_GUARD not in deploy:
        raise RuntimeError("obsolete global attack_support_src guard not found exactly once")
    if deploy.count(OLD_GUARD) != 1:
        raise RuntimeError("obsolete guard appears more than once")
    deploy = deploy.replace(OLD_GUARD, NEW_GUARD)
    DEPLOY.write_text(deploy, encoding="utf-8")

    tests = TESTS.read_text(encoding="utf-8-sig")
    marker = "def test_deploy_guard_allows_only_the_empty_departing_hull_to_drop_attack_source"
    if marker not in tests:
        tests = tests.rstrip() + TEST.rstrip() + "\n"
        TESTS.write_text(tests, encoding="utf-8")

    print("Scoped deploy guard to the empty departing attack-support motor hull")


if __name__ == "__main__":
    main()
