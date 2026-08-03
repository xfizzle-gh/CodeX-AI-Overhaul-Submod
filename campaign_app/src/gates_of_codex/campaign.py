from __future__ import annotations

import random
import uuid
from dataclasses import dataclass

from .models import (
    Battalion,
    BattleParticipant,
    CampaignState,
    Faction,
    PendingBattle,
)


@dataclass(frozen=True, slots=True)
class MoveResult:
    moved: bool
    pending_battle: PendingBattle | None = None


class CampaignEngine:
    TURN_ORDER = (Faction.NATO, Faction.UKRAINE, Faction.RUSSIA, Faction.PRC)

    def __init__(self, state: CampaignState, *, random_seed: int | None = None) -> None:
        state.validate()
        self.state = state
        self._random = random.Random(random_seed)

    def move_or_attack(self, battalion_id: str, target_province_id: str) -> MoveResult:
        if self.state.pending_battle is not None:
            raise RuntimeError("Resolve or cancel the pending battle before moving another battalion")
        battalion = self._get_battalion(battalion_id)
        target = self._get_province(target_province_id)
        origin = self._get_province(battalion.province_id)

        if battalion.movement_remaining <= 0:
            raise ValueError(f"Battalion {battalion_id} has no movement remaining")
        if target_province_id not in origin.neighbors:
            raise ValueError(f"Province {target_province_id} is not adjacent to {origin.province_id}")

        defender = self._battalion_in(target_province_id)
        if target.owner in (battalion.faction, Faction.NEUTRAL) and defender is None:
            battalion.province_id = target_province_id
            battalion.movement_remaining -= 1
            if target.owner == Faction.NEUTRAL:
                target.owner = battalion.faction
            return MoveResult(moved=True)

        if target.owner == battalion.faction:
            if defender is not None:
                raise ValueError(
                    f"Friendly province {target_province_id} already contains battalion "
                    f"{defender.battalion_id}"
                )
            battalion.province_id = target_province_id
            battalion.movement_remaining -= 1
            return MoveResult(moved=True)

        if battalion.combat_actions_remaining <= 0:
            raise ValueError(f"Battalion {battalion_id} has no combat actions remaining")

        pending = self._build_pending_battle(battalion, defender, target_province_id)
        self.state.pending_battle = pending
        return MoveResult(moved=False, pending_battle=pending)

    def auto_resolve_pending_battle(self) -> Faction:
        pending = self._require_pending_battle()
        attacker = self._get_battalion(pending.attacking_participants[0].battalion_id)
        defender = (
            self._get_battalion(pending.defending_participants[0].battalion_id)
            if pending.defending_participants
            else None
        )
        target = self._get_province(pending.target_province_id)

        attacker_score = self._combat_score(attacker)
        defender_score = self._combat_score(defender) if defender else 1.0
        defender_score *= 1.0 + min(target.fortification, 5) * 0.12
        attacker_chance = attacker_score / max(attacker_score + defender_score, 1.0)
        winner = attacker.faction if self._random.random() < attacker_chance else target.owner
        self.apply_battle_result(winner)
        return winner

    def apply_battle_result(self, winner: Faction) -> None:
        pending = self._require_pending_battle()
        attacker = self._get_battalion(pending.attacking_participants[0].battalion_id)
        defender = (
            self._get_battalion(pending.defending_participants[0].battalion_id)
            if pending.defending_participants
            else None
        )

        if winner == pending.attacker_faction:
            if defender is not None:
                self._apply_percentage_losses(defender, 0.65)
            self._apply_percentage_losses(attacker, 0.25)
        else:
            self._apply_percentage_losses(attacker, 0.55)
            if defender is not None:
                self._apply_percentage_losses(defender, 0.20)

        self._finalize_pending_battle_positions(winner)

    def apply_external_battle_result(
        self, winner: Faction, survivors: dict[str, list]
    ) -> None:
        pending = self._require_pending_battle()
        participant_ids = {
            participant.battalion_id
            for participant in (
                *pending.attacking_participants,
                *pending.defending_participants,
            )
        }
        for battalion_id, roster in survivors.items():
            if battalion_id not in participant_ids:
                raise ValueError(
                    f"Survivor roster references non-participant battalion {battalion_id}"
                )
            battalion = self._get_battalion(battalion_id)
            battalion.roster = list(roster)
        self._finalize_pending_battle_positions(winner)

    def _finalize_pending_battle_positions(self, winner: Faction) -> None:
        pending = self._require_pending_battle()
        attacker = self._get_battalion(pending.attacking_participants[0].battalion_id)
        defender = (
            self._get_battalion(pending.defending_participants[0].battalion_id)
            if pending.defending_participants
            and pending.defending_participants[0].battalion_id in self.state.battalions
            else None
        )
        target = self._get_province(pending.target_province_id)

        if winner == pending.attacker_faction:
            if defender is not None:
                if defender.is_destroyed:
                    self.state.battalions.pop(defender.battalion_id, None)
                else:
                    retreat = self._find_retreat_province(
                        defender, excluding=target.province_id
                    )
                    if retreat is None:
                        self.state.battalions.pop(defender.battalion_id, None)
                    else:
                        defender.province_id = retreat
            if attacker.is_destroyed:
                self.state.battalions.pop(attacker.battalion_id, None)
            else:
                attacker.province_id = target.province_id
                target.owner = attacker.faction
        else:
            if attacker.is_destroyed:
                self.state.battalions.pop(attacker.battalion_id, None)
            if defender is not None and defender.is_destroyed:
                self.state.battalions.pop(defender.battalion_id, None)

        if attacker.battalion_id in self.state.battalions:
            attacker.movement_remaining = 0
            attacker.combat_actions_remaining = max(
                0, attacker.combat_actions_remaining - 1
            )
        pending.completed = True
        self.state.pending_battle = None
        self.state.validate()

    def end_turn(self) -> Faction:
        if self.state.pending_battle is not None:
            raise RuntimeError("Cannot end turn while a battle is pending")

        active = [
            faction
            for faction in self.TURN_ORDER
            if faction.value in self.state.factions
            and not self.state.factions[faction.value].is_eliminated
        ]
        if not active:
            raise RuntimeError("Campaign has no active factions")

        try:
            index = active.index(self.state.current_faction)
        except ValueError:
            index = -1
        next_faction = active[(index + 1) % len(active)]
        if index == len(active) - 1 or index == -1:
            self.state.turn_number += 1
            self._grant_income()
            self._reset_battalions()
        self.state.current_faction = next_faction
        return next_faction

    def _grant_income(self) -> None:
        for faction in self.TURN_ORDER:
            faction_state = self.state.factions.get(faction.value)
            if faction_state is None or faction_state.is_eliminated:
                continue
            faction_state.resources += sum(
                province.resource_yield
                for province in self.state.provinces.values()
                if province.owner == faction
            )

    def _reset_battalions(self) -> None:
        for battalion in self.state.battalions.values():
            battalion.movement_remaining = 1
            battalion.combat_actions_remaining = 1
            battalion.supply = min(100, battalion.supply + 20)

    def _build_pending_battle(
        self, attacker: Battalion, defender: Battalion | None, target_province_id: str
    ) -> PendingBattle:
        battle_id = f"goc-{self.state.turn_number}-{uuid.uuid4().hex[:10]}"
        defenders = []
        defender_faction = self.state.provinces[target_province_id].owner
        if defender is not None:
            defenders.append(
                BattleParticipant(
                    battalion_id=defender.battalion_id,
                    faction=defender.faction,
                    stage="stage_2",
                    is_primary=True,
                )
            )
            defender_faction = defender.faction
        return PendingBattle(
            battle_id=battle_id,
            origin_province_id=attacker.province_id,
            target_province_id=target_province_id,
            attacker_faction=attacker.faction,
            defender_faction=defender_faction,
            attacking_participants=[
                BattleParticipant(
                    battalion_id=attacker.battalion_id,
                    faction=attacker.faction,
                    stage="stage_1",
                    is_primary=True,
                )
            ],
            defending_participants=defenders,
            player_faction=self.state.selected_faction,
            player_is_attacker=attacker.faction == self.state.selected_faction,
        )

    def _combat_score(self, battalion: Battalion | None) -> float:
        if battalion is None:
            return 1.0
        category_weights = {
            "infantry": 1.0,
            "recon": 0.8,
            "vehicle": 1.7,
            "ifv": 2.0,
            "tank": 3.0,
            "artillery": 2.2,
            "air_defense": 1.8,
            "unknown": 1.0,
        }
        base = sum(
            entry.quantity * category_weights.get(entry.category, 1.0)
            for entry in battalion.roster
        )
        supply_factor = 0.4 + battalion.supply / 100 * 0.6
        experience_factor = 1.0 + min(battalion.experience, 1000) / 5000
        return max(base * supply_factor * experience_factor, 0.1)

    @staticmethod
    def _apply_percentage_losses(battalion: Battalion, fraction: float) -> None:
        for entry in battalion.roster:
            losses = int(entry.quantity * fraction + 0.5)
            entry.quantity = max(0, entry.quantity - losses)
        battalion.roster = [entry for entry in battalion.roster if entry.quantity > 0]

    def _find_retreat_province(self, battalion: Battalion, excluding: str) -> str | None:
        current = self._get_province(battalion.province_id)
        candidates = [
            neighbor_id
            for neighbor_id in current.neighbors
            if neighbor_id != excluding
            and self.state.provinces[neighbor_id].owner == battalion.faction
            and self._battalion_in(neighbor_id) is None
        ]
        return sorted(candidates)[0] if candidates else None

    def _get_battalion(self, battalion_id: str) -> Battalion:
        try:
            return self.state.battalions[battalion_id]
        except KeyError as exc:
            raise KeyError(f"Unknown battalion: {battalion_id}") from exc

    def _get_province(self, province_id: str):
        try:
            return self.state.provinces[province_id]
        except KeyError as exc:
            raise KeyError(f"Unknown province: {province_id}") from exc

    def _battalion_in(self, province_id: str) -> Battalion | None:
        return next(
            (
                battalion
                for battalion in self.state.battalions.values()
                if battalion.province_id == province_id
            ),
            None,
        )

    def _require_pending_battle(self) -> PendingBattle:
        if self.state.pending_battle is None:
            raise RuntimeError("There is no pending battle")
        return self.state.pending_battle
