# Native allied-support spawn lifecycle FoW diagnostic

Issue: #97

Base: `2e5286bde3b2fff77c3dbc8f1faa2dda8b767c8d`

Branch: `research/tmai-native-support-fow`

## Problem and established native evidence

Friendly support can share detected enemy contacts with the human, but the human terrain FoW mask does not brighten around the legacy support actors.

Do not reopen the rejected ownership hypotheses:

- PR #96 is the attack-side ownership baseline. Friendly human-attack support belongs to the routed non-human Team-A support controller, not `DefenderBotId`.
- PR #98 proved that assigning the legacy support actors to the human does not restore bright terrain FoW.
- On a human-defense mission, ordinary engine/BotApi-created DefenderBot units brightened terrain FoW while legacy support actors transferred to that same DefenderBot did not.
- Both classes used AI control, so ownership, team, and `control AI` are no longer the leading differentiators.

The current hypothesis is actor creation/registration lifecycle.

Vanilla Conquest follows:

`TrySpawnUnit()` -> `GameModeSpawnUnit()` -> `BotApi.Commands:SpawnAt(...)` or `BotApi.Commands:Spawn(...)` -> `GameSpawn`.

Legacy support follows:

pre-authored player-0 actor -> MI placement/move -> MI player transfer -> AI control.

## Vanilla and Evacuation findings

The current Conquest controller already exposes the native path used for ordinary bot actors. `GameModeSpawnUnit()` prefers `SpawnAt` and falls back to `Spawn`, while `OnGameSpawn()` registers normal post-spawn orders.

The prior vanilla source audit recorded in #97 also found stock `aiTeamPlayers` use in Evacuation, routed through `laststand.lua` with normal utility/purchase/native spawning. That is evidence that an extra AI-team slot can participate in native spawning when it has a complete spawn/unit context. It does not prove our custom attack-support slot has that context.

The custom attack-support slot remains explicitly unsafe for `utility.lua`: earlier native testing found no useful spawn deck, `IsUnitAvailable` false, and a crash after loading the normal utility path. This PR does not retry that rejected architecture.

## TMAI v0.17 findings

The source review already recorded on #97 establishes the useful boundary:

- TMAI creates a similar extra Team-A Mate topology.
- TMAI does not provide independent research, economy, purchasing, or reinforcement spawning.
- Units are created through normal game lifecycle first, then transferred to the Mate.
- Its v0.17 battlefield commander uses a Lua-to-MI strategy handoff and MI movement actions rather than solving the Mate native-spawn context.
- Useful v0.17 behaviors include a settle period for newly transferred units, small infantry groups, individual vehicle tasking, objective distribution, managed-unit tracking, reduced order spam, hold detachments, lost-objective counterattack priority, fallback MI behavior, invalid-actor filtering, and conservative handling of mobile guns/crew-served support.
- Mortars, artillery, AT guns, and related support weapons are not treated as solved.

No TMAI runtime code is copied or adapted in this PR. The reviewed v0.17 package was reported to contain an MIT License, but because no TMAI code is incorporated here, no third-party source notice is required by this diff.

### Source-location limitation

The GitHub #97 research record preserves the behavioral findings above, but not the exact TMAI v0.17 source file/function names for each behavior. The local Steam package path supplied for the overnight task is not mounted in this execution environment. Exact TMAI path/function attribution must therefore be re-opened from the local v0.17 package before any follow-up battlefield-command port. This PR deliberately does not invent those identifiers.

## Experiment implemented

This PR chooses the safer DefenderBot positive control instead of trying to bootstrap the custom Team-A support bot into utility/purchase context.

The diagnostic is loaded only after normal `conquest.lua` is loaded. The custom Team-A attack-support route returns before this module can be loaded.

The module then fails closed unless all of the following are true:

1. feature constant `NATIVE_SUPPORT_FOW_TEST_ENABLED` is true;
2. game mode is `campaign_capture_the_flag`;
3. the bot army is `rusa`;
4. `DefenderBotId` has resolved;
5. the current process `playerId` exactly equals `DefenderBotId`;
6. the mission variable `user_is_defender` is exactly `1`.

