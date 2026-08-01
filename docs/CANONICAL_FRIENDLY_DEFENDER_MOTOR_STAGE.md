# Friendly Defender Motor — Stage 2

This stage adds exactly one new runtime path on top of the frozen 60/90 motor baseline:

- Mission: player defense
- Side: friendly defender AI
- Dispatch: one truck at +30 seconds after the defense support engine arms
- Ride: 60 seconds
- Dismount: passengers only
- Cleanup: 90 seconds after dismount

The implementation is mechanically derived from the already runtime-proven friendly-attacker transport blocks at deployment time. The existing friendly-attacker and enemy-attacker engine files are hash-checked before and after the stage-2 overlay and must remain byte-identical.

Not included:

- enemy defender motor support
- recurring trucks
- multiple truck packages
- cadence randomization
- changes to the two already validated motor paths

Runtime acceptance requires a player defense mission where the allied defender truck visibly spawns, retains cab crew and cargo during travel, drives for 60 seconds, emits passengers only, and remains available for 90 seconds afterward.
