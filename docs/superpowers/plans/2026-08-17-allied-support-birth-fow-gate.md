# Allied Support — Actor Birth and FoW Gate Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Birth a support actor by cloning a parked prototype onto a birth waypoint that tags it
on arrival, give it to the human, and hold it stationary under an enforced fog-of-war
observation gate — producing a yes/no answer on whether a cloned actor illuminates terrain.

**Architecture:** Mission-interface (MI) triggers only, in new `.inc` files included by the
fourteen CWA campaign maps. Birth is `{"actor_to_waypoint"}` + `{clone}` +
`{approach "safe teleport & rotate"}` onto a new numeric waypoint whose `{commands}` block
tags the arriving actor. Ownership is the established literal 1–16 `{player}` switch keyed
from `id_attack_support$`. No Lua is added in this plan; one existing Lua order path is
disabled.

**Tech Stack:** GoH mission-interface script (`.inc` / `.mi`, brace-delimited), Lua 5.1
(edit only), Python 3.11.9 + pytest 9.1.1 for structural tests, PowerShell for map patching.

## Why this plan stops at the gate

This plan deliberately ends at the fog-of-war observation and does **not** build the
commander. Per the approved spec, a cloned actor that is dark while human-owned means
candidate A has failed and the commander is irrelevant. Building the commander before that
answer exists risks discarding all of it. The commander is a separate plan, written once this
plan returns a stage-3 result.

## Global Constraints

Every task's requirements implicitly include this section.

- **No TMAI-derived string anywhere** — not in file names, trigger names, variables, tags, or
  comments. This includes `tmai`, `p013`, and `bus_magic`.
- **Namespace:** all new variables and tags are prefixed `allied_support_cmd_`. Bare
  `allied_support_*` is owned by Code:X upstream (`ce_vars.inc:19`, tag
  `allied_support_template`) and must not be extended.
- **Never apply `_lua_mi` or `_lua_ignore`** to our actors. Either tag makes them permanently
  uncommandable.
- **Never hardcode a player id.** The human id comes from `id_attack_support$` through a
  literal 1–16 `{player}` switch. Lobby slot assignment varies between runs.
- **Fail closed on birth.** An arrival that is not tagged ends the wave. Never recover by
  selecting a recently-appeared or "new-looking" human entity — that is the false-positive
  class #112 exposed.
- **One birth mechanism per live run.** Candidate A and the `{"spawn"}` probe are interlocked.
- **Q1 only.** Q2 interfaces are prepared but not wired. Q3/Q4 files are not touched:
  `enemy_defense_support.inc`, `enemy_defense_templates.inc`, `enemy_attack_support.inc`.
- **All on-screen `{"timer"}` diagnostics must be wrapped** in the `support_debug$` gate
  (default 0). An ungated timer is a build failure in the existing toolchain convention.
- **Never read `spawnPointName` or `PlayerSpawnPoint`** in Lua on the extra Team A slot. They
  null-deref natively and `pcall` cannot catch it.

## Established facts this plan relies on

Verified by reading the live tree on 2026-08-17. Do not re-derive; do not contradict.

- **Waypoints in CWA maps are named entries in a `{waypoints}` block** inside each
  `campaign_capture_the_flag.mi` (fields map: line 2542). Form:
  `{"<name>" {position X Y Z} {radius N} {commands ...}}`.
- **A `{commands}` block acts on the arriving actor implicitly.** Waypoints `21` and `22`
  already exist in these maps and open with a bare, selector-less
  `{"entity_state" {tag_add support_e2_arrival}}`. This is the arrival-tagging mechanism, and
  it is already deployed here.
- **Occupied waypoint names in the maps include** `21`, `22`, `23`, `24`, `0`, and the named
  `attack_support_*` set. Our new pads must not reuse any of these.
- **Clones inherit no tags.** Runtime tags do not survive `{clone}`, and static `{Tags}` bind
  to original handles only. This is why arrival tagging is required, and it also means a clone
  does not carry pool tags — so cloning cannot corrupt the production pool count.
- **The prototype pool** is in `attack_support_templates.inc`: `{Human "mp/nato/2022s/..."}`
  entries at `{Player 0}`, parked at `y=-35100`, tagged `attack_support_inf_usmc` (20 bodies,
  4 × 5-strong USMC teams) plus `attack_support_tpl` and `hidden`.
- **Ownership idiom** (`attack_support_waves.inc:543`, `:3324`):
  `{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value N}} {"player" {selector {ignore_captured_by_user 0} {tag TAG}} {operation set} {player "N"}}}`
  repeated for N = 1..16. The engine will not accept a variable in the `{player}` field.
- **Map includes** sit at `campaign_capture_the_flag.mi:2535`–`2540`, with `dcg_script.inc`
  last. Variable declarations are one-per-line `{"name"}` entries in
  `resource/map/multi/dcg_vars.inc` (138 lines).
- **`support_e2_*` is a dormant experiment harness** in `attack_support_waves.inc`, gated on
  `support_e2_test$ == 1` (default 0). It uses `{"placement"}`, never `{clone}`, and owns
  waypoints 21/22. Leave it alone.
- **The fourteen map directories** are `resource/map/multi/dcg_[cwa71]_<name>/` for name in:
  `airbase border europe factory fields fulda grassland industrial monastery outback stasis
  train_station winds_valley woodland`.

## File Structure

| Path | Responsibility |
|---|---|
| `resource/map/multi/allied_support_birth.inc` | Birth only: clone dispatch to the birth pad. Nothing else. |
| `resource/map/multi/allied_support_handoff.inc` | Ownership switch to human, then the enforced FoW hold gate, then the Mate switch. |
| `resource/map/multi/dcg_vars.inc` | Add new variable declarations (modify). |
| `resource/map/multi/dcg_[cwa71]_*/campaign_capture_the_flag.mi` | Add birth waypoints + two includes (modify, 14 files, via tool). |
| `resource/script/multiplayer/modes/attack_support.lua` | Disable the `Scene.Squads` order path; keep identity publication (modify). |
| `tools/patch_allied_support_maps.py` | Idempotent map patcher: injects waypoints and includes. |
| `tools/verify_waypoint_band.py` | Proves a waypoint name is unused across all 14 maps. |
| `tests/test_allied_support_birth.py` | Structural tests for birth + handoff + namespace + global constraints. |
| `tests/test_attack_support_lua_ordering.py` | Structural tests proving the Lua order path is disabled and identity publication survives. |

`allied_support_command.inc` and `allied_support_command.lua` are **not** created in this
plan. They belong to the follow-up commander plan.

---

### Task 1: Test harness and waypoint-band verifier

`tests/` and `tools/` are tracked in git but deleted from the working tree. This task
establishes the harness fresh and delivers the one tool the next task depends on.

**Files:**
- Create: `tools/verify_waypoint_band.py`
- Create: `tests/conftest.py`
- Test: `tests/test_waypoint_band.py`

**Interfaces:**
- Consumes: nothing.
- Produces:
  - `tests/conftest.py` fixture `mod_root` → `pathlib.Path` to the repo root.
  - `tests/conftest.py` fixture `map_files` → `list[pathlib.Path]` of the 14
    `campaign_capture_the_flag.mi` paths.
  - `tools/verify_waypoint_band.py::waypoint_names(text: str) -> set[str]` — returns every
    waypoint name declared in a map's `{waypoints}` block.
  - `tools/verify_waypoint_band.py::band_is_free(root: pathlib.Path, names: list[str]) -> dict[str, list[str]]`
    — maps each requested name to the list of map stems that already declare it. An empty
    list for a name means that name is free.

- [ ] **Step 1: Write the failing test**

Create `tests/conftest.py`:

```python
import pathlib
import pytest

MAP_NAMES = [
    "airbase", "border", "europe", "factory", "fields", "fulda", "grassland",
    "industrial", "monastery", "outback", "stasis", "train_station",
    "winds_valley", "woodland",
]


@pytest.fixture(scope="session")
def mod_root() -> pathlib.Path:
    return pathlib.Path(__file__).resolve().parent.parent


@pytest.fixture(scope="session")
def map_files(mod_root: pathlib.Path) -> list[pathlib.Path]:
    paths = [
        mod_root / "resource" / "map" / "multi" / f"dcg_[cwa71]_{name}"
        / "campaign_capture_the_flag.mi"
        for name in MAP_NAMES
    ]
    missing = [str(p) for p in paths if not p.is_file()]
    assert not missing, f"missing map files: {missing}"
    return paths
```

Create `tests/test_waypoint_band.py`:

```python
import sys
import pathlib

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent / "tools"))

from verify_waypoint_band import waypoint_names, band_is_free


def test_waypoint_names_parses_named_and_numeric_entries():
    text = """
    {waypoints
        {"attack_support_entry_a"
            {position -5691.72 -800.01 0.00}
            {radius 150}
        }
        {"21"
            {position -6545.48 -920.01 0.00}
            {radius 800}
            {commands
                {"entity_state"
                    {tag_add support_e2_arrival}
                }
            }
        }
    }
    """
    assert waypoint_names(text) == {"attack_support_entry_a", "21"}


def test_waypoint_names_ignores_declarations_outside_the_waypoints_block():
    text = """
    {Human "mp/nato/2022s/usmc_medic" 0xaf24
        {Position -3120 -35100}
    }
    {waypoints
        {"only_this_one"
            {position 0 0 0}
        }
    }
    """
    assert waypoint_names(text) == {"only_this_one"}


def test_existing_band_is_reported_as_occupied(map_files, mod_root):
    result = band_is_free(mod_root, ["21", "22"])
    assert result["21"], "waypoint 21 is known to exist and must be reported occupied"
    assert result["22"], "waypoint 22 is known to exist and must be reported occupied"


def test_chosen_birth_band_is_free(map_files, mod_root):
    result = band_is_free(mod_root, ["31", "32"])
    assert result == {"31": [], "32": []}, f"chosen birth band is not free: {result}"
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python -m pytest tests/test_waypoint_band.py -v`
Expected: FAIL — `ModuleNotFoundError: No module named 'verify_waypoint_band'`

- [ ] **Step 3: Write the implementation**

Create `tools/verify_waypoint_band.py`:

```python
"""Waypoint-name verification for the CWA campaign maps.

Waypoint names live in a {waypoints ...} block inside each map's
campaign_capture_the_flag.mi. A name collision crashes every map at load, so a new
name must be proven unused across all fourteen maps before injection.
"""
import pathlib
import re
import sys

MAP_NAMES = [
    "airbase", "border", "europe", "factory", "fields", "fulda", "grassland",
    "industrial", "monastery", "outback", "stasis", "train_station",
    "winds_valley", "woodland",
]

_WAYPOINTS_HEAD = re.compile(r"\{waypoints\b")
_ENTRY = re.compile(r'\{"([^"]+)"')


def _waypoints_block(text: str) -> str:
    """Return the source of the {waypoints ...} block, brace-balanced."""
    match = _WAYPOINTS_HEAD.search(text)
    if not match:
        return ""
    start = match.start()
    depth = 0
    for index in range(start, len(text)):
        char = text[index]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return text[start:index + 1]
    return text[start:]


def waypoint_names(text: str) -> set[str]:
    """Every waypoint name declared in the map's waypoints block."""
    block = _waypoints_block(text)
    if not block:
        return set()
    # Skip the {waypoints head itself, then take every quoted entry key. Nested
    # keys inside {commands ...} are action names like "entity_state", so restrict
    # to entries at the block's first nesting level.
    names: set[str] = set()
    depth = 0
    index = 0
    while index < len(block):
        char = block[index]
        if char == "{":
            depth += 1
            if depth == 2:
                entry = _ENTRY.match(block, index)
                if entry:
                    names.add(entry.group(1))
        elif char == "}":
            depth -= 1
        index += 1
    return names


def map_paths(root: pathlib.Path) -> list[pathlib.Path]:
    return [
        root / "resource" / "map" / "multi" / f"dcg_[cwa71]_{name}"
        / "campaign_capture_the_flag.mi"
        for name in MAP_NAMES
    ]


def band_is_free(root: pathlib.Path, names: list[str]) -> dict[str, list[str]]:
    """Map each requested name to the map stems that already declare it."""
    occupied: dict[str, list[str]] = {name: [] for name in names}
    for path in map_paths(root):
        declared = waypoint_names(path.read_text(encoding="utf-8", errors="replace"))
        for name in names:
            if name in declared:
                occupied[name].append(path.parent.name)
    return occupied


def main(argv: list[str]) -> int:
    if len(argv) < 2:
        print("usage: verify_waypoint_band.py NAME [NAME ...]")
        return 2
    root = pathlib.Path(__file__).resolve().parent.parent
    result = band_is_free(root, argv[1:])
    failed = False
    for name, maps in sorted(result.items()):
        if maps:
            failed = True
            print(f"OCCUPIED {name}: {', '.join(maps)}")
        else:
            print(f"FREE     {name}")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
```

- [ ] **Step 4: Run the tests to verify they pass**

Run: `python -m pytest tests/test_waypoint_band.py -v`
Expected: PASS, 4 passed.

If `test_chosen_birth_band_is_free` fails, the band `31`/`32` is taken. Pick the next free
two-digit pair, update the test and every later task that names the pads, and re-run. Do not
proceed with an occupied band.

- [ ] **Step 5: Confirm the band directly**

Run: `python tools/verify_waypoint_band.py 31 32`
Expected output:
```
FREE     31
FREE     32
```

- [ ] **Step 6: Commit**

```bash
git add tests/conftest.py tests/test_waypoint_band.py tools/verify_waypoint_band.py
git commit -m "test: add waypoint band verifier for allied support birth pads"
```

---

### Task 2: Declare the new variables

**Files:**
- Modify: `resource/map/multi/dcg_vars.inc`
- Test: `tests/test_allied_support_birth.py`

**Interfaces:**
- Consumes: `mod_root` fixture from Task 1.
- Produces these mission variables, all defaulting to 0, referenced by every later task:
  - `allied_support_cmd_enable$` — master enable for candidate A birth. Default 0.
  - `allied_support_cmd_spawn_probe$` — master enable for the `{"spawn"}` probe. Default 0.
    Interlocked with the above: birth refuses to run if both are 1.
  - `allied_support_cmd_stage$` — observable progress code: 0 idle, 10 clone dispatched,
    20 arrival tagged, 30 human-owned, 40 holding at FoW gate, 50 continue received,
    60 mate-owned, 70 settled, 90 failed.
  - `allied_support_cmd_fail$` — failure code: 0 none, 1 no prototype available,
    2 arrival not tagged, 3 human id unresolved, 4 mate id unresolved, 5 both birth
    mechanisms enabled.
  - `allied_support_cmd_fow_continue$` — the observation gate release. Default 0; set to 1 to
    release.
  - `allied_support_cmd_gate_auto$` — set to 1 when the 60s backstop released the gate instead
    of an operator. An auto-release never counts as a pass.
  - `allied_support_cmd_mate_id$` — the runtime-resolved mate player id, 0 until resolved.

- [ ] **Step 1: Write the failing test**

Create `tests/test_allied_support_birth.py`:

```python
import re

REQUIRED_VARS = [
    "allied_support_cmd_enable",
    "allied_support_cmd_spawn_probe",
    "allied_support_cmd_stage",
    "allied_support_cmd_fail",
    "allied_support_cmd_fow_continue",
    "allied_support_cmd_gate_auto",
    "allied_support_cmd_mate_id",
]

# Strings that must never appear anywhere in our new content.
BANNED_SUBSTRINGS = ["tmai", "p013", "bus_magic", "fpc1", "fpc2", "fpc3", "fpc4", "fpc5"]


def _vars_text(mod_root):
    return (mod_root / "resource" / "map" / "multi" / "dcg_vars.inc").read_text(
        encoding="utf-8", errors="replace"
    )


def test_every_new_variable_is_declared(mod_root):
    text = _vars_text(mod_root)
    declared = set(re.findall(r'\{"([^"]+)"\}', text))
    missing = [name for name in REQUIRED_VARS if name not in declared]
    assert not missing, f"undeclared variables: {missing}"


def test_variables_are_declared_exactly_once(mod_root):
    text = _vars_text(mod_root)
    declared = re.findall(r'\{"([^"]+)"\}', text)
    for name in REQUIRED_VARS:
        assert declared.count(name) == 1, f"{name} declared {declared.count(name)} times"


def test_variable_names_carry_no_banned_substrings(mod_root):
    for name in REQUIRED_VARS:
        lowered = name.lower()
        for banned in BANNED_SUBSTRINGS:
            assert banned not in lowered, f"{name} contains banned substring {banned}"


def test_new_variables_do_not_collide_with_upstream_codex_names(mod_root):
    """Code:X owns bare allied_support_* names; ours must all be under the _cmd_ prefix."""
    upstream = {
        "allied_support_initialized", "allied_support_waves_left",
        "allied_support_wave_size", "allied_support_target",
        "allied_support_busy", "allied_support_wave_num",
    }
    for name in REQUIRED_VARS:
        assert name not in upstream
        assert name.startswith("allied_support_cmd_")
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python -m pytest tests/test_allied_support_birth.py -v`
Expected: FAIL on `test_every_new_variable_is_declared` with all seven names listed as
missing. The other three tests pass already (they check name shape, not declaration).

- [ ] **Step 3: Add the declarations**

In `resource/map/multi/dcg_vars.inc`, immediately after the existing line `{"support_e2_flag"}`
(line 134), insert:

```
			{"allied_support_cmd_enable"}
			{"allied_support_cmd_spawn_probe"}
			{"allied_support_cmd_stage"}
			{"allied_support_cmd_fail"}
			{"allied_support_cmd_fow_continue"}
			{"allied_support_cmd_gate_auto"}
			{"allied_support_cmd_mate_id"}
```

