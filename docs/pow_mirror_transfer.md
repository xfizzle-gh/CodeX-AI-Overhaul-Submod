# POW targeting: source-backed Old Boy Player 0

Production surrender present uses the proven Old Boy captive 5-step on the live surrendering human. The civilian-mirror replacement architecture was pruned from this PR so it is not a second targeting implementation. Isolation `{behaviour civilian}` fixtures remain as evidence only.

## Real (in-repo or native-tested)

| What | Evidence |
| --- | --- |
| `{behaviour civilian}` makes a live hostile human untargetable and projectile-safe | Isolation trio PASS. Armed civilian also ignored. Evidence fixtures only; not production. |
| Old Boy captive 5-step on a live combat human | Rifle A–E PASS. Isolation **F PASS** (owner HE). Isolation **G**: AI ignores five-step P0. |
| Runtime `{"player"} {operation set} {player "0"}` alone | Rifle **Run A PASS**. Isolation **F PASS**. Isolation **G**: runtime P0 alone is native AI ignore. |
| `{"effect" aio_morale_surrender_apply}` tags / icons / expire | `{on "aio_morale_surrender_apply"}` in `human_ce.inc` |
| `{entity_state}` routing tags | Existing AIO surrender / cleanup selectors |

## Invented or abandoned (do not use)

| What | Why |
| --- | --- |
| Civilian-mirror replacement / `generated_pow` | Competing targeting architecture. Pruned. Isolation fixtures remain as evidence. |
| `{control AI}` on a POW | Not part of Old Boy's captive path. Body-recovery only. |
| `{able "select" 0}` / `{able "fight" 0}` on the P0 present path | Old CE extras. Production apply no longer sets them. |
| `{fire_mode hold}` / `{move_mode hold}` / `{weapon_prepare off}` / `{ai_move disable}` / `{drop "orders sensor senseless"}` on the P0 actor | Crash-stack extras from old #128 heads `483387c` / `707f425`. Evac `{action move}` no longer uses drop-sensor. |
| `{able "neutral"}` | Already failed native targeting. |
| Authored `{Player 0}` | Old Boy uses runtime `{"player"} {operation set}` only. |
| `{on "aio_pow_ob_fight_off"}` / Editor captive diagnostic | Removed after A–E. Not needed for production. |
| `{on "aio_pow_retire" {delete}}` on `human_ce.inc` | Unproven custom delete. |
| `aio_iso_hostile_civ_rifle_drop` | Obsolete isolation drop fixture. Removed. |

## Production lifecycle

At surrender commit, `aio_morale_surrender_apply` only adds `aio_morale_surrendering` (and clears broken/regrouping) plus icon refresh / 100s expire. It does **not** set `fight 0` / `select 0`.

At surrender present, apply the Old Boy 5-step plus verified `{impregnability harmless}` after P0:

1. `{effect start_white_flag}`
2. `{"player"} {operation set} {player "0"}`
3. `{"entity_state"} {impregnability harmless}` — keep for POW life. Not `{impregnability full}`.
4. `{tag_remove enemy}`
5. `{"action"} {action drop} {volume in_hands}`
6. `{collage stand_giveup_1}`

The five Old Boy operations are intact. Harmless sits immediately after P0. Routing tags `aio_morale_surrendering` / `aio_morale_surrender_presenting` / `aio_morale_surrender_fx` stay so cleanup and tag-driven evac selectors still match.

### Verified impregnability syntax

Vanilla / local map entity property (engine token `harmless`), from `resource/map/multi/dcg_zeeland_sum_TEST/map`:

```
{Impregnability harmless}
```

Local `{"entity_state"}` command slot (same family, lowercase key) from `resource/map/multi/ce/ce_mechanics_triggers.inc`:

```
{"entity_state"
    {selector ...}
    {impregnability full}
}
```

and `ce_player_triggers.inc` / support waves also use `{impregnability disabled}`. This repo has **no** `{impregnability harmless}` on `entity_state` until this overlay. The applied form is the proven command slot + the proven vanilla mode token:

```
{"entity_state" ... {impregnability harmless}}
```

`{impregnability full}` (used on `1a5e363`, then reset) is the wrong mode.

Civilian-mirror remains the fallback **only if** this Conquest overlay still 0x158. Do not implement the mirror now.

## Owner isolation evidence

- **Run A PASS:** live P1 combat human → runtime Player 0 only → shot/killed SAFE.
- **Run B PASS:** `start_white_flag` → runtime player 0 → `tag_remove enemy` → drop `{volume in_hands}` → `stand_giveup_1` → shot/killed SAFE.
- **Run C PASS:** Run B + `fight 0`, no AV.
- **Run D PASS:** Run B + `weapon_prepare off`, no AV.
- **Run E PASS:** Run B + `fire_mode hold`, no AV.

