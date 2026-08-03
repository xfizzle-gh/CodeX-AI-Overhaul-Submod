# Gates of Europa interoperability findings

This document records behavior observed from the user-supplied Workshop archive so Gates of CodeX can implement a clean-room compatibility layer.

## Distribution structure

The Workshop item contains a minimal GoH patch and a separate Unity desktop application. The Unity application uses the Mono managed scripting backend. Its campaign behavior is therefore structurally observable through .NET metadata and runtime data files.

No original executable, Unity asset, or decompiled implementation is committed to this repository.

## Recovered bridge contract

The original application maintains campaign state outside GoH, then exchanges two files through a Dynamic Conquest save archive:

- `status`, containing the battle configuration, faction codes, map point, difficulty, resources, research unlocks, and win counters
- `campaign.scn`, containing persistent Human/Entity object graphs, Inventory blocks, and a `CampaignSquads` stage map

The archive is emitted as `campaign.sav`. After GoH updates that save, the campaign application reads:

- `playedGames` and `wonGames` from `status`
- surviving and captured objects from `campaign.scn`

The observed application also writes a short-lived Lua context containing a battle ID and result path. Gates of CodeX treats this as optional because Code:X's existing Dynamic Conquest flow already updates the save archive.

## Observed status template fields

The original template includes:

- version, gameVersion, timestamp, seed, and save name
- army and enemyArmy
- MP, SP, AP, and RP
- difficulty, duration, resources, fog of war, and manual-control mode
- selected map point, attacking state, region, playedGames, and wonGames
- unlockedResearch
- two map-point records describing the selected battle map

## Observed campaign.scn requirements

The original exporter validates that:

- a `CampaignSquads` block exists
- every referenced object ID has a corresponding Human or Entity block
- every Human or Entity ID has an Inventory block
- IDs are unique

Infantry and vehicle templates are derived from Code:X/GoH conquest unit definitions, breed files, and entity inventories. Gates of CodeX reimplements that materialization from installed game data.

## Code:X integration rule

Gates of CodeX must not replace Code:X's `conquest.lua` or `utility.lua`. The existing Code:X scripts remain authoritative for AI purchasing, waves, doctrines, mission variables, and capture behavior. The compatibility layer only generates and imports Dynamic Conquest save state.
