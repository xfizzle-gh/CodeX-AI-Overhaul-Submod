from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / "tools/apply_woodland_support_probe.py"
SELF = ROOT / "tools/fix_woodland_probe_builder.py"
WORKFLOW = ROOT / ".github/workflows/fix-woodland-probe-builder.yml"
ERROR_LOG = ROOT / "docs/woodland_probe_builder_error.txt"


def replace_once(source: str, old: str, new: str, label: str) -> str:
    count = source.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    return source.replace(old, new, 1)


source = TARGET.read_text(encoding="utf-8")
helper_start = source.index("def selector(")
helper_end = source.index("\n\nfresh_selector =", helper_start)
new_helper = r"""def selector(
    tag: str,
    *,
    inside_gamezone: bool = True,
    exclude_hidden: bool = True,
) -> str:
    zone_part = '''\n\t\t\t\t\t\t\t\t\t{zone\n\t\t\t\t\t\t\t\t\t\t{zone "gamezone"}\n\t\t\t\t\t\t\t\t\t}''' if inside_gamezone else ""
    hidden_part = '''\n\t\t\t\t\t\t\t\t\t{tag\n\t\t\t\t\t\t\t\t\t\t{tag hidden}\n\t\t\t\t\t\t\t\t\t}''' if exclude_hidden else ""
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
\t\t\t\t\t\t\t\t\t}}{hidden_part}
\t\t\t\t\t\t\t\t}}
\t\t\t\t\t\t\t}}
\t\t\t\t\t\t}}'''
"""
source = source[:helper_start] + new_helper + source[helper_end:]
source = replace_once(
    source,
    'fresh_selector = selector("allied_wave_fresh")\n',
    'clone_selector = selector("allied_support_probe_source", exclude_hidden=False)\nfresh_selector = selector("allied_wave_fresh")\n',
    "clone selector declaration",
)
source = replace_once(
    source,
    "{selector('allied_support_probe_source')}",
    "{clone_selector}",
    "clone promotion selector",
)
hidden_marker = r'\t\t\t\t\t\t{{tag_add _ai_defender}}'
hidden_index = source.index(hidden_marker) + len(hidden_marker)
source = (
    source[:hidden_index]
    + "\n"
    + r'\t\t\t\t\t\t{{tag_remove hidden}}'
    + source[hidden_index:]
)
source = replace_once(
    source,
    'for path in cwa_root.glob("dcg_[cwa71]_*/campaign_capture_the_flag.mi"):\n            if path != WOODLAND and "allied_support_ownership_probe.inc" in path.read_text(encoding="utf-8"):',
    'for path in cwa_root.rglob("campaign_capture_the_flag.mi"):\n            if "dcg_[cwa71]_" not in path.parent.name:\n                continue\n            if path != WOODLAND and "allied_support_ownership_probe.inc" in path.read_text(encoding="utf-8"):',
    "literal CWA path scan",
)
source = replace_once(
    source,
    '''    def test_probe_is_one_shot_not_the_final_wave_loop(self) -> None:
''',
    '''    def test_mission_and_probe_delimiters_are_balanced(self) -> None:
        for text in (self.mission, self.probe):
            self.assertEqual(text.count("{"), text.count("}"))
            self.assertEqual(text.count("("), text.count(")"))

    def test_clones_are_unhidden_before_runtime_selection(self) -> None:
        self.assertIn("{tag_remove hidden}", self.probe)
        promote = self.probe.index("{tag_add allied_wave_fresh}")
        unhide = self.probe.index("{tag_remove hidden}", promote)
        ownership = self.probe.index("{operation set}", unhide)
        self.assertLess(promote, unhide)
        self.assertLess(unhide, ownership)

    def test_probe_is_one_shot_not_the_final_wave_loop(self) -> None:
''',
    "delimiter and unhide tests",
)
# Escape the literal square bracket only in GitHub Actions path filters. Do not
# alter the real filesystem path used by the Python builder or tests.
guard_start = source.index("GUARD.write_text")
head, guard_tail = source[:guard_start], source[guard_start:]
guard_tail = guard_tail.replace(
    '"resource/map/multi/dcg_[cwa71]_woodland/campaign_capture_the_flag.mi"',
    '"resource/map/multi/dcg_[[]cwa71]_woodland/campaign_capture_the_flag.mi"',
)
source = head + guard_tail
TARGET.write_text(source, encoding="utf-8")
ERROR_LOG.unlink(missing_ok=True)
SELF.unlink()
WORKFLOW.unlink()