Match the surrounding indentation exactly: three tab characters.

- [ ] **Step 4: Run the tests to verify they pass**

Run: `python -m pytest tests/test_allied_support_birth.py -v`
Expected: PASS, 4 passed.

- [ ] **Step 5: Commit**

```bash
git add resource/map/multi/dcg_vars.inc tests/test_allied_support_birth.py
git commit -m "feat: declare allied support command variables"
```

---

### Task 3: Map patcher — birth waypoints and includes

Injects birth pads `31` (side a) and `32` (side b) into all fourteen maps, positioned at each
map's existing `attack_support_rear_a1` / `attack_support_rear_b1` coordinates, and adds the
two new includes. Idempotent: running it twice changes nothing.

**Files:**
- Create: `tools/patch_allied_support_maps.py`
- Modify: `resource/map/multi/dcg_[cwa71]_*/campaign_capture_the_flag.mi` (14 files, via tool)
- Test: `tests/test_map_patch.py`

**Interfaces:**
- Consumes: `waypoint_names`, `map_paths` from `tools/verify_waypoint_band.py` (Task 1).
- Produces:
  - `tools/patch_allied_support_maps.py::birth_waypoint_block(name: str, x: str, y: str, z: str, side: str) -> str`
    — the waypoint source to inject.
  - `tools/patch_allied_support_maps.py::patch_text(text: str) -> tuple[str, list[str]]`
    — returns the patched map source and a list of change descriptions. Empty list means
    already patched.
  - Arrival tags produced for later tasks: `allied_support_cmd_fresh` on every arrival, plus
    `allied_support_cmd_side_a` or `allied_support_cmd_side_b`.

- [ ] **Step 1: Write the failing test**

Create `tests/test_map_patch.py`:

```python
import sys
import pathlib

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent / "tools"))

from patch_allied_support_maps import birth_waypoint_block, patch_text
from verify_waypoint_band import waypoint_names

MINIMAL_MAP = """{mission
	{script
			(include "../attack_support_waves.inc")
			(include "../enemy_defense_support.inc")
			(include "../dcg_script.inc")
	}
	{waypoints
		{"attack_support_rear_a1"
			{position -6545.48 -920.01 0.00}
			{radius 150}
		}
		{"attack_support_rear_b1"
			{position 6482.88 1507.47 0.00}
			{radius 150}
		}
	}
}
"""


def test_birth_waypoint_block_tags_arrival_without_a_selector():
    block = birth_waypoint_block("31", "-6545.48", "-920.01", "0.00", "a")
    assert '{"31"' in block
    assert "{commands" in block
    # The arrival tag must be applied by a bare entity_state with no selector, which
    # is what makes it act on the arriving actor.
    assert "{tag_add allied_support_cmd_fresh}" in block
    assert "{tag_add allied_support_cmd_side_a}" in block
    assert "{selector" not in block.split("{commands", 1)[1].split("{tag_add", 1)[0]


def test_birth_waypoint_block_never_tags_lua_guards():
    block = birth_waypoint_block("31", "0", "0", "0", "a")
    assert "_lua_mi" not in block
    assert "_lua_ignore" not in block


def test_patch_adds_both_pads_and_both_includes():
    patched, changes = patch_text(MINIMAL_MAP)
    assert changes, "expected changes on a fresh map"
    assert waypoint_names(patched) >= {"31", "32"}
    assert '(include "../allied_support_birth.inc")' in patched
    assert '(include "../allied_support_handoff.inc")' in patched


def test_patch_positions_pads_on_the_existing_rear_pads():
    patched, _ = patch_text(MINIMAL_MAP)
    assert "-6545.48 -920.01" in patched.split('{"31"', 1)[1].split("}", 2)[0] + \
        patched.split('{"31"', 1)[1][:200]
    assert "6482.88 1507.47" in patched.split('{"32"', 1)[1][:200]


def test_patch_is_idempotent():
    once, first_changes = patch_text(MINIMAL_MAP)
    twice, second_changes = patch_text(once)
    assert first_changes
    assert second_changes == []
    assert once == twice


def test_includes_are_added_before_dcg_script():
    """dcg_script.inc must stay last, matching the existing include order."""
    patched, _ = patch_text(MINIMAL_MAP)
    birth = patched.index('allied_support_birth.inc')
    handoff = patched.index('allied_support_handoff.inc')
    dcg = patched.index('dcg_script.inc')
    assert birth < dcg
    assert handoff < dcg


def test_all_fourteen_live_maps_are_patched(map_files):
    for path in map_files:
        text = path.read_text(encoding="utf-8", errors="replace")
        names = waypoint_names(text)
        assert {"31", "32"} <= names, f"{path.parent.name} missing birth pads"
        assert '(include "../allied_support_birth.inc")' in text, path.parent.name
        assert '(include "../allied_support_handoff.inc")' in text, path.parent.name
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python -m pytest tests/test_map_patch.py -v`
Expected: FAIL — `ModuleNotFoundError: No module named 'patch_allied_support_maps'`

- [ ] **Step 3: Write the patcher**

Create `tools/patch_allied_support_maps.py`:

```python
"""Idempotently inject allied-support birth pads and includes into the CWA maps.

Birth pads are numeric waypoints 31 (side a) and 32 (side b), placed at each map's
existing attack_support_rear_a1 / _b1 coordinates. Each pad's {commands} block tags
the ARRIVING actor - a bare entity_state with no selector - which is the only way to
mark a freshly cloned entity, whose provenance no selector in this format can express.

Radius 800 mirrors the proven waypoints 21/22 already in these maps.
"""
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))

from verify_waypoint_band import map_paths, waypoint_names

BIRTH_PAD_A = "31"
BIRTH_PAD_B = "32"

_POSITION = r'\{position\s+(-?[\d.]+)\s+(-?[\d.]+)\s+(-?[\d.]+)\}'


def birth_waypoint_block(name: str, x: str, y: str, z: str, side: str) -> str:
    """Waypoint source for a birth pad. The commands block runs on the arriving actor."""
    return (
        f'\t\t\t{{"{name}"\n'
        f'\t\t\t\t{{position {x} {y} {z}}}\n'
        f'\t\t\t\t{{radius 800}}\n'
        f'\t\t\t\t{{commands\n'
        f'\t\t\t\t\t{{"entity_state"\n'
        f'\t\t\t\t\t\t{{tag_add allied_support_cmd_fresh}}\n'
        f'\t\t\t\t\t\t{{tag_add allied_support_cmd_side_{side}}}\n'
        f'\t\t\t\t\t}}\n'
        f'\t\t\t\t}}\n'
        f'\t\t\t}}\n'
    )


def _rear_pad_position(text: str, pad_name: str) -> tuple[str, str, str]:
    """Read an existing waypoint's position so the birth pad lands on the same spot."""
    anchor = text.find(f'{{"{pad_name}"')
    if anchor < 0:
        raise ValueError(f"map has no {pad_name} waypoint to anchor against")
    match = re.search(_POSITION, text[anchor:anchor + 400])
    if not match:
        raise ValueError(f"could not read position for {pad_name}")
    return match.group(1), match.group(2), match.group(3)


def patch_text(text: str) -> tuple[str, list[str]]:
    changes: list[str] = []
    existing = waypoint_names(text)

    for pad, anchor_name, side in (
        (BIRTH_PAD_A, "attack_support_rear_a1", "a"),
        (BIRTH_PAD_B, "attack_support_rear_b1", "b"),
    ):
        if pad in existing:
            continue
        x, y, z = _rear_pad_position(text, anchor_name)
        block = birth_waypoint_block(pad, x, y, z, side)
        # Insert immediately after the {waypoints line so ordering is deterministic.
        head = re.search(r"\{waypoints[^\n]*\n", text)
        if not head:
            raise ValueError("map has no waypoints block")
        text = text[:head.end()] + block + text[head.end():]
        changes.append(f"waypoint {pad}")

    for include in ("allied_support_birth.inc", "allied_support_handoff.inc"):
        line = f'(include "../{include}")'
        if line in text:
            continue
        anchor = f'(include "../dcg_script.inc")'
        index = text.find(anchor)
        if index < 0:
            raise ValueError("map has no dcg_script.inc include to anchor against")
        indent = ""
        line_start = text.rfind("\n", 0, index) + 1
        indent = text[line_start:index]
        text = text[:line_start] + f"{indent}{line}\n" + text[line_start:]
        changes.append(f"include {include}")

    return text, changes


def main() -> int:
    root = pathlib.Path(__file__).resolve().parent.parent
    total = 0
    for path in map_paths(root):
        original = path.read_text(encoding="utf-8", errors="replace")
        patched, changes = patch_text(original)
        if changes:
            path.write_text(patched, encoding="utf-8")
            total += 1
            print(f"PATCHED {path.parent.name}: {', '.join(changes)}")
        else:
            print(f"OK      {path.parent.name}")
    print(f"{total} map(s) changed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
```

