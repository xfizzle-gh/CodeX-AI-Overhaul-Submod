import re

REQUIRED_VARS = [
    "allied_support_cmd_enable",
    "allied_support_cmd_spawn_probe",
    "allied_support_cmd_stage",
    "allied_support_cmd_fail",
    "allied_support_cmd_fow_continue",
    "allied_support_cmd_gate_auto",
    "allied_support_cmd_gate_ticks",
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


BIRTH_INC = ("resource", "map", "multi", "allied_support_birth.inc")


def _read(mod_root, parts):
    path = mod_root
    for part in parts:
        path = path / part
    return path.read_text(encoding="utf-8", errors="replace")


def test_birth_file_exists_and_declares_its_triggers(mod_root):
    text = _read(mod_root, BIRTH_INC)
    for trigger in (
        '{"allied_support_cmd/birth_init"',
        '{"allied_support_cmd/birth_dispatch"',
        '{"allied_support_cmd/birth_verify"',
    ):
        assert trigger in text, f"missing trigger {trigger}"


def test_birth_uses_clone_dispatch_not_placement(mod_root):
    text = _read(mod_root, BIRTH_INC)
    assert '{"actor_to_waypoint"' in text
    assert "{clone}" in text
    assert '{approach "safe teleport & rotate"}' in text
    assert '{"placement"' not in text, "birth must clone, never place"


def test_birth_targets_both_pads(mod_root):
    text = _read(mod_root, BIRTH_INC)
    assert '{waypoint "31"}' in text
    assert '{waypoint "32"}' in text


def test_birth_is_interlocked_against_the_spawn_probe(mod_root):
    """Both mechanisms enabled must be a hard failure, not a silent preference."""
    text = _read(mod_root, BIRTH_INC)
    assert "allied_support_cmd_spawn_probe$" in text
    assert '{value 5}' in text, "fail code 5 (both mechanisms enabled) must be reachable"


def test_birth_fails_closed_on_untagged_arrival(mod_root):
    text = _read(mod_root, BIRTH_INC)
    assert "allied_support_cmd_fresh" in text
    assert '{value 90}' in text, "stage 90 (failed) must be reachable"
    assert '{value 2}' in text, "fail code 2 (arrival not tagged) must be reachable"


def _strip_comments(text):
    """Drop ';' comment lines. Prose documenting the banned pattern must not trip it."""
    return "\n".join(
        line for line in text.splitlines() if not line.lstrip().startswith(";")
    )


def test_birth_never_selects_by_recency_or_appearance(mod_root):
    """The #112 false-positive class: no fallback that grabs an arbitrary new human."""
    code = _strip_comments(_read(mod_root, BIRTH_INC)).lower()
    for smell in ("newest", "recent", "last_created", "any_human", "unclaimed"):
        assert smell not in code, f"suspicious recovery selector: {smell}"


def test_birth_never_applies_lua_guard_tags(mod_root):
    text = _read(mod_root, BIRTH_INC)
    assert "tag_add _lua_mi" not in text
    assert "tag_add _lua_ignore" not in text


def test_birth_has_no_banned_strings(mod_root):
    text = _read(mod_root, BIRTH_INC).lower()
    for banned in BANNED_SUBSTRINGS:
        assert banned not in text, f"banned substring {banned}"


def test_every_onscreen_timer_is_debug_gated(mod_root):
    """A shipped run must show the player nothing."""
    text = _read(mod_root, BIRTH_INC)
    if '{"timer"' in text:
        assert 'support_debug$' in text, "timers present but no support_debug$ gate"
        assert text.count('{"timer"') <= text.count('support_debug$')
