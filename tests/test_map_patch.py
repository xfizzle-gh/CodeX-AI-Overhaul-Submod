import sys
import pathlib

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent / "tools"))

from patch_allied_support_maps import birth_waypoint_block, patch_text
from verify_waypoint_band import waypoint_names

MINIMAL_MAP = """{mission
	{script
			(include "../attack_support_waves.inc")
			(include "../enemy_defense_support.inc")
			(include "../dcg_script.inc")
	}
	{waypoints
		{"attack_support_rear_a1"
			{position -6545.48 -920.01 0.00}
			{radius 150}
		}
		{"attack_support_rear_b1"
			{position 6482.88 1507.47 0.00}
			{radius 150}
		}
	}
}
"""


def test_birth_waypoint_block_tags_arrival_without_a_selector():
    block = birth_waypoint_block("31", "-6545.48", "-920.01", "0.00", "a")
    assert '{"31"' in block
    assert "{commands" in block
    # The arrival tag must be applied by a bare entity_state with no selector, which
    # is what makes it act on the arriving actor.
    assert "{tag_add allied_support_cmd_fresh}" in block
    assert "{tag_add allied_support_cmd_side_a}" in block
    assert "{selector" not in block.split("{commands", 1)[1].split("{tag_add", 1)[0]


def test_birth_waypoint_block_never_tags_lua_guards():
    block = birth_waypoint_block("31", "0", "0", "0", "a")
    assert "_lua_mi" not in block
    assert "_lua_ignore" not in block


def test_patch_adds_both_pads_and_both_includes():
    patched, changes = patch_text(MINIMAL_MAP)
    assert changes, "expected changes on a fresh map"
    assert waypoint_names(patched) >= {"31", "32"}
    assert '(include "../allied_support_birth.inc")' in patched
    assert '(include "../allied_support_handoff.inc")' in patched


def test_patch_positions_pads_on_the_existing_rear_pads():
    patched, _ = patch_text(MINIMAL_MAP)
    assert "-6545.48 -920.01" in patched.split('{"31"', 1)[1][:200]
    assert "6482.88 1507.47" in patched.split('{"32"', 1)[1][:200]


def test_patch_is_idempotent():
    once, first_changes = patch_text(MINIMAL_MAP)
    twice, second_changes = patch_text(once)
    assert first_changes
    assert second_changes == []
    assert once == twice


def test_includes_are_added_before_dcg_script():
    """dcg_script.inc must stay last, matching the existing include order."""
    patched, _ = patch_text(MINIMAL_MAP)
    birth = patched.index('allied_support_birth.inc')
    handoff = patched.index('allied_support_handoff.inc')
    dcg = patched.index('dcg_script.inc')
    assert birth < dcg
    assert handoff < dcg


def test_all_fourteen_live_maps_are_patched(map_files):
    for path in map_files:
        text = path.read_text(encoding="utf-8", errors="replace")
        names = waypoint_names(text)
        assert {"31", "32"} <= names, f"{path.parent.name} missing birth pads"
        assert '(include "../allied_support_birth.inc")' in text, path.parent.name
        assert '(include "../allied_support_handoff.inc")' in text, path.parent.name
