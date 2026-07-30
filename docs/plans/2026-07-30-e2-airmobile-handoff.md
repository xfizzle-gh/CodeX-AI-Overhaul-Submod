# E2 Airmobile (Helicopter + Paradrop) — Executor Handoff (Codex)

> **Executor: Codex (external session).** Read and obey the MARCHING ORDERS section of
> `docs/plans/2026-07-30-allied-support-expansion.md` — every rule applies verbatim
> (repo-first; `tools\deploy_attack_support_probe.ps1` is the ONLY workshop writer;
> full `python -m pytest tests/` + deploy exit 0 twice byte-identical before "done";
> never loosen a guard or test; only existence-verified asset strings; fresh MID/id
> bands with a collision sweep; commit+push with honest messages; if half-done at
> session end, commit the clean subset and report the remainder).

**Verdict from completed research (2026-07-30): BUILD IS VIABLE — MI-driven helicopter
flight is attested in shipped content, including in this repo's own files.**
Everything below is evidence, decisions already made, and traps already hit.
Do not re-derive; do not deviate without stating why in the report.

**Deliverables:**
1. Gated helicopter insert: helo flies entry-pad → LZ pad (~30s real flight),
   4-man team placed at LZ on arrival, troops advance to flag, helo departs.
   Gate: `support_e2_test$` (default 0, provably inert, test-pinned).
2. Announcement key in the existing support .pot; Lua mirror extension
   (`mirrorEngineState()` in `resource/script/multiplayer/modes/attack_support.lua`)
   so live runs self-report e2 stage/fail state to game.log.
3. Tests in the house style (see `tests/test_attack_support_slot_proof.py`):
   inertness at gate 0, entity pins, MID bands, delimiter balance, deploy markers.
4. Paradrop variant: DEFER to a follow-up unless the helo insert lands cleanly
   with budget to spare — and note research trap #8 (eject covers seats 1–20 only)
   plus the standing "do not hook CE `paratrooper_need_orders` ordering" rule.

---

# E2 HELICOPTER FLIGHT — RESEARCH HANDOFF

**Verdict: BUILD IS VIABLE.** MI-driven aircraft flight is drivable in this stack and is attested in shipped content — including inside our own repo. Nothing was built; no files were edited. Repo was clean on `experiment/attack-mate-slot-proof` at `0b43e93` when I stopped.

---

## 1. Flight-control mechanism

### 1a. Flight is an engine-level **chassis**, not a script behaviour

The `_airborne` suffix is a chassis swap, not a script. Diff of `entity/codx_vehicle/rus/heil/mi-24v/mi-24v.def` vs `mi-24v_airborne.def` inside `E:\Steam\steamapps\workshop\content\400750\3261086933\resource\entity.pak` (a plain ZIP, 115,859 members) — four deltas only:

| | non-airborne | `_airborne` |
|---|---|---|
| include | `/properties/helicopter.ext` | `/properties/airborne_helicopter.ext` |
| pather | `{PatherID "helicopter"}` | `{PatherID "bf109"}` (the WW2 Bf-109 air pather) |
| chassis | `{Chassis "helicopter"}` — hover, terrain-following | `{Chassis "wheel"}` + `{Chassis "airborne"}` — fixed-wing style |
| props | `para_form_air` | `helo` |

`E:\Steam\steamapps\workshop\content\400750\3261086933\resource\properties\helicopter.ext:141-163` is the load-bearing block:

```
{Chassis "helicopter"
	{MaxElevationSpeed		5	} ; m/s
	{ElevationAcceleration	3	} ; m/s^2
	{MaxAltitude			50	} ; m
	{TravelAltitude			22	} ; m minimum altitude above ground, moving is dynamic
	{EngineStartupTime		3} ; seconds
	{DontUseSlope}
	{Airborne}
	{IgnorePatherBoundsAI}		; AI ignores pather bounds, USER is constrained by pather bounds
}
{ChassisManager
	{Current "helicopter"}
}
```
plus `:8-18` — `{Collider "airborne"} {PatherID "helicopter"} {brain "vehicle" {state "airborne"}} {SimulatorId "airborne"}`.

