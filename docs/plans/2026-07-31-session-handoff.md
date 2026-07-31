# Session handoff — 2026-07-31

State capture for the next agent picking up the four-quadrant AI support system
(waves + trucks + helicopter/paradrop call-ins for campaign_capture_the_flag).
Read this alongside `2026-07-30-allied-support-expansion.md` (MARCHING ORDERS —
process rules, verification bar) and `2026-07-30-e2-airmobile-handoff.md`
(flight research + traps). Those rules still bind: repo-first, deploy script is
the only workshop writer, full pytest + deploy exit 0 twice byte-identical
before "done", never loosen guards/tests, verified asset strings only.

## Where things stand (proven live, user-witnessed)

- **Ground waves (Q1–Q4)**: working. Four engines: `attack_support_waves.inc`,
  `enemy_defense_support.inc`, `defense_support_waves.inc`,
  `enemy_attack_support.inc`.
- **Trucks**: package 1 on BOTH sides (8 Link-baked seated riders) does the
  full claim → place → drive (~30 s) → emit at objective sequence. Proven on
  Q1 (NATO FMTV) and Q4 (rusa Ural), fields map, 2026-07-31.
- **Helicopter call-in (E2 helo leg)**: the FLIGHT is real and executed live —
  clone-dispatch, flight to flag, exit, delete; Mi-17 visible on minimap and
  in sky. **Honesty note: in code through `9caa7c2` the 4-man team is
  MOVE-placed at the LZ while the helicopter flies overhead — it is NOT
  emitted from the aircraft.** The binding user requirement (below) replaces
  this with a true land → disembark → take off sequence. NATO currently flies `mi17_b8_rus` (red-star livery — cosmetic
  pass later; do NOT swap back to the Blackhawk, its parked actor has never
  provably instantiated; fail 13/14/9 discriminate it on a future run).
- **C-130 para leg**: plane dispatches at the 3-minute mark and flies visibly.
  The drop itself has never fired (Defect B below).
- **Aircraft recipe (the hard-won part)**: parked airborne template off-map +
  `actor_to_waypoint` + `{clone}` + `{approach "safe teleport & rotate"}` to a
  NUMERIC waypoint whose own `{commands}` block re-tags the arriving clone.
  This is vanilla's own dcg airstrike mechanism (dcg_hills) and Indomitus's.
  Fixed-wing park block: `{Chassis "airborne" {AirborneMode 1}{Altitude N}}` +
  `{ChassisManager {Current "airborne"}}` + `{DisableObstacles}`. Helicopter
  chassis keeps `{Airborne}{EngineStarted}` (1523-vs-58 census, zero overlap).
- **CRITICAL — numeric waypoint ids are a small engine id space, not labels.**
  The 9101–9104 band hard-crashed maps at load
  (`APP_ERROR: Can't use waypoint id, it already used`, eHelperWaypoint.cpp:55).
  Highest numeric waypoint name across 1035 shipped .mi files is 1000.
  **STATUS WARNING: commits through `9caa7c2` still carry 9101–9104 in the
  engine, deploy generator and tests — the migration to the safe band
  (21/22 entry a/b, 23/24 exit a/b) exists only as in-flight/uncommitted work
  at the time of writing. If no later commit contains it, the crash is STILL
  ACTIVE and the migration must be finished first, then woodland proven to
  load.** Keep any new numeric waypoint two-digit and collision-swept.

## In flight at handoff time

An Opus agent was mid-pass on Defects A–D (uncommitted edits to
`attack_support_waves.inc`, `test_e2_airmobile.py`,
`deploy_attack_support_probe.ps1` are its work). If those got committed after
`9caa7c2`, read that commit's message; otherwise finish or redo:

- **A — helo passengers emit as player 0** (white minimap dots, false fail 11).
  Tag pax at emit (`{"emit" {crew {tag …}}}`), literal 1–16 player switch on
  the tag, orders, and key the landed-evidence check on that tag.
  **REQUIREMENT CHANGE (user, binding): the helicopter must LAND at the LZ,
  disembark on the ground, take off, exit — hover-emit is rejected.** Vanilla
  precedent: conquest helicopter transport call-ins + Gostomel heli inserts on
  the same Mi-8 airframes. Extract the real mechanism from shipped sources
  (air_state altitude-to-0, landing effect receiver, or whatever the purchase
  call-in fires) — never guess.
- **B — para release never arms + dishonest success.** `e2_para_band`/`pass`
  stayed 0 through a long visible overflight; stage walked 60→70 with fail 0
  and no drop. Fix the band/position reference, evidence-gate stages 60/70
  (fail 6/7 must fire when no drop happened), route exit off-map before delete.
  The drop must be `{effect drop_paratrooper}` invoked the way the vanilla
  conquest paradrop call-in invokes it — literal parachutes (user requirement).
- **C — motor packages 2–4 never board** (loose pax stand at spawn; truck
  drives off; "troops chasing truck"). Convert packages 2–4 to the proven
  package-1 pattern: Link-bake pax into seats (all four hulls have ≥10
  passenger places; fmtv has no seat12 bone; shaanxi place group is
  `passengers` plural with bones offset by one — see Task A census in
  2026-07-30 plan doc). Also rotate truck placement toward the objective.
