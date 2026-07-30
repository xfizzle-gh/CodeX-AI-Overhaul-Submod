from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GAME_SET = ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"
BOT_MAIN = ROOT / "resource/script/multiplayer/bot.main.lua"
ATTACK_SUPPORT = ROOT / "resource/script/multiplayer/modes/attack_support.lua"
VARS = ROOT / "resource/map/multi/dcg_vars.inc"
WAVES = ROOT / "resource/map/multi/attack_support_waves.inc"
TEMPLATES = ROOT / "resource/map/multi/attack_support_templates.inc"
DEPLOY = ROOT / "tools/deploy_attack_support_probe.ps1"

# Files retired with the productionised wave engine. The Lua brain went with them:
# Scene.Squads is empty for MI-delivered units, so it could never see a squad to
# order, and the inert allied_attack_waves.inc skeleton was never wired to a map.
RETIRED = (
    ROOT / "resource/map/multi/attack_support_probe.inc",
    ROOT / "resource/map/multi/allied_attack_waves.inc",
    ROOT / "resource/script/multiplayer/modes/attack_support_brain.lua",
    ROOT / "tests/test_allied_attack_waves.py",
)

# Where each map's attack_support_entry_<side> waypoint sits, as a fraction of that
# side's spawn centroid. 0,0 is the map centre, so scaling the centroid down pulls
# the arrival point inward. 1.00 puts it exactly on the spawn centroid - the
# map-edge spawn area. This is the balance knob: lower it to have attack support waves
# arrive further forward, and regenerate the 28 waypoints from the spawn markers.
EDGE_FACTOR = 1.00

# composition trigger, command value, pool tag, infantry taken per wave
COMPOSITIONS = (
    ("comp_usmc", 1, "attack_support_inf_usmc", 5),
    ("comp_1ad", 2, "attack_support_inf_1ad", 5),
    ("comp_acav", 3, "attack_support_inf_1ad", 4),
    ("comp_pzgren", 4, "attack_support_inf_pzgd", 6),
)


def strip_comments(text: str) -> str:
    """MI comment-stripped view. Headers quote bad forms as cautionary examples,
    so every structural count must run on code only."""
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
                return text[open_at : pos + 1]
    raise AssertionError("unbalanced block starting at %d" % open_at)


def trigger_block(code: str, name: str) -> str:
    return block_at(code, code.index('{"attack_support/%s"' % name))


def define_body(code: str, name: str) -> str:
    """Return the whole balanced (define "name" ... ) form, calls included."""
    open_at = code.index('(define "%s"' % name)
    depth = 0
    for pos in range(open_at, len(code)):
        char = code[pos]
        if char == "(":
            depth += 1
        elif char == ")":
            depth -= 1
            if depth == 0:
                return code[open_at : pos + 1]
    raise AssertionError("unbalanced define %s" % name)


class AttackSupportSlotProofTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.game_set = GAME_SET.read_text(encoding="utf-8")
        cls.bot_main = BOT_MAIN.read_text(encoding="utf-8")
        cls.attack_support = ATTACK_SUPPORT.read_text(encoding="utf-8")
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.waves = WAVES.read_text(encoding="utf-8")
        cls.templates = TEMPLATES.read_text(encoding="utf-8")
        cls.deploy = DEPLOY.read_text(encoding="utf-8")
        cls.code = strip_comments(cls.waves)

    def test_team_a_requests_exactly_one_ai_mate_slot(self) -> None:
        self.assertEqual(self.game_set.count("{aiTeamPlayers 1}"), 1)
        team_a = self.game_set.index('{"a"')
        team_b = self.game_set.index('{"b"', team_a)
        self.assertIn("{aiTeamPlayers 1}", self.game_set[team_a:team_b])
        self.assertIn("{minTeamSlots 7}", self.game_set[team_a:team_b])

    def test_router_routes_team_a_mate_regardless_of_first_player_id(self) -> None:
        for marker in (
            'local ROUTER_PREFIX = "CODEX_ATTACK_SUPPORT_ROUTER"',
            "local function isAttackSupportCandidate(identity)",
            "local function safeRequire(path)",
            'safeRequire("resource/script/multiplayer/modes/attack_support")',
            'local gameModeScriptPath = "resource/script/multiplayer/modes/" .. mode',
            "pcall(initialize)",
        ):
            self.assertIn(marker, self.bot_main)

        # The FirstPlayerId skip gate is a proven regression: live logs showed the
        # engine publishing FirstPlayerId for the Team A AI process, not the human.
        self.assertNotIn("first_player_slot", self.bot_main)
        self.assertNotIn("identity.playerId == identity.firstPlayerId", self.bot_main)
        self.assertIn("Never use FirstPlayerId to exclude a", self.bot_main)
        self.assertNotIn("team_a_attack_safe_route", self.bot_main)

    def test_mate_publishes_identity_and_arms_the_mi_wave_engine(self) -> None:
        for marker in (
            'local PREFIX = "CODEX_ATTACK_SUPPORT"',
            'sc:SetVar("id_attack_support", id.playerId)',
            'sc:SetVar("attack_support_ready", 1)',
            # The enable switch for the MI wave engine. Lua Spawn on this slot
            # never reports an available unit, so MI delivery is the only path
            # that puts bodies on the map; the attack support module must arm it explicitly.
            'sc:SetVar("attack_support_use_mi", 1)',
            "if id.attacking ~= true then return end",
        ):
            self.assertIn(marker, self.attack_support)

        # Attack support is on by default on an attack mission. Publishing the
        # identity IS the arming step; there is no separate enable var to forget.
        self.assertNotIn("allied_attack_enabled", self.attack_support)

    def test_mate_never_touches_the_slot_unsafe_engine_surface(self) -> None:
        # Every entry here cost a native crash to learn. Reading the spawn-point
        # fields or pulling in utility.lua / logic.main.lua AVs on a slot that has
        # no spawn deck, and the Lua spawn/purchase calls are inert there anyway.
        forbidden = (
            "spawnPointName",
            "PlayerSpawnPoint",
            "require(",
            ":Spawn(",
            ":SpawnAt(",
            ":Purchase(",
            "GameModeSpawnUnit(",
        )
        code = "\n".join(
            line.split("--", 1)[0] for line in self.attack_support.splitlines()
        )
        for marker in forbidden:
            self.assertNotIn(marker, code)

        # Nothing may reach the engine unguarded: every BotApi accessor returns a
        # table/nil fallback and every event body runs inside pcall.
        for marker in (
            "local function safeEvent(name, fn)",
            "local ok, err = pcall(fn, ...)",
            "return (BotApi and BotApi.Instance) or {}",
        ):
            self.assertIn(marker, self.attack_support)

    def test_mate_orders_only_squads_it_can_see(self) -> None:
        # Orders are now live (the read-only diagnostics checkpoint is retired),
        # but they stay wrapped: an unsupported command must not take the slot down.
        self.assertIn("pcall(function() c:CaptureFlag(squad, flagName) end)", self.attack_support)
        self.assertIn("pcall(function() c:SeekAndDestroy(squad) end)", self.attack_support)
        # Flag names come from the scene, never from a hardcoded fpc*/flag list.
        self.assertIn("local function pickFlagName()", self.attack_support)
        self.assertIn('type(sc.Flags) ~= "table"', self.attack_support)
        self.assertNotIn("fpc", self.attack_support)

    def test_lua_locals_are_defined_before_use(self) -> None:
        # Lua resolves a call sited above its `local function` to a nil global,
        # which crashes the bot silently the moment that path first runs. Order
        # matters for every helper the event bodies reach.
        for source, pairs in (
            (
                self.attack_support,
                (
                    ("local function log(", "log("),
                    ("local function identity()", "identity()"),
                    ("local function publishIdentity(id)", "publishIdentity(id)"),
                    ("local function pickFlagName()", "pickFlagName()"),
                    ("local function orderSquad(squad)", "orderSquad(squad)"),
                    ("local function orderNewSquads()", "orderNewSquads()"),
                    ("local function safeEvent(name, fn)", 'safeEvent("GameStart"'),
                ),
            ),
            (
                self.bot_main,
                (
                    ("local function routerLog(", "routerLog("),
                    ("local function safeRequire(path)", "safeRequire("),
                ),
            ),
        ):
            for definition, use in pairs:
                at_def = source.index(definition)
                at_use = source.index(use, at_def + len(definition))
                self.assertLess(at_def, at_use, "%s used before definition" % use)
                self.assertNotIn(use, source[:at_def])

    def test_mi_defines_are_declared_before_they_are_called(self) -> None:
        code = self.code
        for name in (
            "am_place_at_entry",
            "am_own_to_support",
            "am_finish_deploy",
            "am_deploy_next_hmmwv",
            "am_pick_composition",
        ):
            definition = '(define "%s"' % name
            self.assertEqual(code.count(definition), 1, name)
            at_def = code.index(definition)
            self.assertNotIn('("%s")' % name, code[:at_def], "%s called above its define" % name)

    def test_wave_state_is_explicitly_declared(self) -> None:
        for marker in (
            '{"attack_support_armed"}',
            '{"attack_support_transferred"}',
            '{"attack_support_stage"}',
            '{"attack_support_wave_cmd"}',
            '{"attack_support_wave_num"}',
            '{"attack_support_waves_left"}',
            '{"attack_support_busy"}',
            '{"attack_support_next_ok"}',
            '{"attack_support_hmmwv_left"}',
            '{"attack_support_use_mi"}',
            '{"enemy_spawnside"}',
            '{"id_attack_support"}',
            '{"attack_support_ready"}',
        ):
            self.assertIn(marker, self.vars)

        # Every var the engine reads is declared. defense_level$ is the one
        # exception by design: CE owns it and declares it in ce/ce_vars.inc.
        declared = set(re.findall(r'\{"([a-z0-9_]+)"\}', self.vars))
        declared.add("defense_level")
        for name in sorted(set(re.findall(r'\{var "([a-z0-9_]+)\$"\}', self.code))):
            self.assertIn(name, declared, "undeclared var read: %s$" % name)

    def test_retired_wave_skeleton_and_brain_are_gone(self) -> None:
        for path in RETIRED:
            self.assertFalse(path.exists(), "retired file is back: %s" % path)
        # Nothing may point at them any more, and no stale state may linger.
        for name in (
            "allied_attack_enabled",
            "allied_attack_started",
            "allied_attack_wave_num",
            "allied_attack_owner_fail",
            "allied_attack_retasked",
        ):
            self.assertNotIn(name, self.vars)
        self.assertNotIn("attack_support_brain", self.bot_main)
        self.assertNotIn("allied_attack", self.code)
        for mi_path in sorted((ROOT / "resource/map/multi").glob("*/campaign_capture_the_flag.mi")):
            text = mi_path.read_text(encoding="utf-8")
            with self.subTest(map=mi_path.parent.name):
                self.assertNotIn("allied_attack_waves.inc", text)
                self.assertNotIn("attack_support_probe.inc", text)

    def test_init_arms_once_then_hands_over_to_the_clock(self) -> None:
        code = self.code
        self.assertEqual(code.count('{"attack_support/init"'), 1)
        init = trigger_block(code, "init")

        # Arms exactly once and never resets its own latch, so it cannot re-run
        # and stack a second schedule.
        self.assertIn('{"1.cmp_i" {var "attack_support_armed$"} {op "=="} {value 0}}', init)
        self.assertIn('{"set_i" {var "attack_support_armed$"} {op "="} {value 1}}', init)
        self.assertNotIn('{"set_i" {var "attack_support_armed$"} {op "="} {value 0}}', code)

        # Readiness gates: attack side only, and only once attack support has
        # published its identity and armed MI delivery.
        for term in (
            '{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}',
            '{"3.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}',
            '{"4.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}',
            '{"5.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}',
        ):
            self.assertIn(term, init)

        # Opening wave 30-45s in, one small USMC rifle team (composition 1).
        for seconds in (30, 38, 45):
            self.assertEqual(code.count('{"delay" {time %d}}' % seconds), 1, seconds)
        self.assertIn('{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}', init)
        self.assertIn('{"trigger" {name "attack_support/comp_usmc"}}', init)

        # The clock is held shut until the opening wave has landed - its condition
        # is otherwise already true here and it would fire alongside init.
        busy_on = init.index('{"set_i" {var "attack_support_busy$"} {op "="} {value 1}}')
        ok_off = init.index('{"set_i" {var "attack_support_next_ok$"} {op "="} {value 0}}')
        ok_on = init.index('{"set_i" {var "attack_support_next_ok$"} {op "="} {value 1}}')
        busy_off = init.index('{"set_i" {var "attack_support_busy$"} {op "="} {value 0}}')
        opening = init.index('{"trigger" {name "attack_support/comp_usmc"}}')
        self.assertLess(busy_on, opening)
        self.assertLess(ok_off, opening)
        self.assertLess(opening, ok_on)
        self.assertLess(opening, busy_off)
        self.assertIn('{"trigger" {name "attack_support/clock"}}', init)

    def test_wave_budget_scales_with_defense_level(self) -> None:
        init = trigger_block(self.code, "init")
        # defense_level$ is the campaign progression signal computed by the CE
        # mission setup (1-3). Level 0 - not published yet - falls back to L1.
        for level, waves in ((3, 8), (2, 6)):
            case = init.index(
                '{condition {type cmp_i} {var "defense_level$"} {op "=="} {value %d}}' % level
            )
            body = init[case : case + 400]
            self.assertIn(
                '{"set_i" {var "attack_support_waves_left$"} {op "="} {value %d}}' % waves,
                body,
            )
        self.assertIn(
            '{"set_i" {var "attack_support_waves_left$"} {op "="} {value 4}}', init
        )
        # The opening wave is part of the budget, not a freebie on top of it.
        self.assertIn('{"set_i" {var "attack_support_waves_left$"} {op "-"} {value 1}}', init)

    def test_cadence_clock_is_self_rearming_and_randomized(self) -> None:
        code = self.code
        self.assertEqual(code.count('{"attack_support/clock"'), 1)
        clock = trigger_block(code, "clock")

        for term in (
            '{"1.cmp_i" {var "attack_support_next_ok$"} {op "=="} {value 1}}',
            '{"2.cmp_i" {var "attack_support_busy$"} {op "=="} {value 0}}',
            '{"3.cmp_i" {var "attack_support_waves_left$"} {op ">"} {value 0}}',
            '{"4.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}',
        ):
            self.assertIn(term, clock)

        # Randomized 150-300s cadence as a weighted {type rand} cascade. The
        # 0.2/0.25/0.33/0.5 ladder is what makes the five buckets ~20% each.
        for value in ("0.2", "0.25", "0.33", "0.5"):
            self.assertIn("{condition {type rand} {value %s}}" % value, clock)
        for seconds in (150, 190, 225, 260, 300):
            self.assertEqual(clock.count('{"delay" {time %d}}' % seconds), 1, seconds)

        # Re-arms itself: one cycle always ends by clearing busy and firing again.
        self.assertIn('{"set_i" {var "attack_support_busy$"} {op "="} {value 1}}', clock)
        self.assertIn('{"set_i" {var "attack_support_busy$"} {op "="} {value 0}}', clock)
        self.assertIn('{"trigger" {name "attack_support/clock"}}', clock)
        # Exhaustion is the condition simply ceasing to match, plus a report.
        self.assertIn("ATTACK SUPPORT WAVES EXHAUSTED", clock)

    def test_live_unit_cap_defers_without_consuming_a_wave(self) -> None:
        code = self.code
        clock = trigger_block(code, "clock")
        # Counted on the simple selector form the mission scripts use for live
        # units. The advanced selector's prop/state decorations zero the match on
        # these entities, and attack_support_src is never removed, so it is the
        # roster marker. Fails open: a bad count over-spawns rather than stalling.
        self.assertIn(
            "{selector\n"
            "\t\t\t\t\t\t\t\t\t{ignore_captured_by_user 0}\n"
            "\t\t\t\t\t\t\t\t\t{tag attack_support_src}\n"
            "\t\t\t\t\t\t\t\t\t{type human}\n"
            '\t\t\t\t\t\t\t\t\t{state "not dead"}\n'
            "\t\t\t\t\t\t\t\t}",
            clock,
        )
        self.assertIn('{count {op ">"} {value 14}}', clock)

        defer = block_at(clock, clock.rindex('{"case"', 0, clock.index("ATTACK SUPPORT NEAR CAP DEFER")))
        self.assertIn("ATTACK SUPPORT NEAR CAP DEFER", defer)
        # A defer costs nothing: no wave consumed, no composition dispatched.
        self.assertNotIn('{"set_i" {var "attack_support_waves_left$"}', defer)
        self.assertNotIn('{"set_i" {var "attack_support_wave_num$"}', defer)
        self.assertNotIn("am_pick_composition", defer)

        dispatch = block_at(clock, clock.index('{"default"', clock.index("ATTACK SUPPORT NEAR CAP DEFER")))
        self.assertIn('{"set_i" {var "attack_support_wave_num$"} {op "+"} {value 1}}', dispatch)
        self.assertIn('{"set_i" {var "attack_support_waves_left$"} {op "-"} {value 1}}', dispatch)
        self.assertIn('("am_pick_composition")', dispatch)

    def test_composition_pool_widens_with_the_campaign_level(self) -> None:
        code = self.code
        pick = define_body(code, "am_pick_composition")
        level_case = {}
        for level in (3, 2):
            at = pick.index(
                '{condition {type cmp_i} {var "defense_level$"} {op "=="} {value %d}}' % level
            )
            level_case[level] = block_at(pick, pick.rindex('{"case"', 0, at))

        def offered(block: str) -> set:
            return set(
                int(m)
                for m in re.findall(
                    r'\{"set_i" \{var "attack_support_wave_cmd\$"\} \{op "="\} \{value (\d)\}\}',
                    block,
                )
            )

        # L3 draws from all four, L2 from the first three, L1 (and an unpublished
        # level 0, which lands in the default) from the two infantry-only teams.
        self.assertEqual(offered(level_case[3]), {1, 2, 3, 4})
        self.assertEqual(offered(level_case[2]), {1, 2, 3})
        after_l2 = pick.index(level_case[2]) + len(level_case[2])
        level1 = block_at(pick, pick.index('{"default"', after_l2))
        self.assertEqual(offered(level1), {1, 2})

        # Every case picks exactly one composition and pokes exactly that trigger.
        for cmd, name in ((4, "comp_pzgren"), (3, "comp_acav"), (2, "comp_1ad"), (1, "comp_usmc")):
            at = pick.index(
                '{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value %d}}' % cmd
            )
            self.assertIn(
                '{"trigger" {name "attack_support/%s"}}' % name, pick[at : at + 200]
            )

        # Pool-short fallback: step down to the deepest pool, then give up on this
        # cycle rather than spin. A composition clears the command on entry, so a
        # command still standing means that pool could not field the wave.
        self.assertIn("ATTACK SUPPORT POOL SHORT - RIFLE TEAM INSTEAD", pick)
        self.assertIn("ATTACK SUPPORT POOL EXHAUSTED", pick)
        short = pick.index("ATTACK SUPPORT POOL SHORT - RIFLE TEAM INSTEAD")
        gaveup = pick.index("ATTACK SUPPORT POOL EXHAUSTED")
        self.assertLess(short, gaveup)
        self.assertIn(
            '{condition {type cmp_i} {var "attack_support_wave_cmd$"} {op ">"} {value 1}}',
            pick[:short],
        )
        self.assertIn(
            '{condition {type cmp_i} {var "attack_support_wave_cmd$"} {op ">"} {value 0}}',
            pick[short:gaveup],
        )

    def test_every_composition_is_command_gated_and_pool_gated(self) -> None:
        code = self.code
        for name, cmd, pool, size in COMPOSITIONS:
            with self.subTest(composition=name):
                self.assertEqual(code.count('{"attack_support/%s"' % name), 1)
                block = trigger_block(code, name)
                head = block[: block.index("{actions")]
                actions = block[block.index("{actions") :]

                # COMMAND GATING is the fix for waves auto-firing on entity
                # presence alone, which detonated all three of the old test waves
                # at once. Each composition needs its own command value AND clears
                # it as its first action, so one issue runs exactly one wave.
                self.assertIn(
                    '{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value %d}}' % cmd,
                    head,
                )
                self.assertIn('{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}', head)
                self.assertIn('{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}', head)
                self.assertIn(
                    '{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}',
                    actions[:200],
                )

                # Pool gating: a deploy strips the pool tag from the bodies it
                # takes, so counting the tag is exactly "still parked".
                self.assertIn("{selector {tag %s}}" % pool, head)
                self.assertIn('{count {op ">="} {value %d}}' % size, head)
                self.assertIn("{amount %d}" % size, actions)
                self.assertIn("{group {select {tag {tag %s}}}}" % pool, actions)
                self.assertIn("{tag_remove %s}" % pool, actions)
                self.assertIn("{tag_add attack_support_deploy}", actions)

                # Placement then the shared deploy flow, in that order.
                self.assertIn('("am_place_at_entry")', actions)
                self.assertIn('("am_finish_deploy")', actions)
                self.assertLess(
                    actions.index('("am_place_at_entry")'),
                    actions.index('("am_finish_deploy")'),
                )

        # Waves stay small: a fireteam, optionally with one or two vehicles.
        for _, _, _, size in COMPOSITIONS:
            self.assertGreaterEqual(size, 3)
            self.assertLessEqual(size, 7)

        # Stage reporting: 1 armed, then <composition>1 entered / <composition>2 done.
        for value in (1, 11, 12, 21, 22, 31, 32, 41, 42):
            self.assertIn(
                '{"set_i" {var "attack_support_stage$"} {op "="} {value %d}}' % value, code
            )

    def test_vehicle_instances_deploy_whole_and_in_order(self) -> None:
        code = self.code
        deploy = define_body(code, "am_deploy_next_hmmwv")
        # A humvee's crew is {Link}ed to that one hull, so an instance can only move
        # as a whole - never a count of bodies out of a shared vehicle pool, which
        # would land a hull without its driver. Instances are taken in order off a
        # countdown, so two concurrent waves cannot claim the same one.
        for left, instance in ((4, 1), (3, 2), (2, 3), (1, 4)):
            at = deploy.index(
                '{condition {type cmp_i} {var "attack_support_hmmwv_left$"} {op "=="} {value %d}}'
                % left
            )
            case = block_at(deploy, deploy.rindex('{"case"', 0, at))
            self.assertIn(
                "{group {select {tag {tag attack_support_hmmwv%d}}}}" % instance, case
            )
            self.assertIn(
                '{"set_i" {var "attack_support_hmmwv_left$"} {op "-"} {value 1}}', case
            )
        self.assertEqual(code.count('("am_deploy_next_hmmwv")'), 3)

        # Level 2 fields one vehicle, level 3 fields two - and each gates on there
        # being that many instances left.
        acav = trigger_block(code, "comp_acav")
        pzgren = trigger_block(code, "comp_pzgren")
        self.assertIn(
            '{"5.cmp_i" {var "attack_support_hmmwv_left$"} {op ">"} {value 0}}',
            acav[: acav.index("{actions")],
        )
        self.assertIn(
            '{"5.cmp_i" {var "attack_support_hmmwv_left$"} {op ">"} {value 1}}',
            pzgren[: pzgren.index("{actions")],
        )
        self.assertEqual(acav.count('("am_deploy_next_hmmwv")'), 1)
        self.assertEqual(pzgren.count('("am_deploy_next_hmmwv")'), 2)
        # Infantry go in first and the vehicles follow with a gap: placed into the
        # same spot together they clip and flip.
        for block in (acav, pzgren):
            self.assertLess(
                block.index('("am_finish_deploy")'), block.index('("am_deploy_next_hmmwv")')
            )

    def test_entry_side_is_chosen_at_runtime(self) -> None:
        code = self.code
        # The dynamic campaign swaps attacker/defender spawns per mission
        # instance, so a static entry waypoint is never correct.
        self.assertEqual(code.count('{"placement"'), 3)
        self.assertEqual(code.count('{target_waypoint "attack_support_entry_a"}'), 1)
        self.assertEqual(code.count('{target_waypoint "attack_support_entry_b"}'), 2)

        # Enemy on side a means we enter from b, and vice versa - never the same.
        side_a = code.index('{var "enemy_spawnside$"} {op "=="} {value 1}')
        side_b = code.index('{var "enemy_spawnside$"} {op "=="} {value 2}')
        self.assertLess(side_a, side_b)
        self.assertIn('{target_waypoint "attack_support_entry_b"}', code[side_a:side_b])
        self.assertIn('{target_waypoint "attack_support_entry_a"}', code[side_b:])

        # Placement happens before promotion, on every deploy: four compositions
        # plus the shared vehicle step.
        self.assertEqual(code.count('("am_place_at_entry")'), 5)
        self.assertEqual(code.count('("am_finish_deploy")'), 5)
        place = code.index('(define "am_place_at_entry"')
        finish = code.index('(define "am_finish_deploy"')
        self.assertLess(place, finish)

    def test_ownership_switch_covers_every_literal_player_slot(self) -> None:
        code = self.code
        # The engine will not accept a var in the {player} node, so all sixteen
        # slots are spelled out and matched against id_attack_support$.
        own = code.index('(define "am_own_to_support"')
        block = code[own : code.index('(define "am_finish_deploy"')]
        for n in range(1, 17):
            self.assertIn(
                '{condition {type cmp_i} {var "id_attack_support$"} {op "=="} '
                '{value %d}}' % n,
                block,
            )
            self.assertIn('{player "%d"}' % n, block)
        self.assertNotIn('{player "id_attack_support$"}', code)
        self.assertNotIn('{player "17"}', block)
        # Ownership is handed over exactly once per deploy, after placement.
        self.assertEqual(code.count('("am_own_to_support")'), 1)
        self.assertIn('{"set_i" {var "attack_support_transferred$"} {op "="} {value 1}}', code)

    def test_engine_never_clones_and_never_decorates_the_pool_selector(self) -> None:
        code = self.code
        # NO CLONING. Three promote designs (runtime tag, gamezone, player-0
        # identity) each matched zero freshly created entities: a new entity's
        # provenance is invisible to every selector we can express here. The pool
        # originals are MOVED instead, so they keep the tags we put on them.
        self.assertNotIn("{clone}", code)
        self.assertNotIn('{zone {zone "gamezone"}}', code)
        self.assertNotIn('{player "0"}', code)
        self.assertNotIn("{zone ", code)

        # SELECTOR RULE: decorating the advanced selector that addresses pool
        # units zeroes the match. Live proof in one run: a bare select moved all
        # four; the same select plus a prop/state decoration matched nothing in
        # the very next action. Selecting the deploy set must stay bare.
        self.assertNotIn("{prop {prop human}}", code)
        self.assertNotIn("{include {prop human}}", code)
        self.assertNotIn("{state {state operatable}}", code)
        self.assertNotIn("{include", code)
        self.assertIn("{group {select {tag {tag attack_support_deploy}}}}", code)

        # fpc1..fpc5 tags are absent from one of the fourteen maps entirely, which
        # left the units standing still on a live run. Capture points are
        # addressed as {tag flag}, the way the mission scripts do it throughout.
        self.assertNotIn("fpc", code)
        self.assertEqual(code.count("{select {tag {tag flag}}}"), 3)

        # attack_support_src is never removed: it marks everything the engine owns
        # and the live-unit cap counts it.
        self.assertNotIn("{tag_remove attack_support_src}", self.waves)

    def test_defense_side_state_is_never_reused(self) -> None:
        # The defense engine's pool, tags and owner var are a separate system.
        # Reaching into them from here hands attack support units to the defender
        # bot, or lets CE wipe them.
        for forbidden in (
            "allied_wave_fresh",
            "{tag allied_support}",
            "{tag allied_support_template}",
            "_ai_defender",
            "allied_support_src",
            '{var "id_defenderbot$"}',
        ):
            self.assertNotIn(forbidden, self.code)

    def test_only_active_flag_points_are_targeted(self) -> None:
        code = self.code
        # A mission activates only ~2 of a map's flag points; without this filter
        # the squad sprinted to a dead objective. All three shuffled picks must
        # exclude inactive, and each must exclude the earlier picks so the three
        # tags land on three different flags.
        self.assertEqual(code.count("{state {state inactive}}"), 3)
        self.assertEqual(code.count("{sort {type shuffle}}"), 3)
        for n in (1, 2, 3):
            anchor = "{tag_add attack_support_flag%d}" % n
            self.assertEqual(code.count(anchor), 1)
            pick = code.rindex("{select {tag {tag flag}}}", 0, code.index(anchor))
            window = code[pick : code.index(anchor)]
            self.assertIn("{state {state inactive}}", window)
            for earlier in range(1, n):
                self.assertIn("{tag {tag attack_support_flag%d}}" % earlier, window)

        # Every fireteam advances on a claimed flag, not on a raw coordinate.
        for n in (1, 2, 3, 4):
            self.assertIn(
                "{selector {ignore_captured_by_user 0} {tag attack_support_g%d}}" % n, code
            )
        self.assertIn(
            "{target {ignore_captured_by_user 0} {tag attack_support_flag1}}", code
        )
        self.assertIn(
            "{target {ignore_captured_by_user 0} {tag attack_support_flag2}}", code
        )
        self.assertIn(
            "{target {ignore_captured_by_user 0} {tag attack_support_flag3}}", code
        )

    def test_deploy_promotes_hands_to_ai_and_splits_into_fireteams(self) -> None:
        code = self.code
        finish = code.index('(define "am_finish_deploy"')
        block = code[finish : code.index('(define "am_deploy_next_hmmwv"')]
        for marker in (
            "{tag_add attack_support_src}",
            "{tag_remove attack_support_tpl}",
            "{tag_remove hidden}",
            "{inactive off}",
            "{impregnability disabled}",
            "{discovered on}",
            "{control AI}",
            "{ai_move {mode enable}}",
            "{weapon_prepare on}",
            "{fire_mode open}",
            # Selection is stripped so the human cannot inherit attack support units.
            "{remove select}",
        ):
            self.assertIn(marker, block)

        # Four staggered fireteams rather than one blob walking a single line.
        for n in (1, 2, 3, 4):
            self.assertIn("{tag_add attack_support_g%d}" % n, block)
            self.assertIn("{tag_remove attack_support_g%d}" % n, block)
        self.assertEqual(block.count("{amount 2}"), 3)
        # A cover beat in the middle breaks the line before the final push.
        self.assertIn('{"actor_to_cover"', block)

        # The deploy tag is consumed at the end of every deploy, so the next wave
        # starts from an empty set instead of re-ordering the previous one.
        self.assertIn("{tag_remove attack_support_deploy}", block)

    def test_wave_pool_is_deep_enough_for_the_level_budget(self) -> None:
        code = strip_comments(self.templates)

        # 64 parked prototypes. A wave MOVES pool originals out and never returns
        # them, so the pool carries the whole L3 budget of 8 waves across every
        # composition it can draw. Parked off-map at player 0, claimed by tag.
        self.assertEqual(code.count('{Able "-select"}'), 64)
        self.assertEqual(code.count("{Tags "), 64)
        self.assertEqual(code.count("{Player 0}"), 64)
        self.assertEqual(code.count('"attack_support_tpl"'), 64)
        self.assertEqual(code.count('"hidden"'), 64)
        for pool, count in (
            ("attack_support_inf_usmc", 20),
            ("attack_support_inf_1ad", 20),
            ("attack_support_inf_pzgd", 12),
        ):
            self.assertEqual(code.count('"%s"' % pool), count, pool)
        # Deepest pool is the fallback composition's, since every short draw ends there.
        self.assertGreaterEqual(
            code.count('"attack_support_inf_usmc"'), code.count('"attack_support_inf_pzgd"')
        )

        # Real breeds only. The breed-less {Human ""} + baked {Inventory} pool was
        # where every selector anomaly showed up, and it spawned unarmed bodies.
        self.assertNotIn('{Human ""', code)
        self.assertNotIn("{Inventory", code)
        for breed in (
            "mp/nato/2022s/usmc_rifleman",
            "mp/nato/2022s/1ad_rifleman",
            "mp/nato/2022s/pzgd_rifleman",
            "mp/nato/2022s/usarmy_crew",
        ):
            self.assertIn('{Human "%s"' % breed, code)
            self.assertTrue(
                (
                    ROOT.parent / "3261086933/resource/set/breed" / (breed + ".set")
                ).exists()
                or breed in (ROOT / "resource/set/multiplayer/units/conquest/inf_nato.set")
                .read_text(encoding="utf-8"),
                "breed not resolvable: %s" % breed,
            )

        # Four crewed humvee instances, each with explicit links, so every one
        # arrives drivable with the M2HB manned rather than as an empty hull.
        self.assertEqual(code.count('{Entity "humvee_m2hb_usa"'), 4)
        self.assertEqual(code.count("{Link "), 8)
        for host in ("0xaf54", "0xaf57", "0xaf5a", "0xaf5d"):
            self.assertIn('{%s "driver"}' % host, code)
            self.assertIn('{%s "gunner2"}' % host, code)
        # One tag per instance: a crew is bound to one hull, so instances deploy
        # whole and one at a time.
        for n in (1, 2, 3, 4):
            self.assertEqual(code.count('"attack_support_hmmwv%d"' % n), 3)

        # Ids and MIDs stay unique, and must not disturb the defense pool's block.
        ids = re.findall(r"\{(?:Entity|Human) \"[^\"]*\" (0x[0-9a-f]+)", code)
        self.assertEqual(len(ids), 64)
        self.assertEqual(len(set(ids)), 64)
        mids = re.findall(r"\{MID (\d+)\}", code)
        self.assertEqual(len(set(mids)), 64)
        self.assertNotIn("0xaf0", code)
        self.assertEqual(code.count("{"), code.count("}"))

    def test_all_cwa_maps_include_the_wave_engine(self) -> None:
        maps = sorted(
            p for p in (ROOT / "resource/map/multi").iterdir()
            if p.is_dir() and p.name.startswith("dcg_[cwa71]_")
        )
        self.assertEqual(len(maps), 14)
        for d in maps:
            mi = (d / "campaign_capture_the_flag.mi").read_text(encoding="utf-8")
            with self.subTest(map=d.name):
                self.assertEqual(mi.count('(include "../attack_support_waves.inc")'), 1)
                # The engine's real-breed pool goes in the ENTITIES section, and it is
                # the first thing there now: the retired allied-support experiment's
                # includes were the original anchors and the deploy script converts
                # them into these two in place rather than deleting them, so a pristine
                # map still lands the includes in exactly the same positions.
                self.assertEqual(
                    mi.count('(include "../attack_support_templates.inc")'), 1
                )
                # read_text normalises CRLF, so match on \n here.
                self.assertIn(
                    '(include "../attack_support_templates.inc")\n'
                    '\t(include "../enemy_defense_templates.inc")',
                    mi,
                )
                # Nothing from the retired allied-support experiment survives: the two
                # .inc files are deleted, so a surviving include is a dangling
                # reference the engine cannot resolve.
                self.assertNotIn("allied_support", mi)
                # Engine state declaration on every map, border included: an
                # undeclared MI var read is a silent zero, and a map without
                # dcg_vars.inc leaves all four wave engines gated shut.
                self.assertEqual(mi.count('(include "../dcg_vars.inc")'), 1)

                # Both entry sides. The dynamic campaign swaps attacker/defender
                # spawns per mission instance - the same map put us on the safe
                # side one run and in enemy territory the next - so a single
                # static entry can never be right. Each map carries one waypoint
                # per side and the engine chooses at runtime.
                self.assertEqual(mi.count('{"attack_support_entry_a"'), 1)
                self.assertEqual(mi.count('{"attack_support_entry_b"'), 1)
                # The pre-split name must be fully gone, not merely rare.
                self.assertNotIn('{"attack_support_entry"', mi)

                entries = {}
                for side in ("a", "b"):
                    wp = re.search(
                        r'\{"attack_support_entry_%s"\s*\n\s*\{position '
                        r'(-?[\d.]+) (-?[\d.]+) [\d.]+\}\s*\n\s*\{radius 150\}' % side,
                        mi,
                    )
                    self.assertIsNotNone(
                        wp, "malformed attack_support_entry_%s block" % side
                    )
                    entries[side] = (float(wp.group(1)), float(wp.group(2)))

                tags = dict(
                    (m.group(2).lower(), re.findall(r'"([^"]*)"', m.group(1)))
                    for m in re.finditer(
                        r'\{Tags ((?:"[^"]*"\s*)+)(0x[0-9a-fA-F]+)\}', mi
                    )
                )
                pos = {}
                for m in re.finditer(
                    r'\{(?:Entity|Human|Vehicle)\s+"[^"]*"\s+(0x[0-9a-fA-F]+)(.*?)\n\t\}',
                    mi,
                    re.S,
                ):
                    p = re.search(r"\{Position\s+(-?[\d.]+)\s+(-?[\d.]+)", m.group(2))
                    if p:
                        pos[m.group(1).lower()] = (float(p.group(1)), float(p.group(2)))

                def centroid(side):
                    pts = [pos[e] for e, ts in tags.items() if side in ts and e in pos]
                    self.assertTrue(pts, "no %s spawns in %s" % (side, d.name))
                    return (
                        sum(p[0] for p in pts) / len(pts),
                        sum(p[1] for p in pts) / len(pts),
                    )

                cent = {"a": centroid("spawn_a"), "b": centroid("spawn_b")}
                for side, (x, y) in entries.items():
                    other = "b" if side == "a" else "a"
                    own, opp = cent[side], cent[other]
                    d_own = ((x - own[0]) ** 2 + (y - own[1]) ** 2) ** 0.5
                    d_opp = ((x - opp[0]) ** 2 + (y - opp[1]) ** 2) ** 0.5
                    self.assertLess(
                        d_own, d_opp, "entry_%s is on the wrong side" % side
                    )
                    # Each entry sits ON its own spawn centroid, so attack support waves
                    # arrive at the map-edge spawn area rather than already
                    # pushed into open ground. EDGE_FACTOR is the tuning knob:
                    # scale the centroid toward the origin (0,0 is map centre) to
                    # move the arrival point forward. 1.00 = spawn edge.
                    self.assertAlmostEqual(x, own[0] * EDGE_FACTOR, places=1)
                    self.assertAlmostEqual(y, own[1] * EDGE_FACTOR, places=1)

    def test_deployment_patches_exactly_the_cwa_map_family(self) -> None:
        for marker in (
            "$MyInvocation.MyCommand.Path",
            'Join-Path $ScriptDirectory ".."',
            '$ExpectedBranch = "experiment/attack-mate-slot-proof"',
            "git -C $RepoRoot branch --show-current",
            "Never use FirstPlayerId to exclude a",
            "safeRequire",
            'resource\\map\\multi\\dcg_vars.inc',
            'resource\\map\\multi\\attack_support_waves.inc',
            'resource\\map\\multi\\attack_support_templates.inc',
            '(include "../attack_support_templates.inc")',
            # The retired allied-support includes are the insertion anchors for these
            # two, and a pristine base map has nothing else in either place. So they
            # are converted in place rather than deleted.
            '@(\'(include "../allied_support_templates.inc")\', \'(include "../attack_support_templates.inc")\')',
            '{Human "mp/nato/2022s/usmc_rifleman"',
            "Expected exactly one wave-templates include in",
            "Expected exactly one wave-engine include in",
            "^dcg_\\[cwa71\\]_",
            "Expected 14 CWA campaign_capture_the_flag.mi files",
            '(include "../attack_support_waves.inc")',
            "_attack_support_probe_backups",
            '{var "user_is_defender$"}',
            '{var "attack_support_wave_cmd$"}',
            '{var "attack_support_waves_left$"}',
            '{var "attack_support_hmmwv_left$"}',
            "superseded blind startup delay",
            # New-design guards.
            '{"attack_support/clock"',
            '{"attack_support/comp_pzgren"',
            '{"trigger" {name "attack_support/clock"}}',
            "ATTACK SUPPORT NEAR CAP DEFER",
            "{state \"not dead\"}",
            "must park 64 prototypes",
            # Border's inline vars block is converted to the shared include so the
            # engine gates stop reading silent zeroes there.
            "BORDER-VARS converted inline vars block in",
            '(include "../dcg_vars.inc")',
        ):
            self.assertIn(marker, self.deploy)

        # The old include name must be stripped from any map an earlier deploy
        # touched, and the orphaned files removed, or a map loads two wave engines.
        self.assertIn('(include "../attack_support_probe.inc")', self.deploy)
        self.assertIn("REMOVED ORPHAN", self.deploy)
        for orphan in (
            'resource\\map\\multi\\attack_support_probe.inc',
            'resource\\map\\multi\\allied_attack_waves.inc',
            'resource\\script\\multiplayer\\modes\\attack_support_brain.lua',
        ):
            self.assertIn(orphan, self.deploy)

        # Deploy script may mention the bad route only as a rejection check.
        self.assertIn('SimpleMatch "team_a_attack_safe_route"', self.deploy)
        self.assertNotIn("team_a_attack_safe_route =", self.deploy)

    def test_delimiters_are_balanced(self) -> None:
        for text in (self.bot_main, self.attack_support):
            self.assertEqual(text.count("("), text.count(")"))

        for text in (self.waves, self.templates):
            code = strip_comments(text)
            self.assertEqual(code.count("{"), code.count("}"))
            self.assertEqual(code.count("("), code.count(")"))


if __name__ == "__main__":
    unittest.main()
