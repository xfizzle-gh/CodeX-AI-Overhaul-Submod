# PR A — Static morale / command classification

Static identity only. No Shaken / Panic / Broken / retreat / surrender runtime.

Companion files: `docs/morale_command_phase0_audit.md`, `docs/morale_command_classification.tsv`, `docs/morale_legacy_visual_allowlist.txt`.

**GitHub CI does not prove Code:X freshness.** The hosted runner skips `test_stale_upstream_against_local_codex_when_present` because Workshop Code:X is not present. CI only proves repo-local tag invariants. Stale-upstream proof is the local run of that test against `3261086933`.

## Baselines

- AIO `main` SHA: `2e5286bde3b2fff77c3dbc8f1faa2dda8b767c8d`
- Code:X workshop: `3261086933` `resource/set/breed`
- Code:X `.set` paths: **2091**
- Code:X `.inc` files: **25**
- All `.set` files use `{behaviour soldier}`
- Issue #116 snapshot breed count: 2,091. Local count matches.
- Issue #116 zip SHA-256 was recorded as `7d61324d4406679b2423e6ab138b51ca42cc6410e402ee76e1a7cb8cc061088e`.
  Local live tree was re-inventoried; faction / veterancy / role-candidate counts match that snapshot.
  A zip-identical SHA was not recomputed because the original snapshot archive is not in-repo.

## Marker names

Locked:

- morale: `aio_morale_low` `aio_morale_regular` `aio_morale_trained` `aio_morale_elite`
- command: `aio_cmd_junior` `aio_cmd_primary` `aio_cmd_senior` `aio_cmd_independent`
- special: `aio_discipline` `aio_steadfast`

Stored as breed `{tags}` tokens. Dynamic Shaken/Panic/Broken/link states are **not** written into breeds.

## Totals

- breeds classified / overridden: **2091**
- pre-existing AIO visual overrides preserved: **79**

### Faction

- rusa: 759
- ukr: 566
- nato: 520
- sov: 149
- prc: 52
- usam: 45

### Path family

- nato/2022s: 248
- nato/early: 2
- nato/era2022: 234
- nato/mid: 2
- nato/新建文件夹: 34
- prc/2022s: 48
- prc/early: 2
- prc/mid: 2
- rusa/2022s: 372
- rusa/early: 3
- rusa/era1960: 1
- rusa/era2022: 375
- rusa/mid: 3
- rusa/新建文件夹: 5
- sov/era1960: 149
- ukr/2022s: 238
- ukr/early: 2
- ukr/era1960: 23
- ukr/era2022: 290
- ukr/mid: 2
- ukr/新建文件夹: 11
- usam/(root): 45

### Morale

- aio_morale_low: 168
- aio_morale_regular: 926
- aio_morale_trained: 803
- aio_morale_elite: 194

### Command

- junior: 98
- primary: 164
- senior: 5
- independent: 263
- discipline: 5
- steadfast: 52

## Classification rules

Reviewed from path, formation, role, perks, starting veterancy, skill/SF rank, and crew/command evidence.
Filename regex was only a candidate filter, not the authority.

- LOW: TDF / conscript / Storm-Z / reservist / `basic` militia-quality evidence
- REGULAR: ordinary line formations
- TRAINED: airborne, marines, guards, assault brigades, professional recon, PMC/assault cadres
- ELITE: actual SOF evidence (`seals`/`sas`/`specialforces`/`fsb`/`spetsnaz`/`hurmo`, SSO, SAS, MARSOC, Kraken, HUR, 45 VDV)
- junior: team leaders, assistant SL, `seniorrifleman`, Soviet/PLA `*_senior` deputies
- primary: squad/section leaders
- senior: explicit `reg_officer` plus infantry `rus_cmd` / `ukr_cmd` (Code:X unit tables tag those breeds `rusa_officer` / `ukr_officer` with `cp -25`)
- independent: SOF / selected recon-specialist profiles
- discipline: the same five senior infantry command breeds only
- steadfast: SAS / SEAL / FSB / 45 VDV only
- vehicle/tank/crew `*_cmd` get **no** infantry command beacon, even when the purchase table reuses an officer cost category

## Accepted safe-defaults

These flagged groups stay as classified unless a later owner override names specific paths:

- `seniorrifleman` and Soviet/PLA `*_senior` remain **junior**, not primary/senior
- `nato_cmd` remains **no infantry command** (crewman / tech icon; `nato_cmd` purchase define is not `nato_officer`)
- `usmc_officer` is referenced by `inf_nato.set` but **no breed file exists**; not invented
- vehicle `*_cmd` / `tank_commander` remain non-beacons
- Storm-Z / Demon / TDF / reservist / `basic` remain LOW
- Rangers (`rng_`) remain TRAINED + independent, not elite
- Wagner / Akhmat / Rusich remain TRAINED, not elite
- FR / MAR remain ELITE from `seals` / formation evidence
- rus90 2022s vet-5 line remains REGULAR (era2022 copies are vet 0)
- no MP / commissar / political-officer breeds were found
- later runtime must not assume Senior/Discipline exist on every map; they exist on 5 breed paths only

## Senior / Discipline lock

| Path | Decision | Evidence |
|---|---|---|
| `mp/sov/era1960/reg_officer.set` | senior + discipline | explicit officer breed |
| `mp/rusa/2022s/rus_cmd.set` | senior + discipline | `("rusa_officer")`, tags include `officer`, `cp -25`, solo HQ squad, cost 2500 |
| `mp/rusa/era2022/rus_cmd.set` | senior + discipline | same identity, parallel path |
| `mp/ukr/2022s/ukr_cmd.set` | senior + discipline | `("ukr_officer")`, tags include `officer`, `cp -25` |
| `mp/ukr/era2022/ukr_cmd.set` | senior + discipline | same identity, parallel path |

NATO 2022 play still has **no** Senior/Discipline breed because `usmc_officer` is a dangling unit-table name. Do not invent it.

## Ambiguous owner decisions

Historical flagged rows: **334**. The four infantry `rus_cmd` / `ukr_cmd` rows below were **resolved** to senior+discipline from Code:X officer unit-table evidence. Remaining groups are accepted safe-defaults unless overridden.

### resolved_infantry_cmd_to_senior (4)

- `mp/rusa/2022s/rus_cmd.set` — aio_morale_trained cmd=aio_cmd_senior discipline=1
- `mp/rusa/era2022/rus_cmd.set` — aio_morale_trained cmd=aio_cmd_senior discipline=1
- `mp/ukr/2022s/ukr_cmd.set` — aio_morale_trained cmd=aio_cmd_senior discipline=1
- `mp/ukr/era2022/ukr_cmd.set` — aio_morale_trained cmd=aio_cmd_senior discipline=1

### elite_looking_low_vet (8)

- `mp/nato/era2022/mar_antitank.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_grenadier.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_marksman.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_operator.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_rifleman.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_sniper.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_spotter.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_squadlead.set` — aio_morale_elite cmd=aio_cmd_primary ind=True vet=0 skill=5 sf=None

### no_veterancy_marker (2)

- `mp/ukr/era1960/ukr_wepcrew.set` — aio_morale_regular cmd=None ind=False vet=None skill=None sf=None
- `mp/ukr/era1960/ukr_wepcrew_m2.set` — aio_morale_regular cmd=None ind=False vet=None skill=None sf=None

### officer (1 remaining explicit filename)

- `mp/sov/era1960/reg_officer.set` — aio_morale_trained cmd=aio_cmd_senior discipline=1 vet=5 skill=2

### owner_review_formation_demon (28)

- `mp/ukr/2022s/demon_antitank.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/2022s/demon_antitank_assist.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/2022s/demon_antitank_rpg22.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/2022s/demon_antitank_rpg26.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/2022s/demon_grenadier.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/2022s/demon_marksman.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/2022s/demon_medic.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/2022s/demon_mg.set` — aio_morale_low cmd=None ind=False vet=0 skill=None sf=None
- `mp/ukr/2022s/demon_mg_assist.set` — aio_morale_low cmd=None ind=False vet=0 skill=None sf=None
- `mp/ukr/2022s/demon_mg_rpk.set` — aio_morale_low cmd=None ind=False vet=0 skill=None sf=None
- `mp/ukr/2022s/demon_rifleman.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/2022s/demon_saperi.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/2022s/demon_saperi_RPO.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/2022s/demon_seniorrifleman.set` — aio_morale_low cmd=aio_cmd_junior ind=False vet=0 skill=2 sf=None
- `mp/ukr/2022s/demon_squadlead.set` — aio_morale_low cmd=aio_cmd_primary ind=False vet=0 skill=3 sf=None
- `mp/ukr/era2022/demon_antitank.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/era2022/demon_antitank_assist.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/era2022/demon_antitank_rpg22.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/era2022/demon_antitank_rpg26.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/era2022/demon_grenadier.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/era2022/demon_marksman.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/era2022/demon_medic.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/era2022/demon_mg.set` — aio_morale_low cmd=None ind=False vet=0 skill=None sf=None
- `mp/ukr/era2022/demon_mg_assist.set` — aio_morale_low cmd=None ind=False vet=0 skill=None sf=None
- `mp/ukr/era2022/demon_mg_rpk.set` — aio_morale_low cmd=None ind=False vet=0 skill=None sf=None
- `mp/ukr/era2022/demon_rifleman.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/ukr/era2022/demon_seniorrifleman.set` — aio_morale_low cmd=aio_cmd_junior ind=False vet=0 skill=2 sf=None
- `mp/ukr/era2022/demon_squadlead.set` — aio_morale_low cmd=aio_cmd_primary ind=False vet=0 skill=3 sf=None

