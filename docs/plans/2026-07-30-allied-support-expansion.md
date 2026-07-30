# Allied Support Expansion — Implementation Plan (APPROVED — executor: Grok)

> **MARCHING ORDERS FOR THE EXECUTOR (Grok).** These rules are non-negotiable; they exist because every one of them was earned by a production incident in this repo:
> 1. **Repo-first, always.** Edit files in `CodeX AI Overhaul Submod` (git), never in the workshop folder `3636883799`. The ONLY writer to the workshop is `tools\deploy_attack_support_probe.ps1`. Workshop-first edits have already nearly destroyed 2,900 lines of work once.
> 2. **Commit and push per phase.** Do not report work as "in git" unless `git log` proves it. End every commit message with your own attribution trailer.
> 3. **Run the full test suite (`python -m pytest tests/`) and the deploy script (must exit 0, twice, byte-identical) before declaring any phase done.** If a guard or test blocks you, the fix is to rename/re-approach your change — NEVER to loosen a guard or delete a pin. The substring `allied_support` is banned by design; so are third-party mod names in code/comments.
> 4. **Engine rules (violations = silent CTDs or invisible units, all previously hit live):** MOVE placement only, never `{clone}`; bare `{select {tag …}}` selectors on parked templates — no `{include {prop human}}` or `{state operatable}` decorations; `SetVar` integers only; literal 1–16 `{player}` switches with fail-closed defaults (never transfer on unresolved id); define-before-use for every Lua local; MI delimiter balance; new entity blocks need fresh MID/id/position bands (collision-sweep against ALL existing pools).
> 5. **Diagnostics vs announcements:** debug timers behind `support_debug$` (default 0); player-facing announcements behind `support_announce$` (default 1). Never mix.
> 6. **CE mirror rule:** anything under `resource/map/multi/ce/` is duplicated byte-identically to `resource/map_scripts/`.
> 7. **Asset strings:** only the verified names in this document. Any new entity/breed string requires an existence proof (`.set` under Code:X breed tree / `.def` in entity.pak) BEFORE use.
> 8. Phases in order, live-test gates respected. If something is half-done at session end, commit the clean subset and report the remainder honestly.

**Goal:** Add announcements, flanking arrivals, a rare IFV wave, and unmanned flag props to the four-quadrant support system — using only assets verified to exist in this stack.

**Architecture:** Every feature reuses the proven MI delivery pipeline (parked real-breed pool → runtime tag claim → MOVE placement → literal 1–16 `{player}` switch → orders). No Lua spawning, no flight simulation, no CE modifications except the two designated dead-code insertion points. Repo-first through `tools\deploy_attack_support_probe.ps1`; nothing edits the workshop directly.

**Tech stack:** GoH MI triggers, Code:X 3261086933 assets (verified), gettext `.pot` localization, existing deploy geometry generator.

## Corrections to the originating proposal (recon-verified)

1. `mi17_rus` has **no passenger seats** — cannot carry troops. In-stack transport is `mi17_b8_rus` / `mi17_b8_ukr` (12 seats, verified).
2. `uh-60m_blackhawk_mg` and `c130_para` live in **West-81 (2897299509), not Code:X** — using them makes West-81 a hard dependency (it already is de facto, but must be stated).
3. `c130_paratrooper_82nd` / `il-76td_paratrooper` are **conquest squad defs, not breeds**; no paradrop breed variants exist.
4. **PRC has no air path at all** and our own `doctrine_units_prc.set:3` forbids aircraft — PRC is scoped out of anything airborne.
5. **No `world/call-ins` sound bank exists** in Code:X or base — VO reuse as proposed is impossible; verified base-game sounds (`interface/task_new`, `world/alarm/*`, `interface/morse`) are the available palette.
6. CE paratrooper machinery (`ai_logic/paratrooper_orders`, tag lifecycle) **is real and complete** — but lives in TWO mirrored trees (`resource/map/multi/ce/` AND `resource/map_scripts/`) that must always be edited together.
7. ~25 existing `mission/multi/*` keys referenced by CE triggers are **unlocalized** (render as raw keys) — pre-existing defect, cheap optional fix.
8. "Give troops to AI" community ask: **already shipped** — support units are bot-owned and MI-ordered. Answer the commenter yes.

