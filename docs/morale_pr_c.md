# PR C — Shaken / Panic / recovery

FE recovery port. Trigger is native `{state suppressed}`.

Smoke test: get suppressed → Shaken → Panic → leave combat ~20s → recover.

`CE_MORALE_SYS` should show `shaken=1 panic=1` in the fight, then `recover=1` after contact breaks.