### owner_review_formation_fr (16)

- `mp/nato/2022s/fr_antitank.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/fr_grenadier.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/fr_medic.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/fr_operator.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/fr_rifleman.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/fr_sniper.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/fr_spotter.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/fr_squadlead.set` — aio_morale_elite cmd=aio_cmd_primary ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/fr_antitank.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/fr_grenadier.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/fr_medic.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/fr_operator.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/fr_rifleman.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/fr_sniper.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/fr_spotter.set` — aio_morale_elite cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/fr_squadlead.set` — aio_morale_elite cmd=aio_cmd_primary ind=True vet=5 skill=5 sf=None

### owner_review_formation_kor (48)

- `mp/rusa/2022s/kor_antitank_rpg29.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_antitank_rpg30.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_antitank_rpg7.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_assault_rpg26.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_assault_rpg27.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_assault_rpg30.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_atassist.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_atassist_rpg29.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_crew.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_crew_ags.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_crew_nsv.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_crew_spg9.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_grenadier.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_marksman.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_marksman1.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_medic.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_mg.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_mgunasst.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_recon.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_recon1.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_rifleman.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_rifleman3.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_saperi.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_saperi_rpo.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_seniorrifleman.set` — aio_morale_trained cmd=aio_cmd_junior ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/kor_squadlead.set` — aio_morale_trained cmd=aio_cmd_primary ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_antitank_rpg29.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_antitank_rpg30.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_antitank_rpg7.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_assault_rpg26.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_assault_rpg27.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_assault_rpg30.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_atassist.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_atassist_rpg29.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_grenadier.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_marksman.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_marksman1.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_medic.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_mg.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_mgunasst.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_recon.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_recon1.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_rifleman.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_rifleman1.set` — aio_morale_trained cmd=None ind=False vet=0 skill=3 sf=None
- `mp/rusa/era2022/kor_rifleman2.set` — aio_morale_trained cmd=None ind=False vet=0 skill=3 sf=None
- `mp/rusa/era2022/kor_rifleman3.set` — aio_morale_trained cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_seniorrifleman.set` — aio_morale_trained cmd=aio_cmd_junior ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/kor_squadlead.set` — aio_morale_trained cmd=aio_cmd_primary ind=False vet=0 skill=None sf=None

### owner_review_formation_mar (8)

- `mp/nato/era2022/mar_antitank.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_grenadier.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_marksman.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_operator.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_rifleman.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_sniper.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_spotter.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/nato/era2022/mar_squadlead.set` — aio_morale_elite cmd=aio_cmd_primary ind=True vet=0 skill=5 sf=None

### owner_review_formation_mvd (4)

- `mp/sov/era1960/mvd_antitank.set` — aio_morale_low cmd=None ind=False vet=0 skill=1 sf=None
- `mp/sov/era1960/mvd_mg.set` — aio_morale_low cmd=None ind=False vet=0 skill=1 sf=None
- `mp/sov/era1960/mvd_rifleman.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/sov/era1960/mvd_squadlead.set` — aio_morale_low cmd=aio_cmd_primary ind=False vet=0 skill=1 sf=None

### owner_review_formation_rg (5)

- `mp/nato/2022s/rg_javelin.set` — aio_morale_trained cmd=None ind=True vet=4 skill=None sf=5
- `mp/nato/2022s/rg_m2hb_crew.set` — aio_morale_trained cmd=None ind=False vet=4 skill=None sf=5
- `mp/nato/2022s/rg_rifleman.set` — aio_morale_trained cmd=None ind=True vet=4 skill=None sf=5
- `mp/nato/2022s/rg_squadlead.set` — aio_morale_trained cmd=aio_cmd_primary ind=True vet=4 skill=None sf=5
- `mp/nato/2022s/rg_teamleader.set` — aio_morale_trained cmd=aio_cmd_junior ind=True vet=4 skill=5 sf=None

