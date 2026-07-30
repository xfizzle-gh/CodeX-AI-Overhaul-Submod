# E2 Sequential Combo Test Mode Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add and deploy a temporary mode `3` that runs the E2 helicopter probe to terminal cleanup, then automatically runs the existing E2 paradrop probe in the same player-attack mission.

**Architecture:** Keep the existing single E2 lifecycle owner and shared stage/failure tags. Mode `3` enters the helicopter branch, a claim-free stage-70 transition preserves the helicopter failure and atomically changes the active mode to `2`, and the existing dispatcher launches the paradrop. The repository remains default-off; the sole deployment writer applies validated, exact replacements only to workshop item `3636883799`.

**Tech Stack:** Gates of Hell `.inc` mission scripting, Lua BotApi telemetry, PowerShell deployment/validation, Python `unittest`/`pytest` static contract tests.

## Global Constraints

- `support_e2_test$`: `0` off, `1` helicopter, `2` paradrop, `3` helicopter then paradrop.
- Committed source must keep `support_e2_test$ = 0`.
- Mode `3` is player-attack only; player-defense routing remains outside this change.
- The helicopter and paradrop must never run concurrently or decrement production wave/aircraft budgets.
- The transition must require mode `3`, stage `70`, and no `support_e2_claim` entity.
- Copy `support_e2_fail$` to `support_e2_combo_helo_fail$` before clearing the active failure; then set mode `2` before stage `0`.
- `tools/deploy_attack_support_probe.ps1` remains the sole writer to workshop item `3636883799`.
- `-E2TestMode` accepts only `0`, `1`, `2`, `3`, defaults to `0`, and disables `attack_support_air_test$` only when nonzero.
- Final deployment must use `-E2TestMode 3`, pass twice idempotently, and be followed by a fresh game restart.

---

## File Structure

- `tests/test_e2_airmobile.py`: pins state declaration, lifecycle ordering, budget neutrality, telemetry, and deployment override contracts.
- `resource/map/multi/dcg_vars.inc`: declares the preserved helicopter result integer.
- `resource/map/multi/attack_support_waves.inc`: initializes the result, admits mode `3` to helicopter children/dispatch, and owns the terminal handoff to mode `2`.
- `resource/script/multiplayer/modes/attack_support.lua`: mirrors the preserved helicopter result into `game.log`.
- `tools/deploy_attack_support_probe.ps1`: validates the mode parameter, applies exact target-only overrides, validates deployed values, and reports the intentional wave hash difference.

### Task 1: Sequential lifecycle and diagnostics

**Files:**
- Modify: `tests/test_e2_airmobile.py`
- Modify: `resource/map/multi/dcg_vars.inc`
- Modify: `resource/map/multi/attack_support_waves.inc`
- Modify: `resource/script/multiplayer/modes/attack_support.lua`

**Interfaces:**
- Consumes: existing `support_e2_test$`, `support_e2_stage$`, `support_e2_fail$`, `support_e2_claim`, faction helicopter child triggers, and faction paradrop child triggers.
- Produces: integer `support_e2_combo_helo_fail$` and trigger `attack_support/e2_combo_transition`.

- [ ] **Step 1: Write failing lifecycle tests**

Add this class to `tests/test_e2_airmobile.py`:

