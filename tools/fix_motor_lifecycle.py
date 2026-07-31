from __future__ import annotations

from pathlib import Path

CONFIGS = [
    {
        "path": "resource/map/multi/attack_support_waves.inc",
        "finisher": "as_finish_motor",
        "owner_old": "am_own_to_support",
        "owner_new": "as_own_motor_to_support",
        "deploy": "attack_support_deploy",
        "transfer": "attack_support_motor_transfer",
        "hull": "attack_support_motor_hull",
        "pax": "attack_support_motor_pax",
        "crew": "attack_support_motor_crew",
        "src": "attack_support_src",
        "flag": "attack_support_motor_flag",
        "stage": "attack_support_motor_stage$",
        "drive": "attack_support_motor_drive_t$",
        "band": "attack_support_motor_band$",
        "band_macro": "as_motor_band",
        "leaving": "am_motor_leaving",
        "transferred": "attack_support_transferred$",
        "template_tags": ["attack_support_tpl", "ally_sup_tpl"],
        "group_tags": ["attack_support_g1", "attack_support_g2", "attack_support_g3", "attack_support_g4"],
        "pax_group": None,
        "exit_side": "opposite",
        "no_retreat": "on",
    },
    {
        "path": "resource/map/multi/defense_support_waves.inc",
        "finisher": "ds_finish_motor",
        "owner_old": "ds_own_to_defenderbot",
        "owner_new": "ds_own_motor_to_defenderbot",
        "deploy": "def_sup_deploy",
        "transfer": "def_sup_motor_transfer",
        "hull": "def_sup_motor_hull",
        "pax": "def_sup_motor_pax",
        "crew": "def_sup_motor_crew",
        "src": "def_sup_src",
        "flag": "def_sup_motor_flag",
        "stage": "defense_support_motor_stage$",
        "drive": "defense_support_motor_drive_t$",
        "band": "defense_support_motor_band$",
        "band_macro": "ds_motor_band",
        "leaving": "def_sup_motor_leaving",
        "transferred": "defense_support_transferred$",
        "template_tags": ["ally_sup_tpl"],
        "group_tags": ["def_sup_h1", "def_sup_h2", "def_sup_h3"],
        "pax_group": None,
        "exit_side": "opposite",
        "no_retreat": "off",
    },
    {
        "path": "resource/map/multi/enemy_attack_support.inc",
        "finisher": "ea_finish_motor",
        "owner_old": "ea_own_to_enemy",
        "owner_new": "ea_own_motor_to_enemy",
        "deploy": "ea_deploy",
        "transfer": "ea_motor_transfer",
        "hull": "ea_motor_hull",
        "pax": "ea_motor_pax",
        "crew": "ea_motor_crew",
        "src": "ea_src",
        "flag": "ea_motor_flag",
        "stage": "enemy_attack_motor_stage$",
        "drive": "enemy_attack_motor_drive_t$",
        "band": "enemy_attack_motor_band$",
        "band_macro": "ea_motor_band",
        "leaving": "ea_motor_leaving",
        "transferred": "enemy_attack_transferred$",
        "template_tags": ["ally_sup_tpl"],
        "group_tags": ["ea_g1", "ea_g2", "ea_g3", "ea_g4"],
        "pax_group": None,
        "exit_side": "same",
        "no_retreat": "on",
    },
    {
        "path": "resource/map/multi/enemy_defense_support.inc",
        "finisher": "ed_finish_motor",
        "owner_old": "ed_own_to_enemy",
        "owner_new": "ed_own_motor_to_enemy",
        "deploy": "enemy_def_deploy",
        "transfer": "enemy_def_motor_transfer",
        "hull": "enemy_def_motor_hull",
        "pax": "enemy_def_motor_pax",
        "crew": "enemy_def_motor_crew",
        "src": "enemy_def_src",
        "flag": "enemy_def_motor_flag",
        "stage": "enemy_defense_motor_stage$",
        "drive": "enemy_defense_motor_drive_t$",
        "band": "enemy_defense_motor_band$",
        "band_macro": "ed_motor_band",
        "leaving": "enemy_def_motor_leaving",
        "transferred": "enemy_defense_transferred$",
        "template_tags": ["ally_sup_tpl"],
        "group_tags": ["enemy_def_p1", "enemy_def_p2", "enemy_def_p3", "enemy_def_p4"],
        "pax_group": "enemy_def_p4",
        "exit_side": "same",
        "no_retreat": "off",
    },
]


