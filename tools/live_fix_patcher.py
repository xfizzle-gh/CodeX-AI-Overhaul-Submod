from __future__ import annotations

from pathlib import Path


SCRIPT = Path('/tmp/apply_live_air_motor_fixes.py')
text = SCRIPT.read_text(encoding='utf-8')

replacements = [
    (
        '        "attack_support/motor_cleanup",\n    ),',
        '        "attack_support/motor_cleanup",\n        "as_finish_motor",\n    ),',
    ),
    (
        '        "defense_support/motor_cleanup",\n    ),',
        '        "defense_support/motor_cleanup",\n        "ds_finish_motor",\n    ),',
    ),
    (
        '        "enemy_attack/motor_cleanup",\n    ),',
        '        "enemy_attack/motor_cleanup",\n        "ea_finish_motor",\n    ),',
    ),
    (
        '        "enemy_defense/motor_cleanup",\n    ),',
        '        "enemy_defense/motor_cleanup",\n        "ed_finish_motor",\n    ),',
    ),
    (
        'for path, (hull, objective, pax, cleanup_trigger) in motor_files.items():',
        'for path, (hull, objective, pax, cleanup_trigger, finisher_name) in motor_files.items():',
    ),
]
for old, new in replacements:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f'expected one script fragment, found {count}: {old!r}')
    text = text.replace(old, new, 1)

start_token = '    # Scope the foot order to bodies that the emit has actually unlinked.'
end_token = '    motor_texts[path] = text'
start = text.index(start_token)
end = text.index(end_token, start)

replacement = '''    # Scope the foot order to bodies that the emit has actually unlinked. Work inside
    # the named finisher because each engine uses different prose before this action.
    finisher_start = text.index('(define "' + finisher_name + '"')
    next_define = text.find('(define "', finisher_start + 10)
    finisher_end = len(text) if next_define < 0 else next_define
    finisher = text[finisher_start:finisher_end]
    old_pax_selector = '{selector {ignore_captured_by_user 0} {tag ' + pax + '}}'
    new_pax_selector = """{selector
\t\t\t\t\t\t{source advanced}
\t\t\t\t\t\t{ignore_captured_by_user 0}
\t\t\t\t\t\t{group
\t\t\t\t\t\t\t{select {tag {tag """ + pax + """}}}
\t\t\t\t\t\t\t{exclude {state {state linked}} {state {state inactive}} {state {state dead}}}
\t\t\t\t\t\t}
\t\t\t\t\t}"""
    finisher = replace_exact(
        finisher,
        old_pax_selector,
        new_pax_selector,
        1,
        f'{path}: order only emitted passengers',
    )
    text = text[:finisher_start] + finisher + text[finisher_end:]

'''
text = text[:start] + replacement + text[end:]
SCRIPT.write_text(text, encoding='utf-8')

# The final correction script was intentionally strict while its test edits were being
# isolated. Patch its known old-form assumptions before it runs against the repaired tree.
CORRECTION = Path('tools/live_final_corrections.py')
correction = CORRECTION.read_text(encoding='utf-8')

old_match_gate = '''    matches = list(pattern.finditer(text))
    if len(matches) != 1:
        raise RuntimeError(
            'update legacy completed delivery count: '
            f'expected exactly one structural assertion, found {len(matches)}'
        )
    match = matches[0]
'''
new_match_gate = '''    matches = list(pattern.finditer(text))
    if not matches:
        return text
    if len(matches) > 1:
        raise RuntimeError(
            'update legacy completed delivery count: '
            f'expected at most one structural assertion, found {len(matches)}'
        )
    match = matches[0]
'''

post_repair_anchor = '''e2_tests = normalize_legacy_waves_count(e2_tests)

helo_anchor ='''
post_repair_replacement = '''e2_tests = normalize_legacy_waves_count(e2_tests)
e2_tests = replace_once(
    e2_tests,
    '        self.assertIn("{distance 600}", self.para)',
    '        self.assertNotIn("{distance 600}", self.para)',
    'retire the obsolete 600-unit para radius assertion',
)

helo_anchor ='''

motor_test_anchor = '''motor_tests = replace_once(motor_tests, old_motor, new_motor, 'define motor emit boundary')

waves = strip_trailing_whitespace(waves)
'''
motor_test_replacement = '''motor_tests = replace_once(motor_tests, old_motor, new_motor, 'define motor emit boundary')

old_motor_prefixes = ''' + '"""' + '''        prefixes = {
            "attack_support_waves": ("attack_support", "as_motor_band"),
            "defense_support_waves": ("defense_support", "ds_motor_band"),
            "enemy_attack_support": ("enemy_attack", "ea_motor_band"),
            "enemy_defense_support": ("enemy_defense", "ed_motor_band"),
        }
        for engine, (pfx, band_define) in sorted(prefixes.items()):''' + '"""' + '''
new_motor_prefixes = ''' + '"""' + '''        prefixes = {
            "attack_support_waves": ("attack_support", "as_motor_band", "attack_support_motor_flag"),
            "defense_support_waves": ("defense_support", "ds_motor_band", "def_sup_motor_flag"),
            "enemy_attack_support": ("enemy_attack", "ea_motor_band", "ea_motor_flag"),
            "enemy_defense_support": ("enemy_defense", "ed_motor_band", "enemy_def_motor_flag"),
        }
        for engine, (pfx, band_define, objective) in sorted(prefixes.items()):''' + '"""' + '''

# Limit the telemetry edits to the drive-phase method. The generic flag assertion also
# appears in a different legacy test and must not inherit this method's objective variable.
drive_start = motor_tests.index('    def test_the_drive_phase_is_instrumented_in_every_engine')
drive_end = motor_tests.index('\\n    def ', drive_start + 8)
drive_test = motor_tests[drive_start:drive_end]
drive_test = replace_once(
    drive_test,
    old_motor_prefixes,
    new_motor_prefixes,
    'bind each telemetry probe to its dedicated objective',
)
drive_test = replace_once(
    drive_test,
    '                    probe.count("{near_to {ignore_captured_by_user 0} {tag flag}}"), 3',
    '                    probe.count("{near_to {ignore_captured_by_user 0} {tag %s}}" % objective), 3',
    'assert dedicated objective in each motor band probe',
)
motor_tests = motor_tests[:drive_start] + drive_test + motor_tests[drive_end:]

motor_tests = replace_once(
    motor_tests,
    '                self.assertEqual(code.count(flag), body.count(flag), flag)',
    '                self.assertEqual(code.count(flag), body.count(flag) + probe.count(flag), flag)',
    'count the dedicated objective in finisher and probe',
)

waves = strip_trailing_whitespace(waves)
'''

correction_replacements = [
    (
        old_match_gate,
        new_match_gate,
        'make absent legacy count assertion non-blocking',
    ),
    (
        "para_end = self.live.index('; ===== THE PARACHUTE LINKERS', para_start)",
        "para_end = self.live.index('{\"attack_support/e2_paradrop_link_0\"', para_start)",
        'use the real first paradrop-link trigger as takeover boundary',
    ),
    (
        post_repair_anchor,
        post_repair_replacement,
        'apply the obsolete radius test correction after base repair',
    ),
    (
        motor_test_anchor,
        motor_test_replacement,
        'align motor telemetry assertions with dedicated objectives',
    ),
]
for old, new, label in correction_replacements:
    count = correction.count(old)
    if count != 1:
        raise RuntimeError(f'{label}: expected one fragment, found {count}')
    correction = correction.replace(old, new, 1)

CORRECTION.write_text(correction, encoding='utf-8')
