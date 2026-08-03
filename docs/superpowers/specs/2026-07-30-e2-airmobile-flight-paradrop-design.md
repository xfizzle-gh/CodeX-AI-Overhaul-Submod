# E2 Airmobile Flight and Paradrop Design

**Status:** Approved direction on 2026-07-30: implement option 2, reusing the
modern Code:X aircraft/ejection behavior while keeping route selection and
post-landing orders inside the support-wave engine.

## Objective

Add two live-test-only attack-support variants:

1. A real transport helicopter proves the scripted-flight path by flying from
   the friendly entry side to one of the existing
   `attack_support_air_<side>1/2` pads, inserts a four-person fireteam, and
   departs while the fireteam advances on the selected active flag. Exact
   per-flag 150-250 m helicopter LZ geometry is a follow-up gated on this live
   flight proof; v1 does not claim that final geometry requirement.
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
| NATO | `uh-60m_blackhawk_mg` | `mp/nato/2022s/nato_pilot` |

Initial paradrop coverage follows existing modern Conquest squads:

| Faction | Aircraft | Pilots | Four-person v1 payload |
|---|---|---|---|
| RUSA | `il-76td_para` | `mp/rusa/2022s/rus_pliot` | `mp/rusa/2022s/106vdv_squadlead`, `mp/rusa/2022s/106vdv_mg`, 2x `mp/rusa/2022s/106vdv_rifleman` |
| UKR | `c130_para` | `mp/ukr/2022s/ukr_pilot` | `mp/ukr/2022s/ukr13_squadlead`, `mp/ukr/2022s/ukr13_lmg`, 2x `mp/ukr/2022s/ukr13_rifleman` |
| NATO | `c130_para` | `mp/nato/2022s/nato_pilot` | `mp/nato/2022s/82nd_squadlead`, `mp/nato/2022s/82nd_mg`, 2x `mp/nato/2022s/82nd_rifleman` |

PRC is fail-closed for both variants in this phase. The permitted Mi-171Sh
adaptation remains a separately existence-verified follow-up, and there is no
approved PRC paradrop aircraft.

`uh-60m_blackhawk_mg` and `c130_para` are supplied by West-81 workshop item
2897299509, not by Code:X. That mod is an existing runtime prerequisite in the
tested stack, although this submod's `mod.info` does not declare dependencies.
NATO helicopter and NATO/UKR paradrop variants must fail closed when that
prerequisite is absent; they must never substitute an unverified entity.

Only passenger places covered by the existing ejection implementation are
used. No troop is linked to `seat00` or any place above `seat20`.

The modern Code:X plane handlers listen for `drop_paratrooper` (singular), and
the existing DCG detector invokes `{effect drop_paratrooper}`. The CE
`drop_paratroopers` (plural) handler is a different path and is not invoked by
E2. Tests pin both the required singular spelling and the absence of the
plural spelling in E2 actions.

## Objective and LZ Geometry

The attack engine selects one active flag and marks it with an E2-specific tag.
For the time-boxed helicopter flight proof, it reuses the two existing
`attack_support_air_<side>1/2` pads generated at 65% depth. The existing
enemy-proximity guard rejects an unsafe pad, and the other pad is tried before
the test records failure. This deliberately avoids building up to 210 new
per-flag waypoints and a runtime flag-to-LZ switch before the core flight
mechanism has passed a live test on two maps.

If helicopter flight passes that gate, the follow-up geometry phase may derive
two or three candidates per flag at 1,500-2,500 map units, on the side opposite
the enemy spawn centroid. Until then, v1 reports only the pad it actually used
and does not claim exact-radius helicopter insertion.

The paradrop does not need a ground marker at the release point. The plane is
placed at the friendly entry side, ordered toward the selected flag, and the
proven near-target pattern fires the release effect while the aircraft is
1,500-2,500 units from the flag. The approach direction therefore places the
drop on the friendly/far side of the objective.

## Helicopter Lifecycle

1. Under `support_e2_test$ == 1`, claim exactly one faction transport, its two
   linked pilots, and an independent four-person insertion pool.
2. The parked aircraft template must carry the attested state snapshot
   `{Chassis "helicopter" {Airborne} {EngineStarted} {Altitude 22}}`. No
   grounded helicopter template is permitted: there is no evidence in this
   stack that a grounded helicopter takes off in response to an order.
3. Transfer the package through the existing literal 1-16 support-player
   switch. An unresolved player id takes no ownership action.
4. MOVE-place the already-airborne helicopter at the friendly entry pad.
5. Apply the attested order sequence: `air_state` at altitude 30,
   `actor_state {control AI}`, then `action move` to the selected named LZ.
6. Use a bounded arrival window and mirror a near/distance observation for
   live evidence. The first implementation does not depend solely on an
   unproven distance predicate.
