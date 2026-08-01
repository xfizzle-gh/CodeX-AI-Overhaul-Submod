from __future__ import annotations

from pathlib import Path

PATH = Path("tests/test_defense_mission_support.py")


def replace_method(text: str, name: str, body: str) -> str:
    marker = f"    def {name}("
    start = text.index(marker)
    signature_end = text.index("\n", start)
    signature = text[start:signature_end]
    candidates = []
    next_method = text.find("\n    def ", signature_end)
    if next_method >= 0:
        candidates.append(next_method + 1)
    next_class = text.find("\nclass ", signature_end)
    if next_class >= 0:
        candidates.append(next_class + 1)
    next_main = text.find("\nif __name__", signature_end)
    if next_main >= 0:
        candidates.append(next_main + 1)
    end = min(candidates) if candidates else len(text)
    return text[:start] + signature + "\n" + body.rstrip() + "\n\n" + text[end:]


def replace_exact(text: str, old: str, new: str, label: str) -> str:
    if old in text:
        return text.replace(old, new, 1)
    if new in text:
        return text
    raise RuntimeError(f"required contract not found: {label}")


MOTOR_INVENTORY_BODY = '''        for name, spec in MOTOR_ENGINES.items():
            (_p, _gate, ns, pattern, poke, finish,
             own, place, _var_pfx, tag_pfx) = spec
            code = self.code[name]
            with self.subTest(engine=name):
                for fac in MOTOR_FACTIONS:
                    self.assertIn('{"%s/%s"' % (ns, pattern % fac), code)
                self.assertIn('{"%s/motor_clock"' % ns, code)
                self.assertIn('{"%s/motor_cleanup"' % ns, code)
                self.assertEqual(code.count('(define "%s"' % poke), 1)
                self.assertEqual(code.count('(define "%s"' % finish), 1)
                body = define_body(code, finish)
                # Motor ownership is isolated from ordinary infantry ownership so
                # seated riders cannot be selected or ordered before explicit emit.
                self.assertIn('("%s")' % own, body)
                self.assertIn("{mode passengers}", body)
                # Empty hulls return through the real named entry pads, not the old
                # generic waypoint 0 that could route them back into active combat.
                self.assertNotIn('{waypoint "0"}', body)
                for side in ("a1", "b1"):
                    self.assertIn(
                        '{waypoint "attack_support_entry_%s"}' % side,
                        body,
                    )
                self.assertIn("{tag_add %s}" % MOTOR_LEAVING[name], body)
                self.assertIn("{tag_add %s_hull}" % tag_pfx, code)
                self.assertIn("{tag_add %s_pax}" % tag_pfx, code)
                # Placement runs before promotion on every motor deploy.
                for fac in MOTOR_FACTIONS:
                    blk = trigger_block(code, "%s/%s" % (ns, pattern % fac))
                    self.assertLess(
                        blk.index('("%s")' % place), blk.index('("%s")' % finish)
                    )
                # And the placement run is wide enough for the widest package:
                # hull + 2 crew + 8 linked riders = 11 bodies.
                self.assertGreaterEqual(
                    define_body(code, place).count('("%s")' % PLACE_ONE[place]), 11
                )
                # Cleanup removes the departed hull after 45s, wreck included.
                cleanup = trigger_block(code, "%s/motor_cleanup" % ns)
                self.assertIn('{"delay" {time 45}}', cleanup)
                self.assertIn(
                    '{"delete" {selector {ignore_captured_by_user 0} {tag %s}}}'
                    % MOTOR_LEAVING[name],
                    cleanup,
                )
'''


def main() -> None:
    text = PATH.read_text(encoding="utf-8-sig")

    # The normal infantry finishes still use their original owner helpers.
    text = replace_exact(
        text,
        '(self.ds, "ds_own_motor_to_defenderbot", "ds_place_at_entry", "ds_finish"),',
        '(self.ds, "ds_own_to_defenderbot", "ds_place_at_entry", "ds_finish"),',
        "Q2 normal owner loop",
    )
    text = replace_exact(
        text,
        '(self.ea, "ea_own_motor_to_enemy", "ea_place_at_entry", "ea_finish"),',
        '(self.ea, "ea_own_to_enemy", "ea_place_at_entry", "ea_finish"),',
        "Q3 normal owner loop",
    )

    start = text.index("MOTOR_ENGINES = {")
    end = text.index("\nMOTOR_FACTIONS =", start)
    mapping = '''MOTOR_ENGINES = {
    "Q1 friendly attack": (Q1, 0, "attack_support", "ally_%s_motor",
                           "as_poke_faction_motor", "as_finish_motor",
                           "as_own_motor_to_support", "am_place_at_entry",
                           "attack_support", "attack_support_motor"),
    "Q2 friendly defence": (DS, 1, "defense_support", "ally_%s_motor",
                            "ds_poke_motor", "ds_finish_motor",
                            "ds_own_motor_to_defenderbot", "ds_place_at_entry",
                            "defense_support", "def_sup_motor"),
    "Q3 enemy attack": (EA, 1, "enemy_attack", "%s_motor",
                        "ea_poke_motor", "ea_finish_motor",
                        "ea_own_motor_to_enemy", "ea_place_at_entry",
                        "enemy_attack", "ea_motor"),
    "Q4 enemy defence": (Q4, 0, "enemy_defense", "%s_motor",
                         "ed_poke_motor", "ed_finish_motor",
                         "ed_own_motor_to_enemy", "ed_place",
                         "enemy_defense", "enemy_def_motor"),
}'''
    text = text[:start] + mapping + text[end:]
    text = replace_method(
        text,
        "test_every_engine_carries_the_full_motor_inventory",
        MOTOR_INVENTORY_BODY,
    )
    PATH.write_text(text, encoding="utf-8")
    print("Aligned remaining motor inventory and ownership contracts")


if __name__ == "__main__":
    main()