- **D — motor band metric always 0** even on perfect drives (after the
  600/1500/4000 decimetre fix). Fix the reference or replace the metric.

## Latest findings (2026-07-31 fields run, log-decoded — newest evidence)

- Helo leg: stage 30→60 with fail 11; the 4 troops at the LZ were the
  MOVE-placed team, unowned (player 0, white minimap dots). Fail 11 is a false
  negative — the landed check cannot see neutral troops. One ownership fix
  closes both.
- Para leg: stage 20→60→70 with band 0, pass 0, fail 0 — plane visibly flew
  for minutes, never dropped, deleted MID-MAP. Release detector never armed
  (check its position reference + units, decimetre trap), exit path is not
  evidence-gated (fail 6/7 must be reachable), delete must follow an off-map
  exit like the helo leg does.
- Trucks: #1 both sides did full claim→place→drive(~30s)→emit. Packages 2–4
  never board their loose pax (troops stand at spawn, later chase the truck);
  convert them to package 1's Link-baked seated riders. Trucks spawn facing
  off-map. motor band metric still reads 0 even on perfect drives after the
  unit fix — reference or mechanism still wrong.
- CAVEAT for the pax fix: `{"emit"} {crew {tag ...}}` may be a FILTER (selects
  which occupants to emit), not a tag-assigner — verify against shipped usage
  before relying on it for tagging.
- Working agent (mid-pass at time of writing) split the work: air legs (A+B)
  first, then motor (C+D), serialized because both touch
  attack_support_waves.inc.

## Update (2026-07-31, later): air-leg sub-pass DONE in working tree (uncommitted)

Suite at 281 passed / 1524 subtests with these in the tree. Root causes proven:

- **Defect A closed**: the 4-man helo team was MOVE-placed by `e2_place_one`
  with ownership (`e2_own_current`) run at stage 20 — BEFORE placement, while
  the bodies were still hidden/inactive templates. Every proven path places →
  unhides → THEN owns; E2 did it backwards → player-0 white dots. Fix: new
  `support_e2_pax` tag written by the placement itself + `e2_own_pax` literal
  1–16 switch (fail-closed → fail 8); landed check now counts bare
  `support_e2_pax`. Fail 11 was a state decoration zeroing the selector —
  third live proof of that landmine.
- **`{"emit"} {crew {tag X}}` is a FILTER, not a tag-assigner** — proven from
  vanilla `1941_12_tikhvin/0.mi:32284` and Code:X `dcg_script.inc:13362`
  (both name map-authored tags on pre-existing occupants). Never rely on it
  to tag.
- **Defect B closed**: para stage stuck at 20 on the interlock branch — every
  release monitor was stage-30-gated, so none ever armed (band 0 explained);
  the range poll also used dead tag co-residence + a banned state decoration.
  Re-keyed on bare `support_e2_arrival` vs `support_e2_flag_target`; new
  `e2_para_require_release_or_fail` gates every exit (release recorded or
  fail 6); off-map transit delay 60 s before any delete (orphan sweep too).
- **Helo teams are NOT aboard the helicopters** (Link census: helos link only
  driver+commander; the planes DO link pax into seats). The in-flight
  land-and-disembark task therefore Link-bakes the teams into `seat1..seat4`
  (mi17 place names) so they ride the clone, then lands via a mechanism
  extracted from shipped sources (Code:X `interaction_entity/helicopter.inc`
  has takeoff/landing receivers + an altitude_checker; Gostomel 3261086933
  `gostomel/9.mi` lands real Mi-8s). Paradrop must match the vanilla
  call-in's invocation (`drop_paratrooper` receiver alone only opens the
  cargo bay). CE eject covers `seat1..seat9`/`seat01..seat20` only — keep pax
  inside that range.

## Parked (do not touch without user say-so)

- Woodland crash (was the 910x id collision — likely resolved by the 21–24
  resweep, but woodland has NOT been proven to load since; prove it).
- Blackhawk instantiation (fail 13/14/9 discriminate).
- Red-star livery on NATO's Mi-17 (cosmetic).
- E2 production roll: committed source keeps `support_e2_test$ = 0`; testing
  uses `deploy -E2TestMode 3` (sequential combo). Plain deploy restores 0.
- Flag emplacement crewing (user parked it).

## Process (user directives, standing)

- Opus subagents write all code; the main agent diagnoses, reviews, verifies
  (commits are re-verified: pytest re-run, deploy re-run for idempotency).
- The user's eyes outrank any script state: a stage/status variable is NOT
  evidence an aircraft or truck did anything. Never suggest the user failed to
  notice something. Telemetry lives in `game.log` as `CODEX_ATTACK_SUPPORT`
  mirror lines (e2 stage/fail/band/pass, per-engine motor stage/drive_t/band).
- Log path: `%LOCALAPPDATA%\digitalmindsoft\gates of hell\log\game.log`.
- Deployed copy: `E:\Steam\steamapps\workshop\content\400750\3636883799`
  (never edit directly; `tools\deploy_attack_support_probe.ps1` only).
- Test suite baseline at `9caa7c2`: 263 passed / 1448 subtests.
