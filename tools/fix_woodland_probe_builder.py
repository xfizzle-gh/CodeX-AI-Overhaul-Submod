from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TARGET = ROOT / "tools/apply_woodland_support_probe.py"
SELF = ROOT / "tools/fix_woodland_probe_builder.py"
WORKFLOW = ROOT / ".github/workflows/fix-woodland-probe-builder.yml"


def replace_once(source: str, old: str, new: str, label: str) -> str:
    count = source.count(old)
    if count != 1:
        raise RuntimeError(f"{label}: expected one match, found {count}")
    return source.replace(old, new, 1)


source = TARGET.read_text(encoding="utf-8")
old_function = '''def selector(tag: str, *, inside_gamezone: bool = True) -> str:
    zone_part = ''' + "'''" + '''\n\t\t\t\t\t\t\t\t\t{zone\n\t\t\t\t\t\t\t\t\t\t{zone "gamezone"}\n\t\t\t\t\t\t\t\t\t}''' + "'''" + ''' if inside_gamezone else ""
    return f''' + "'''" + '''\t\t\t\t\t\t{{selector
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
\t\t\t\t\t\t}}''' + "'''" + '''
'''
new_function = '''def selector(
    tag: str,
    *,
    inside_gamezone: bool = True,
    exclude_hidden: bool = True,
) -> str:
    zone_part = ''' + "'''" + '''\n\t\t\t\t\t\t\t\t\t{zone\n\t\t\t\t\t\t\t\t\t\t{zone "gamezone"}\n\t\t\t\t\t\t\t\t\t}''' + "'''" + ''' if inside_gamezone else ""
    hidden_part = ''' + "'''" + '''\n\t\t\t\t\t\t\t\t\t{tag\n\t\t\t\t\t\t\t\t\t\t{tag hidden}\n\t\t\t\t\t\t\t\t\t}''' + "'''" + ''' if exclude_hidden else ""
    return f''' + "'''" + '''\t\t\t\t\t\t{{selector
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
\t\t\t\t\t\t}}''' + "'''" + '''
'''
source = replace_once(source, old_function, new_function, "selector helper")
source = replace_once(
    source,
    'fresh_selector = selector("allied_wave_fresh")\n',
    'clone_selector = selector("allied_support_probe_source", exclude_hidden=False)\nfresh_selector = selector("allied_wave_fresh")\n',
    "clone selector declaration",
)
source = replace_once(
    source,
    "{selector('allied_support_probe_source')}\n\\t\\t\\t\\t\\t\\t{{tag_add allied_wave_fresh}}",
    "{clone_selector}\n\\t\\t\\t\\t\\t\\t{{tag_add allied_wave_fresh}}",
    "clone promotion selector",
)
source = replace_once(
    source,
    '\\t\\t\\t\\t\\t\\t{{tag_add _ai_defender}}\n\\t\\t\\t\\t\\t\\t{{tag_remove allied_support_probe_source}}',
    '\\t\\t\\t\\t\\t\\t{{tag_add _ai_defender}}\n\\t\\t\\t\\t\\t\\t{{tag_remove hidden}}\n\\t\\t\\t\\t\\t\\t{{tag_remove allied_support_probe_source}}',
    "explicit hidden removal",
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
source = source.replace(
    '"resource/map/multi/dcg_[cwa71]_woodland/campaign_capture_the_flag.mi"',
    '"resource/map/multi/dcg_[[]cwa71]_woodland/campaign_capture_the_flag.mi"',
)
TARGET.write_text(source, encoding="utf-8")
SELF.unlink()
WORKFLOW.unlink()
