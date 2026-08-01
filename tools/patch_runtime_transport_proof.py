from __future__ import annotations

import re
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]


@dataclass(frozen=True)
class Engine:
    path: str
    place_one: str
    place_tag: str
    placed_tag: str
    entry_next: str
    place_template_tags: tuple[str, ...]
    finisher: str
    owner: str
    deploy: str
    transfer: str
    hull: str
    pax: str
    crew: str
    source: str
    leaving: str
    stage_var: str
    drive_var: str
    band_var: str
    motor_template_tags: tuple[str, ...]


ENGINES = (
    Engine(
        "resource/map/multi/attack_support_waves.inc",
        "am_place_one", "attack_support_place_one", "attack_support_placed", "am_entry_next",
        ("attack_support_tpl", "ally_sup_tpl"),
        "as_finish_motor", "as_own_motor_to_support", "attack_support_deploy",
        "attack_support_motor_transfer", "attack_support_motor_hull",
        "attack_support_motor_pax", "attack_support_motor_crew",
        "attack_support_src", "am_motor_leaving",
        "attack_support_motor_stage$", "attack_support_motor_drive_t$",
        "attack_support_motor_band$", ("attack_support_tpl", "ally_sup_tpl"),
    ),
    Engine(
        "resource/map/multi/defense_support_waves.inc",
        "ds_place_one", "def_sup_place_one", "def_sup_placed", "ds_entry_next",
        ("attack_support_tpl", "ally_sup_tpl"),
        "ds_finish_motor", "ds_own_motor_to_defenderbot", "def_sup_deploy",
        "def_sup_motor_transfer", "def_sup_motor_hull", "def_sup_motor_pax",
        "def_sup_motor_crew", "def_sup_src", "def_sup_motor_leaving",
        "defense_support_motor_stage$", "defense_support_motor_drive_t$",
        "defense_support_motor_band$", ("attack_support_tpl", "ally_sup_tpl"),
    ),
    Engine(
        "resource/map/multi/enemy_attack_support.inc",
        "ea_place_one", "ea_place_one", "ea_placed", "ea_entry_next",
        ("enemy_def_tpl",),
        "ea_finish_motor", "ea_own_motor_to_enemy", "ea_deploy",
        "ea_motor_transfer", "ea_motor_hull", "ea_motor_pax", "ea_motor_crew",
        "ea_src", "ea_motor_leaving", "enemy_attack_motor_stage$",
        "enemy_attack_motor_drive_t$", "enemy_attack_motor_band$", ("ally_sup_tpl",),
    ),
    Engine(
        "resource/map/multi/enemy_defense_support.inc",
        "ed_place_one", "enemy_def_place_one", "enemy_def_placed", "ed_entry_next",
        ("enemy_def_tpl",),
        "ed_finish_motor", "ed_own_motor_to_enemy", "enemy_def_deploy",
        "enemy_def_motor_transfer", "enemy_def_motor_hull", "enemy_def_motor_pax",
        "enemy_def_motor_crew", "enemy_def_src", "enemy_def_motor_leaving",
        "enemy_defense_motor_stage$", "enemy_defense_motor_drive_t$",
        "enemy_defense_motor_band$", ("ally_sup_tpl",),
    ),
)


def block_span(text: str, token: str) -> tuple[int, int]:
    start = text.index(token)
    opener = token[0]
    closer = ")" if opener == "(" else "}"
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
        elif char == opener:
            depth += 1
        elif char == closer:
            depth -= 1
            if depth == 0:
                return start, index + 1
    raise RuntimeError(f"Unbalanced block: {token}")


def replace_block(text: str, token: str, replacement: str) -> str:
    start, end = block_span(text, token)
    return text[:start] + replacement + text[end:]


def indent(text: str, prefix: str = "\t") -> str:
    return "\n".join(prefix + line if line else line for line in text.splitlines())