```python
class E2SequentialComboTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.lua = LUA.read_text(encoding="utf-8")
        cls.e2 = block(cls.waves, "; ===== E2 REAL AIR INSERT PROBES =====", "; ===== MOTORIZED INSERT")

    def test_combo_result_is_declared_initialized_and_mirrored(self) -> None:
        name = "support_e2_combo_helo_fail"
        self.assertIn(f'{{"{name}"}}', self.vars)
        self.assertIn(f'{{var "{name}$"}} {{op "="}} {{value 0}}', self.waves)
        self.assertIn(f'readVar("{name}")', self.lua)
        init = block(self.waves, '{"attack_support/init"', '{"attack_support/clock"')
        self.assertIn('{var "support_e2_test$"} {op "="} {value 0}', init)

    def test_mode_three_enters_only_the_helicopter_dispatch_first(self) -> None:
        dispatch = mi_block(self.e2, '{"attack_support/e2_dispatch"')
        mode3 = dispatch.split('{var "support_e2_test$"} {op "=="} {value 3}', 1)[1]
        mode3 = mode3.split('{var "support_e2_test$"} {op "=="} {value 2}', 1)[0]
        self.assertIn('("e2_trigger_helo_by_army")', mode3)
        self.assertNotIn('("e2_trigger_para_by_army")', mode3)

    def test_helicopter_children_accept_exactly_modes_one_and_three(self) -> None:
        for faction in ("rusa", "ukr", "nato"):
            child = mi_block(self.e2, f'{{"attack_support/e2_helo_{faction}"')
            condition = child.split("{actions", 1)[0]
            self.assertIn('{var "support_e2_test$"} {op "=="} {value 1}', condition)
            self.assertIn('{var "support_e2_test$"} {op "=="} {value 3}', condition)
            self.assertNotIn('{var "support_e2_test$"} {op ">"}', condition)

    def test_combo_transition_is_claim_free_and_ordered(self) -> None:
        transition = mi_block(self.e2, '{"attack_support/e2_combo_transition"')
        condition, actions = transition.split("{actions", 1)
        self.assertIn('{var "support_e2_test$"} {op "=="} {value 3}', condition)
        self.assertIn('{var "support_e2_stage$"} {op "=="} {value 70}', condition)
        self.assertIn('{tag support_e2_claim}', condition)
        self.assertIn("!3", condition)
        copy_at = actions.index('{var "support_e2_combo_helo_fail$"} {op "="} {var "support_e2_fail$"}')
        clear_at = actions.index('{var "support_e2_fail$"} {op "="} {value 0}')
        mode2_at = actions.index('{var "support_e2_test$"} {op "="} {value 2}')
        stage0_at = actions.index('{var "support_e2_stage$"} {op "="} {value 0}')
        self.assertLess(copy_at, clear_at)
        self.assertLess(clear_at, mode2_at)
        self.assertLess(mode2_at, stage0_at)

    def test_combo_is_budget_neutral_and_mode_two_cannot_retrigger(self) -> None:
        transition = mi_block(self.e2, '{"attack_support/e2_combo_transition"')
        for budget in ("attack_support_waves_left$", "attack_support_air_left$", "attack_support_motor_left$", "attack_support_ifv_left$"):
            self.assertNotIn(budget, transition)
        self.assertNotIn('{var "support_e2_test$"} {op "=="} {value 2}', transition.split("{actions", 1)[0])
```

- [ ] **Step 2: Run the focused tests and confirm red**

Run:

```powershell
python -m pytest tests/test_e2_airmobile.py::E2SequentialComboTests -q
```

Expected: failures for the missing variable, transition, mode-3 dispatch, and Lua diagnostic.

- [ ] **Step 3: Add state and telemetry**

In `resource/map/multi/dcg_vars.inc`, declare:

```text
{"support_e2_combo_helo_fail"}
```

In the `attack_support/init` actions, directly after initializing `support_e2_fail$`, add:

```text
{"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {value 0}}
```

In `resource/script/multiplayer/modes/attack_support.lua`, extend the E2 emission without changing its label:

```lua
emit("e2", "e2_test", readVar("support_e2_test"),
	"e2_stage", readVar("support_e2_stage"),
	"e2_fail", readVar("support_e2_fail"),
	"e2_combo_helo_fail", readVar("support_e2_combo_helo_fail"),
	"e2_lz", readVar("support_e2_lz"),
	"e2_flag", readVar("support_e2_flag"))
```

- [ ] **Step 4: Admit mode three to the helicopter lifecycle**

For each of `attack_support/e2_helo_rusa`, `attack_support/e2_helo_ukr`, and `attack_support/e2_helo_nato`, change the condition expression to include an OR pair for terms `5` and `11`:

```text
{expression "1 & 2 & 3 & 4 & (5 | 11) & 6 & 7 & 8 & 9 & 10"}
```

