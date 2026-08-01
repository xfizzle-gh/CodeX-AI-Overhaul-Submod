from __future__ import annotations

import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CONFIGS = [
    ("resource/map/multi/attack_support_waves.inc", "as_finish_motor", "as_own_motor_to_support", "attack_support_deploy", "attack_support_motor_transfer", "attack_support_motor_hull", "attack_support_motor_pax", "attack_support_motor_crew", "attack_support_src", "am_motor_leaving"),
    ("resource/map/multi/defense_support_waves.inc", "ds_finish_motor", "ds_own_motor_to_defenderbot", "def_sup_deploy", "def_sup_motor_transfer", "def_sup_motor_hull", "def_sup_motor_pax", "def_sup_motor_crew", "def_sup_src", "def_sup_motor_leaving"),
    ("resource/map/multi/enemy_attack_support.inc", "ea_finish_motor", "ea_own_motor_to_enemy", "ea_deploy", "ea_motor_transfer", "ea_motor_hull", "ea_motor_pax", "ea_motor_crew", "ea_src", "ea_motor_leaving"),
    ("resource/map/multi/enemy_defense_support.inc", "ed_finish_motor", "ed_own_motor_to_enemy", "enemy_def_deploy", "enemy_def_motor_transfer", "enemy_def_motor_hull", "enemy_def_motor_pax", "enemy_def_motor_crew", "enemy_def_src", "enemy_def_motor_leaving"),
]


def block(text: str, token: str) -> str:
    start = text.index(token)
    opener = token[0]
    closer = ")" if opener == "(" else "}"
    depth = 0
    quoted = False
    escaped = False
    for index in range(start, len(text)):
        char = text[index]
        if quoted:
            if escaped:
                escaped = False
            elif char == "\\":
                escaped = True
            elif char == '"':
                quoted = False
            continue
        if char == '"':
            quoted = True
        elif char == opener:
            depth += 1
        elif char == closer:
            depth -= 1
            if depth == 0:
                return text[start:index + 1]
    raise AssertionError(token)


def test_normal_attack_support_no_longer_uses_midmap_flank_pads() -> None:
    text = (ROOT / "resource/map/multi/attack_support_waves.inc").read_text(encoding="utf-8-sig")
    choose = block(text, '(define "as_choose_entry"')
    assert '{type rand}' not in choose
    assert 'as_announce_flank' not in choose
    assert '{value 1}' not in choose


def test_motor_packages_leave_shared_deploy_namespace_before_waiting() -> None:
    for path, finisher, _, deploy, transfer, hull, pax, crew, _, _ in CONFIGS:
        text = (ROOT / path).read_text(encoding="utf-8-sig")
        body = block(text, f'(define "{finisher}"')
        first_delay = body.index('{"delay"')
        prefix = body[:first_delay]
        for tag in (hull, pax, crew):
            assert f'{{selector {{tag {tag}}}}}' in prefix
        assert prefix.count(f'{{tag_remove {deploy}}}') == 3
        assert prefix.count(f'{{tag_add {transfer}}}') == 3


def test_seated_passengers_are_not_generic_infantry_before_emit() -> None:
    for path, finisher, _, deploy, _, _, pax, _, src, _ in CONFIGS:
        text = (ROOT / path).read_text(encoding="utf-8-sig")
        body = block(text, f'(define "{finisher}"')
        emit = body.index('{"emit"')
        before, after = body[:emit], body[emit:]
        assert f'{{tag_add {src}}}' not in before
        assert f'{{tag_add {src}}}' in after
        assert ('{selector {ignore_captured_by_user 0} {tag ' + deploy + '}}') not in body
        assert after.index(f'{{tag_add {src}}}') < after.index(f'{{tag_remove {pax}}}')


def test_motor_ownership_uses_dedicated_transfer_tag() -> None:
    for path, _, owner, deploy, transfer, *_ in CONFIGS:
        text = (ROOT / path).read_text(encoding="utf-8-sig")
        body = block(text, f'(define "{owner}"')
        assert f'{{tag {transfer}}}' in body
        assert f'{{tag {deploy}}}' not in body