- **Isolation F PASS:** owner-controlled T-62 HE killed CONTROL, runtime-P0-only, and exact five-step; no AV.
- **Isolation G:** no AV. AI T-62 killed CONTROL and would not target runtime-P0-only or five-step P0. Owner then killed both P0 subjects with no AV. ProcDump was SetThreadName/startup, not a GoH AV. G does **not** test AI-owned damage into P0.

Do not resume direct-target tests. Do not add `{able "select" 0}`. Remaining extras stay off P0.

Old crashing heads (`483387c` / `707f425`) stacked P0 on top of fight/select/hold-fire/weapon_prepare/drop-sensor/evac/control AI and AVed in `scene.quant.bullets`.

## Native Conquest targeting evidence

The ~18:21 Conquest red-dot/shot-while-moving run was on installed `d7fa808`, **not** `b95f3cdc`. That attribution is withdrawn.

## Native damage evidence

**Isolation F PASS:** owner-controlled T-62 HE hit/killed CONTROL, runtime-P0-only, and exact five-step P0 with no AV. Conquest owner/player damage to P0 POWs was also safe, including a vehicle hit.

**Isolation G:** AI-owned T-62 killed CONTROL and would **not** target runtime-P0-only or the exact five-step P0 (native ignore). Owner then manually killed both P0 subjects, no AV. ProcDump was SetThreadName/startup, not a GoH AV. G does **not** test AI-owned damage into P0. It does show runtime Player 0 alone is enough for native AI ignore, and the five-step retains that ignore.

Two production AVs followed **AI-owned** damage to neutral/P0 POWs: T-62/HE via `scene.quant.dmg`, AI M4/rifle via `scene.quant.bullets`. Both `EXCEPTION_ACCESS_VIOLATION` read `0x00000158`, same RIP class. Nearby `deleted` log lines are correlation only unless the crash victim ID enters delete/expire/egress on the same tick.

Do not infer targeting from those tank events. Target-ignore and damage-safety stay separate gates. Do not add `select 0`. Do not return to Conquest permutations for this isolate. Do not infer an expire/delete race from nearby unrelated `deleted` log lines unless the exact crash-victim entity ID is shown entering that transition on the same tick.

Isolation G shows runtime Player 0 alone is sufficient for native AI ignore, and the five-step keeps that ignore. Direct targeting will not deliver AI-owned damage into P0. The five-step remains **HOLD as production-safe** until AI-owned *collateral* damage is isolated. Production present is unchanged.

F/G/H Editor files stay unused. Isolation H is **cancelled** as the next owner test. Do not ask Paul to run Editor. Next owner test is **one normal Conquest reproduction** (AIO arms ProcDump).

## Conquest CE_POW_DIAG instrumentation

Diagnostic-only. Production P0 / five-step / evac / expire / delete semantics are unchanged except added log tags and `{"set_i"}` counters. No `{able "select" 0}`, `{able "fight" 0}`, hold-fire, `{control AI}`, civilian-mirror, or issue #133 / `preparationTime 480`.

There is no mission `{"log"}` command in this repo. The source-proven `game.log` style is Lua `print()` (`CE_POW`, `CE_MORALE_EVENT`, `CE_POW_DIAG`) in `resource/script/multiplayer/modes/utility_ce.lua`.

`startPowDiagWatch()` is always-on from `StartCeMoraleProbeLog()` (not gated on debug/autodemo). It polls declared mission vars every 1s. Do **not** use `IsSquadTagged` for this trail — entity-level tags are invisible to that API (`CE_POW alive=1` never printed even when `observe_surrender` set `ce_morale_diag_surrender$`).

Declared vars (same grammar as `ce_morale_diag_surrender` in `ce_vars.inc`): `aio_pow_next_id`, `aio_pow_seq`, `aio_pow_last_evt`, `ce_morale_diag_present`, `ce_morale_diag_assign`, `ce_morale_diag_p0`, `ce_morale_diag_impregnable`, `ce_morale_diag_drop`.

Unconditional `{"set_i"}` (no entity selector) is the first action of `surrender_present` (`ce_morale_diag_present$=1`) and of sibling `surrender_diag_assign` (`ce_morale_diag_assign$=1`). `ce_morale_diag_p0$=1` is immediately after `{player "0"}`. Overlay `aio_pow_last_evt$` / `aio_pow_seq$` sit **after** drop, not between P0 and drop (undeclared `set_i` there is the hunch for the missed drop on `788397d`). `ce_morale_diag_drop$=1` is immediately after `{action drop}{volume in_hands}`.

