from __future__ import annotations

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
        assert ("{selector {ignore_captured_by_user 0} {tag " + pax + "}}") in body
        assert ("{select {tag {tag " + pax + "}}}") not in body


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