def patch_place_one(text: str, engine: Engine) -> str:
    token = f'(define "{engine.place_one}"'
    start, end = block_span(text, token)
    body = text[start:end]
    if "Per-body activation spacing" in body:
        raise RuntimeError(f"{engine.path}: placement already patched")

    pattern = re.compile(
        r'\{"entity_state"\s*'
        + re.escape(f'{{selector {{tag {engine.place_tag}}}}}')
        + r'\s*'
        + re.escape(f'{{tag_add {engine.placed_tag}}}')
        + r'\s*'
        + re.escape(f'{{tag_remove {engine.place_tag}}}')
        + r'\s*\}\s*\{"delay"\s*\{time 0\.2\}\}',
        re.MULTILINE,
    )
    removals = "\n".join(
        f"\t\t\t\t\t\t\t{{tag_remove {tag}}}" for tag in engine.place_template_tags
    )
    replacement = f'''\t\t\t\t\t\t; Per-body activation spacing. The old batch finisher activated every infantryman
\t\t\t\t\t\t; at the same coordinate on the same frame, so the squad collided and died.
\t\t\t\t\t\t; sup_linked bodies never receive {engine.place_tag}, so linked vehicle packages
\t\t\t\t\t\t; remain inactive and atomic until their dedicated finisher runs.
\t\t\t\t\t\t{{"entity_state"
\t\t\t\t\t\t\t{{selector {{ignore_captured_by_user 0}} {{tag {engine.place_tag}}} {{type human}}}}
{removals}
\t\t\t\t\t\t\t{{tag_remove hidden}}
\t\t\t\t\t\t\t{{inactive off}}
\t\t\t\t\t\t\t{{impregnability disabled}}
\t\t\t\t\t\t\t{{discovered on}}
\t\t\t\t\t\t}}
\t\t\t\t\t\t{{"delay" {{time 0.75}}}}
\t\t\t\t\t\t("{engine.entry_next}")
\t\t\t\t\t\t{{"entity_state"
\t\t\t\t\t\t\t{{selector {{tag {engine.place_tag}}}}}
\t\t\t\t\t\t\t{{tag_add {engine.placed_tag}}}
\t\t\t\t\t\t\t{{tag_remove {engine.place_tag}}}
\t\t\t\t\t\t}}
\t\t\t\t\t\t{{"delay" {{time 0.05}}}}'''
    body, count = pattern.subn(replacement, body, count=1)
    if count != 1:
        raise RuntimeError(f"{engine.path}: place-one completion pattern count {count}")
    return text[:start] + body + text[end:]


