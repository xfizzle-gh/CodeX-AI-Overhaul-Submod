# PR D/E — command, cohesion, resistance

Fixes vs first #120 head: link is a heartbeat, Weak/Lost come from missing a nearby commander, Discipline resists instead of skipping recovery, commander corpses shock once.

## Command

- Pulse: living junior/primary/senior within 50 m → `aio_cmd_linked`
- Pulse: within 80 m → `aio_cmd_in_range`
- Recompute: miss two pulses → drop link. In-range but not linked → weak. No in-range → lost
- Commanders and independent stay linked
- `see_actors {enemy}` is FE/Old Boy grammar for the other party

## Resistance (existing machine)

- Everyone can Shaken. Encouraged blocks Panic escalate only. Steadfast is longer hold + faster recover, not immunity.
- just_shaken hold: steadfast 12s, elite 10s, linked/trained 8s, regular 6s, low 4s, lost 3s
- Recover: encouraged/steadfast 8/10, elite 10/12, trained 14/16, linked 16/16, regular 20/20, low 24/24, lost 28/28
- Live veterancy: probe only (`vet_live=`). Not used for math unless the probe reads 1

No movement seizure. No surrender.
