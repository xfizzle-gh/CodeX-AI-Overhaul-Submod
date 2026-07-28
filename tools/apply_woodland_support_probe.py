from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
WOODLAND = ROOT / "resource/map/multi/dcg_[cwa71]_woodland/campaign_capture_the_flag.mi"
PROBE = ROOT / "resource/map/multi/allied_support_ownership_probe.inc"
TEST = ROOT / "tests/test_woodland_support_probe.py"
GUARD = ROOT / ".github/workflows/woodland-support-probe-guard.yml"
SELF = ROOT / "tools/apply_woodland_support_probe.py"
WORKFLOW = ROOT / ".github/workflows/apply-woodland-support-probe.yml"
AUDITS = [
    ROOT / "docs/cwa_support_probe_audit.txt",
    ROOT / "docs/cwa_support_probe_details.txt",
    ROOT / "docs/mi_stage_selection_audit.txt",
]


def replace_once(source: str, old: str, new: str, label: str) -> str:
    count = source.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    return source.replace(old, new, 1)


mission = WOODLAND.read_text(encoding="utf-8")
mission = replace_once(
    mission,
    '\t\t\t(include "../dcg_script.inc")\n',
    '\t\t\t(include "../allied_support_ownership_probe.inc")\n\t\t\t(include "../dcg_script.inc")\n',
    "woodland trigger include",
)
mission = replace_once(
    mission,
    '\t\t{waypoints\n\t\t\t{"0"\n',
    '\t\t{waypoints\n\t\t\t{"allied_support_entry"\n\t\t\t\t{position -6250.83 -572.56 53.84}\n\t\t\t\t{radius 150}\n\t\t\t}\n\t\t\t{"0"\n',
    "woodland support waypoint",
)
WOODLAND.write_text(mission, encoding="utf-8")


def selector(tag: str, *, inside_gamezone: bool = True) -> str:
    zone_part = '''\n\t\t\t\t\t\t\t\t\t{zone\n\t\t\t\t\t\t\t\t\t\t{zone "gamezone"}\n\t\t\t\t\t\t\t\t\t}''' if inside_gamezone else ""
    return f'''\t\t\t\t\t\t{{selector
\t\t\t\t\t\t\t{{source advanced}}
\t\t\t\t\t\t\t{{group
\t\t\t\t\t\t\t\t{{select
\t\t\t\t\t\t\t\t\t{{tag
\t\t\t\t\t\t\t\t\t\t{{tag {tag}}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t{{include
\t\t\t\t\t\t\t\t\t{{prop
\t\t\t\t\t\t\t\t\t\t{{prop human}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t\t{{state
\t\t\t\t\t\t\t\t\t\t{{state operatable}}
\t\t\t\t\t\t\t\t\t}}{zone_part}
\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t{{exclude
\t\t\t\t\t\t\t\t\t{{state
\t\t\t\t\t\t\t\t\t\t{{state dead}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t\t{{state
\t\t\t\t\t\t\t\t\t\t{{state inactive}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t\t{{state
\t\t\t\t\t\t\t\t\t\t{{state linked}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t\t{{state
\t\t\t\t\t\t\t\t\t\t{{state user_control}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t\t{{tag
\t\t\t\t\t\t\t\t\t\t{{tag hidden}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t}}
\t\t\t\t\t\t}}'''


fresh_selector = selector("allied_wave_fresh")
case_blocks: list[str] = []
for player_id in range(1, 17):
    case_blocks.append(f'''\t\t\t\t{{"case"
\t\t\t\t\t{{condition
\t\t\t\t\t\t{{type cmp_i}}
\t\t\t\t\t\t{{var "id_defenderbot$"}}
\t\t\t\t\t\t{{op "=="}}
\t\t\t\t\t\t{{value {player_id}}}
\t\t\t\t\t}}
\t\t\t\t\t{{"player"
{fresh_selector}
\t\t\t\t\t\t{{operation set}}
\t\t\t\t\t\t{{player "{player_id}"}}
\t\t\t\t\t}}
\t\t\t\t\t{{"entity_state"
{fresh_selector}
\t\t\t\t\t\t{{tag_add allied_support_probe_owner_{player_id}}}
\t\t\t\t\t}}
\t\t\t\t}}''')