### owner_review_formation_rng (40)

- `mp/nato/2022s/rng_antitank.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/2022s/rng_antitank_r.set` — aio_morale_trained cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/rng_atteamlead.set` — aio_morale_trained cmd=aio_cmd_junior ind=True vet=5 skill=4 sf=None
- `mp/nato/2022s/rng_grenadier.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/2022s/rng_grenadier_r.set` — aio_morale_trained cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/rng_javelin.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/2022s/rng_marksman.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/2022s/rng_medic.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/2022s/rng_medic_r.set` — aio_morale_trained cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/rng_mg.set` — aio_morale_trained cmd=None ind=True vet=5 skill=None sf=None
- `mp/nato/2022s/rng_mg1.set` — aio_morale_trained cmd=None ind=True vet=5 skill=None sf=None
- `mp/nato/2022s/rng_mg_r.set` — aio_morale_trained cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/rng_rifleman.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/2022s/rng_rifleman1.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/2022s/rng_rifleman_r.set` — aio_morale_trained cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/rng_spotter.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/2022s/rng_spotter_r.set` — aio_morale_trained cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/rng_squadlead.set` — aio_morale_trained cmd=aio_cmd_primary ind=True vet=5 skill=4 sf=None
- `mp/nato/2022s/rng_squadlead_r.set` — aio_morale_trained cmd=aio_cmd_primary ind=True vet=5 skill=5 sf=None
- `mp/nato/2022s/rng_vehicleman.set` — aio_morale_trained cmd=None ind=False vet=0 skill=4 sf=None
- `mp/nato/era2022/rng_antitank.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/era2022/rng_antitank_r.set` — aio_morale_trained cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/rng_atteamlead.set` — aio_morale_trained cmd=aio_cmd_junior ind=True vet=5 skill=4 sf=None
- `mp/nato/era2022/rng_grenadier.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/era2022/rng_grenadier_r.set` — aio_morale_trained cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/rng_javelin.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/era2022/rng_marksman.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/era2022/rng_medic.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/era2022/rng_medic_r.set` — aio_morale_trained cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/rng_mg.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/era2022/rng_mg1.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/era2022/rng_mg_r.set` — aio_morale_trained cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/rng_rifleman.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/era2022/rng_rifleman1.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/era2022/rng_rifleman_r.set` — aio_morale_trained cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/rng_spotter.set` — aio_morale_trained cmd=None ind=True vet=5 skill=4 sf=None
- `mp/nato/era2022/rng_spotter_r.set` — aio_morale_trained cmd=None ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/rng_squadlead.set` — aio_morale_trained cmd=aio_cmd_primary ind=True vet=5 skill=4 sf=None
- `mp/nato/era2022/rng_squadlead_r.set` — aio_morale_trained cmd=aio_cmd_primary ind=True vet=5 skill=5 sf=None
- `mp/nato/era2022/rng_vehicleman.set` — aio_morale_trained cmd=None ind=False vet=0 skill=4 sf=None

### owner_review_formation_rusc (16)

- `mp/rusa/2022s/rusc_antitank.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/rusa/2022s/rusc_marksman.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/rusa/2022s/rusc_medic.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/rusa/2022s/rusc_mg.set` — aio_morale_elite cmd=None ind=True vet=0 skill=None sf=None
- `mp/rusa/2022s/rusc_rifleman.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/rusa/2022s/rusc_rifleman2.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/rusa/2022s/rusc_rifleman3.set` — aio_morale_elite cmd=None ind=True vet=0 skill=4 sf=None
- `mp/rusa/2022s/rusc_squadlead.set` — aio_morale_elite cmd=aio_cmd_primary ind=True vet=0 skill=5 sf=None
- `mp/rusa/era2022/rusc_antitank.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/rusa/era2022/rusc_marksman.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/rusa/era2022/rusc_medic.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/rusa/era2022/rusc_mg.set` — aio_morale_elite cmd=None ind=True vet=0 skill=None sf=None
- `mp/rusa/era2022/rusc_rifleman.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/rusa/era2022/rusc_rifleman2.set` — aio_morale_elite cmd=None ind=True vet=0 skill=5 sf=None
- `mp/rusa/era2022/rusc_rifleman3.set` — aio_morale_elite cmd=None ind=True vet=0 skill=4 sf=None
- `mp/rusa/era2022/rusc_squadlead.set` — aio_morale_elite cmd=aio_cmd_primary ind=True vet=0 skill=5 sf=None