**Entity scripts contain zero movement verbs.** `3261086933\resource\set\interaction_entity\helicopter.inc` (132 KB), `airborne_helicopter.inc`, `airborne_modern.inc`, `Airborne_M.inc`, `airborne_cw.inc` were all read: `{on "takeoff"}`, `{on "takeoff_load"}`, `{on "landing"}` are **receivers** that only `play_sound` / `ani_play` / `view start` / `tags add`. The comment at `airborne_helicopter.inc` on `takeoff_load` says it outright: `;// plane spawns in chassis "airborne"`. `grep` for any chassis-changing verb (`chassis_switch|switch_chassis|set_chassis`) across Code:X + vanilla `set/` trees returns **nothing**. The only movement-adjacent verbs are `{movement_limit slow|normal|fast}` (locomotion tier) and `{constrain_velocity 80}` (a crash handler).

`helicopter.inc:233-282`, `{on "altitude_checker"}`, proves the direction of causality — `heli_in_flight` / `wheel_pullback` / `w81_landing` are set by `{if altitude 5}`, i.e. flight state is a **consequence of altitude**, never of receiving an order.

`tp_control.set` (`3261086933\resource\set\tp_control.set:43-79`) is **camera only** — and `_airborne` and non-`_airborne` variants `inherit` the *same* preset, which independently confirms the suffix is purely a chassis distinction. Nothing there is usable for E2.

### 1b. The MI verbs that actually drive flight

Registered in `E:\Steam\steamapps\workshop\content\400750\CTA GoH Vanilla\resource\set\registry\command.reg`: `{"$air_state"}`, `{"$air_attack"}`, `{"$action"}`, `{"$actor_to_waypoint"}`, `{"$squad_to_waypoint"}`.

**Three attested idioms, ranked:**

**(A) `{"air_state"} {selector …} {altitude N}` — puts/holds the unit in the air.** Standalone trigger-action form, `3261086933\resource\map\multi\bakhmut_1\campaign_capture_the_flag.mi:16803-16827` (`altitude_keeper`, self-retriggering). Waypoint-command form, `3261086933\resource\map\multi\gostomel\12.mi:9150-9183`.

**(B) `{"action"} {action move} {waypoint "<name>"}` — the move order. THIS IS THE GOLDEN PATH.** Named (non-numeric) waypoints work here and are already used **26 times in our own repo**: `resource\map\multi\defense_support_waves.inc:2011,2019,2046,2054,2069,2077` + repeats at `2232-2298`, `2453-2519`, and `enemy_defense_support.inc:1595,1603,1737,1745,1879,1887,2021,2029`. Example verbatim (`defense_support_waves.inc:2011`):

```
{"action"
	{selector {ignore_captured_by_user 0} {tag def_sup_h1}}
	{drop orders}
	{action move}
	{waypoint "attack_support_entry_b1"}
}
```

Proven **on an aircraft** inside our own deployed maps — `resource\map\multi\dcg_[cwa71]_factory\campaign_capture_the_flag.mi:3778-3795`, waypoint `"0"`'s command block, the HF airstrike path:
```
{"action"
	{selector {source standart} {ignore_captured_by_user 0} {tag enemy_air}}
	{drop orders}
	{action move}
	{target {ignore_captured_by_user 0} {amount 1} {tag spawn_a}}
}
```

**(C) `{"waypoint"} {who {type actor} {actors {tag X}}} {action {type start} {waypoint "N"} {approach force}}` — routed flight along a `{transition}` graph.** The gostomel helicopter insert, `3261086933\resource\map\multi\gostomel\9.mi:~26707`. This is the richest form (per-node `{"air_state"}`, `{"emit" {mode passengers}}` for a real troop unload, terminal `{"delete"}`) — **but see trap #1: it will not accept our named waypoints.**

