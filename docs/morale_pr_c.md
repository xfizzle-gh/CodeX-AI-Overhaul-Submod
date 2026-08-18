# PR C — Shaken / Panic / recovery

Stacked on #118. No Broken, retreat, surrender, or command.

`CE_MORALE_SYS mi=1 human=1 tag_add=1 tag_read=1 known_tag=1 pr_a=1 canary=1 inv=1 ai=1 pressure=1 suppressed=1 shaken=1 recover=1 recover_panic=1 panic=1 player_ex=1`

- Production pressure is native `{state suppressed}` (observed in Conquest `game.log`).
- Initial entry adds Shaken/`just_shaken`. Ongoing suppression only refreshes `recent_pressure`.
- Only suppressed soldiers are Shaken or promoted to Panic.
- Per-actor age tags tick together, oldest first. No serialized per-actor sleep.
- Recovery requires pressure expiry and latches after the tag transition.
- Players may be Shaken/Panic.
