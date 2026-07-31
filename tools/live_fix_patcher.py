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
]
for old, new, label in correction_replacements:
    count = correction.count(old)
    if count != 1:
        raise RuntimeError(f'{label}: expected one fragment, found {count}')
    correction = correction.replace(old, new, 1)

CORRECTION.write_text(correction, encoding='utf-8')
