from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
PROBE = ROOT / "resource/script/multiplayer/modes/native_support_birth_probe.lua"
ATTACK = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
UNIT = ROOT / "resource/set/multiplayer/units/conquest/native_support_rusa.set"
GAME = ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"


class NativeSupportBirthFoWTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.probe = PROBE.read_text(encoding="utf-8")
        cls.attack = ATTACK.read_text(encoding="utf-8")
        cls.unit = UNIT.read_text(encoding="utf-8")
        cls.game = GAME.read_text(encoding="utf-8")

    def test_diagnostic_unit_is_stage_zero_inside_conquest_unitset(self) -> None:
        self.assertIn("name(codex_native_support_rifle)", self.unit)
        self.assertIn("min_stage(0)", self.unit)
        self.assertIn("side(rusa)", self.unit)
        self.assertIn("period(2022s)", self.unit)

    def test_probe_hard_gates_native_spawn_on_positive_availability(self) -> None:
        check = self.probe.find("if not unitAvailable(cmd) then")
        spawn = self.probe.find("cmd:SpawnAt(RUSA_UNIT")
        self.assertGreater(check, 0)
        self.assertGreater(spawn, check)
        gated = self.probe[check:spawn]
        self.assertIn('"native_call", "suppressed"', gated)
        self.assertIn("return", gated)

    def test_probe_requires_gamespawn_before_tmai_style_settle_and_order(self) -> None:
        self.assertIn("ev:Subscribe(ev.GameSpawn", self.probe)
        self.assertIn('"native_birth_confirmed", true', self.probe)
        self.assertIn("SETTLE_MS = 3000", self.probe)
        self.assertIn("cmd:CaptureFlag", self.probe)
        self.assertIn("cmd:SeekAndDestroy", self.probe)

    def test_legacy_player_zero_support_is_inert_in_probe(self) -> None:
        for expected in (
            'setVar("attack_support_ready", 0)',
            'setVar("attack_support_use_mi", 0)',
            'setVar("attack_support_motor_left", 0)',
            'setVar("attack_support_hmmwv_left", 0)',
            'setVar("attack_support_motor_test", 0)',
            'setVar("transport_as_done", 1)',
        ):
            self.assertIn(expected, self.probe)

    def test_research_switch_routes_only_attack_support_runtime(self) -> None:
        prefix = self.attack[:600]
        self.assertIn("NATIVE_SUPPORT_BIRTH_RESEARCH = true", prefix)
        self.assertIn('require("resource/script/multiplayer/modes/native_support_birth_probe")', prefix)
        self.assertIn("return", prefix)
        # Keep production controller source present so the branch is easy to revert/audit.
        self.assertIn("local function enforceIfvOnlyTransport()", self.attack)

    def test_enemy_and_human_unitset_authority_is_not_replaced(self) -> None:
        # Explicit regression against rejected #104: no global 2022s/custom unitset swap.
        self.assertGreaterEqual(self.game.count('{value "conquest"}'), 2)
        self.assertNotIn('{value "2022s"}', self.game)
        self.assertNotIn("codex_conquest_native", self.game)
        self.assertIn("{aiTeamPlayers 1}", self.game)


if __name__ == "__main__":
    unittest.main()
