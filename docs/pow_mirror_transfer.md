# POW targeting: source-backed Old Boy Player 0

Production surrender present uses the proven Old Boy captive 5-step on the live surrendering human. The civilian-mirror replacement architecture was pruned from this PR so it is not a second targeting implementation. Isolation `{behaviour civilian}` fixtures remain as evidence only.

## Real (in-repo or native-tested)

| What | Evidence |
| --- | --- |
| `{behaviour civilian}` makes a live hostile human untargetable and projectile-safe | Isolation trio PASS. Armed civilian also ignored. Evidence fixtures only; not production. |
| Old Boy captive 5-step on a live combat human | Owner isolation **Run B PASS**. Production present path. |
| Runtime `{"player"} {operation set} {player "0"}` alone | Owner isolation **Run A PASS**. Shot/killed SAFE. |
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

Isolation series stopped. Do not resume the one-variable shooting matrix. Remaining extras stay off P0.

Old crashing heads (`483387c` / `707f425`) stacked P0 on top of fight/select/hold-fire/weapon_prepare/drop-sensor/evac/control AI and AVed in `scene.quant.bullets`.

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
