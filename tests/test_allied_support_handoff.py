HANDOFF_INC = ("resource", "map", "multi", "allied_support_handoff.inc")
BANNED_SUBSTRINGS = ["tmai", "p013", "bus_magic", "fpc1", "fpc2", "fpc3", "fpc4", "fpc5"]


def _read(mod_root):
    path = mod_root
    for part in HANDOFF_INC:
        path = path / part
    return path.read_text(encoding="utf-8", errors="replace")


def test_handoff_declares_its_three_triggers(mod_root):
    text = _read(mod_root)
    for trigger in (
        '{"allied_support_cmd/handoff_human"',
        '{"allied_support_cmd/handoff_gate"',
        '{"allied_support_cmd/handoff_mate"',
    ):
        assert trigger in text, f"missing trigger {trigger}"


def test_human_ownership_uses_a_full_sixteen_case_switch(mod_root):
    """The engine will not accept a variable in the {player} field."""
    text = _read(mod_root)
    human_block = text.split('handoff_human')[1].split('handoff_gate')[0]
    for n in range(1, 17):
        assert f'{{value {n}}}' in human_block, f"missing case for player {n}"
        assert f'{{player "{n}"}}' in human_block, f"missing player literal {n}"


def test_human_switch_is_keyed_from_id_attack_support(mod_root):
    text = _read(mod_root)
    human_block = text.split('handoff_human')[1].split('handoff_gate')[0]
    assert 'id_attack_support$' in human_block
    assert human_block.count('id_attack_support$') == 16


def test_mate_switch_is_keyed_from_the_runtime_resolved_mate_id(mod_root):
    text = _read(mod_root)
    mate_block = text.split('handoff_mate')[1]
    assert 'allied_support_cmd_mate_id$' in mate_block
    assert mate_block.count('allied_support_cmd_mate_id$') == 17  # 16 cases + the guard
    for n in range(1, 17):
        assert f'{{player "{n}"}}' in mate_block, f"missing mate player literal {n}"


def _gate_block(text):
    """The gate trigger plus its poll loop, up to the mate transfer.

    Index slicing, not split: 'handoff_gate' is a prefix of 'handoff_gate_poll',
    so splitting would cut the poll loop off.
    """
    start = text.index('{"allied_support_cmd/handoff_gate"')
    end = text.index('{"allied_support_cmd/handoff_mate"')
    return text[start:end]


def test_gate_blocks_on_the_continue_variable(mod_root):
    text = _read(mod_root)
    assert 'allied_support_cmd_fow_continue$' in text
    assert 'allied_support_cmd_fow_continue$' in _gate_block(text)


def _strip_comments(text):
    """Drop ';' comment lines. Prose naming a banned form must not trip the check."""
    return "\n".join(
        line for line in text.splitlines() if not line.lstrip().startswith(";")
    )


def test_gate_uses_a_self_retriggering_poll_not_a_blocking_wait(mod_root):
    """A wait action has no precedent in this codebase; self-retrigger does."""
    text = _read(mod_root)
    assert '{"wait"' not in _strip_comments(text), \
        "no shipped precedent for a wait action here"
    assert '{"trigger" {name "allied_support_cmd/handoff_gate_poll"}}' in text
    assert '{"allied_support_cmd/handoff_gate_poll"' in text


def test_gate_auto_release_is_recorded_and_bounded(mod_root):
    text = _read(mod_root)
    gate = _gate_block(text)
    assert 'allied_support_cmd_gate_auto$' in gate, \
        "an auto-release must be recorded so it is never counted as a pass"
    # Bound is 30 ticks x 2.0s = 60s, matching the spec's observation backstop.
    assert '{time 2.0}' in gate, "expected the 2s poll interval"
    assert '{value 30}' in gate, "expected the 30-tick cap (30 x 2.0s = 60s)"


def test_gate_poll_increments_its_tick_counter(mod_root):
    """Without an increment the loop would never reach the backstop."""
    gate = _gate_block(_read(mod_root))
    assert 'allied_support_cmd_gate_ticks$' in gate
    assert '{op "+"} {value 1}' in gate, "poll must increment the tick counter"


def test_mate_transfer_cannot_precede_the_gate(mod_root):
    """Stage ordering: mate transfer requires stage 50 (continue received)."""
    text = _read(mod_root)
    mate_block = text.split('handoff_mate')[1]
    condition = mate_block.split('{actions')[0]
    assert '{value 50}' in condition, \
        "mate transfer must be conditioned on stage 50, not reachable from stage 40"


def test_settle_is_three_seconds_before_stage_seventy(mod_root):
    text = _read(mod_root)
    mate_block = text.split('handoff_mate')[1]
    assert '{time 3.0}' in mate_block
    assert '{value 70}' in mate_block


def test_handoff_never_applies_lua_guard_tags(mod_root):
    text = _read(mod_root)
    assert 'tag_add _lua_mi' not in text
    assert 'tag_add _lua_ignore' not in text


def test_handoff_has_no_banned_strings(mod_root):
    text = _read(mod_root).lower()
    for banned in BANNED_SUBSTRINGS:
        assert banned not in text, f"banned substring {banned}"
