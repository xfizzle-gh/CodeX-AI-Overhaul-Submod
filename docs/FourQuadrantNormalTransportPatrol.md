# Four-Quadrant Normal Transport Patrol

## Decision

Use ordinary AI-controlled troop transports rather than timer-driven insertion lifecycles. The game AI keeps linked passengers seated during movement and unloads them naturally under contact. There is no scripted passenger emit, turnaround, withdrawal, deletion, or post-dismount cleanup.

## Mission coverage

Exactly one friendly and one enemy transport can activate in either mission perspective:

| Player mission | Friendly path | Enemy path |
|---|---|---|
| Attack | `attack_support_waves.inc` | `enemy_defense_support.inc` |
| Defense | `defense_support_waves.inc` | `enemy_attack_support.inc` |

The four engines are mutually gated by `user_is_defender$`, so one mission receives exactly two transport packages.

## Faction vehicles

- Russia: `ural`
- Ukraine: `ural_vsu`
- NATO: `fmtv`
- PRC: `shaanxi_sx2190_passenger`

Each package contains the truck, two cab crew, and eight linked passenger seats.

## Route behavior

Each of the fourteen CWA conquest maps receives five generated route waypoints named:

- `transport_patrol_flag_1`
- `transport_patrol_flag_2`
- `transport_patrol_flag_3`
- `transport_patrol_flag_4`
- `transport_patrol_flag_5`

A route waypoint is centered 320 map units from its campaign flag and uses a radius of 140 map units. The closest requested arrival point therefore remains approximately 180 map units from the flag center, preventing trucks from driving directly into the flag post or sandbags.

Maps contain two to five campaign flags. On maps with fewer than five flags, later route slots revisit flags from rotated perimeter angles rather than duplicating the same destination.

The truck receives a new route point every 45 seconds while at least one tagged passenger remains within 80 map units of the hull. Once normal AI unloads the squad and it moves away, the proximity condition stops matching and no further patrol order is issued. The truck and infantry then remain under ordinary combat AI.

## Disabled legacy behavior

The active transport paths do not use:

- timer-driven passenger emit;
- fixed 60- or 75-second dismount timing;
- forced turnaround;
- return-to-entry orders;
- scripted truck deletion;
- scripted post-dismount cleanup.

The old motor-test triggers and scripted motor budgets are disabled where applicable.

## Deployment

```powershell
cd "E:\Steam\steamapps\workshop\content\400750\CodeX AI Overhaul Submod"
git fetch origin
git switch experiment/defense-transport-control-comparison
git pull --ff-only origin experiment/defense-transport-control-comparison
powershell -ExecutionPolicy Bypass -File .\tools\deploy_transport_control_comparison.ps1 -WorkshopRoot "E:\Steam\steamapps\workshop\content\400750\3636883799"
```

## Runtime validation matrix

Test at least one player-attack and one player-defense mission. Each should produce exactly two trucks: one friendly and one enemy. Confirm that trucks follow flag-perimeter routes, avoid the central flag props, and stop receiving patrol reorders after their passengers dismount under contact.