It then performs exactly one spawn attempt for the hidden diagnostic unit `codex_native_support_test(rusa)`:

1. log identity and API capability context;
2. check `IsUnitAvailable`;
3. log `CanSpawn` if the API exists;
4. attempt `SpawnAt(unit, 6, 0)`;
5. if `SpawnAt` fails, attempt `Spawn(unit, 6)`;
6. capture the matching next `GameSpawn` while the diagnostic is awaiting its spawn event;
7. publish the returned squad ID to `codex_native_support_test_squad` when possible;
8. issue one `SeekAndDestroy` order so the squad moves beyond player LOS;
9. never attempt another diagnostic spawn that mission, even if the spawn call fails.

The normal Conquest controller remains loaded and its normal `GameSpawn` handler remains intact.

## Diagnostic test unit

A dedicated RUSA-only four-man infantry unit is used instead of borrowing a normal player-purchasable squad:

- `rus90_squadlead` x1
- `rus90_seniorrifleman` x1
- `rus90_mg` x1
- `rus90_rifleman` x1

The definition uses existing canonical breeds only, `cost 0`, `cp 0`, and `not_for_player_sale 1`. It is not added to any repository AI purchase table. Runtime `IsUnitAvailable` is still treated as authoritative and the experiment fails closed if the engine does not register the standalone definition as available.

## What this PR deliberately does not change

- No change to attack-support ownership.
- No change to `attack_support.lua`.
- No change to the 100% support mission gate.
- No deletion or replacement of parked player-0 support templates.
- No change to the four support quadrants.
- No custom Team-A `utility.lua` load.
- No normal enemy Conquest purchase-table change.
- No enemy roster change.
- No support-economy system.
- No TMAI battlefield-command port.
- No claim that terrain FoW is fixed.

## Expected game.log markers

Search for `CODEX_NATIVE_SUPPORT_TEST:`.

Expected successful sequence on the RUSA human-defense control:

- `CODEX_NATIVE_SUPPORT_TEST: module_loaded ...`
- `CODEX_NATIVE_SUPPORT_TEST: armed one_shot true ...`
- `CODEX_NATIVE_SUPPORT_TEST: GameStart ...`
- `CODEX_NATIVE_SUPPORT_TEST: spawn_context ... controller_playerId <N> ... DefenderBotId <N> ...`
- `CODEX_NATIVE_SUPPORT_TEST: spawn_request ... requested_unit codex_native_support_test(rusa) ...`
- `CODEX_NATIVE_SUPPORT_TEST: unit_check ... IsUnitAvailable true`
- `CODEX_NATIVE_SUPPORT_TEST: CanSpawn ...`
- `CODEX_NATIVE_SUPPORT_TEST: SpawnAt attempt ... result true`
- or, if needed, `CODEX_NATIVE_SUPPORT_TEST: Spawn fallback_attempt ... result true`
- `CODEX_NATIVE_SUPPORT_TEST: GameSpawn ... squadId <id>`
- `CODEX_NATIVE_SUPPORT_TEST: order SeekAndDestroy ...`

Any `gate_skip`, `spawn_failed`, `event_error`, `IsUnitAvailable false`, or missing `GameSpawn` is a failed diagnostic and must be reported as such.

## Morning native acceptance test

Use a RUSA human-defense Dynamic Conquest mission so the dedicated diagnostic unit can be available to the DefenderBot.

Observe three friendly actor classes beyond the human player's own LOS:

| Actor class | Bright terrain FoW | Enemy contacts |
| --- | --- | --- |
| normal vanilla/Conquest DefenderBot actor | YES/NO | YES/NO |
| legacy transferred `def_sup_*` support actor | YES/NO | YES/NO |
| `codex_native_support_test(rusa)` BotApi-created diagnostic actor | YES/NO | YES/NO |

Decisive expected result for the lifecycle hypothesis: `YES / NO / YES` in the bright-terrain column.

