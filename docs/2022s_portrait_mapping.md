# 2022s skirmish portrait mapping

The skirmish rebuild preserves Code:X unit IDs wherever possible so the parent mod's `_00` through `_03` interface images resolve without duplicate art.

## Normal squads and detachments

Normal purchases resolve against Code:X files under:

```text
resource/interface/scene/portrait_squad/<unit-id>_00.png
resource/interface/scene/portrait_squad/<unit-id>_01.png
resource/interface/scene/portrait_squad/<unit-id>_02.png
resource/interface/scene/portrait_squad/<unit-id>_03.png
```

Every normal unit introduced by Batches 1 through 3 was checked against the supplied Code:X `scene.zip`. The unit definitions retain the corresponding Code:X basename.

Code:X capitalizes the initial `Squad_` in several NATO image filenames while the unit IDs use lowercase `squad_`. Gates of Hell on Windows resolves those filenames case-insensitively, matching the parent mod's existing convention.

## Doctrine units

Doctrine purchases use Code:X artwork under `resource/interface/scene/unit_icon`.

| Faction | Doctrine unit ID | Code:X image basename |
| --- | --- | --- |
| NATO | `doctrine(nato)` | `doctrine(nato)` |
| NATO | `doctrine_vehicle_m2a3(nato)` | `doctrine_vehicle_m2a3(nato)` |
| Ukraine | `doctrine(ukr)` | `doctrine(ukr)` |
| Ukraine | `doctrine_squad_47th(ukr)` | `doctrine_squad_47th(ukr)` |
| Russia | `doctrine(rusa)` | `doctrine(rusa)` |
| Russia | `doctrine_squad_dsh(rusa)` | `doctrine_squad_dsh(rusa)` |
| PRC | `doctrine_squad_skirmish_prc(prc)` | generated from `squad_pla112_rifle(prc)` |
| PRC | `doctrine_squad_skirmish_prc_139(prc)` | generated from `squad_pla139_rifle(prc)` |

Code:X does not provide native PRC doctrine images under `unit_icon`. Run the included generator after checking out Batch 4:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\install_prc_doctrine_portraits.ps1
```

The script reads the existing Code:X PLA artwork from Workshop item `3261086933`, crops it to the Code:X doctrine-icon size of 144 by 72, and writes all four UI states to the final-loading submod using the exact doctrine IDs above.

A different Steam library path can be supplied explicitly:

```powershell
powershell -ExecutionPolicy Bypass -File .\tools\install_prc_doctrine_portraits.ps1 `
  -CodeXResource "D:\SteamLibrary\steamapps\workshop\content\400750\3261086933\resource"
```
