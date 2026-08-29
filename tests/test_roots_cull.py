"""Source-guard for the 2026-08-29 roots cull.

Pack 3651979039 maps still include three AIO paths. Those files must remain
as comment-only stubs. Live enemy/defense support and DCG AI wiring must stay.
Deleted morale / POW / cohesion / breed / map overlays must stay gone.
"""

from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

STUBS = [
    ROOT / "resource/map/multi/attack_support_waves.inc",
    ROOT / "resource/map/multi/attack_support_templates.inc",
    ROOT / "resource/map/multi/ce/ce_morale_helpers.inc",
]

LIVE_SUPPORT = [
    ROOT / "resource/map/multi/enemy_attack_support.inc",
    ROOT / "resource/map/multi/enemy_defense_support.inc",
    ROOT / "resource/map/multi/defense_support_waves.inc",
    ROOT / "resource/map/multi/enemy_defense_templates.inc",
    ROOT / "resource/map/multi/faction_support_templates.inc",
    ROOT / "resource/map/multi/support_mission_roll.inc",
    ROOT / "resource/map/multi/dcg_script.inc",
    ROOT / "resource/map/multi/dcg_vars.inc",
    ROOT / "resource/map/multi/dcg_functions.inc",
]

DELETED = [
    ROOT / "resource/map/multi/ce/ce_morale_classification_triggers.inc",
    ROOT / "resource/map/multi/ce/ce_morale_marker_apply_triggers.inc",
    ROOT / "resource/map/multi/ce/ce_morale_machine_triggers.inc",
    ROOT / "resource/map/multi/ce/ce_broken_behavior_triggers.inc",
    ROOT / "resource/map/multi/ce/ce_command_cohesion_triggers.inc",
    ROOT / "resource/map/multi/ce/ce_pow_camp_triggers.inc",
    ROOT / "resource/map/multi/ce/ce_pow_camp_manage_triggers.inc",
    ROOT / "resource/map/multi/ce/ce_pow_dmg_editor.inc",
    ROOT / "resource/map/multi/ce/morale_system.mod",
    ROOT / "resource/map/multi/ce/sniper_range.mod",
    ROOT / "resource/map/multi/ce/vet_range.mod",
    ROOT / "resource/set/breed",
    ROOT / "docs/pow_mirror_transfer.md",
    ROOT / "tests/test_ce_broken_behavior.py",
    ROOT / "tests/test_ce_pow_camp.py",
    ROOT / "tests/test_ce_pow_camp_manage.py",
    ROOT / "tests/test_ce_pow_oldboy_captive.py",
    ROOT / ".github/workflows/morale-breed-metadata-guard.yml",
]

# Includes that resolve in the base game / parent Code:X, not this submod.
BASE_GAME_INCLUDES = {
    "/map/nikral's trigger.mi",
    "/map/north_trigger.mi",
    "/map/common_sp_scripts.inc",
    "/map/ordos's trigger.inc",
    "resupply_vanilla.inc",
    "human_idle.inc",
    "tree.inc",
    "construction.inc",
    "seasons.inc",
    "button.inc",
    "tank-damage_ce.inc",
}

INCLUDE_RE = re.compile(r'^\s*(;?)\s*\(include\s+"([^"]+)"', re.MULTILINE)


def uncommented_includes(text: str) -> list[str]:
    out = []
    for m in INCLUDE_RE.finditer(text):
        if m.group(1) == ";":
            continue
        out.append(m.group(2))
    return out


def resolve_include(src: Path, inc: str) -> Path | None:
    if inc in BASE_GAME_INCLUDES:
        return None
    raw = inc.split()[0]
    if raw.startswith("/"):
        return ROOT / "resource" / raw.lstrip("/")
    return (src.parent / raw).resolve()


