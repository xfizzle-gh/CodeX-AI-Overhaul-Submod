# Allied Support Command — Design

**Date:** 2026-08-17
**Status:** Approved
**Scope:** Q1 (human attack, friendly support) only in production. Q2 reuse architected, not wired. Q3/Q4 untouched.

## Goal

Add friendly support units that do not come from the player's campaign roster, hand them
automatically to the Team A AI mate, and have a new commander move them toward objectives.

The chain to prove, in order:

```
new actor birth
  -> human ownership
  -> ISOLATED FoW CHECKPOINT (gate)
  -> automatic Mate handoff
  -> FoW parity retained
  -> commander intake
  -> movement / objective behaviour
```

## Naming

No TMAI-derived string appears anywhere in this work — not in file names, trigger names,
variables, tags, or comments.

Code:X already owns the bare `allied_support_*` namespace: `allied_support_initialized`,
`allied_support_waves_left`, `allied_support_wave_size`, `allied_support_target`,
`allied_support_busy`, `allied_support_wave_num` in `resource/map/multi/ce/ce_vars.inc:19`,
and the tag `allied_support_template` in
`resource/map/multi/ce/map_setup/ce_map_setup_triggers.inc:1390`. Those are upstream CE
names, not ours. All new work therefore lives in a `_cmd_` sub-namespace that cannot collide.

| Kind | Name |
|---|---|
| Birth triggers | `resource/map/multi/allied_support_birth.inc` |
| Handoff triggers | `resource/map/multi/allied_support_handoff.inc` |
| Commander triggers | `resource/map/multi/allied_support_command.inc` |
| Strategy publisher | `resource/script/multiplayer/modes/allied_support_command.lua` |
| Variables | `allied_support_cmd_*` |
| Tags | `allied_support_cmd_*` |

## Terminology

"Human ownership" and "native Dynamic Conquest registration" are **not** the same thing and
this document never conflates them. Setting an actor's owner to the human player id is an
ownership operation only. Whether a cloned actor acquires genuine Conquest registration —
and therefore terrain fog-of-war illumination — is the open question under test. The FoW
checkpoint below is the only evidence that settles it.

## Components

### `allied_support_birth.inc` — actor birth

Consumes a composition request from the existing Q1 wave clock. Births via:

```
{"actor_to_waypoint"
    {selector ... parked prototype pool ... {amount N}}
    {waypoint "<birth pad>"}
    {clone}
    {approach "safe teleport & rotate"}
}
```

The birth waypoint's own `{commands}` block executes on the **arriving** clone and applies
`{tag_add allied_support_cmd_fresh}`. This is the mechanism that defeats the wall which
killed three earlier promote designs: a freshly created entity's provenance is invisible to
every selector this format can express, so the design never selects one — the waypoint tags
it on arrival instead.

Prototype originals never move, so birth is infinitely repeatable and consumes nothing from
the player's roster. Source pools (`attack_support_templates.inc`,
`faction_support_templates.inc`) are reused unchanged.

Precedent for the transport: `resource/map/multi/HF-dcg_script.inc:4877` and `:4938` already
run `{clone}` + `{approach "safe teleport & rotate"}` for the airstrike chain in this mod.

### `allied_support_handoff.inc` — ownership and Mate transfer

1. Select `allied_support_cmd_fresh`, apply the literal 1–16 `{player}` switch onto the
   resolved human player id, retag `allied_support_cmd_human`.
2. **FoW checkpoint gate.** While still human-owned and before any Mate transfer, the actor
   must be observed illuminating terrain on its own. This is a hard gate: a birth mechanism
   that fails here does not become the production path.
3. Settle, then transfer to the Mate by the same idiom — a second literal 1–16 `{player}`
   switch, selecting on `allied_support_cmd_human` and setting the owner to the runtime-resolved
   mate player id — and retag `allied_support_cmd_mate`. The transfer is a tag-driven ownership
   switch, not a Lua call; the mate bot never issues the transfer itself.
4. Re-verify FoW parity after transfer.

These actors are **never** tagged `_lua_mi` or `_lua_ignore`. Either tag places them in the
commander's exclude set permanently and makes them uncommandable.

The mate player id is resolved at runtime and fed to the literal switch. It is never
hardcoded — lobby slot assignment varies between runs, which previously made proofs look
contradictory when identical files behaved differently.

### `allied_support_command.inc` — the commander

Intake is by ownership, not by prior tagging:

- include: mate-owned, in `gamezone`, `prop human` or `prop vehicle`, `state operatable`
- exclude: own lifecycle tags, `_lua_mi`, `_lua_ignore`, `_animal`, `repairing`,
  `user_control`, `dead`, `inactive`, `linked`

Flag targeting uses the CWA idiom only: `flag_point_campaign_N` entities carrying engine tag
`flag`, always with `{exclude {state {state inactive}}}` because roughly two of five map
points are active per mission. **No `fpc1`–`fpc5` zone or tag is referenced anywhere** —
those are Indomitus/40k names and do not exist on the fourteen CWA maps. Hold-at-captured-flag
therefore tests proximity to the flag entity rather than zone containment.

Behaviours: ~3s settle before first order, small infantry groups, individual vehicle tasking,
objectives distributed rather than stacked, counterattack recently lost flags, hold recently
captured flags, keep reserves, and no order spam (re-order on state change, not on a timer).

