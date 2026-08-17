import sys
import pathlib

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent / "tools"))

from arm_fow_run import arm, disarm, is_armed, ANCHOR

BASE = (
    '\t\t\t{"allied_support_cmd/birth_init"\n'
    "\t\t\t\t{actions\n"
    + ANCHOR
    + '\t\t\t\t\t{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 0}}\n'
    "\t\t\t\t}\n"
    "\t\t\t}\n"
)


def test_arm_then_disarm_round_trips_byte_identically():
    """Toggling must never corrupt the shipped file."""
    armed, changed = arm(BASE)
    assert changed and is_armed(armed)
    back, changed = disarm(armed)
    assert changed and not is_armed(back)
    assert back == BASE


def test_arming_sets_both_required_variables():
    armed, _ = arm(BASE)
    assert '{"set_i" {var "allied_support_cmd_enable$"} {op "="} {value 1}}' in armed
    assert '{"set_i" {var "support_debug$"} {op "="} {value 1}}' in armed


def test_arm_and_disarm_are_both_idempotent():
    armed, _ = arm(BASE)
    again, changed = arm(armed)
    assert not changed and again == armed
    clean, _ = disarm(armed)
    still, changed = disarm(clean)
    assert not changed and still == clean


def test_disarm_leaves_the_anchor_intact():
    """The anchor is real shipped content, not part of the arming block."""
    armed, _ = arm(BASE)
    back, _ = disarm(armed)
    assert ANCHOR in back
