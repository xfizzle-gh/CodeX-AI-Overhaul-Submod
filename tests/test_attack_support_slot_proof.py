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
FACTION_TEMPLATES = ROOT / "resource/map/multi/faction_support_templates.inc"
# The publisher of user_nation$, which the faction fold reads.
DCG_SCRIPT = ROOT / "resource/map/multi/dcg_script.inc"
DEPLOY = ROOT / "tools/deploy_attack_support_probe.ps1"

# The four wave engines, one per quadrant of the support parity. Anything that has to
# hold across all of them - the support_debug$ diagnostic gate, the triple-entry
# round robin - is pinned against this list rather than one file at a time.
ENGINES = (
    "attack_support_waves",
    "enemy_defense_support",
    "defense_support_waves",
    "enemy_attack_support",
)

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
    ("comp_arf", 5, "attack_support_inf_arf", 5),
)

# Faction-aware pools, keyed by the player's own nation rather than a fixed NATO
# roster. faction_support_army$ folds user_nation$ down to these four: sov and pol
# fall in with rusa, csa and frg with nato, exactly as the enemy engines fold
# bot_army$. Depths are per faction; every one is shared by the attack and the
# defence engine, which never run on the same mission.
FACTION_ARMIES = (("rusa", 1), ("ukr", 2), ("nato", 3), ("prc", 4))
# comp suffix -> (wave command, bodies drawn per wave, pool depth per faction)
FACTION_COMPS = (
    ("line", 10, 4, 24),
    ("wpn", 11, 4, 16),
    ("recon", 12, 3, 15),
    ("assault", 13, 4, 16),
    ("eng", 14, 3, 12),
    ("manpad", 15, 2, 8),
)
# Light vehicles are attack-only and counter-gated rather than pool-counted:
# Ukraine fields three humvees, NATO two Fenneks, and no other faction has any.
FACTION_VEH = (("ukr", 2, 3), ("nato", 3, 2))
# The full fold from user_nation$ (published by dcg/player_nation) to the four
# faction pools. Anything unmapped fails closed to NATO.
NATION_FOLD = {1: 1, 5: 1, 8: 1, 2: 2, 3: 3, 4: 3, 7: 3, 6: 4}


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
        cls.faction_tpl = strip_comments(FACTION_TEMPLATES.read_text(encoding="utf-8"))
        cls.dcg_script = strip_comments(DCG_SCRIPT.read_text(encoding="utf-8"))

    def test_player_nation_fold_matches_what_the_mission_publishes(self) -> None:
        """faction_support_army$ is folded out of user_nation$, which dcg_script.inc's
        dcg/player_nation trigger publishes. If the fold and the publisher ever
        disagree, a player draws another nation's troops - or, worse, silently gets
        the NATO default. So both halves are pinned here, together."""
        resolve = define_body(self.code, "as_resolve_army")
        for nation, army in sorted(NATION_FOLD.items()):
            with self.subTest(user_nation=nation):
                case = (
                    '{"case"\n'
                    '\t\t\t\t\t\t{condition {type cmp_i} {var "user_nation$"} '
                    '{op "=="} {value %d}}\n' % nation
                )
                at = resolve.index(
                    '{condition {type cmp_i} {var "user_nation$"} {op "=="} '
                    "{value %d}}" % nation
                )
                self.assertIn(
                    '{"set_i" {var "faction_support_army$"} {op "="} {value %d}}' % army,
                    resolve[at : at + 200],
                    "user_nation %d must fold to army %d" % (nation, army),
                )
                del case
        # Exactly the eight published nations are handled, nothing invented.
        handled = set(
            int(m) for m in re.findall(
                r'\{condition \{type cmp_i\} \{var "user_nation\$"\} \{op "=="\} '
                r"\{value (\d+)\}\}",
                resolve,
            )
        )
        self.assertEqual(handled, set(NATION_FOLD))
        # Fail closed: an unpublished nation lands on NATO, never on nothing.
        default = resolve[resolve.rindex('{"default"'):]
        self.assertIn(
            '{"set_i" {var "faction_support_army$"} {op "="} {value 3}}', default
        )

        # The publisher side. dcg/player_nation must emit every value the fold maps.
        # It writes some cases one-line and some across four lines, so match on the
        # var/op/value triple with whitespace collapsed rather than on a literal.
        flat = re.sub(r"\s+", " ", self.dcg_script)
        published = set(
            int(m) for m in re.findall(
                r'\{ ?"set_i" \{var "user_nation\$"\} \{op "="\} \{value (\d+)\} ?\}',
                flat,
            )
        )
        self.assertTrue(published, "found no user_nation$ writes at all")
        missing = set(NATION_FOLD) - published
        self.assertFalse(
            missing, "the fold maps nations the mission never publishes: %s" % missing
        )
        # And nothing is published that the fold would drop on the floor.
        unmapped = published - set(NATION_FOLD)
        self.assertFalse(
            unmapped, "published nations with no fold entry: %s" % unmapped
        )
        # Published ~1s in (0.9s delay), and every consumer of the fold runs far
        # later than that - the attack opening wave is 30s+ - so nothing reads a
        # silent zero. See the header note in attack_support_waves.inc.
        init = trigger_block(self.code, "init")
        before_resolve = init[: init.index('("as_resolve_army")')]
        waits = [int(m) for m in re.findall(r'\{"delay" \{time (\d+)\}\}',
                                            before_resolve)]
        self.assertTrue(waits, "resolve runs with no delay ahead of it")
        # Worst case the engine waits the shortest opening bucket, which is 30s -
        # comfortably past the ~1s at which dcg/player_nation publishes.
        self.assertGreaterEqual(min(waits), 30)

    def test_faction_pool_is_deep_enough_for_every_faction(self) -> None:
        """Depths are per faction, and each pool is shared by the attack and defence
        engines - which never run on the same mission, so each only has to cover ONE
        engine's worst case. The binding number is the L3 budget of 8 waves."""
        code = self.faction_tpl
        self.assertEqual(code.count('{Able "-select"}'), 379)
        self.assertEqual(code.count("{Player 0}"), 379)
        self.assertEqual(code.count('"ally_sup_tpl"'), 379)
        self.assertEqual(code.count('"hidden"'), 379)

        for key, _army in FACTION_ARMIES:
            for suffix, _cmd, take, depth in FACTION_COMPS:
                tag = "ally_sup_%s_%s" % (key, suffix)
                with self.subTest(pool=tag):
                    self.assertEqual(code.count('"%s"' % tag), depth, tag)
                    # A pool must field at least four consecutive draws of its own
                    # comp; beyond that the gate declines and the pick falls back to
                    # the faction line pool rather than deploying a partial team.
                    self.assertGreaterEqual(depth // take, 4, tag)
            # No faction can be exhausted outright: the largest per-wave draw is 4
            # bodies, so an 8-wave L3 run can consume at most 32 from a faction that
            # parks 91 or more.
            total = sum(
                code.count('"ally_sup_%s_%s"' % (key, suffix))
                for suffix, _c, _t, _d in FACTION_COMPS
            )
            self.assertGreaterEqual(total, 8 * 4, key)
            self.assertEqual(total, 91)

        # Vehicles: Ukraine three, NATO two, nobody else any. Counter-gated rather
        # than pool-counted, so the counter is what must match the parked instances.
        for key, _army, instances in FACTION_VEH:
            self.assertEqual(code.count('"ally_sup_%s_veh"' % key), instances * 3, key)
            for n in range(1, instances + 1):
                self.assertEqual(code.count('"ally_sup_%s_veh%d"' % (key, n)), 3)
            self.assertIn(
                '{"set_i" {var "attack_support_%s_veh_left$"} {op "="} {value %d}}'
                % (key, instances),
                self.code,
            )
        for key in ("rusa", "prc"):
            self.assertNotIn('"ally_sup_%s_veh"' % key, code)

        # Bands: this pool must not collide with either neighbour in a resolved map.
        ids = re.findall(r"\{(?:Entity|Human) \"[^\"]*\" (0x[0-9a-f]+)", code)
        self.assertEqual(len(ids), 379)
        self.assertEqual(len(set(ids)), 379)
        self.assertTrue(all(i.startswith(("0xb2", "0xb3")) for i in ids))
        mids = [int(m) for m in re.findall(r"\{MID (\d+)\}", code)]
        self.assertEqual(len(set(mids)), 379)
        self.assertGreaterEqual(min(mids), 9300)
        # Real breeds only, and none of the retired idioms.
        self.assertNotIn('{Human ""', code)
        self.assertNotIn("{Inventory", code)
        for banned in ("{clone}", "{include {prop human}}", "allied_support"):
            self.assertNotIn(banned, code, banned)
        self.assertEqual(code.count("{"), code.count("}"))

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

    def test_engine_state_is_mirrored_to_the_game_log(self) -> None:
        """With the on-screen diagnostics gated behind support_debug$, game.log is the
        only place a shipped run can be read back. This slot loads on every
        campaign_capture_the_flag mission, attack or defence, so it mirrors all four
        wave engines from one place - always on, log only."""
        lua = self.attack_support

        # Reads are pcall-guarded with printable fallbacks. GetVar is not proven on this
        # BotApi surface and this is the slot that AVs when a native getter is misused.
        self.assertIn("local function readVar(name)", lua)
        self.assertIn(
            "local ok, v = pcall(function() return sc:GetVar(name) end)", lua
        )
        for fallback in ('return "na"', 'return "err"', 'return "nil"'):
            self.assertIn(fallback, lua)

        # One line per engine, plus the resolved player-faction pool.
        self.assertIn(
            'emit("mirror", "q", state.quant,\n'
            '\t\t"faction_support_army", readVar("faction_support_army"))',
            lua,
        )
        body = lua[lua.index("local function mirrorEngineState()") :]
        body = body[: body.index("\nend\n")]
        calls = body.split('emit("mirror", ')
        self.assertEqual(len(calls), 6, "expected the header line plus four engines")
        for engine, extra in (
            ("attack_support", ()),
            (
                "enemy_defense",
                (
                    ("garrison_place", "enemy_defense_place"),
                    ("garrison_group", "enemy_defense_group"),
                ),
            ),
            ("defense_support", ()),
            ("enemy_attack", ()),
        ):
            call = next(c for c in calls if c.startswith('"%s",' % engine))
            for field in ("armed", "wave_num", "waves_left"):
                self.assertIn(
                    '"%s", readVar("%s_%s")' % (field, engine, field), call, engine
                )
            for label, var in extra:
                self.assertIn('"%s", readVar("%s")' % (label, var), call)

        # Ungated writer: the mirror must not disappear with the DEBUG_LOG chatter.
        self.assertIn("local function emit(", lua)
        self.assertIn(
            "local function log(...)\n\tif not DEBUG_LOG then return end\n\temit(...)", lua
        )
        self.assertNotIn("\tlog(", body)

        # Cadence, and the pre-existing heartbeat is untouched.
        self.assertIn("local MIRROR_QUANTS = 200", lua)
        self.assertIn(
            "if state.quant % MIRROR_QUANTS == 0 then\n\t\tmirrorEngineState()", lua
        )
        self.assertIn('log("heartbeat", "q", state.quant)', lua)

    def test_lua_locals_are_defined_before_use(self) -> None:
        # Lua resolves a call sited above its `local function` to a nil global,
        # which crashes the bot silently the moment that path first runs. Order
        # matters for every helper the event bodies reach.
        for source, pairs in (
            (
                self.attack_support,
                (
                    ("local function emit(", "emit("),
                    ("local function log(", "log("),
                    ("local function readVar(name)", "readVar("),
                    ("local function identity()", "identity()"),
                    ("local function publishIdentity(id)", "publishIdentity(id)"),
                    ("local function pickFlagName()", "pickFlagName()"),
                    ("local function orderSquad(squad)", "orderSquad(squad)"),
                    ("local function orderNewSquads()", "orderNewSquads()"),
                    ("local function mirrorEngineState()", "mirrorEngineState()"),
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

        # Opening wave 30-45s in. Since the faction-aware pools landed the opening
        # wave is no longer hardcoded to the USMC team: init resolves the player's
        # faction first and then goes through the ordinary weighted pick, so at L1
        # it is a line or recon team of the player's own nation.
        for seconds in (30, 38, 45):
            self.assertEqual(code.count('{"delay" {time %d}}' % seconds), 1, seconds)
        self.assertIn('("as_resolve_army")', init)
        self.assertIn('("am_pick_composition")', init)
        # The faction must be resolved before the pick reads it, or the pick falls
        # through to the NATO default on a non-NATO player.
        self.assertLess(init.index('("as_resolve_army")'),
                        init.index('("am_pick_composition")'))
        # The command is cleared on entry so a stale command cannot deploy a wave.
        self.assertIn('{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}', init)

        # The clock is held shut until the opening wave has landed - its condition
        # is otherwise already true here and it would fire alongside init.
        busy_on = init.index('{"set_i" {var "attack_support_busy$"} {op "="} {value 1}}')
        ok_off = init.index('{"set_i" {var "attack_support_next_ok$"} {op "="} {value 0}}')
        ok_on = init.index('{"set_i" {var "attack_support_next_ok$"} {op "="} {value 1}}')
        busy_off = init.index('{"set_i" {var "attack_support_busy$"} {op "="} {value 0}}')
        opening = init.index('("am_pick_composition")')
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

        # Randomized 120-240s cadence as a weighted {type rand} cascade - about 20%
        # tighter than the retired 150-300s ladder, so wave 2 arrives in ~3-5 min
        # rather than 5-6. The 0.2/0.25/0.33/0.5 ladder keeps the five buckets ~20%.
        for value in ("0.2", "0.25", "0.33", "0.5"):
            self.assertIn("{condition {type rand} {value %s}}" % value, clock)
        for seconds in (120, 150, 180, 210, 240):
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

        # Anchored on the live-count condition rather than on the timer title: every
        # diagnostic now sits inside its own support_debug$ switch, so the nearest
        # {"case"} above a title is that gate's case, not the defer branch's.
        defer = block_at(clock, clock.rindex('{"case"', 0, clock.index('{count {op ">"} {value 14}}')))
        self.assertIn("ATTACK SUPPORT NEAR CAP DEFER", defer)
        # A defer costs nothing: no wave consumed, no composition dispatched.
        self.assertNotIn('{"set_i" {var "attack_support_waves_left$"}', defer)
        self.assertNotIn('{"set_i" {var "attack_support_wave_num$"}', defer)
        self.assertNotIn("am_pick_composition", defer)

        dispatch = block_at(clock, clock.index('{"default"', clock.index(defer) + len(defer)))
        self.assertIn('{"set_i" {var "attack_support_wave_num$"} {op "+"} {value 1}}', dispatch)
        self.assertIn('{"set_i" {var "attack_support_waves_left$"} {op "-"} {value 1}}', dispatch)
        self.assertIn('("am_pick_composition")', dispatch)

    def test_composition_pool_widens_with_the_campaign_level(self) -> None:
        """The pick is two-branched since the faction-aware pools landed: a player
        on a non-NATO nation draws entirely from its own faction pools, while NATO
        keeps the original specialty compositions and has the shared hybrid comps
        injected on top at L2/L3. Both branches must still widen with the level."""
        code = self.code
        pick = define_body(code, "am_pick_composition")
        hybrid = define_body(code, "as_pick_hybrid_non_nato")

        def levels(body: str) -> dict:
            """L3 / L2 case bodies plus the L1 default that trails them."""
            out = {}
            for level in (3, 2):
                at = body.index(
                    '{condition {type cmp_i} {var "defense_level$"} {op "=="} {value %d}}'
                    % level
                )
                out[level] = block_at(body, body.rindex('{"case"', 0, at))
            after = body.index(out[2]) + len(out[2])
            out[1] = block_at(body, body.index('{"default"', after))
            return out

        def offered(block: str) -> set:
            return set(
                int(m)
                for m in re.findall(
                    r'\{"set_i" \{var "attack_support_wave_cmd\$"\} \{op "="\} \{value (\d+)\}\}',
                    block,
                )
            )

        # Non-NATO: L1 is line + recon only; L2 adds wpn/assault/eng and the rare
        # vehicle; L3 additionally unlocks the MANPAD team.
        nn = levels(hybrid)
        self.assertEqual(offered(nn[1]), {10, 12})
        self.assertEqual(offered(nn[2]), {10, 11, 12, 13, 14, 16})
        self.assertEqual(offered(nn[3]), {10, 11, 12, 13, 14, 15, 16})
        # MANPAD is the L3-only unlock, and L1 offers no vehicle.
        self.assertNotIn(15, offered(nn[2]))
        self.assertNotIn(16, offered(nn[1]))

        # NATO: specialty comps 1-5 survive, with hybrid comps injected at L2/L3.
        na = levels(pick)
        self.assertEqual(offered(na[1]), {1, 2, 5, 12})
        self.assertEqual(offered(na[2]), {1, 2, 3, 5, 12, 13, 14, 16})
        self.assertEqual(offered(na[3]), {1, 2, 3, 4, 5, 12, 13, 14, 15, 16})
        # The NATO branch never draws the generic faction line/wpn pools directly;
        # those are reached only through the pool-short fallback below.
        for lvl in (1, 2, 3):
            self.assertNotIn(10, offered(na[lvl]))
            self.assertNotIn(11, offered(na[lvl]))

        # Every specialty case picks one composition and pokes exactly that trigger.
        for cmd, name in ((4, "comp_pzgren"), (3, "comp_acav"), (5, "comp_arf"),
                          (2, "comp_1ad"), (1, "comp_usmc")):
            at = pick.index(
                '{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value %d}}' % cmd
            )
            self.assertIn(
                '{"trigger" {name "attack_support/%s"}}' % name, pick[at : at + 200]
            )
        # Every hybrid case pokes the matching faction fan-out define.
        for cmd, poke in ((10, "line"), (11, "wpn"), (12, "recon"), (13, "assault"),
                          (14, "eng"), (15, "manpad"), (16, "veh")):
            at = hybrid.index(
                '{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value %d}}' % cmd
            )
            self.assertIn('("as_poke_faction_%s")' % poke, hybrid[at : at + 200])

        # Pool-short fallback: step down to the player's own line pool, then give up
        # on this cycle rather than spin. A composition clears the command on entry,
        # so a command still standing means that pool could not field the wave.
        self.assertIn("ATTACK SUPPORT POOL SHORT - FACTION LINE", pick)
        self.assertIn("ATTACK SUPPORT POOL EXHAUSTED", pick)
        short = pick.index("ATTACK SUPPORT POOL SHORT - FACTION LINE")
        gaveup = pick.index("ATTACK SUPPORT POOL EXHAUSTED")
        self.assertLess(short, gaveup)
        # The step-down sets the faction line command and pokes it.
        self.assertIn(
            '{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}',
            pick[:short],
        )
        self.assertIn('("as_poke_faction_line")', pick[:short])
        # Both fallback stages trigger on "a command is still standing".
        self.assertIn(
            '{condition {type cmp_i} {var "attack_support_wave_cmd$"} {op ">"} {value 0}}',
            pick[:short],
        )
        self.assertIn(
            '{condition {type cmp_i} {var "attack_support_wave_cmd$"} {op ">"} {value 0}}',
            pick[short:gaveup],
        )
        # Giving up clears the command so the next cycle starts clean.
        self.assertIn(
            '{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}',
            pick[short:gaveup + 200],
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
        # The dynamic campaign swaps attacker/defender spawns per mission instance, so a
        # static entry waypoint is never correct. Each side has THREE main pads (3 branches
        # x 3 pads = 9) plus TWO flank pads on the flank path (3 branches x 2 pads = 6).
        self.assertEqual(code.count('{"placement"'), 15)
        for point in (1, 2, 3):
            self.assertEqual(
                code.count('{target_waypoint "attack_support_entry_a%d"}' % point), 1
            )
            self.assertEqual(
                code.count('{target_waypoint "attack_support_entry_b%d"}' % point), 2
            )
        for point in (1, 2):
            self.assertEqual(
                code.count('{target_waypoint "attack_support_flank_a%d"}' % point), 1
            )
            self.assertEqual(
                code.count('{target_waypoint "attack_support_flank_b%d"}' % point), 2
            )
        # Never the bare legacy alias: that one exists for move orders, not placements.
        for side in "ab":
            self.assertNotIn(
                '{target_waypoint "attack_support_entry_%s"}' % side, code
            )

        # Enemy on side a means we enter from b, and vice versa - never the same.
        # Each spawnside==1 case block must only place on b pads; spawnside==2 on a pads.
        for m in re.finditer(
            r'\{var "enemy_spawnside\$"\} \{op "=="\} \{value 1\}(.*?)'
            r'(?=\{var "enemy_spawnside\$"\} \{op "=="\} \{value 2\}|\Z)',
            code,
            re.S,
        ):
            chunk = m.group(1)
            if "target_waypoint" not in chunk:
                continue
            self.assertNotIn("entry_a", chunk)
            self.assertNotIn("flank_a", chunk)
            self.assertTrue(
                ("entry_b" in chunk) or ("flank_b" in chunk), chunk[:120]
            )
        for m in re.finditer(
            r'\{var "enemy_spawnside\$"\} \{op "=="\} \{value 2\}(.*?)'
            r'(?=\{var "enemy_spawnside\$"\} \{op "=="\} \{value 1\}|\{"default"|\Z)',
            code,
            re.S,
        ):
            chunk = m.group(1)
            if "target_waypoint" not in chunk:
                continue
            # Stop at the branch's own default that still places on b (fallback side).
            cut = chunk.find('{"default"')
            if cut > 0:
                chunk = chunk[:cut]
            if "target_waypoint" not in chunk:
                continue
            self.assertNotIn("entry_b", chunk)
            self.assertNotIn("flank_b", chunk)
            self.assertTrue(
                ("entry_a" in chunk) or ("flank_a" in chunk), chunk[:120]
            )

        # Placement happens before promotion on EVERY deploy. Pinning a bare count
        # went stale the moment the faction pools added comps, so instead require
        # that the two always come in pairs and that every deploying trigger uses
        # them - that is the property that actually matters.
        self.assertEqual(code.count('("am_place_at_entry")'),
                         code.count('("am_finish_deploy")'))
        deployers = [n for n in re.findall(r'\{"attack_support/([a-z0-9_]+)"', code)
                     if n not in ("init", "clock")]
        self.assertTrue(deployers)
        for name in deployers:
            with self.subTest(deployer=name):
                block = trigger_block(code, name)
                self.assertIn('("am_place_at_entry")', block)
                self.assertIn('("am_finish_deploy")', block)
                self.assertLess(block.index('("am_place_at_entry")'),
                                block.index('("am_finish_deploy")'))
        place = code.index('(define "am_place_at_entry"')
        finish = code.index('(define "am_finish_deploy"')
        self.assertLess(place, finish)

        # Bodies land one at a time. Placing a whole fireteam onto one pad piles
        # them up, so am_place_at_entry is a run of single-body placements and then
        # clears the one-shot marker.
        placer = define_body(code, "am_place_at_entry")
        self.assertGreaterEqual(placer.count('("am_place_one")'), 6)
        self.assertIn('{tag_remove attack_support_placed}', placer)
        one = define_body(code, "am_place_one")
        self.assertIn('{amount 1}', one)
        self.assertIn('{exclude {tag {tag attack_support_placed}}}', one)

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

        # The retired {"actor_to_cover"} cover beat is gone; the line is now broken
        # by scattering the fireteams across up to three DIFFERENT active flags, so
        # elements peel apart instead of pausing together. Each pick excludes the
        # ones already taken, which is what stops all three landing on one flag.
        self.assertNotIn('{"actor_to_cover"', block)
        for n in (1, 2, 3):
            self.assertEqual(block.count("{tag_add attack_support_flag%d}" % n), 1, n)
            # Stale picks from the previous wave are cleared before choosing again.
            self.assertIn("{tag_remove attack_support_flag%d}" % n, block)
        second = block.index("{tag_add attack_support_flag2}")
        third = block.index("{tag_add attack_support_flag3}")
        self.assertIn("{tag {tag attack_support_flag1}}", block[:second])
        for prior in (1, 2):
            self.assertIn("{tag {tag attack_support_flag%d}}" % prior, block[:third])
        # Every fireteam then advances rather than beelining from a raw coordinate.
        self.assertEqual(block.count("{action advance}"), 4)

        # The deploy tag is consumed at the end of every deploy, so the next wave
        # starts from an empty set instead of re-ordering the previous one.
        self.assertIn("{tag_remove attack_support_deploy}", block)

    def test_wave_pool_is_deep_enough_for_the_level_budget(self) -> None:
        code = strip_comments(self.templates)

        # 84 parked prototypes - the original 64 plus the 20-strong ARF pool that
        # came in with composition 5. A wave MOVES pool originals out and never
        # returns them, so the pool carries the whole L3 budget of 8 waves across
        # every composition it can draw. Parked off-map at player 0, claimed by tag.
        self.assertEqual(code.count('{Able "-select"}'), 84)
        self.assertEqual(code.count("{Tags "), 84)
        self.assertEqual(code.count("{Player 0}"), 84)
        self.assertEqual(code.count('"attack_support_tpl"'), 84)
        self.assertEqual(code.count('"hidden"'), 84)
        for pool, count in (
            ("attack_support_inf_usmc", 20),
            ("attack_support_inf_1ad", 20),
            ("attack_support_inf_pzgd", 12),
            ("attack_support_inf_arf", 20),
        ):
            self.assertEqual(code.count('"%s"' % pool), count, pool)

        # This file shares every fully-resolved map with enemy_defense_templates.inc,
        # whose MID band opens at 9100. The ARF block originally numbered itself
        # 9084..9103 and duplicated four MIDs in all fourteen maps, so the whole
        # file must stay strictly below that band.
        mids = [int(m) for m in re.findall(r"\{MID (\d+)\}", code)]
        self.assertEqual(len(mids), 84)
        self.assertEqual(len(set(mids)), 84, "duplicate MID inside the pool")
        self.assertLess(max(mids), 9100, "pool runs into the enemy-defence MID band")
        self.assertEqual((min(mids), max(mids)), (9000, 9083))
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

        # Ids and MIDs stay unique, and must not disturb either neighbouring pool's
        # block: enemy_defense_templates.inc owns 0xb1xx / MID 9100+ and
        # faction_support_templates.inc owns 0xb2xx-0xb3xx / MID 9300+.
        ids = re.findall(r"\{(?:Entity|Human) \"[^\"]*\" (0x[0-9a-f]+)", code)
        self.assertEqual(len(ids), 84)
        self.assertEqual(len(set(ids)), 84)
        mids = re.findall(r"\{MID (\d+)\}", code)
        self.assertEqual(len(set(mids)), 84)
        self.assertNotIn("0xaf0", code)
        self.assertTrue(all(i.startswith("0xaf") for i in ids), "id band drifted")
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
                    '\t(include "../faction_support_templates.inc")\n'
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
            "must park 84 prototypes",
            # The player-nation pool has to be shipped, include-injected and depth
            # checked by the deploy, or the faction waves reference nothing.
            "resource\\map\\multi\\faction_support_templates.inc",
            '(include "../faction_support_templates.inc")',
            "must park 379 prototypes",
            'ally_sup_rusa_line',
            "must not park a vehicle pool",
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


class SupportDiagnosticGateTests(unittest.TestCase):
    """Every on-screen timer is gated on support_debug$ (diagnostics, default OFF)
    or support_announce$ (player text, default ON via init). No ungated timers."""

    DEBUG_GATE = '{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}'
    ANNOUNCE_GATE = (
        '{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}'
    )
    ANY_GATE = re.compile(
        r'\{condition \{type cmp_i\} \{var "support_(?:debug|announce)\$"\} '
        r'\{op "=="\} \{value 1\}\}\s*\{"timer"'
    )

    @classmethod
    def setUpClass(cls) -> None:
        cls.engines = {
            name: strip_comments(
                (ROOT / ("resource/map/multi/%s.inc" % name)).read_text(encoding="utf-8")
            )
            for name in ENGINES
        }
        cls.raw_engines = {
            name: (ROOT / ("resource/map/multi/%s.inc" % name)).read_text(
                encoding="utf-8"
            )
            for name in ENGINES
        }
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.deploy = DEPLOY.read_text(encoding="utf-8")
        cls.pot = (
            ROOT
            / "localizations/default/interface/text/mission/multi/support_events.pot"
        ).read_text(encoding="utf-8")

    def test_no_engine_ships_an_ungated_on_screen_timer(self) -> None:
        for name, code in self.engines.items():
            timers = code.count('{"timer"')
            gated = len(self.ANY_GATE.findall(code))
            self.assertGreater(timers, 0, "%s lost its timers entirely" % name)
            self.assertEqual(
                timers - gated, 0, "%s has %d ungated timer(s)" % (name, timers - gated)
            )

    def test_gate_uses_one_consistent_minimal_shape(self) -> None:
        for name, code in self.engines.items():
            gates = code.count(self.DEBUG_GATE) + code.count(self.ANNOUNCE_GATE)
            self.assertEqual(code.count('{"timer"'), gates, name)
            self.assertGreaterEqual(code.count('{"default"}'), gates, name)

    def test_debug_toggle_is_declared_and_never_forced_on(self) -> None:
        self.assertIn('{"support_debug"}', self.vars)
        for name, code in self.engines.items():
            self.assertNotIn(
                '{var "support_debug$"} {op "="}', code, "%s writes debug toggle" % name
            )

    def test_announce_toggle_is_declared_and_enabled_at_init(self) -> None:
        self.assertIn('{"support_announce"}', self.vars)
        for name, code in self.engines.items():
            self.assertIn(
                '{var "support_announce$"} {op "="} {value 1}',
                code,
                "%s never enables announcements" % name,
            )

    def test_announce_keys_are_localized(self) -> None:
        keys = [
            "mission/multi/support/wave_inbound",
            "mission/multi/support/vehicle_inbound",
            "mission/multi/support/flank_inbound",
            "mission/multi/support/waves_exhausted",
            "mission/multi/support/defense_reinforced",
            "mission/multi/support/enemy_activity",
        ]
        for key in keys:
            self.assertIn('msgctxt "%s"' % key, self.pot)
            # filled msgstr (repo convention)
            idx = self.pot.index('msgctxt "%s"' % key)
            chunk = self.pot[idx : idx + 400]
            self.assertRegex(chunk, r'msgstr "[^"]+"')

    def test_engines_reference_only_known_announce_keys(self) -> None:
        key_re = re.compile(r'title "mission/multi/support/([^"]+)"')
        known = {
            "wave_inbound",
            "vehicle_inbound",
            "flank_inbound",
            "waves_exhausted",
            "defense_reinforced",
            "enemy_activity",
        }
        for name, code in self.raw_engines.items():
            for match in key_re.finditer(code):
                self.assertIn(match.group(1), known, name)

    def test_toggle_is_documented_in_every_engine_header_and_the_deploy(self) -> None:
        for name in ENGINES:
            header = self.raw_engines[name]
            head = "\n".join(
                line for line in header.splitlines() if line.startswith(";")
            )
            self.assertIn("support_debug$", head, "%s header undocumented" % name)
            self.assertIn("support_announce$", head, "%s announce undocumented" % name)
        self.assertIn("Test-SupportTimerGate", self.deploy)
        self.assertEqual(self.deploy.count("Test-SupportTimerGate $pair[0]"), 2)
        self.assertIn("support_announce", self.deploy)



class AttackSupportFlankTests(unittest.TestCase):
    """Phase 2: flanking arrival pads for friendly attack support only."""

    @classmethod
    def setUpClass(cls) -> None:
        cls.waves = (ROOT / "resource/map/multi/attack_support_waves.inc").read_text(
            encoding="utf-8"
        )
        cls.deploy = DEPLOY.read_text(encoding="utf-8")
        cls.other = {
            name: (ROOT / ("resource/map/multi/%s.inc" % name)).read_text(encoding="utf-8")
            for name in (
                "defense_support_waves",
                "enemy_defense_support",
                "enemy_attack_support",
            )
        }

    def test_choose_entry_rolls_and_guards(self) -> None:
        self.assertIn('(define "as_choose_entry"', self.waves)
        self.assertIn('{type rand} {value 0.25}', self.waves)
        self.assertIn('{distance 120}', self.waves)
        self.assertIn('attack_support_use_flank$', self.waves)
        self.assertIn('attack_support_flank_rr$', self.waves)
        self.assertIn('("as_announce_flank")', self.waves)

    def test_place_one_addresses_flank_pads(self) -> None:
        for side in ("a", "b"):
            for n in (1, 2):
                self.assertIn(
                    'target_waypoint "attack_support_flank_%s%d"' % (side, n),
                    self.waves,
                )

    def test_other_engines_never_reference_flank_pads(self) -> None:
        for name, code in self.other.items():
            self.assertNotIn("attack_support_flank_", code, name)

    def test_deploy_generates_flank_geometry(self) -> None:
        self.assertIn("$FlankDepth", self.deploy)
        self.assertIn("$FlankSpread", self.deploy)
        self.assertIn("attack_support_flank_", self.deploy)


if __name__ == "__main__":
    unittest.main()