### owner_review_formation_sto (37)

- `mp/rusa/2022s/sto_antitank.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/2022s/sto_antitank1.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/2022s/sto_antitank2.set` — aio_morale_low cmd=None ind=False vet=0 skill=3 sf=None
- `mp/rusa/2022s/sto_atassist.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/2022s/sto_atassist1.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/2022s/sto_eng.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/2022s/sto_eng_rpo.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/2022s/sto_marksman.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/2022s/sto_medic.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/2022s/sto_medic1.set` — aio_morale_low cmd=None ind=False vet=0 skill=3 sf=None
- `mp/rusa/2022s/sto_mg.set` — aio_morale_low cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/sto_mg2.set` — aio_morale_low cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/sto_recon.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/2022s/sto_rifleman.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/2022s/sto_rifleman1.set` — aio_morale_low cmd=None ind=False vet=0 skill=3 sf=None
- `mp/rusa/2022s/sto_sniper.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/2022s/sto_sniper1.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/2022s/sto_squadlead.set` — aio_morale_low cmd=aio_cmd_primary ind=False vet=0 skill=4 sf=None
- `mp/rusa/2022s/sto_squadlead1.set` — aio_morale_low cmd=aio_cmd_primary ind=False vet=0 skill=5 sf=None
- `mp/rusa/era2022/sto_antitank.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/era2022/sto_antitank1.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/era2022/sto_antitank2.set` — aio_morale_low cmd=None ind=False vet=0 skill=3 sf=None
- `mp/rusa/era2022/sto_atassist.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/era2022/sto_atassist1.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/era2022/sto_eng.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/era2022/sto_marksman.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/era2022/sto_medic.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/era2022/sto_medic1.set` — aio_morale_low cmd=None ind=False vet=0 skill=3 sf=None
- `mp/rusa/era2022/sto_mg.set` — aio_morale_low cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/sto_mg2.set` — aio_morale_low cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/sto_recon.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/era2022/sto_rifleman.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/era2022/sto_rifleman1.set` — aio_morale_low cmd=None ind=False vet=0 skill=3 sf=None
- `mp/rusa/era2022/sto_sniper.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/era2022/sto_sniper1.set` — aio_morale_low cmd=None ind=False vet=0 skill=2 sf=None
- `mp/rusa/era2022/sto_squadlead.set` — aio_morale_low cmd=aio_cmd_primary ind=False vet=0 skill=4 sf=None
- `mp/rusa/era2022/sto_squadlead1.set` — aio_morale_low cmd=aio_cmd_primary ind=False vet=0 skill=5 sf=None

### owner_review_formation_wgn (30)

- `mp/rusa/2022s/wgn_ammo.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/2022s/wgn_antitank.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/2022s/wgn_assault.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/2022s/wgn_atassist.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/2022s/wgn_egn.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/2022s/wgn_lmg.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/2022s/wgn_lmgassist.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/2022s/wgn_marksman.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/2022s/wgn_medic.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/2022s/wgn_rifleman.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/2022s/wgn_rifleman2.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/2022s/wgn_rifleman3.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/2022s/wgn_squadlead.set` — aio_morale_trained cmd=aio_cmd_primary ind=False vet=3 skill=None sf=4
- `mp/rusa/2022s/wgn_vehicleman.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/2022s/wgn_wepcrew.set` — aio_morale_elite cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/era2022/wgn_ammo.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/era2022/wgn_antitank.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/era2022/wgn_assault.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/era2022/wgn_atassist.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/era2022/wgn_egn.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/era2022/wgn_lmg.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/era2022/wgn_lmgassist.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/era2022/wgn_marksman.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/era2022/wgn_medic.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/era2022/wgn_rifleman.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/era2022/wgn_rifleman2.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/era2022/wgn_rifleman3.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/era2022/wgn_squadlead.set` — aio_morale_trained cmd=aio_cmd_primary ind=False vet=3 skill=None sf=4
- `mp/rusa/era2022/wgn_vehicleman.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=3
- `mp/rusa/era2022/wgn_wepcrew.set` — aio_morale_elite cmd=None ind=False vet=3 skill=None sf=4

### owner_review_formation_wgn2 (20)

