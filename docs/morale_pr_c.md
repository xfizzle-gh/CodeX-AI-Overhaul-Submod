# PR C — Shaken / Panic / recovery

Stacked on #118. No Broken, retreat, surrender, or command.

`CE_MORALE_SYS mi=1 human=1 tag_add=1 tag_read=1 known_tag=1 pr_a=1 canary=1 inv=1 ai=1 pressure=1 shaken=1 recover=1 panic=1 player_ex=1`

- Live pressure is engine `{state suppressed}`, not `see_enemy`
- Production Shaken apply latches `shaken=1` before `tag_add`
- Timed recovery after suppression ends must prove `recover=1`
- Autodemo on one AI human: Shaken latch → recover → Panic