## Global constraints (apply to every phase)

- Repo-first; deploy script is the only writer to the workshop; hash-verify after every phase.
- MOVE placement only — never `{clone}`; bare `{select {tag …}}` selectors on parked templates; no `{include {prop human}}`/`{state operatable}` decorations; `SetVar` integers only; literal `{player}` switches with fail-closed defaults.
- New on-screen **diagnostics** gate behind `support_debug$` (default 0). New **player-facing announcements** gate behind new `support_announce$` (default 1) — never behind `support_debug$`.
- No third-party mod references in any new code/comments (permission header in `codex_ai_combat.inc` is the sole exception).
- Every entity/breed string must appear in the verification tables below (all recon-proven) — any addition requires a new existence check first.
- Each phase: tests + deploy markers updated in the same commit series; full suite green; deploy exit 0 twice (idempotent); MID bands documented and collision-swept.
- CE mirror rule: any change under `resource/map/multi/ce/` is duplicated to `resource/map_scripts/` byte-identically.

---

### Phase 1 — Localized announcements (Grok item 1) — LOW RISK

**Files:**
- Create: `localizations/default/interface/text/mission/multi/support_events.pot`
- Modify: `resource/map/multi/attack_support_waves.inc`, `defense_support_waves.inc`, `enemy_attack_support.inc`, `enemy_defense_support.inc` (announce blocks at wave-dispatch/garrison/flank events)
- Modify: `resource/map/multi/dcg_vars.inc` (+`support_announce`), `tools/deploy_attack_support_probe.ps1` (ship the .pot + markers), tests.

**Mechanism (verified in-stack):** `{"timer"}` headline with a localized `{title "mission/multi/support/<key>"}` for major events + `{"sound" {name "interface/task_new"}}` for friendly arrivals / `{"sound" {name "world/alarm/sirene"}}` for enemy pressure on defense. `{"talk"}` variant (Code:X `dcg_script.inc:9858` pattern with tag-cursor anti-stacking) reserved for flank/airborne flavor lines. `.pot` format: `msgctxt "mission/multi/support/<key>"` with **filled `msgstr`** (our repo's convention; avoids Code:X's `mmsgstr` typo class).

**Keys (initial set, one per event class × side):** `support/wave_inbound`, `support/vehicle_inbound`, `support/flank_inbound`, `support/waves_exhausted`, `support/defense_reinforced`, `support/enemy_activity` (enemy waves get deliberately vague text — intel flavor, not omniscience).

**Gates:** every announce block wrapped in `support_announce$ == 1` switch.

**Steps:** write `.pot` → pin key-coverage test (every referenced key has a msgctxt entry; parse balance) → add announce blocks per engine → deploy marker + copy-list updates → full suite → deploy ×2 → commit "Add localized support announcements" → push.

**Optional bonus (separate commit, pre-approved?):** add the ~25 missing `msgctxt` entries for CE's orphaned keys ("Fix unlocalized CE mission messages").

### Phase 2 — Flanking arrival pads (Grok item 3) — LOW-MEDIUM RISK

**Files:** `tools/deploy_attack_support_probe.ps1` (geometry generator: 2 flank pads per side), `attack_support_waves.inc` (roll + guard + round-robin), `dcg_vars.inc` (+`attack_support_flank_rr`), tests, 14 maps via deploy.

**Geometry:** `attack_support_flank_<side>1/2` at 50% depth toward map center from the side's spawn centroid, laterally offset ±35% of the perpendicular spawn-line spread (knobs `FLANK_DEPTH`, `FLANK_SPREAD` beside `EDGE_FACTOR`). Generated and self-healed exactly like the existing pads.

**Behavior:** friendly **attack** waves only (Q1). Per wave: 25% roll (`{type rand} {value 0.25}`) to arrive via flank; round-robin between the two flank pads; **safety guard** — if any enemy human within 120 of the chosen pad (`{distance 120}` simple-selector count), fall back to main pads this wave. Announcement `support/flank_inbound` fires only on actual flank use. Enemy engines get a test pin asserting they never reference flank pads.

**Steps:** generator + pads → engine roll/guard/rr → pins (2 pads/side/map; Q1-only; guard present) → suite → deploy ×2 → commit "Add flanking arrival pads for attack support" → push.