- `mp/rusa/2022s/wgn2_antitank.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/2022s/wgn2_assault.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/2022s/wgn2_lmg.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/2022s/wgn2_lmgassist.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/2022s/wgn2_marksman.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/2022s/wgn2_medic.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/2022s/wgn2_rifleman.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/2022s/wgn2_rifleman2.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/2022s/wgn2_rifleman3.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/2022s/wgn2_squadlead.set` — aio_morale_trained cmd=aio_cmd_primary ind=False vet=3 skill=5 sf=None
- `mp/rusa/era2022/wgn2_antitank.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/era2022/wgn2_assault.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/era2022/wgn2_lmg.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/era2022/wgn2_lmgassist.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/era2022/wgn2_marksman.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/era2022/wgn2_medic.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/era2022/wgn2_rifleman.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/era2022/wgn2_rifleman2.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/era2022/wgn2_rifleman3.set` — aio_morale_trained cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/era2022/wgn2_squadlead.set` — aio_morale_trained cmd=aio_cmd_primary ind=False vet=3 skill=5 sf=None

### reservist (24)

- `mp/rusa/2022s/rus114_rez_antitank.set` — aio_morale_low cmd=None ind=False vet=2 skill=2 sf=None
- `mp/rusa/2022s/rus114_rez_antitank_og7v.set` — aio_morale_low cmd=None ind=False vet=2 skill=2 sf=None
- `mp/rusa/2022s/rus114_rez_antitank_rpg26.set` — aio_morale_low cmd=None ind=False vet=2 skill=2 sf=None
- `mp/rusa/2022s/rus114_rez_medic.set` — aio_morale_low cmd=None ind=False vet=2 skill=2 sf=None
- `mp/rusa/2022s/rus114_rez_mg.set` — aio_morale_low cmd=None ind=False vet=2 skill=None sf=None
- `mp/rusa/2022s/rus114_rez_mg_assault.set` — aio_morale_low cmd=None ind=False vet=2 skill=None sf=None
- `mp/rusa/2022s/rus114_rez_rifleman.set` — aio_morale_low cmd=None ind=False vet=2 skill=2 sf=None
- `mp/rusa/2022s/rus114_rez_rifleman_assault.set` — aio_morale_low cmd=None ind=False vet=2 skill=2 sf=None
- `mp/rusa/2022s/rus114_rez_seniorrifleman.set` — aio_morale_low cmd=aio_cmd_junior ind=False vet=2 skill=2 sf=None
- `mp/rusa/2022s/rus114_rez_seniorrifleman_assault.set` — aio_morale_low cmd=aio_cmd_junior ind=False vet=2 skill=2 sf=None
- `mp/rusa/2022s/rus114_rez_squadlead.set` — aio_morale_low cmd=aio_cmd_primary ind=False vet=2 skill=4 sf=None
- `mp/rusa/2022s/rus114_rez_squadlead_assault.set` — aio_morale_low cmd=aio_cmd_primary ind=False vet=2 skill=4 sf=None
- `mp/rusa/era2022/rus114_rez_antitank.set` — aio_morale_low cmd=None ind=False vet=2 skill=2 sf=None
- `mp/rusa/era2022/rus114_rez_antitank_og7v.set` — aio_morale_low cmd=None ind=False vet=2 skill=2 sf=None
- `mp/rusa/era2022/rus114_rez_antitank_rpg26.set` — aio_morale_low cmd=None ind=False vet=2 skill=2 sf=None
- `mp/rusa/era2022/rus114_rez_medic.set` — aio_morale_low cmd=None ind=False vet=2 skill=2 sf=None
- `mp/rusa/era2022/rus114_rez_mg.set` — aio_morale_low cmd=None ind=False vet=2 skill=None sf=None
- `mp/rusa/era2022/rus114_rez_mg_assault.set` — aio_morale_low cmd=None ind=False vet=2 skill=None sf=None
- `mp/rusa/era2022/rus114_rez_rifleman.set` — aio_morale_low cmd=None ind=False vet=2 skill=2 sf=None
- `mp/rusa/era2022/rus114_rez_rifleman_assault.set` — aio_morale_low cmd=None ind=False vet=2 skill=2 sf=None
- `mp/rusa/era2022/rus114_rez_seniorrifleman.set` — aio_morale_low cmd=aio_cmd_junior ind=False vet=2 skill=2 sf=None
- `mp/rusa/era2022/rus114_rez_seniorrifleman_assault.set` — aio_morale_low cmd=aio_cmd_junior ind=False vet=2 skill=2 sf=None
- `mp/rusa/era2022/rus114_rez_squadlead.set` — aio_morale_low cmd=aio_cmd_primary ind=False vet=2 skill=4 sf=None
- `mp/rusa/era2022/rus114_rez_squadlead_assault.set` — aio_morale_low cmd=aio_cmd_primary ind=False vet=2 skill=4 sf=None

### sof_or_ranger_crew (16)

- `mp/nato/2022s/rg_m2hb_crew.set` — aio_morale_trained cmd=None ind=False vet=4 skill=None sf=5
- `mp/nato/2022s/rng_vehicleman.set` — aio_morale_trained cmd=None ind=False vet=0 skill=4 sf=None
- `mp/nato/era2022/rng_vehicleman.set` — aio_morale_trained cmd=None ind=False vet=0 skill=4 sf=None
- `mp/nato/era2022/usa_vehicleman.set` — aio_morale_trained cmd=None ind=False vet=0 skill=3 sf=None
- `mp/rusa/2022s/sso_9m133_crew.set` — aio_morale_elite cmd=None ind=False vet=5 skill=None sf=5
- `mp/rusa/2022s/wgn_wepcrew.set` — aio_morale_elite cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/era2022/spz_eng.set` — aio_morale_elite cmd=None ind=False vet=0 skill=None sf=5
- `mp/rusa/era2022/wgn_wepcrew.set` — aio_morale_elite cmd=None ind=False vet=3 skill=None sf=4
- `mp/rusa/新建文件夹/sso_9m133_crew.set` — aio_morale_elite cmd=None ind=False vet=0 skill=None sf=5
- `mp/sov/era1960/spz_guncrew_9p132.set` — aio_morale_elite cmd=None ind=False vet=4 skill=4 sf=None
- `mp/ukr/2022s/hur_crew.set` — aio_morale_elite cmd=None ind=False vet=0 skill=None sf=3
- `mp/ukr/2022s/hur_wepcrew_mk19.set` — aio_morale_elite cmd=None ind=False vet=0 skill=None sf=3
- `mp/ukr/2022s/hur_wepcrew_tow2.set` — aio_morale_elite cmd=None ind=False vet=0 skill=None sf=3
- `mp/ukr/era2022/hur_crew.set` — aio_morale_elite cmd=None ind=False vet=0 skill=None sf=3
- `mp/ukr/era2022/hur_wepcrew_mk19.set` — aio_morale_elite cmd=None ind=False vet=0 skill=None sf=3
- `mp/ukr/era2022/hur_wepcrew_tow2.set` — aio_morale_elite cmd=None ind=False vet=0 skill=None sf=3