probe = f'''; Woodland-only ownership proof for engine-created DefenderBot.
; This is intentionally one-shot and uses a 60-second post-preparation delay.
; Do not generalize into recurring support waves until ownership is proven live.

\t\t\t{{"allied_support/probe_woodland_ownership"
\t\t\t\t{{condition
\t\t\t\t\t{{expression "1 & 2 & 3"}}
\t\t\t\t\t{{terms
\t\t\t\t\t\t{{"1.cmp_i"
\t\t\t\t\t\t\t{{var "user_is_defender$"}}
\t\t\t\t\t\t\t{{op "=="}}
\t\t\t\t\t\t\t{{value 1}}
\t\t\t\t\t\t}}
\t\t\t\t\t\t{{"2.cmp_i"
\t\t\t\t\t\t\t{{var "id_defenderbot$"}}
\t\t\t\t\t\t\t{{op ">"}}
\t\t\t\t\t\t\t{{value 0}}
\t\t\t\t\t\t}}
\t\t\t\t\t\t{{"3.cmp_i"
\t\t\t\t\t\t\t{{var "prep_inform$"}}
\t\t\t\t\t\t\t{{op "=="}}
\t\t\t\t\t\t\t{{value 1}}
\t\t\t\t\t\t}}
\t\t\t\t\t}}
\t\t\t\t}}
\t\t\t\t{{actions
\t\t\t\t\t{{"delay"
\t\t\t\t\t\t{{time 60}}
\t\t\t\t\t}}
\t\t\t\t\t; Mark exactly five hidden cmp_def infantry templates. The clone inherits this
\t\t\t\t\t; temporary marker; it is removed from the off-map sources immediately after cloning.
\t\t\t\t\t{{"entity_state"
\t\t\t\t\t\t{{selector
\t\t\t\t\t\t\t{{source advanced}}
\t\t\t\t\t\t\t{{group
\t\t\t\t\t\t\t\t{{select
\t\t\t\t\t\t\t\t\t{{tag
\t\t\t\t\t\t\t\t\t\t{{tag cmp_def}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t{{include
\t\t\t\t\t\t\t\t\t{{prop
\t\t\t\t\t\t\t\t\t\t{{prop human}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t\t{{tag
\t\t\t\t\t\t\t\t\t\t{{tag hidden}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t{{exclude
\t\t\t\t\t\t\t\t\t{{zone
\t\t\t\t\t\t\t\t\t\t{{zone "gamezone"}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t\t{{state
\t\t\t\t\t\t\t\t\t\t{{state dead}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t\t{{state
\t\t\t\t\t\t\t\t\t\t{{state inactive}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t\t{{state
\t\t\t\t\t\t\t\t\t\t{{state linked}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t\t{{state
\t\t\t\t\t\t\t\t\t\t{{state user_control}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t{{amount 5}}
\t\t\t\t\t\t}}
\t\t\t\t\t\t{{tag_add allied_support_probe_source}}
\t\t\t\t\t}}
\t\t\t\t\t{{"delay" {{time 0.1}}}}
\t\t\t\t\t{{"placement"
\t\t\t\t\t\t{{selector
\t\t\t\t\t\t\t{{source advanced}}
\t\t\t\t\t\t\t{{group
\t\t\t\t\t\t\t\t{{select
\t\t\t\t\t\t\t\t\t{{tag
\t\t\t\t\t\t\t\t\t\t{{tag allied_support_probe_source}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t{{amount 5}}
\t\t\t\t\t\t}}
\t\t\t\t\t\t{{target_waypoint "allied_support_entry"}}
\t\t\t\t\t\t{{clone}}
\t\t\t\t\t}}
\t\t\t\t\t{{"delay" {{time 0.5}}}}
\t\t\t\t\t; Remove the temporary marker from hidden source templates only.
\t\t\t\t\t{{"entity_state"
\t\t\t\t\t\t{{selector
\t\t\t\t\t\t\t{{source advanced}}
\t\t\t\t\t\t\t{{group
\t\t\t\t\t\t\t\t{{select
\t\t\t\t\t\t\t\t\t{{tag
\t\t\t\t\t\t\t\t\t\t{{tag allied_support_probe_source}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t{{include
\t\t\t\t\t\t\t\t\t{{tag
\t\t\t\t\t\t\t\t\t\t{{tag hidden}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t{{exclude
\t\t\t\t\t\t\t\t\t{{zone
\t\t\t\t\t\t\t\t\t\t{{zone "gamezone"}}
\t\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t}}
\t\t\t\t\t\t}}
\t\t\t\t\t\t{{tag_remove allied_support_probe_source}}
\t\t\t\t\t}}
\t\t\t\t\t; The in-game clones retain the inherited marker. Promote only those clones.
\t\t\t\t\t{{"entity_state"
{selector('allied_support_probe_source')}
\t\t\t\t\t\t{{tag_add allied_wave_fresh}}
\t\t\t\t\t\t{{tag_add allied_support_probe}}
\t\t\t\t\t\t{{tag_add _def}}
\t\t\t\t\t\t{{tag_add _ai_defender}}
\t\t\t\t\t\t{{tag_remove allied_support_probe_source}}
\t\t\t\t\t}}
\t\t\t\t\t{{"switch"
{chr(10).join(case_blocks)}
\t\t\t\t\t\t{{"default"
\t\t\t\t\t\t\t{{"entity_state"
{fresh_selector}
\t\t\t\t\t\t\t\t{{tag_add allied_support_probe_owner_unsupported}}
\t\t\t\t\t\t\t}}
\t\t\t\t\t\t}}
\t\t\t\t\t}}
\t\t\t\t\t{{"actor_state"
{fresh_selector}
\t\t\t\t\t\t{{control AI}}
\t\t\t\t\t\t{{ai
\t\t\t\t\t\t\t{{no_retreat on}}
\t\t\t\t\t\t}}
\t\t\t\t\t}}
\t\t\t\t\t{{"action"
{fresh_selector}
\t\t\t\t\t\t{{drop orders}}
\t\t\t\t\t\t{{action advance}}
\t\t\t\t\t\t{{target
\t\t\t\t\t\t\t{{tag fpc1}}
\t\t\t\t\t\t}}
\t\t\t\t\t}}
\t\t\t\t\t{{"entity_state"
{fresh_selector}
\t\t\t\t\t\t{{tag_remove allied_wave_fresh}}
\t\t\t\t\t}}
\t\t\t\t}}
\t\t\t}}
'''
PROBE.write_text(probe, encoding="utf-8")