Lua prints `CE_POW_DIAG event=present|assign|p0|impregnable|drop` when those declared vars flip (same flip pattern as `CE_MORALE_EVENT surrender`). `ce_morale_diag_impregnable$=1` is immediately after the post-P0 `{impregnability harmless}` `entity_state`.

### Stable POW diagnostic ID

On apply, the actor gets `aio_pow_need_id`. Trigger `broken/surrender_diag_assign` (sibling of present, not nested inside it) stamps `aio_pow_d01` … `aio_pow_d16` plus `aio_pow_did` using the same-file evac `{"switch"}` `{condition {type cmp_i} {var ...} {op "=="} {value N}}` grammar on declared `aio_pow_next_id$`. Do not use `{type entities}` + `{count {op "=="}}` in that switch — that nest failed map load (`dda_accessorraw.cpp:342`). Overflow (17th+ surrender in the match) is tagged `aio_pow_overflow`.

### Line format

```
CE_POW_DIAG id=aio_pow_d01 entity=unreadable breed=unreadable orig_player=unreadable curr_player=0_inferred squad=<squad or unreadable> event=<name> t=<os.time or unreadable> clock=<os.clock or unreadable> flags=<tag list or gone> sensor=unreadable last_evt=<int> seq=<int>
```

Unreadables are explicit. Script cannot read entity hex, breed, original Player slot, or sensor/detect registration. `curr_player=0_inferred` only on `event=p0` after the declared `ce_morale_diag_p0$` flip. Do not use `IsSquadTagged` for this trail.

`event=` values: `watch_armed`, `apply`, `p0`, `present_complete`, `evac_start`, `move_a`, `move_b`, `arrive`, `expire`, `delete`, `die`, `hit`, `state_change`, `seq`, `absent`, `overflow`.

`last_evt` ints: 1 apply, 2 p0, 3 present_complete, 4 evac, 5 move_a, 6 move_b, 7 arrive, 8 expire, 9 delete.

### Hooked production paths (log tags only)

| Event | Hook |
| --- | --- |
| surrender apply | `human_ce.inc` `aio_morale_surrender_apply` tags `aio_pow_evt_apply` / `aio_pow_pre_p0` / `aio_pow_need_id` |
| P0 transfer | after present `{"player"} {player "0"}`: `aio_pow_post_p0` / `aio_pow_evt_p0` |
| present complete | after five-step + `aio_morale_surrender_fx`: `aio_pow_evt_present_done` |
| evac start | after `aio_morale_surrender_evacuating`: `aio_pow_evt_evac` |
| move stages | after `to_a` / `to_b`: `aio_pow_evt_move_a` / `_b` (engine mid-path waypoints are not script-readable) |
| egress / arrive | before arrive `{"delete"}`: `aio_pow_evt_arrive` then `aio_pow_evt_delete` |
| expire | apply 100s tag + before expire `{"delete"}`: `aio_pow_evt_expire` then `aio_pow_evt_delete` |
| delete | `{"set_i"} aio_pow_last_evt$=9` immediately before production `{"delete"}` (no extra delay) |
| death | existing `{on "die"}` adds `aio_pow_evt_die` if `aio_pow_did` (no `{delete}`) |
| damage | existing `{on bullet_hit}` adds `aio_pow_evt_hit` once if `aio_pow_did` |

No script-readable sensor / detect / player-registration cleanup command exists. `sensor=unreadable` on every line. Do not add `{drop "orders sensor senseless"}`.

### Grep a victim hex ID after AV

`CE_POW_DIAG` cannot print entity hex. After an AV, take the engine line:

`eActorSensorDetect.cpp:81 No detect time found for unit human[0xHHH:...]`

1. Note the hex (`0xHHH`) and nearby `game.log` timestamp.
2. `grep -n "CE_POW_DIAG" game.log` (or the session log) and keep the trail for every `id=aio_pow_dNN` still alive or just gone at that timestamp.
3. `grep -n "0xHHH" game.log` for `deleted` / detect / damage lines on the same tick.
4. A nearby `deleted` line is correlation only unless that same hex also has `CE_POW_DIAG event=delete` or `event=expire` on the same tick.
5. `grep CE_POW_DIAG game.log | grep event=delete` / `event=die` / `event=hit` / `event=absent` to see whether the POW trail ended in egress delete, expire delete, death, or vanished without a stamped event.

## Old Boy vs Conquest post-P0 cleanup delta (audit only)

Do **not** silently add Old Boy extras onto production P0. This is a report, not a license to restack drop-sensor / leave-squad / fight / select.

