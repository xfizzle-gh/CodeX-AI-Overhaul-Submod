from __future__ import annotations

from pathlib import Path

ATTACK_PATH = Path("tests/test_attack_support_slot_proof.py")
DEFENSE_PATH = Path("tests/test_defense_mission_support.py")
E2_PATH = Path("tests/test_e2_airmobile.py")
ENEMY_DEFENSE_PATH = Path("tests/test_enemy_defense_support.py")


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


def replace_exact(text: str, old: str, new: str, label: str) -> str:
    if old in text:
        return text.replace(old, new, 1)
    if new in text:
        return text
    raise RuntimeError(f"required contract not found: {label}")


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


ENEMY_OWNER_BODY = '''        code = self.code
        infantry_own = define_body(code, "ed_own_to_enemy")
        motor_own = define_body(code, "ed_own_motor_to_enemy")
        for n in range(1, 17):
            condition = (
                '{condition {type cmp_i} {var "id_1st_enemy$"} '
                '{op "=="} {value %d}}' % n
            )
            player = '{player "%d"}' % n
            for own in (infantry_own, motor_own):
                self.assertIn(condition, own)
                self.assertIn(player, own)
        self.assertNotIn('{player "id_1st_enemy$"}', code)
        for own in (infantry_own, motor_own):
            self.assertNotIn('{player "17"}', own)
        self.assertNotIn('{player "0"}', code)
        self.assertEqual(code.count('("ed_own_to_enemy")'), 1)
        self.assertEqual(code.count('("ed_own_motor_to_enemy")'), 1)
        self.assertIn('("ed_own_to_enemy")', define_body(code, "ed_finish"))
        self.assertIn(
            '("ed_own_motor_to_enemy")',
            define_body(code, "ed_finish_motor"),
        )
        self.assertIn(
            'BotApi.Scene:SetVar("id_1st_enemy", firstEnemyId)',
            self.conquest,
        )
'''


ENEMY_PATROL_BODY = '''        code = self.code
        assign = define_body(code, "ed_assign_group")
        for n in (1, 2, 3, 4):
            self.assertIn("{tag_add enemy_def_p%d}" % n, assign)
        for n in (1, 2, 3):
            self.assertIn(
                '{condition {type cmp_i} {var "enemy_defense_group$"} '
                '{op "=="} {value %d}}' % n,
                assign,
            )

        normal_finish = define_body(code, "ed_finish")
        motor_finish = define_body(code, "ed_finish_motor")
        for n in (1, 2, 3, 4):
            marker = "{tag_remove enemy_def_p%d}" % n
            self.assertNotIn(marker, normal_finish)
            self.assertIn(marker, motor_finish)
        self.assertNotIn("{tag_remove enemy_def_src}", normal_finish)
        self.assertIn("{tag_remove enemy_def_src}", motor_finish)
        self.assertNotIn("{tag_add enemy_def_rusa_line}", code)
        self.assertIn(
            "{tag_remove enemy_def_deploy}",
            normal_finish,
        )
'''


def patch_attack_support() -> None:
    text = ATTACK_PATH.read_text(encoding="utf-8-sig")
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

    text = replace_exact(
        text,
        '''        # attack_support_src is never removed: it marks everything the engine owns
        # and the live-unit cap counts it.
        self.assertNotIn("{tag_remove attack_support_src}", self.waves)
''',
        '''        # Infantry keeps the live-roster marker. Only an empty motor hull drops it
        # before its dedicated exit path so patrol and cap logic cannot reclaim it.
        normal_finish = _mi_define(self.code, "am_finish_deploy")
        motor_finish = _mi_define(self.code, "as_finish_motor")
        self.assertNotIn("{tag_remove attack_support_src}", normal_finish)
        self.assertIn("{tag_remove attack_support_src}", motor_finish)
''',
        "attack support live-roster scope",
    )
    ATTACK_PATH.write_text(text, encoding="utf-8")


