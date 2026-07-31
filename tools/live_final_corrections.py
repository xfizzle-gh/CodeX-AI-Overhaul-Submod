from __future__ import annotations

from pathlib import Path


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f'{label}: expected 1, found {count}')
    return text.replace(old, new, 1)


def normalize_once(text: str, old: str, new: str, label: str) -> str:
    candidates = [(old, new)]
    alternate_old = old.replace(
        'count("{tag_add support_e2_pax}")',
        "count('{tag_add support_e2_pax}')",
    )
    alternate_new = new.replace(
        'count("{tag_add support_e2_pax}")',
        "count('{tag_add support_e2_pax}')",
    )
    if alternate_old != old:
        candidates.append((alternate_old, alternate_new))

    old_hits = [(candidate_old, candidate_new) for candidate_old, candidate_new in candidates if text.count(candidate_old) == 1]
    if len(old_hits) == 1:
        candidate_old, candidate_new = old_hits[0]
        return text.replace(candidate_old, candidate_new, 1)

    normalized_hits = sum(text.count(candidate_new) for _, candidate_new in candidates)
    if not old_hits and normalized_hits == 1:
        return text

    old_count = sum(text.count(candidate_old) for candidate_old, _ in candidates)
    raise RuntimeError(
        f'{label}: expected one old or one normalized form, found old={old_count}, new={normalized_hits}'
    )


def strip_trailing_whitespace(text: str) -> str:
    had_final_newline = text.endswith('\n')
    cleaned = '\n'.join(line.rstrip() for line in text.splitlines())
    return cleaned + ('\n' if had_final_newline else '')


waves_path = Path('resource/map/multi/attack_support_waves.inc')
templates_path = Path('resource/map/multi/faction_support_templates.inc')
e2_tests_path = Path('tests/test_e2_airmobile.py')
motor_tests_path = Path('tests/test_attack_support_slot_proof.py')

waves = waves_path.read_text(encoding='utf-8')
templates = templates_path.read_text(encoding='utf-8')
e2_tests = e2_tests_path.read_text(encoding='utf-8')
motor_tests = motor_tests_path.read_text(encoding='utf-8')

decorated_marker = '''{selector
\t\t\t\t\t\t{source advanced}
\t\t\t\t\t\t{group
\t\t\t\t\t\t\t{select {tag {tag support_e2_marker_tpl}}}
\t\t\t\t\t\t\t{include {tag {tag hidden}}}
\t\t\t\t\t\t}
\t\t\t\t\t\t{amount 1}
\t\t\t\t\t}'''
bare_marker = '{selector {ignore_captured_by_user 0} {tag support_e2_marker_tpl} {amount 1}}'
waves = replace_once(waves, decorated_marker, bare_marker, 'replace decorated marker selector')

templates = replace_once(
    templates,
    '{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_marker_tpl" "hidden" 0xc207}',
    '{Tags "support_e2_marker_tpl" 0xc207}',
    'isolate marker tag and remove hidden state',
)
templates = templates.replace(
    '; A dedicated, unlinked hidden body used only as the near_to anchor for helicopter',
    '; A dedicated, unlinked parked body used only as the near_to anchor for helicopter',
    1,
)

e2_tests = normalize_once(
    e2_tests,
    '        self.assertEqual(self.live.count("{tag_add support_e2_pax}"), 3)',
    '        self.assertEqual(self.live.count("{tag_add support_e2_pax}"), 4)',
    'update completed delivery count',
)
e2_tests = normalize_once(
    e2_tests,
    '        self.assertEqual(self.waves.count("{tag_add support_e2_pax}"), 3)',
    '        self.assertEqual(self.waves.count("{tag_add support_e2_pax}"), 4)',
    'update legacy completed delivery count',
)
helo_anchor = '''        self.assertIn("{tag_remove support_e2_helo_pax}", helo)

    def test_the_pax_tag_carries_the_literal_1_to_16_switch_and_fails_closed(self) -> None:
'''
takeover_assert = '''        self.assertIn("{tag_remove support_e2_helo_pax}", helo)

        para_start = self.live.index('{"attack_support/e2_para_takeover"')
        para_end = self.live.index('; ===== THE PARACHUTE LINKERS', para_start)
        para = self.live[para_start:para_end]
        self.assertEqual(para.count("{tag_add support_e2_pax}"), 1)
        self.assertIn("{tag paratrooper_need_orders}", para)
        self.assertIn("{state {state linked}}", para)
        self.assertIn("{state {state inactive}}", para)
        self.assertIn("{state {state dead}}", para)
        self.assertIn('(\"e2_own_pax\")', para)
        self.assertIn('(\"e2_order_team\")', para)

    def test_the_pax_tag_carries_the_literal_1_to_16_switch_and_fails_closed(self) -> None:
'''
e2_tests = replace_once(e2_tests, helo_anchor, takeover_assert, 'pin paratrooper delivery path')

redundant_takeover_test = '''

    def test_landed_paratroopers_are_owned_and_ordered_immediately(self) -> None:
        takeover = mi_block(self.waves, '{"attack_support/e2_para_takeover"')
        condition, actions = takeover.split('{actions', 1)
        self.assertIn('{autoreset}', takeover)
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 40}', condition)
        self.assertIn('{tag paratrooper_need_orders}', condition)
        self.assertIn('{state {state linked}}', condition)
        self.assertIn('{tag_add support_e2_pax}', actions)
        self.assertIn('(\"e2_own_pax\")', actions)
        self.assertIn('(\"e2_order_team\")', actions)
'''
e2_tests = replace_once(
    e2_tests,
    redundant_takeover_test,
    '',
    'remove redundant standalone takeover test',
)

old_motor = '''                # The reference tag is the same tag the advance order targeted.
                advance = body.index("{action advance}")
                self.assertNotIn("{action move}", body[:emit])
                target = block_at(body, body.index("{target", advance))
'''
new_motor = '''                # The reference tag is the same tag the advance order targeted.
                emit = body.index('{"emit"')
                advance = body.index("{action advance}")
                self.assertLess(advance, emit)
                self.assertNotIn("{action move}", body[:emit])
                target = block_at(body, body.index("{target", advance))
'''
motor_tests = replace_once(motor_tests, old_motor, new_motor, 'define motor emit boundary')

waves = strip_trailing_whitespace(waves)

waves_path.write_text(waves, encoding='utf-8')
templates_path.write_text(templates, encoding='utf-8')
e2_tests_path.write_text(e2_tests, encoding='utf-8')
motor_tests_path.write_text(motor_tests, encoding='utf-8')
