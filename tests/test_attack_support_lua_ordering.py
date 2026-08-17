import re

LUA = ("resource", "script", "multiplayer", "modes", "attack_support.lua")


def _lua(mod_root):
    path = mod_root
    for part in LUA:
        path = path / part
    return path.read_text(encoding="utf-8", errors="replace")


def _lua_code(mod_root):
    """Source with '--' comment lines dropped.

    The file documents the forbidden BotApi fields by name in a warning comment;
    a check for those names must inspect code, not prose.
    """
    return "\n".join(
        line for line in _lua(mod_root).splitlines()
        if not line.lstrip().startswith("--")
    )


def test_ordering_is_gated_by_a_single_named_switch(mod_root):
    text = _lua(mod_root)
    assert re.search(r"^local ORDERING_ENABLED = false", text, re.M), \
        "expected a top-level ORDERING_ENABLED = false switch"


def test_order_issuing_functions_return_before_commanding(mod_root):
    """orderSquad and orderNewSquads must bail out before any command call."""
    text = _lua(mod_root)
    for func in ("local function orderSquad(", "local function orderNewSquads("):
        signature = text.index(func)
        # Scan from AFTER the signature line: the function's own name contains
        # "Squads", which is not a command call.
        start = text.index("\n", signature) + 1
        body = text[start:start + 400]
        guard = body.index("if not ORDERING_ENABLED then return end")
        # The guard must precede any command call inside the function body.
        for call in ("CaptureFlag", "SeekAndDestroy", "Squads"):
            hit = body.find(call)
            if hit >= 0:
                assert guard < hit, f"{func} calls {call} before the ORDERING_ENABLED guard"


def test_periodic_reorder_loop_is_gated(mod_root):
    """The 400-quant re-order of every squad must not run."""
    text = _lua(mod_root)
    start = text.index("local function onQuant()")
    body = text[start:text.index("local function onGameEnd()")]
    assert "% 400 == 0" in body
    reorder = body.index("% 400 == 0")
    guard = body.rindex("ORDERING_ENABLED", 0, reorder)
    assert guard < reorder, "the 400-quant re-order block is not gated"


def test_identity_publication_and_mirror_survive(mod_root):
    text = _lua(mod_root)
    assert "publishIdentity(id)" in text, "identity publication must be kept"
    assert "mirrorEngineState()" in text, "engine-state mirror must be kept"
    assert "local function mirrorEngineState()" in text


def test_no_banned_strings_added(mod_root):
    text = _lua(mod_root).lower()
    for banned in ("tmai", "p013"):
        assert banned not in text


def test_mate_id_is_published_from_the_slots_own_player_id(mod_root):
    text = _lua(mod_root)
    assert "allied_support_cmd_mate_id" in text, "mate id must be published for the MI handoff"
    assert re.search(r'SetVar\(\s*"allied_support_cmd_mate_id"', text), \
        "expected a SetVar publication of the mate id"


def test_mate_id_is_never_hardcoded(mod_root):
    """Lobby slot assignment varies; a literal 1 made earlier proofs contradictory."""
    text = _lua(mod_root)
    block_start = text.index("allied_support_cmd_mate_id")
    block = text[max(0, block_start - 300):block_start + 300]
    assert not re.search(r'allied_support_cmd_mate_id"\s*,\s*1\s*\)', block), \
        "mate id must come from identity, never the literal 1"
    assert "id.playerId" in block or "identity.playerId" in block


def test_mate_id_publication_does_not_read_forbidden_fields(mod_root):
    """These fields null-deref natively on the extra Team A slot; pcall cannot catch it."""
    code = _lua_code(mod_root)
    assert "spawnPointName" not in code
    assert "PlayerSpawnPoint" not in code
