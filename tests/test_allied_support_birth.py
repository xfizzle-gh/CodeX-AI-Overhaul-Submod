import re

REQUIRED_VARS = [
    "allied_support_cmd_enable",
    "allied_support_cmd_spawn_probe",
    "allied_support_cmd_stage",
    "allied_support_cmd_fail",
    "allied_support_cmd_fow_continue",
    "allied_support_cmd_gate_auto",
    "allied_support_cmd_mate_id",
]

# Strings that must never appear anywhere in our new content.
BANNED_SUBSTRINGS = ["tmai", "p013", "bus_magic", "fpc1", "fpc2", "fpc3", "fpc4", "fpc5"]


def _vars_text(mod_root):
    return (mod_root / "resource" / "map" / "multi" / "dcg_vars.inc").read_text(
        encoding="utf-8", errors="replace"
    )


def test_every_new_variable_is_declared(mod_root):
    text = _vars_text(mod_root)
    declared = set(re.findall(r'\{"([^"]+)"\}', text))
    missing = [name for name in REQUIRED_VARS if name not in declared]
    assert not missing, f"undeclared variables: {missing}"


def test_variables_are_declared_exactly_once(mod_root):
    text = _vars_text(mod_root)
    declared = re.findall(r'\{"([^"]+)"\}', text)
    for name in REQUIRED_VARS:
        assert declared.count(name) == 1, f"{name} declared {declared.count(name)} times"


def test_variable_names_carry_no_banned_substrings(mod_root):
    for name in REQUIRED_VARS:
        lowered = name.lower()
        for banned in BANNED_SUBSTRINGS:
            assert banned not in lowered, f"{name} contains banned substring {banned}"


def test_new_variables_do_not_collide_with_upstream_codex_names(mod_root):
    """Code:X owns bare allied_support_* names; ours must all be under the _cmd_ prefix."""
    upstream = {
        "allied_support_initialized", "allied_support_waves_left",
        "allied_support_wave_size", "allied_support_target",
        "allied_support_busy", "allied_support_wave_num",
    }
    for name in REQUIRED_VARS:
        assert name not in upstream
        assert name.startswith("allied_support_cmd_")
