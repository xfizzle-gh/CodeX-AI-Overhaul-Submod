# TMAI-referenced allied-support commander

## Scope

PR #100 is the battlefield-command follow-up to PR #99. It does not replace #99's native-spawn/FoW diagnostic. The intended native test sequence is:

1. Test PR #99 first and record the native DefenderBot diagnostic FoW result.
2. Then deploy PR #100 and evaluate the allied-support commander behavior.

PR #100 is stacked on the exact #99 head so the second test contains the same FoW diagnostic baseline plus the commander change.

## Why TMAI v0.17 is the reference

The earlier source audit recorded on issue #97 reviewed TMAI v0.17 from Steam Workshop item `3746791037`. That review found an architecture directly relevant to our support force: an extra Team-A Mate bot controls allied actors while normal Conquest bots keep using normal Conquest AI.

TMAI does not provide the reinforcement economy/spawn system we need. Its value here is battlefield command after units already exist.

The source audit identified these useful behaviors:

- approximately 3 seconds of settle time before a newly transferred unit is first tasked;
- infantry controlled in small groups and vehicles independently;
- managed-unit filtering for invalid, dead, inactive, linked, user-controlled, repairing, or ignored actors;
- reduced repeated move spam so combat is not continually interrupted;
- distinct objective distribution to reduce bunching and vehicle collisions;
- up to four infantry held on newly captured points;
- recently lost friendly flags receive strong counterattack priority;
- fallback behavior if the Lua strategy publication path fails;
- mobile guns and crew-served support are deliberately held rather than blindly pushed;
- TMAI uses a one-way Lua-to-MI strategy bus and MI `action move` for final execution.

The same audit found TMAI does not solve autonomous purchasing/reinforcement spawning, mortars, AT guns, artillery, or a native spawn context for the Mate.

## Source provenance and licensing

The actual v0.17 source path supplied for the audit is:

`E:\steam\steamapps\workshop\content\400750\3746791037`

That local Windows path is not mountable from the GitHub connector execution environment used for PR #100. Therefore this PR does not invent or claim exact TMAI source filenames/functions that were not preserved in the #97 audit record.

The #97 source audit records that the v0.17 package includes an MIT License. No TMAI source code is copied verbatim in PR #100. The implementation is a clean adaptation of the documented behavioral architecture to our existing support controller.

Before any later literal code port, reopen the local v0.17 package, record the exact source path/function, and preserve the required copyright/license notice.

## Existing Code:X support architecture retained

The support force itself is still delivered by our MI wave system. On human attack missions the routed non-human Team-A bot publishes its real runtime player ID as `id_attack_support`, and the MI support engine transfers delivered actors to that controller.

The following are deliberately unchanged:

- PR #96 ownership routing;
- the 100% support mission gate;
- all four player/enemy attack/defense support quadrants;
- MI wave delivery and existing wave pacing;
- PR #99 native DefenderBot FoW diagnostic;
- normal enemy Conquest AI and purchasing;
- no `FirstPlayerId` ownership fallback;
- no `utility.lua` load on the special attack-support slot.

## What PR #100 replaces

Before PR #100, `attack_support.lua` had a very weak commander:

- enumerate `BotApi.Scene.Squads`;
- give each newly observed squad a random capture flag;
- every 400 quants, blindly reissue orders to every squad.

That is not strategic command. It also creates order churn that can interrupt useful local combat behavior.

PR #100 removes the random picker and the unconditional 400-quant reissue loop.

## Commander state

The controller now maintains a managed record for each live support squad. Each record includes:

- squad identity/key;
- whether its initial settle period has completed;
- current role;
- current objective;
- count of issued strategic orders.

Squads that disappear from `BotApi.Scene.Squads` are pruned from the commander registry. Stale timer callbacks are generation-gated across `GameStart` resets.

The existing support slot does not expose a safely proven actor-level classifier without loading the crashing utility path. PR #100 therefore treats each delivered MI squad as the atomic command group. This is intentionally conservative.

## Three-second settle

When a new support squad appears, the commander uses the engine's existing millisecond `BotApi.Events:SetQuantTimer` API and waits `3000` ms before that group becomes eligible for strategic tasking.

If the timer API is unexpectedly unavailable on this slot, the controller logs the failure and fails open to immediate battlefield command rather than permanently orphaning the squad.

## Objective model

The commander reads the same live `BotApi.Scene.Flags` objects used by normal Conquest AI. It compares `flag.occupant` to the support bot's `BotApi.Instance.team` and `enemyTeam` without loading `utility.lua`.

Each flag is classified as friendly, enemy, or neutral.

The commander also keeps ownership history:

- friendly -> enemy/neutral marks a recently lost flag;
- enemy/neutral -> friendly marks newly captured ground;
- a recaptured recently lost point clears its lost marker.

## Planning policy

Planning is deterministic and state-change driven.

First, settled groups are spread across distinct capturable objectives. Recently lost friendly objectives sort ahead of other targets and receive the `counterattack` role.