### unusual_high_vet_regular (31)

- `mp/nato/2022s/usarmy_teamlead.set` — aio_morale_regular cmd=aio_cmd_junior ind=False vet=5 skill=3 sf=None
- `mp/nato/2022s/usarmy_teamlead_xm7.set` — aio_morale_regular cmd=aio_cmd_junior ind=False vet=5 skill=None sf=None
- `mp/nato/era2022/acav_teamlead.set` — aio_morale_regular cmd=aio_cmd_junior ind=False vet=5 skill=3 sf=None
- `mp/nato/era2022/acav_teamlead_xm7.set` — aio_morale_regular cmd=aio_cmd_junior ind=False vet=5 skill=None sf=None
- `mp/nato/era2022/usarmy_teamlead.set` — aio_morale_regular cmd=aio_cmd_junior ind=False vet=5 skill=3 sf=None
- `mp/nato/era2022/usarmy_teamlead_xm7.set` — aio_morale_regular cmd=aio_cmd_junior ind=False vet=5 skill=None sf=None
- `mp/nato/新建文件夹/1cav_teamlead.set` — aio_morale_regular cmd=aio_cmd_junior ind=False vet=5 skill=4 sf=None
- `mp/rusa/2022s/rus90_antitank.set` — aio_morale_regular cmd=None ind=False vet=5 skill=2 sf=None
- `mp/rusa/2022s/rus90_antitank_rpg26.set` — aio_morale_regular cmd=None ind=False vet=5 skill=2 sf=None
- `mp/rusa/2022s/rus90_antitank_rpg27.set` — aio_morale_regular cmd=None ind=False vet=5 skill=2 sf=None
- `mp/rusa/2022s/rus90_assault.set` — aio_morale_regular cmd=None ind=False vet=5 skill=2 sf=None
- `mp/rusa/2022s/rus90_lmgassist.set` — aio_morale_regular cmd=None ind=False vet=5 skill=None sf=None
- `mp/rusa/2022s/rus90_marksman.set` — aio_morale_regular cmd=None ind=False vet=5 skill=2 sf=None
- `mp/rusa/2022s/rus90_medic.set` — aio_morale_regular cmd=None ind=False vet=5 skill=2 sf=None
- `mp/rusa/2022s/rus90_mg.set` — aio_morale_regular cmd=None ind=False vet=5 skill=None sf=None
- `mp/rusa/2022s/rus90_recon.set` — aio_morale_regular cmd=None ind=False vet=5 skill=2 sf=None
- `mp/rusa/2022s/rus90_rifleman.set` — aio_morale_regular cmd=None ind=False vet=5 skill=2 sf=None
- `mp/rusa/2022s/rus90_rifleman_assault.set` — aio_morale_regular cmd=None ind=False vet=5 skill=2 sf=None
- `mp/rusa/2022s/rus90_saperi.set` — aio_morale_regular cmd=None ind=False vet=5 skill=2 sf=None
- `mp/rusa/2022s/rus90_saperi_mg.set` — aio_morale_regular cmd=None ind=False vet=5 skill=None sf=None
- `mp/rusa/2022s/rus90_saperi_rpo.set` — aio_morale_regular cmd=None ind=False vet=5 skill=2 sf=None
- `mp/rusa/2022s/rus90_saperi_squadlead.set` — aio_morale_regular cmd=aio_cmd_primary ind=False vet=5 skill=3 sf=None
- `mp/rusa/2022s/rus90_seniorrifleman.set` — aio_morale_regular cmd=aio_cmd_junior ind=False vet=5 skill=2 sf=None
- `mp/rusa/2022s/rus90_sniper.set` — aio_morale_regular cmd=None ind=False vet=5 skill=2 sf=None
- `mp/rusa/2022s/rus90_squadlead.set` — aio_morale_regular cmd=aio_cmd_primary ind=False vet=5 skill=3 sf=None
- `mp/rusa/2022s/rus90_vehicleman.set` — aio_morale_regular cmd=None ind=False vet=5 skill=2 sf=None
- `mp/sov/era1960/reg_officer.set` — aio_morale_trained cmd=aio_cmd_senior ind=False vet=5 skill=2 sf=None
- `mp/sov/era1960/sup_manpad_operator.set` — aio_morale_regular cmd=None ind=False vet=8 skill=3 sf=None
- `mp/usam/1ad_teamlead.set` — aio_morale_regular cmd=aio_cmd_junior ind=False vet=5 skill=2 sf=None
- `mp/usam/1cav_teamlead.set` — aio_morale_regular cmd=aio_cmd_junior ind=False vet=5 skill=4 sf=None
- `mp/usam/4id_teamlead.set` — aio_morale_regular cmd=aio_cmd_junior ind=False vet=5 skill=4 sf=None