def patch_motor_finisher(text: str, engine: Engine) -> str:
    token = f'(define "{engine.finisher}"'
    start, end = block_span(text, token)
    body = text[start:end]
    if "Atomic linked-package activation" in body:
        raise RuntimeError(f"{engine.path}: motor finisher already patched")

    promote_start = body.index("; Promote the three roles independently.")
    owner_call = f'("{engine.owner}")'
    promote_end = body.index(owner_call, promote_start)
    template_removals = "\n".join(
        f"\t\t\t\t\t{{tag_remove {tag}}}" for tag in engine.motor_template_tags
    )
    atomic = f'''; Atomic linked-package activation. Hull, driver, commander and passengers
\t\t\t\t; must leave inactive/hidden state in one entity-state operation. Activating
\t\t\t\t; the three roles separately breaks the Link seat bindings and produces a
\t\t\t\t; white empty truck with bodies clipping through its center.
\t\t\t\t{{"entity_state"
\t\t\t\t\t{{selector {{tag {engine.transfer}}}}}
{template_removals}
\t\t\t\t\t{{tag_remove hidden}}
\t\t\t\t\t{{inactive off}}
\t\t\t\t\t{{impregnability disabled}}
\t\t\t\t\t{{discovered on}}
\t\t\t\t}}
\t\t\t\t'''
    body = body[:promote_start] + atomic + body[promote_end:]

    simple_hull = f'{{selector {{ignore_captured_by_user 0}} {{tag {engine.hull}}}}}'
    typed_hull = f'{{selector {{ignore_captured_by_user 0}} {{tag {engine.hull}}} {{type vehicle}}}}'
    if simple_hull not in body:
        raise RuntimeError(f"{engine.path}: no simple hull selector found")
    body = body.replace(simple_hull, typed_hull)

    units_hull = f'{{units {{ignore_captured_by_user 0}} {{tag {engine.hull}}}}}'
    typed_units_hull = f'{{units {{ignore_captured_by_user 0}} {{tag {engine.hull}}} {{type vehicle}}}}'
    text = text.replace(units_hull, typed_units_hull)

    stage_two = f'{{"set_i" {{var "{engine.stage_var}"}} {{op "="}} {{value 2}}}}'
    split = body.index(stage_two)
    close = body.rfind(")")
    prefix = body[:split]
    suffix = body[split:close].rstrip()

    retirement = suffix.index(f'{{tag_add {engine.leaving}}}')
    return_prefix = suffix[:retirement]
    return_tail = suffix[retirement:]
    typed_leaving = f'{{selector {{ignore_captured_by_user 0}} {{tag {engine.leaving}}} {{type vehicle}}}}'
    return_tail = return_tail.replace(typed_hull, typed_leaving)
    suffix = return_prefix + return_tail

    default_cleanup = f'''{{"default"
\t\t\t\t\t; INVALID MOTOR PACKAGE - REHIDE. No truck order, passenger emit or
\t\t\t\t\t; return-to-edge order is legal unless the atomic package resolves as
\t\t\t\t\t; one inhabited vehicle. This prevents white trucks and walking riders.
\t\t\t\t\t{{"entity_state"
\t\t\t\t\t\t{{selector {{tag {engine.hull}}}}}
\t\t\t\t\t\t{{tag_add {engine.leaving}}}
\t\t\t\t\t\t{{tag_add hidden}}
\t\t\t\t\t\t{{inactive on}}
\t\t\t\t\t\t{{tag_remove {engine.deploy}}}
\t\t\t\t\t\t{{tag_remove {engine.hull}}}
\t\t\t\t\t}}
\t\t\t\t\t{{"entity_state"
\t\t\t\t\t\t{{selector {{tag {engine.pax}}}}}
\t\t\t\t\t\t{{tag_add hidden}}
\t\t\t\t\t\t{{inactive on}}
\t\t\t\t\t\t{{tag_remove {engine.deploy}}}
\t\t\t\t\t\t{{tag_remove {engine.pax}}}
\t\t\t\t\t}}
\t\t\t\t\t{{"entity_state"
\t\t\t\t\t\t{{selector {{tag {engine.crew}}}}}
\t\t\t\t\t\t{{tag_add hidden}}
\t\t\t\t\t\t{{inactive on}}
\t\t\t\t\t\t{{tag_remove {engine.deploy}}}
\t\t\t\t\t\t{{tag_remove {engine.crew}}}
\t\t\t\t\t}}
\t\t\t\t\t{{"set_i" {{var "{engine.stage_var}"}} {{op "="}} {{value 9}}}}
\t\t\t\t\t{{"set_i" {{var "{engine.drive_var}"}} {{op "="}} {{value 0}}}}
\t\t\t\t\t{{"set_i" {{var "{engine.band_var}"}} {{op "="}} {{value 0}}}}
\t\t\t\t}}'''

    wrapped = f'''\t\t\t\t; Fail closed before any drive or emit. The 2026-07-31 runtime showed both
\t\t\t\t; pipelines reaching stage 4 with band 0 while the hull was uninhabited.
\t\t\t\t{{"switch"
\t\t\t\t\t{{"case"
\t\t\t\t\t\t{{condition
\t\t\t\t\t\t\t{{type entities}}
\t\t\t\t\t\t\t{{selector {{ignore_captured_by_user 0}} {{tag {engine.hull}}} {{type vehicle}} {{state inhabited}}}}
\t\t\t\t\t\t\t{{count {{op ">="}} {{value 1}}}}
\t\t\t\t\t\t}}
{indent(suffix, chr(9))}
\t\t\t\t\t}}
\t\t\t\t\t{default_cleanup}
\t\t\t\t}}
\t\t\t)'''
    new_body = prefix + wrapped
    text = text[:start] + new_body + text[end:]
    return text