Second, spare groups may be assigned to newly captured friendly ground with the `hold` role, bounded by the TMAI-inspired cap of four command groups.

Third, the commander keeps one reserve group when possible.

Fourth, if a large support wave still has unassigned groups while capturable objectives remain, the remaining active groups reinforce the least-loaded attack objective rather than all collapsing onto the first flag.

The strategic roles are therefore:

- `attack`
- `counterattack`
- `reinforce`
- `hold`
- `reserve`

## Order-spam suppression

Each managed group remembers its last role and target. If the desired role and target have not changed, no new strategic command is issued.

This is the key TMAI behavior we want to preserve: the strategic layer should choose where a group belongs, then let the game's local unit AI fight instead of resetting its order on an arbitrary timer.

Replanning occurs when meaningful state changes are detected, such as a squad appearing/disappearing or flag ownership changing.

## Command transport

TMAI v0.17 deliberately uses a Lua-to-MI one-way strategy bus and MI `action move` execution.

PR #100 does not pretend that exact interface exists in our current support controller. Repository review found no established equivalent strategy bus on this stripped bot slot, while `CaptureFlag` and `SeekAndDestroy` are already exercised by our current implementation.

Therefore PR #100 uses:

1. `BotApi.Commands:CaptureFlag(squad, flagName)` for strategic flag assignments.
2. `BotApi.Commands:SeekAndDestroy(squad)` only as a transport fallback if `CaptureFlag` is unavailable/fails.

This separates two questions cleanly: whether the TMAI-style commander behavior improves allied support, and whether a later MI transport port is desirable.

## Deliberately deferred TMAI mechanisms

These are not fabricated in PR #100:

- actor-level infantry batching;
- explicit per-vehicle classification;
- `user_control`, repairing, linked, inactive, and ignored actor filtering beyond the live support-squad registry;
- mobile-gun / crew-served weapon classification;
- mortar, AT-gun, or artillery tactical AI;
- literal TMAI Lua-to-MI strategy-bus code.

They require a safely proven actor classification/control surface on this special bot process or a later exact-source MI integration pass.

## CI and static regression

`tests/test_tmai_support_commander.py` protects the new behavioral contract. It checks:

- explicit TMAI v0.17 provenance and logging;
- 3-second timer-based settle;
- removal of random target selection and 400-quant order spam;
- managed-group discovery/pruning;
- distinct-objective first-pass planning;
- recently lost counterattack priority;
- captured-point hold cap;
- reserve behavior;
- proven `CaptureFlag` / `SeekAndDestroy` transport;
- absence of the crashing `utility.lua` path;
- strict custom attack-support routing;
- separation from PR #99's native FoW diagnostic.

The narrow allied-support workflow runs this suite alongside the existing ownership/gate, native FoW, DefenderBot, and CWA mission-integrity regressions.

## Native test procedure after PR #99

Deploy PR #100 only after completing the #99 FoW diagnostic test.

Use a human-attack Dynamic Conquest mission where allied support is enabled.

Observe at least two support waves if practical.

Check:

1. Support units still spawn and transfer to the friendly Team-A support controller.
2. No support group receives its first commander order immediately on discovery; the log should show `discovered`, then `settled ... after_ms 3000`.
3. Multiple groups initially spread among distinct enemy/neutral objectives rather than all choosing one random point.
4. Orders are not reissued continuously while the desired target is unchanged.
5. When a friendly flag is lost, a subsequent plan gives that point counterattack priority.
6. When a point becomes friendly, spare groups can receive `hold` assignments there.
7. A reserve is retained when enough groups exist.
8. Infantry is not constantly interrupted by strategic order spam while fighting near its objective.
9. Vehicles delivered as their own squad are commanded independently because each squad is an independent managed group.
10. Enemy Conquest AI and enemy purchases remain normal.
11. Support wave cadence and all existing ownership/FoW observations remain otherwise unchanged from #99.
12. `game.log` contains no `CODEX_TMAI_SUPPORT: event_error` lines.

## Log markers

Search `game.log` for:

```text
CODEX_TMAI_SUPPORT:
CODEX_TMAI_SUPPORT: armed
CODEX_TMAI_SUPPORT: discovered
CODEX_TMAI_SUPPORT: settled
CODEX_TMAI_SUPPORT: flag_lost
CODEX_TMAI_SUPPORT: flag_captured
CODEX_TMAI_SUPPORT: order
CODEX_TMAI_SUPPORT: reserve
CODEX_TMAI_SUPPORT: pruned
CODEX_TMAI_SUPPORT: plan
CODEX_TMAI_SUPPORT: heartbeat
CODEX_TMAI_SUPPORT: event_error
```

Useful order examples will identify the managed group, role, target flag, command transport, and order count.

## Interpretation

PR #100 is successful as a commander experiment if support groups are visibly more coherent than the old random/spam policy, remain responsive to objective changes, and do not show repeated order churn or controller errors.

The PR is not evidence by itself that TMAI's exact MI transport has been reproduced. It intentionally tests the TMAI strategic model using the transport already proven in our architecture.