7. On arrival, announce the insertion and MOVE-place the fireteam one member
   every 0.5 seconds at the LZ.
8. Order the fireteam to advance on the selected flag and order the helicopter
   back to the friendly entry pad.
9. When the helicopter reaches the exit pad, or when its bounded exit timeout
   expires, remove it with the attested `{"delete" {selector ...}}` action so
   aircraft do not accumulate or loiter over friendly lines.
10. Clear all temporary E2 tags. Preserve stage/fail values for the Lua mirror.

If reliable flight is not demonstrated in live play, retain the guarded
standoff-LZ infantry insertion and disable the real helicopter movement path.
That null flight result is an acceptable, explicitly reported outcome.

## Paradrop Lifecycle

1. Under `support_e2_test$ == 2`, claim one faction aircraft with pilots and a
   small linked airborne payload. Every payload human carries the permanent
   distinguishing tag `support_e2_para_pax` from park time, before any drop can
   occur. The payload uses only ejectable seat places.
2. Transfer the plane and payload through the literal support-player switch.
3. MOVE-place the plane at the friendly entry side, set its required airborne
   state/altitude, give AI control, and order it toward the selected flag.
4. A near-target detector in the 150-250 m band invokes
   `effect drop_paratrooper` on only the E2 plane.
5. Immediately mark the aircraft released and order it back toward the entry
   side so the release cannot repeat.
6. Before the plane path is introduced, add `support_e2_para_pax` as an
   explicit exclusion in CE's `ai_logic/paratrooper_orders` selector and apply
   the edit byte-identically in both CE mirror trees. This is a deterministic
   isolation boundary, not a race to remove `paratrooper_need_orders` after
   CE's ten-second order loop has already selected a unit.
7. When the shared ground-hit lifecycle exposes an E2 survivor as needing
   orders, the E2 engine removes `paratrooper_need_orders` and related
   temporary order tags, then issues its own advance order toward the selected
   flag. E2 never targets CE waypoints 5004-5006.
8. When the plane reaches the exit pad, or when its bounded exit timeout
   expires, remove it with the attested `{"delete" {selector ...}}` action.
9. A bounded timeout records a fail code if no survivor reaches the landing
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
  out, delete it after the exit timeout, and leave the feature test-gated.
- No landed paratroopers: set fail code; do not fabricate success.
- Any delimiter, asset, id/MID, mirror, gate, or deploy guard failure blocks
  the phase. The guard is not loosened.

## Testing and Verification

Implementation follows strict red-green TDD. Tests must pin:

- `support_e2_test$ == 0` inertness across every E2 trigger;
- test-mode dispatch values 1 and 2;
- exact verified aircraft and crew strings;
- exact `{Chassis "helicopter" {Airborne} {EngineStarted} {Altitude 22}}`
  parked state;
- supported faction matrix and fail-closed PRC behavior;
- documented West-81 prerequisite and fail-closed dependent variants;
- seat range 1-20 with explicit bans on `seat00` and seat 21+;
- unique fresh ids, MIDs, and parking coordinates after a managed-map sweep;
- MOVE placement and absence of `{clone}`;
- literal player 1-16 ownership switching and fail-closed default;
- 0.5-second infantry placement cadence;
- reuse of exactly the existing two helicopter air pads per side, with no new
  per-flag waypoint generator in v1;
- 1,500-2,500 unit paradrop release distance from the selected flag;
- exact singular `effect drop_paratrooper` spelling and no E2 invocation of
  plural `drop_paratroopers`;
- near-target paradrop release and one-shot cleanup;
- park-time `support_e2_para_pax` tagging, deterministic exclusion from CE
  post-drop ordering, and absence of waypoint 5004-5006;
- CE mirror byte identity if either mirror is touched;
- deletion of both aircraft variants on exit or bounded timeout;
- Lua mirror fields, localization keys, deploy markers, and MI delimiter
  balance;
- no mutation of the existing E1, IFV, motorized, flank, or wave-budget state.
  The forced E2 probe consumes no production wave slot. Any future production
  integration must consume exactly one normal attack-support wave.

Completion requires a fresh full `python -m pytest tests/` pass followed by two
successful deploy runs with byte-identical results. Git status and the final
diff are inspected after deployment before any completion claim.

## Subagent Execution Strategy

Fresh `gpt-5.6-terra` agents implement the mechanical tasks sequentially
because templates, engine state, and deployment guards overlap:

1. failing tests plus collision/asset evidence;
2. gated state, templates, Lua/localization, and deploy assertions;
3. deterministic CE mirror exclusion and its tests;
4. helicopter lifecycle and fallback, using the strongest available model;
5. paradrop lifecycle and CE-order isolation, using the strongest available
   model.

Each task receives an independent spec/quality review before the next task.
The final whole-change review uses the strongest available reviewer because it
must assess cross-file engine interactions and the default-off safety proof.