def patch_air_test_defaults() -> None:
    old = '{"set_i" {var "attack_support_air_test$"} {op "="} {value 1}}'
    new = '{"set_i" {var "attack_support_air_test$"} {op "="} {value 0}}'
    for rel in (
        "resource/map/multi/attack_support_waves.inc",
        "resource/map/multi/defense_support_waves.inc",
    ):
        path = ROOT / rel
        text = path.read_text(encoding="utf-8-sig")
        count = text.count(old)
        if count != 1:
            raise RuntimeError(f"{rel}: expected one forced-air assignment, found {count}")
        text = text.replace(old, new)
        text = text.replace("DAY-2 TEST default ON", "Legacy forced airmobile test OFF")
        path.write_text(text, encoding="utf-8")


def patch_engines() -> None:
    for engine in ENGINES:
        path = ROOT / engine.path
        text = path.read_text(encoding="utf-8-sig")
        text = patch_place_one(text, engine)
        text = patch_motor_finisher(text, engine)
        path.write_text(text, encoding="utf-8")


def patch_deploy_guard() -> None:
    path = ROOT / "tools/deploy_attack_support_probe.ps1"
    text = path.read_text(encoding="utf-8-sig")
    anchor = "# The retired allied-support experiment owned these. Its files are gone; a stale\n"
    if text.count(anchor) != 1:
        raise RuntimeError("deploy guard insertion anchor not found exactly once")
    guard = r'''# Runtime-proof guards earned by game(38).log. E2TestMode 0 must not leave the
# older narrative airmobile test armed, and every ground engine must retain the
# atomic motor package plus fail-closed and per-body arrival paths.
$forcedAirOn = '{"set_i" {var "attack_support_air_test$"} {op "="} {value 1}}'
$forcedAirOff = '{"set_i" {var "attack_support_air_test$"} {op "="} {value 0}}'
foreach ($pair in @(@($wavesSource, 'attack support'), @($dsSource, 'defence support'))) {
    $code = Get-MiCode $pair[0]
    if ($code.Contains($forcedAirOn)) {
        throw "Source $($pair[1]) engine still forces the legacy mid-map airmobile test"
    }
    if (-not $code.Contains($forcedAirOff)) {
        throw "Source $($pair[1]) engine does not explicitly park the legacy airmobile test"
    }
}
foreach ($pair in @(
    @($wavesSource, 'attack support'),
    @($defSource, 'enemy defence'),
    @($dsSource, 'defence support'),
    @($eaSource, 'enemy attack')
)) {
    $raw = [System.IO.File]::ReadAllText($pair[0])
    foreach ($marker in @('Per-body activation spacing', 'Atomic linked-package activation', 'INVALID MOTOR PACKAGE - REHIDE', 'Fail closed before any drive or emit')) {
        if (-not $raw.Contains($marker)) {
            throw "Source $($pair[1]) engine is missing runtime-proof marker: $marker"
        }
    }
}

'''
    text = text.replace(anchor, guard + anchor)
    path.write_text(text, encoding="utf-8")