- [ ] **Step 4: Run the unit tests to verify the patcher logic passes**

Run: `python -m pytest tests/test_map_patch.py -v -k "not live_maps"`
Expected: PASS, 6 passed, 1 deselected.

- [ ] **Step 5: Patch the live maps**

Run: `python tools/patch_allied_support_maps.py`
Expected: 14 lines reading `PATCHED dcg_[cwa71]_<name>: waypoint 31, waypoint 32, include allied_support_birth.inc, include allied_support_handoff.inc`, then `14 map(s) changed`.

- [ ] **Step 6: Prove idempotency against the live maps**

Run: `python tools/patch_allied_support_maps.py`
Expected: 14 lines reading `OK      dcg_[cwa71]_<name>`, then `0 map(s) changed`.

- [ ] **Step 7: Run the full test file including the live-map check**

Run: `python -m pytest tests/test_map_patch.py tests/test_waypoint_band.py -v`
Expected: PASS. Note `test_chosen_birth_band_is_free` from Task 1 now **fails by design** —
the band is occupied because we just injected it. Update that test to assert the band is
occupied by exactly our own pads:

```python
def test_chosen_birth_band_is_claimed_by_our_pads(map_files, mod_root):
    """After patching, 31/32 must exist in all fourteen maps and nowhere unexpected."""
    result = band_is_free(mod_root, ["31", "32"])
    assert len(result["31"]) == 14, result["31"]
    assert len(result["32"]) == 14, result["32"]
```

Delete the old `test_chosen_birth_band_is_free`. Re-run and expect PASS.

- [ ] **Step 8: Commit**

```bash
git add tools/patch_allied_support_maps.py tests/test_map_patch.py tests/test_waypoint_band.py "resource/map/multi/dcg_[cwa71]_airbase/campaign_capture_the_flag.mi" "resource/map/multi/dcg_[cwa71]_border/campaign_capture_the_flag.mi" "resource/map/multi/dcg_[cwa71]_europe/campaign_capture_the_flag.mi" "resource/map/multi/dcg_[cwa71]_factory/campaign_capture_the_flag.mi" "resource/map/multi/dcg_[cwa71]_fields/campaign_capture_the_flag.mi" "resource/map/multi/dcg_[cwa71]_fulda/campaign_capture_the_flag.mi" "resource/map/multi/dcg_[cwa71]_grassland/campaign_capture_the_flag.mi" "resource/map/multi/dcg_[cwa71]_industrial/campaign_capture_the_flag.mi" "resource/map/multi/dcg_[cwa71]_monastery/campaign_capture_the_flag.mi" "resource/map/multi/dcg_[cwa71]_outback/campaign_capture_the_flag.mi" "resource/map/multi/dcg_[cwa71]_stasis/campaign_capture_the_flag.mi" "resource/map/multi/dcg_[cwa71]_train_station/campaign_capture_the_flag.mi" "resource/map/multi/dcg_[cwa71]_winds_valley/campaign_capture_the_flag.mi" "resource/map/multi/dcg_[cwa71]_woodland/campaign_capture_the_flag.mi"
git commit -m "feat: inject allied support birth pads into all fourteen CWA maps"
```

---

### Task 4: Disable the existing Lua order path

Done before any live run so that only one component can issue orders. Keeps identity
publication, the engine-state mirror, and all logging.

**Files:**
- Modify: `resource/script/multiplayer/modes/attack_support.lua`
- Test: `tests/test_attack_support_lua_ordering.py`

**Interfaces:**
- Consumes: nothing.
- Produces: the constant `ORDERING_ENABLED = false` in `attack_support.lua`, which the
  commander plan flips or removes when it takes over ordering.

- [ ] **Step 1: Write the failing test**

Create `tests/test_attack_support_lua_ordering.py`:

```python
import re

LUA = ("resource", "script", "multiplayer", "modes", "attack_support.lua")


def _lua(mod_root):
    path = mod_root
    for part in LUA:
        path = path / part
    return path.read_text(encoding="utf-8", errors="replace")


def test_ordering_is_gated_by_a_single_named_switch(mod_root):
    text = _lua(mod_root)
    assert re.search(r"^local ORDERING_ENABLED = false", text, re.M), \
        "expected a top-level ORDERING_ENABLED = false switch"


def test_order_issuing_functions_return_before_commanding(mod_root):
    """orderSquad and orderNewSquads must bail out before any command call."""
    text = _lua(mod_root)
    for func in ("local function orderSquad(", "local function orderNewSquads("):
        start = text.index(func)
        body = text[start:start + 400]
        guard = body.index("if not ORDERING_ENABLED then return end")
        # The guard must precede any command call inside the function body.
        for call in ("CaptureFlag", "SeekAndDestroy", "Squads"):
            hit = body.find(call)
            if hit >= 0:
                assert guard < hit, f"{func} calls {call} before the ORDERING_ENABLED guard"


def test_periodic_reorder_loop_is_gated(mod_root):
    """The 400-quant re-order of every squad must not run."""
    text = _lua(mod_root)
    start = text.index("local function onQuant()")
    body = text[start:text.index("local function onGameEnd()")]
    assert "% 400 == 0" in body
    reorder = body.index("% 400 == 0")
    guard = body.rindex("ORDERING_ENABLED", 0, reorder)
    assert guard < reorder, "the 400-quant re-order block is not gated"


def test_identity_publication_and_mirror_survive(mod_root):
    text = _lua(mod_root)
    assert "publishIdentity(id)" in text, "identity publication must be kept"
    assert "mirrorEngineState()" in text, "engine-state mirror must be kept"
    assert "local function mirrorEngineState()" in text


def test_no_banned_strings_added(mod_root):
    text = _lua(mod_root).lower()
    for banned in ("tmai", "p013"):
        assert banned not in text
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python -m pytest tests/test_attack_support_lua_ordering.py -v`
Expected: FAIL on the first three tests — `ORDERING_ENABLED` does not exist.
`test_identity_publication_and_mirror_survive` and `test_no_banned_strings_added` pass already.

- [ ] **Step 3: Add the switch**

In `resource/script/multiplayer/modes/attack_support.lua`, immediately after the existing
line `local DEBUG_LOG = true` (line 21), insert:

```lua
-- Direct Scene.Squads ordering is disabled while the allied support commander is under
-- test, so exactly one component can issue orders. Identity publication, the engine-state
-- mirror, and all logging are unaffected.
--
-- This path is believed inert today because MI-delivered units have not appeared in
-- Scene.Squads. But the new birth pipeline exists specifically to change registration, so
-- it could activate exactly when a second commander must not exist.
local ORDERING_ENABLED = false
```

- [ ] **Step 4: Gate `orderSquad`**

Change the opening of `orderSquad` (line 163) from:

```lua
local function orderSquad(squad)
	local c = cmds()
	if not c then return end
```

to:

```lua
local function orderSquad(squad)
	if not ORDERING_ENABLED then return end
	local c = cmds()
	if not c then return end
```

- [ ] **Step 5: Gate `orderNewSquads`**

Change the opening of `orderNewSquads` (line 180) from:

```lua
local function orderNewSquads()
	local sc = scene()
	if not sc or type(sc.Squads) ~= "table" then return end
```

to:

```lua
local function orderNewSquads()
	if not ORDERING_ENABLED then return end
	local sc = scene()
	if not sc or type(sc.Squads) ~= "table" then return end
```

- [ ] **Step 6: Gate the periodic re-order in `onQuant`**

Change the block at lines 237–245 from:

```lua
	orderNewSquads()
	if state.quant > 0 and state.quant % 400 == 0 then
		local sc = scene()
		if sc and type(sc.Squads) == "table" then
			for _, squad in pairs(sc.Squads) do
				orderSquad(squad)
			end
		end
	end
```

to:

```lua
	orderNewSquads()
	if ORDERING_ENABLED and state.quant > 0 and state.quant % 400 == 0 then
		local sc = scene()
		if sc and type(sc.Squads) == "table" then
			for _, squad in pairs(sc.Squads) do
				orderSquad(squad)
			end
		end
	end
```

- [ ] **Step 7: Run the tests to verify they pass**

Run: `python -m pytest tests/test_attack_support_lua_ordering.py -v`
Expected: PASS, 5 passed.

- [ ] **Step 8: Commit**

```bash
git add resource/script/multiplayer/modes/attack_support.lua tests/test_attack_support_lua_ordering.py
git commit -m "fix: disable attack_support Scene.Squads ordering during commander test"
```

---

### Task 5: Birth — clone dispatch

**Files:**
- Create: `resource/map/multi/allied_support_birth.inc`
- Test: `tests/test_allied_support_birth.py` (extend)