**The exact ordering the shipped content uses** — `bakhmut_1\campaign_capture_the_flag.mi:16502-16520`:
```
{"air_state"   {selector … {tag plane_test_to_exit_selected}} {altitude 48}}
{"actor_state" {selector … {tag plane_test_to_exit_selected}} {drop sensor} {control AI}}
{"action"      {selector … {tag plane_test_to_exit_selected}} …}
```
`air_state` → `actor_state {control AI}` → `action`. Replicate that order.

CE's own air pipeline in our repo confirms the same shape at `resource\map\multi\ce\ai_logic\ce_ai_logic_triggers.inc:4000` (`move_planes_offmap`, `{"air_state"} {altitude 80}` then relocate), `:4287` (`send_plane_to_fly_by`), `:4990` (`send_plane_on_sorty`), mirrored byte-identically under `resource\map_scripts\ai_logic\`.

### 1c. `{approach}` semantics — full enumeration across all four trees

| count | form | meaning |
|---:|---|---|
| 1550 | `{approach "safe teleport & rotate"}` | teleport |
| 92 | `{approach "teleport & rotate"}` | teleport |
| 75 | `{approach force}` | **pathed move, overrides existing orders** |
| 8 | `{approach "safe teleport"}` | teleport |
| 2 | `{approach teleport}` | teleport |
| — | omitted | plain pathed move |

A bare `{approach}` with no argument never occurs. `{"actor_to_waypoint"} {waypoint "N"}` **without** `{approach}` is a genuine pathed move order — 81 call sites, only one carries an approach. Proof it paths rather than teleports: `CTA GoH Vanilla\resource\map\single\00-bootcamp\03_artillery-reinforcements\0.mi:11367-11392` fires `{effect engine_on}` then `{"actor_to_waypoint"}` and the vehicles drive.

---

## 2. Transport selection

**Selected: `mi17_b8_rus` (Code:X, non-airborne variant).** Do **not** use `mi17_b8_rus_airborne`.

Rationale: the `_airborne` variant runs `{Chassis "airborne"}` via the `heli_model` macro at `2897299509\resource\properties\mobility.inc:333`, which hard-codes `{maxElevationSpeed 0}` and `{elevationAcceleration 0}` — the `climb_speed(10)` argument every `_airborne` def passes is **dead**, the real formula is commented out. Such a helo cannot change its own altitude and requires a permanently self-retriggering `air_state` keeper trigger; it also cannot hover or stop at an LZ (50 m turn radius, fixed-wing behaviour). The non-airborne `mi17_b8_rus` gets `{MaxElevationSpeed 5}` + `{TravelAltitude 22}` + `{Airborne}` from `helicopter.ext` and self-manages altitude — **no keeper trigger needed**. This is also the chassis gostomel's MI-routed troop-insert helos use.

### Verified specifics for `mi17_b8_rus` / `mi17_b8_ukr`

Path in pak: `entity/codx_vehicle/rus/heil/mi17/mi17_b8_rus.def`. Entity name = filename minus `.def` (root token is bare `{actor`, there is no internal name field). Roster existence proof: `3261086933\resource\set\multiplayer\units\conquest\units_rusa.set:784` —
```
("squad_vehicle7"  side(rusa) period(2022s) min_stage(16) max_stage(99) vehicle(mi17_b8_rus) cw(0) cp(4) crew1(rus_pliot:2) …)
```
and `units_ukr.set:588` for `mi17_b8_ukr` with `crew1(ukr_pilot:2)`. Registry entries `units_rusa.set:1760`, `units_ukr.set:1334`.

The `.def`'s own chassis block is only a locomotion override; everything else comes from the include:
```
{Chassis "helicopter"
	{Locomotion ("locomotion_attack")
		{MaxSpeed		65	}
	}
}
```
Speed 65 km/h ≈ 18 m/s.