### Phase 3 — Rare mechanized IFV wave (Grok item 2, ground slice) — MEDIUM RISK

**Verified vehicles (Code:X entity.pak, crew/passenger slots confirmed):**

| Faction | Hull | Crew breeds (verified) | Dismounts |
|---|---|---|---|
| RUSA | `bmp2_rus` | 3× `mp/rusa/2022s/rus_vehicleman` | 4× rus90 line |
| NATO | `m2a3` | 3× `mp/nato/2022s/usarmy_crew` | 4× nato line |
| UKR | `bmp2_ukr` | 3× `mp/ukr/2022s/ukr_vehicleman` | 4× ter line |
| PRC | `zbl08` (proper passenger groups; avoid `zbd04_a`'s group(crew) quirk) | 3× `mp/prc/2022s/pla_crew` | 4× pla line |

**Files:** `faction_support_templates.inc` (4 IFV packages, new MID band **9300–9339**, fresh x-band — collision sweep required), `attack_support_waves.inc` (cmd 17), `dcg_vars.inc` (+`attack_support_ifv_left`), tests, deploy markers.

**Behavior:** attack-only, L2 = 10% roll / L3 = 15%, hard cap `attack_support_ifv_left$ = 1` per mission, consumes 1 wave from the normal budget. Template mirrors the proven humvee package structure (Link-baked crew, hull placed last on the countdown so crews never separate; dismounts advance with the wave). Announcement `support/vehicle_inbound`.

**Steps (user decision: ALL FOUR factions in one series):** build all four IFV packages together; pins: entity names, cap, level gating, attack-only, MID separation, pool-depth math note. Live-test gate after (one L2+ attack mission) before Phase 4.

### Phase 4 — Unmanned flag props (community ask 2) — MEDIUM RISK

**Verified assets:** crate `para_ammo` (Code:X `codx_inventory/inf_crate_fin/para_ammo.def` — modern, ammo-supply 250, open inventory box); crewless weapons `mg_stand_nsvt_rus_ai`, `mg_stand_nsvt_ukr_ai`, `mg_stand_qjz171` (PRC), `bgm71_tow_ai` (NATO), `spg9_ai`. Spawn verb precedent: `{"spawn"} {entity "…"} {waypoint …}` (`base_map_setup_triggers.inc:9041`) and offset-spawn (`vehicle_ce.inc:625`).

**v1 scope (user decision: DEFENDER-SIDE ALWAYS, both mission types — mirrors vanilla's defender-emplacement logic):** props spawn at the active flags of whichever side is DEFENDING that mission. Human-defense missions: placed by `defense_support_waves.inc`'s garrison step at prep end, weapon faction via `user_nation$` fold. Human-attack missions: placed by `enemy_defense_support.inc`'s garrison step at its init, weapon faction via `bot_army$` fold. Per active flag: 1 `para_ammo` crate always; at L2+ additionally 1 faction-matched crewless weapon (`mg_stand_nsvt_rus_ai` / `mg_stand_nsvt_ukr_ai` / `mg_stand_qjz171` / `bgm71_tow_ai`). Spawned unowned (player-0) → mannable by anyone on either side, exactly the MoW feel requested. New tag namespace `flag_prop` + test-pinned exclusion from every engine's claim selectors. **No CE modification** — both placements run from our own engines' garrison steps. Weapon-in-crate inventory injection deferred to v2 pending item-name recon.

**Steps:** prop placement block in `defense_support_waves.inc` garrison phase → tag exclusions swept across engines → pins (defense-only, 1/flag caps, entity names, exclusions) → suite → deploy ×2 → live-test gate → commit "Add unmanned supply and weapon props to defended flags" → push.

### Phase 5 — Airmobile insert (E1) — APPROVED (2026-07-30) — LOW RISK

**Concept:** "helicopter-inserted" fireteams with zero actual aircraft: announcement + audio cue, then a 4-man team appears at a deep pad. The helicopter is narrative; the delivery is the proven teleport pipeline.

**Files:** `attack_support_waves.inc` (airmobile roll + insert flow), `support_events.pot` (+`support/airborne_inbound` key, faction-flavored text mentioning UH-60M / Mi-8 / Mi-17 / Mi-171 per faction), deploy generator if a deeper pad tier is added (`attack_support_air_<side>` at ~65% depth, knob `AIR_DEPTH`), `dcg_vars.inc` (+`attack_support_air_left`), tests, deploy markers.

**Behavior:** attack missions, L2+; ~15% of waves upgrade to airmobile (rand case before the normal comp pick); announcement fires, then after a 4–5s beat the team is placed at the air pad (enemy-proximity guard 120, fall back to normal delivery on failure — announcement suppressed in that case, or use a "wave inbound" fallback line). Cap `attack_support_air_left$ = 2` per mission. Composition: faction line or recon fireteam (existing verified pools — 82nd/45vdv flavor comes from the announcement text and, where pool depth allows, the 82nd/45vdv breeds already parked).

**Factions: ALL FOUR (user decision).** Flavor mapping — NATO: UH-60M; RUSA: Mi-8/Mi-17; UKR: Mi-8/Mi-17; PRC: Mi-171/Mi-171Sh (Army Aviation). No aircraft entity is used in E1, so PRC needs no new asset for this phase.

**Doctrine revision (user-directed, part of this phase):** amend the PRC restriction comment in `resource/set/multiplayer/units/2022s/doctrine_units_prc.set` from "no aircraft/helicopters" wording to: *"No offensive air or fixed-wing call-ins. Army Aviation troop transport (Mi-171 family) is allowed."* Comment-only change; no roster edits.

**Do NOT hook CE's `paratrooper_need_orders` lifecycle** — its order path (waypoints 5004–5006 / grid effects) conflicts with our ordering. Our own order flow applies after insert.

### Parked (approval recorded 2026-07-30)

- **E2 — flyover theater (PARKED, time-boxed experiment AFTER Phases 1–5 ship):** a real transport entity crossing the map as set dressing during airmobile inserts. Assets: `uh-60m_blackhawk_mg` (West-81 — dependency already declared by the stack), `mi17_b8_rus`/`mi17_b8_ukr` (Code:X). **PRC deliverable for E2:** a PLA-owned Mi-171Sh adaptation cloned from the in-stack `mi17_b8` asset (in-repo `.def` clone; precedent: `resource/entity/construction/_military/fortifications/*.def`). Time-box: one session; success criterion = reliable scripted flight across two maps without crashes; failure = kill E2 permanently.
- **F — real paradrop (PARKED INDEFINITELY):** revisit ONLY if E2 proves aircraft movement reliable. Assets if ever: `il-76td_para` (+`_ai`), `c130_para` (West-81); known trap: `drop_paratroopers` ejects only seats 1–20 (`seat00`, `seat21+` never jump).
- The dead WW2 `drop_vehicle` block in `airborne_ce.inc` (~35 lines, unreachable faction gates) remains the reserved insertion point for any future modern crate/emplacement drop.

## Execution order & approval gates

| Phase | Risk | Live-test gate before next |
|---|---|---|
| 1 Announcements (+optional CE key fix) | Low | No (log-verifiable) |
| 2 Flank pads | Low-Med | Yes — one attack mission |
| 3 IFV (RUSA pilot → all four) | Med | Yes — L2+ attack mission |
| 4 Flag props | Med | Yes — one defense mission |
| 5 Airmobile insert (E1) | Low-Med | Yes — one L2+ attack mission |

## Approval record (2026-07-30, from user)
1. ✅ Phases 1–4 approved in order, live-test gates respected.
1b. ✅ Phase 5 = E1 airmobile insert, all four factions, PRC doctrine comment revised (Army Aviation transport allowed). E2 parked as a post-ship time-boxed experiment; F parked indefinitely, E2-conditional.
2. ✅ Phase 1 bonus approved: fix the ~25 orphaned CE `mission/multi/*` localization keys (separate commit).
3. ✅ Phase 3: ALL FOUR factions in one series (no single-faction pilot).
4. ✅ Phase 4: defender-side ALWAYS, both mission types (see revised v1 scope above).

Executor: Grok (external session), under the marching orders at the top of this document. Post-execution audit by the Claude toolchain against this document is expected — deviations from the marching orders will be treated as defects even if the feature works.
