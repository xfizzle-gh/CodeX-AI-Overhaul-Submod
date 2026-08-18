# PR C — Shaken / Panic / recovery

Stacked on #118. No Broken, retreat, surrender, or command.

`CE_MORALE_SYS mi=1 human=1 tag_add=1 tag_read=1 known_tag=1 pr_a=1 canary=1 inv=1 ai=1 pressure=1 suppressed=0 shaken=1 recover=1 recover_panic=1 panic=1 player_ex=1`

- Production pressure is proven CE `see_enemy`. Phase 0 found zero native `suppressed` hits in local FE/West 81/CE.
- `{state suppressed}` is observe-only (`suppressed=`). It does not gate production.
- Recovery is `PANIC -> SHAKEN -> STEADY`.
- Autodemo on one AI human must prove `shaken=1 recover=1 recover_panic=1 panic=1`.
