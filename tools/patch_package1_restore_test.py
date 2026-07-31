from __future__ import annotations

from pathlib import Path

PATH = Path("tests/test_motor_package1_restore.py")


def replace_function(text: str, name: str, replacement: str) -> str:
    marker = f"def {name}("
    start = text.index(marker)
    next_function = text.find("\ndef ", start + len(marker))
    end = next_function + 1 if next_function >= 0 else len(text)
    return text[:start] + replacement.rstrip() + "\n\n" + text[end:]


def main() -> None:
    text = PATH.read_text(encoding="utf-8")
    replacement = '''def test_finishers_isolate_linked_passengers_until_emit() -> None:
    source_tags = {
        "resource/map/multi/attack_support_waves.inc": "attack_support_src",
        "resource/map/multi/defense_support_waves.inc": "def_sup_src",
        "resource/map/multi/enemy_attack_support.inc": "ea_src",
        "resource/map/multi/enemy_defense_support.inc": "enemy_def_src",
    }
    for relative, (finisher, pax, _) in ENGINES.items():
        text = (ROOT / relative).read_text(encoding="utf-8")
        body = block(text, f'(define "{finisher}"')
        emit_at = body.index('{"emit"')
        before_emit = body[:emit_at]
        after_emit = body[emit_at:]

        # Formatting is irrelevant. The contract is that the linked riders remain
        # addressable, receive no infantry-source tag before emit, and become normal
        # infantry only after the truck has explicitly unloaded them.
        assert ("{tag " + pax + "}") in body
        assert ("{select {tag {tag " + pax + "}}}") not in body
        assert ("{tag_add " + source_tags[relative] + "}") not in before_emit
        assert ("{tag_add " + source_tags[relative] + "}") in after_emit
        assert "{action advance}" not in before_emit
        assert "{action advance}" in after_emit
'''
    old_name = "test_finishers_use_live_proven_passenger_selector"
    if f"def {old_name}(" not in text:
        raise RuntimeError(f"stale package-1 selector test not found: {old_name}")
    text = replace_function(text, old_name, replacement)
    PATH.write_text(text, encoding="utf-8")


if __name__ == "__main__":
    main()
