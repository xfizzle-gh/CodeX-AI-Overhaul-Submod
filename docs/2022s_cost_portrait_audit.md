# 2022s Battle Zones cost and portrait audit

## Cost behavior

All curated Battle Zones infantry formations use the same member compositions as their Code:X conquest definitions. The squad and detachment templates both use:

```set
{cost 0}
{squad_cost_factor 1}
```

That means the displayed MP price is calculated from the inherited conquest cost of every soldier in the formation. There is no separate Battle Zones price table and no manual discount or markup.

| Faction | Unit | Calculated MP | Conquest parity |
|---|---|---:|---|
| NATO | `squad_usmc_eng(nato)` | 280.0 | Exact composition |
| NATO | `squad_inf2_rifle_m3(nato)` | 239.0 | Exact composition |
| NATO | `squad_usmc_weapon_at(nato)` | 101.0 | Exact composition |
| NATO | `squad_usmc_mg(nato)` | 139.5 | Exact composition |
| NATO | `arf_medic(nato)` | 112.0 | Same four `nato_medic` members |
| Ukraine | `ter_22_1(ukr)` | 70.0 | Exact composition |
| Ukraine | `93th_alcatraz_rifle(ukr)` | 150.5 | Exact composition |
| Ukraine | `47th_inf_rifle(ukr)` | 157.5 | Exact current conquest calculation |
| Ukraine | `93th_alcatraz_mg_pkm(ukr)` | 147.0 | Exact composition |
| Ukraine | `ukr_22_5(ukr)` | 42.0 | Same four `ukr_medic` members |
| Russia | `rus90_inf_rifle(rusa)` | 196.0 | Exact composition |
| Russia | `rus90_inf_assault(rusa)` | 210.0 | Exact composition |
| Russia | `rus90_inf_mg(rusa)` | 126.0 | Exact composition |
| Russia | `rus90_inf_at(rusa)` | 98.0 | Exact composition |
| Russia | `rus_22_5(rusa)` | 56.0 | Same four `rus_medic` members |
| PRC | `squad_pla112_rifle(prc)` | 163.8 | Exact composition |
| PRC | `squad_pla112_rifle_dzj08(prc)` | 171.7 | Exact composition |
| PRC | `squad_pla112_mg(prc)` | 99.4 | Exact composition |
| PRC | `squad_pla112_pf98(prc)` | 103.6 | Exact composition |
| PRC | `squad_pla112_recon(prc)` | 146.8 | Exact composition |

### Known inherited anomaly

Code:X uses `ukr47_antitank_rpg7` in `47th_inf_rifle`, but its conquest infantry cost file does not contain a corresponding cost row. The game therefore calculates the squad at 157.5 MP, with that member contributing no additional MP. This submod preserves the current conquest price rather than silently inventing a Battle Zones-only value. A later balance change can explicitly price that breed if desired.

Some curated units are available earlier in Battle Zones than their conquest research stage. That affects availability, not MP price:

- `squad_usmc_mg`: conquest stage 3, Battle Zones stage 1
- `squad_pla112_mg`: conquest stage 3, Battle Zones stage 1
- `squad_pla112_pf98`: conquest stage 4, Battle Zones stage 1

## Portrait diagnosis

Code:X contains correct four-state portraits for every curated unit under:

```text
resource/interface/scene/portrait_squad/
```

Those assets are vertical 91 by 114 conquest portraits. Battle Zones purchase cards resolve horizontal 144 by 72 assets from:

```text
resource/interface/scene/unit_icon/
```

None of the current curated squad IDs had matching `unit_icon` files, so Battle Zones could not display the unit-specific art even though the conquest portraits existed.

This change adds exact-name 144 by 72 `unit_icon` assets for all currently curated units. Each icon is derived from that unit's own Code:X conquest portrait. The same image is supplied for states `_00` through `_03` so normal, hover, selected, and disabled lookups all resolve instead of falling back or appearing mismatched.