def balanced(text: str, token: str) -> tuple[int, int, str]:
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
                return start, index + 1, text[start : index + 1]
    raise RuntimeError(f"unbalanced block: {token}")


def replace_block(text: str, token: str, replacement: str) -> str:
    start, end, _ = balanced(text, token)
    return text[:start] + replacement + text[end:]


def entity_state(tag: str, commands: list[str]) -> str:
    lines = [f'\t\t\t\t{{"entity_state"', f"\t\t\t\t\t{{selector {{tag {tag}}}}}"]
    lines.extend(f"\t\t\t\t\t{{{command}}}" for command in commands)
    lines.append("\t\t\t\t}")
    return "\n".join(lines)


def actor_state(tag: str, no_retreat: str) -> str:
    return f'''\t\t\t\t{{"actor_state"
\t\t\t\t\t{{selector {{ignore_captured_by_user 0}} {{tag {tag}}}}}
\t\t\t\t\t{{control AI}}
\t\t\t\t\t{{ai_move {{mode enable}}}}
\t\t\t\t\t{{weapon_prepare on}}
\t\t\t\t\t{{fire_mode open}}
\t\t\t\t\t{{move_mode free}}
\t\t\t\t\t{{movement {{speed normal}} {{kind normal}} {{type normal}}}}
\t\t\t\t\t{{ai {{no_retreat {no_retreat}}} {{advance_ratio 1}} {{retreat_ratio 0}}}}
\t\t\t\t}}'''


def ables(tag: str) -> str:
    return f'''\t\t\t\t{{"ables"
\t\t\t\t\t{{selector {{ignore_captured_by_user 0}} {{tag {tag}}}}}
\t\t\t\t\t{{remove select}}
\t\t\t\t}}'''


def exit_switch(hull: str, side: str) -> str:
    if side == "opposite":
        one, two, default = "attack_support_entry_b1", "attack_support_entry_a1", "attack_support_entry_b1"
    else:
        one, two, default = "attack_support_entry_a1", "attack_support_entry_b1", "attack_support_entry_a1"

    def order(waypoint: str, indent: str) -> str:
        return f'''{indent}{{"action"
{indent}\t{{selector {{ignore_captured_by_user 0}} {{tag {hull}}}}}
{indent}\t{{drop orders}}
{indent}\t{{action move}}
{indent}\t{{waypoint "{waypoint}"}}
{indent}}}'''

    return f'''\t\t\t\t{{"switch"
\t\t\t\t\t{{"case"
\t\t\t\t\t\t{{condition {{type cmp_i}} {{var "enemy_spawnside$"}} {{op "=="}} {{value 1}}}}
{order(one, chr(9) * 6)}
\t\t\t\t\t}}
\t\t\t\t\t{{"case"
\t\t\t\t\t\t{{condition {{type cmp_i}} {{var "enemy_spawnside$"}} {{op "=="}} {{value 2}}}}
{order(two, chr(9) * 6)}
\t\t\t\t\t}}
\t\t\t\t\t{{"default"
{order(default, chr(9) * 6)}
\t\t\t\t\t}}
\t\t\t\t}}'''


