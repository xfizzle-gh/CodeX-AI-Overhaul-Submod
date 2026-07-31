# Allied Support Expansion — Implementation Plan (APPROVED — executor: Grok)

> **MARCHING ORDERS FOR THE EXECUTOR (Grok).** These rules are non-negotiable; they exist because every one of them was earned by a production incident in this repo:
> 1. **Repo-first, always.** Edit files in `CodeX AI Overhaul Submod` (git), never in the workshop folder `3636883799`. The ONLY writer to the workshop is `tools\deploy_attack_support_probe.ps1`. Workshop-first edits have already nearly destroyed 2,900 lines of work once.
> 2. **Commit and push per phase.** Do not report work as "in git" unless `git log` proves it. End every commit message with your own attribution trailer.
> 3. **Run the full test suite (`python -m pytest tests/`) and the deploy script (must exit 0, twice, byte-identical) before declaring any phase done.** If a guard or test blocks you, the fix is to rename/re-approach your change — NEVER to loosen a guard or delete a pin. The substring `allied_support` is banned by design; so are third-party mod names in code/comments.
> 4. **Engine rules (violations = silent CTDs or invisible units, all previously hit live):** MOVE placement only, never `{clone}` — **one scoped exception, see below**; bare `{select {tag …}}` selectors on parked templates — no `{include {prop human}}` or `{state operatable}` decorations; `SetVar` integers only; literal 1–16 `{player}` switches with fail-closed defaults (never transfer on unresolved id); define-before-use for every Lua local; MI delimiter balance; new entity blocks need fresh MID/id/position bands (collision-sweep against ALL existing pools).
>    - **`{clone}` exception (approved 2026-07-30, E2 AIRCRAFT DISPATCH ONLY).** The ban exists because a freshly created entity's provenance is invisible to every selector this stack can express — three promote designs each matched zero clones. The base game's own aircraft call-in solves exactly that, and only for aircraft: it clones a hidden parked template to a **numeric** waypoint whose `{commands}` block then runs *on the arriving actor* and re-tags it. E2 aircraft dispatch adopts that recipe verbatim. `{clone}` may appear nowhere else under `resource/`; that scope is test-pinned repo-wide (`E2ScopedCloneExceptionTests`), including a frozen list of the four vendored files that already shipped with a `{clone}` we did not author. Infantry and vehicles remain MOVE-placement only.
> 5. **Diagnostics vs announcements:** debug timers behind `support_debug$` (default 0); player-facing announcements behind `support_announce$` (default 1). Never mix.
> 6. **CE mirror rule:** anything under `resource/map/multi/ce/` is duplicated byte-identically to `resource/map_scripts/`.
> 7. **Asset strings:** only the verified names in this document. Any new entity/breed string requires an existence proof (`.set` under Code:X breed tree / `.def` in entity.pak) BEFORE use.
> 8. Phases in order, live-test gates respected. If something is half-done at session end, commit the clean subset and report the remainder honestly.

**Goal:** Add announcements, flanking arrivals, a rare IFV wave, and unmanned flag props to the four-quadrant support system — using only assets verified to exist in this stack.

**Architecture:** Every feature reuses the proven MI delivery pipeline (parked real-breed pool → runtime tag claim → MOVE placement → literal 1–16 `{player}` switch → orders). No Lua spawning, no flight simulation, no CE modifications except the two designated dead-code insertion points. Repo-first through `tools\deploy_attack_support_probe.ps1`; nothing edits the workshop directly.

**Tech stack:** GoH MI triggers, Code:X 3261086933 assets (verified), gettext `.pot` localization, existing deploy geometry generator.

## Corrections to the originating proposal (recon-verified)

1. `mi17_rus` has **no passenger seats** — cannot carry troops. In-stack transport is `mi17_b8_rus` / `mi17_b8_ukr` (12 seats, verified).
2. `uh-60m_blackhawk_mg`, `c130_para` **and `il-76td_para`** live in **West-81 (2897299509), not Code:X** — using them makes West-81 a hard dependency (it already is de facto, but must be stated). Note both paradrop planes are West-81, so there is no "NATO = West-81, everyone else = Code:X" split on the para leg; see the E2 airframe findings below. `uh-60m_blackhawk_mg` is now blocked pending an instantiation proof and NATO's E2 helo flies the Code:X `mi17_b8_rus` hull.
3. `c130_paratrooper_82nd` / `il-76td_paratrooper` are **conquest squad defs, not breeds**; no paradrop breed variants exist.
4. **PRC has no air path at all** and our own `doctrine_units_prc.set:3` forbids aircraft — PRC is scoped out of anything airborne.
5. **No `world/call-ins` sound bank exists** in Code:X or base — VO reuse as proposed is impossible; verified base-game sounds (`interface/task_new`, `world/alarm/*`, `interface/morse`) are the available palette.
6. CE paratrooper machinery (`ai_logic/paratrooper_orders`, tag lifecycle) **is real and complete** — but lives in TWO mirrored trees (`resource/map/multi/ce/` AND `resource/map_scripts/`) that must always be edited together.
7. ~25 existing `mission/multi/*` keys referenced by CE triggers are **unlocalized** (render as raw keys) — pre-existing defect, cheap optional fix.
8. "Give troops to AI" community ask: **already shipped** — support units are bot-owned and MI-ordered. Answer the commenter yes.