**Interfaces:**
- Consumes: variables from Task 2; birth pads `31`/`32` and arrival tags
  `allied_support_cmd_fresh`, `allied_support_cmd_side_a`, `allied_support_cmd_side_b` from
  Task 3; the prototype pool tag `attack_support_inf_usmc` from
  `attack_support_templates.inc`.
- Produces:
  - Trigger `allied_support_cmd/birth_init` — arms on `init`, zeroes state.
  - Trigger `allied_support_cmd/birth_dispatch` — performs one clone dispatch, sets
    `allied_support_cmd_stage$` to 10.
  - Trigger `allied_support_cmd/birth_verify` — confirms arrival tagging within a bounded
    window; sets stage 20 on success, or stage 90 + `allied_support_cmd_fail$` 2 on failure.
  - Actors tagged `allied_support_cmd_fresh`, consumed by Task 6.

- [ ] **Step 1: Write the failing test**

Append to `tests/test_allied_support_birth.py`:

```python
BIRTH_INC = ("resource", "map", "multi", "allied_support_birth.inc")


def _read(mod_root, parts):
    path = mod_root
    for part in parts:
        path = path / part
    return path.read_text(encoding="utf-8", errors="replace")


def test_birth_file_exists_and_declares_its_triggers(mod_root):
    text = _read(mod_root, BIRTH_INC)
    for trigger in (
        '{"allied_support_cmd/birth_init"',
        '{"allied_support_cmd/birth_dispatch"',
        '{"allied_support_cmd/birth_verify"',
    ):
        assert trigger in text, f"missing trigger {trigger}"


def test_birth_uses_clone_dispatch_not_placement(mod_root):
    text = _read(mod_root, BIRTH_INC)
    assert '{"actor_to_waypoint"' in text
    assert "{clone}" in text
    assert '{approach "safe teleport & rotate"}' in text
    assert '{"placement"' not in text, "birth must clone, never place"


def test_birth_targets_both_pads(mod_root):
    text = _read(mod_root, BIRTH_INC)
    assert '{waypoint "31"}' in text
    assert '{waypoint "32"}' in text


def test_birth_is_interlocked_against_the_spawn_probe(mod_root):
    """Both mechanisms enabled must be a hard failure, not a silent preference."""
    text = _read(mod_root, BIRTH_INC)
    assert "allied_support_cmd_spawn_probe$" in text
    assert '{value 5}' in text, "fail code 5 (both mechanisms enabled) must be reachable"


def test_birth_fails_closed_on_untagged_arrival(mod_root):
    text = _read(mod_root, BIRTH_INC)
    assert "allied_support_cmd_fresh" in text
    assert '{value 90}' in text, "stage 90 (failed) must be reachable"
    assert '{value 2}' in text, "fail code 2 (arrival not tagged) must be reachable"


def test_birth_never_selects_by_recency_or_appearance(mod_root):
    """The #112 false-positive class: no fallback that grabs an arbitrary new human."""
    text = _read(mod_root, BIRTH_INC)
    lowered = text.lower()
    for smell in ("newest", "recent", "last_created", "any_human", "unclaimed"):
        assert smell not in lowered, f"suspicious recovery selector: {smell}"


def test_birth_never_applies_lua_guard_tags(mod_root):
    text = _read(mod_root, BIRTH_INC)
    assert "tag_add _lua_mi" not in text
    assert "tag_add _lua_ignore" not in text


def test_birth_has_no_banned_strings(mod_root):
    text = _read(mod_root, BIRTH_INC).lower()
    for banned in BANNED_SUBSTRINGS:
        assert banned not in text, f"banned substring {banned}"


def test_every_onscreen_timer_is_debug_gated(mod_root):
    """A shipped run must show the player nothing."""
    text = _read(mod_root, BIRTH_INC)
    if '{"timer"' in text:
        assert 'support_debug$' in text, "timers present but no support_debug$ gate"
        assert text.count('{"timer"') <= text.count('support_debug$')
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python -m pytest tests/test_allied_support_birth.py -v`
Expected: FAIL — `FileNotFoundError` for `allied_support_birth.inc` on the new tests. The four
Task 2 variable tests still pass.

- [ ] **Step 3: Write the birth file**

Create `resource/map/multi/allied_support_birth.inc`:

```
; Allied support command - actor birth.
;
; Births one support actor by CLONING a parked prototype from the existing off-map pool
; onto birth pad 31 (side a) or 32 (side b). The pad's own {commands} block tags the
; ARRIVING actor - see the pads in each campaign_capture_the_flag.mi. That is the entire
; reason this works: a freshly created entity's provenance is invisible to every selector
; this format can express, so nothing here ever tries to select one.
;
; Prototype originals never move, so birth is repeatable and consumes nothing from the
; player's roster. Clones inherit no tags at all - neither runtime tags nor static {Tags} -
; so the pool count in attack_support_templates.inc is unaffected.
;
; FAIL CLOSED. If the arrival is not tagged, the wave ends with fail code 2. There is no
; fallback, and in particular nothing here may recover by selecting a recently-appeared or
; otherwise "new-looking" human. That recovery shape produces telemetry indistinguishable
; from success while proving nothing, which is the exact false positive #112 exposed.
;
; Gated by allied_support_cmd_enable$ (default 0) and interlocked with
; allied_support_cmd_spawn_probe$: if both are 1 this refuses to run with fail code 5, so a
; fog-of-war observation is always attributable to one mechanism.

	{"allied_support_cmd/birth_init"
		{condition {terms {"1.event" {id "init"}}}}
		{actions
			{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 0}}
			{"set_i" {var "allied_support_cmd_fail$"} {op "="} {value 0}}
			{"set_i" {var "allied_support_cmd_fow_continue$"} {op "="} {value 0}}
			{"set_i" {var "allied_support_cmd_gate_auto$"} {op "="} {value 0}}
			{"set_i" {var "allied_support_cmd_mate_id$"} {op "="} {value 0}}
		}
	}

	{"allied_support_cmd/birth_dispatch"
		{condition
			{terms
				{"1.cmp_i" {var "allied_support_cmd_enable$"} {op "=="} {value 1}}
				{"2.cmp_i" {var "allied_support_cmd_stage$"} {op "=="} {value 0}}
				{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
			}
		}
		{actions
			{"switch"
				{"case"
					{condition {type cmp_i} {var "allied_support_cmd_spawn_probe$"} {op "=="} {value 1}}
					{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 90}}
					{"set_i" {var "allied_support_cmd_fail$"} {op "="} {value 5}}
				}
				{"default"
					{"switch"
						{"case"
							{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
							{"actor_to_waypoint"
								{selector
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{select {tag {tag attack_support_inf_usmc}}}
										{include {tag {tag hidden}}}
									}
									{amount 1}
								}
								{waypoint "32"}
								{clone}
								{approach "safe teleport & rotate"}
							}
						}
						{"default"
							{"actor_to_waypoint"
								{selector
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{select {tag {tag attack_support_inf_usmc}}}
										{include {tag {tag hidden}}}
									}
									{amount 1}
								}
								{waypoint "31"}
								{clone}
								{approach "safe teleport & rotate"}
							}
						}
					}
					{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 10}}
				}
			}
		}
	}

	{"allied_support_cmd/birth_verify"
		{condition
			{terms
				{"1.cmp_i" {var "allied_support_cmd_stage$"} {op "=="} {value 10}}
			}
		}
		{actions
			{"delay" {time 3.0}}
			{"switch"
				{"case"
					{condition {type entities}
						{selector
							{ignore_captured_by_user 0}
							{group {select {tag {tag allied_support_cmd_fresh}}}}
						}
						{op ">"} {value 0}
					}
					{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 20}}
				}
				{"default"
					{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 90}}
					{"set_i" {var "allied_support_cmd_fail$"} {op "="} {value 2}}
				}
			}
		}
	}
```

- [ ] **Step 4: Run the tests to verify they pass**

Run: `python -m pytest tests/test_allied_support_birth.py -v`
Expected: PASS, 13 passed.

- [ ] **Step 5: Commit**

```bash
git add resource/map/multi/allied_support_birth.inc tests/test_allied_support_birth.py
git commit -m "feat: add allied support clone birth with fail-closed arrival check"
```

---

### Task 6: Handoff — human ownership, FoW hold gate, mate transfer

**Files:**
- Create: `resource/map/multi/allied_support_handoff.inc`
- Test: `tests/test_allied_support_handoff.py`

**Interfaces:**
- Consumes: `allied_support_cmd_fresh` and stage 20 from Task 5; `id_attack_support$`.
- Produces:
  - Trigger `allied_support_cmd/handoff_human` — 1–16 switch to the human, stage 30, retag
    `allied_support_cmd_human`.
  - Trigger `allied_support_cmd/handoff_gate` — the enforced hold, stage 40.
  - Trigger `allied_support_cmd/handoff_mate` — 1–16 switch to the mate, stage 60, retag
    `allied_support_cmd_mate`, then stage 70 after a 3s settle.
  - Actors tagged `allied_support_cmd_mate` — the intake set for the commander plan.