def build_finisher(c: dict[str, object]) -> str:
    c = {key: value for key, value in c.items()}
    template_removes = [f"tag_remove {tag}" for tag in c["template_tags"]]
    promote_commands = template_removes + [
        "tag_remove hidden",
        "inactive off",
        "impregnability disabled",
        "discovered on",
    ]
    initial_group_strips = []
    for subject in (c["hull"], c["pax"], c["crew"]):
        if c["group_tags"]:
            initial_group_strips.append(
                entity_state(str(subject), [f"tag_remove {tag}" for tag in c["group_tags"]])
            )
    pax_commands = [f"tag_add {c['src']}", "tag_remove hidden", "inactive off", "discovered on"]
    if c["pax_group"]:
        pax_commands.append(f"tag_add {c['pax_group']}")
    hull_exit_removes = [f"tag_remove {c['src']}"] + [
        f"tag_remove {tag}" for tag in c["group_tags"]
    ]

    return f'''\t\t\t(define "{c['finisher']}"
\t\t\t\t; Motor package isolation: the placement helper requires the shared deploy tag,
\t\t\t\t; but no infantry finisher may see the truck or its linked riders after placement.
{entity_state(str(c['hull']), [f"tag_remove {c['deploy']}", f"tag_add {c['transfer']}"])}
{entity_state(str(c['pax']), [f"tag_remove {c['deploy']}", f"tag_add {c['transfer']}"])}
{entity_state(str(c['crew']), [f"tag_remove {c['deploy']}", f"tag_add {c['transfer']}"])}
{chr(10).join(initial_group_strips)}
\t\t\t\t{{"delay" {{time 0.1}}}}
\t\t\t\t; Promote the three roles independently. Seated passengers remain outside the
\t\t\t\t; generic infantry source/patrol tags until the explicit emit below.
{entity_state(str(c['hull']), promote_commands)}
{entity_state(str(c['pax']), promote_commands)}
{entity_state(str(c['crew']), promote_commands)}
\t\t\t\t("{c['owner_new']}")
\t\t\t\t{{"delay" {{time 0.2}}}}
{actor_state(str(c['hull']), str(c['no_retreat']))}
{actor_state(str(c['crew']), str(c['no_retreat']))}
{ables(str(c['hull']))}
{ables(str(c['crew']))}
{entity_state(str(c['hull']), [f"tag_remove {c['transfer']}"])}
{entity_state(str(c['pax']), [f"tag_remove {c['transfer']}"])}
{entity_state(str(c['crew']), [f"tag_remove {c['transfer']}"])}
\t\t\t\t{{"set_i" {{var "{c['stage']}"}} {{op "="}} {{value 2}}}}
\t\t\t\t{{"set_i" {{var "{c['drive']}"}} {{op "="}} {{value 0}}}}
\t\t\t\t{{"set_i" {{var "{c['band']}"}} {{op "="}} {{value 0}}}}
\t\t\t\t{{"entity_state" {{selector {{tag {c['flag']}}}}} {{tag_remove {c['flag']}}}}}
\t\t\t\t{{"entity_state"
\t\t\t\t\t{{selector
\t\t\t\t\t\t{{source advanced}}
\t\t\t\t\t\t{{group
\t\t\t\t\t\t\t{{select {{tag {{tag flag}}}}}}
\t\t\t\t\t\t\t{{exclude {{state {{state inactive}}}}}}
\t\t\t\t\t\t}}
\t\t\t\t\t\t{{sort {{type shuffle}}}}
\t\t\t\t\t\t{{amount 1}}
\t\t\t\t\t}}
\t\t\t\t\t{{tag_add {c['flag']}}}
\t\t\t\t}}
\t\t\t\t{{"delay" {{time 0.1}}}}
\t\t\t\t; Vehicles use MOVE. ADVANCE lets infantry AI and patrol logic reinterpret the hull.
\t\t\t\t{{"action"
\t\t\t\t\t{{selector {{ignore_captured_by_user 0}} {{tag {c['hull']}}}}}
\t\t\t\t\t{{drop orders}}
\t\t\t\t\t{{action move}}
\t\t\t\t\t{{target {{ignore_captured_by_user 0}} {{tag {c['flag']}}}}}
\t\t\t\t}}
\t\t\t\t{{"set_i" {{var "{c['stage']}"}} {{op "="}} {{value 3}}}}
\t\t\t\t{{"delay" {{time 7}}}}
\t\t\t\t{{"set_i" {{var "{c['drive']}"}} {{op "="}} {{value 1}}}}
\t\t\t\t{{"delay" {{time 7}}}}
\t\t\t\t{{"set_i" {{var "{c['drive']}"}} {{op "="}} {{value 2}}}}
\t\t\t\t{{"delay" {{time 7}}}}
\t\t\t\t{{"set_i" {{var "{c['drive']}"}} {{op "="}} {{value 3}}}}
\t\t\t\t{{"delay" {{time 7}}}}
\t\t\t\t{{"set_i" {{var "{c['drive']}"}} {{op "="}} {{value 4}}}}
\t\t\t\t("{c['band_macro']}")
\t\t\t\t{{"emit"
\t\t\t\t\t{{selector
\t\t\t\t\t\t{{ignore_captured_by_user 0}}
\t\t\t\t\t\t{{tag {c['hull']}}}
\t\t\t\t\t\t{{type vehicle}}
\t\t\t\t\t\t{{state inhabited}}
\t\t\t\t\t}}
\t\t\t\t\t{{drop orders}}
\t\t\t\t\t{{emit {{mode passengers}}}}
\t\t\t\t}}
\t\t\t\t{{"set_i" {{var "{c['stage']}"}} {{op "="}} {{value 4}}}}
\t\t\t\t{{"delay" {{time 1}}}}
\t\t\t\t; Only now are the passengers normal infantry eligible for combat/patrol orders.
{entity_state(str(c['pax']), pax_commands)}
{actor_state(str(c['pax']), str(c['no_retreat']))}
{ables(str(c['pax']))}
\t\t\t\t{{"action"
\t\t\t\t\t{{selector {{ignore_captured_by_user 0}} {{tag {c['pax']}}}}}
\t\t\t\t\t{{drop orders}}
\t\t\t\t\t{{action advance}}
\t\t\t\t\t{{target {{ignore_captured_by_user 0}} {{tag {c['flag']}}}}}
\t\t\t\t}}
\t\t\t\t; The empty hull is never a support infantry/patrol member. It returns to the
\t\t\t\t; actual edge it entered from and remains cleanup-addressable for 45 seconds.
{entity_state(str(c['hull']), [f"tag_add {c['leaving']}"] + hull_exit_removes)}
{exit_switch(str(c['hull']), str(c['exit_side']))}
{entity_state(str(c['hull']), [f"tag_remove {c['deploy']}", f"tag_remove {c['hull']}"])}
{entity_state(str(c['pax']), [f"tag_remove {c['deploy']}", f"tag_remove {c['pax']}"])}
{entity_state(str(c['crew']), [f"tag_remove {c['deploy']}", f"tag_remove {c['crew']}"])}
\t\t\t\t{{"set_i" {{var "{c['transferred']}"}} {{op "="}} {{value 1}}}}
\t\t\t)'''


