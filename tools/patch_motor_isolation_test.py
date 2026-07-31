from pathlib import Path

path = Path("tests/test_motor_runtime_isolation.py")
text = path.read_text(encoding="utf-8")
old = "        assert f'{selector {ignore_captured_by_user 0} {tag {deploy}}}' not in body"
# The generated source contains escaped literal braces; replace the exact rendered line.
rendered_old = "        assert f'{{selector {{ignore_captured_by_user 0}} {{tag {deploy}}}}' not in body"
new = "        assert ('{selector {ignore_captured_by_user 0} {tag ' + deploy + '}}') not in body"
count = text.count(rendered_old)
if count != 1:
    raise RuntimeError(f"expected one malformed selector assertion, found {count}")
path.write_text(text.replace(rendered_old, new, 1), encoding="utf-8")