Q2 reuse: mission side, owning player id, and target flag set enter as variables, never as
literals. Q2 binds different inputs later without touching commander logic. Q2 is not wired
in this build.

### `allied_support_command.lua` — strategy publisher

Runs on the mate slot. Reads `Scene.Flags`, publishes `allied_support_cmd_*` mission
variables one-way via `SetVar` behind a fail-closed magic value: the commander treats the bus
as invalid unless every payload write in a publication succeeded.

Constraints:

- No BotApi squad commands. Units delivered this way have not been observed in
  `Scene.Squads`, so command stays MI-side.
- Never read `spawnPointName` or `PlayerSpawnPoint`. On the extra Team A slot these
  null-deref natively (access violation in `lua.start`) and `pcall` cannot catch it.
  Fields proven safe on that slot: `playerId`, `team`, `army`, `difficulty`, `gameMode`,
  `isHuman`, `Attacking`, `FirstPlayerId`, `FirstEnemyId`, `DefenderBotId`.
- Never identify the human via `FirstPlayerId` — it can point at the Team A AI bot. Use
  `isHuman`.

### Single-commander requirement

While the new commander is under test, the existing direct-order path in
`resource/script/multiplayer/modes/attack_support.lua` is disabled so that exactly one
component issues orders:

- `orderNewSquads()` (`:180`), called every quant at `:237`
- the periodic re-order of every squad every 400 quants at `:238`–`:245`
- `orderSquad()` (`:163`) and its `CaptureFlag` / `SeekAndDestroy` calls at `:167` and `:174`

Identity publication, the engine-state mirror, and all logging are **kept**.

Rationale: this path is likely inert today because MI-delivered units have not appeared in
`Scene.Squads`. But altering registration is the explicit purpose of the new birth path, so
the path could activate exactly when a second commander must not exist.

### `{"spawn"}` side probe

`{"spawn" {entity "<breed>"} {waypoint "N"}}` is a true creation primitive requiring no
source prototype. Every shipped use — in this mod, Code:X, and vanilla — births only
non-combat helpers (`conquest_spawn_helper`, `conquest_spawn_indicator`,
`artillery_barrage_rocket`), and the action carries no `{player}` parameter in any precedent.

Kept as a throwaway probe: one map, behind a variable defaulting to 0. It becomes the
production birth path only if it passes the same isolated FoW checkpoint as candidate A.
Neither candidate is production until that gate passes.

## Flow

```
Q1 wave clock (existing, unchanged)
    -> birth request
    -> clone to birth pad, {commands} tags arrival
    -> 1-16 {player} switch -> human owner
    -> [FoW CHECKPOINT: isolated actor must illuminate terrain]
    -> settle -> automatic Mate handoff
    -> [FoW re-verify: illumination retained]
    -> commander intake -> move / objective orders
                              ^
    Lua brain (mate slot) -- SetVar --+
```

Unchanged and reused: wave schedule, composition pools, budgets, live-unit cap, entry pads,
`attack_support_templates.inc`, `faction_support_templates.inc`.

## Retirement

Q1's existing direct-command logic goes behind a kill variable first and is deleted only
after the full chain passes live. No duplicate support system runs concurrently: when birth
switches over, the old `{"placement"}` path is off, not parallel.

## Error handling

- Commander issues nothing unless the strategy bus magic is valid.
- Birth does nothing unless the mate id resolved and the existing wave gates pass.
- Live-unit cap retained from the current engine.
- If handoff fails, the actor remains human-owned and simply goes uncommanded. No orphan
  and no partially-owned actor.

## Verification

### Structural

- chosen numeric waypoint band provably unused across all fourteen maps
- zero TMAI-derived strings
- zero `fpc` references
- no `_lua_mi` or `_lua_ignore` applied to our actors
- all fourteen maps patched idempotently

### Live, staged on one map

Each stage is a distinct observable, in order:

1. clone appears at the birth pad
2. owner reads as the human
3. **isolated FoW checkpoint** — that actor alone illuminates terrain (hard gate)
4. owner reads as the Mate
5. FoW illumination retained after transfer
6. commander intake logged
7. movement and objective behaviour observed

A stage-3 failure stops the birth candidate from becoming production, regardless of whether
later stages would pass.

## Risks

- **Infantry-squad `{clone}` is unproven.** Aircraft and their passengers are verified; a
  rifle team is not. Largest assumption in the design.
- **Whether a birth-pad `{commands}` block fires for an arriving infantry actor** is verified
  only for aircraft.
- **Numeric waypoint ids occupy a small engine id space and a collision crashes every map at
  load.** The band must be proven unused before injection. Note that a grep for numeric
  waypoint declarations in the CWA maps returned nothing while the airstrike chain plainly
  uses `"0"` and `"6"`, so the declaration form is not yet understood and must be
  established before a band is chosen.
- **Mate playerId varies by lobby slot** and must be resolved at runtime.

## Out of scope

Two pre-existing defects in the path being disabled, recorded but not fixed here:

- `pickFlagName()` at `attack_support.lua:150` selects a uniformly random flag with no
  `inactive` exclusion, though only ~2 of 5 map points are active per mission.
- The same function uses `math.random`, giving non-deterministic order targets.