- [ ] **Step 1: Write the failing test**

Create `tests/test_allied_support_handoff.py`:

```python
import re

HANDOFF_INC = ("resource", "map", "multi", "allied_support_handoff.inc")
BANNED_SUBSTRINGS = ["tmai", "p013", "bus_magic", "fpc1", "fpc2", "fpc3", "fpc4", "fpc5"]


def _read(mod_root):
    path = mod_root
    for part in HANDOFF_INC:
        path = path / part
    return path.read_text(encoding="utf-8", errors="replace")


def test_handoff_declares_its_three_triggers(mod_root):
    text = _read(mod_root)
    for trigger in (
        '{"allied_support_cmd/handoff_human"',
        '{"allied_support_cmd/handoff_gate"',
        '{"allied_support_cmd/handoff_mate"',
    ):
        assert trigger in text, f"missing trigger {trigger}"


def test_human_ownership_uses_a_full_sixteen_case_switch(mod_root):
    """The engine will not accept a variable in the {player} field."""
    text = _read(mod_root)
    human_block = text.split('handoff_human')[1].split('handoff_gate')[0]
    for n in range(1, 17):
        assert f'{{value {n}}}' in human_block, f"missing case for player {n}"
        assert f'{{player "{n}"}}' in human_block, f"missing player literal {n}"


def test_human_switch_is_keyed_from_id_attack_support(mod_root):
    text = _read(mod_root)
    human_block = text.split('handoff_human')[1].split('handoff_gate')[0]
    assert 'id_attack_support$' in human_block
    assert human_block.count('id_attack_support$') == 16


def test_mate_switch_is_keyed_from_the_runtime_resolved_mate_id(mod_root):
    text = _read(mod_root)
    mate_block = text.split('handoff_mate')[1]
    assert 'allied_support_cmd_mate_id$' in mate_block
    assert mate_block.count('allied_support_cmd_mate_id$') == 16
    assert '{player "1"}' in mate_block  # case 1 exists, but only as one of sixteen
    for n in range(1, 17):
        assert f'{{player "{n}"}}' in mate_block, f"missing mate player literal {n}"


def test_gate_blocks_on_the_continue_variable(mod_root):
    text = _read(mod_root)
    assert 'allied_support_cmd_fow_continue$' in text
    gate_block = text.split('handoff_gate')[1].split('handoff_mate')[0]
    assert 'allied_support_cmd_fow_continue$' in gate_block


def test_gate_auto_release_is_recorded_and_bounded(mod_root):
    text = _read(mod_root)
    gate_block = text.split('handoff_gate')[1].split('handoff_mate')[0]
    assert 'allied_support_cmd_gate_auto$' in gate_block, \
        "an auto-release must be recorded so it is never counted as a pass"
    assert '{time 60' in gate_block, "expected the 60s observation backstop"


def test_mate_transfer_cannot_precede_the_gate(mod_root):
    """Stage ordering: mate transfer requires stage 50 (continue received)."""
    text = _read(mod_root)
    mate_block = text.split('handoff_mate')[1]
    condition = mate_block.split('{actions')[0]
    assert '{value 50}' in condition, \
        "mate transfer must be conditioned on stage 50, not reachable from stage 40"


def test_settle_is_three_seconds_before_stage_seventy(mod_root):
    text = _read(mod_root)
    mate_block = text.split('handoff_mate')[1]
    assert '{time 3.0}' in mate_block
    assert '{value 70}' in mate_block


def test_handoff_never_applies_lua_guard_tags(mod_root):
    text = _read(mod_root)
    assert 'tag_add _lua_mi' not in text
    assert 'tag_add _lua_ignore' not in text


def test_handoff_has_no_banned_strings(mod_root):
    text = _read(mod_root).lower()
    for banned in BANNED_SUBSTRINGS:
        assert banned not in text, f"banned substring {banned}"
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python -m pytest tests/test_allied_support_handoff.py -v`
Expected: FAIL — `FileNotFoundError` for `allied_support_handoff.inc`.

- [ ] **Step 3: Write the handoff file**

Create `resource/map/multi/allied_support_handoff.inc`. The 1–16 switches are written out in
full because the engine will not accept a variable in the `{player}` field.

```
; Allied support command - ownership and Mate handoff.
;
; Reuses the mechanism confirmed working in #110, unchanged: tag the actor, keep it
; human-controlled briefly, run a literal 1-16 {player} switch keyed from
; id_attack_support$, switch to AI, wait 3 seconds, then move. The transfer is a
; tag-driven ownership switch, not a Lua call; the mate bot never issues it.
;
; Human ownership is NOT native Dynamic Conquest registration. Setting the owner to the
; human is an ownership operation only. Whether a cloned actor acquires genuine Conquest
; registration - and therefore terrain fog-of-war illumination - is the open question, and
; the gate below is the only evidence that settles it.
;
; THE GATE IS AN ENFORCED HOLD, NOT A PASS-THROUGH. The actor stops while still
; human-owned and does not advance until allied_support_cmd_fow_continue$ is set. A fast
; automatic transfer would destroy the single most important distinction in the design: if
; the isolated actor is already dark while human-owned, the clone birth has failed and the
; commander is irrelevant. That observation must be made deliberately, by eye, on a
; stationary actor. If no continue arrives within 60s the hold releases and records
; allied_support_cmd_gate_auto$ - an auto-release is never counted as a pass.
;
; These actors are never tagged _lua_mi or _lua_ignore. Either tag would place them in the
; commander's exclude set permanently and make them uncommandable.

	{"allied_support_cmd/handoff_human"
		{condition
			{terms
				{"1.cmp_i" {var "allied_support_cmd_stage$"} {op "=="} {value 20}}
			}
		}
		{actions
			{"switch"
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "1"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "2"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "3"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "4"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "5"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "6"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "7"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "8"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "9"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "10"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "11"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "12"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "13"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "14"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "15"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "16"}}}
			}
			{"entity_state"
				{selector {ignore_captured_by_user 0} {group {select {tag {tag allied_support_cmd_fresh}}}}}
				{tag_add allied_support_cmd_human}
				{tag_remove allied_support_cmd_fresh}
			}
			{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 30}}
		}
	}

	{"allied_support_cmd/handoff_gate"
		{condition
			{terms
				{"1.cmp_i" {var "allied_support_cmd_stage$"} {op "=="} {value 30}}
			}
		}
		{actions
			{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 40}}
			{"switch"
				{"case"
					{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
					{"timer" {mode show} {text "FOW GATE: actor human-owned and holding"}}
				}
				{"default"}
			}
			{"wait"
				{condition
					{terms
						{"1.cmp_i" {var "allied_support_cmd_fow_continue$"} {op "=="} {value 1}}
					}
				}
				{time 60.0}
			}
			{"switch"
				{"case"
					{condition {type cmp_i} {var "allied_support_cmd_fow_continue$"} {op "=="} {value 1}}
					{"set_i" {var "allied_support_cmd_gate_auto$"} {op "="} {value 0}}
				}
				{"default"
					{"set_i" {var "allied_support_cmd_gate_auto$"} {op "="} {value 1}}
				}
			}
			{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 50}}
		}
	}

	{"allied_support_cmd/handoff_mate"
		{condition
			{terms
				{"1.cmp_i" {var "allied_support_cmd_stage$"} {op "=="} {value 50}}
				{"2.cmp_i" {var "allied_support_cmd_mate_id$"} {op ">"} {value 0}}
			}
		}
		{actions
			{"switch"
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "1"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "2"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "3"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "4"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "5"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "6"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "7"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "8"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "9"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "10"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "11"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "12"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "13"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "14"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "15"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "16"}}}
			}
			{"actor_state"
				{selector {source standart} {ignore_captured_by_user 0} {tag allied_support_cmd_human}}
				{control AI}
			}
			{"entity_state"
				{selector {ignore_captured_by_user 0} {group {select {tag {tag allied_support_cmd_human}}}}}
				{tag_add allied_support_cmd_mate}
				{tag_remove allied_support_cmd_human}
			}
			{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 60}}
			{"delay" {time 3.0}}
			{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 70}}
		}
	}
```

- [ ] **Step 4: Run the tests to verify they pass**

Run: `python -m pytest tests/test_allied_support_handoff.py -v`
Expected: PASS, 10 passed.

- [ ] **Step 5: Run the whole suite**

Run: `python -m pytest tests/ -v`
Expected: PASS, all tests.

- [ ] **Step 6: Commit**

```bash
git add resource/map/multi/allied_support_handoff.inc tests/test_allied_support_handoff.py
git commit -m "feat: add allied support handoff with enforced FoW observation gate"
```

---

### Task 7: Resolve the mate player id at runtime

The handoff's mate switch requires `allied_support_cmd_mate_id$ > 0`. Nothing sets it yet.
It is published from the existing Lua slot, which is the only component that can read
identity.

