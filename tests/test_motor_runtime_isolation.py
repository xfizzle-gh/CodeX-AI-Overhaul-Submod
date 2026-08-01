from __future__ import annotations

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
