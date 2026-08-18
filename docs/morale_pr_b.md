# PR B — marker init and runtime tag apply

Stacked on PR A head `46bea08b47130e87d3a80fa6a0d9850b07ad168b`.
No automatic Steady/Shaken/Panic machine. No Broken / retreat / surrender.

Static identity is hidden `aio_marker_*` inventory items from PR A. PR B converts those to runtime `aio_*` tags. Dynamic Shaken/Panic stays tag-based.

## Native procedure (authoritative)

One 30–60 second CWA Conquest start in folder `3636883799` after exact-head re-audit.

Place a canary in the starting force: RUSA Militsiya Rifleman (`lud_rifleman`) or UKR `ter_rifleman` / `ukr_rifleman`.

Read `game.log` for:

`CE_MORALE_ARCH mi=1 human=1 tag_add=1 tag_read=1 known_tag=1 pr_a_source=1 canary_present=1 inventory_canary=1 shaken=1 panic=1 player_excluded=1`

Success now requires `pr_a_source=1` (hidden marker → runtime tag). `pr_a_source=0` is a fail.

Shaken/Panic bits are independent and need a non-player AI human.

On-screen talks are not the acceptance signal.

## CI vs native

Exact-head CE morale workflow runs the full morale test set. Native proof is the `CE_MORALE_ARCH` line after re-audit.