After runtime P0, the in-repo Old Boy captive 5-step (Isolation B / production present) is only:

1. `{effect start_white_flag}`
2. `{"player"} {operation set} {player "0"}`
3. `{tag_remove enemy}`
4. `{"action"} {action drop} {volume in_hands}`
5. `{collage stand_giveup_1}`

Not on that path: `{control AI}` (body-recovery only), authored `{Player 0}`, `{able "fight" 0}` / `{able "select" 0}`, hold-fire, `weapon_prepare`, `{drop "orders sensor senseless"}`, leave-squad, expire/evac/delete, or any script-visible detect-registration cleanup.

Conquest extras **after** the same 5-step:

- `{"entity_state"} {impregnability harmless}` immediately after P0, kept for POW life
- CE routing tags (`aio_morale_surrendering` / `_presenting` / `_fx` / `_evacuating` / `_to_a` / `_to_b` / `_at_egress` / `_expire`)
- 100s expire → mission `{"delete"}`
- tag-driven evac `{action move}` to captor entry
- egress mission `{"delete"}`
- `{on "die"}` strips CE morale tags (already production)
- **no** drop-sensor on P0 evac moves (stripped earlier)
- **no** squad leave
- **no** detect-registration cleanup

Old Boy source is not vendored (`docs/morale_command_phase0_audit.md`, Workshop `3604287428`). If the native captive path does extra engine-side squad / sensor / player-registration cleanup after P0, it is not exposed as a script command here. The production crash `eActorSensorDetect.cpp:81 No detect time found for unit human[0xHHH:...]` is consistent with leftover detect state after player/squad change, but script cannot read or clear that state. Do not restack `{drop "orders sensor senseless"}` onto production P0 from this delta.

## Isolation H splash mission (cancelled / unused)

Editor-only on `dcg_zeeland_sum`:

- `resource/map/multi/dcg_zeeland_sum/aio_p0_runtime_h.mi`
- `resource/map/multi/dcg_zeeland_sum/aio_p0_runtime_h.inc`
- `resource/map/multi/dcg_zeeland_sum/aio_p0_runtime_h.info`

Paul does nothing. One AI-owned T-62 (`aio_p0_h_ai`, Player 2, not `user_control`) fires HE left-to-right at three normal dummy humans. A bystander at each station is 12 units off the dummy (splash only). Stations are 1200 units apart so one blast cannot reach the next.

1. `aio_p0_h_dummy_1` + `aio_p0_h_bystander_1` — ordinary control
2. `aio_p0_h_dummy_2` + `aio_p0_h_bystander_p0` — runtime P0 only (same as G)
3. `aio_p0_h_dummy_3` + `aio_p0_h_bystander_ob` — exact Old Boy five-step from F/G

Attack grammar matches G: `{action attack}` from the tank (operatable, exclude user_control/dead/inactive) at the **dummy** tags only. Never attack a P0 bystander. No authored `{Player 0}`. No `{control AI}`. No CE surrender tags, expire, evac, delete, select/fight.

- splash kills P0-ONLY and crashes ⇒ abandon Player 0
- P0-ONLY safe, FIVE-STEP crashes ⇒ five-step interaction
- both P0 splash cases safe ⇒ AI ownership/collateral alone is not sufficient; only then one controlled production-state fixture (CE tags / expire / evac / delete timing), not Conquest rematches

Unused tag fixture: `resource/map/multi/ce/ce_pow_dmg_editor.inc`.

## Evacuation

`3707e1c` overlay Conquest: pose-then-delete **worked** (present/p0/drop 00:04:28–29, delete 00:04:32). Later 0x158 at 00:10:38 was a **different** crash-tick EVT (“AI: Veteran AKM vs neutral 90 Rifleman”), 366s later, no second `CE_POW_DIAG`. Walk/egress hypothesis is **FAIL**. This is not delete-on-stale-detect of the despawned POW.

Real Conquest still AVs when AI bullets hit a live neutral/P0. This overlay restores the real POW lifecycle (3s pose → `{action move}` to `attack_support_entry_a` / `_b`) and applies `{impregnability harmless}` after P0 so crossfire should be a no-op. 100s expire stays as a backstop. No drop-sensor.

Later bounded end-to-end native check: surrender → hostile AI ignores → `CE_POW_DIAG event=impregnable` → POW walks to the correct captor entry → disappears at egress → AI bullets into the live P0 do not 0x158.

## Evidence fixtures kept

Isolation trio only, under `resource/set/breed/isolation_test/`:

- `aio_iso_hostile_soldier`
- `aio_iso_hostile_civ`
- `aio_iso_hostile_civ_rifle`

Not wired into Conquest or CE production. No CWA-map edits.
