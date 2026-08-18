# PR C — Shaken / Panic / Broken state / reverse recovery

Trigger is native `{state suppressed}`. Recovery is FE per-actor 20s delay, reverse order only:

`Steady → Shaken → Panic → Broken → Panic → Shaken → Steady`

No retreat movement, no surrender, no command aura.

Smoke: stay suppressed until `broken=1`, leave combat ~60s.

`CE_MORALE_SYS` should show `shaken=1 panic=1 broken=1` in the fight, then `recover_broken=1 recover_panic=1 recover=1 recover_clear=1` after contact breaks.