## Global constraints (apply to every phase)

- Repo-first; deploy script is the only writer to the workshop; hash-verify after every phase.
- MOVE placement only — never `{clone}`, except the E2 aircraft-dispatch exception recorded in marching order 4; bare `{select {tag …}}` selectors on parked templates; no `{include {prop human}}`/`{state operatable}` decorations (the one `{include {tag {tag hidden}}}` in the E2 clone dispatch is part of the same exception and is what lets the selector reach a hidden parked template at all); `SetVar` integers only; literal `{player}` switches with fail-closed defaults.
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

**v1 scope (user decision: DEFENDER-SIDE ALWAYS, both mission types — mirrors vanilla's defender-emplacement logic):** props spawn at the active flags of whichever side is DEFENDING that mission. Human-defense missions: placed by `defense_support_waves.inc`'s garrison step at prep end, weapon faction via `user_nation$` fold. Human-attack missions: placed by `enemy_defense_support.inc`'s garrison step at its init, weapon faction via `bot_army$` fold. Per active flag, at L2+: 1 faction-matched crewless weapon (`mg_stand_nsvt_rus_ai` / `mg_stand_nsvt_ukr_ai` / `mg_stand_qjz171` / `bgm71_tow_ai`). Spawned unowned (player-0) → mannable by anyone on either side, exactly the MoW feel requested. New tag namespace `flag_prop` + test-pinned exclusion from every engine's claim selectors. **No CE modification** — both placements run from our own engines' garrison steps.

**Crate half RETIRED (user decision, 2026-07-30 — superseded, see Phase 6).** The `para_ammo` crate that v1 placed on every defended flag is gone: the three prototypes (ids `0xb040`-`0xb042`, MIDs 9260-9262) are out of `flag_props_templates.inc`, which is now a 12-prototype weapons-only pool, and the claim/place blocks are out of both garrison paths. Reason: Phase 6 gives every flag a real linked supply point, which is strictly better in every dimension — it is the vanilla mechanism, it follows the flag when it changes hands, it costs no pool depth and it needs no engine step. The crate would have been a second, worse supply source sitting on top of it. Test-pinned as retired in all three files; the L2+ crewless weapon placement is untouched.

**Steps:** prop placement block in `defense_support_waves.inc` garrison phase → tag exclusions swept across engines → pins (defense-only, 1/flag caps, entity names, exclusions) → suite → deploy ×2 → live-test gate → commit "Add unmanned supply and weapon props to defended flags" → push.

### Phase 6 — Live ammo supply on every flag (community ask, vanilla mechanism) — SHIPPED 2026-07-30

**Problem:** every flag in the CWA CTF family carries an empty built-in placer socket, `{Placer {State "ammo" {Unlinked}}}`, and nothing on the map ever fills it. Holding a flag bought the holder no resupply at all.

**Mechanism (recon-verified against the base game, no invention):** the base game's own CTF maps fill that socket the other way round and never carry the Placer block — a childless `{Entity "flagpoint_ammo"}` holding the `supply_zone` extender, plus a `{Link <child> {<flag> "ammo"}}` line binding it into the slot. Reference shape: `multi/2v2_countryside/battle_zones.mi` lines 353-357 and 401. The two forms are mutually exclusive; the empty socket is removed as the link lands.

**Modern ammo table:** shipped as a shadow def at the vanilla virtual path, `resource/entity/service/-multiplayer/flag_point/flagpoint_ammo/flagpoint_ammo.def`, byte-identical to the base def except `(include "/properties/resupply.inc")` → `(include "/properties/resupply_hotmod.inc")`. Its `("flag_ammo_heavy")` call then resolves to Code:X's `resupply_hotmod.inc:1019` — 24m radius, 5s regeneration, limit 750, modern items — instead of the base table, whose items are WW2 and whose regeneration is disabled by gameclass. Only the `.def` is shadowed: the `.mdl` and `supply_zone.ebm` resolve from the pak through the same virtual path (precedent: `barbwire_on_wall.def`).

**Delivery:** an idempotent, self-healing step in `tools/deploy_attack_support_probe.ps1` (`Set-FlagAmmoSupply`), same shape as the waypoint generator — strip everything the previous run wrote, then rebuild from the flags actually present. Runs over the deployed copy AND the repo copy of each map, so repo == workshop. Child ids come from `0xfd00` upward, a band collision-swept across every `.mi` and `.inc` in `resource/map/multi` (highest id anywhere else is `0xf801`) and re-asserted per file. **48 supply points across the 14 managed maps.**

**Scope:** exactly the 14 `dcg_[cwa71]_*` CTF maps the deploy owns. `bakhmut_1`, `forest_` and `map_ukrcity` also ship CTF maps but are NOT managed by this deploy — no includes, no waypoints, no supply points — and that exclusion is now test-pinned so widening it has to be deliberate.

### Phase 5 — Airmobile insert (E1) — APPROVED (2026-07-30) — LOW RISK

**Concept:** "helicopter-inserted" fireteams with zero actual aircraft: announcement + audio cue, then a 4-man team appears at a deep pad. The helicopter is narrative; the delivery is the proven teleport pipeline.

**Files:** `attack_support_waves.inc` (airmobile roll + insert flow), `support_events.pot` (+`support/airborne_inbound` key, faction-flavored text mentioning UH-60M / Mi-8 / Mi-17 / Mi-171 per faction), deploy generator if a deeper pad tier is added (`attack_support_air_<side>` at ~65% depth, knob `AIR_DEPTH`), `dcg_vars.inc` (+`attack_support_air_left`), tests, deploy markers.

**Behavior:** attack missions, L2+; ~15% of waves upgrade to airmobile (rand case before the normal comp pick); announcement fires, then after a 4–5s beat the team is placed at the air pad (enemy-proximity guard 120, fall back to normal delivery on failure — announcement suppressed in that case, or use a "wave inbound" fallback line). Cap `attack_support_air_left$ = 2` per mission. Composition: faction line or recon fireteam (existing verified pools — 82nd/45vdv flavor comes from the announcement text and, where pool depth allows, the 82nd/45vdv breeds already parked).

**Factions: ALL FOUR (user decision).** Flavor mapping — NATO: UH-60M; RUSA: Mi-8/Mi-17; UKR: Mi-8/Mi-17; PRC: Mi-171/Mi-171Sh (Army Aviation). No aircraft entity is used in E1, so PRC needs no new asset for this phase.

**Doctrine revision (user-directed, part of this phase):** amend the PRC restriction comment in `resource/set/multiplayer/units/2022s/doctrine_units_prc.set` from "no aircraft/helicopters" wording to: *"No offensive air or fixed-wing call-ins. Army Aviation troop transport (Mi-171 family) is allowed."* Comment-only change; no roster edits.

**Do NOT hook CE's `paratrooper_need_orders` lifecycle** — its order path (waypoints 5004–5006 / grid effects) conflicts with our ordering. Our own order flow applies after insert.


**Defense-side airmobile parity added post-approval (2026-07-30):** E1 was attack-only in the original approval; defense_support_waves.inc now mirrors it (cmd 18, L2+ ~15%, `defense_support_air_left$` cap 2, faction talk keys, 120-unit enemy-proximity guard falling back to edge pads, deep `attack_support_air_*` LZs). **No aircraft entity** — narrative helo only; delivery is teleport placement. Day-2 force toggle `attack_support_air_test$` is **shared**: attack init sets it on attack missions; defense `garrison_init` sets it on defense missions (same default ON for testing; set 0 to ship production-only rolls).

### Parked (approval recorded 2026-07-30)

- **E2 — flyover theater (PARKED, time-boxed experiment AFTER Phases 1–5 ship):** a real transport entity crossing the map as set dressing during airmobile inserts. Assets: `uh-60m_blackhawk_mg` (West-81 — dependency already declared by the stack), `mi17_b8_rus`/`mi17_b8_ukr` (Code:X). **PRC deliverable for E2:** a PLA-owned Mi-171Sh adaptation cloned from the in-stack `mi17_b8` asset (in-repo `.def` clone; precedent: `resource/entity/construction/_military/fortifications/*.def`). Time-box: one session; success criterion = reliable scripted flight across two maps without crashes; failure = kill E2 permanently.

#### E2 flight mechanism — CANONICAL as of 2026-07-30 (supersedes the air_state/long-route design)

Two prior passes built E2 flight on `{"air_state"}` + `{"action"} {action move}` from a **hand-forged** parked-aircraft snapshot. Live result: the NATO helo claim returned no aircraft (fail 9), and the para leg walked stage 30 for ~92 s with **no aircraft in the sky and no minimap contact** before failing 6. The base game's own aircraft call-in has now been extracted and it is a **completely different mechanism**. Our snapshot was invented; the base game's is editor-written. E2 is rebuilt on the base game's.

**1. The parked block is a chassis STATE SNAPSHOT, and the two airborne families use two different vocabularies.** A census of every `{Chassis "…"}` block in every `.mi`/`.inc` in the installed stack splits cleanly with no overlap at all:

| chassis | blocks | state tokens, in every single one | siblings |
|---|---|---|---|
| `"airborne"` (fixed wing) | 1523 | `{AirborneMode 1}` `{Altitude N}` — **never** `{Airborne}`/`{EngineStarted}` | `{ChassisManager {Current "airborne"}}` (1486), `{DisableObstacles}` |
| `"helicopter"` | 58 | `{Airborne}` `{EngineStarted}` `{Altitude ~22}` — **never** `{AirborneMode}` | none required (`helicopter.ext` already defaults `ChassisManager` to `"helicopter"`) |

So the **helicopter** snapshot we already shipped was correct; the three **fixed-wing** hulls were written in the helicopter vocabulary and are now converted. Both families additionally carry the explicit `{ChassisManager}` sibling and `{DisableObstacles}` (what every editor-written *parked, hidden* aircraft carries). Positions stay 2D — altitude is the snapshot's job, never the position's. Test-pinned in both directions, so neither vocabulary can leak into the other again.

*Correction to an earlier entry below:* `il-76td_para.def` and `c130_para.def` **both already declare** `{Chassis "airborne" {Altitude 65}}` + `{ChassisManager {Current "airborne"}}` at def level (il-76 in both the base body and `(mod "mp")`, the C-130 in `(mod "mp")`). The "`Airborne_M.ext` defaults to wheel, therefore the planes sat on the ground" explanation is therefore **not** the whole story — the def overrides that default. The invented `{Airborne}`/`{EngineStarted}` tokens on an airborne chassis are the more likely cause.

**2. Dispatch CLONES and TELEPORTS; it does not fly from the park.** `{"actor_to_waypoint"}` with `{source advanced}`, `{include {tag {tag hidden}}}` (without which the selector cannot reach a hidden parked template), `{amount 1}`, a **numeric** `{waypoint}`, `{clone}` and `{approach "safe teleport & rotate"}`.

**3. The arrival is driven by the destination waypoint's own `{commands}` block**, which runs *on the arriving actor* — re-tag, `{"actor_state"}`, first move order, `{effect takeoff_load}`. That is how the base game defeats "a clone carries none of the parked template's tags".

**Consequence — aircraft call-ins are now REPEATABLE and cost the pool nothing.** The parked originals are never consumed: the dispatch marks one hull with `support_e2_src`, clones it, and clears the marker. Budget bookkeeping is simplified accordingly — the E2 helo leg's only pool spend is its four independent troops, and the E2 para leg spends nothing at all. Crews stay `{Link}`-baked in the parked hulls forever and ride the clone as copies.

**Numeric waypoint band 9101–9104, deploy-generated per map, collision-swept.** `{"actor_to_waypoint"}` and the `{waypoint}` order term accept numeric names only, so these cannot use the `attack_support_*` pad naming. Sweep across all fourteen managed maps and every `.inc`: the only numeric waypoint names present are `"0"` (on **all fourteen**, and on `factory`/`train_station` it is the base game's airstrike entry node with its own `enemy_air` `{commands}` block) and `"1".."6"` (on `factory` and `train_station` only — also base airstrike geometry). None is touched. 9101/9102 = air entry per spawn side (z 0, `{radius 800}`, `{commands}`); 9103/9104 = air exit (z 170, no radius, no commands — the base game's mid-route altitude-node shape). Generated idempotently and self-healing by `tools/deploy_attack_support_probe.ps1` via a brace-matching stripper, because the flat pad regex cannot reach a nested `{commands}` block.

**Namespace.** The base game's `ai_air` chain is **live** in this stack — `airstrike_ger/fin/usa/eng/rus`, `enemy_air` and `ai_air_target` survive into the shared `dcg_script.inc` and into two managed maps. Every tag E2 writes is in the `support_e2_*` space, and "no engine of ours names those three" is test-pinned.

**Paradrop release.** `drop_paratrooper` (singular) is the existence-verified receiver for both airframes — Code:X declares `{on "drop_paratrooper"}` in its own `"il-76td_para"` and `"c130_para"` interaction groups. What it does there is **open the cargo bay** (`damage_report component/cargobay_opened` → `open_cargo`); it does **not** unlink the seats. The actual release is `{"emit"} … {emit {mode passengers}}`, the same verb the motorised insert already uses. `drop_paratroopers` (plural, the seat-by-seat ejector with the `seat1..seat9`/`seat01..seat20` limitation) belongs to a different prop group these airframes do not carry.

**Jumper ordering is deliberately NOT ours.** `attack_support/e2_para_landed` is retired: it selected landed jumpers by `support_e2_para_pax` + `support_e2_claim`, and under the clone dispatch the jumpers are copies that carry none of this engine's tags — no selector form can tell one aircraft's untagged jumpers from anyone else's. Ordering is left to CE's own paratrooper lifecycle, which already owns everything it tags `paratrooper_need_orders`; E2 only *proves* a jumper landed (a live `paratrooper_need_orders` human within 6000 of the chosen flag, else fail 7). This engine issues no order to a CE-tagged paratrooper, so the ordering conflict the plan warns about cannot arise.

**New fail code 14** (append-only, 1–13 unchanged): "the clone never arrived" — `{"actor_to_waypoint"} … {clone}` produced nothing for the destination waypoint's `{commands}` block to tag. 13 / 14 / 9 stay three separate facts: nothing was parked / the parked hull was copied but no copy reached the waypoint / a flying aircraft has gone (stage-30 liveness monitor).

#### E2 airframe findings (recon-verified 2026-07-30, live run with `-E2TestMode 3`)

**`uh-60m_blackhawk_mg` verdict — RE-EVALUATED 2026-07-30 against the canonical form, and STILL BLOCKED.** The canonical snapshot does not change its situation: the Blackhawk is helicopter-chassis, and the *helicopter* vocabulary is the one form that was already correct. Its previous park block therefore had nothing wrong with it that the restructure fixes, so no new evidence exists either way and there is nothing to unblock it with short of another live run. NATO stays on `mi17_b8_rus`; the airframe stays out of every live pool (deploy throws on an `{Entity "uh-60m_blackhawk_mg"`, test-pinned as comment-only); fail code 13 remains the discriminator. The original record follows — the def is *not* the blockage:

- It is **helicopter-chassis-compatible, not airborne-family**: `(include "/properties/helicopter.ext")`, `{PatherID "helicopter"}`, `{targetClass "helicopter"}`, `{ObstacleId "helicopter" "in_air"}`, and its own `{Chassis "helicopter" {Locomotion ("locomotion_transport")}}`. The `para_form_air` string in its `{props}` is a receiver name in Code:X `set/interaction_entity/helicopter.inc`, **not** a chassis family. Every macro it uses (`helicopter_tier2`, `crew_visible`, `armor_medium`, `locomotion_transport`, `missile_aimpoint_air`) resolves under the Code:X layer that shadows `helicopter.ext`. So our park snapshot `{Chassis "helicopter" {Airborne}{EngineStarted}{Altitude 22}}` **was valid for this def**.
- Its **def demonstrably loaded at map parse** in the live run: it emitted `no "pivot_front" & "pivot_back" bones in uh-60m_blackhawk_mg[0x0 raw] (/properties/helicopter.ext, 119)` and `(…, 141)` — the same two lines `mi17_b8_rus` produces, i.e. through the same Code:X `helicopter.ext`.
- What was **never proved is that the parked ACTOR instantiated**. Unlike `mi17_b8_rus`, `mi17_b8_ukr`, `shaanxi_sx2190_passenger`, `bmp2_*` and `m2a3` — all of which produced `[0x0 actor] (…/faction_support_templates.inc, NNNN)` lines — the Blackhawk produced none, and the **NATO helo claim came back empty** (`e2_combo_helo_fail 9`) even though `attack_support/e2_helo_nato` had fired, meaning its condition terms 8/9/10 had already counted the parked hull, 2 crew and 4-man team.

**Action taken:** NATO's E2 helo hull is now the Code:X `mi17_b8_rus` (id `0xb40f`, MID 9814 unchanged; NATO pilot breeds `mp/nato/2022s/nato_pilot` on `driver`/`commander` and the 82nd team unchanged). That is the one airframe with a live parked-actor instantiation proof, and it makes the NATO package structurally identical to the rusa/ukr ones — the discriminator the next live run needs. The Blackhawk is out of every live pool (test-pinned as comment-only; the deploy throws if an `{Entity "uh-60m_blackhawk_mg"` reappears). **New stable fail code 13 = "parked airframe absent at dispatch"**: a pre-claim probe in every E2 faction dispatch trigger (helo ×4, para ×3) counts the intersection of the package's own pool tag with `support_e2_aircraft` and sets 13 + `e2_fail_and_cleanup` when it is empty, with the whole rest of the leg fenced behind `support_e2_fail$ == 0`. 13 = the entity was never there; 9 = it was there and the claim selector did not take it.

**Correction to a working hypothesis — do NOT re-derive it.** It is **not** true that "NATO = West-81 airframes, everyone else = Code:X". **Both paradrop planes are West-81**: `il-76td_para` is `-vehicle/sov/airborne/il-76td/il-76td_para.def` and `c130_para` is `-vehicle/csa/airborne/c130/c130_para.def`, both members of West-81's `-vehicle.pak`. RUSA, UKR and NATO all fly a West-81 plane on the para leg, so **any para-leg defect is universal, never NATO-specific**. The West-81-vs-Code:X asymmetry existed only on the helo leg, and after the swap above it no longer exists at all.

**Para-leg defect (closed).** `properties/Airborne_M.ext` declares **two** chassis for both planes — `{Chassis "wheel" … {MaxSpeed 30}}` and `{Chassis "airborne" … {Maxspeed 95} {turnRadius 40} {StartTime 4} {BrakeTime 6} {StopTime 10}}` — plus `{chassisManager {current "wheel"}}`. The three para hulls (`0xb416`, `0xb420`, `0xb428`) were parked with only Position/Player/MID/Able and **no chassis snapshot at all**, so they came up on the **ground on the wheel chassis** at the off-map park position: that is why a live run reached stage 30 (claim + `{state operatable}` both proved), ran ~92 s, and the player saw **no aircraft anywhere in the sky and no minimap contact**. All three now carry `{Chassis "airborne" {Airborne}{EngineStarted}{Altitude 65}}`, matching the `{"air_state"} {altitude 65}` the para dispatch issues.

**Release-band defects (closed).** Map coordinates are decimetres; the airborne chassis at 95 km/h covers ~264 units/s.
1. The band was term 5 `{distance 2500}` AND `!` term 6 `{distance 1500}` — a 1000-unit shell only 150–250 m from the flag, under 4 s of dwell, and never entered at all by a run-in whose closest approach was 2600. It is now **600–4000**, a 3400-unit annulus (~13 s of dwell) that an off-axis run-in cannot skirt.
2. Added a **bounded closest-approach fallback** (term 8, `support_e2_para_pass$`): `e2_para_range_poll` records the tightest coarse band reached in `support_e2_para_range_band$` (1 = inside 1000 … 4 = inside 4000, 0 = never inside the outer ring, SetVar integers only, literal case folds, no var-to-var copy). A poll reporting a looser band than the recorded best while still inside the outer ring — or reaching band 1 at all — grants the release. The `attack_support/e2_para_range` trigger drives it and re-arms itself only while stage 30 is open. **Fail 6 stays meaningful**: a plane that never gets inside 4000 satisfies neither term 5 nor term 8, never releases, and is still reported as "never approached".
3. Stage 30 was reachable and then unproved: nothing after the one-shot `e2_fly_para_or_fail` ever re-checked that the aircraft still existed. `attack_support/e2_para_alive` is now armed for the whole of stage 30 and fires on the *absence* of the proof — **fail 9** if no claimed aircraft exists at all, **fail 10** if the hull is present but not operatable — so stage 30 is unsustainable without a live aircraft.
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
| 6 Flag ammo supply | Low | Yes — any mission; walk a rifleman onto a held flag |

## Approval record (2026-07-30, from user)
1. ✅ Phases 1–4 approved in order, live-test gates respected.
1b. ✅ Phase 5 = E1 airmobile insert, all four factions, PRC doctrine comment revised (Army Aviation transport allowed). E2 parked as a post-ship time-boxed experiment; F parked indefinitely, E2-conditional.
2. ✅ Phase 1 bonus approved: fix the ~25 orphaned CE `mission/multi/*` localization keys (separate commit).
3. ✅ Phase 3: ALL FOUR factions in one series (no single-faction pilot).
4. ✅ Phase 4: defender-side ALWAYS, both mission types (see revised v1 scope above).
5. ✅ Phase 6 (2026-07-30): link a `flagpoint_ammo` into every CTF flag's built-in "ammo" placer slot using the vanilla mechanism, with a shadow `.def` swapping the ammo table to Code:X's modern one. The same decision retires the crate half of Phase 4 as superseded; the L2+ crewless weapon placement stays.

Executor: Grok (external session), under the marching orders at the top of this document. Post-execution audit by the Claude toolchain against this document is expected — deviations from the marching orders will be treated as defects even if the feature works.

## Live test 2026-07-30 (evening) — first successful aircraft manifestation, and three defects

**THE GOOD NEWS, recorded first because it must not be "fixed" away: the E2 clone + teleport rebuild PHYSICALLY WORKS.** The user watched an Mi-17 fly across the map and hover over the target flag; screenshots confirm it both in the sky and on the minimap. This is the first time any aircraft in this system has manifested at all. The mechanism that achieved it is load-bearing and stays exactly as it is: `e2_clone_aircraft` (`{"actor_to_waypoint"}` + `{source advanced}` + `{include {tag {tag hidden}}}` + `{amount 1}` + a NUMERIC waypoint + `{clone}` + `{approach "safe teleport & rotate"}`), the numeric waypoint band 9101-9104, the destination waypoint's `{commands}` re-tag block, and the parked chassis snapshots. Nothing below touches any of them.

**Cosmetic oddity to address in a later pass (no code change now): NATO's `mi17_b8_rus` flies with RED-STAR LIVERY.** It is a direct consequence of the airframe swap recorded above — NATO's E2 helo hull is the Code:X `mi17_b8_rus`, the one airframe with a live parked-actor instantiation proof, and it carries its own faction skin. Correct behaviour, wrong paint. A future cosmetic pass should look for a NATO-liveried variant of the same hull, or a skin override, and must not undo the swap: the swap is what made the NATO leg structurally identical to the RUSA/UKR ones.

### Defect 1 — a false fail 10 killed the helicopter leg at dispatch (FIXED)

**Evidence.** Mission starts 03:28. At 04:07 — the first `e2` mirror after `faction_support_army` published 3 — the state is already `e2_test 3 e2_stage 20 e2_fail 10 e2_lz 1 e2_flag 1`. The helicopter was demonstrably alive: it flew to the flag and **hovered there indefinitely — no unload, no departure, no delete**. The stage machine walked 20 -> 60 -> 70 with fail 10 already set.

**Root cause — SECOND LIVE PROOF of the selector-decoration class.** `e2_fly_helo_or_fail` gated the flight leg on `{selector ... {tag support_e2_claim} {state operatable}}` and that selector **matched nothing**, one second after the clone arrived, on an aircraft the player could see. A `{state operatable}` decoration on a selector zeroes the match on these units. This repo had already been burned by the ADVANCED spelling of the same idiom — `tools\deploy_attack_support_probe.ps1` has banned the doubly-nested state form, `{include {prop human}}` and `{prop {prop human}}` on pool selectors since the first proof, with the note *"a bare select moved all four; the same select plus a prop/state decoration matched nothing in the very next action"*. The SIMPLE spelling was never banned, and the E2 lifecycle used it in 18 places.

**Why it was fatal rather than merely noisy.** With the flight leg refused, `e2_order_aircraft_lz` never ran — so the aircraft kept the only order it had ever been given, the `{action move}` toward `support_e2_flag_target` written by the numeric waypoint's own `{commands}` block. That is precisely why it flew to the flag and stopped there. The whole unload -> takeoff -> off-map -> delete chain hung off a fixed 40-second delay *inside the same action list*, so when the leg abandoned itself the chain went with it and the physical clone was orphaned in the air.

**Fixes.**
1. Every E2 liveness/existence gate is a **bare-tag count** now. Where corpses genuinely must be excluded the gate uses the advanced `{group {select ...} {exclude {state {state dead}} {state {state inactive}}}}` form (`e2_finish_team_or_fail`) or the proven `{state "not dead"}` spelling on `{type near}` clauses (para run-in, release band, survivor proof). 18 of the 22 occurrences are gone; the 4 that remain are the enemy-proximity LZ guard and address live bot humans, never our own entities.
2. The unload chain is **armed off the aircraft's arrival**. `attack_support/e2_helo_lz` is a top-level trigger whose CONDITION is a near check on the arrived, re-tagged hull against the LZ marker; it owns stages 40 and 60. `attack_support/e2_helo_timeout` closes the window and owns fail 5. Both always issue a departure order and a delete, and so does the dispatch-time refusal branch — **a leg that fails its evidence gate still departs.**
3. `e2_delete_aircraft` and `e2_order_aircraft_exit` each carry a **provenance-keyed backstop** alongside the existing claim-keyed one: `support_e2_arrival` is written ONLY by the numeric waypoint's `{commands}` block, so it exists on every dispatched clone and on nothing else in the mission — no parked template can carry it, so the backstop cannot eat the pool. `attack_support/e2_orphan_sweep` is a standing, self-re-arming last resort.
4. Every `support_e2_stage$` write is evidence-gated: 20 now requires the target entity itself rather than "nothing has failed yet"; 40 requires either a claimed team to deliver or the aircraft actually being at the LZ; 30 and 50 only exist inside their gates; 60/70 are terminal and always clean up. The full ledger is test-pinned.
5. The deploy guard is **narrowed, not loosened**: `{state operatable}` is banned on any selector that also addresses a `support_e2_*` entity, in all four wave engines, on the source AND the shipped copy, with this run recorded as the second proof. The remaining legal scope is pinned at exactly two uses. The fixed 40-second arrival window is a forbidden marker.

### Defect 2 — truck passengers appear with no drive phase (INSTRUMENTED; sequencing cleared, one concrete race fixed)

**Evidence.** `attack_support_motor_stage` jumps 0 -> 4 between the 06:18 and 06:23 mirrors (~5 s), and `enemy_defense` does the same. The user saw troops materialize at the truck with no drive phase, and confirmed this run's trucks behaved "like last run".

**What the diff actually says.** `git diff 27449ea..HEAD -- resource/map/multi/attack_support_waves.inc` over the whole motorized path: **`as_finish_motor`'s drive -> 28s -> emit sequence is byte-for-byte the sequence `27449ea` shipped.** The only differences inside that define are the added `motor_stage` writes and the departure-instead-of-idle ending. The emit block, its `{state inhabited}` selector and the 28-second clock are all unchanged — and the user *does* see passengers come out, so `{state inhabited}` demonstrably still matches and is NOT an instance of the defect-1 class. **The regression is not in the emit sequencing.**

**The one concrete behavioural change since `27449ea` that touches this path** is `attack_support_motor_left$` **1 -> 4** plus the self-re-arming `motor_clock` (commit `27d643b`, "Run motorized inserts on both sides of every mission"). Both the infantry wave path and the motorized path run off ONE shared deploy tag, and both used to clear it from every body carrying it when they finished. With one truck a mission that was survivable; with four the collision window is hit often — and a hull that loses the deploy tag before ownership and `{control AI}` / `{ai_move {mode enable}}` are set is left neutral, driverless and motionless. From outside that is indistinguishable from "the passengers appeared with no drive phase". Fixed in all four engines: the deploy tag is re-asserted from the package's own hull/pax/crew marks immediately before ownership, and released per package rather than globally (a new `*_motor_crew` mark makes the crew addressable without the shared tag).

**And the hypothesis is now falsifiable from the log alone,** which it was not before: `motor_stage` 3 -> 4 looks identical whether the standoff ran or not. Two new integers per engine, declared in `dcg_vars.inc` and mirrored by `mirrorEngineState()`:
- `*_motor_drive_t$` — 0 -> 4, one step per 7 seconds of the standoff that actually elapsed. **An emit at `drive_t < 4` proves the delay did not run.** The total standoff is unchanged at 28 s; it is four instrumented 7 s steps instead of one opaque block, and the deploy refuses a build that collapses them back.
- `*_motor_band$` — the hull's distance to its objective at the instant before the emit, as a bounded integer band (1 inside 60, 2 inside 150, 3 inside 400, 0 further out). **`drive_t 4` with `band 0` proves the hull never moved.**

Between them the next run separates "the delay did not run" from "the truck never drove" without another guess.

### Defect 3 — the para leg looked like it inherited the helo leg's fail code (FIXED; diagnosis corrected)

**Evidence.** After the combo transition (`e2_test 2` by 06:23, `combo_helo_fail 10` correctly recorded) `e2_fail` reads **10** for the entire para leg, which reached stage 70 with `e2_para_band 0`, `e2_para_pass 0` and no plane ever seen.

**Correction to the working diagnosis — the para leg did NOT inherit anything.** `attack_support/e2_combo_transition` clears `support_e2_fail$`, `attack_support/e2_dispatch` clears it again before routing to any child, and the log shows the transition fired correctly. The para leg then produced its **own, independent** fail 10 from `e2_fly_para_or_fail` — the same gate, carrying the same simple-selector `{state operatable}` decoration, as the helo leg. Defect 3 is a second instance of defect 1, not a state leak. That is also why `e2_para_band` and `e2_para_pass` stayed 0: stage 30 was never reached, so the run-in tracker was never armed.

**Fixes.** The bare-tag gate from defect 1 removes the cause. Belt and braces on top: `e2_trigger_para_by_army` asserts `support_e2_fail$ = 0` at the last point before any faction child can run, so the para leg's diagnostics are readable whatever happened upstream. And because the transition legitimately requires that no claimed entity remain (`!3`) — a requirement an orphaned aircraft could block forever — `attack_support/e2_combo_clear` **retires the leftover rather than relaxing the pin**: it is armed on the exact complement of the transition's condition, orders the leftover off the map, deletes it, strips the leg's bookkeeping tags and re-arms the transition. Defect 1(b) should stop the orphan arising at all; this is what happens if it ever does.