If the diagnostic actor is `NO`, the lifecycle hypothesis is not confirmed. Do not reinterpret that result as success.

Also verify:

- only one diagnostic squad appears;
- the existing legacy support system still appears normally;
- diagnostic soldiers are not available in human purchasing UI;
- enemy AI still purchases its normal roster;
- no human-attack diagnostic spawn occurs;
- no second diagnostic spawn occurs over time;
- game completes without a support save/roster leak;
- `game.log` contains no diagnostic event errors.

## TMAI standalone transfer test

Run TMAI v0.17 without this overhaul if practical:

1. Start Dynamic Conquest.
2. Purchase and deploy a normal infantry squad.
3. Verify that squad brightens terrain FoW before transfer.
4. Transfer it to the TMAI Mate using the normal unit-transfer UI.
5. Let the Mate move it beyond human LOS.
6. Record whether the transferred squad continues to brighten human terrain FoW.

Interpretation:

- YES supports the theory that native-created actors retain terrain-FoW registration across a native transfer.
- NO means the extra Mate itself may not share the terrain mask, making TMAI primarily a battlefield-command reference for this problem.

Do not record either outcome until native testing is performed.

## Follow-up TMAI battlefield-command integration plan

No item below is implemented here. Exact TMAI source file/function names must be recovered from the local v0.17 package before implementation.

| Pattern from #97 v0.17 source review | Exact TMAI source location | Current Code:X equivalent | Decision | Reason |
| --- | --- | --- | --- | --- |
| settle/grace period after transfer | must re-open local v0.17 source | immediate support deployment/order finalizers and Lua squad ordering | Adapt | reduces race/order loss after transfer or future native spawn |
| small infantry groups | must re-open local v0.17 source | wave batches and `attack_support.lua` squad iteration | Adapt | avoids one giant blob while preserving our wave composition authority |
| per-vehicle control | must re-open local v0.17 source | motorized support placement/order paths | Adapt | reduces multi-vehicle command loss and bunching |
| distribute vehicles among objectives | must re-open local v0.17 source | current flag selection/round-robin placement | Adapt | improves objective coverage without changing spawn architecture |
| managed-unit marker | must re-open local v0.17 source | `state.ordered` plus MI deploy tags | Adapt | prevents repeated command resets and can unify Lua/MI ownership of task state |
| reduced infantry order spam in combat | must re-open local v0.17 source | periodic `orderNewSquads`/reorder loop | Adapt | current periodic reorders can interrupt local combat AI |
| hold captured objectives | must re-open local v0.17 source | support flag movement currently emphasizes advance | Adapt | keeps a small security element on gains |
| lost-objective counterattack priority | must re-open local v0.17 source | generic flag selection | Adapt | gives strategic response without rewriting local combat AI |
| fallback MI strategy | must re-open local v0.17 source | existing MI wave movement can remain fallback | Port concept | our MI layer is already a natural degraded path |
| invalid actor filtering | must re-open local v0.17 source | partial squad existence/tag checks | Adapt | add user-control/dead/inactive/linked/repairing exclusions before movement orders |
| mortar/artillery/AT/mobile-gun exclusions | must re-open local v0.17 source | support waves can include motorized/special classes | Adapt conservatively | TMAI itself does not solve these; hold/exclude until class-specific behavior exists |

## Static-test note

`tests/test_conquest_defender_bot.py` contained assertions that were already stale on the exact base before this experiment: old named cadence markers, the removed `NormalWaveSizeScale` shape, and the old `setDocVarsInNattorSpeak(currentDivision)` call. The production Conquest code was not changed for those behaviors in this PR. The tests are updated to assert the current equivalent invariants instead: role-separated start/wave cadence, the active difficulty-scaled `rollWaveSize()` path, and the current zero-argument mission-doc-var call.

## Rollback

The branch is diagnostic only. Rollback is simply restoring the deployed test folder from `origin/main`; no migration or save conversion is introduced by this PR.
