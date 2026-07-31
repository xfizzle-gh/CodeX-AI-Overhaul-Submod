from __future__ import annotations

from pathlib import Path
import re

TEST_PATH = Path("tests/test_attack_support_slot_proof.py")


def extract_helper(indent: str = "        ") -> str:
    return f'''{indent}def extract(source: str, token: str) -> str:
{indent}    start = source.index(token)
{indent}    opener = token[0]
{indent}    closer = ")" if opener == "(" else "}}"
{indent}    depth = 0
{indent}    quoted = False
{indent}    escaped = False
{indent}    for index in range(start, len(source)):
{indent}        char = source[index]
{indent}        if quoted:
{indent}            if escaped:
{indent}                escaped = False
{indent}            elif char == "\\\\":
{indent}                escaped = True
{indent}            elif char == '"':
{indent}                quoted = False
{indent}            continue
{indent}        if char == '"':
{indent}            quoted = True
{indent}        elif char == opener:
{indent}            depth += 1
{indent}        elif char == closer:
{indent}            depth -= 1
{indent}            if depth == 0:
{indent}                return source[start:index + 1]
{indent}    raise AssertionError(token)
'''


def motor_drive_method(signature: str) -> str:
    return signature + "\n" + extract_helper() + '''
        root = Path(__file__).resolve().parents[1]
        configs = (
            ("resource/map/multi/attack_support_waves.inc", "as_finish_motor"),
            ("resource/map/multi/defense_support_waves.inc", "ds_finish_motor"),
            ("resource/map/multi/enemy_attack_support.inc", "ea_finish_motor"),
            ("resource/map/multi/enemy_defense_support.inc", "ed_finish_motor"),
        )
        for relative, finisher in configs:
            source = (root / relative).read_text(encoding="utf-8-sig")
            body = extract(source, f'(define "{finisher}"')
            emit_at = body.index('{"emit"')
            before_emit = body[:emit_at]
            after_emit = body[emit_at:]
            self.assertIn("{action move}", before_emit)
            self.assertNotIn("{action advance}", before_emit)
            self.assertIn("{action advance}", after_emit)
            self.assertNotIn('{waypoint "0"}', body)
            self.assertIn("attack_support_entry_a1", body)
            self.assertIn("attack_support_entry_b1", body)

        templates = (root / "resource/map/multi/faction_support_templates.inc").read_text(
            encoding="utf-8-sig"
        )
        for faction in ("rusa", "ukr", "nato", "prc"):
            self.assertIn(f'"ally_sup_{faction}_p1_hull"', templates)
'''


def linked_order_method(signature: str) -> str:
    return signature + "\n" + extract_helper() + '''
        root = Path(__file__).resolve().parents[1]
        configs = (
            ("resource/map/multi/attack_support_waves.inc", "as_finish_motor", "attack_support_motor_pax", "attack_support_src"),
            ("resource/map/multi/defense_support_waves.inc", "ds_finish_motor", "def_sup_motor_pax", "def_sup_src"),
            ("resource/map/multi/enemy_attack_support.inc", "ea_finish_motor", "ea_motor_pax", "ea_src"),
            ("resource/map/multi/enemy_defense_support.inc", "ed_finish_motor", "enemy_def_motor_pax", "enemy_def_src"),
        )
        for relative, finisher, pax_tag, source_tag in configs:
            source = (root / relative).read_text(encoding="utf-8-sig")
            body = extract(source, f'(define "{finisher}"')
            emit_at = body.index('{"emit"')
            before_emit = body[:emit_at]
            after_emit = body[emit_at:]

            # Linked riders may be promoted and transferred, but they must not be
            # enrolled in infantry command/patrol namespaces or ordered before emit.
            self.assertNotIn(f"{{tag_add {source_tag}}}", before_emit)
            self.assertNotIn("{action advance}", before_emit)
            self.assertIn(f"{{tag_add {source_tag}}}", after_emit)
            self.assertIn(f"{{tag {pax_tag}}}", after_emit)
            self.assertIn("{action advance}", after_emit)
'''


def flank_method(signature: str) -> str:
    return signature + "\n" + extract_helper() + '''
        root = Path(__file__).resolve().parents[1]
        source = (root / "resource/map/multi/attack_support_waves.inc").read_text(
            encoding="utf-8-sig"
        )
        choose = extract(source, '(define "as_choose_entry"')
        self.assertNotIn("{type rand}", choose)
        self.assertNotIn("as_announce_flank", choose)
        self.assertNotIn("{value 1}", choose)
        self.assertIn("{value 0}", choose)
'''


def replace_method(text: str, method_name: str, renderer) -> tuple[str, bool]:
    pattern = re.compile(
        rf"(?m)^    def {re.escape(method_name)}\([^\n]*\):[^\n]*$"
    )
    match = pattern.search(text)
    if not match:
        return text, False
    start = match.start()
    signature = match.group(0)
    next_method = re.search(r"(?m)^    def [A-Za-z0-9_]+\(", text[match.end():])
    next_class = re.search(r"(?m)^class [A-Za-z0-9_]+", text[match.end():])
    candidates = []
    if next_method:
        candidates.append(match.end() + next_method.start())
    if next_class:
        candidates.append(match.end() + next_class.start())
    end = min(candidates) if candidates else len(text)
    replacement = renderer(signature).rstrip() + "\n\n"
    return text[:start] + replacement + text[end:], True


def main() -> None:
    text = TEST_PATH.read_text(encoding="utf-8-sig")
    if "from pathlib import Path" not in text:
        insertion = text.find("\n", text.find("import ")) + 1
        text = text[:insertion] + "from pathlib import Path\n" + text[insertion:]

    exact = {
        "test_motor_templates_are_live_not_purged_and_use_safe_drive_orders": motor_drive_method,
        "test_order_selectors_exclude_linked_inactive_and_dead": linked_order_method,
    }
    for name, renderer in exact.items():
        text, changed = replace_method(text, name, renderer)
        if not changed:
            raise RuntimeError(f"required stale motor test not found: {name}")

    method_names = re.findall(r"(?m)^    def (test_[A-Za-z0-9_]*flank[A-Za-z0-9_]*)\(", text)
    if not method_names:
        raise RuntimeError("no flank tests found to update")
    for name in method_names:
        text, changed = replace_method(text, name, flank_method)
        if not changed:
            raise RuntimeError(f"failed to update flank test: {name}")

    TEST_PATH.write_text(text, encoding="utf-8")
    print("Updated exact motor tests:", ", ".join(exact))
    print("Updated retired flank tests:", ", ".join(method_names))


if __name__ == "__main__":
    main()
