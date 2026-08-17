# Phase 0 — evidence freeze

PR A classification authority. No runtime behavior.

## Repositories

| Tree | Path | Role |
|---|---|---|
| AIO working git | `3636883799` | dirty `feature/allied-support-command`; left untouched |
| AIO PR worktree | `3636883799-morale` | branched from accepted `main` |
| Code:X | `3261086933` | current authoritative breed source |
| West 81 | `2897299509` | syntax/inheritance reference only |
| Old Boy | `3604287428` | command-structure source |
| Fixed Emplacement breeds | `3702483522` | morale metadata source |
| Fixed Emplacement scripts | `3669912659` | morale modifier / recovery / surrender scripts |

Accepted AIO `main`: `2e5286bde3b2fff77c3dbc8f1faa2dda8b767c8d`

## Code:X vs issue #116 snapshot

Local live Code:X `resource/set/breed`:

- 2,091 `.set` files
- 25 `.inc` files
- all 2,091 use `{behaviour soldier}`
- 111 have no `{tags ...}` line
- RUSA 759 / UKR 566 / NATO 520 / SOV 149 / PRC 52 / USAM 45
- veterancy: 0=1608, 1=89, 2=136, 3=78, 4=62, 5=113, 6=1, 8=2, none=2
- no-veterancy files are the same two era1960 UKR weapon-crew breeds named in #116
- command-name candidates match: teamlead 35, squadlead 166, seniorrifleman 52, `_cmd` 12, tank_commander 2, officer 1
- `skill_rank_sf_*` assignments = 274
- `spetsnaz` perk = 40; `spetsnaz_sso` adds 20 more files

The #116 zip SHA-256 was not recomputed; the original snapshot archive is not in this repo. Every recorded count matches the live Workshop tree. **Local Code:X wins and was used as the file base.**

Parallel families `2022s`, `era2022`, `early`, `mid`, and `新建文件夹` remain independent identities.

## Old Boy findings that affect later PRs

- Inventory dummy rank tokens convert to runtime commander/private tags.
- Any NCO/officer is an equal faction-wide commander. No squad binding.
- Hysteresis: warning >60 m, lost >80 m, recover-from-warning <50 m, regroup complete <20 m.
- Lost command seizes movement (`ai_move disable`, drop orders, forced move to nearest commander).
- Recovery drops orders and forces hold + cover.
- Surrender is lost-command + enemy <30 m, then player 0 + delete.
- Vehicle inhabited = commander.
- `advance_ratio` / `retreat_ratio` are **not** part of Old Boy.

Do not copy faction-wide aura, player-control seizure, or surrender-delete. Keep hysteresis / witness-death / commanded-commander-exempt ideas for later PRs.

## Fixed Emplacement findings that affect later PRs

- Shaken accuracy ×0.75 and Panic accuracy ×0.50 / range ×0.80 are confirmed in `morale_system.mod`.
- Local mods never apply `shaken` / `panic` / `morale_broken`; they only react.
- Recovery effect bodies are not local.
- Discipline encouragement radius is 25 m.
- Fanatical is not break-immune locally.
- Native `suppressed` is not used in either FE folder.
- Projectile/shell impact suppression zones are **not present locally**. Phase H remains research.
- FE Broken immediately 50/50 retreat vs surrender. Our locked design rejects that: surrender is last.

## West 81

Not required for PR A. Code:X soldier breeds are complete files, not West 81 includes.

## AIO architecture that later runtime PRs must respect

- Live CE tree is `resource/map/multi/ce/` via `dcg_script.inc`, not `resource/map_scripts/` alone.
- No existing infantry morale/surrender engine.
- Recurring movement writers: `conquest.lua` CaptureFlag, four support-wave engines, `ce_lua_triggers.inc`, patrol/hold/zone setup, spawn waypoint graphs.
- Existing ownership channel: `_lua_mi` / `repairing`.
- Existing AIO breed overrides on `main`: 79 visual/equipment files. All 79 differ from current Code:X. PR A preserved those files and only added tags.

## Marker lock

Static breed tags only:

- `aio_morale_low|regular|trained|elite`
- `aio_cmd_junior|primary|senior|independent`
- `aio_discipline`
- `aio_steadfast`

Dynamic states stay out of breeds.

## Live veterancy

Not probed in-game in this PR. Starting `veterancy_lvl_*` is classification evidence only. Campaign-earned veterancy is **not** faked.

## Attribution

Old Boy `3604287428`. Fixed Emplacement `3702483522` and `3669912659`. Source mods are not vendored.
