# E2 Airmobile Flight and Paradrop Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add default-off E2 live probes that prove a real helicopter flight/insertion and a real Code:X paradrop for RUSA, UKR, and NATO without changing production wave budgets.

**Architecture:** Extend the existing repo-owned attack-support MI engine and its parked faction pool. Both probes use MOVE-placement, literal support-player ownership, integer lifecycle telemetry, bounded cleanup, and the existing active-flag/air-pad infrastructure; the paradrop delegates only passenger ejection to the verified singular `drop_paratrooper` effect and deterministically excludes E2 passengers from CE routing.

**Tech Stack:** Gates of Hell `.mi`/`.inc` mission scripting, Lua BotApi telemetry, PowerShell deployment/validation, Python `unittest`/`pytest` static contract tests.

## Global Constraints

- Obey `docs/plans/2026-07-30-allied-support-expansion.md` MARCHING ORDERS.
- Work repo-first; `tools/deploy_attack_support_probe.ps1` is the sole writer to workshop item `3636883799`.
- `support_e2_test$` ships as integer `0`; `1` forces one helicopter probe and `2` forces one paradrop probe.
- Use MOVE placement only. Never add `{clone}`.
- Use bare selectors for conditions and the existing advanced/group selector form only for consuming tagged prototypes.
- Transfer ownership with literal `{player "1"}` through `{player "16"}` cases and a fail-closed default that performs no player action.
- Place independent infantry one unit at a time with at least `0.5` seconds between placements.
- Gate player announcements with `support_announce$` and developer timers with `support_debug$`.
- Keep `resource/map/multi/ce/ai_logic/ce_ai_logic_triggers.inc` byte-identical to `resource/map_scripts/ai_logic/ce_ai_logic_triggers.inc`.
- Use entity IDs `0xb401`-`0xb42f`, MIDs `9800`-`9846`, and parking row `y=-36800`; the collision sweep found the larger reserved bands `0xb401`-`0xb460` and `9800`-`9895` free across all 14 managed maps and existing template includes. Do not use colliding ID `0xb400`.
- Helicopter templates must contain `{Chassis "helicopter" {Airborne} {EngineStarted} {Altitude 22}}` before they are ever MOVE-placed.
- E2 uses `{effect drop_paratrooper}` singular. It must not invoke CE's plural `drop_paratroopers` effect.
- E2 para passengers carry `support_e2_para_pax` from park time and CE excludes that tag before its 10-second order loop can select them.
- `uh-60m_blackhawk_mg` and `c130_para` require the already-installed West-81 runtime dependency (workshop item `2897299509` as recorded in the approved spec); never substitute an unverified asset.
- PRC remains fail-closed for both probes.
- A failed paradrop never produces teleported fallback troops.
- Forced E2 probes do not decrement `attack_support_waves_left$`, `attack_support_air_left$`, or any existing E1/IFV/motor budget.
- Use strict red-green TDD and commit/push after each accepted task.

---

## File Map

- `tests/test_e2_airmobile.py`: focused static contracts for pool inventory, gates, lifecycle grammar, CE isolation, cleanup, and deployment.
- `resource/map/multi/faction_support_templates.inc`: 47 parked E2 prototypes, crew/passenger links, and immutable pool tags.
- `resource/map/multi/dcg_vars.inc`: five integer E2 variables.
- `resource/script/multiplayer/modes/attack_support.lua`: pcall-guarded E2 lifecycle mirror fields.
- `resource/map/multi/attack_support_waves.inc`: default-zero initialization, ownership/target helpers, helicopter lifecycle, paradrop lifecycle, and failure codes.
- `resource/map/multi/ce/ai_logic/ce_ai_logic_triggers.inc` and `resource/map_scripts/ai_logic/ce_ai_logic_triggers.inc`: one deterministic para-passenger exclusion, byte-identical.
- `localizations/default/interface/text/mission/multi/support_events.pot`: helicopter/para inbound and failure radio keys.
- `tools/deploy_attack_support_probe.ps1`: source and workshop assertions, both CE mirrors in the copy manifest, and updated template depth.

## Fixed Interfaces and Codes

The tasks below must use these names exactly:

| Item | Exact contract |
|---|---|
| Test gate | `support_e2_test$`: `0=off`, `1=helicopter`, `2=paradrop` |
| State | `support_e2_stage$`: `0=idle`, `10=dispatch reserved`, `20=package claimed/target selected`, `30=inbound`, `40=insert/release`, `50=survivors ordered`, `60=aircraft exiting`, `70=cleaned` |
| Failure | `support_e2_fail$`: `0=none`, `1=unsupported faction`, `2=pool short`, `3=no active flag`, `4=no safe LZ`, `5=arrival timeout`, `6=release timeout`, `7=no landed survivor`, `8=ownership unresolved` |
| LZ | `support_e2_lz$`: `0=none`, `1=air pad 1`, `2=air pad 2` |
| Flag | `support_e2_flag$`: `0=none`, `1=one portable active target selected` |
| Target tags | `support_e2_flag_target`, `support_e2_aircraft`, `support_e2_helo`, `support_e2_plane`, `support_e2_team`, `support_e2_para_pax`, `support_e2_released`, `support_e2_landed` |
| Defines | `e2_reset_target`, `e2_choose_flag`, `e2_own_current`, `e2_place_one`, `e2_order_team`, `e2_delete_aircraft`, `e2_fail_and_cleanup` |
| Triggers | `attack_support/e2_dispatch`, `attack_support/e2_helo_rusa`, `attack_support/e2_helo_ukr`, `attack_support/e2_helo_nato`, `attack_support/e2_para_rusa`, `attack_support/e2_para_ukr`, `attack_support/e2_para_nato`, `attack_support/e2_para_landed` |

