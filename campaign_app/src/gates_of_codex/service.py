from __future__ import annotations

import json
from dataclasses import asdict, dataclass
from datetime import UTC, datetime
from pathlib import Path

from .bridge.archive import CampaignSaveArchive
from .bridge.result import BattleImportResult, BattleResultImporter
from .bridge.scn import CampaignScnBuilder
from .bridge.status import BattleStatusOptions, StatusBuilder, StatusResult
from .campaign import CampaignEngine
from .codex.catalog import CodeXCatalogScanner
from .state_io import load, save


@dataclass(frozen=True, slots=True)
class BattleExportManifest:
    battle_id: str
    state_path: str
    save_path: str
    map_string: str
    catalog_signature: str
    played_games: int
    won_games: int
    exported_at_utc: str

    @property
    def baseline_status(self) -> StatusResult:
        return StatusResult(self.played_games, self.won_games)

    def write(self, path: str | Path) -> Path:
        destination = Path(path)
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_text(
            json.dumps(asdict(self), indent=2, ensure_ascii=False) + "\n",
            encoding="utf-8",
        )
        return destination

    @classmethod
    def read(cls, path: str | Path) -> "BattleExportManifest":
        data = json.loads(Path(path).read_text(encoding="utf-8"))
        return cls(**data)


def manifest_path_for(save_path: str | Path) -> Path:
    source = Path(save_path)
    return source.with_suffix(source.suffix + ".goc.json")


class GatesOfCodeXService:
    def export_pending_battle(
        self,
        *,
        state_path: str | Path,
        code_x_directory: str | Path,
        save_path: str | Path,
        options: BattleStatusOptions,
    ) -> BattleExportManifest:
        campaign_path = Path(state_path)
        state = load(campaign_path)
        pending = state.pending_battle
        if pending is None:
            raise RuntimeError("Campaign has no pending battle to export")

        catalog = CodeXCatalogScanner().scan(code_x_directory)
        scn = CampaignScnBuilder(catalog, code_x_directory).build(state, pending)
        status = StatusBuilder().build(pending, options)
        save_destination = CampaignSaveArchive().write(
            save_path, status=status, campaign_scn=scn
        )

        pending.exported_save_path = str(save_destination.resolve())
        pending.started = True
        state.code_x_directory = str(Path(code_x_directory).resolve())
        save(state, campaign_path)

        manifest = BattleExportManifest(
            battle_id=pending.battle_id,
            state_path=str(campaign_path.resolve()),
            save_path=str(save_destination.resolve()),
            map_string=options.map_string,
            catalog_signature=catalog.source_signature,
            played_games=options.played_games,
            won_games=options.won_games,
            exported_at_utc=datetime.now(UTC).isoformat(),
        )
        manifest.write(manifest_path_for(save_destination))
        return manifest

    def import_completed_battle(
        self,
        *,
        state_path: str | Path,
        save_path: str | Path,
        manifest_path: str | Path | None = None,
    ) -> BattleImportResult:
        campaign_path = Path(state_path)
        state = load(campaign_path)
        engine = CampaignEngine(state)
        manifest = BattleExportManifest.read(
            manifest_path or manifest_path_for(save_path)
        )
        pending = state.pending_battle
        if pending is None:
            raise RuntimeError("Campaign has no pending battle")
        if pending.battle_id != manifest.battle_id:
            raise ValueError(
                f"Manifest battle {manifest.battle_id} does not match pending battle "
                f"{pending.battle_id}"
            )
        result = BattleResultImporter().import_save(
            engine,
            save_path,
            previous_status=manifest.baseline_status,
        )
        save(state, campaign_path)
        return result