def patch_defense_support() -> None:
    text = DEFENSE_PATH.read_text(encoding="utf-8-sig")
    text = replace_exact(
        text,
        '''                       "ds_place_at_entry", "ds_place_one",
                       "ds_own_to_defenderbot",
                       "ds_report_owner",''',
        '''                       "ds_place_at_entry", "ds_place_one",
                       "ds_own_to_defenderbot", "ds_own_motor_to_defenderbot",
                       "ds_report_owner",''',
        "defense dedicated motor owner define",
    )
    text = replace_exact(
        text,
        '''                       "ea_place_at_entry", "ea_place_one",
                       "ea_own_to_enemy",
                       "ea_resolve_army",''',
        '''                       "ea_place_at_entry", "ea_place_one",
                       "ea_own_to_enemy", "ea_own_motor_to_enemy",
                       "ea_resolve_army",''',
        "enemy attack dedicated motor owner define",
    )
    text = replace_exact(
        text,
        '''        self.assertIn('(\"ea_own_to_enemy\")', define_body(self.ea, "ea_finish_motor"))
''',
        '''        self.assertIn(
            '(\"ds_own_motor_to_defenderbot\")',
            define_body(self.ds, "ds_finish_motor"),
        )
        self.assertIn(
            '(\"ea_own_motor_to_enemy\")',
            define_body(self.ea, "ea_finish_motor"),
        )
''',
        "dedicated motor owner calls",
    )
    text = replace_exact(
        text,
        '''                # The roster marker is never removed, or the cap would stop counting.
                self.assertNotIn("{tag_remove %s}" % tag, code)
''',
        '''                # Normal infantry stays counted. The empty motor hull drops the
                # roster marker before exiting so cap and patrol logic cannot reclaim it.
                normal_name = "ds_finish" if prefix == "defense_support" else "ea_finish"
                motor_name = (
                    "ds_finish_motor" if prefix == "defense_support" else "ea_finish_motor"
                )
                self.assertNotIn(
                    "{tag_remove %s}" % tag,
                    define_body(code, normal_name),
                )
                self.assertIn(
                    "{tag_remove %s}" % tag,
                    define_body(code, motor_name),
                )
''',
        "defense live-cap marker scope",
    )
    text = replace_exact(
        text,
        '''        self.assertNotIn("ea_g3", code)
''',
        '''        self.assertNotIn("ea_g3", finish)
''',
        "enemy attack infantry group scope",
    )
    for old, new, label in (
        ('"am_own_to_support", "am_place_at_entry",',
         '"as_own_motor_to_support", "am_place_at_entry",',
         "Q1 motor owner mapping"),
        ('"ds_own_to_defenderbot", "ds_place_at_entry",',
         '"ds_own_motor_to_defenderbot", "ds_place_at_entry",',
         "Q2 motor owner mapping"),
        ('"ea_own_to_enemy", "ea_place_at_entry",',
         '"ea_own_motor_to_enemy", "ea_place_at_entry",',
         "Q3 motor owner mapping"),
        ('"ed_own_to_enemy", "ed_place",',
         '"ed_own_motor_to_enemy", "ed_place",',
         "Q4 motor owner mapping"),
    ):
        text = replace_exact(text, old, new, label)
    DEFENSE_PATH.write_text(text, encoding="utf-8")


def patch_e2() -> None:
    text = E2_PATH.read_text(encoding="utf-8-sig")
    text = replace_exact(
        text,
        '''        # "0" is the base-game roam/exit node the motorised insert already uses.
        self.assertEqual(numeric_targets, {"0"} | set(self.BAND))
''',
        '''        # Motor cleanup now routes through named map-edge entry waypoints.
        self.assertEqual(numeric_targets, set(self.BAND))
        self.assertNotIn('{waypoint "0"}', mi_define(live, "as_finish_motor"))
''',
        "E2 numeric waypoint set",
    )
    E2_PATH.write_text(text, encoding="utf-8")


def patch_enemy_defense() -> None:
    text = ENEMY_DEFENSE_PATH.read_text(encoding="utf-8-sig")
    text = replace_exact(
        text,
        '''            "ed_own_to_enemy",
            "ed_place",''',
        '''            "ed_own_to_enemy",
            "ed_own_motor_to_enemy",
            "ed_place",''',
        "enemy defense dedicated motor owner define",
    )
    text = replace_method(
        text,
        "test_ownership_switch_covers_every_literal_player_slot",
        ENEMY_OWNER_BODY,
    )
    text = replace_method(
        text,
        "test_patrollers_are_tag_swapped_out_of_the_spawner_pools",
        ENEMY_PATROL_BODY,
    )
    ENEMY_DEFENSE_PATH.write_text(text, encoding="utf-8")


def main() -> None:
    patch_attack_support()
    patch_defense_support()
    patch_e2()
    patch_enemy_defense()
    print(
        "Updated motor lifecycle contracts in:",
        ATTACK_PATH,
        DEFENSE_PATH,
        E2_PATH,
        ENEMY_DEFENSE_PATH,
    )


if __name__ == "__main__":
    main()
