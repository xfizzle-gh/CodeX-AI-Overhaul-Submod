# Skirmish AI stack test plan

Test the pull requests in order. Do not combine failures from later content with the base spawn-reliability test.

## PR #23: allied bot spawn reliability

1. Start Battle Zones with one human player, one allied AI, and at least one enemy AI.
2. Use a map with a friendly backfield objective.
3. Confirm the allied AI prints the no-Aux fallback at most once.
4. Confirm every AI logs a purchase attempt and spawns a squad.
5. Let the match run for at least five purchase cycles.
6. Confirm the allied AI sends its first available squad toward the backfield objective.

Expected diagnostic:

```text
has no Aux purchase; selecting a normal unit for the backfield objective
```

A failed purchase should retry in about 16 seconds rather than remaining idle for roughly 91 seconds.

## PR #24: role-aware buying and tactics

Run at least one match with each faction controlled by AI.

- Full squads should remain the most common purchases.
- MG, medic, recon, sniper, engineer, and AT teams should appear without dominating the army.
- Dedicated AT teams and AT-equipped squads should become more likely when the bot has too little anti-tank infantry.
- Bots should prioritize uncaptured objectives during the opening phase.
- Once objectives are contested, some squads may use SeekAndDestroy while the majority continue objective play.
- `_lua_alert` and `lua_alert` squads should both use SeekAndDestroy.

## PR #25: doctrine coverage

Run `tools\install_prc_doctrine_portraits.bat` once after pulling the branch.

For NATO, Ukraine, Russia, and PRC:

1. Open the doctrine tab.
2. Confirm exactly two doctrine cards appear.
3. Confirm both cards have a matching portrait.
4. Purchase each doctrine once as a human player.
5. Let an AI-controlled faction complete multiple purchase cycles.
6. Confirm no missing breed, doctrine identifier, unit icon, or purchase-module errors appear in the log.

## Failure capture

For any failure, preserve the complete log from game start through at least 30 seconds after the first bad event. Record:

- map name
- human faction
- allied AI faction
- enemy AI faction or factions
- AI difficulty
- which pull request branch was installed
- whether the PRC portrait installer was run
