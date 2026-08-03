from __future__ import annotations

import re
from dataclasses import dataclass, field
from pathlib import Path

from ..codex.catalog import CodeXCatalog
from ..models import BattalionRosterEntry, CampaignState, PendingBattle


@dataclass(slots=True)
class ParsedCampaignSquad:
    unit_name: str
    stage: str
    object_ids: list[str] = field(default_factory=list)


class ObjectIdAllocator:
    def __init__(self, start: int = 0x10000001) -> None:
        self._next = start

    def allocate(self) -> str:
        value = f"0x{self._next:08x}"
        self._next += 1
        return value


class CampaignScnBuilder:
    def __init__(self, catalog: CodeXCatalog, code_x_directory: str | Path) -> None:
        self.catalog = catalog
        self.root = Path(code_x_directory)
        self._breed_index: dict[str, str] | None = None

    def build(self, state: CampaignState, pending: PendingBattle) -> str:
        allocator = ObjectIdAllocator()
        object_blocks: list[str] = []
        inventory_blocks: list[str] = []
        squad_rows: list[str] = []
        participant_stages = {
            participant.battalion_id: participant.stage
            for participant in (*pending.attacking_participants, *pending.defending_participants)
        }

        for battalion_id, stage in participant_stages.items():
            battalion = state.battalions.get(battalion_id)
            if battalion is None:
                raise KeyError(f"Pending battle references missing battalion {battalion_id}")
            for entry in battalion.roster:
                definition = self.catalog.units.get(entry.unit_name)
                if definition is None:
                    raise KeyError(f"Code:X catalog has no definition for {entry.unit_name}")
                for _ in range(entry.quantity):
                    ids: list[str] = []
                    if definition.vehicles:
                        entity_id = allocator.allocate()
                        ids.append(entity_id)
                        object_blocks.append(self._entity_block(definition.vehicles[0], entity_id))
                        inventory_blocks.append(self._empty_inventory(entity_id))
                    for breed, count in definition.members.items():
                        for _ in range(count):
                            human_id = allocator.allocate()
                            ids.append(human_id)
                            object_blocks.append(
                                self._human_block(
                                    breed,
                                    definition.side,
                                    definition.period or "2022s",
                                    human_id,
                                )
                            )
                            inventory_blocks.append(self._empty_inventory(human_id))
                    if not ids:
                        raise ValueError(
                            f"Unit {definition.name} has no materializable Human or Entity members"
                        )
                    ids_text = " ".join(ids)
                    squad_rows.append(
                        f'\t\t{{"{self._escape(entry.unit_name)}" "{self._escape(stage)}" {ids_text}}}'
                    )

        lines = ["{campaign"]
        lines.extend(object_blocks)
        lines.extend(inventory_blocks)
        lines.append("\t{CampaignSquads")
        lines.extend(squad_rows)
        lines.append("\t}")
        lines.append("}")
        text = "\n".join(lines) + "\n"
        self.validate(text)
        return text

    @classmethod
    def validate(cls, text: str) -> None:
        parser = CampaignScnParser()
        squads = parser.parse_squads(text)
        if not squads:
            raise ValueError("campaign.scn has no CampaignSquads rows")
        objects = set(re.findall(r'\{\s*(?:Human|Entity)\s+"[^"]+"\s+(0x[0-9a-fA-F]+)', text))
        inventories = set(re.findall(r'\{\s*Inventory\s+(0x[0-9a-fA-F]+)', text))
        object_count = len(
            re.findall(r'\{\s*(?:Human|Entity)\s+"[^"]+"\s+0x[0-9a-fA-F]+', text)
        )
        if len(objects) != object_count:
            raise ValueError("campaign.scn contains duplicate Human or Entity IDs")
        for squad in squads:
            for object_id in squad.object_ids:
                if object_id not in objects:
                    raise ValueError(
                        f"CampaignSquads references missing Human or Entity ID {object_id}"
                    )
                if object_id not in inventories:
                    raise ValueError(f"Object {object_id} has no Inventory block")

    def _human_block(self, breed: str, side: str, period: str, object_id: str) -> str:
        breed_path = self._resolve_breed_path(breed, side, period)
        return "\n".join(
            [
                f'\t{{Human "{self._escape(breed_path)}" {object_id}',
                "\t\t{Position 0 0}",
                '\t\t{TexMod "auto"}',
                "\t\t{SpawnedInFog}",
                '\t\t{Volume "ram"',
                "\t\t\t{able {visible 0}{bullet 0}{throwing 0}{obstacle 0}{contact 0}{contact_ground 0}{blast 0}{select 0}{touch 0}{blockcamera 0}}",
                "\t\t\t{disabled}",
                "\t\t}",
                "\t\t{Player 0}",
                f"\t\t{{MID {int(object_id, 16)}}}",
                '\t\t{FsmState "stand_noaim"}',
                "\t}",
            ]
        )

    @staticmethod
    def _entity_block(entity: str, object_id: str) -> str:
        return "\n".join(
            [
                f'\t{{Entity "{CampaignScnBuilder._escape(entity)}" {object_id}',
                "\t\t{Position 0 0}",
                '\t\t{TexMod "auto"}',
                "\t\t{SpawnedInFog}",
                "\t\t{Player 0}",
                f"\t\t{{MID {int(object_id, 16)}}}",
                "\t}",
            ]
        )

    @staticmethod
    def _empty_inventory(object_id: str) -> str:
        return "\n".join(
            [
                f"\t{{Inventory {object_id}",
                "\t\t{box",
                "\t\t\t{clear}",
                "\t\t}",
                "\t}",
            ]
        )

    def _resolve_breed_path(self, breed: str, side: str, period: str) -> str:
        direct_candidates = [
            self.root / f"resource/set/breed/mp/{side}/{breed}.set",
            self.root / f"resource/set/breed/mp/{period}/{side}/{breed}.set",
            self.root / f"resource/set/breed/{breed}.set",
        ]
        for candidate in direct_candidates:
            if candidate.is_file():
                return candidate.relative_to(self.root / "resource").as_posix()
        if self._breed_index is None:
            self._breed_index = {}
            breed_root = self.root / "resource/set/breed"
            if breed_root.is_dir():
                for path in breed_root.rglob("*.set"):
                    self._breed_index.setdefault(
                        path.stem.lower(),
                        path.relative_to(self.root / "resource").as_posix(),
                    )
        result = self._breed_index.get(breed.lower()) if self._breed_index else None
        if result:
            return result
        raise FileNotFoundError(f"Could not resolve Code:X breed file for {breed}")

    @staticmethod
    def _escape(value: str) -> str:
        return value.replace("\\", "\\\\").replace('"', '\\"')