### Task 1: Park the E2 Pools and Wire Default-Off State

**Agent:** fresh `gpt-5.6-terra`, low reasoning. This is mechanical serialization and guard work only.

**Files:**
- Create: `tests/test_e2_airmobile.py`
- Modify: `resource/map/multi/faction_support_templates.inc`
- Modify: `resource/map/multi/dcg_vars.inc`
- Modify: `resource/script/multiplayer/modes/attack_support.lua`
- Modify: `resource/map/multi/attack_support_waves.inc`
- Modify: `localizations/default/interface/text/mission/multi/support_events.pot`
- Modify: `tools/deploy_attack_support_probe.ps1`

**Interfaces:**
- Consumes: existing `ally_sup_tpl` parked-pool convention, `readVar(name)` Lua helper, `$files` append-only deployment manifest.
- Produces: exact pool tags and five variables used by Tasks 2-4; no lifecycle trigger may run yet.

- [ ] **Step 1: Create the failing pool/state tests**

Create `tests/test_e2_airmobile.py` with these imports/helpers and contracts:

```python
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TEMPLATES = ROOT / "resource/map/multi/faction_support_templates.inc"
VARS = ROOT / "resource/map/multi/dcg_vars.inc"
WAVES = ROOT / "resource/map/multi/attack_support_waves.inc"
LUA = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
POT = ROOT / "localizations/default/interface/text/mission/multi/support_events.pot"
DEPLOY = ROOT / "tools/deploy_attack_support_probe.ps1"
CE_MAP = ROOT / "resource/map/multi/ce/ai_logic/ce_ai_logic_triggers.inc"
CE_SCRIPT = ROOT / "resource/map_scripts/ai_logic/ce_ai_logic_triggers.inc"


def block(text: str, start: str, end: str) -> str:
    return text.split(start, 1)[1].split(end, 1)[0]


class E2PoolAndStateTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.tpl = TEMPLATES.read_text(encoding="utf-8")
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.lua = LUA.read_text(encoding="utf-8")
        cls.pot = POT.read_text(encoding="utf-8")
        cls.deploy = DEPLOY.read_text(encoding="utf-8")

    def test_exact_e2_id_mid_and_parking_bands(self) -> None:
        ids = re.findall(r'\{(?:Entity|Human) "[^"]+" (0xb4[0-9a-f]{2})', self.tpl)
        mids = [int(v) for v in re.findall(r"\{MID (98\d\d)\}", self.tpl)]
        self.assertEqual(ids, [f"0x{n:x}" for n in range(0xB401, 0xB430)])
        self.assertEqual(mids, list(range(9800, 9847)))
        self.assertEqual(self.tpl.count(" -36800}"), 47)
        self.assertNotIn("0xb400", self.tpl)

    def test_exact_aircraft_and_crews(self) -> None:
        for asset in ("mi17_b8_rus", "mi17_b8_ukr", "uh-60m_blackhawk_mg",
                      "il-76td_para", "c130_para"):
            self.assertIn(f'{{Entity "{asset}"', self.tpl)
        for breed in ("mp/rusa/2022s/rus_pliot", "mp/ukr/2022s/ukr_pilot",
                      "mp/nato/2022s/nato_pilot"):
            self.assertIn(f'{{Human "{breed}"', self.tpl)
        self.assertEqual(
            self.tpl.count('{Chassis "helicopter"\n\t\t\t{Airborne}\n\t\t\t{EngineStarted}\n\t\t\t{Altitude 22}\n\t\t}'), 3)

    def test_payloads_and_ejectable_links_are_pinned(self) -> None:
        for breed in ("106vdv_squadlead", "106vdv_mg", "106vdv_rifleman",
                      "ukr13_squadlead", "ukr13_lmg", "ukr13_rifleman",
                      "82nd_squadlead", "82nd_mg", "82nd_rifleman"):
            self.assertIn(breed, self.tpl)
        for place in ("seat01", "seat02", "seat03", "seat04",
                      "seat03", "seat04", "seat05"):
            self.assertIn(f'"{place}"', self.tpl)
        self.assertNotIn('"seat00"', self.tpl)
        for n in range(21, 49):
            self.assertNotRegex(self.tpl, rf'"seat0?{n}"')
        self.assertEqual(self.tpl.count('"support_e2_para_pax"'), 12)

    def test_default_off_integer_state_and_lua_mirror(self) -> None:
        for name in ("support_e2_test", "support_e2_stage", "support_e2_fail",
                     "support_e2_lz", "support_e2_flag"):
            self.assertIn(f'{{"{name}"}}', self.vars)
            self.assertIn(f'{{var "{name}$"}} {{op "="}} {{value 0}}', self.waves)
            self.assertIn(f'readVar("{name}")', self.lua)
        init = block(self.waves, '{"attack_support/init"', '{"attack_support/clock"')
        self.assertNotIn('{var "support_e2_test$"} {op "="} {value 1}', init)
        self.assertNotIn('{var "support_e2_test$"} {op "="} {value 2}', init)

    def test_dependency_localization_and_deploy_guards(self) -> None:
        self.assertIn("West-81", self.tpl)
        for key in ("e2_helo_inbound", "e2_para_inbound", "e2_insert_failed"):
            self.assertIn(f'msgctxt "mission/multi/support/{key}"', self.pot)
        for marker in ("must park 502 prototypes", "support_e2_test",
                       "support_e2_para_pax", "ce_ai_logic_triggers.inc"):
            self.assertIn(marker, self.deploy)
```