**Files:**
- Modify: `resource/script/multiplayer/modes/attack_support.lua`
- Test: `tests/test_attack_support_lua_ordering.py` (extend)

**Interfaces:**
- Consumes: the existing `identity()` helper and `publishIdentity` in `attack_support.lua`.
- Produces: mission variable `allied_support_cmd_mate_id$`, set to the mate slot's own
  `playerId`, consumed by Task 6's `handoff_mate` trigger.

- [ ] **Step 1: Write the failing test**

Append to `tests/test_attack_support_lua_ordering.py`:

```python
def test_mate_id_is_published_from_the_slots_own_player_id(mod_root):
    text = _lua(mod_root)
    assert "allied_support_cmd_mate_id" in text, "mate id must be published for the MI handoff"
    assert re.search(r'SetVar\(\s*"allied_support_cmd_mate_id"', text), \
        "expected a SetVar publication of the mate id"


def test_mate_id_is_never_hardcoded(mod_root):
    """Lobby slot assignment varies; a literal 1 made earlier proofs contradictory."""
    text = _lua(mod_root)
    block_start = text.index("allied_support_cmd_mate_id")
    block = text[max(0, block_start - 300):block_start + 300]
    assert not re.search(r'allied_support_cmd_mate_id"\s*,\s*1\s*\)', block), \
        "mate id must come from identity, never the literal 1"
    assert "id.playerId" in block or "identity.playerId" in block


def test_mate_id_publication_does_not_read_forbidden_fields(mod_root):
    """spawnPointName / PlayerSpawnPoint null-deref natively on the extra Team A slot."""
    text = _lua(mod_root)
    assert "spawnPointName" not in text
    assert "PlayerSpawnPoint" not in text
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python -m pytest tests/test_attack_support_lua_ordering.py -v`
Expected: FAIL on `test_mate_id_is_published_from_the_slots_own_player_id`.

- [ ] **Step 3: Publish the mate id**

`publishIdentity` at line 136 currently reads:

```lua
local function publishIdentity(id)
	if id.attacking ~= true then return end
	local sc = scene()
	if not sc or not sc.SetVar then
		log("identity_publish_skipped", "Scene.SetVar_missing")
		return
	end
	sc:SetVar("id_attack_support", id.playerId)
	sc:SetVar("attack_support_ready", 1)
	-- MI waves are the working delivery path for attack support units.
	sc:SetVar("attack_support_use_mi", 1)
	log("identity_published", "id_attack_support", id.playerId, "mi_waves", 1)
end
```

Change it to:

```lua
local function publishIdentity(id)
	if id.attacking ~= true then return end
	local sc = scene()
	if not sc or not sc.SetVar then
		log("identity_publish_skipped", "Scene.SetVar_missing")
		return
	end
	sc:SetVar("id_attack_support", id.playerId)
	sc:SetVar("attack_support_ready", 1)
	-- MI waves are the working delivery path for attack support units.
	sc:SetVar("attack_support_use_mi", 1)
	-- The MI handoff needs this slot's own player id as a literal it can switch on.
	-- Never hardcode it: lobby slot assignment varies between runs, and a hardcoded
	-- value silently disabled this route whenever the human did not hold slot one.
	sc:SetVar("allied_support_cmd_mate_id", id.playerId)
	log("identity_published", "id_attack_support", id.playerId, "mi_waves", 1,
		"mate_id", id.playerId)
end
```

Note the early return on `id.attacking ~= true`: nothing is published on a defense mission.
That is correct for Q1 and is why Q2 will need its own publication path later.

- [ ] **Step 4: Run the tests to verify they pass**

Run: `python -m pytest tests/test_attack_support_lua_ordering.py -v`
Expected: PASS, 8 passed.

- [ ] **Step 5: Run the whole suite**

Run: `python -m pytest tests/ -v`
Expected: PASS.

- [ ] **Step 6: Commit**

```bash
git add resource/script/multiplayer/modes/attack_support.lua tests/test_attack_support_lua_ordering.py
git commit -m "feat: publish runtime-resolved mate player id for the MI handoff"
```

---

## Deliberate deviation from the spec: birth is not wave-driven here

The spec describes birth as consuming a composition request from the existing Q1 wave clock.
This plan instead fires **one** birth, once, gated only on `allied_support_cmd_enable$` and a
published `id_attack_support$`.

That is intentional. The deliverable is a fog-of-war reading on a single isolated actor, and
the spec's own staging calls for exactly that. Driving birth from the wave clock would add the
schedule, the composition pool selection, the level budget, and the live-unit cap as
confounders to a one-bit observation — a stage-3 FAIL would then have five candidate
explanations instead of one.

Wave-clock integration (budget, cadence, cap, composition variety) belongs to the commander
plan, once birth is known to produce an actor worth scheduling.

---

### Task 8: Live run — the fog-of-war observation

This task produces the plan's actual deliverable: a recorded yes/no on whether a cloned actor
illuminates terrain while human-owned. It is manual, because the observation is visual.

**Files:**
- Create: `docs/superpowers/results/2026-08-17-fow-gate-run-01.md`
- Modify: none.

**Interfaces:**
- Consumes: everything from Tasks 2–7.
- Produces: a stage-3 verdict that determines whether the commander plan is written at all.

- [ ] **Step 1: Confirm the structural suite is green**

Run: `python -m pytest tests/ -v`
Expected: PASS, all tests. Do not start a live run on a red suite.

- [ ] **Step 2: Confirm the mod is enabled and loads last**

Launch the game, open the Mods menu, and confirm this submod is enabled and ordered after
West 81 and Code:X.

Then check the log for a silent deactivation:

```bash
grep -n "disable mod\|mods \[" "$LOCALAPPDATA/digitalmindsoft/gates of hell/log/game.log"
```

Expected: no `disable mod mod_3636883799` line. If present, the submod was silently disabled
after the file changes — re-enable it before diagnosing anything else. Zero
`CODEX_ATTACK_SUPPORT` lines with this symptom means the mod never ran.

- [ ] **Step 3: Arm candidate A only**

Set the enables for this run. `allied_support_cmd_enable$` must be 1 and
`allied_support_cmd_spawn_probe$` must be 0 — if both are 1 the birth refuses to run with
fail code 5, by design.

The simplest arming for a manual run is a temporary `{"set_i"}` in the `birth_init` trigger:

```
			{"set_i" {var "allied_support_cmd_enable$"} {op "="} {value 1}}
			{"set_i" {var "support_debug$"} {op "="} {value 1}}
```

Add both lines to `allied_support_cmd/birth_init` in
`resource/map/multi/allied_support_birth.inc`. These are run-only and are reverted in Step 8.

- [ ] **Step 4: Run a human attack mission on one map**

Start a Dynamic Conquest **attack** mission on `fields`. The birth arms once
`id_attack_support$` is published, so wait for the identity line in the log.

- [ ] **Step 5: Observe stage by stage**

Watch the on-screen timer (now enabled by `support_debug$`) and the log. Record each:

| Stage | Meaning | Observable |
|---|---|---|
| 10 | clone dispatched | a squad appears at the rear pad |
| 20 | arrival tagged | stage advances past 10 within ~3s |
| 90 + fail 2 | **arrival not tagged** | stage jumps to 90; the clone exists but is unmarked |
| 30 | human-owned | the squad shows as yours |
| 40 | holding at the gate | the FoW gate timer is displayed |

**If stage reaches 90 with fail code 2, stop.** The arrival-tagging mechanism does not fire
for a teleported infantry actor. Record it and go to Task 9. Do not attempt to select the
untagged clone by any other means.

- [ ] **Step 6: Make the fog-of-war observation — the hard gate**

While stage is 40 and the actor is stationary and human-owned, look at the minimap and the
terrain around the actor.

Record one of:
- **PASS** — the isolated actor illuminates terrain around itself.
- **FAIL** — the actor is dark; terrain around it is not revealed.

A FAIL here means candidate A has failed and the commander is irrelevant. This is the whole
point of the plan.

- [ ] **Step 7: Release the gate and observe again**

Release the hold by setting `allied_support_cmd_fow_continue$` to 1. If you have no in-game
way to set it, let the 60s backstop release it — but note that
`allied_support_cmd_gate_auto$` will read 1, and an auto-release does not count as a stage-3
pass.

After stage 70 (3s settle past mate ownership), observe fog of war again and record whether
illumination was retained.

- [ ] **Step 8: Revert the run-only arming**

Remove the two `{"set_i"}` lines added in Step 3 from
`resource/map/multi/allied_support_birth.inc`, so the shipped default is disabled and silent.

Run: `python -m pytest tests/ -v`
Expected: PASS.

- [ ] **Step 9: Record the result**

Create `docs/superpowers/results/2026-08-17-fow-gate-run-01.md` with exactly these fields:

