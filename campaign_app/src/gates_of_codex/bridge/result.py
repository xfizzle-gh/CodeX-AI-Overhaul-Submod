from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from ..campaign import CampaignEngine
from ..models import Faction
from .archive import CampaignSaveArchive
from .scn import CampaignScnParser
from .status import StatusBuilder, StatusResult


@dataclass(frozen=True, slots=True)
class BattleImportResult:
    winner: Faction
    player_won: bool
    previous_status: StatusResult
    current_status: StatusResult
    survivor_counts: dict[str, int]


class BattleResultImporter:
    def __init__(self) -> None:
        self.archive = CampaignSaveArchive()
        self.status = StatusBuilder()
        self.scn = CampaignScnParser()

    def import_save(
        self,
        engine: CampaignEngine,
        save_path: str | Path,
        *,
        previous_status: StatusResult,
    ) -> BattleImportResult:
        pending = engine.state.pending_battle
        if pending is None:
            raise RuntimeError("Campaign has no pending battle to import")
        contents = self.archive.read(save_path)
        current = self.status.parse_result(contents.status)
        if current.played_games <= previous_status.played_games:
            raise ValueError(
                "GoH save does not show a newly completed battle: "
                f"playedGames {current.played_games} <= {previous_status.played_games}"
            )
        player_won = current.player_won_since(previous_status)
        winner = (
            pending.player_faction
            if player_won
            else (
                pending.defender_faction
                if pending.player_is_attacker
                else pending.attacker_faction
            )
        )
        survivors = self.scn.survivor_rosters(contents.campaign_scn, pending)
        engine.apply_external_battle_result(winner, survivors)
        return BattleImportResult(
            winner=winner,
            player_won=player_won,
            previous_status=previous_status,
            current_status=current,
            survivor_counts={
                battalion_id: sum(entry.quantity for entry in roster)
                for battalion_id, roster in survivors.items()
            },
        )
