# Canonical Motorized Insert Fix

**Status:** LIVE-RUNTIME VALIDATED

**Validated:** 2026-08-01

**Canonical code commit:** `38785d41db871dd989f72a64a532e62dfc1bb4dd`

**Frozen archive branch:** `archive/runtime-validated-motor-base-entry-2026-08-01`

**Canonical tracking issue:** `#68`

## What was broken

The motorized-insert timer and package claim were working. The vehicle actor was created, but the linked truck package was placed at generated `attack_support_rear_a1` / `attack_support_rear_b1` pads.

Those projected rear pads can lie beyond the usable battlefield or outside a navigable approach on CWA maps. The result is a valid truck actor that exists off-map and never becomes visible to the player.

The generic infantry placement helper also places entities one at a time. That is not a safe placement path for a linked package containing a vehicle hull, crew, and passengers.

## The canonical fix

1. Claim the full linked package: truck hull, crew, and passengers.
2. Place the full linked package in one placement operation.
3. Select the correct original map spawn-centroid waypoint:
   - `attack_support_entry_a`
   - `attack_support_entry_b`
4. Never use `attack_support_rear_a1` or `attack_support_rear_b1` for motor vehicles.
5. Never run a linked vehicle package through the generic one-entity-at-a-time infantry placement helper.
6. Preserve the proven lifecycle after placement:
   - truck advances toward the selected objective;
   - passengers emit after the travel window;
   - passengers advance on foot;
   - the empty truck receives a withdrawal order;
   - delayed cleanup removes the truck or wreck.

## Runtime validation

The validated run showed:

- a NATO FMTV visibly on the battlefield at approximately `01:00`;
- the infantry package present around the truck;
- infantry dismounted and advancing;
- the empty truck continuing to move at approximately `01:14`;
- no crash during the sequence.

The runtime log also records the active `fmtv` actor while the one-shot motor test was complete and its budget had been consumed:

- `motor_left 0`
- `test 1`
- `test_done 1`

## Regression rule

Any production port, recurring scheduler, defender-side implementation, or refactor must preserve this invariant:

> **Whole linked motor package + original base-entry waypoint placement**

Restoring motor placement to generated rear pads is a confirmed regression.

## Production integration checklist

- [ ] Port the exact placement invariant into the current experiment architecture.
- [ ] Apply it to the friendly attack motor path.
- [ ] Apply it to the enemy attack motor path.
- [ ] Apply it to the friendly defense motor path.
- [ ] Apply it to the enemy defense motor path.
- [ ] Preserve dismount, infantry advance, withdrawal, and cleanup behavior.
- [ ] Add recurrence only after all four one-shot paths are runtime-validated.