class RootsCullGuard(unittest.TestCase):
    def test_pack_facing_stubs_exist_and_are_noop(self):
        for path in STUBS:
            self.assertTrue(path.is_file(), f"missing pack-facing stub: {path}")
            text = path.read_text(encoding="utf-8", errors="replace")
            self.assertTrue(
                text.lstrip().startswith(";"),
                f"{path.name} must be comment-only",
            )
            code = "\n".join(
                line for line in text.splitlines() if not line.lstrip().startswith(";")
            )
            self.assertEqual(
                code.strip(),
                "",
                f"{path.name} must have no uncommented content",
            )

    def test_live_support_and_dcg_wiring_not_stubs(self):
        for path in LIVE_SUPPORT:
            self.assertTrue(path.is_file(), f"missing live file: {path}")
            text = path.read_text(encoding="utf-8", errors="replace")
            self.assertGreater(len(text), 200, f"{path.name} looks stubbed")
            self.assertNotIn("{exclude}}", text, f"{path.name} has an empty exclude")
        roll = (ROOT / "resource/map/multi/support_mission_roll.inc").read_text()
        self.assertIn('{"support_mission/roll"', roll)
        dcg = (ROOT / "resource/map/multi/dcg_script.inc").read_text()
        self.assertIn("support_mission_roll.inc", dcg)
        self.assertNotIn("ce_morale_machine_triggers.inc", dcg)
        self.assertNotIn("ce_command_cohesion_triggers.inc", dcg)
        self.assertNotIn("ce_broken_behavior_triggers.inc", dcg)
        self.assertIn("cmp_def", dcg)

    def test_deleted_systems_are_gone(self):
        for path in DELETED:
            self.assertFalse(path.exists(), f"cull target still present: {path}")

    def test_no_playable_map_directories(self):
        multi = ROOT / "resource/map/multi"
        leftover = sorted(
            p.name
            for p in multi.iterdir()
            if p.is_dir() and p.name != "ce"
        )
        self.assertEqual(leftover, [], f"AIO still ships map dirs: {leftover}")
        for name in ("bakhmut_1", "forest_", "map_ukrcity", "dcg_wasteland"):
            self.assertFalse((multi / name).exists(), name)

    def test_infantry_vision_cone_270(self):
        text = (ROOT / "resource/set/vision/vision_fields.inc").read_text()
        human = re.search(
            r'\{\s*"human".*?\n\}',
            text,
            re.DOTALL,
        )
        self.assertIsNotNone(human, "missing {\"human\"} vision field")
        block = human.group(0)
        self.assertIn('("h_fov" h(150))', block)
        self.assertNotIn('("h_fov" h(100))', block)
        # Vehicles/air keep their own cones; do not blindly rewrite them.
        self.assertIn('{"vehicle_main"', text)
        self.assertIn('{"airborne_main"', text)

    def test_attack_support_lua_is_inert_but_routed(self):
        lua = (ROOT / "resource/script/multiplayer/modes/attack_support.lua").read_text()
        self.assertNotIn('SetVar("id_attack_support"', lua)
        self.assertNotIn('SetVar("attack_support_ready"', lua)
        self.assertNotIn('SetVar("attack_support_use_mi"', lua)
        self.assertNotIn("require(utility", lua)
        self.assertNotIn("TrySpawnUnit", lua)
        router = (ROOT / "resource/script/multiplayer/bot.main.lua").read_text()
        self.assertIn("isAttackSupportCandidate", router)
        self.assertIn('safeRequire("resource/script/multiplayer/modes/attack_support")', router)

    def test_human_ce_keeps_roots_tags_without_morale(self):
        text = (ROOT / "resource/set/interaction_entity/human_ce.inc").read_text()
        for token in (
            "radio_man",
            "tag_ai_engineer",
            "tag_ai_tank_crew",
            "set_auto_resupply",
            "ignore_recrew",
        ):
            self.assertIn(token, text)
        for token in (
            "aio_morale_",
            "white_flag",
            "aio_cmd_",
            "aio_pow_",
        ):
            self.assertNotIn(token, text)

    def test_no_aim_range_multipliers(self):
        hits = []
        for path in ROOT.joinpath("resource").rglob("*"):
            if not path.is_file():
                continue
            if path.suffix not in {".mod", ".ext", ".inc", ".lua"}:
                continue
            text = path.read_text(encoding="utf-8", errors="replace")
            code = "\n".join(
                line for line in text.splitlines() if not line.lstrip().startswith(";")
            )
            if "aim_range" in code:
                hits.append(str(path.relative_to(ROOT)))
        self.assertEqual(hits, [], f"aim_range leftovers: {hits}")

    def test_culled_localization_entries_are_gone(self):
        loc_root = ROOT / "localizations"
        loc_text = []
        for path in loc_root.rglob("*"):
            if path.suffix.lower() not in {".pot", ".po", ".csv", ".txt"}:
                continue
            loc_text.append(path.read_text(encoding="utf-8", errors="replace"))
        blob = "\n".join(loc_text)
        for token in (
            "ce_morale_",
            "ce_pow_",
            "aio_morale_",
            "white_flag",
            "mission/multi/support/vehicle_inbound",
            "mission/multi/support/flank_inbound",
            "mission/multi/support/motorized_inbound",
            "mission/multi/support/e2_helo_inbound",
            "mission/multi/support/e2_para_inbound",
            "mission/multi/support/e2_insert_failed",
        ):
            self.assertNotIn(token, blob, f"orphan loc leftover: {token}")
        support = (
            loc_root / "default/interface/text/mission/multi/support_events.pot"
        ).read_text()
        self.assertNotIn(
            'msgctxt "mission/multi/support/airborne_inbound"',
            support,
        )
        for token in (
            "mission/multi/support/wave_inbound",
            "mission/multi/support/defense_reinforced",
            "mission/multi/support/enemy_activity",
            "mission/multi/support/airborne_inbound_nato",
            "mission/multi/support/waves_exhausted",
        ):
            self.assertIn(token, support)

    def test_preparation_time_untouched(self):
        text = (
            ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"
        ).read_text()
        self.assertIn("{preparationTime		1200}", text)

    def test_include_graph_resolves(self):
        missing = []
        scan_roots = [
            ROOT / "resource/map",
            ROOT / "resource/map_scripts",
            ROOT / "resource/set/interaction_entity",
        ]
        for scan in scan_roots:
            if not scan.exists():
                continue
            for path in scan.rglob("*"):
                if path.suffix not in {".inc", ".mi"}:
                    continue
                text = path.read_text(encoding="utf-8", errors="replace")
                for inc in uncommented_includes(text):
                    target = resolve_include(path, inc)
                    if target is None:
                        continue
                    if not target.exists():
                        missing.append(
                            f"{path.relative_to(ROOT)} -> {inc} ({target})"
                        )
        self.assertEqual(missing, [], "dangling includes:\n" + "\n".join(missing))


if __name__ == "__main__":
    unittest.main()
