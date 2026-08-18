# PR C — Shaken / Panic / recovery

FE recovery port. Trigger is native `{state suppressed}`.

Recovery is reverse order: Panic → Shaken → Steady. No Broken in this slice.

Smoke test: get suppressed → Shaken → Panic, then wait ~40s. Recovery starts on the tags, same as FE.

`CE_MORALE_SYS` should show `shaken=1 panic=1` in the fight, then `recover_panic=1 recover=1 recover_clear=1` after contact breaks.
