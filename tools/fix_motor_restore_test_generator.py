from pathlib import Path

path = Path("tools/restore_motor_package1.py")
text = path.read_text(encoding="utf-8")
replacements = (
    (
        "        assert f'{{selector {{ignore_captured_by_user 0}} {{tag {pax}}}}' in body",
        '        assert ("{selector {ignore_captured_by_user 0} {tag " + pax + "}}") in body',
    ),
    (
        "        assert f'{{select {{tag {{tag {pax}}}}}}' not in body",
        '        assert ("{select {tag {tag " + pax + "}}}") not in body',
    ),
)
for old, new in replacements:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"expected one generated-test line, found {count}: {old}")
    text = text.replace(old, new, 1)
path.write_text(text, encoding="utf-8")