- [ ] **Step 2: Run the new tests and observe the red state**

Run: `python -m pytest tests/test_e2_airmobile.py::E2PoolAndStateTests -q`

Expected: FAIL because IDs `0xb401`-`0xb42f`, E2 vars, mirror fields, strings, and deploy guards do not exist.

- [ ] **Step 3: Append exactly 47 parked prototypes and their tags/links**

In `resource/map/multi/faction_support_templates.inc`, insert entities before `; ===== TAGS =====`, links immediately before the same marker, and tag records at the end of the tag list. Serialize every entity with `{Player 0}`, its listed MID, `{Able "-select"}`, and a unique x coordinate beginning at `-9000` and increasing by `5` while y remains `-36800`.

Use this exact inventory; the four helicopter-team humans are independent (no link), while pilots and para passengers use exactly the links shown:

| IDs / MIDs | Package | Exact members and links |
|---|---|---|
| `0xb401`-`0xb407` / `9800`-`9806` | RUSA helo | `mi17_b8_rus`; 2x `rus_pliot` linked `driver`,`commander`; `106vdv_squadlead`,`106vdv_mg`,2x `106vdv_rifleman` independent |
| `0xb408`-`0xb40e` / `9807`-`9813` | UKR helo | `mi17_b8_ukr`; 2x `ukr_pilot` linked `driver`,`commander`; `ukr13_squadlead`,`ukr13_lmg`,2x `ukr13_rifleman` independent |
| `0xb40f`-`0xb415` / `9814`-`9820` | NATO helo | `mi17_b8_rus` (**superseded 2026-07-30**: was `uh-60m_blackhawk_mg`, now blocked pending a parked-actor instantiation proof — fail code 13 decides it; see the E2 airframe findings in `docs/plans/2026-07-30-allied-support-expansion.md`); 2x `nato_pilot` linked `driver`,`commander`; `82nd_squadlead`,`82nd_mg`,2x `82nd_rifleman` independent |
| `0xb416`-`0xb41f` / `9821`-`9830` | RUSA para | `il-76td_para`; 5x `rus_pliot` linked `driver`,`driver1`,`driver2`,`commander`,`commander1`; payload linked `seat01`-`seat04` |
| `0xb420`-`0xb427` / `9831`-`9838` | UKR para | `c130_para`; 3x `ukr_pilot` linked `driver`,`driver2`,`commander`; payload linked `seat02`-`seat05` |
| `0xb428`-`0xb42f` / `9839`-`9846` | NATO para | `c130_para`; 3x `nato_pilot` linked `driver`,`driver2`,`commander`; payload linked `seat02`-`seat05` |

Each helicopter entity must include this exact state inside the entity record:

```text
		{Chassis "helicopter"
			{Airborne}
			{EngineStarted}
			{Altitude 22}
		}
```

Use exact immutable tags:

```text
helo hull:  "ally_sup_tpl" "support_e2_tpl" "support_e2_<faction>_helo" "support_e2_aircraft" "hidden"
helo pilots:"ally_sup_tpl" "support_e2_tpl" "support_e2_<faction>_helo_crew" "hidden"
helo team:  "ally_sup_tpl" "support_e2_tpl" "support_e2_<faction>_helo_team" "hidden"
para hull:  "ally_sup_tpl" "support_e2_tpl" "support_e2_<faction>_para" "support_e2_aircraft" "hidden"
para pilots:"ally_sup_tpl" "support_e2_tpl" "support_e2_<faction>_para_crew" "hidden"
para pax:   "ally_sup_tpl" "support_e2_tpl" "support_e2_<faction>_para_pax" "support_e2_para_pax" "hidden"
```

- [ ] **Step 4: Add default-zero variables, mirror fields, and localization**

Append these declarations to `resource/map/multi/dcg_vars.inc`:

```text
			{"support_e2_test"}
			{"support_e2_stage"}
			{"support_e2_fail"}
			{"support_e2_lz"}
			{"support_e2_flag"}
```

In `attack_support/init`, initialize all five to `0`; do not write the test gate anywhere else. Extend the existing attack-support mirror emission with:

```lua
		"e2_test", readVar("support_e2_test"),
		"e2_stage", readVar("support_e2_stage"),
		"e2_fail", readVar("support_e2_fail"),
		"e2_lz", readVar("support_e2_lz"),
		"e2_flag", readVar("support_e2_flag"))
```

Add these POT entries with English text:

```po
msgctxt "mission/multi/support/e2_helo_inbound"
msgid "Airmobile flight inbound."
msgstr ""

msgctxt "mission/multi/support/e2_para_inbound"
msgid "Airborne drop inbound."
msgstr ""

msgctxt "mission/multi/support/e2_insert_failed"
msgid "Air insertion aborted."
msgstr ""
```

- [ ] **Step 5: Extend source/deployed guards without weakening existing checks**

In `tools/deploy_attack_support_probe.ps1`:

1. Append both CE mirror paths to `$files`; bind them as `$ceMapSource = Join-Path $RepoRoot $files[17]` and `$ceScriptSource = Join-Path $RepoRoot $files[18]`; include both in the source existence loop.
2. Change only the faction-pool expected count from `455` to `502` and its error strings to `must park 502 prototypes`; preserve all existing per-tag depths.
3. Require the five E2 declarations in both source and workshop var-marker arrays.
4. Add E2 assets/tags/chassis state to source and workshop faction-template marker arrays.
5. Add the three localization keys to the source/workshop POT marker arrays.
6. Add `support_e2_test` through `support_e2_flag` to `$MirrorMarkers`.
7. Compare the two source CE files byte-for-byte before copy and the two workshop targets byte-for-byte after copy:

```powershell
if ([System.IO.File]::ReadAllBytes($ceMapSource) -cne [System.IO.File]::ReadAllBytes($ceScriptSource)) {
    throw "Source CE ai_logic mirrors are not byte-identical"
}
```

Use `Get-FileHash -Algorithm SHA256` for the actual comparison because PowerShell array `-cne` is element-wise:

```powershell
$ceSourceHashes = @($ceMapSource, $ceScriptSource) | ForEach-Object { (Get-FileHash -LiteralPath $_ -Algorithm SHA256).Hash }
if ($ceSourceHashes[0] -ne $ceSourceHashes[1]) { throw "Source CE ai_logic mirrors are not byte-identical" }
```

- [ ] **Step 6: Run focused and existing tests**

Run: `python -m pytest tests/test_e2_airmobile.py::E2PoolAndStateTests tests/test_attack_support_slot_proof.py -q`

Expected: PASS. If an old test still expects `455`, update that assertion/error-marker expectation to `502`; do not reduce any tag-depth assertion.

- [ ] **Step 7: Commit and push**

```powershell
git add tests/test_e2_airmobile.py resource/map/multi/faction_support_templates.inc resource/map/multi/dcg_vars.inc resource/script/multiplayer/modes/attack_support.lua resource/map/multi/attack_support_waves.inc localizations/default/interface/text/mission/multi/support_events.pot tools/deploy_attack_support_probe.ps1
git commit -m "feat: park default-off E2 air packages"
git push origin experiment/attack-mate-slot-proof
```

### Task 2: Deterministically Exclude E2 Passengers from CE Orders

**Agent:** fresh `gpt-5.6-terra`, low reasoning. This task makes one exact mirror edit.

**Files:**
- Modify: `tests/test_e2_airmobile.py`
- Modify: `resource/map/multi/ce/ai_logic/ce_ai_logic_triggers.inc`
- Modify: `resource/map_scripts/ai_logic/ce_ai_logic_triggers.inc`

**Interfaces:**
- Consumes: permanent park-time tag `support_e2_para_pax` from Task 1.
- Produces: deterministic CE isolation before any E2 paradrop trigger exists.

- [ ] **Step 1: Add failing CE isolation tests**

Append:

```python
class E2CeIsolationTests(unittest.TestCase):
    def test_ce_mirrors_are_byte_identical(self) -> None:
        self.assertEqual(CE_MAP.read_bytes(), CE_SCRIPT.read_bytes())

    def test_paratrooper_order_selector_excludes_e2_at_selection_time(self) -> None:
        text = CE_MAP.read_text(encoding="utf-8")
        order_block = block(text, '{"ai_logic/paratrooper_orders"', '{"ai_logic/')
        selector = block(order_block, '{selector', '{sort')
        exclude = selector.split('{exclude', 1)[1]
        self.assertIn('{tag paratrooper_need_orders}', selector)
        self.assertIn('{tag {tag support_e2_para_pax}}', exclude)
```

- [ ] **Step 2: Run and observe failure**

Run: `python -m pytest tests/test_e2_airmobile.py::E2CeIsolationTests -q`

Expected: FAIL because the exclusion tag is absent.

- [ ] **Step 3: Make the byte-identical selector edit**

Inside the existing `{exclude ...}` group of `ai_logic/paratrooper_orders`, after `{state {state user_control}}`, add exactly:

```text
										{tag
											{tag support_e2_para_pax}
										}
```

Apply the same textual patch to both mirror files. Do not touch the trigger delay, `paratrooper_need_orders`, or waypoint routing.

- [ ] **Step 4: Verify isolation and mirror identity**

Run: `python -m pytest tests/test_e2_airmobile.py::E2CeIsolationTests -q`

