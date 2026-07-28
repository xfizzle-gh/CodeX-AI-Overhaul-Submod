from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
LUA = ROOT / "resource/script/multiplayer/modes/conquest.lua"
TEST = ROOT / "tests/test_conquest_defender_bot.py"
WORKFLOW = ROOT / ".github/workflows/apply-wave-size-reduction.yml"

lua = LUA.read_text(encoding="utf-8")

anchor = "enableWaveCounter = true\n"
insert = (
    "enableWaveCounter = true\n\n"
    "-- Global reduction for normal calculated waves. Allied 1-3 support waves return early and are unchanged.\n"
    "local NormalWaveSizeScale = 0.85\n"
)
if "local NormalWaveSizeScale = 0.85" not in lua:
    if lua.count(anchor) != 1:
        raise RuntimeError(f"Expected exactly one wave-counter anchor, found {lua.count(anchor)}")
    lua = lua.replace(anchor, insert, 1)

old_formula = "waveUnitTotal = math.max(3, math.round(rawWaveTotal * ActiveDifficultySettings.waveScale))"
new_formula = "waveUnitTotal = math.max(3, math.round(rawWaveTotal * ActiveDifficultySettings.waveScale * NormalWaveSizeScale))"
if new_formula not in lua:
    if lua.count(old_formula) != 1:
        raise RuntimeError(f"Expected exactly one normal wave formula, found {lua.count(old_formula)}")
    lua = lua.replace(old_formula, new_formula, 1)

old_debug = 'if printDebug then print("Print: waveUnitTotal", waveUnitTotal, "waveNumber", waveNumber, "isAlliedDefenderBot", isAlliedDefenderBot) end'
new_debug = 'if printDebug then print("Print: waveUnitTotal", waveUnitTotal, "waveNumber", waveNumber, "normalWaveSizeScale", NormalWaveSizeScale, "isAlliedDefenderBot", isAlliedDefenderBot) end'
if new_debug not in lua:
    if lua.count(old_debug) != 1:
        raise RuntimeError(f"Expected exactly one normal wave debug line, found {lua.count(old_debug)}")
    lua = lua.replace(old_debug, new_debug, 1)

LUA.write_text(lua, encoding="utf-8")

test = TEST.read_text(encoding="utf-8")
method = '''\n    def test_normal_calculated_waves_use_global_fifteen_percent_reduction(self) -> None:\n        self.assertIn("local NormalWaveSizeScale = 0.85", self.source)\n        self.assertIn(\n            "rawWaveTotal * ActiveDifficultySettings.waveScale * NormalWaveSizeScale",\n            self.source,\n        )\n        allied_branch = self.source.index(\n            "if isAlliedDefenderBot and waveNumber > 0 then"\n        )\n        allied_return = self.source.index("\\t\\treturn", allied_branch)\n        normal_formula = self.source.index(\n            "rawWaveTotal * ActiveDifficultySettings.waveScale * NormalWaveSizeScale"\n        )\n        self.assertLess(allied_return, normal_formula)\n        self.assertIn("Min_AlliedSupport = 1", self.source)\n        self.assertIn("Max_AlliedSupport = 3", self.source)\n'''
if "test_normal_calculated_waves_use_global_fifteen_percent_reduction" not in test:
    marker = '\n\nif __name__ == "__main__":\n'
    if test.count(marker) != 1:
        raise RuntimeError(f"Expected unittest main marker once, found {test.count(marker)}")
    test = test.replace(marker, method + marker, 1)
    TEST.write_text(test, encoding="utf-8")

# Remove the one-shot patch machinery from the resulting branch commit.
Path(__file__).unlink()
if WORKFLOW.exists():
    WORKFLOW.unlink()