def add_owner_macro(text: str, c: dict[str, object]) -> str:
    old_token = f'(define "{c["owner_old"]}"'
    new_token = f'(define "{c["owner_new"]}"'
    if new_token in text:
        raise RuntimeError(f"dedicated owner already exists: {c['path']}")
    start, end, old = balanced(text, old_token)
    new = old.replace(old_token, new_token, 1)
    generic = f"{{tag {c['deploy']}}}"
    dedicated = f"{{tag {c['transfer']}}}"
    count = new.count(generic)
    if count < 1:
        raise RuntimeError(f"no ownership selectors replaced in {c['path']}")
    new = new.replace(generic, dedicated)
    return text[:end] + "\n\n\t\t\t; Motor ownership is deliberately isolated from infantry deployment.\n" + new + text[end:]


def disable_midmap_flank(text: str) -> str:
    replacement = '''\t\t\t; Normal infantry support always enters from the map edge. Mid-map flank pads
\t\t\t; produced the visible 45-second center-map spawn and are reserved from now on.
\t\t\t(define "as_choose_entry"
\t\t\t\t{"set_i" {var "attack_support_use_flank$"} {op "="} {value 0}}
\t\t\t)'''
    return replace_block(text, '(define "as_choose_entry"', replacement)


def write_tests() -> None:
    test = r'''from __future__ import annotations

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
        assert f'{{selector {{ignore_captured_by_user 0}} {{tag {deploy}}}}' not in body
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
'''
    Path("tests/test_motor_runtime_isolation.py").write_text(test, encoding="utf-8")


def main() -> None:
    for c in CONFIGS:
        path = Path(str(c["path"]))
        text = path.read_text(encoding="utf-8-sig")
        text = add_owner_macro(text, c)
        text = replace_block(text, f'(define "{c["finisher"]}"', build_finisher(c))
        if c["finisher"] == "as_finish_motor":
            text = disable_midmap_flank(text)
        path.write_text(text, encoding="utf-8")
    write_tests()


if __name__ == "__main__":
    main()