Keep term `5` as mode `1` and add:

```text
{"11.cmp_i" {var "support_e2_test$"} {op "=="} {value 3}}
```

Do not widen the gate to `> 0`, because that would admit paradrop mode `2`.

- [ ] **Step 5: Factor faction dispatch and add the combo transition**

Immediately before `attack_support/e2_dispatch`, add two defines using the existing faction mappings:

```text
(define "e2_trigger_helo_by_army"
	{"switch"
		{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 1}} {"trigger" {name "attack_support/e2_helo_rusa"}}}
		{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 2}} {"trigger" {name "attack_support/e2_helo_ukr"}}}
		{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 3}} {"trigger" {name "attack_support/e2_helo_nato"}}}
		{"default" {"set_i" {var "support_e2_fail$"} {op "="} {value 1}} ("e2_fail_and_cleanup")}
	}
)

(define "e2_trigger_para_by_army"
	{"switch"
		{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 1}} {"trigger" {name "attack_support/e2_para_rusa"}}}
		{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 2}} {"trigger" {name "attack_support/e2_para_ukr"}}}
		{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 3}} {"trigger" {name "attack_support/e2_para_nato"}}}
		{"default" {"set_i" {var "support_e2_fail$"} {op "="} {value 1}} ("e2_fail_and_cleanup")}
	}
)
```

Replace the dispatcher’s duplicated faction switches with three mode cases: mode `1` calls `e2_trigger_helo_by_army`, mode `3` calls `e2_trigger_helo_by_army`, and mode `2` calls `e2_trigger_para_by_army`. Keep the existing one-second child-claim timeout unchanged.

Immediately after the dispatcher, add:

```text
{"attack_support/e2_combo_transition"
	{condition
		{expression "1 & 2 & !3"}
		{terms
			{"1.cmp_i" {var "support_e2_test$"} {op "=="} {value 3}}
			{"2.cmp_i" {var "support_e2_stage$"} {op "=="} {value 70}}
			{"3.entities" {selector {tag support_e2_claim}}}
		}
	}
	{actions
		{"set_i" {var "support_e2_combo_helo_fail$"} {op "="} {var "support_e2_fail$"}}
		{"set_i" {var "support_e2_fail$"} {op "="} {value 0}}
		{"set_i" {var "support_e2_test$"} {op "="} {value 2}}
		{"set_i" {var "support_e2_stage$"} {op "="} {value 0}}
	}
}
```

- [ ] **Step 6: Run lifecycle tests and the existing E2 suite**

Run:

```powershell
python -m pytest tests/test_e2_airmobile.py::E2SequentialComboTests -q
python -m pytest tests/test_e2_airmobile.py -q
```

Expected: both commands exit `0`; existing helicopter, paradrop, and CE-isolation tests remain green.

- [ ] **Step 7: Commit lifecycle work**

```powershell
git add tests/test_e2_airmobile.py resource/map/multi/dcg_vars.inc resource/map/multi/attack_support_waves.inc resource/script/multiplayer/modes/attack_support.lua
git commit -m "feat: add E2 sequential combo lifecycle"
```

### Task 2: Validated target-only deployment override

**Files:**
- Modify: `tests/test_e2_airmobile.py`
- Modify: `tools/deploy_attack_support_probe.ps1`

**Interfaces:**
- Consumes: committed source initializers for `support_e2_test$ = 0` and `attack_support_air_test$ = 1`.
- Produces: deploy parameter `[ValidateSet(0, 1, 2, 3)][int]$E2TestMode = 0` and exact deployed-copy override/validation.

- [ ] **Step 1: Write failing deploy-contract tests**

Add these methods to `E2SequentialComboTests`:

