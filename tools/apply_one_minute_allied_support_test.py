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

end_marker = (
    "\t\t\t\t\t{tag_remove allied_wave_fresh}\n"
    "\t\t\t\t}\n"
    "\t\t\t}\n"
    "\t\t}\n"
)
if support.count(end_marker) != 1:
    raise RuntimeError(
        f"support recurrence anchor: expected one match, found {support.count(end_marker)}"
    )
recurrence = (
    "\t\t\t\t\t{tag_remove allied_wave_fresh}\n"
    "\t\t\t\t}\n"
    "\t\t\t\t; Test-only recurrence. The trigger's opening 60-second delay controls cadence.\n"
    "\t\t\t\t{\"trigger\"\n"
    "\t\t\t\t\t{name \"allied_support/test_cwa_one_minute_waves\"}\n"
    "\t\t\t\t}\n"
    "\t\t\t}\n"
    "\t\t}\n"
)
support = support.replace(end_marker, recurrence, 1)

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

needle = '        self.assertIn(\'{amount 5}\', self.probe)\n'
replacement = (
    '        self.assertIn(\'{amount 5}\', self.probe)\n'
    '        self.assertIn(\'{target_waypoint "allied_support_entry"}\', self.probe)\n'
    '        self.assertIn(\'{tag fpc1}\', self.probe)\n'
)
test = replace_once(test, needle, replacement, "waypoint assertions")

if test.count("{") != test.count("}"):
    raise RuntimeError("test source brace characters are unbalanced")

TEST.write_text(test, encoding="utf-8")