def test_hulls_use_vehicle_move_and_explicit_edge_exit() -> None:
    for path, finisher, _, _, _, hull, _, _, src, leaving in CONFIGS:
        text = (ROOT / path).read_text(encoding="utf-8-sig")
        body = block(text, f'(define "{finisher}"')
        assert f'{{tag_add {leaving}}}' in body
        assert f'{{tag_remove {src}}}' in body
        assert '{action advance}' in body  # passenger order remains tactical
        assert body.count('{action move}') >= 4  # objective plus three exit switch arms
        assert '{waypoint "0"}' not in body
        assert 'attack_support_entry_a1' in body
        assert 'attack_support_entry_b1' in body
        first_hull_action = body.index(f'{{tag {hull}}}', body.index('; Vehicles use MOVE'))
        action_slice = body[first_hull_action:first_hull_action + 400]
        assert '{action move}' in action_slice
        assert '{action advance}' not in action_slice


def test_enemy_defender_patrol_tag_moves_to_pax_not_hull() -> None:
    text = (ROOT / "resource/map/multi/enemy_defense_support.inc").read_text(encoding="utf-8-sig")
    body = block(text, '(define "ed_finish_motor"')
    emit = body.index('{"emit"')
    assert '{tag_add enemy_def_p4}' in body[emit:]
    hull_cleanup = body.index('{tag_add enemy_def_motor_leaving}')
    for group in ("enemy_def_p1", "enemy_def_p2", "enemy_def_p3", "enemy_def_p4"):
        assert f'{{tag_remove {group}}}' in body[hull_cleanup:]

def test_deploy_guard_allows_only_the_empty_departing_hull_to_drop_attack_source() -> None:
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

def test_deploy_guard_scopes_remaining_motor_source_removals() -> None:
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


def test_runtime_proof_requires_role_reassertion_and_movement_release() -> None:
    for path, finisher, owner, deploy, transfer, hull, pax, crew, _, leaving in CONFIGS:
        text = (ROOT / path).read_text(encoding="utf-8-sig")
        body = block(text, f'(define "{finisher}"')
        drive = body.index('; Vehicles use MOVE')
        prefix = body[:drive]

        # Ownership keeps one witness per role; linked crew are not separately activated.
        for role in (hull, pax, crew):
            assert f'{role}_tx' in prefix
            assert f'{{selector {{tag {role}_tx}}}}' in prefix
        assert '{state inhabited}' not in prefix
        # A linked driver must not receive a separate actor-state activation before
        # the hull moves. The previous split-based check bled into the following
        # commands and falsely classified the hull actor-state block as a crew block.
        assert (
            f'{{selector {{ignore_captured_by_user 0}} {{tag {crew}}}}}'
            not in prefix
        )

        # No passenger release is legal until objective-distance movement is proven.
        marker = 'MOTOR RELEASE REQUIRES PROVEN MOVEMENT'
        assert body.count(marker) == 1
        proof = body[body.index(marker):body.index('{"emit"')]
        assert '{op ">"} {value 0}' in proof
        assert f'{{tag_add {transfer}_release}}' in proof
        assert 'Band 0' in proof
        assert 'INVALID MOTOR PACKAGE - REHIDE' in body

        # Both the movement-failure and invalid-package paths expose stage 9, then the
        # finisher resets to zero so a later numbered package can run.
        assert body.count('{value 9}') >= 2
        assert body.rfind('{value 0}') > body.rfind('{value 9}')

        retirement = body.index(f'{{tag_add {leaving}}}')
        return_path = body[retirement:]
        assert f'{{tag {leaving}}} {{type vehicle}}' in return_path
        assert f'{{tag {hull}}} {{type vehicle}}' not in return_path

def test_deploy_guard_pins_runtime_transport_repairs() -> None:
    deploy = (ROOT / "tools/deploy_attack_support_probe.ps1").read_text(encoding="utf-8-sig")
    assert "still forces the legacy mid-map airmobile test" in deploy
    assert "Per-body activation spacing" in deploy
    assert "Atomic linked-package activation" in deploy
    assert "INVALID MOTOR PACKAGE - REHIDE" in deploy


def test_standard_infantry_arrivals_are_five_or_six() -> None:
    configs = (
        ("resource/map/multi/attack_support_waves.inc", "attack_support"),
        ("resource/map/multi/defense_support_waves.inc", "defense_support"),
        ("resource/map/multi/enemy_attack_support.inc", "enemy_attack"),
        ("resource/map/multi/enemy_defense_support.inc", "enemy_defense"),
    )
    for path, prefix in configs:
        text = (ROOT / path).read_text(encoding="utf-8-sig")
        pattern = re.compile(
            rf'\{{"(?P<name>{re.escape(prefix)}/(?:ally_)?(?:rusa|ukr|nato|prc)_'
            r'(?P<role>line|wpn|recon|assault|eng|light))"'
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
