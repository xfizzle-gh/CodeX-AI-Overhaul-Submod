from __future__ import annotations

import json
import os
import tempfile
from dataclasses import fields
from pathlib import Path
from typing import Any, TypeVar

from .models import (
    Battalion,
    BattalionRosterEntry,
    BattalionType,
    BattleParticipant,
    CampaignState,
    Faction,
    FactionState,
    PendingBattle,
    Province,
)

T = TypeVar("T")


def _filter_kwargs(cls: type[T], data: dict[str, Any]) -> dict[str, Any]:
    allowed = {item.name for item in fields(cls)}
    return {key: value for key, value in data.items() if key in allowed}


def _roster_entry(data: dict[str, Any]) -> BattalionRosterEntry:
    return BattalionRosterEntry(**_filter_kwargs(BattalionRosterEntry, data))


def _battalion(data: dict[str, Any]) -> Battalion:
    payload = _filter_kwargs(Battalion, data)
    payload["faction"] = Faction(payload["faction"])
    payload["battalion_type"] = BattalionType(
        payload.get("battalion_type", BattalionType.COMBINED_ARMS)
    )
    payload["roster"] = [_roster_entry(item) for item in payload.get("roster", [])]
    return Battalion(**payload)


def _province(data: dict[str, Any]) -> Province:
    payload = _filter_kwargs(Province, data)
    payload["owner"] = Faction(payload.get("owner", Faction.NEUTRAL))
    return Province(**payload)


def _faction_state(data: dict[str, Any]) -> FactionState:
    payload = _filter_kwargs(FactionState, data)
    payload["faction"] = Faction(payload["faction"])
    payload["recruited_pool"] = [
        _roster_entry(item) for item in payload.get("recruited_pool", [])
    ]
    return FactionState(**payload)


def _participant(data: dict[str, Any]) -> BattleParticipant:
    payload = _filter_kwargs(BattleParticipant, data)
    payload["faction"] = Faction(payload["faction"])
    return BattleParticipant(**payload)


def _pending_battle(data: dict[str, Any] | None) -> PendingBattle | None:
    if not data:
        return None
    payload = _filter_kwargs(PendingBattle, data)
    payload["attacker_faction"] = Faction(payload["attacker_faction"])
    payload["defender_faction"] = Faction(payload["defender_faction"])
    payload["player_faction"] = Faction(payload["player_faction"])
    payload["attacking_participants"] = [
        _participant(item) for item in payload.get("attacking_participants", [])
    ]
    payload["defending_participants"] = [
        _participant(item) for item in payload.get("defending_participants", [])
    ]
    return PendingBattle(**payload)


def from_dict(data: dict[str, Any]) -> CampaignState:
    payload = _filter_kwargs(CampaignState, data)
    payload["current_faction"] = Faction(payload.get("current_faction", Faction.NATO))
    payload["selected_faction"] = Faction(payload.get("selected_faction", Faction.NATO))
    payload["factions"] = {
        key: _faction_state(value) for key, value in payload.get("factions", {}).items()
    }
    payload["provinces"] = {
        key: _province(value) for key, value in payload.get("provinces", {}).items()
    }
    payload["battalions"] = {
        key: _battalion(value) for key, value in payload.get("battalions", {}).items()
    }
    payload["pending_battle"] = _pending_battle(payload.get("pending_battle"))
    state = CampaignState(**payload)
    state.validate()
    return state


def load(path: str | Path) -> CampaignState:
    source = Path(path)
    try:
        data = json.loads(source.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise FileNotFoundError(f"Campaign file does not exist: {source}") from exc
    except json.JSONDecodeError as exc:
        raise ValueError(f"Campaign file contains invalid JSON: {source}: {exc}") from exc
    if not isinstance(data, dict):
        raise ValueError(f"Campaign root must be a JSON object: {source}")
    return from_dict(data)


def save(state: CampaignState, path: str | Path) -> Path:
    state.validate()
    destination = Path(path)
    destination.parent.mkdir(parents=True, exist_ok=True)
    serialized = json.dumps(state.to_dict(), indent=2, ensure_ascii=False) + "\n"

    fd, temporary_name = tempfile.mkstemp(
        prefix=f".{destination.name}.", suffix=".tmp", dir=destination.parent
    )
    try:
        with os.fdopen(fd, "w", encoding="utf-8", newline="\n") as handle:
            handle.write(serialized)
            handle.flush()
            os.fsync(handle.fileno())
        os.replace(temporary_name, destination)
    except Exception:
        try:
            os.unlink(temporary_name)
        except FileNotFoundError:
            pass
        raise
    return destination
