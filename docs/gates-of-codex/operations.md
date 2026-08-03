# Gates of CodeX operations

## Save ownership

Gates of CodeX owns the strategic `campaign.json`. Gates of Hell owns the exported `campaign.sav` while a tactical battle is in progress. The `.goc.json` manifest beside the save binds the tactical result to one pending strategic battle.

Never import a save whose manifest battle ID differs from the pending battle ID in the campaign state.

## Code:X compatibility

The application does not replace `resource/script/multiplayer/modes/conquest.lua`, `utility.lua`, or Code:X purchase lists. It creates Dynamic Conquest state and lets Code:X remain responsible for tactical AI, doctrine purchases, wave timing, capture behavior, and mission scripting.

A catalog signature is recorded at export time. Code:X updates do not invalidate old campaign JSON files, but a major unit-definition change can make an in-progress tactical save incompatible. Finish or cancel pending battles before updating Code:X when possible.

## Recovery

Campaign JSON writes are atomic. If a tactical battle is abandoned, retain the campaign JSON and remove the pending save and manifest. The pending battle can then be auto-resolved or exported again.

## Live validation checklist

1. Run `gates-of-codex doctor` and confirm the game, profile, and Code:X paths.
2. Run `gates-of-codex scan --codex <path>` and confirm all four modern factions appear in the catalog.
3. Create a fresh campaign and export one NATO versus Russia battle.
4. Confirm GoH opens the generated save with Code:X enabled.
5. Confirm infantry have weapons, vehicle crews are assigned, and both stages spawn correctly.
6. Complete the battle and import it.
7. Confirm destroyed squads disappear, survivors persist, the loser retreats or is removed, and province ownership changes only on an attacker victory.