def patch_tests() -> None:
    path = ROOT / "tests/test_motor_runtime_isolation.py"
    text = path.read_text(encoding="utf-8-sig")
    marker = "def test_runtime_proof_requires_atomic_inhabited_motor_packages"
    if marker in text:
        raise RuntimeError("runtime-proof tests already present")
    addition = r'''


def test_legacy_forced_airmobile_test_is_parked() -> None:
    off = '{"set_i" {var "attack_support_air_test$"} {op "="} {value 0}}'
    on = '{"set_i" {var "attack_support_air_test$"} {op "="} {value 1}}'
    for path in (
        "resource/map/multi/attack_support_waves.inc",
        "resource/map/multi/defense_support_waves.inc",
    ):
        text = (ROOT / path).read_text(encoding="utf-8-sig")
        assert text.count(off) == 1
        assert on not in text


def test_arrivals_activate_and_rotate_one_unlinked_human_at_a_time() -> None:
    configs = [
        ("resource/map/multi/attack_support_waves.inc", "am_place_one", "attack_support_place_one", "attack_support_placed", "am_entry_next"),
        ("resource/map/multi/defense_support_waves.inc", "ds_place_one", "def_sup_place_one", "def_sup_placed", "ds_entry_next"),
        ("resource/map/multi/enemy_attack_support.inc", "ea_place_one", "ea_place_one", "ea_placed", "ea_entry_next"),
        ("resource/map/multi/enemy_defense_support.inc", "ed_place_one", "enemy_def_place_one", "enemy_def_placed", "ed_entry_next"),
    ]
    for path, define, one, placed, entry_next in configs:
        text = (ROOT / path).read_text(encoding="utf-8-sig")
        body = block(text, f'(define "{define}"')
        activation = body.index(f'{{tag {one}}} {{type human}}')
        clear = body.index(f'{{tag_remove {one}}}', activation)
        assert activation < clear
        assert '{inactive off}' in body[activation:clear]
        assert '{"delay" {time 0.75}}' in body[activation:clear]
        assert f'("{entry_next}")' in body[activation:clear]
        assert f'{{tag_add {placed}}}' in body[activation:clear + 200]
        selector_start = body.rindex('{source advanced}', 0, activation)
        assert 'sup_linked' in body[selector_start:activation]


def test_runtime_proof_requires_atomic_inhabited_motor_packages() -> None:
    for path, finisher, owner, deploy, transfer, hull, pax, crew, _, leaving in CONFIGS:
        text = (ROOT / path).read_text(encoding="utf-8-sig")
        body = block(text, f'(define "{finisher}"')
        promote = body[body.index('Atomic linked-package activation'):body.index(f'("{owner}")')]
        assert promote.count(f'{{selector {{tag {transfer}}}}}') == 1
        assert f'{{selector {{tag {hull}}}}}' not in promote
        assert f'{{selector {{tag {pax}}}}}' not in promote
        assert f'{{selector {{tag {crew}}}}}' not in promote

        gate = body.index(f'{{tag {hull}}} {{type vehicle}} {{state inhabited}}')
        stage_two = body.index('{value 2}', gate)
        stage_nine = body.index('{value 9}', stage_two)
        assert gate < stage_two < stage_nine
        assert 'INVALID MOTOR PACKAGE - REHIDE' in body[stage_two:]

        untyped = f'{{selector {{ignore_captured_by_user 0}} {{tag {hull}}}}}'
        assert untyped not in body
        retirement = body.index(f'{{tag_add {leaving}}}', stage_two)
        return_path = body[retirement:]
        assert f'{{tag {leaving}}} {{type vehicle}}' in return_path
        assert f'{{tag {hull}}} {{type vehicle}}' not in return_path


def test_deploy_guard_pins_runtime_transport_repairs() -> None:
    deploy = (ROOT / "tools/deploy_attack_support_probe.ps1").read_text(encoding="utf-8-sig")
    assert "still forces the legacy mid-map airmobile test" in deploy
    assert "Per-body activation spacing" in deploy
    assert "Atomic linked-package activation" in deploy
    assert "INVALID MOTOR PACKAGE - REHIDE" in deploy
'''
    path.write_text(text.rstrip() + addition + "\n", encoding="utf-8")


def main() -> None:
    patch_air_test_defaults()
    patch_engines()
    patch_deploy_guard()
    patch_tests()
    print("Patched forced mid-map insert, per-body arrival spacing, and atomic motor lifecycle")


if __name__ == "__main__":
    main()
