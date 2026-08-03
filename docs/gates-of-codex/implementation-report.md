# Gates of CodeX implementation report

## Executive summary

Gates of CodeX is implemented as a clean-room strategic campaign application and Dynamic Conquest bridge for Call to Arms: Gates of Hell with Code:X.

The project was added through four merged implementation checkpoints:

- PR #84: strategic campaign core
- PR #85: Code:X installation and unit catalog scanner
- PR #86: Dynamic Conquest save export and result import bridge
- PR #87: command-line application, desktop map, installer, packaging workflow, and operations documentation

The implementation does not redistribute the supplied Gates of Europa executable, Unity assets, or decompiled source. It reproduces observable file contracts and behavior using new source code.

## Recovered architecture

The supplied Workshop project is not primarily a conventional GoH data mod. It contains a separate Unity Mono campaign application that maintains strategic state outside Gates of Hell. Tactical battles are exchanged through a Dynamic Conquest save archive.

The recovered interoperability contract uses:

- `status` for factions, map configuration, resources, research, and battle counters
- `campaign.scn` for persistent Human and Entity objects, Inventory blocks, and CampaignSquads stage assignments
- `campaign.sav` as the archive containing those two files
- updated `playedGames` and `wonGames` values to determine whether a battle completed and whether the player won
- updated CampaignSquads rows to determine surviving tactical squads

Gates of CodeX implements this contract without replacing Code:X's tactical AI files.

## Strategic campaign implementation

The campaign model supports:

- NATO, Ukraine, Russia, PRC, and neutral ownership
- reciprocal province adjacency validation
- province resources, terrain metadata, map regions, fortifications, and display coordinates
- persistent battalions with faction, type, roster, supply, experience, movement, and combat actions
- neutral capture and hostile battle creation
- pending battle identity and attacker/defender stage assignment
- auto-resolve for battles that are not played in GoH
- casualties, destroyed battalion cleanup, retreats, province capture, resources, and turn order
- atomic JSON save replacement to reduce campaign corruption risk

A packaged four-faction theater is included. Its battalion rosters are replaced at campaign creation with valid units chosen from the installed Code:X catalog.

## Code:X discovery and catalog

The application searches Steam libraries, local GoH mods, Workshop content, and GoH profile directories.

The Code:X scanner reads `.set` and Lua files and records:

- unit name and faction
- period and doctrine
- squad member breeds and counts
- vehicle entities
- action references
- purchase type tags
- doctrine costs
- estimated manpower costs
- inferred categories such as infantry, recon, vehicle, IFV, tank, artillery, and air defense
- source files and a deterministic catalog signature

The installed Code:X data remains authoritative. This avoids embedding a stale private snapshot of the mod into the campaign application.

## Dynamic Conquest bridge

The bridge generates the observed version 9 GoH status structure with Code:X faction codes:

- `nato`
- `ukr`
- `rusa`
- `prc`

It generates `campaign.scn` with:

- Human objects resolved to installed Code:X breed paths
- vehicle Entity objects
- unique object IDs
- one Inventory block for every Human or Entity
- CampaignSquads rows linked to attacker and defender stages
- graph validation that rejects missing objects, missing inventories, duplicate IDs, and empty squad output

It then atomically writes `campaign.sav` and a neighboring `.goc.json` manifest. The manifest binds the save to one campaign and one pending battle and records baseline win counters and the Code:X catalog signature.

After the tactical battle, the importer:

1. verifies that `playedGames` advanced
2. compares `wonGames` against the export baseline
3. identifies the tactical winner from the player's stance
4. parses surviving CampaignSquads by stage
5. replaces strategic battalion rosters with the surviving units
6. applies retreat, destruction, and province ownership rules
7. atomically persists the updated strategic campaign

## User interfaces

### Command line

The `gates-of-codex` entry point provides:

- `doctor`
- `scan`
- `new`
- `show`
- `move`
- `auto-resolve`
- `end-turn`
- `export-battle`
- `import-battle`
- `launch`
- `ui`

### Desktop map

The Tk desktop application provides:

- campaign open and save
- rendered province connections and faction ownership
- battalion selection and roster details
- movement and hostile attack creation
- auto-resolve
- tactical save export and import
- turn advancement

### Installation and packaging

The repository includes:

- a PowerShell source installer using an isolated Python virtual environment
- an executable entry point for PyInstaller
- a GitHub Actions matrix for Python 3.11 and 3.13 on Windows and Linux
- a Windows executable artifact job

## Validation completed

Fourteen automated tests pass locally. Coverage includes:

- scenario and graph validation
- neutral capture
- pending battle creation
- deterministic auto-resolve completion
- campaign JSON round trip
- Code:X `.set` and Lua catalog merging
- catalog cache round trip
- Code:X Workshop discovery
- exact faction and research fields in generated `status`
- campaign.scn Human, Entity, Inventory, and CampaignSquads graph generation
- campaign.sav archive round trip
- post-battle win and survivor import
- persistent service export and import with manifest validation
- starter roster replacement for all four Code:X factions
- command-line parser coverage

The package entry point was installed and `gates-of-codex --help` was executed successfully in the implementation environment using the locally installed build backend.

## GitHub Actions result

The first repository workflow run created four matrix jobs, but every job terminated before checkout or any other step. GitHub returned no job steps and no downloadable job-log blob. Therefore no source, test, or packaging failure was observed from that run.

This is consistent with a runner or account-level refusal. The workflow remains committed and can be re-run when Actions execution is available.

## External actions still required

### Repository rename

The connected GitHub App does not expose the repository-settings rename endpoint. Issue #88 records the exact admin action to rename the repository from `CodeX-AI-Overhaul-Submod` to `Gates-of-CodeX`.

### Live game acceptance

This environment does not contain a Windows Gates of Hell installation and cannot launch Code:X. Issue #89 contains the required acceptance sequence.

The live test must verify:

- the generated save is accepted by the current GoH engine
- a valid Code:X map identifier is used
- infantry receive the intended default weapons and inventory
- vehicle entities and crews initialize correctly
- attacker and defender stages spawn and are controlled correctly
- the battle completes through Code:X's existing tactical scripts
- the updated save preserves survivors accurately

These are engine-level checks and cannot be truthfully claimed from synthetic tests alone.

## Repository safety decisions

Gates of CodeX does not overwrite Code:X's `conquest.lua`, `utility.lua`, doctrine lists, purchasing AI, wave timing, capture behavior, or mission scripts. Code:X remains the tactical authority.

The project is isolated under `campaign_app/` with documentation under `docs/gates-of-codex/`. Existing AI overhaul content remains intact.

## Final status

The clean-room campaign application, Code:X data adapter, save bridge, result importer, CLI, desktop map, installer, tests, packaging workflow, and documentation are implemented and merged.

The only incomplete actions require capabilities unavailable to the implementation environment:

1. repository administrator rename
2. live execution against the installed Windows game and Code:X
3. GitHub Actions runner availability for hosted CI and Windows artifact production
