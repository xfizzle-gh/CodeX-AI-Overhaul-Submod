# E2 Sequential Combo Test Mode Design

## Objective

Add a temporary live-test mode that exercises both existing E2 probes in one
player-attack mission without allowing their shared lifecycle state to collide.
Mode `3` runs the helicopter probe to terminal cleanup first, preserves its
result, then automatically starts the existing paradrop probe.

This change does not add player-defense support. The defender dispatcher and
ownership path remain a separate follow-up because they use `id_defenderbot$`
rather than `id_attack_support$`.

## Test Modes

`support_e2_test$` retains the existing values and adds one:

- `0`: E2 off; committed source default.
- `1`: helicopter only.
- `2`: paradrop only.
- `3`: helicopter followed by paradrop.

Mode `3` is sequential. Running both aircraft concurrently is forbidden because
the probes intentionally share `support_e2_stage$`, `support_e2_fail$`, the
claim tags, the selected flag tag, and aircraft cleanup helpers.

## Combo Lifecycle

The shared dispatcher treats mode `3` like mode `1` for its first child:

1. Dispatch reserves stage `10` and explicitly fires the faction helicopter.
2. The helicopter runs its unchanged `10 -> 20 -> 30 -> 40 -> 50 -> 60 -> 70`
   lifecycle, including honest failure and aircraft cleanup.
3. A dedicated combo transition triggers only when test mode is `3`, stage is
   `70`, and no E2 claim remains.
4. It copies the helicopter result into `support_e2_combo_helo_fail$`, changes
   `support_e2_test$` to `2`, clears `support_e2_fail$`, and resets stage to `0`.
5. The existing dispatcher then launches the normal paradrop lifecycle exactly
   once. Because the mode is now `2`, reaching stage `70` cannot restart the
   combo.

The transition occurs after both successful and failed helicopter attempts so
one failed probe never prevents observation of the other. The preserved combo
failure value prevents the paradrop dispatcher from erasing the helicopter
result.

## Timing

The combo is independent of the production wave clock:

- Helicopter launch: approximately 1-3 seconds after attack-support readiness.
- Helicopter arrival decision: 40 seconds after launch.
- Helicopter terminal cleanup: approximately 100 seconds after launch on the
  successful path; earlier on bounded failure paths.
- Paradrop launch: immediately after helicopter terminal cleanup.
- Paradrop release: as soon as the plane enters the 1,500-2,500-unit band, with
  a 90-second release deadline.
- Paradrop settlement: aircraft deletion at 90 seconds after release and final
  survivor/fail-7 decision no later than 119 seconds after release.

Expected visual test time is roughly two to four minutes.

## Temporary 799 Deployment

Committed source remains `support_e2_test$ = 0`. The deploy script gains an
optional validated `-E2TestMode` parameter accepting `0`, `1`, `2`, or `3` and
defaulting to `0`.

After copying the verified source wave include to workshop item `3636883799`,
the sole deploy writer applies an exact, asserted override only to the deployed
copy:

- set `support_e2_test$` to the requested value;
- when the requested value is nonzero, set legacy
  `attack_support_air_test$ = 0` so the older forced 30-second E1 insert cannot
  be mistaken for E2.

Workshop validation must assert the requested E2 value and the legacy-test
value. Re-running with the same parameter is idempotent. Running the deploy
script without `-E2TestMode` restores the deployed copy to committed mode `0`.

The final operation for this task deploys item 799 with `-E2TestMode 3` and
reports that source/workshop wave hashes intentionally differ only because of
the two asserted test overrides.

## Diagnostics

Declare, initialize, and mirror `support_e2_combo_helo_fail$` alongside the
existing E2 integer state. During the paradrop half:

- `e2_combo_helo_fail = 0` means the helicopter probe reached its successful
  terminal cleanup;
- nonzero preserves its original failure code;
- `e2_fail` records the current/final paradrop result.

The existing stage and failure taxonomy remains unchanged.

## Safety and Tests

Implementation follows red-green TDD and must pin:

- committed source default remains mode `0`;
- mode `3` dispatches only the helicopter at first;
- helicopter faction children accept exactly modes `1` and `3` at reserved
  stage `10`;
- the combo transition requires mode `3`, stage `70`, and no remaining claim;
- helicopter failure is copied before the active failure is cleared;
- transition changes mode to `2` before resetting stage to `0`;
- no production wave or aircraft budget is decremented;
- mode `2` cannot retrigger the combo;
- Lua logging mirrors the preserved helicopter result;
- deploy mode validation accepts only `0`-`3`, defaults to `0`, performs exact
  target-only overrides, disables legacy E1 for nonzero E2 tests, validates the
  deployed values, and remains idempotent;
- the complete existing E2, CE-isolation, and repository test suites remain
  green.

After implementation, an independent lifecycle review must approve the stage
handoff and deploy override before item 799 is placed in combo-test mode.
