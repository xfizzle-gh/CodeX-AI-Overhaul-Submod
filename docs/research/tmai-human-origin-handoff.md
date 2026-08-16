# TMAI human-origin automatic handoff

## Goal

This is the direct implementation of the desired TMAI-style lifecycle for friendly attack support:

1. Extra support troops come from the mod's support pool, not from the player's Dynamic Conquest purchase roster.
2. The support pool is assigned to the real human commander before support waves are allowed to arm.
3. A selected wave enters the battlefield as human-owned troops.
4. The deployed batch is briefly put into user-control state, representing the same pre-transfer state a normal player-owned TMAI unit has.
5. The batch is automatically transferred to the extra Team-A Mate AI.
6. The Mate-owned batch gets TMAI's settle period.
7. Mission-script `action move` orders then distribute the force in small infantry groups and task vehicles independently.

There is no manual transfer button and the player does not buy these troops from the campaign roster.

## Why this is different from rejected PR #98

PR #98 proved that simply changing ownership late is not enough. Its support actors began as pre-authored Player-0 mission actors and were transferred directly to the human during final deployment, immediately returning to AI control. Terrain FoW remained absent.

This experiment moves the human-ownership boundary much earlier:

- the parked support pool is human-owned before `attack_support_ready` may become 1;
- the normal wave engine's first ownership pass also points at the human;
- the visible/deployed actors enter a short human/user-control phase;
- only after that phase are they handed to the Mate.

That is materially closer to the TMAI workflow where an ordinary player-owned unit exists first and is then transferred to the Mate.

## Why this is different from PR #99

PR #99 directly called the DefenderBot's BotApi `SpawnAt`/`Spawn` path for a diagnostic unit. Native testing did not show the expected shared terrain FoW.

This experiment is not another DefenderBot spawn test. It tests the human-origin -> Mate-transfer lifecycle instead.

## Why this is different from PR #100

PR #100 introduced a TMAI-inspired stateful commander but retained BotApi `CaptureFlag` / `SeekAndDestroy` as the final command transport. It did not change the FoW lifecycle.

This experiment replaces that command path for attack support with the TMAI-style one-way split:

- Lua resolves identities and publishes mission variables.
- MI performs the ownership handoff.
- MI `action move` performs the strategic movement.

The enemy Dynamic Conquest AI is untouched.

## Exact runtime flow

### 1. Resolve the Mate

`bot.main.lua` still routes only the extra non-human Team-A process to `attack_support.lua`. The real DefenderBot is explicitly excluded and continues normal Conquest AI.

The routed process's own `playerId` is the Mate destination.

### 2. Resolve the real human commander

With `{aiTeamPlayers 1}`, `BotApi.Conquest.FirstPlayerId` is not trustworthy by itself because the extra Mate can occupy it.

`attack_support.lua` therefore uses the already-shipped `BotApi.Scene:QueryScene({"soldier"}, 5)` primitive directly and narrowly. It gathers player IDs that currently own soldiers and excludes:

- its own Mate ID;
- `FirstEnemyId`;
- `DefenderBotId`.

It then uses `FirstPlayerId` or `hostId` only as corroboration of a candidate returned by the scene query. A single remaining non-bot soldier owner is accepted. Multiple unresolved candidates fail closed rather than guessing.

### 3. Human-seed the hidden support pool

Lua publishes:

- `id_attack_support_human`
- `id_attack_support_mate`
- `id_attack_support = humanId`
- `tmai_handoff_prepare = 1`

It does **not** set `attack_support_ready = 1` yet.

`attack_support_tmai_handoff.inc` sees `tmai_handoff_prepare` and transfers all still-hidden/inactive `attack_support_tpl` actors to the real human using the mission script's literal 1-16 player switch.

It then sets `tmai_handoff_prepared = 1`.

Only after Lua observes that acknowledgement does it enable the handoff and set `attack_support_ready = 1`.

This makes human ownership a hard prerequisite for the wave system rather than a late afterthought.

### 4. Existing support wave selects the extra troops

The existing support wave pool, composition, cadence, level budget, placement pads, and live-unit cap remain in use.

The troops are still separate mod-provided support assets. Nothing is added to the human campaign purchase tables.

When a wave is activated it receives the stable `attack_support_src` tag and is moved onto the battlefield. The existing `am_own_to_support` first ownership pass sees `id_attack_support = humanId`, so the deployed wave stays human-owned.

