from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SUPPORT = ROOT / "resource/map/multi/allied_support_ownership_probe.inc"
TEST = ROOT / "tests/test_woodland_support_probe.py"


def replace_once(text: str, old: str, new: str, label: str) -> str:
    count = text.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected exactly one match, found {count}")
    return text.replace(old, new, 1)


def matching_block_end(text: str, block_start: int) -> int:
    depth = 0
    entered = False
    for index in range(block_start, len(text)):
        char = text[index]
        if char == "{":
            depth += 1
            entered = True
        elif char == "}":
            depth -= 1
            if entered and depth == 0:
                return index + 1
    raise RuntimeError("unterminated MI action block")


support = SUPPORT.read_text(encoding="utf-8")

support = replace_once(
    support,
    "; CWA Dynamic Conquest ownership proof for the engine-created DefenderBot.\n"
    "; This is intentionally one-shot and uses a 60-second post-preparation delay.\n"
    "; Do not generalize into recurring support waves until ownership is proven live.\n",
    "; CWA Dynamic Conquest one-minute reinforcement test for the engine-created DefenderBot.\n"
    "; Each wave waits 60 seconds, clones five defenders at the map-local support entry,\n"
    "; assigns them to DefenderBot, and advances them toward the existing fpc1 route.\n",
    "support header",
)

support = replace_once(
    support,
    '{"allied_support/probe_cwa_ownership"',
    '{"allied_support/test_cwa_one_minute_waves"',
    "trigger name",
)

cleanup_tag = "{tag_remove allied_wave_fresh}"
tag_index = support.rfind(cleanup_tag)
if tag_index < 0:
    raise RuntimeError("final allied_wave_fresh cleanup tag not found")
block_start = support.rfind('{"entity_state"', 0, tag_index)
if block_start < 0:
    raise RuntimeError("final cleanup entity_state block not found")
block_end = matching_block_end(support, block_start)
line_end = support.find("\n", block_end)
if line_end < 0:
    line_end = block_end
else:
    line_end += 1

recurrence = (
    '\t\t\t\t; Test-only recurrence. The trigger opening delay controls the 60-second cadence.\n'
    '\t\t\t\t{"trigger"\n'
    '\t\t\t\t\t{name "allied_support/test_cwa_one_minute_waves"}\n'
    '\t\t\t\t}\n'
)
support = support[:line_end] + recurrence + support[line_end:]

required_support_markers = (
    '{time 60}',
    '{target_waypoint "allied_support_entry"}',
    '{tag fpc1}',
    '{control AI}',
    '{tag_add _def}',
    '{tag_add _ai_defender}',
    '{name "allied_support/test_cwa_one_minute_waves"}',
)
for marker in required_support_markers:
    if marker not in support:
        raise RuntimeError(f"required support marker missing: {marker}")

for forbidden in (
    '{control user}',
    '{tag_add _bot}',
    'probe_cwa_ownership',
    'probe_woodland_ownership',
):
    if forbidden in support:
        raise RuntimeError(f"forbidden support marker remains: {forbidden}")

if support.count('{"trigger"') != 1:
    raise RuntimeError("expected exactly one self-retrigger action")
if support.count("{") != support.count("}"):
    raise RuntimeError("support include braces are unbalanced")
if support.count("(") != support.count(")"):
    raise RuntimeError("support include parentheses are unbalanced")

SUPPORT.write_text(support, encoding="utf-8")

test = TEST.read_text(encoding="utf-8")
test = replace_once(
    test,
    "    def test_probe_remains_hard_gated_and_one_shot(self) -> None:\n",
    "    def test_support_test_is_hard_gated_and_repeats_every_minute(self) -> None:\n",
    "test name",
)
test = replace_once(
    test,
    '        self.assertEqual(self.probe.count(\'{"placement"\'), 1)\n'
    '        self.assertNotIn(\'{"loop"\', self.probe)\n'
    '        self.assertIn("probe_cwa_ownership", self.probe)\n'
    '        self.assertNotIn("probe_woodland_ownership", self.probe)\n',
    '        self.assertEqual(self.probe.count(\'{"placement"\'), 1)\n'
    '        self.assertNotIn(\'{"loop"\', self.probe)\n'
    '        self.assertEqual(self.probe.count(\'{"trigger"\'), 1)\n'
    '        self.assertIn(\'{name "allied_support/test_cwa_one_minute_waves"}\', self.probe)\n'
    '        self.assertNotIn("probe_cwa_ownership", self.probe)\n'
    '        self.assertNotIn("probe_woodland_ownership", self.probe)\n',
    "recurrence assertions",
)

test = replace_once(
    test,
    '        self.assertIn(\'{amount 5}\', self.probe)\n',
    '        self.assertIn(\'{amount 5}\', self.probe)\n'
    '        self.assertIn(\'{target_waypoint "allied_support_entry"}\', self.probe)\n'
    '        self.assertIn(\'{tag fpc1}\', self.probe)\n',
    "waypoint assertions",
)

TEST.write_text(test, encoding="utf-8")
