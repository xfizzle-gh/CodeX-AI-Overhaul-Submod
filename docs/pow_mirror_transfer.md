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

At surrender present, apply **only** the Old Boy 5-step:

1. `{effect start_white_flag}`
2. `{"player"} {operation set} {player "0"}`
3. `{tag_remove enemy}`
4. `{"action"} {action drop} {volume in_hands}`
5. `{collage stand_giveup_1}`

Routing tags `aio_morale_surrendering` / `aio_morale_surrender_presenting` / `aio_morale_surrender_fx` stay so cleanup and tag-driven evac selectors still match.

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

F/G Editor files stay unused. The current native isolate is Isolation H.

## Isolation H splash mission

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
- both P0 splash cases safe ⇒ stop P0 permutations; next is Conquest-only CE tags/evac/delete lifecycle

Unused tag fixture: `resource/map/multi/ce/ce_pow_dmg_editor.inc`.

## Evacuation

Tag-driven evac still runs: `aio_morale_surrender_fx` → `aio_morale_surrender_evacuating` → `{action move}` to captor entry → mission `{"delete"}` at egress. The evac actor_state pile and `{drop "orders sensor senseless"}` on those move branches are stripped.

`{action move}` / egress delete on this stripped P0 actor is a **hunch, not a fact**. A–E prove presentation projectile/death safety only.

Later bounded end-to-end native check (not another isolation shooting matrix): surrender → hostile AI ignores → POW walks to the correct captor entry → disappears at egress → no AV → no unintended kill/score/ticket → clean `game.log`.

## Evidence fixtures kept

Isolation trio only, under `resource/set/breed/isolation_test/`:

- `aio_iso_hostile_soldier`
- `aio_iso_hostile_civ`
- `aio_iso_hostile_civ_rifle`

Not wired into Conquest or CE production. No CWA-map edits.
