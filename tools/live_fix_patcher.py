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
