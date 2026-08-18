# F0 + Broken / regroup / surrender

## F0 conclusion

FE and Old Boy do **not** move Broken units with `advance_ratio` / `retreat_ratio`.
They use tags + `{action move}` to a rally/commander.

AIO writers stamp `retreat_ratio 0` and `no_retreat on`. Do not change global `human.ext` (`advance_ratio 0.5` / `retreat_ratio 4`).

Production path:
- actor-scoped `advance_ratio 0.1` / `retreat_ratio 4` as assist only
- explicit move to nearest living junior/primary/senior commander
- `aio_morale_owned` makes Conquest/support/CE writers yield

## Production

- Broken: Panic + (lost or shock), after 8s `just_panic`. Not every Panic. Not players.
- Owned: drop orders, free move, rally to commander.
- 20s still lost → `aio_morale_regroup_failed`
- Recover when linked and not suppressed: Broken → Panic → existing Panic→Shaken→Steady. Releases owned.
- Surrender: Broken + failed regroup + enemy human <30m. Not elite/steadfast/independent/player. No delete, no player 0.