```python
    def test_deploy_mode_parameter_is_validated_and_defaults_off(self) -> None:
        self.assertRegex(self.deploy, r"\[ValidateSet\(0,\s*1,\s*2,\s*3\)\]\s*\[int\]\$E2TestMode\s*=\s*0")

    def test_deploy_override_is_exact_target_only_and_validated(self) -> None:
        self.assertIn("Set-ExactSingleReplacement", self.deploy)
        self.assertIn("$deployedWaveCode", self.deploy)
        self.assertIn("[System.IO.File]::WriteAllText($deployedWaves", self.deploy)
        self.assertIn("Requested E2 test mode was not written exactly once", self.deploy)
        self.assertIn("Legacy E1 air test value is incorrect", self.deploy)
        self.assertIn("if ($E2TestMode -ne 0)", self.deploy)
        self.assertNotIn("WriteAllText($wavesSource", self.deploy)
```

Also assign `cls.deploy = DEPLOY.read_text(encoding="utf-8")` in this class’s `setUpClass`.

- [ ] **Step 2: Run deploy-contract tests and confirm red**

Run:

```powershell
python -m pytest tests/test_e2_airmobile.py::E2SequentialComboTests -q
```

Expected: the two deployment tests fail because the parameter and override are absent.

- [ ] **Step 3: Add the validated parameter and exact replacement helper**

Change the deploy script parameter block to:

```powershell
param(
    [string]$RepoRoot = "",
    [string]$WorkshopRoot = "E:\Steam\steamapps\workshop\content\400750\3636883799",
    [ValidateSet(0, 1, 2, 3)]
    [int]$E2TestMode = 0
)
```

After path normalization, add:

```powershell
function Set-ExactSingleReplacement {
    param([string]$Text, [string]$Old, [string]$New, [string]$Label)
    $count = [regex]::Matches($Text, [regex]::Escape($Old)).Count
    if ($count -ne 1) { throw "$Label expected exactly one source marker, found $count" }
    return $Text.Replace($Old, $New)
}
```

- [ ] **Step 4: Apply the override after the verified copy loop**

Immediately after the `$files` copy/hash loop, add:

```powershell
$deployedWaves = Join-Path $WorkshopRoot $files[4]
$deployedWaveCode = [System.IO.File]::ReadAllText($deployedWaves)
$sourceE2Init = '{"set_i" {var "support_e2_test$"} {op "="} {value 0}}'
$targetE2Init = '{"set_i" {var "support_e2_test$"} {op "="} {value ' + $E2TestMode + '}}'
$deployedWaveCode = Set-ExactSingleReplacement $deployedWaveCode $sourceE2Init $targetE2Init 'E2 test override'

if ($E2TestMode -ne 0) {
    $sourceLegacyInit = '{"set_i" {var "attack_support_air_test$"} {op "="} {value 1}}'
    $targetLegacyInit = '{"set_i" {var "attack_support_air_test$"} {op "="} {value 0}}'
    $deployedWaveCode = Set-ExactSingleReplacement $deployedWaveCode $sourceLegacyInit $targetLegacyInit 'Legacy E1 air-test override'
}

[System.IO.File]::WriteAllText($deployedWaves, $deployedWaveCode, [System.Text.UTF8Encoding]::new($false))
```

This occurs only after a byte-identical source copy, so rerunning any mode always restores source first and then reapplies the requested override.

- [ ] **Step 5: Validate deployed values and report the intentional hash difference**

After `$waves = Join-Path $WorkshopRoot $files[4]`, add:

```powershell
$validatedWaveCode = [System.IO.File]::ReadAllText($waves)
$expectedE2Init = '{"set_i" {var "support_e2_test$"} {op "="} {value ' + $E2TestMode + '}}'
if ([regex]::Matches($validatedWaveCode, [regex]::Escape($expectedE2Init)).Count -ne 1) {
    throw "Requested E2 test mode was not written exactly once"
}
$expectedLegacyMode = if ($E2TestMode -eq 0) { 1 } else { 0 }
$expectedLegacyInit = '{"set_i" {var "attack_support_air_test$"} {op "="} {value ' + $expectedLegacyMode + '}}'
if ([regex]::Matches($validatedWaveCode, [regex]::Escape($expectedLegacyInit)).Count -ne 1) {
    throw "Legacy E1 air test value is incorrect"
}
$sourceWaveHash = (Get-FileHash -LiteralPath $wavesSource -Algorithm SHA256).Hash
$deployedWaveHash = (Get-FileHash -LiteralPath $waves -Algorithm SHA256).Hash
Write-Host "E2 test mode: $E2TestMode; source wave hash: $sourceWaveHash; deployed wave hash: $deployedWaveHash"
```

