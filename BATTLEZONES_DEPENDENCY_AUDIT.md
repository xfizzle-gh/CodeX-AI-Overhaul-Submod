# Self-contained Code:X Battle Zones

This overlay owns the complete Battle Zones definition chain needed by the four supported Code:X factions.

## Factions

- Eastern: `rusa`, `prc`
- Western: `ukr`, `nato`

The four army IDs are copied unchanged from Code:X:

- `rusa = 0`
- `ukr = 1`
- `nato = 2`
- `prc = 3`

## Loader chain

`battle_zones.set`
→ `presets_battlezones.inc`
→ `preset_codex_battlezones.inc`
→ `only_roster_codex_battlezones.set`
→ `roster_codex_battlezones.set`
→ `battlezones_codex/settings.set`
→ four infantry tables
→ four unit tables

The Battle Zones stack now follows Modern Conflict's working skirmish contract:

- `armySelectionMode "alliance"` expands each alliance's repeated `armies` entries into the per-slot nation selector.
- `unitMode "conquest"` remains the mode identity so bot purchasing resolves the four copied `conquest.<army>.lua` modules.
- The player unit filter is `2022s^conquestonly|doctrineonly`.
- Imported purchase templates are retagged from `conquestonly` to normal skirmish `all` units while retaining `conquest` for the mode and bot tables.
- Ukrainian infantry is retagged from the mismatched `era1960` token to `2022s`.

## Imported parent files

- Code:X army definitions for all four factions
- Code:X conquest unit-definition settings
- Code:X infantry and unit tables for all four factions
- Code:X bot purchase modules for all four factions
- Code:X `battlezones.lua` and its required `utility.lua`
- Battle Zones preset support includes copied from the working Shattered Galaxy structure

## Parent syntax correction

The copied Ukrainian infantry table fixes the three extra closing parentheses on:

- `ukr21_medic`
- `demon_seniorrifleman`
- `kra_sniper`

The copied bot purchase tables also remove ten active purchase rows whose unit IDs are absent from the four imported rosters. This prevents a Battle Zones bot from waiting on or attempting to spawn an unavailable unit:

- Russia: `squad_rus90_bmp1(rusa)`, `sto_22_2(rusa)`, `sto_22_3(rusa)`
- Ukraine: `47th_inf_sniper(ukr)`, `47th_inf_sniper_m107(ukr)`, `2s22_bohdana`
- NATO: `lav25`, `vilkas`, `wiesel1_gun`, `wiesel1_tow`

No Code:X multiplayer set or script file required by this Battle Zones chain remains inherited from the parent mod.

The obsolete `codex2022` smoke-test chain and its earlier `roster_2022s` wrapper are removed so they cannot be included accidentally. Code:X still supplies the underlying entity, model, texture, and sound assets referenced by these imported unit tables.