```markdown
# FoW Gate Run 01

- Date:
- Map:
- Birth mechanism: clone (candidate A)
- Spawn probe enabled: no
- Stage reached:
- Fail code:
- Arrival tagged: yes / no
- FoW at stage 3 (human-owned, stationary): PASS / FAIL
- Gate released: manually / auto-released (auto does not count as a pass)
- FoW at stage 5 (mate-owned, after 3s settle): retained / lost
- Notes:
```

- [ ] **Step 10: Commit**

```bash
git add docs/superpowers/results/2026-08-17-fow-gate-run-01.md resource/map/multi/allied_support_birth.inc
git commit -m "docs: record FoW gate run 01 result"
```

---

### Task 9: The `{"spawn"}` probe — only if candidate A failed

Do **not** run this task if Task 8 recorded a stage-3 PASS. It exists only because candidate A
might fail at stage 1 (arrival not tagged) or stage 3 (dark while human-owned).

The two mechanisms are mutually exclusive: `allied_support_cmd_enable$` must be 0 for this run.

**Files:**
- Create: `resource/map/multi/allied_support_spawn_probe.inc`
- Modify: `resource/map/multi/dcg_[cwa71]_fields/campaign_capture_the_flag.mi` (one map only)
- Create: `docs/superpowers/results/2026-08-17-fow-gate-run-02.md`
- Test: `tests/test_spawn_probe.py`

**Interfaces:**
- Consumes: `allied_support_cmd_spawn_probe$`, `allied_support_cmd_stage$`,
  `allied_support_cmd_fail$` from Task 2.
- Produces: the same `allied_support_cmd_fresh` tag contract as Task 5, so Task 6's handoff
  is reused unchanged.

- [ ] **Step 1: Write the failing test**

Create `tests/test_spawn_probe.py`:

```python
PROBE_INC = ("resource", "map", "multi", "allied_support_spawn_probe.inc")
BANNED_SUBSTRINGS = ["tmai", "p013", "bus_magic", "fpc1", "fpc2", "fpc3", "fpc4", "fpc5"]


def _read(mod_root):
    path = mod_root
    for part in PROBE_INC:
        path = path / part
    return path.read_text(encoding="utf-8", errors="replace")


def test_probe_uses_the_spawn_action(mod_root):
    text = _read(mod_root)
    assert '{"spawn"' in text
    assert "{clone}" not in text, "the probe must not clone; that is candidate A"


def test_probe_is_interlocked_against_candidate_a(mod_root):
    text = _read(mod_root)
    assert "allied_support_cmd_spawn_probe$" in text
    assert "allied_support_cmd_enable$" in text
    assert "{value 5}" in text, "fail code 5 (both mechanisms enabled) must be reachable"


def test_probe_produces_the_same_tag_contract(mod_root):
    """So the handoff is reused unchanged."""
    text = _read(mod_root)
    assert "allied_support_cmd_fresh" in text
    assert "{value 20}" in text


def test_probe_is_limited_to_one_map(map_files):
    included = [
        p.parent.name for p in map_files
        if '(include "../allied_support_spawn_probe.inc")'
        in p.read_text(encoding="utf-8", errors="replace")
    ]
    assert included == ["dcg_[cwa71]_fields"], f"probe must be one map only, found {included}"


def test_probe_has_no_banned_strings(mod_root):
    text = _read(mod_root).lower()
    for banned in BANNED_SUBSTRINGS:
        assert banned not in text
```

- [ ] **Step 2: Run the tests to verify they fail**

Run: `python -m pytest tests/test_spawn_probe.py -v`
Expected: FAIL — `FileNotFoundError` for `allied_support_spawn_probe.inc`.

- [ ] **Step 3: Write the probe**

Create `resource/map/multi/allied_support_spawn_probe.inc`:

```
; Allied support command - {"spawn"} birth probe. THROWAWAY.
;
; {"spawn"} is a true creation primitive needing no source prototype, but every shipped
; use in this mod, Code:X, and vanilla births only non-combat helpers
; (conquest_spawn_helper, conquest_spawn_indicator, artillery_barrage_rocket), and the
; action carries no {player} parameter in any precedent. This probe exists to find out
; whether it can birth a soldier at all.
;
; Mutually exclusive with candidate A: if allied_support_cmd_enable$ is also 1 this refuses
; to run with fail code 5, so a fog-of-war observation is always attributable to one
; mechanism.
;
; It produces the same allied_support_cmd_fresh tag contract as the clone birth, so
; allied_support_handoff.inc is reused unchanged.
;
; One map only. Delete this file once the question is answered either way.

	{"allied_support_cmd/spawn_probe"
		{condition
			{terms
				{"1.cmp_i" {var "allied_support_cmd_spawn_probe$"} {op "=="} {value 1}}
				{"2.cmp_i" {var "allied_support_cmd_stage$"} {op "=="} {value 0}}
				{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
			}
		}
		{actions
			{"switch"
				{"case"
					{condition {type cmp_i} {var "allied_support_cmd_enable$"} {op "=="} {value 1}}
					{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 90}}
					{"set_i" {var "allied_support_cmd_fail$"} {op "="} {value 5}}
				}
				{"default"
					{"spawn"
						{entity "mp/nato/2022s/usmc_rifleman"}
						{waypoint "31"}
					}
					{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 10}}
					{"delay" {time 3.0}}
					{"switch"
						{"case"
							{condition {type entities}
								{selector
									{ignore_captured_by_user 0}
									{group {select {tag {tag allied_support_cmd_fresh}}}}
								}
								{op ">"} {value 0}
							}
							{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 20}}
						}
						{"default"
							{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 90}}
							{"set_i" {var "allied_support_cmd_fail$"} {op "="} {value 2}}
						}
					}
				}
			}
		}
	}
```

Note: the probe reuses birth pad `31`, so its arrival is tagged by the same pad `{commands}`
block. If Task 8 failed at stage 1 (arrival not tagged), that failure applies here too and the
probe cannot distinguish itself — in that case, record stage 90 fail 2 and stop, because the
blocker is arrival tagging, not the birth primitive.

- [ ] **Step 4: Add the include to the fields map only**

In `resource/map/multi/dcg_[cwa71]_fields/campaign_capture_the_flag.mi`, immediately before the
existing `(include "../dcg_script.inc")` line, add:

```
			(include "../allied_support_spawn_probe.inc")
```

Do not run `tools/patch_allied_support_maps.py` for this — the probe is deliberately one map.

- [ ] **Step 5: Run the tests to verify they pass**

Run: `python -m pytest tests/test_spawn_probe.py -v`
Expected: PASS, 5 passed.

- [ ] **Step 6: Run the probe live**

Repeat Task 8 Steps 2–8, but arm `allied_support_cmd_spawn_probe$` to 1 and leave
`allied_support_cmd_enable$` at 0.

- [ ] **Step 7: Record the result**

Create `docs/superpowers/results/2026-08-17-fow-gate-run-02.md` using the same field list as
Task 8 Step 9, with `Birth mechanism: spawn (probe)` and `Spawn probe enabled: yes`.

- [ ] **Step 8: Commit**

```bash
git add resource/map/multi/allied_support_spawn_probe.inc "resource/map/multi/dcg_[cwa71]_fields/campaign_capture_the_flag.mi" tests/test_spawn_probe.py docs/superpowers/results/2026-08-17-fow-gate-run-02.md
git commit -m "test: probe {\"spawn\"} birth after clone candidate result"
```

---

## Known unknowns this plan will resolve or expose

These are genuine uncertainties, not gaps in the plan. Each is surfaced by a specific step
rather than assumed away.

1. **Does `{commands}` fire for a teleported infantry actor?** Verified only for arriving
   aircraft. Exposed by Task 8 Step 5 as stage 90 / fail 2.
2. **Does an infantry squad survive `{clone}` at all?** Aircraft and their passengers are
   verified; a rifle team is not. Exposed by Task 8 Step 5 as no squad appearing at the pad.
3. **Does the clone inherit `{Able "-select"}` from the prototype?** The pool entries carry
   it. If the clone is unselectable while human-owned, note it in the run record — it does not
   block the fog-of-war reading, but it matters for the commander plan.
4. **Does `{clone}` produce one body or the whole parked squad?** The dispatch uses
   `{amount 1}`, but the arrival tag applies to whatever arrives. Record the actual body count
   at stage 20.
5. **Whether `{"wait"}` with a `{time}` bound is the right hold primitive.** If the engine
   rejects it, replace the gate body with a polled `{"switch"}` on
   `allied_support_cmd_fow_continue$` plus a delay loop, keeping stage 40 and the
   `allied_support_cmd_gate_auto$` record intact.

## Follow-up plan, not written yet

`allied_support_command.inc` and `allied_support_command.lua` — the commander and its strategy
publisher — are deliberately deferred until Task 8 returns a stage-3 verdict. When written,
that plan consumes the `allied_support_cmd_mate` tag produced by Task 6, targets flags via
`flag_point_campaign_N` and the `{tag flag}` idiom with `{exclude {state inactive}}`, and
retires Q1's existing direct-command logic. Q2 reuse comes after that, as its own step.
