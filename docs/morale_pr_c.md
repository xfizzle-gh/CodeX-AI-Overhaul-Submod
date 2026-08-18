# PR C — Shaken / Panic / recovery

Stacked on #118. No Broken, retreat, surrender, or command.

`CE_MORALE_SYS mi=1 human=1 tag_add=1 tag_read=1 known_tag=1 pr_a=1 canary=1 inv=1 ai=1 pressure=1 suppressed=0 shaken=1 recover=1 recover_panic=1 panic=1 player_ex=1`

- Combat pressure is a proven `see_actors` + `tag pair` proxy, not a proven suppression signal.
- Only the matched soldier is Shaken or promoted to Panic.
- One contact cannot instantly Steady → Panic (`just_shaken` hold).
- Recovery requires per-actor `recent_pressure` expiry and latches only after the tag transition.
- Players may be Shaken/Panic. Autodemo still uses one AI human.