class CampaignScnParser:
    _row = re.compile(r'\{\s*"([^"]+)"\s+"([^"]*)"([^}]*)\}')
    _id = re.compile(r"0x[0-9a-fA-F]+")

    def parse_squads(self, text: str) -> list[ParsedCampaignSquad]:
        block = self._extract_named_block(text, "CampaignSquads")
        if block is None:
            return []
        squads: list[ParsedCampaignSquad] = []
        for match in self._row.finditer(block):
            squads.append(
                ParsedCampaignSquad(
                    unit_name=match.group(1),
                    stage=match.group(2),
                    object_ids=self._id.findall(match.group(3)),
                )
            )
        return squads

    @staticmethod
    def _extract_named_block(text: str, name: str) -> str | None:
        match = re.search(r"\{\s*" + re.escape(name) + r"\b", text)
        if not match:
            return None
        start = match.start()
        depth = 0
        in_quote = False
        escaped = False
        for index in range(start, len(text)):
            char = text[index]
            if in_quote:
                if escaped:
                    escaped = False
                elif char == "\\":
                    escaped = True
                elif char == '"':
                    in_quote = False
                continue
            if char == '"':
                in_quote = True
            elif char == "{":
                depth += 1
            elif char == "}":
                depth -= 1
                if depth == 0:
                    return text[start : index + 1]
        raise ValueError(f"Unterminated {name} block")

    def survivor_rosters(
        self, text: str, pending: PendingBattle
    ) -> dict[str, list[BattalionRosterEntry]]:
        stage_to_battalion = {
            participant.stage: participant.battalion_id
            for participant in (*pending.attacking_participants, *pending.defending_participants)
        }
        counts: dict[str, dict[str, int]] = {
            battalion_id: {} for battalion_id in stage_to_battalion.values()
        }
        for squad in self.parse_squads(text):
            battalion_id = stage_to_battalion.get(squad.stage)
            if battalion_id is None:
                continue
            unit_counts = counts[battalion_id]
            unit_counts[squad.unit_name] = unit_counts.get(squad.unit_name, 0) + 1
        return {
            battalion_id: [
                BattalionRosterEntry(unit_name=name, quantity=quantity)
                for name, quantity in sorted(unit_counts.items())
            ]
            for battalion_id, unit_counts in counts.items()
        }