TEST.write_text('''from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
WOODLAND = ROOT / "resource/map/multi/dcg_[cwa71]_woodland/campaign_capture_the_flag.mi"
PROBE = ROOT / "resource/map/multi/allied_support_ownership_probe.inc"


class WoodlandSupportOwnershipProbeTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.mission = WOODLAND.read_text(encoding="utf-8")
        cls.probe = PROBE.read_text(encoding="utf-8")

    def test_probe_is_woodland_only_and_loaded_before_shared_dcg_logic(self) -> None:
        self.assertEqual(self.mission.count("allied_support_ownership_probe.inc"), 1)
        self.assertLess(
            self.mission.index("allied_support_ownership_probe.inc"),
            self.mission.index("dcg_script.inc"),
        )
        cwa_root = ROOT / "resource/map/multi"
        other_hits = []
        for path in cwa_root.glob("dcg_[cwa71]_*/campaign_capture_the_flag.mi"):
            if path != WOODLAND and "allied_support_ownership_probe.inc" in path.read_text(encoding="utf-8"):
                other_hits.append(path)
        self.assertEqual(other_hits, [])

    def test_woodland_has_explicit_rear_entry_waypoint(self) -> None:
        self.assertIn('{"allied_support_entry"', self.mission)
        self.assertIn("{position -6250.83 -572.56 53.84}", self.mission)
        self.assertIn("{radius 150}", self.mission)

    def test_probe_is_hard_gated_to_human_defense_after_prep(self) -> None:
        self.assertIn('{var "user_is_defender$"}', self.probe)
        self.assertIn('{var "id_defenderbot$"}', self.probe)
        self.assertIn('{var "prep_inform$"}', self.probe)
        self.assertIn('{expression "1 & 2 & 3"}', self.probe)
        self.assertIn('{time 60}', self.probe)

    def test_probe_clones_exactly_five_hidden_cmp_def_infantry(self) -> None:
        self.assertIn('{tag cmp_def}', self.probe)
        self.assertIn('{tag hidden}', self.probe)
        self.assertIn('{amount 5}', self.probe)
        self.assertIn('{target_waypoint "allied_support_entry"}', self.probe)
        self.assertIn('{clone}', self.probe)
        self.assertIn('{tag_add allied_support_probe_source}', self.probe)
        self.assertIn('{tag_remove allied_support_probe_source}', self.probe)

    def test_probe_covers_every_real_woodland_player_slot(self) -> None:
        self.assertIn('{count 17}', self.mission)
        for player_id in range(1, 17):
            self.assertIn(f'{{value {player_id}}}', self.probe)
            self.assertIn(f'{{player "{player_id}"}}', self.probe)
            self.assertIn(f'{{tag_add allied_support_probe_owner_{player_id}}}', self.probe)
        self.assertIn("allied_support_probe_owner_unsupported", self.probe)

    def test_units_remain_ai_owned_and_ce_defender_compatible(self) -> None:
        self.assertIn('{operation set}', self.probe)
        self.assertIn('{control AI}', self.probe)
        self.assertNotIn('{control user}', self.probe)
        self.assertIn('{tag_add _def}', self.probe)
        self.assertIn('{tag_add _ai_defender}', self.probe)
        self.assertIn('{tag_add allied_support_probe}', self.probe)
        self.assertNotIn('{tag_add _bot}', self.probe)
        self.assertNotIn('{tag _bot}', self.probe)

    def test_probe_is_one_shot_not_the_final_wave_loop(self) -> None:
        self.assertEqual(self.probe.count('{"placement"'), 1)
        self.assertNotIn('4 * 60', self.probe)
        self.assertNotIn('8 * 60', self.probe)
        self.assertNotIn('{"trigger"', self.probe)
        self.assertNotIn('{"loop"', self.probe)


if __name__ == "__main__":
    unittest.main()
''', encoding="utf-8")

GUARD.write_text('''name: Woodland support ownership probe guard

on:
  pull_request:
    paths:
      - "resource/map/multi/allied_support_ownership_probe.inc"
      - "resource/map/multi/dcg_[cwa71]_woodland/campaign_capture_the_flag.mi"
      - "tests/test_woodland_support_probe.py"
      - ".github/workflows/woodland-support-probe-guard.yml"
  push:
    branches:
      - main
    paths:
      - "resource/map/multi/allied_support_ownership_probe.inc"
      - "resource/map/multi/dcg_[cwa71]_woodland/campaign_capture_the_flag.mi"
      - "tests/test_woodland_support_probe.py"
      - ".github/workflows/woodland-support-probe-guard.yml"

jobs:
  source-guard:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6
      - uses: actions/setup-python@v6
        with:
          python-version: "3.13"
      - run: python -m unittest tests/test_woodland_support_probe.py
''', encoding="utf-8")

for audit in AUDITS:
    audit.unlink(missing_ok=True)
SELF.unlink()
WORKFLOW.unlink()
