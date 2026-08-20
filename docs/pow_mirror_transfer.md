# POW mirror transfer: real vs invented

Editor prototype only. Production surrender still uses the original soldier until this path is accepted. Isolation `{behaviour civilian}` result is unchanged.

## Real (in-repo or native-tested)

| What | Evidence |
| --- | --- |
| `{behaviour civilian}` makes a live hostile human untargetable and projectile-safe | Isolation trio PASS. Armed civilian also ignored. |
| 1:1 combat-breed → civilian-behaviour unarmed file transform | `tools/generate_pow_mirrors.py`. No runtime behaviour/targetClass/breed setter exists. |
| `{"placement"}` MOVE without `{clone}` | `ce_functions.inc`, `map_setup/base_map_setup_triggers.inc` |
| `{target}` on a tagged entity | Same AIO placement blocks (FE-shaped; copies position) |
| `{"1.near"}` `{units}` `{near_to}` `{distance}` | `ce_player_triggers.inc`, `ce_ai_logic_triggers.inc` |
| Mission-script `{"delete"}` on a human selector | `ce_map_setup_triggers.inc` extra-defender cull |
| Vanilla `{on "delete"}` / `{call "delete"}` | `human.inc` only. Not used by this prototype. |
| `{"effect" aio_morale_surrender_apply}` | Existing `{on "aio_morale_surrender_apply"}` in `human_ce.inc` |
| `{entity_state}` tags / `{inactive off}` | Existing AIO |
| `{actor_state}` hold-fire / hold-move / `weapon_prepare off` | Existing surrender presentation |
| `{action move}` `{waypoint}` | Existing POW walk (`aio_pow_walk` here) |
| Skin / body / portrait / armors / visual inventory | Copied in the generated breed file, not at runtime |

## Invented or abandoned (do not use)

| What | Why |
| --- | --- |
| `{on "aio_pow_retire" {delete}}` on `human_ce.inc` | Custom `{on "..."}` with bare `{delete}` is unproven. `human_ce.inc` is globally included from `entity.set`. |
| Hide + `{inactive on}` as “gone” | Leaves a duplicate actor. |
| Tag-live handshake then delete | Insufficient. Retire requires the 5 m near pose-live condition, then vanilla `{"delete"}`. |
| `{control AI}` on a POW | Not part of Old Boy's captive path. Body-recovery only. Do not add. |
| `{able "neutral"}` | Already failed native targeting. |
| Runtime `{behaviour}` / targetClass / breed setter | No supported setter in audited script surfaces. |
| Living-actor breed query, IE spawn-at-human | Not found. |
| Runtime copy of health / facing / player / squad | Not found. Template is authored `{Player 2}`. |
| `{target}` heading / exact pose | Editor-unproven. Position MOVE is the real transfer. |
| Generic faction POW proxies | Architecture is 1:1 combat-breed mirrors only. |
| Isolation `clear_inventory` delay | Drop fixture only. Not the POW disarm path. |
| Recruiting / trucks / release / interrogation | Out of scope. |

## Live tree rule

Generator may map all eligible combat breeds. Checked-in `resource/set/breed/generated_pow/**` is one prototype:

`generated_pow/mp/nato/2022s/nato_rifleman.set`

Do not leave a `{Human "generated_pow/..." ...}` include after deleting that breed.

## Player 0 research correction (2026-08-20)

Old Boy's POW path is **not** `{player "0"}` + `{control AI}`. The captive lifecycle is:

1. combat human starts Player 1 (`pw` captive)
2. `start_white_flag`
3. runtime `{"player"} {operation set} {player "0"}`
4. remove `enemy`
5. drop the weapon in hands
6. apply `stand_giveup_1`

Owner isolation on `d7fa808`: **P1->P0 alone PASS**. Live reassignment is projectile/death-safe. The `311ef20` AV did not reproduce this sequence; it tested our added combination.

Civilian-mirror Editor prototype stays **parked** while this diagnostic runs. Production surrender (`ce_broken_behavior_triggers.inc` / `aio_morale_surrender_apply`) stays off Player 0.

Editor-only file: `ce_pow_oldboy_captive_editor.inc` (not in `ce_triggers.inc` / `dcg_script.inc`).

1. Open Editor. Place one Player-1 `mp/nato/2022s/nato_rifleman`, tag `aio_pow_ob_src`.
2. After 2 s the five-step sequence runs.
3. Deliberately shoot the actor. PASS = no AV + sequence applied. If AV, reduce one state at a time.
4. No Conquest. Do not include the civilian-mirror Editor files for this test.

## Editor civilian-mirror sequence (parked)

1. Open `call_to_arms_ed.exe`. `page_scene_editor` must launch.
2. Include `ce_pow_replace_editor_templates.inc` in the mission entity block and `ce_pow_replace_editor.inc` in the trigger block. Not from `ce_triggers.inc` / `dcg_script.inc`.
3. Place `mp/nato/2022s/nato_rifleman`, tag `aio_pow_replace_src`. Optional waypoint `aio_pow_walk`.
4. After 3 s the source gets `aio_morale_surrender_apply`, then `aio_pow_need_replace`.
5. Parked civilian MOVE to the source. Source is `{"delete"}` only if the civilian is within 5 m.
6. Civilian gets surrender apply (white-flag / camp tags) and optional walk.
7. Stop. No Conquest.
