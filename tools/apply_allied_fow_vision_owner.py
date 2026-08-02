#!/usr/bin/env python3
"""Route attack-support units to the real allied AI player for native team FoW sharing."""

from __future__ import annotations

import argparse
import re
from pathlib import Path

RUNTIME_PATH = Path("resource/script/multiplayer/modes/attack_support.lua")

ASSIGNMENT_RE = re.compile(
    r'''(?m)^(?P<indent>[ \t]*)sc:SetVar\(\s*["']id_attack_support["']\s*,\s*(?P<owner>[A-Za-z_][A-Za-z0-9_.]*)\s*\)\s*(?:--[^\r\n]*)?$'''
)

OWNER_DECLARATION = "local ownerId = positiveId(id.defenderBotId, id.playerId)"
FOW_LOG_MARKER = 'log("fow_owner", "id_attack_support", ownerId,'


def read_text(path: Path) -> tuple[str, bool]:
    raw = path.read_bytes()
    has_bom = raw.startswith(b"\xef\xbb\xbf")
    return raw.decode("utf-8-sig"), has_bom


def write_text(path: Path, text: str, has_bom: bool) -> None:
    encoding = "utf-8-sig" if has_bom else "utf-8"
    path.write_text(text, encoding=encoding, newline="")


def owner_matches(text: str) -> list[re.Match[str]]:
    return list(ASSIGNMENT_RE.finditer(text))


def validate_text(text: str) -> None:
    matches = owner_matches(text)
    if len(matches) != 1:
        raise RuntimeError(
            f"expected exactly one id_attack_support SetVar assignment, found {len(matches)}"
        )
    if matches[0].group("owner") != "ownerId":
        raise RuntimeError(
            "id_attack_support is not assigned to the resolved allied AI owner"
        )
    if text.count(OWNER_DECLARATION) != 1:
        raise RuntimeError("expected exactly one allied AI owner declaration")
    if text.count(FOW_LOG_MARKER) != 1:
        raise RuntimeError("expected exactly one allied FoW owner diagnostic")
    if 'sc:SetVar("attack_support_ready", 1)' not in text:
        raise RuntimeError("attack support readiness publication was lost")
    if 'sc:SetVar("attack_support_use_mi", 1)' not in text:
        raise RuntimeError("MI delivery publication was lost")


def has_overlay(text: str) -> bool:
    matches = owner_matches(text)
    return (
        len(matches) == 1
        and matches[0].group("owner") == "ownerId"
        and OWNER_DECLARATION in text
        and FOW_LOG_MARKER in text
    )


def replacement(indent: str) -> str:
    return "\n".join(
        (
            f"{indent}-- The Lua controller slot is not the real lobby teammate. Keep",
            f"{indent}-- support units AI-owned, but assign them to Conquest's actual",
            f"{indent}-- allied AI player so native team FoW/LOS sharing can apply.",
            f"{indent}{OWNER_DECLARATION}",
            f"{indent}if ownerId <= 0 then",
            f'{indent}\tlog("identity_publish_skipped", "allied_owner_unresolved",',
            f'{indent}\t\t"controller_playerId", id.playerId,',
            f'{indent}\t\t"defenderBotId", id.defenderBotId,',
            f'{indent}\t\t"team", id.team)',
            f"{indent}\treturn",
            f"{indent}end",
            f'{indent}sc:SetVar("id_attack_support", ownerId)',
            f'{indent}log("fow_owner", "id_attack_support", ownerId,',
            f'{indent}\t"controller_playerId", id.playerId,',
            f'{indent}\t"defenderBotId", id.defenderBotId,',
            f'{indent}\t"team", id.team)',
        )
    )


def apply(root: Path, check_only: bool = False) -> bool:
    path = root / RUNTIME_PATH
    if not path.is_file():
        raise FileNotFoundError(f"missing deployed attack-support controller: {path}")

    text, has_bom = read_text(path)
    if has_overlay(text):
        validate_text(text)
        return False

    matches = owner_matches(text)
    if len(matches) != 1:
        raise RuntimeError(
            f"expected one id_attack_support SetVar assignment, found {len(matches)}: {path}"
        )

    current_owner = matches[0].group("owner")
    if "firstPlayerId" in current_owner:
        raise RuntimeError("refusing to patch a human-owned support assignment")
    if OWNER_DECLARATION in text or FOW_LOG_MARKER in text:
        raise RuntimeError("partial allied FoW overlay detected; refusing a second insertion")
    if check_only:
        raise RuntimeError("allied FoW owner overlay has not been applied")

    match = matches[0]
    updated = text[: match.start()] + replacement(match.group("indent")) + text[match.end() :]
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
