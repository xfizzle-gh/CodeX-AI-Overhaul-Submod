from __future__ import annotations

from pathlib import Path

DEPLOY = Path("tools/deploy_attack_support_probe.ps1")
TESTS = Path("tests/test_motor_runtime_isolation.py")

OLD_GUARD = '''if (Select-String -Quiet -LiteralPath $defSource -SimpleMatch '{tag_remove enemy_def_src}') {
    throw "Source enemy defence engine removes enemy_def_src, but the live-unit cap counts it"
}
'''

NEW_GUARD = '''# Every engine keeps its source tag on delivered infantry because the live-unit cap
# and downstream order flow select on it. The only legal removal is from the EMPTY
# motor hull after passenger emission, immediately after the engine-specific leaving
# tag is applied and inside the exact hull retirement block.
function Assert-ScopedMotorSourceRemoval {
    param(
        [string]$Path,
        [string]$Label,
        [string]$SourceTag,
        [string]$HullTag,
        [string]$LeavingTag,
        [string[]]$GroupTags
    )

    $token = '{tag_remove ' + $SourceTag + '}'
    $text = Get-Content -LiteralPath $Path -Raw
    $count = [regex]::Matches($text, [regex]::Escape($token)).Count
    if ($count -ne 1) {
        throw "Source $Label engine must remove $SourceTag exactly once, from the empty departing motor hull; found $count removals"
    }

    $compact = [regex]::Replace($text, '\\s+', ' ').Trim()
    $groupRemoval = ($GroupTags | ForEach-Object { '{tag_remove ' + $_ + '}' }) -join ' '
    $allowed = '{"entity_state" {selector {tag ' + $HullTag + '}} {tag_add ' + $LeavingTag + '} ' + $token + ' ' + $groupRemoval + ' }'
    if (-not $compact.Contains($allowed)) {
        throw "Source $Label engine removes $SourceTag outside the exact empty-hull retirement block"
    }
}

Assert-ScopedMotorSourceRemoval `
    -Path $defSource `
    -Label 'enemy defence' `
    -SourceTag 'enemy_def_src' `
    -HullTag 'enemy_def_motor_hull' `
    -LeavingTag 'enemy_def_motor_leaving' `
    -GroupTags @('enemy_def_p1', 'enemy_def_p2', 'enemy_def_p3', 'enemy_def_p4')
Assert-ScopedMotorSourceRemoval `
    -Path $dsSource `
    -Label 'defence support' `
    -SourceTag 'def_sup_src' `
    -HullTag 'def_sup_motor_hull' `
    -LeavingTag 'def_sup_motor_leaving' `
    -GroupTags @('def_sup_h1', 'def_sup_h2', 'def_sup_h3')
Assert-ScopedMotorSourceRemoval `
    -Path $eaSource `
    -Label 'enemy attack' `
    -SourceTag 'ea_src' `
    -HullTag 'ea_motor_hull' `
    -LeavingTag 'ea_motor_leaving' `
    -GroupTags @('ea_g1', 'ea_g2', 'ea_g3', 'ea_g4')
'''

TEST = '''def test_deploy_guard_scopes_remaining_motor_source_removals() -> None:
    deploy = (ROOT / "tools/deploy_attack_support_probe.ps1").read_text(encoding="utf-8-sig")
    configs = [
        ("resource/map/multi/enemy_defense_support.inc", "ed_finish_motor", "enemy_def_src", "enemy_def_motor_hull", "enemy_def_motor_leaving", ("enemy_def_p1", "enemy_def_p2", "enemy_def_p3", "enemy_def_p4")),
        ("resource/map/multi/defense_support_waves.inc", "ds_finish_motor", "def_sup_src", "def_sup_motor_hull", "def_sup_motor_leaving", ("def_sup_h1", "def_sup_h2", "def_sup_h3")),
        ("resource/map/multi/enemy_attack_support.inc", "ea_finish_motor", "ea_src", "ea_motor_hull", "ea_motor_leaving", ("ea_g1", "ea_g2", "ea_g3", "ea_g4")),
    ]

    assert "function Assert-ScopedMotorSourceRemoval" in deploy
    for path, finisher, source, hull, leaving_tag, groups in configs:
        text = (ROOT / path).read_text(encoding="utf-8-sig")
        token = f"{{tag_remove {source}}}"
        assert text.count(token) == 1
        motor = block(text, f'(define "{finisher}"')
        leaving = motor.index(f"{{tag_add {leaving_tag}}}")
        removal = motor.index(token)
        assert leaving < removal
        retirement = motor[motor.rindex('{"entity_state"', 0, leaving):removal + len(token) + 300]
        assert f"{{selector {{tag {hull}}}}}" in retirement
        for group in groups:
            assert f"{{tag_remove {group}}}" in retirement
        assert f"-SourceTag '{source}'" in deploy
        assert f"-HullTag '{hull}'" in deploy
        assert f"-LeavingTag '{leaving_tag}'" in deploy

    assert "removes enemy_def_src, but the live-unit cap counts it" not in deploy
'''


def main() -> None:
    deploy = DEPLOY.read_text(encoding="utf-8-sig")
    if deploy.count(OLD_GUARD) != 1:
        raise RuntimeError("obsolete enemy-defence source guard not found exactly once")
    deploy = deploy.replace(OLD_GUARD, NEW_GUARD)
    DEPLOY.write_text(deploy, encoding="utf-8")

    tests = TESTS.read_text(encoding="utf-8-sig")
    marker = "def test_deploy_guard_scopes_remaining_motor_source_removals"
    if marker not in tests:
        tests = tests.rstrip() + "\n\n" + TEST.rstrip() + "\n"
        TESTS.write_text(tests, encoding="utf-8")

    print("Scoped all remaining motor source-tag guards")


if __name__ == "__main__":
    main()