**Seat/place table (14 places).** Note the off-by-one — `{Link}` takes the *place* name, not the bone:

| place (use in `{Link}`) | bone | group |
|---|---|---|
| `driver` | `driver` | crew |
| `commander` | `commander` | crew |
| `seat1` … `seat12` | `seat01` … `seat12` | passengers |

**Crew breeds, existence-proven** (`3261086933\resource\set\breed\mp\…`):
- `mp/rusa/2022s/rus_pliot` — **note the typo, `pliot` not `pilot`**; has `("traits_pilot")`, `{armors {head zsh7apn}}`
- `mp/ukr/2022s/ukr_pilot` — exists, but **lacks `traits_pilot`** (only `skill_rank_1` + `crewman`) — asymmetry worth knowing
- Also verified: `mp/nato/2022s/{nato,usarmy,usmc,gb}_pilot`, `mp/prc/2022s/pla_crew`, `mp/sov/era1960/sup_pilot`. **No `aircrew` breed exists anywhere.**

**Parking template — the proven form** (derived from `gostomel\9.mi:6718-6759` + our repo's humvee package at `attack_support_templates.inc:469-552`):
```
{Entity "mi17_b8_rus" 0xb400
	{Position -4000 -36600}
	{Player 0}
	{MID 9800}
	{Able "-select"}
	{Chassis "helicopter"
		{Airborne}
		{EngineStarted}
		{Altitude 22}
	}
}
{Human "mp/rusa/2022s/rus_pliot" 0xb401 { … {Player 0} {MID 9801} {Able "-select"} } }
{Human "mp/rusa/2022s/rus_pliot" 0xb402 { … {Player 0} {MID 9802} {Able "-select"} } }

{Link 0xb401 {0xb400 "driver"}}
{Link 0xb402 {0xb400 "commander"}}

{Tags "e2_helo_tpl" "hidden" 0xb400}
```
`{Altitude}` must be ≥ `TravelAltitude` (22); 22.0 is exactly what the editor writes. `{Tags "tag1" "tag2" … 0xID}` — hex id **last, unquoted**. Our repo's template `.inc` files already carry `{Entity}` / `{Human}` / `{Link}` / `{Tags}` sections in that order, so a helo package drops straight in (`attack_support_templates.inc:469-552` is the exact precedent).

---

## 3. LZ approach, arrival, departure, fallbacks (decided, not built)

**LZ candidates: reuse the existing `attack_support_air_<side>1/2` waypoints.** They already exist in all 14 deployed maps, generated idempotently by `tools\deploy_attack_support_probe.ps1:1346-1361` at `AirDepth = 0.65` toward map centre with a lateral spread of `max(350, len*0.25)`. **No generator work is required for E2.** Phase 5 E1 already teleports troops onto these same pads (`attack_support_waves.inc:253-297`), so helo and troops share one LZ for free.

**Side selection: mirror E1 exactly** — `enemy_spawnside$ == 1` → `_b` pads; `== 2` → `_a` pads; default `_b1` (`attack_support_waves.inc:256,274,291`).

**Launch origin:** teleport the helo with `{"placement"} {target_waypoint "attack_support_entry_<side>1"}` first, then fly. Do **not** order it straight from the parked off-map position: pool parking is at y ≈ −35100…−36400 while maps span ~±9500 units, i.e. a ~27,000-unit (2.7 km) leg ≈ 150 s at 18 m/s. Entry-pad → LZ is ~65% of the centroid length ≈ 5,300 units ≈ 530 m ≈ **~30 s of real flight** — the right size for the experiment.

**Coordinate scale (from `deploy_attack_support_probe.ps1:1223-1229`): map units are DECIMETRES.** ~52 units ≈ 5 m; pads carry `{radius 150}` = 15 m; maps span ~19,000 units ≈ 1.9 km. So the plan's "150–250 m ring" is **1500–2500 units** — easy to get wrong by 10×.

**Sequence decided:**
1. claim by tag → own via the literal 1–16 `{player}` switch on `id_attack_support$` (copy `am_own_to_support`, `attack_support_waves.inc:521-547`, fail-closed default `player "3"`)
2. `{"placement"} {target_waypoint "attack_support_entry_<side>1"}`
3. `{"delay"}` → `{"air_state"} {altitude 30}`
4. `{"delay"}` → `{"actor_state"} {control AI} {movement {speed fast}}`
5. `{"delay"}` → `{"action"} {drop orders} {action move} {waypoint "attack_support_air_<side><n>"}`
6. arrival → announce + place team
7. depart: second `{action move}` back to the entry pad

**Arrival detection: time-based primary (~40 s), by design.** A distance/zone condition is unproven for this case. Recommended experiment hygiene: *also* evaluate a `{distance}` count into a separate mirror var so the live run self-reports whether distance detection *would* have worked, without depending on it.

**Enemy-proximity rejection:** copy the shipped guard shape verbatim from `attack_support_waves.inc:475-494` (`{type entities}` + `{selector … {tag _bot} {distance 120}}` + `{count {op ">"} {value 0}}` → fall back to the other pad). Caveat: that selector has **no explicit anchor entity**, so what `{distance 120}` is measured *from* is not established by reading alone — it is shipped and passing, so mirror its shape exactly rather than inventing a variant.

**Troops:** E1's pipeline already delivers to these same pads. Recommended: give E2 its **own** small 4-man pool + minimal placement/advance flow rather than poking `attack_support_wave_cmd$` / `attack_support_use_air$` / `attack_support_air_left$`. Those production budget vars are read by four `ally_*_air` triggers (`attack_support_waves.inc:2695,2796,2897,2998`) that roll their own rand and decrement the cap — coupling E2 to them risks corrupting production state and destroys the "provably inert when gated off" property.

**Gating:** `support_e2_test$` default 0, declared in `dcg_vars.inc` (93 lines, flat list of `{"name"}` entries; add `support_e2_test`, `support_e2_stage`, `support_e2_fail`, `support_e2_lz`). Mirror extension goes in `resource\script\multiplayer\modes\attack_support.lua:168-189`, `mirrorEngineState()` — add one `emit("mirror", "e2", …)` line using the existing pcall-guarded `readVar`.

---

## 4. Collision sweep — state at interruption

**Completed** (full scan of the four template pools):

| file | MID band | id band | y |
|---|---|---|---|
| `attack_support_templates.inc` | 9000–9083 (84) | 0xaf20–0xaf73 | −35100 |
| `enemy_defense_templates.inc` | 9100–9259 (160) | 0xb100–0xb19f | −35400 |
| `flag_props_templates.inc` | 9260–9274 (15) | 0xb040–0xb04e | −35200 |
| `faction_support_templates.inc` | 9300–9731 (411) | 0xb200–0xb39f | −35700, −36000, −36200, −36400 |

**Proposed free bands (NOT yet verified against the 14 deployed `.mi` files — the confirming sweep is the call that was killed):**
- MID **9800–9809** (highest in use is 9731)
- ids **0xb400–0xb40f** (highest in use is 0xb39f)
- position y = **−36600**, x band −4000…−3900 (lowest in use is −36400)

**The next implementer MUST still run:** `grep -rn '0xb4[0-9a-f][0-9a-f]' resource/map/` and `grep -rn 'Position -[0-9]* -366' resource/map/` and a `{MID 98xx}` scan **across the 14 `dcg_[cwa71]_*\campaign_capture_the_flag.mi` base maps**, not just the four `.inc` pools — base-map geometry carries its own ids and MIDs and was never checked.

---

## 5. Traps and dead ends

1. **`{"waypoint"}` and `{"actor_to_waypoint"}` do not take named waypoints.** I bucketed every `{waypoint "…"}` in all four trees by enclosing action: `{"waypoint"}` 2821 numeric / **0 named**; `{"actor_to_waypoint"}` 81 numeric / **0 named**. Only `{"action"} {action move} {waypoint "…"}` (26 named, all ours) and `{"placement"} {target_waypoint "…"}` accept names. **This kills the richer gostomel routed-flight idiom for us** unless someone allocates numeric waypoint ids in the generator. Use `{"action"} {action move}`. (Absence of evidence, not a proven engine restriction — but 2,902 shipped call sites with zero counter-examples.)

2. **`gostomel`'s pilot breeds are dangling.** `single_svo/rus/gostomel_vdv/vdv_vehicleman` and `single_svo/ukr/azov/spz_vehicleman` **do not exist** in any of the four trees — zero `single_svo` directories, zero `gostomel_vdv` grep hits across all of `content/400750`. Do not copy them. Use `mp/rusa/2022s/rus_pliot`.

3. **`rus_pliot` is spelled with the typo.** `rus_pilot` will silently fail.

4. **No ground-parked `{Chassis "helicopter"}` exists anywhere.** All 20 map-placed helicopter chassis blocks across all four trees carry `{Airborne}`; 11 also carry `{EngineStarted}` and `{Altitude}`. There is **no existence proof** that a ground-parked helo self-takes-off on a move order, and `altitude_checker` shows flight state is derived from altitude, not from orders. Park it airborne at `{Altitude 22}`; do not gamble on "order it and it climbs."

5. **`{Airborne}` is baked into `helicopter.ext`, not a per-instance opt-in.** The `{Chassis "helicopter" {Airborne} {EngineStarted} {Altitude N}}` block in a `.mi` is a **state snapshot** the editor writes, not a mode switch.

6. **Gostomel's `{Tags}` include runtime tags** — `wiper_worked`, `wheel_pullback`, `heli_in_flight` are added at runtime by `helicopter.inc`'s `altitude_checker` and were captured in a saved live state. Author only your own tags.

7. **Map coordinates are decimetres** (see §3). A 10× error here is the single easiest way to fly the helo off the map.

8. **`airborne_ce.inc`'s `drop_paratroopers` confirms the plan's known trap** (`resource\set\interaction_entity\airborne_ce.inc:298`): only `seat1..seat9` and `seat01..seat20` are ejected — `seat00` and `seat21+` never jump. Relevant only if anyone revisits F.

9. **CE waypoints 5004/5005/5006 do not exist in our submod.** `ce_generic_waypoint.inc` defines only `"3000"`. On our 14 maps that branch is additionally gated behind `ai_grid_system_active$ > 0` and falls through. Do not build on it — and the plan's existing "do not hook `paratrooper_need_orders`" instruction stands.

10. **`{"reinforcement"}` is not a spawn verb** — it only shows/hides menu entries (`CTA GoH Vanilla\…\04_reinforcements-offmap-support\0.mi:22065`). **No MI script in any of the four trees uses `{"spawn"}` to create an aircraft.** Every aircraft precedent is a pre-placed or cloned entity. Park it; don't try to spawn it.

11. **Cloning tension, noted not resolved:** `HF-dcg_script.inc:4598` successfully clones aircraft with `{"actor_to_waypoint"} … {clone} {approach "safe teleport & rotate"}`, while `attack_support_waves.inc:6-11` documents that cloning was proven unusable for our infantry (provenance invisible to selectors). HF gets away with it because the waypoint's own `{commands}` block re-tags the arrival with `{source standart}`. **The marching orders ban `{clone}` — obey that**; MOVE placement is the house rule and works fine here.

12. **West-81 supplies `airborne_helicopter.ext`, not Code:X** — and West-81 contains **zero** MI air scripting (no `air_attack`, `air_state`, `drop_plane` hits). Irrelevant to E2 if you use the non-airborne mi17, which is the recommendation.

13. **`car_crew.ext` (source of `crew_human`) is not in Code:X** — it resolves from the vanilla layer. Fine, but don't go looking for it in `3261086933\resource\properties\`.