Run: `git diff --no-index -- resource/map/multi/ce/ai_logic/ce_ai_logic_triggers.inc resource/map_scripts/ai_logic/ce_ai_logic_triggers.inc`

Expected: tests PASS; diff command exits `0` with no output.

- [ ] **Step 5: Commit and push**

```powershell
git add tests/test_e2_airmobile.py resource/map/multi/ce/ai_logic/ce_ai_logic_triggers.inc resource/map_scripts/ai_logic/ce_ai_logic_triggers.inc
git commit -m "fix: isolate E2 para troops from CE orders"
git push origin experiment/attack-mate-slot-proof
```

### Task 3: Implement the Helicopter Flight-Proof Lifecycle

**Agent:** fresh `gpt-5.6-sol`, high reasoning. Do not assign this lifecycle task to the cheap implementer.

**Files:**
- Modify: `tests/test_e2_airmobile.py`
- Modify: `resource/map/multi/attack_support_waves.inc`
- Modify: `tools/deploy_attack_support_probe.ps1`

**Interfaces:**
- Consumes: Task 1 helo tags, fixed stage/failure codes, existing `attack_support_entry_<side>1` and `attack_support_air_<side>1/2` waypoints.
- Produces: one default-off flight probe for each supported faction and reusable ownership/target/cleanup defines for Task 4.

- [ ] **Step 1: Add failing helicopter lifecycle contracts**

Append a test class that extracts from `; ===== E2 REAL AIR INSERT PROBES =====` to `; ===== MOTORIZED INSERT` and asserts all of the following exact strings/counts:

```python
class E2HelicopterLifecycleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")

    def test_dispatch_is_strictly_default_off_and_budget_neutral(self) -> None:
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== MOTORIZED INSERT")
        self.assertIn('{var "support_e2_test$"} {op "=="} {value 1}', e2)
        self.assertIn('{var "support_e2_test$"} {op "=="} {value 2}', e2)
        for budget in ("attack_support_waves_left$", "attack_support_air_left$",
                       "attack_support_motor_left$", "attack_support_ifv_left$"):
            self.assertNotIn(f'{{var "{budget}"}} {{op "-"}}', e2)

    def test_helicopter_uses_attested_flight_sequence_and_existing_pads(self) -> None:
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== E2 PARADROP")
        self.assertEqual(e2.count('{"air_state"'), 3)
        self.assertEqual(e2.count('{altitude 30}'), 3)
        self.assertEqual(e2.count('{control AI}'), 3)
        self.assertGreaterEqual(e2.count('{action move}'), 6)
        for side in "ab":
            self.assertIn(f'{{waypoint "attack_support_entry_{side}1"}}', e2)
            for n in (1, 2):
                self.assertIn(f'{{waypoint "attack_support_air_{side}{n}"}}', e2)
        self.assertNotRegex(e2, r"support_e2_lz_fpc|e2_lz_fpc")

    def test_helicopter_places_four_independent_troops_at_half_second_cadence(self) -> None:
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== E2 PARADROP")
        self.assertIn('(define "e2_place_one"', e2)
        self.assertIn('{"delay" {time 0.5}}', e2)
        self.assertGreaterEqual(e2.count('(e2_place_one)'), 12)
        self.assertIn('{action advance}', e2)
        self.assertIn('{tag support_e2_flag_target}', e2)

    def test_helicopter_has_fail_closed_faction_and_bounded_delete(self) -> None:
        e2 = block(self.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== E2 PARADROP")
        for faction in ("rusa", "ukr", "nato"):
            self.assertIn(f'{{"attack_support/e2_helo_{faction}"', e2)
        self.assertNotIn('attack_support/e2_helo_prc', e2)
        self.assertIn('{value 1}', e2)  # unsupported faction fail code
        self.assertIn('(define "e2_delete_aircraft"', e2)
        self.assertIn('{"delete"', e2)
        self.assertRegex(e2, r'\{"delay" \{time (?:45|60|75|90)\}\}')

    def test_ownership_switch_lists_1_through_16_and_default_has_no_player(self) -> None:
        e2 = block(self.waves, '(define "e2_own_current"', '(define "e2_place_one"')
        for player in range(1, 17):
            self.assertIn(f'{{player "{player}"}}', e2)
        default = e2.split('{"default"', 1)[1]
        self.assertNotIn('{player "', default)
```

- [ ] **Step 2: Run the helicopter contracts red**

Run: `python -m pytest tests/test_e2_airmobile.py::E2HelicopterLifecycleTests -q`

Expected: FAIL at the absent E2 section/defines/triggers.

- [ ] **Step 3: Add common E2 helpers and dispatch**

Insert a new E2 section immediately before the current motorized section. Implement:

```text
(define "e2_reset_target")
    remove support_e2_flag_target from any previous entity; set flag/lz to 0
(define "e2_choose_flag")
    choose one operatable, non-inactive entity tagged flag; add support_e2_flag_target;
    set support_e2_flag$ to 1 as the portable active-target sentinel;
    if none exists set fail=3; the caller invokes the single cleanup path
(define "e2_own_current")
    switch on id_attack_support$ with literal cases 1..16, each applying entity_state
    {player "N"} to selector {tag support_e2_claim}; default sets fail=8 and does no ownership
(define "e2_place_one")
    consume amount 1 from support_e2_team into support_e2_place_one;
    place it on the currently selected air pad; promote/remove hidden/template tags;
    delay 0.5; restore tag support_e2_team and clear support_e2_place_one
(define "e2_order_team")
    action advance for support_e2_team targeting support_e2_flag_target
(define "e2_delete_aircraft")
    {"delete" {selector {ignore_captured_by_user 0} {tag support_e2_aircraft}}}
(define "e2_fail_and_cleanup")
    set stage=70; delete any claimed aircraft; clear claim/team/flag temporary tags;
    announce e2_insert_failed only through support_announce$
```

`attack_support/e2_dispatch` must require all mission-side safety gates and branch only on values 1 and 2:

```text
{var "user_is_defender$"} == 0
{var "attack_support_ready$"} == 1
{var "attack_support_use_mi$"} == 1
{var "id_attack_support$"} > 0
{var "support_e2_stage$"} == 0
{var "support_e2_test$"} > 0
```

Map faction values exactly: `1=RUSA`, `2=UKR`, `3=NATO`; default/`4=PRC` sets fail `1`, stage `70`, and stops. Dispatch must not set `attack_support_wave_cmd$` and must not decrement any production budget.

- [ ] **Step 4: Implement three helicopter triggers with the attested sequence**

Each faction trigger must:

1. Claim one hull plus its linked pilots by changing the hull package tag to `support_e2_claim`, `support_e2_aircraft`, `support_e2_helo`; claim exactly four independent team humans as `support_e2_claim` + `support_e2_team`.
2. Require the dispatch-reserved stage `10`, claim the package, call `e2_choose_flag`, advance to stage `20`, then call `e2_own_current`.
3. Choose side exactly as E1: `enemy_spawnside$ == 1` uses `_b`; `== 2` uses `_a`; default `_b`.
4. Choose pad 1 or 2 with the existing enemy-within-120 guard, recording `support_e2_lz$`; if both are unsafe, set fail `4` and use the documented infantry-only standoff fallback while deleting the unused helicopter.
5. MOVE-place the hull at `attack_support_entry_<side>1`, then run this exact order:

```text
{"air_state" {selector {ignore_captured_by_user 0} {tag support_e2_helo}} {altitude 30}}
{"actor_state" {selector {ignore_captured_by_user 0} {tag support_e2_helo}} {drop sensor} {control AI} {movement {speed fast}}}
{"action"
    {selector {ignore_captured_by_user 0} {tag support_e2_helo}}
    {drop orders}
    {action move}
    {waypoint "attack_support_air_<resolved-side><resolved-pad>"}
}
```

6. Set stage `30`, delay `40` seconds as the time-based primary arrival window, and record fail `5` if the aircraft is no longer operatable.
7. Under `support_announce$ == 1`, talk `mission/multi/support/e2_helo_inbound`; set stage `40`; call `e2_place_one` four times.
8. Call `e2_order_team`, set stage `50`, order the helicopter back to `attack_support_entry_<side>1`, set stage `60`, delay at most `60`, delete the helo, and set stage `70`.
9. Clear only temporary claim/aircraft/helo/target tags. Do not remove the permanent roster tag from deployed infantry.

- [ ] **Step 5: Add source/workshop lifecycle markers to deploy validation**

Require the E2 section header, dispatch, all three helo triggers, `air_state`, `Altitude 22`, `support_e2_lz`, and `{"delete"` in both source and workshop checks. Explicitly reject `attack_support/e2_helo_prc`, `{clone}`, and any `support_e2_lz_fpc` marker.

- [ ] **Step 6: Run focused tests and regression suite**

Run: `python -m pytest tests/test_e2_airmobile.py::E2HelicopterLifecycleTests tests/test_attack_support_slot_proof.py -q`

Expected: PASS with no existing E1, IFV, motor, flank, or budget test changed except the pool total already raised in Task 1.

- [ ] **Step 7: Commit and push**

```powershell
git add tests/test_e2_airmobile.py resource/map/multi/attack_support_waves.inc tools/deploy_attack_support_probe.ps1
git commit -m "feat: add E2 helicopter flight probe"
git push origin experiment/attack-mate-slot-proof
```

### Task 4: Implement the Real Paradrop Lifecycle

**Agent:** fresh `gpt-5.6-sol`, high reasoning. Do not assign this lifecycle task to the cheap implementer.

**Files:**
- Modify: `tests/test_e2_airmobile.py`
- Modify: `resource/map/multi/attack_support_waves.inc`
- Modify: `tools/deploy_attack_support_probe.ps1`

**Interfaces:**
- Consumes: Task 1 plane packages and park-time para tags, Task 2 CE exclusion, Task 3 common helpers/dispatch/target tag.
- Produces: one-shot real para release, E2 survivor routing, and honest timeout cleanup.

- [ ] **Step 1: Add failing paradrop lifecycle contracts**

Append:

```python
class E2ParadropLifecycleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = WAVES.read_text(encoding="utf-8")

    def test_supported_matrix_and_exact_effect(self) -> None:
        e2 = block(self.waves, "; ===== E2 PARADROP", "; ===== MOTORIZED INSERT")
        for faction in ("rusa", "ukr", "nato"):
            self.assertIn(f'{{"attack_support/e2_para_{faction}"', e2)
        self.assertNotIn('attack_support/e2_para_prc', e2)
        self.assertEqual(e2.count('{effect drop_paratrooper}'), 3)
        self.assertNotIn('{effect drop_paratroopers}', e2)

    def test_release_is_near_target_one_shot_and_has_no_fake_fallback(self) -> None:
        e2 = block(self.waves, "; ===== E2 PARADROP", "; ===== MOTORIZED INSERT")
        self.assertIn('{distance 2500}', e2)
        self.assertIn('{distance 1500}', e2)
        self.assertIn('{tag support_e2_released}', e2)
        self.assertIn('{var "support_e2_fail$"} {op "="} {value 6}', e2)
        self.assertNotIn('(e2_place_one)', e2)
        self.assertNotIn('{"placement"\n\t\t\t\t\t{selector {tag support_e2_para_pax}', e2)

    def test_survivors_leave_ce_tag_and_advance_on_e2_flag(self) -> None:
        e2 = block(self.waves, "; ===== E2 PARADROP", "; ===== MOTORIZED INSERT")
        self.assertIn('{"attack_support/e2_para_landed"', e2)
        self.assertIn('{tag_remove paratrooper_need_orders}', e2)
        self.assertIn('{tag support_e2_para_pax}', e2)
        self.assertIn('{action advance}', e2)
        self.assertIn('{tag support_e2_flag_target}', e2)
        for wp in (5004, 5005, 5006):
            self.assertNotIn(f'waypoint "{wp}"', e2)

    def test_plane_always_has_bounded_delete_and_no_survivor_is_failure(self) -> None:
        e2 = block(self.waves, "; ===== E2 PARADROP", "; ===== MOTORIZED INSERT")
        self.assertIn('{var "support_e2_fail$"} {op "="} {value 7}', e2)
        self.assertIn('(e2_delete_aircraft)', e2)
        self.assertRegex(e2, r'\{"delay" \{time (?:60|75|90|120)\}\}')
```

- [ ] **Step 2: Run the paradrop contracts red**

Run: `python -m pytest tests/test_e2_airmobile.py::E2ParadropLifecycleTests -q`

Expected: FAIL because the paradrop section does not exist.

- [ ] **Step 3: Implement the three faction plane launches**

For each supported faction trigger:

1. Require `support_e2_stage$ == 10`: the shared dispatcher atomically reserves stage `10` before explicitly firing exactly one faction child. The para child must not also be eligible at stage `0`.
2. Claim exactly one plane package; retain `support_e2_para_pax` on all passengers while replacing pool-specific tags with `support_e2_claim`, `support_e2_aircraft`, and `support_e2_plane`.
3. Call `e2_choose_flag`; only after a target is selected, advance stage `10 -> 20` and call the literal ownership helper. Ownership transfer of the linked hull transfers its crew/pax; nevertheless the claim selector must include hull and linked people so cleanup/failure is complete. Advancing to `20` also proves to the dispatcher's one-second check that the reserved child successfully claimed its package.
4. Place at `attack_support_entry_<side>1` with the same side resolution as Task 3.
5. Apply, in order, `air_state` altitude `65`, `actor_state {drop sensor} {control AI} {movement {speed fast}}`, then `action move` targeting `{tag support_e2_flag_target}`. Set stage `30`.
6. Announce `mission/multi/support/e2_para_inbound` only behind `support_announce$`.

- [ ] **Step 4: Implement the one-shot release and exit**

The release condition must select only an operatable `support_e2_plane`, require it to be inside `2500` units of `support_e2_flag_target`, outside `1500` units, and exclude `support_e2_released`. On success:

```text
{"entity_state" {selector {tag support_e2_plane}} {tag_add support_e2_released}}
{"effect" {selector {tag support_e2_plane}} {effect drop_paratrooper}}
{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}
{"action" {selector {tag support_e2_plane}} {drop orders} {action move}
    {waypoint "attack_support_entry_<resolved-side>1"}}
```

Use the repo-attested effect serialization if it is `{effect drop_paratrooper}` directly rather than nested under `{"effect"...}`; the static contract pins the singular token, while the implementation must copy the exact grammar from `resource/map/multi/bakhmut_1/campaign_capture_the_flag.mi`. Do not set stage `60` at release: the monotonic success path is `30 -> 40 -> 50 -> 60 -> 70`, where landing proves `50`, settlement advances to `60`, and cleanup records `70`. A parallel bounded timer of at most `90` seconds sets fail `6`, orders exit, and deletes the plane when `support_e2_released` was never applied. The successful path deletes the plane after at most `90` seconds while preserving the target and passenger claim through a final `29`-second survivor window (a `119`-second total deadline, within the required `120`-second bound).

- [ ] **Step 5: Implement E2 landing detection and honest no-survivor failure**

`attack_support/e2_para_landed` must select operatable, unlinked humans carrying both `support_e2_para_pax` and `paratrooper_need_orders`. For those survivors:

```text
{"entity_state"
    {selector {ignore_captured_by_user 0} {tag support_e2_para_pax}
        {tag support_e2_claim} {tag paratrooper_need_orders}
        {type human} {state operatable}}
    {tag_add support_e2_landed_candidate}
}
{"entity_state"
    {selector {source advanced} {group
        {select {tag {tag support_e2_landed_candidate}}}
        {exclude {state {state dead}} {state {state linked}} {state {state inactive}}}
    }}
    {tag_add support_e2_landed}
    {tag_remove support_e2_landed_candidate}
    {tag_remove paratrooper_need_orders}
    {tag_remove ai_spawn}
}
{"action"
    {selector {ignore_captured_by_user 0} {tag support_e2_landed}}
    {drop orders}
    {action advance}
    {target {ignore_captured_by_user 0} {tag support_e2_flag_target}}
}
```

Set stage `50` only when at least one `support_e2_landed` human exists. At a bounded `120`-second landing deadline, if the count is zero, set fail `7`; clean tags and aircraft but do not invoke `e2_place_one`, do not MOVE-place para passengers, and do not clear the fail code.

- [ ] **Step 6: Strengthen deploy guards for exact para behavior**

Require all three para triggers, `attack_support/e2_para_landed`, singular `drop_paratrooper`, the `1500`/`2500` band, `support_e2_released`, and fail codes `6`/`7` on both source and workshop sides. Reject plural `{effect drop_paratroopers}`, E2 references to waypoints `5004`-`5006`, and any para-pax placement fallback.

- [ ] **Step 7: Run focused and full static regression tests**

Run: `python -m pytest tests/test_e2_airmobile.py tests/test_attack_support_slot_proof.py -q`

Expected: PASS. Confirm `E2CeIsolationTests` still passes after the landing trigger is present.

- [ ] **Step 8: Commit and push**

```powershell
git add tests/test_e2_airmobile.py resource/map/multi/attack_support_waves.inc tools/deploy_attack_support_probe.ps1
git commit -m "feat: add E2 real paradrop probe"
git push origin experiment/attack-mate-slot-proof
```

### Task 5: Deploy Idempotently and Produce the Live-Test Handoff

**Agent:** controller performs this verification; use a fresh `gpt-5.6-sol`, high-reasoning reviewer before any completion claim.

**Files:**
- Modify only if a verification defect is found: files already listed in Tasks 1-4.
- Verify: complete repository and deployed workshop item `3636883799`.

**Interfaces:**
- Consumes: all accepted task commits.
- Produces: byte-identical deployed output, clean repo state, exact operator instructions for modes 1 and 2.

- [ ] **Step 1: Run the complete test suite from a clean task boundary**

Run: `python -m pytest tests/ -q`

Expected: PASS with zero failures. Save the reported test count for the final handoff.

- [ ] **Step 2: Run the sole deploy writer once**

Run:

```powershell
powershell.exe -ExecutionPolicy Bypass -File .\tools\deploy_attack_support_probe.ps1 -RepoRoot 'E:\Steam\steamapps\workshop\content\400750\CodeX AI Overhaul Submod'
```

Expected: exit `0`; source/deployed guard summary includes the 502-prototype pool and CE mirror checks.

- [ ] **Step 3: Snapshot hashes, redeploy, and prove idempotence**

Hash every path in the deploy script's `$files` manifest plus all 14 managed `campaign_capture_the_flag.mi` files using SHA256. Run the deploy command a second time, hash the same paths again, and compare the two sorted `path=hash` sets.

Expected: second deploy exits `0`; comparison has no differences.

- [ ] **Step 4: Re-run tests after deployment and inspect repository state**

Run:

```powershell
python -m pytest tests/ -q
git status --short
git diff --check
git log -5 --oneline
```

Expected: tests PASS; `git diff --check` exits `0`; status is clean unless a documented verification fix was required and committed.

- [ ] **Step 5: Request final cross-file code review**

Reviewer must check:

- every E2 trigger includes the default-off gate directly or is reachable only from a directly gated trigger;
- no production budget decrement appears inside the E2 section;
- helicopter snapshots are airborne before MOVE placement;
- `air_state` precedes `actor_state`, which precedes `action move`;
- para passengers are CE-excluded at selection time, not de-tagged in a race;
- singular effect spelling, one-shot release, no teleported para fallback;
- literal ownership cases 1-16 and fail-closed PRC/default paths;
- exit/timeout deletes exist for both aircraft;
- source/workshop CE mirrors are byte-identical;
- deploy remains sole workshop writer and is idempotent.

- [ ] **Step 6: Provide the operator handoff without claiming unrun live behavior**

The final handoff must state:

```text
support_e2_test$ = 0  shipped/off
support_e2_test$ = 1  one helicopter flight proof; verify stage 10→20→30→40→50→60→70
support_e2_test$ = 2  one real paradrop; verify release at 1500-2500 units and at least one landed survivor
```

Ask for two-map live evidence for mode `1` before proposing exact per-flag helicopter LZ generation. For mode `2`, report `support_e2_fail$=7` as an honest failed drop and do not call the probe successful merely because the plane flew.

