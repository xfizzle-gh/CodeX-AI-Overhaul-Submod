# PR C — combined morale machine

Stacked on #118. One playable loop plus one log line.

`CE_MORALE_SYS mi=1 human=1 tag_add=1 tag_read=1 known_tag=1 pr_a=1 canary=1 inv=1 ai=1 cmd=1 pressure=1 shaken=1 recover=1 panic=1 broken=1 retreat=1 surrender=1 player_ex=1`

- Combat `see_enemy` can Shaken/Panic/Broken AI only
- Broken retreats first
- Surrender is last, only after retreat, never player seizure/delete
- Autodemo walks the full pipeline on one AI human so one 60–90s match reports every bit
- `AI_ABSENT` if no non-player human appears