- [ ] **Step 6: Run focused and full static verification**

Run:

```powershell
python -m pytest tests/test_e2_airmobile.py::E2SequentialComboTests -q
python -m pytest tests/test_e2_airmobile.py tests/test_attack_support_slot_proof.py -q
python -m pytest tests/ -q
```

Expected: every command exits `0` with no skipped E2 contract tests.

- [ ] **Step 7: Commit deploy work**

```powershell
git add tests/test_e2_airmobile.py tools/deploy_attack_support_probe.ps1
git commit -m "feat: add validated E2 test deployment mode"
```

### Task 3: Independent review, idempotent deployment, and live-test handoff

**Files:**
- Verify: `resource/map/multi/attack_support_waves.inc`
- Verify: `tools/deploy_attack_support_probe.ps1`
- Deploy: `E:\Steam\steamapps\workshop\content\400750\3636883799\resource\map\multi\attack_support_waves.inc`

**Interfaces:**
- Consumes: Tasks 1 and 2 plus the existing full repository verification suite.
- Produces: workshop item `3636883799` in mode `3`, with legacy E1 disabled and exact deployed hashes recorded.

- [ ] **Step 1: Run an independent lifecycle review**

Reviewer must verify from the diff that mode `3` starts only helicopter children, all helicopter terminal paths reach claim-free stage `70`, the handoff copies failure before clearing it, mode changes to `2` before stage `0`, and mode `2` cannot hand off again. Any lifecycle or source-default issue blocks deployment.

- [ ] **Step 2: Run final repository verification from a clean command**

```powershell
python -m pytest tests/ -q
git diff --check
git status --short
```

Expected: pytest exits `0`, `git diff --check` prints nothing, and status contains only intentional task files if the review produced uncommitted corrections.

- [ ] **Step 3: Deploy combo mode to workshop item 799**

```powershell
powershell.exe -ExecutionPolicy Bypass -File .\tools\deploy_attack_support_probe.ps1 -RepoRoot 'E:\Steam\steamapps\workshop\content\400750\CodeX AI Overhaul Submod' -E2TestMode 3
```

Expected: exit `0`; completion output reports E2 mode `3`, legacy mode `0`, and an intentional source/deployed wave hash difference.

- [ ] **Step 4: Prove idempotence by deploying the same mode again**

Run the same command a second time. Hash the deployed wave before and after the second run:

```powershell
Get-FileHash 'E:\Steam\steamapps\workshop\content\400750\3636883799\resource\map\multi\attack_support_waves.inc' -Algorithm SHA256
```

Expected: both mode-3 deployed hashes are identical and both deployments exit `0`.

- [ ] **Step 5: Assert source-off and deployed-combo values directly**

```powershell
Select-String -Path '.\resource\map\multi\attack_support_waves.inc' -SimpleMatch '{"set_i" {var "support_e2_test$"} {op "="} {value 0}}'
Select-String -Path 'E:\Steam\steamapps\workshop\content\400750\3636883799\resource\map\multi\attack_support_waves.inc' -SimpleMatch '{"set_i" {var "support_e2_test$"} {op "="} {value 3}}','{"set_i" {var "attack_support_air_test$"} {op "="} {value 0}}'
```

Expected: one source mode-0 match and exactly one deployed match for each mode-3/legacy-0 initializer.

- [ ] **Step 6: Give the live-test instructions**

Fully restart Gates of Hell, begin a new player-attack mission as RUSA, UKR, or NATO, and observe for two to four minutes. In `game.log`, confirm the E2 sequence begins at test `3`, progresses through the helicopter stages to `70`, then changes to test `2`/stage `0` and starts the paradrop. Read `e2_combo_helo_fail` as the helicopter result and `e2_fail` as the paradrop result.