### 5. Automatic equivalent of the TMAI transfer-button step

The handoff trigger finds newly deployed `attack_support_src` actors that do not yet carry `tmai_handoff_done`.

It marks them `tmai_handoff_pending`, drops their current orders, sets:

- `control user`
- `ai_move disable`

and waits 3 seconds.

That phase is the automated equivalent of having a normal player-owned force immediately before the player presses the transfer button in TMAI.

After the dwell, the same literal 1-16 mission ownership primitive transfers only that pending batch to `id_attack_support_mate`.

The actors are then returned to:

- `control AI`
- `ai_move enable`
- unselectable state

and wait another 3 seconds before strategic orders.

### 6. TMAI-style MI movement

After the post-transfer settle:

- up to four unlinked infantry are assigned to the first small group;
- remaining unlinked infantry form the second group;
- linked vehicle crew are excluded from those infantry groups;
- the four production Humvee instances are addressed independently by their persistent `attack_support_hmmwv1..4` tags;
- three distinct active flags are selected where available;
- final movement uses mission-script `{action move}` rather than BotApi `CaptureFlag` or `SeekAndDestroy`.

The transfer batch is marked `tmai_handoff_done` so it cannot be processed twice.

## Important limitation

The extra support actors are still authored in the mission file as hidden Player-0 prototypes because the current support architecture has no proven way to ask the human client to create arbitrary non-roster units through its native purchase/spawn API.

This experiment assigns those actors to the human while they are still hidden/inactive and before any support wave may activate. That is significantly closer to the TMAI lifecycle than PR #98, but it is not identical to a player purchasing/spawning a brand-new unit through the normal human game UI.

If terrain FoW still fails after this experiment, the remaining line becomes much sharper: the engine likely requires true human-native actor creation/registration, not merely human ownership before battlefield activation and transfer.

## Save/roster risk

Because the hidden support pool is temporarily human-owned, native testing must verify that unused pool actors and deployed support survivors do not leak into the player's Dynamic Conquest survivor roster or save.

This is a required acceptance check, not an assumed property.

## Native test

Use a human-ATTACK Dynamic Conquest battle with allied support enabled.

Check the first two waves if practical.

1. Support still uses the mod's normal extra compositions and does not deduct or consume one of the player's campaign squads.
2. `game.log` resolves distinct human and Mate IDs.
3. The handoff arms only after the human pool seed completes.
4. A deployed support batch automatically moves through the handoff without pressing the transfer button.
5. After handoff, the troops are Mate-controlled and cannot be directly controlled as normal player units.
6. Infantry split into small groups rather than all receiving one blob order.
7. Humvees receive independent MI movement orders.
8. The final command transport is MI `action move`.
9. Move/observe a handed-off support group beyond the player's own line of sight.
10. **Decisive FoW check:** does that transferred group brighten the player's terrain FoW?
11. Enemy contacts remain shared normally.
12. Enemy Conquest purchases and AI remain normal.
13. Complete the battle and verify no support troops appear in the player's post-battle survivor/roster/save state.
14. `game.log` contains no handoff event errors.

## Log markers

Search for:

```text
CODEX_TMAI_HANDOFF:
CODEX_TMAI_HANDOFF: wait
CODEX_TMAI_HANDOFF: armed
CODEX_TMAI_HANDOFF: completed
CODEX_TMAI_HANDOFF: event_error
CODEX_ATTACK_SUPPORT: mirror
```

A healthy resolved line should identify different IDs for `human` and `mate` and report:

```text
flow human_origin_to_mate_to_MI_action_move
```

A completed transfer reports:

```text
order_transport MI_action_move
```

## Interpretation

### FoW YES

This is the desired result. It would strongly support the TMAI lifecycle hypothesis: establish the support actors as human-owned before battlefield activation, then transfer them to the Mate, and keep TMAI-style MI command afterward.

The next work would be hardening save cleanup, expanding vehicle/support-weapon handling, and replacing any remaining legacy attack-support order paths with the same commander authority.

### FoW NO

Do not reinterpret it as success. It would show that even early human ownership plus a user-control phase plus automatic Mate transfer is insufficient for pre-authored support actors.

The next experiment would then need true human-native actor creation/registration rather than another ownership/control permutation.
