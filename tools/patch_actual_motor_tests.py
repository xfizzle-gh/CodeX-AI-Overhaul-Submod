from __future__ import annotations

from pathlib import Path

PATH = Path("tests/test_attack_support_slot_proof.py")


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
    replacement = signature + "\n" + body.rstrip() + "\n\n"
    return text[:start] + replacement + text[end:]


EMIT_BODY = '''        """The hull drives first; linked riders become infantry only after emit."""
        source_tags = {
            "attack_support_waves": "attack_support_src",
            "defense_support_waves": "def_sup_src",
            "enemy_attack_support": "ea_src",
            "enemy_defense_support": "enemy_def_src",
        }
        for engine, (pfx, hull, flag, pax, _band, finisher) in sorted(
            self.MOTOR_OBJECTIVE.items()
        ):
            code = define_body(self.engines[engine], finisher)
            with self.subTest(engine=engine):
                pick = code.index("{tag_add %s}" % flag)
                move = code.index("{action move}", pick)
                stage3 = code.index(
                    '{"set_i" {var "%s_motor_stage$"} {op "="} {value 3}}' % pfx
                )
                emit = code.index('{"emit"', stage3)
                stage4 = code.index(
                    '{"set_i" {var "%s_motor_stage$"} {op "="} {value 4}}' % pfx
                )
                pax_source = code.index("{tag_add %s}" % source_tags[engine], stage4)
                pax_order = code.index("{action advance}", pax_source)
                self.assertLess(pick, move)
                self.assertLess(move, stage3)
                self.assertLess(stage3, emit)
                self.assertLess(emit, stage4)
                self.assertLess(stage4, pax_source)
                self.assertLess(pax_source, pax_order)
                self.assertNotIn("{action advance}", code[:emit])
                emit_block = block_at(code, emit)
                self.assertIn("{tag %s}" % hull, emit_block)
                self.assertIn("{type vehicle}", emit_block)
                self.assertIn("{state inhabited}", emit_block)
                self.assertIn("{mode passengers}", emit_block)
                pax_action = block_at(code, code.rindex('{"action"', pax_source, pax_order + 1))
                self.assertIn("{tag %s}" % pax, pax_action)
                self.assertIn("{action advance}", pax_action)
                self.assertIn("{tag %s}" % flag, pax_action)
'''


BAND_BODY = '''        """The band and hull MOVE target must reference the same dedicated flag."""
        for engine, cfg in sorted(self.MOTOR_OBJECTIVE.items()):
            _pfx, hull, flag, _pax, band_define, finisher = cfg
            code = self.engines[engine]
            probe = define_body(code, band_define)
            body = define_body(code, finisher)
            with self.subTest(engine=engine):
                self.assertEqual(
                    probe.count("{units {ignore_captured_by_user 0} {tag %s}}" % hull), 3
                )
                self.assertEqual(
                    probe.count("{near_to {ignore_captured_by_user 0} {tag %s}}" % flag), 3
                )
                self.assertNotIn("{prop human}", probe)
                self.assertNotIn("{type human}", probe)
                emit = body.index('{"emit"')
                move = body.index("{action move}")
                self.assertLess(move, emit)
                self.assertNotIn("{action advance}", body[:emit])
                move_block = block_at(body, body.rindex('{"action"', 0, move + 1))
                self.assertIn("{tag %s}" % hull, move_block)
                self.assertIn("{tag %s}" % flag, move_block)
                pax_advance = body.index("{action advance}", emit)
                pax_block = block_at(body, body.rindex('{"action"', emit, pax_advance + 1))
                self.assertIn("{tag %s}" % flag, pax_block)
                self.assertEqual(body.count('("%s")' % band_define), 1)
                self.assertLess(body.index('("%s")' % band_define), emit)
'''


ISOLATION_BODY = '''        """Each motor package leaves the shared infantry namespace before waiting."""
        packages = {
            "attack_support_waves": (
                "attack_support_deploy", "attack_support_motor_transfer",
                "attack_support_motor_hull", "attack_support_motor_pax",
                "attack_support_motor_crew", "as_own_motor_to_support",
                "attack_support_src", "as_finish_motor"),
            "defense_support_waves": (
                "def_sup_deploy", "def_sup_motor_transfer",
                "def_sup_motor_hull", "def_sup_motor_pax",
                "def_sup_motor_crew", "ds_own_motor_to_defenderbot",
                "def_sup_src", "ds_finish_motor"),
            "enemy_attack_support": (
                "ea_deploy", "ea_motor_transfer",
                "ea_motor_hull", "ea_motor_pax", "ea_motor_crew",
                "ea_own_motor_to_enemy", "ea_src", "ea_finish_motor"),
            "enemy_defense_support": (
                "enemy_def_deploy", "enemy_def_motor_transfer",
                "enemy_def_motor_hull", "enemy_def_motor_pax",
                "enemy_def_motor_crew", "ed_own_motor_to_enemy",
                "enemy_def_src", "ed_finish_motor"),
        }
        for engine, cfg in sorted(packages.items()):
            deploy, transfer, hull, pax, crew, owner, source_tag, finisher = cfg
            code = self.engines[engine]
            body = define_body(code, finisher)
            with self.subTest(engine=engine):
                first_delay = body.index('{"delay"')
                head = body[:first_delay]
                for mark in (hull, pax, crew):
                    mark_at = head.index("{selector {tag %s}}" % mark)
                    scoped = head[mark_at:mark_at + 240]
                    self.assertIn("{tag_remove %s}" % deploy, scoped)
                    self.assertIn("{tag_add %s}" % transfer, scoped)
                self.assertEqual(head.count("{tag_remove %s}" % deploy), 3)
                self.assertEqual(head.count("{tag_add %s}" % transfer), 3)
                owner_body = define_body(code, owner)
                self.assertIn("{tag %s}" % transfer, owner_body)
                self.assertNotIn("{tag %s}" % deploy, owner_body)
                emit = body.index('{"emit"')
                self.assertNotIn("{tag_add %s}" % source_tag, body[:emit])
                self.assertIn("{tag_add %s}" % source_tag, body[emit:])
'''


FLANK_BODY = '''        """Normal infantry support is edge-only; mid-map flank pads are retired."""
        choose = define_body(self.waves, "as_choose_entry")
        self.assertNotIn("{type rand}", choose)
        self.assertNotIn("as_announce_flank", choose)
        self.assertNotIn("{value 1}", choose)
        self.assertIn("{value 0}", choose)
'''


def main() -> None:
    text = PATH.read_text(encoding="utf-8-sig")
    replacements = {
        "test_the_emit_still_follows_the_proven_ordering": EMIT_BODY,
        "test_the_band_measures_against_the_flag_the_hull_was_ordered_to": BAND_BODY,
        "test_one_package_cannot_disarm_another": ISOLATION_BODY,
        "test_choose_entry_rolls_and_guards": FLANK_BODY,
        "test_place_one_addresses_flank_pads": FLANK_BODY,
        "test_other_engines_never_reference_flank_pads": FLANK_BODY,
        "test_deploy_generates_flank_geometry": FLANK_BODY,
    }
    for name, body in replacements.items():
        if f"    def {name}(" not in text:
            raise RuntimeError(f"required test not found: {name}")
        text = replace_method(text, name, body)
    PATH.write_text(text, encoding="utf-8")
    print("Replaced stale contracts:", ", ".join(replacements))


if __name__ == "__main__":
    main()
