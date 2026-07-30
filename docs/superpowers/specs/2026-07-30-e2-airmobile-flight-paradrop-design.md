# E2 Airmobile Flight and Paradrop Design

**Status:** Approved direction on 2026-07-30: implement option 2, reusing the
modern Code:X aircraft/ejection behavior while keeping route selection and
post-landing orders inside the support-wave engine.

## Objective

Add two live-test-only attack-support variants:

1. A real transport helicopter flies from the friendly entry side to a safe
   standoff landing zone, inserts a four-person fireteam, and departs while the
   fireteam advances on the selected active flag.
2. A real transport plane flies toward the selected active flag and uses the
   aircraft's existing `drop_paratrooper` effect to release linked troops in a
   150-250 m standoff band. Survivors then advance on that flag under the
   support engine's orders.

Neither variant becomes production behavior in this change. Both remain inert
unless an integer test gate is deliberately enabled for a live mission.

## Binding Constraints

- Obey the MARCHING ORDERS in
  `docs/plans/2026-07-30-allied-support-expansion.md`.
- Work repo-first. `tools/deploy_attack_support_probe.ps1` is the only workshop
  writer.
- Use MOVE placement, never `{clone}`.
- Use only existence-verified entities and breeds.
- Use literal player 1-16 ownership switches with a fail-closed default.
- Place infantry one unit at a time with at least 0.5 seconds between units.
- Keep announcements behind `support_announce$` and diagnostics behind
  `support_debug$`.
- Keep every change below `resource/map/multi/ce/` byte-identical to its
  `resource/map_scripts/` mirror.
- Allocate fresh entity id, MID, and parking-position bands only after a
  collision sweep over all managed base maps and include pools.
- Do not weaken an existing guard or test to make this feature fit.

## Why Option 2

The Conquest squad entry's `{action "airstrike:paratank_trigger"}` is metadata
consumed by the reinforcement/cursor call-in system; it is not a registered MI
action that the support engine can invoke directly. Importing that entire
purchase and targeting path would couple E2 to UI state and make gate-zero
inertness difficult to prove.

Option 2 retains the useful, modern Code:X part: the verified para aircraft,
its cargo-door handler, and its `drop_paratrooper` effect. Our MI engine owns
only the parked pool, player transfer, objective selection, flight order,
near-target release, cleanup, and assault order. CE does not choose the route,
drop target, or post-drop objective.

## Test Gate and State

`support_e2_test$` is declared but never enabled by mission initialization:

- `0`: inert; no E2 pool claim, transfer, placement, order, announcement, or
  timer can run.
- `1`: force one helicopter insertion for live testing.
- `2`: force one paradrop for live testing.

The engine records integer-only diagnostic state:

- `support_e2_stage$`: lifecycle stage, initially 0.
- `support_e2_fail$`: fail code, initially 0.
- `support_e2_lz$`: selected LZ candidate, initially 0.
- `support_e2_flag$`: selected flag region/index, initially 0.

`mirrorEngineState()` emits these values to `game.log` using the existing
pcall-guarded variable reader. No on-screen diagnostic is required.

## Supported Factions and Assets

Initial helicopter coverage uses verified transports:

| Faction | Aircraft | Crew |
|---|---|---|
| RUSA | `mi17_b8_rus` | `mp/rusa/2022s/rus_pliot` |
| UKR | `mi17_b8_ukr` | `mp/ukr/2022s/ukr_pilot` |
| NATO | `uh-60m_blackhawk_mg` | verified NATO pilot breed |

Initial paradrop coverage follows existing modern Conquest squads:

| Faction | Aircraft | Payload source |
|---|---|---|
| RUSA | `il-76td_para` | verified 106th VDV breeds |
| UKR | `c130_para` | verified 13th Air Assault breeds |
| NATO | `c130_para` | verified 82nd Airborne breeds |

PRC is fail-closed for both variants in this phase. The permitted Mi-171Sh
adaptation remains a separately existence-verified follow-up, and there is no
approved PRC paradrop aircraft.

Only passenger places covered by the existing ejection implementation are
used. No troop is linked to `seat00` or any place above `seat20`.

## Objective and LZ Geometry

The attack engine selects one active flag and marks it with an E2-specific tag.
The deploy script derives two or three named helicopter LZ candidates for each
flag region in every managed map:

- candidate radius from flag: 1,500-2,500 map units (150-250 m);
- primary direction: the side of the flag opposite the enemy spawn centroid;
- lateral alternates: rotated offsets around the same standoff band;
- existing enemy-proximity guard idiom rejects a candidate before launch;
- if every candidate is rejected, the variant records a failure and uses the
  documented standoff teleport fallback rather than moving the aircraft into
  an unsafe point.

The generator self-heals its own E2 waypoints and validates exact per-map
counts on every deploy. Runtime flag-to-waypoint selection is an explicit
integer switch; it does not attempt unsupported ring arithmetic in MI.

