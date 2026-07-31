from __future__ import annotations

from pathlib import Path
import re

FILES = {
    "resource/map/multi/attack_support_waves.inc": (
        "as_finish_motor",
        "attack_support_motor_pax",
        "attack_support/ally_",
    ),
    "resource/map/multi/defense_support_waves.inc": (
        "ds_finish_motor",
        "def_sup_motor_pax",
        "defense_support/ally_",
    ),
    "resource/map/multi/enemy_attack_support.inc": (
        "ea_finish_motor",
        "ea_motor_pax",
        "enemy_attack/",
    ),
    "resource/map/multi/enemy_defense_support.inc": (
        "ed_finish_motor",
        "enemy_def_motor_pax",
        "enemy_defense/",
    ),
}
FACTIONS = ("rusa", "ukr", "nato", "prc")


def balanced_block(text: str, token: str) -> tuple[int, int, str]:
    start = text.index(token)
    opener = token[0]
    if opener not in "({":
        raise RuntimeError(f"unsupported block token: {token}")
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


def replace_block(text: str, token: str, transform) -> str:
    start, end, current = balanced_block(text, token)
    changed = transform(current)
    if changed == current:
        raise RuntimeError(f"no change made in {token}")
    return text[:start] + changed + text[end:]


def restore_finisher(block: str, path: str, finisher_name: str, pax_tag: str) -> str:
    pattern = re.compile(
        r"\{selector\s*\n\s*\{source advanced\}\s*\n"
        r"\s*\{ignore_captured_by_user 0\}\s*\n"
        r"\s*\{group\s*\n"
        r"\s*\{select \{tag \{tag "
        + re.escape(pax_tag)
        + r"\}\}\}\s*\n"
        r"\s*\{exclude \{state \{state linked\}\} \{state \{state inactive\}\} \{state \{state dead\}\}\s*\n"
        r"\s*\}\s*\n\s*\}"
    )
    replacement = f"{{selector {{ignore_captured_by_user 0}} {{tag {pax_tag}}}}}"
    changed, count = pattern.subn(replacement, block)
    if count != 1:
        raise RuntimeError(
            f"{path}:{finisher_name}: expected one advanced pax selector, found {count}"
        )
    return changed


def pin_trigger_to_package1(block: str, path: str, faction: str, trigger: str) -> str:
    generic = f"{{selector {{tag ally_sup_{faction}_motor_hull}}}}"
    package1 = f"{{selector {{tag ally_sup_{faction}_p1_hull}}}}"
    condition, actions = block.split("{actions", 1)
    count = condition.count(generic)
    if count != 1:
        raise RuntimeError(
            f"{path}:{trigger}: expected one generic hull condition, found {count}"
        )
    condition = condition.replace(generic, package1, 1)
    return condition + "{actions" + actions


def write_regression_test() -> None:
    content = r'''from __future__ import annotations

from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
FACTIONS = ("rusa", "ukr", "nato", "prc")
ENGINES = {
    "resource/map/multi/attack_support_waves.inc": ("as_finish_motor", "attack_support_motor_pax", "attack_support/ally_"),
    "resource/map/multi/defense_support_waves.inc": ("ds_finish_motor", "def_sup_motor_pax", "defense_support/ally_"),
    "resource/map/multi/enemy_attack_support.inc": ("ea_finish_motor", "ea_motor_pax", "enemy_attack/"),
    "resource/map/multi/enemy_defense_support.inc": ("ed_finish_motor", "enemy_def_motor_pax", "enemy_defense/"),
}


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


def test_motor_triggers_are_package1_only() -> None:
    for relative, (_, _, prefix) in ENGINES.items():
        text = (ROOT / relative).read_text(encoding="utf-8")
        for faction in FACTIONS:
            trigger = block(text, '{"' + prefix + faction + '_motor"')
            condition = trigger.split("{actions", 1)[0]
            assert f'{{selector {{tag ally_sup_{faction}_p1_hull}}}}' in condition
            assert f'{{selector {{tag ally_sup_{faction}_motor_hull}}}}' not in condition


def test_finishers_use_live_proven_passenger_selector() -> None:
    for relative, (finisher, pax, _) in ENGINES.items():
        text = (ROOT / relative).read_text(encoding="utf-8")
        body = block(text, f'(define "{finisher}"')
        assert f'{{selector {{ignore_captured_by_user 0}} {{tag {pax}}}}' in body
        assert f'{{select {{tag {{tag {pax}}}}}}' not in body


def test_russian_package1_is_still_a_loaded_ural() -> None:
    text = (ROOT / "resource/map/multi/faction_support_templates.inc").read_text(encoding="utf-8")
    ids = set(re.findall(r'\{Tags [^\n]*"ally_sup_rusa_p1_[^"]+"[^\n]*(0x[0-9a-fA-F]+)\}', text))
    assert ids == {"0xb3a0", "0xb3a1", "0xb3a2", "0xb3a3", "0xb3a4", "0xb3a5", "0xb3a6", "0xb3a7", "0xb3a8", "0xb3a9", "0xb3aa"}
    assert '{Entity "ural" 0xb3a0' in text
    expected = {
        ("0xb3a1", "driver"), ("0xb3a2", "commander"),
        ("0xb3a3", "seat1"), ("0xb3a4", "seat2"), ("0xb3a5", "seat3"), ("0xb3a6", "seat4"),
        ("0xb3a7", "seat5"), ("0xb3a8", "seat6"), ("0xb3a9", "seat7"), ("0xb3aa", "seat8"),
    }
    actual = set(re.findall(r'\{Link (0xb3a[1-9a]) \{0xb3a0 "([^"]+)"\}\}', text))
    assert actual == expected
'''
    Path("tests/test_motor_package1_restore.py").write_text(content, encoding="utf-8")


def main() -> None:
    for path, (finisher_name, pax_tag, trigger_prefix) in FILES.items():
        source = Path(path).read_text(encoding="utf-8")
        source = replace_block(
            source,
            f'(define "{finisher_name}"',
            lambda current, path=path, finisher_name=finisher_name, pax_tag=pax_tag: restore_finisher(
                current, path, finisher_name, pax_tag
            ),
        )
        for faction in FACTIONS:
            trigger = f"{trigger_prefix}{faction}_motor"
            source = replace_block(
                source,
                '{"' + trigger + '"',
                lambda current, path=path, faction=faction, trigger=trigger: pin_trigger_to_package1(
                    current, path, faction, trigger
                ),
            )
        Path(path).write_text(source, encoding="utf-8")
    write_regression_test()


if __name__ == "__main__":
    main()