### vehicle_or_crew_cmd (10)

- `mp/nato/2022s/nato_cmd.set` — aio_morale_regular cmd=None ind=False vet=0 skill=1 sf=None
- `mp/nato/era2022/nato_cmd.set` — aio_morale_regular cmd=None ind=False vet=0 skill=1 sf=None
- `mp/rusa/2022s/ldnr_tank_commander.set` — aio_morale_low cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/2022s/rus_vehicleman_cmd.set` — aio_morale_regular cmd=None ind=False vet=0 skill=1 sf=None
- `mp/rusa/era2022/ldnr_tank_commander.set` — aio_morale_low cmd=None ind=False vet=0 skill=None sf=None
- `mp/rusa/era2022/rus_vehicleman_cmd.set` — aio_morale_regular cmd=None ind=False vet=0 skill=1 sf=None
- `mp/ukr/2022s/ukr47_crew_cmd.set` — aio_morale_trained cmd=None ind=False vet=1 skill=4 sf=None
- `mp/ukr/2022s/ukr_vehicleman_cmd.set` — aio_morale_regular cmd=None ind=False vet=0 skill=1 sf=None
- `mp/ukr/era2022/ukr47_crew_cmd.set` — aio_morale_trained cmd=None ind=False vet=1 skill=4 sf=None
- `mp/ukr/era2022/ukr_vehicleman_cmd.set` — aio_morale_regular cmd=None ind=False vet=0 skill=1 sf=None

## Stale-upstream audit

For every new overlay, AIO file minus AIO morale/command tags must equal the current Code:X file.

New overlays: **2012** clean (tags-only delta).

Pre-existing AIO visual overrides retained and then tagged: **79** (expected; already differed from Code:X on `main`).

## Attribution

- Command-structure concepts adapted from Old Boy Command Structure, Workshop `3604287428`.
- Morale/suppression concepts adapted from Fixed Emplacement, Workshop `3702483522` and `3669912659`.
- Source mods are not vendored into this repository.