The paradrop does not need a ground marker at the release point. The plane is
placed at the friendly entry side, ordered toward the selected flag, and the
proven near-target pattern fires the release effect while the aircraft is
1,500-2,500 units from the flag. The approach direction therefore places the
drop on the friendly/far side of the objective.

## Helicopter Lifecycle

1. Under `support_e2_test$ == 1`, claim exactly one faction transport, its two
   linked pilots, and an independent four-person insertion pool.
2. Transfer the package through the existing literal 1-16 support-player
   switch. An unresolved player id takes no ownership action.
3. MOVE-place the helicopter at the friendly entry pad.
4. Apply the attested order sequence: `air_state` at altitude 30,
   `actor_state {control AI}`, then `action move` to the selected named LZ.
5. Use a bounded arrival window and mirror a near/distance observation for
   live evidence. The first implementation does not depend solely on an
   unproven distance predicate.
6. On arrival, announce the insertion and MOVE-place the fireteam one member
   every 0.5 seconds at the LZ.
7. Order the fireteam to advance on the selected flag and order the helicopter
   back to the friendly entry pad.
8. Clear all temporary E2 tags. Preserve stage/fail values for the Lua mirror.

If reliable flight is not demonstrated in live play, retain the guarded
standoff-LZ infantry insertion and disable the real helicopter movement path.
That null flight result is an acceptable, explicitly reported outcome.

## Paradrop Lifecycle

1. Under `support_e2_test$ == 2`, claim one faction aircraft with pilots and a
   small linked airborne payload. The payload uses only ejectable seat places.
2. Transfer the plane and payload through the literal support-player switch.
3. MOVE-place the plane at the friendly entry side, set its required airborne
   state/altitude, give AI control, and order it toward the selected flag.
4. A near-target detector in the 150-250 m band invokes
   `effect drop_paratrooper` on only the E2 plane.
5. Immediately mark the aircraft released and order it back toward the entry
   side so the release cannot repeat.
6. E2 troops remain excluded from CE's `ai_logic/paratrooper_orders` selector.
   Any necessary exclusion is applied identically in both CE mirror trees.
7. When the shared ground-hit lifecycle exposes an E2 survivor as needing
   orders, the E2 engine removes `paratrooper_need_orders` and related
   temporary order tags, then issues its own advance order toward the selected
   flag. E2 never targets CE waypoints 5004-5006.
8. A bounded timeout records a fail code if no survivor reaches the landing
   state. It does not silently replace a failed real paradrop with teleported
   troops.

This design uses Code:X's modern aircraft behavior and ejection effect. The
CE-derived ordering trigger is isolated, not trusted to perform E2 routing.
Live verification remains mandatory before the gate can ever become a
production roll.

## Failure Handling

- No eligible pool or unsupported faction: set fail code and stop without
  touching production wave budgets.
- No safe helicopter LZ: set fail code and use only the documented standoff
  infantry fallback.
- Aircraft misses the bounded arrival/release window: set fail code, order it
  out, and leave the feature test-gated.
- No landed paratroopers: set fail code; do not fabricate success.
- Any delimiter, asset, id/MID, mirror, gate, or deploy guard failure blocks
  the phase. The guard is not loosened.

## Testing and Verification

Implementation follows strict red-green TDD. Tests must pin:

- `support_e2_test$ == 0` inertness across every E2 trigger;
- test-mode dispatch values 1 and 2;
- exact verified aircraft and crew strings;
- supported faction matrix and fail-closed PRC behavior;
- seat range 1-20 with explicit bans on `seat00` and seat 21+;
- unique fresh ids, MIDs, and parking coordinates after a managed-map sweep;
- MOVE placement and absence of `{clone}`;
- literal player 1-16 ownership switching and fail-closed default;
- 0.5-second infantry placement cadence;
- 1,500-2,500 unit standoff geometry and per-flag waypoint counts;
- near-target paradrop release and one-shot cleanup;
- exclusion from CE post-drop ordering and absence of waypoint 5004-5006;
- CE mirror byte identity if either mirror is touched;
- Lua mirror fields, localization keys, deploy markers, and MI delimiter
  balance;
- no mutation of the existing E1, IFV, motorized, flank, or wave-budget state.

Completion requires a fresh full `python -m pytest tests/` pass followed by two
successful deploy runs with byte-identical results. Git status and the final
diff are inspected after deployment before any completion claim.

## Subagent Execution Strategy

Fresh `gpt-5.6-terra` agents at low reasoning implement narrowly bounded tasks
sequentially because templates, engine state, and deployment guards overlap:

1. failing tests plus collision/asset evidence;
2. geometry generator and deploy assertions;
3. gated state, templates, and Lua/localization integration;
4. helicopter lifecycle and fallback;
5. paradrop lifecycle and CE-order isolation.

Each task receives an independent spec/quality review before the next task.
The final whole-change review uses the strongest available reviewer because it
must assess cross-file engine interactions and the default-off safety proof.
