#!/usr/bin/env python3
"""Route attack-support units to the real allied AI player for native team FoW sharing.

The attack-support Lua controller runs on a phantom controller slot. Its BotApi
playerId is not the actual AI teammate shown in the game lobby. Conquest exposes
that real allied AI as DefenderBotId. Support units must remain AI-owned and
AI-controlled, but must belong to that real teammate so normal team vision can
be shared with the human player.
"""

from __future__ import annotations

import argparse
from pathlib import Path

RUNTIME_PATH = Path("resource/script/multiplayer/modes/attack_support.lua")

OLD_BLOCK = '''\tsc:SetVar("id_attack_support", id.playerId)
\tsc:SetVar("attack_support_ready", 1)
\t-- MI waves are the working delivery path for attack support units.
\tsc:SetVar("attack_support_use_mi", 1)
\tlog("identity_published", "id_attack_support", id.playerId, "mi_waves", 1)
'''

NEW_BLOCK = '''\t-- The controller slot is not the real lobby teammate. Runtime logs show the
\t-- controller as player 1 while the actual allied AI is DefenderBotId/player 4.
\t-- Ownership must stay AI-controlled, but use the real team member so the
\t-- engine's normal allied fog-of-war/LOS sharing can include these units.
\tlocal ownerId = positiveId(id.defenderBotId, id.playerId)
\tif ownerId <= 0 then
\t\tlog("identity_publish_skipped", "allied_owner_unresolved",
\t\t\t"controller_playerId", id.playerId,
\t\t\t"defenderBotId", id.defenderBotId,
\t\t\t"team", id.team)
\t\treturn
\tend
\tsc:SetVar("id_attack_support", ownerId)
\tsc:SetVar("attack_support_ready", 1)
\t-- MI waves are the working delivery path for attack support units.
\tsc:SetVar("attack_support_use_mi", 1)
\tlog("identity_published", "id_attack_support", ownerId,
\t\t"controller_playerId", id.playerId,
\t\t"defenderBotId", id.defenderBotId,
\t\t"team", id.team,
\t\t"mi_waves", 1)
'''

REQUIRED_MARKERS = (
    "local ownerId = positiveId(id.defenderBotId, id.playerId)",
    'sc:SetVar("id_attack_support", ownerId)',
    '"controller_playerId", id.playerId',
    '"defenderBotId", id.defenderBotId',
    '"team", id.team',
)

FORBIDDEN_MARKERS = (
    'sc:SetVar("id_attack_support", id.firstPlayerId)',
    'sc:SetVar("id_attack_support", id.playerId)',
)


def read_text(path: Path) -> tuple[str, bool]:
    raw = path.read_bytes()
    has_bom = raw.startswith(b"\xef\xbb\xbf")
    return raw.decode("utf-8-sig"), has_bom


def write_text(path: Path, text: str, has_bom: bool) -> None:
    encoding = "utf-8-sig" if has_bom else "utf-8"
    path.write_text(text, encoding=encoding, newline="")


def validate_text(text: str) -> None:
    for marker in REQUIRED_MARKERS:
        if text.count(marker) != 1:
            raise RuntimeError(f"expected exactly one allied FoW marker: {marker}")
    for marker in FORBIDDEN_MARKERS:
        if marker in text:
            raise RuntimeError(f"forbidden stale owner assignment remains: {marker}")

    publish_start = text.find("local function publishIdentity(id)")
    publish_end = text.find("\nend\n", publish_start)
    if publish_start < 0 or publish_end < 0:
        raise RuntimeError("publishIdentity function could not be isolated")
    publish = text[publish_start:publish_end]
    if 'sc:SetVar("attack_support_ready", 1)' not in publish:
        raise RuntimeError("attack support readiness publication was lost")
    if 'sc:SetVar("attack_support_use_mi", 1)' not in publish:
        raise RuntimeError("MI delivery publication was lost")
    if "id.firstPlayerId" in publish:
        raise RuntimeError("human ownership is forbidden; units must remain AI-owned")


def apply(root: Path, check_only: bool = False) -> bool:
    path = root / RUNTIME_PATH
    if not path.is_file():
        raise FileNotFoundError(f"missing deployed attack-support controller: {path}")

    text, has_bom = read_text(path)
    if all(marker in text for marker in REQUIRED_MARKERS):
        validate_text(text)
        return False

    count = text.count(OLD_BLOCK)
    if count != 1:
        raise RuntimeError(
            f"expected one canonical attack-support owner block, found {count}: {path}"
        )
    if check_only:
        raise RuntimeError("allied FoW owner overlay has not been applied")

    updated = text.replace(OLD_BLOCK, NEW_BLOCK, 1)
    validate_text(updated)
    write_text(path, updated, has_bom)
    return True


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", required=True, type=Path)
    parser.add_argument("--check", action="store_true")
    args = parser.parse_args()

    changed = apply(args.root.resolve(), check_only=args.check)
    print(
        "allied_fow_owner="
        + ("patched_to_real_ai_teammate" if changed else "already_valid")
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
