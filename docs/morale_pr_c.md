# PR C — Shaken / Panic / recovery

Native PASS on `8ca59a85550a02a8aa8831378a0336311493fab6`.

Trigger is native `{state suppressed}`. Recovery is FE per-actor 20s, reverse order: Panic → Shaken → Steady. No Broken.

Proven in Conquest `game.log`:

- `00:02:25` Shaken
- `00:02:45` `recover=1 recover_clear=1`
- `00:03:25` Panic
- `00:03:45` `recover_panic=1 recover_clear=1`

Zero `Can't call effect` / inactive-entity errors.
