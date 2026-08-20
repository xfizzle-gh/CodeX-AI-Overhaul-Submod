# POW targeting: source-backed Old Boy Player 0

Production surrender present now uses the proven Old Boy captive 5-step on the live surrendering human. Isolation `{behaviour civilian}` result is unchanged and stays parked. Do not ask the owner to run more Editor isolation tests.

## Real (in-repo or native-tested)

| What | Evidence |
| --- | --- |
| `{behaviour civilian}` makes a live hostile human untargetable and projectile-safe | Isolation trio PASS. Armed civilian also ignored. Parked; not the production targeting representation. |
| 1:1 combat-breed → civilian-behaviour unarmed file transform | `tools/generate_pow_mirrors.py`. No runtime behaviour/targetClass/breed setter exists. Parked. |
| `{"placement"}` MOVE without `{clone}` | `ce_functions.inc`, `map_setup/base_map_setup_triggers.inc` |
| `{target}` on a tagged entity | Same AIO placement blocks (FE-shaped; copies position) |
| `{"1.near"}` `{units}` `{near_to}` `{distance}` | `ce_player_triggers.inc`, `ce_ai_logic_triggers.inc` |
| Mission-script `{"delete"}` on a human selector | `ce_map_setup_triggers.inc` extra-defender cull |
| Vanilla `{on "delete"}` / `{call "delete"}` | `human.inc` only. Not used by this prototype. |
| `{"effect" aio_morale_surrender_apply}` | Existing `{on "aio_morale_surrender_apply"}` in `human_ce.inc` — tags / icons / expire only |
| `{entity_state}` tags / `{inactive off}` | Existing AIO |
| Old Boy captive 5-step on a live combat human | Owner isolation **Run B PASS**. Production present path. |
| Runtime `{"player"} {operation set} {player "0"}` alone | Owner isolation **Run A PASS**. Shot/killed SAFE. |
| Skin / body / portrait / armors / visual inventory | Copied in the generated breed file, not at runtime |

## Invented or abandoned (do not use)

| What | Why |
| --- | --- |
| `{on "aio_pow_retire" {delete}}` on `human_ce.inc` | Custom `{on "..."}` with bare `{delete}` is unproven. `human_ce.inc` is globally included from `entity.set`. |
| Hide + `{inactive on}` as “gone” | Leaves a duplicate actor. |
| Tag-live handshake then delete | Insufficient. Retire requires the 5 m near pose-live condition, then vanilla `{"delete"}`. |
| `{control AI}` on a POW | Not part of Old Boy's captive path. Body-recovery only. Do not add. |
| `{able "select" 0}` / `{able "fight" 0}` on the P0 present path | Old CE extras. Production `aio_morale_surrender_apply` no longer sets them. Diagnostic `{on "aio_pow_ob_fight_off"}` stays inert. |
| `{fire_mode hold}` / `{move_mode hold}` / `{weapon_prepare off}` / `{ai_move disable}` / `{drop "orders sensor senseless"}` as a surrender actor_state pile | Crash-stack extras from old #128 heads `483387c` / `707f425`. Do not restack onto P0. |
| `{able "neutral"}` | Already failed native targeting. |
| Runtime `{behaviour}` / targetClass / breed setter | No supported setter in audited script surfaces. |
| Living-actor breed query, IE spawn-at-human | Not found. |
| Runtime copy of health / facing / player / squad | Not found. Template is authored `{Player 2}`. |
| `{target}` heading / exact pose | Editor-unproven. Position MOVE is the real transfer. |
| Generic faction POW proxies | Architecture is 1:1 combat-breed mirrors only. |
| Isolation `clear_inventory` delay | Drop fixture only. Not the POW disarm path. |
| Recruiting / trucks / release / interrogation | Out of scope. |
| Authored `{Player 0}` | Old Boy uses runtime `{"player"} {operation set}` only. |
| Civilian-mirror replacement as production targeting | Parked. Do not wire. |

## Live tree rule

Generator may map all eligible combat breeds. Checked-in `resource/set/breed/generated_pow/**` is one parked prototype:

`generated_pow/mp/nato/2022s/nato_rifleman.set`

Do not leave a `{Human "generated_pow/..." ...}` include after deleting that breed. Do not mass-generate.

## Production lifecycle (source-backed)

At surrender commit, `aio_morale_surrender_apply` only adds `aio_morale_surrendering` (and clears broken/regrouping) plus icon refresh / 100s expire. It does **not** set `fight 0` / `select 0`.

At surrender present, apply **only** the Old Boy 5-step to the live surrendering human:

1. `{effect start_white_flag}`
2. `{"player"} {operation set} {player "0"}`
3. `{tag_remove enemy}`
4. `{"action"} {action drop} {volume in_hands}`
5. `{collage stand_giveup_1}`

Routing tags `aio_morale_surrendering` / `aio_morale_surrender_presenting` / `aio_morale_surrender_fx` stay so cleanup and tag-driven evac selectors still match. Those tags must not pull in the old hold/fight/control-AI actor_state block.

Drop uses the Old Boy action primitive `{volume in_hands}`, not the CE inventory `{with_item {type using} {item "weapon"}}` path. Those are different commands; do not treat them as equivalent.

## Owner isolation evidence (treat as facts)

- **Run A PASS:** live P1 combat human → runtime `{"player"} {operation set} {player "0"}` only → shot/killed SAFE.
- **Run B PASS:** `start_white_flag` → runtime player 0 → `tag_remove enemy` → drop `{volume in_hands}` → `stand_giveup_1` → shot/killed SAFE. No `{control AI}`. No authored `{Player 0}`.
- **Run C/D/E PASS** as single extras on Run B (`{able "fight" 0}`, `{weapon_prepare off}`, `{fire_mode hold}`). Isolation series STOPPED here by owner. Remaining extras were **not** individually proven and must **not** be stacked back onto P0.
- Old crashing #128 heads (`483387c` / `707f425`) put P0 **on top of** the existing CE surrender machine (fight 0 + select 0 + hold-fire + weapon_prepare + drop sensor + evac + sometimes control AI). That combo AVed in `scene.quant.bullets`. Do not recreate it.

## Evacuation

Existing tag-driven evac (`aio_morale_surrender_fx` → `aio_morale_surrender_evacuating` → `{action move}` to captor entry, then `{"delete"}` at egress) still runs on the P0 POW. The evac actor_state pile (hold-fire / weapon_prepare / drop-sensor / move-mode / ai_move) is stripped so those extras do not stack onto P0.

Hunch, not a fact: `{action move}` can stay tag-driven without hold-fire / control AI. If a later native pass shows evac cannot walk without those extras, park evac and document that rather than re-adding the crash stack. Current evac/move/delete on a P0 POW is a later risk; do not expand it.

## Parked (do not wire)

- Civilian-mirror Editor prototype: `ce_pow_replace_editor.inc` / `ce_pow_replace_editor_templates.inc` / `generated_pow/**`
- Editor-only Old Boy diagnostic: `ce_pow_oldboy_captive_editor.inc` (not in `ce_triggers.inc` / `dcg_script.inc`)
- `{on "aio_pow_ob_fight_off"}` in `human_ce.inc` — inert unless that diagnostic calls it. No `{delete}`.

Keep civilian-behaviour isolation fixtures. No CWA production map edits. No Conquest fixtures. No mass `generated_pow`.
