from __future__ import annotations

import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VARS = ROOT / "resource/map/multi/dcg_vars.inc"
WAVES = ROOT / "resource/map/multi/allied_attack_waves.inc"
BRAIN = ROOT / "resource/script/multiplayer/modes/attacker_mate_brain.lua"
BOT_MAIN = ROOT / "resource/script/multiplayer/bot.main.lua"
MAP_ROOT = ROOT / "resource/map/multi"

NEW_VARS = (
    "allied_attack_enabled",
    "allied_attack_started",
    "allied_attack_wave_num",
    "allied_attack_owner_fail",
    "allied_attack_retasked",
)


def strip_mi_comments(text: str) -> str:
    """MI comments start with ';' and run to end of line."""
    return "\n".join(line.split(";", 1)[0] for line in text.splitlines())


def block_at(text: str, start: int) -> str:
    """Return the balanced {...} block that opens at or after `start`."""
    open_at = text.index("{", start)
    depth = 0
    for pos in range(open_at, len(text)):
        char = text[pos]
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                return text[open_at:pos + 1]
    raise AssertionError("unbalanced block starting at %d" % open_at)


class AlliedAttackWavesTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.brain = BRAIN.read_text(encoding="utf-8")
        cls.bot_main = BOT_MAIN.read_text(encoding="utf-8")

    # (a) state declaration -------------------------------------------------

    def test_new_wave_vars_are_declared(self) -> None:
        for name in NEW_VARS:
            self.assertIn('{"%s"}' % name, self.vars)
        # The attack-mate identity vars this engine depends on must stay declared.
        self.assertIn('{"id_attacker_mate"}', self.vars)
        self.assertIn('{"attacker_mate_ready"}', self.vars)

    # (b) mission-side wave engine -----------------------------------------

    def test_wave_engine_gates_on_attack_side_readiness(self) -> None:
        for marker in (
            '{var "allied_attack_enabled$"}',
            '{var "user_is_defender$"}',
            '{var "attacker_mate_ready$"}',
            '{var "id_attacker_mate$"}',
            '{var "allied_attack_started$"}',
            "ALLIED ATTACK INIT",
        ):
            self.assertIn(marker, self.waves)

    def test_every_trigger_is_namespaced(self) -> None:
        for name in (
            "allied_attack/init",
            "allied_attack/wave_clock",
            "allied_attack/wave_spawn",
            "allied_attack/retask",
        ):
            self.assertIn('{"%s"' % name, self.waves)
        # No stray trigger names from the defense engine or the probe.
        self.assertNotIn("allied_support/", self.waves)
        self.assertNotIn("attack_mate/", self.waves)

    def test_wave_clock_has_cap_defer_and_exhaustion(self) -> None:
        for marker in (
            "ALLIED ATTACK NEAR CAP DEFER",
            "ALLIED ATTACK WAVES EXHAUSTED",
            "{condition {type rand} {value 0.2}}",
            '{"delay" {time 240}}',
            '{"delay" {time 480}}',
            '{"trigger" {name "allied_attack/wave_clock"}}',
        ):
            self.assertIn(marker, self.waves)

        # Exhaustion must not re-arm the clock: the last thing in that case block
        # is the diagnostic timer.
        exhausted = self.waves.index("ALLIED ATTACK WAVES EXHAUSTED")
        case_start = self.waves.rindex('{"case"', 0, exhausted)
        case_block = block_at(self.waves, case_start)
        self.assertNotIn("allied_attack/wave_clock", case_block)

        # Near-cap defer must re-arm without spawning.
        defer = self.waves.index("ALLIED ATTACK NEAR CAP DEFER")
        defer_case = block_at(self.waves, self.waves.rindex('{"case"', 0, defer))
        self.assertIn('{"trigger" {name "allied_attack/wave_clock"}}', defer_case)
        self.assertNotIn("allied_attack/wave_spawn", defer_case)

    def test_ownership_switch_covers_all_slots_with_no_fallback(self) -> None:
        for slot in range(1, 17):
            self.assertIn(
                '{condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value %d}}' % slot,
                self.waves,
            )
            self.assertIn('{player "%d"}' % slot, self.waves)

        self.assertIn("ALLIED ATTACK OWNER UNRESOLVED", self.waves)
        self.assertIn('{var "allied_attack_owner_fail$"}', self.waves)

        unresolved = self.waves.index("ALLIED ATTACK OWNER UNRESOLVED")
        default_start = self.waves.rindex('{"default"', 0, unresolved)
        default_block = block_at(self.waves, default_start)
        # The default case reports only. No transfer of any kind.
        self.assertNotIn('"player"', default_block)
        self.assertNotIn("{player ", default_block)
        self.assertIn("ALLIED ATTACK OWNER UNRESOLVED", default_block)
        self.assertIn('{var "allied_attack_owner_fail$"}', default_block)

        # The probe's hardcoded P3 crutch must never appear here.
        self.assertNotIn("FALLBACK P3", self.waves)

    def test_wave_spawn_clones_pool_and_dispatches(self) -> None:
        for marker in (
            "{tag allied_support_template}",
            "{tag_add allied_attack_unit}",
            '{target_waypoint "allied_support_entry"}',
            "{clone}",
            "{control AI}",
            "{remove select}",
            "{tag_add allied_attack_active}",
            "{tag_remove allied_attack_unit}",
            '{var "allied_attack_wave_num$"}',
            "ALLIED ATTACK WAVE DISPATCHED",
            "{drop orders}",
            "{action advance}",
            "{tag fpc1}",
            '{function "fpc_inf_to_flag1"}',
        ):
            self.assertIn(marker, self.waves)

        clone = self.waves.index("{clone}")
        owner = self.waves.index('{player "1"}')
        swap = self.waves.index("{tag_add allied_attack_active}")
        dispatched = self.waves.index("ALLIED ATTACK WAVE DISPATCHED")
        leg1 = self.waves.index('{function "fpc_inf_to_flag1"}')
        self.assertLess(clone, owner)
        self.assertLess(owner, swap)
        self.assertLess(swap, dispatched)
        self.assertLess(dispatched, leg1)

    def test_defense_side_tags_are_never_reused(self) -> None:
        for forbidden in (
            "allied_wave_fresh",
            "{tag allied_support}",
            "_ai_defender",
            "allied_support_src",
            "attack_mate_probe",
            '{var "id_defenderbot$"}',
        ):
            self.assertNotIn(forbidden, self.waves)

    def test_retask_is_guarded_and_documented_as_fallback(self) -> None:
        for marker in (
            '{var "allied_attack_retasked$"}',
            '{zone "fpc1"}',
            "{tag fpc2}",
            "ALLIED ATTACK FPC1 REACHED",
            "ALLIED ATTACK RETASKED TO FPC2",
            "attacker_mate_brain.lua",
        ):
            self.assertIn(marker, self.waves)

    # (c) inertness ---------------------------------------------------------

    def test_no_map_includes_the_new_wave_engine_yet(self) -> None:
        maps = sorted(MAP_ROOT.glob("*/campaign_capture_the_flag.mi"))
        self.assertGreaterEqual(len(maps), 14)
        for mi_path in maps:
            text = mi_path.read_text(encoding="utf-8")
            with self.subTest(map=mi_path.parent.name):
                self.assertNotIn("allied_attack_waves.inc", text)

    # (d) router untouched --------------------------------------------------

    def test_router_does_not_reference_the_brain(self) -> None:
        self.assertNotIn("attacker_mate_brain", self.bot_main)
        # The existing probe route must still be there — this branch changes nothing.
        self.assertIn(
            'safeRequire("resource/script/multiplayer/modes/attacker_mate")',
            self.bot_main,
        )

    # (e) strategic brain ---------------------------------------------------

    def test_brain_respects_squad_guards_and_command_budget(self) -> None:
        for marker in (
            'local PREFIX = "CODEX_ATTACK_MATE_BRAIN"',
            '"_lua_mi"',
            '"_lua_ignore"',
            '"_lua_alert"',
            '"repairing"',
            '"dead"',
            "BotApi.Scene",
            "MAX_ORDERS_PER_PULSE",
            "ORDER_REFRESH_QUANTS",
            "order_sent",
            "order_failed",
            "pcall(",
        ):
            self.assertIn(marker, self.brain)

    def test_brain_is_deterministic_and_command_limited(self) -> None:
        self.assertNotIn("math.random", self.brain)
        for forbidden in (
            "Spawn(",
            "SpawnAt(",
            "Purchase(",
            "GameModeSpawnUnit(",
            "require(",
            "Teleport(",
        ):
            self.assertNotIn(forbidden, self.brain)
        # Exactly two command verbs are permitted.
        self.assertIn("cmd:CaptureFlag(squad, flagName)", self.brain)
        self.assertIn("cmd:SeekAndDestroy(squad)", self.brain)

    def test_brain_is_gated_behind_allied_attack_enabled(self) -> None:
        for marker in (
            'sc:GetVar("allied_attack_enabled")',
            "brain_disarmed",
            "brain_armed",
            "local enabled, reason = isEnabled()",
        ):
            self.assertIn(marker, self.brain)

        gate = self.brain.index("local enabled, reason = isEnabled()")
        subscribe = self.brain.index("ev:Subscribe(")
        self.assertLess(gate, subscribe)

    # (f) delimiter balance -------------------------------------------------

    def test_delimiters_are_balanced(self) -> None:
        code = strip_mi_comments(self.waves)
        self.assertEqual(code.count("{"), code.count("}"))
        self.assertEqual(code.count("("), code.count(")"))
        self.assertEqual(self.brain.count("("), self.brain.count(")"))


if __name__ == "__main__":
    unittest.main()
