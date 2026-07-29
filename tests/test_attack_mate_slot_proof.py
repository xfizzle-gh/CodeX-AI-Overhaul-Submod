from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
GAME_SET = ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"
BOT_MAIN = ROOT / "resource/script/multiplayer/bot.main.lua"
ATTACK_MATE = ROOT / "resource/script/multiplayer/modes/attacker_mate.lua"
VARS = ROOT / "resource/map/multi/dcg_vars.inc"
RETASK = ROOT / "resource/map/multi/attack_mate_retask_probe.inc"
TEMPLATES = ROOT / "resource/map/multi/attack_mate_probe_templates.inc"
DEPLOY = ROOT / "tools/deploy_attack_mate_probe.ps1"


def strip_comments(text: str) -> str:
    """MI comment-stripped view. Headers quote bad forms as cautionary examples,
    so every structural count must run on code only."""
    return "\n".join(line.split(";", 1)[0] for line in text.splitlines())


class AttackMateSlotProofTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.game_set = GAME_SET.read_text(encoding="utf-8")
        cls.bot_main = BOT_MAIN.read_text(encoding="utf-8")
        cls.attack_mate = ATTACK_MATE.read_text(encoding="utf-8")
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.retask = RETASK.read_text(encoding="utf-8")
        cls.templates = TEMPLATES.read_text(encoding="utf-8")
        cls.deploy = DEPLOY.read_text(encoding="utf-8")
        cls.code = strip_comments(cls.retask)

    def test_team_a_requests_exactly_one_ai_mate_slot(self) -> None:
        self.assertEqual(self.game_set.count("{aiTeamPlayers 1}"), 1)
        team_a = self.game_set.index('{"a"')
        team_b = self.game_set.index('{"b"', team_a)
        self.assertIn("{aiTeamPlayers 1}", self.game_set[team_a:team_b])
        self.assertIn("{minTeamSlots 7}", self.game_set[team_a:team_b])

    def test_router_routes_team_a_mate_regardless_of_first_player_id(self) -> None:
        for marker in (
            'local ROUTER_PREFIX = "CODEX_ATTACK_MATE_ROUTER"',
            "local function isAttackMateCandidate(identity)",
            "local function safeRequire(path)",
            'safeRequire("resource/script/multiplayer/modes/attacker_mate")',
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
            'local PREFIX = "CODEX_ATTACK_MATE"',
            'sc:SetVar("id_attacker_mate", id.playerId)',
            'sc:SetVar("attacker_mate_ready", 1)',
            # The enable switch for the MI wave engine. Lua Spawn on this slot
            # never reports an available unit, so MI delivery is the only path
            # that puts bodies on the map; the mate must arm it explicitly.
            'sc:SetVar("attack_mate_use_mi_probe", 1)',
            "if id.attacking ~= true then return end",
        ):
            self.assertIn(marker, self.attack_mate)

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
            line.split("--", 1)[0] for line in self.attack_mate.splitlines()
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
            self.assertIn(marker, self.attack_mate)

    def test_mate_orders_only_squads_it_can_see(self) -> None:
        # Orders are now live (the read-only diagnostics checkpoint is retired),
        # but they stay wrapped: an unsupported command must not take the slot down.
        self.assertIn("pcall(function() c:CaptureFlag(squad, flagName) end)", self.attack_mate)
        self.assertIn("pcall(function() c:SeekAndDestroy(squad) end)", self.attack_mate)
        # Flag names come from the scene, never from a hardcoded fpc*/flag list.
        self.assertIn("local function pickFlagName()", self.attack_mate)
        self.assertIn('type(sc.Flags) ~= "table"', self.attack_mate)
        self.assertNotIn("fpc", self.attack_mate)

    def test_lua_locals_are_defined_before_use(self) -> None:
        # Lua resolves a call sited above its `local function` to a nil global,
        # which crashes the bot silently the moment that path first runs. Order
        # matters for every helper the event bodies reach.
        for source, pairs in (
            (
                self.attack_mate,
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

    def test_probe_state_is_explicitly_declared(self) -> None:
        for marker in (
            '{"attack_mate_probe_started"}',
            '{"attack_mate_probe_transferred"}',
            '{"attack_mate_probe_stage"}',
            # The wave clock and the MI-delivery enable switch.
            '{"attack_mate_wave_cmd"}',
            '{"attack_mate_use_mi_probe"}',
            '{"enemy_spawnside"}',
            '{"id_attacker_mate"}',
            '{"attacker_mate_ready"}',
        ):
            self.assertIn(marker, self.vars)

    def test_wave_schedule_is_armed_once_and_command_gated(self) -> None:
        code = self.code

        # One arming trigger plus three waves.
        for name in (
            '{"attack_mate/schedule"',
            '{"attack_mate/wave1"',
            '{"attack_mate/wave2"',
            '{"attack_mate/wave3"',
        ):
            self.assertEqual(code.count(name), 1, name)

        # The schedule arms exactly once and never resets its own latch, so it
        # cannot re-run and stack a second set of waves.
        self.assertIn(
            '{"1.cmp_i" {var "attack_mate_probe_started$"} {op "=="} {value 0}}', code
        )
        self.assertIn(
            '{"set_i" {var "attack_mate_probe_started$"} {op "="} {value 1}}', code
        )
        self.assertNotIn(
            '{"set_i" {var "attack_mate_probe_started$"} {op "="} {value 0}}', code
        )
        # Attack side only, and only once the mate has published its identity and
        # armed MI delivery.
        self.assertIn('{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}', code)
        self.assertIn('{"3.cmp_i" {var "attacker_mate_ready$"} {op "=="} {value 1}}', code)
        self.assertIn(
            '{"4.cmp_i" {var "attack_mate_use_mi_probe$"} {op "=="} {value 1}}', code
        )

        # COMMAND GATING is the fix for waves auto-firing on entity presence
        # alone, which detonated all three at once. Each wave requires its own
        # command value AND clears the command as its first action, so a wave
        # runs exactly once per issue.
        schedule = code.index('{"attack_mate/schedule"')
        for n in (1, 2, 3):
            wave = code.index('{"attack_mate/wave%d"' % n)
            body = code[wave:]
            self.assertIn(
                '{"2.cmp_i" {var "attack_mate_wave_cmd$"} {op "=="} {value %d}}' % n,
                body[: body.index("{actions")],
            )
            actions = body[body.index("{actions") :]
            self.assertIn(
                '{"set_i" {var "attack_mate_wave_cmd$"} {op "="} {value 0}}',
                actions[:200],
            )
            # The clock issues the command from the schedule, above every wave.
            issue = code.index(
                '{"set_i" {var "attack_mate_wave_cmd$"} {op "="} {value %d}}' % n
            )
            self.assertLess(schedule, issue)
            self.assertLess(issue, wave)

        # 30 / 90 / 150 seconds: a 30s lead-in then two 60s gaps.
        self.assertEqual(code.count('{"delay" {time 30}}'), 1)
        self.assertEqual(code.count('{"delay" {time 60}}'), 2)
        # Every wave also needs its own pool present, so an exhausted wave is a
        # no-op rather than an empty deploy.
        for n in (1, 2, 3):
            self.assertIn("{selector {tag attack_mate_w%d}}" % n, code)

        # Stage reporting: 1 armed, then <wave>1 entered / <wave>2 completed.
        for value in (1, 11, 12, 21, 22, 31, 32):
            self.assertIn(
                '{"set_i" {var "attack_mate_probe_stage$"} {op "="} {value %d}}' % value,
                code,
            )

    def test_entry_side_is_chosen_at_runtime(self) -> None:
        code = self.code
        # The dynamic campaign swaps attacker/defender spawns per mission
        # instance, so a static entry waypoint is never correct.
        self.assertEqual(code.count('{"placement"'), 3)
        self.assertEqual(code.count('{target_waypoint "attack_mate_entry_a"}'), 1)
        self.assertEqual(code.count('{target_waypoint "attack_mate_entry_b"}'), 2)

        # Enemy on side a means we enter from b, and vice versa - never the same.
        side_a = code.index('{var "enemy_spawnside$"} {op "=="} {value 1}')
        side_b = code.index('{var "enemy_spawnside$"} {op "=="} {value 2}')
        self.assertLess(side_a, side_b)
        self.assertIn('{target_waypoint "attack_mate_entry_b"}', code[side_a:side_b])
        self.assertIn('{target_waypoint "attack_mate_entry_a"}', code[side_b:])

        # Placement happens before promotion, on every wave.
        self.assertEqual(code.count('("am_place_at_entry")'), 5)
        self.assertEqual(code.count('("am_finish_deploy")'), 5)
        place = code.index('(define "am_place_at_entry"')
        finish = code.index('(define "am_finish_deploy"')
        self.assertLess(place, finish)

    def test_ownership_switch_covers_every_literal_player_slot(self) -> None:
        code = self.code
        # The engine will not accept a var in the {player} node, so all sixteen
        # slots are spelled out and matched against id_attacker_mate$.
        own = code.index('(define "am_own_to_mate"')
        block = code[own : code.index('(define "am_finish_deploy"')]
        for n in range(1, 17):
            self.assertIn(
                '{condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} '
                '{value %d}}' % n,
                block,
            )
            self.assertIn('{player "%d"}' % n, block)
        self.assertNotIn('{player "id_attacker_mate$"}', code)
        self.assertNotIn('{player "17"}', block)
        # Ownership is handed over exactly once per deploy, after placement.
        self.assertEqual(code.count('("am_own_to_mate")'), 1)
        self.assertIn('{"set_i" {var "attack_mate_probe_transferred$"} {op "="} {value 1}}', code)

    def test_probe_never_clones_and_never_decorates_the_pool_selector(self) -> None:
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
        for match in re.finditer(
            r"\{group \{select \{tag \{tag attack_mate_deploy\}\}\}\}", code
        ):
            self.assertTrue(match)
        self.assertIn("{group {select {tag {tag attack_mate_deploy}}}}", code)

        # fpc1..fpc5 tags are absent from one of the fourteen maps entirely, which
        # left the units standing still on a live run. Capture points are
        # addressed as {tag flag}, the way the mission scripts do it throughout.
        self.assertNotIn("fpc", code)
        self.assertEqual(code.count("{select {tag {tag flag}}}"), 3)

        # attack_mate_src is never removed: it marks everything the probe owns.
        self.assertNotIn("{tag_remove attack_mate_src}", self.retask)

    def test_only_active_flag_points_are_targeted(self) -> None:
        code = self.code
        # A mission activates only ~2 of a map's flag points; without this filter
        # the squad sprinted to a dead objective. All three shuffled picks must
        # exclude inactive, and each must exclude the earlier picks so the three
        # tags land on three different flags.
        self.assertEqual(code.count("{state {state inactive}}"), 3)
        self.assertEqual(code.count("{sort {type shuffle}}"), 3)
        for n in (1, 2, 3):
            anchor = "{tag_add attack_mate_flag%d}" % n
            self.assertEqual(code.count(anchor), 1)
            pick = code.rindex("{select {tag {tag flag}}}", 0, code.index(anchor))
            window = code[pick : code.index(anchor)]
            self.assertIn("{state {state inactive}}", window)
            for earlier in range(1, n):
                self.assertIn("{tag {tag attack_mate_flag%d}}" % earlier, window)

        # Every fireteam advances on a claimed flag, not on a raw coordinate.
        for n in (1, 2, 3, 4):
            self.assertIn(
                "{selector {ignore_captured_by_user 0} {tag attack_mate_g%d}}" % n, code
            )
        self.assertIn(
            "{target {ignore_captured_by_user 0} {tag attack_mate_flag1}}", code
        )
        self.assertIn(
            "{target {ignore_captured_by_user 0} {tag attack_mate_flag2}}", code
        )
        self.assertIn(
            "{target {ignore_captured_by_user 0} {tag attack_mate_flag3}}", code
        )

    def test_deploy_promotes_hands_to_ai_and_splits_into_fireteams(self) -> None:
        code = self.code
        finish = code.index('(define "am_finish_deploy"')
        block = code[finish:]
        for marker in (
            "{tag_remove attack_mate_tpl}",
            "{tag_remove hidden}",
            "{inactive off}",
            "{impregnability disabled}",
            "{discovered on}",
            "{control AI}",
            "{ai_move {mode enable}}",
            "{weapon_prepare on}",
            "{fire_mode open}",
            # Selection is stripped so the human cannot inherit mate units.
            "{remove select}",
        ):
            self.assertIn(marker, block)

        # Four staggered fireteams rather than one blob walking a single line.
        for n in (1, 2, 3, 4):
            self.assertIn("{tag_add attack_mate_g%d}" % n, block)
            self.assertIn("{tag_remove attack_mate_g%d}" % n, block)
        self.assertEqual(block.count("{amount 2}"), 3)

        # The deploy tag is consumed at the end of every deploy, so the next wave
        # starts from an empty set instead of re-ordering the previous one.
        self.assertIn("{tag_remove attack_mate_deploy}", block)

    def test_probe_templates_are_real_breed_prototypes(self) -> None:
        code = strip_comments(self.templates)

        # 27 parked prototypes: three seven-strong fireteams plus two crewed
        # humvees. Parked off-map at player 0 and claimed by tag on deploy.
        self.assertEqual(code.count("{Able \"-select\"}"), 27)
        self.assertEqual(code.count("{Tags "), 27)
        self.assertEqual(code.count("{Player 0}"), 27)
        self.assertEqual(code.count('"attack_mate_tpl"'), 27)
        self.assertEqual(code.count('"hidden"'), 27)
        for n in (1, 2, 3):
            self.assertEqual(code.count('"attack_mate_w%d"' % n), 7 if n < 3 else 13)

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

        # Both humvees are crewed by explicit links, so they arrive drivable and
        # with the M2HB manned rather than as empty hulls.
        self.assertEqual(code.count('{Entity "humvee_m2hb_usa"'), 2)
        self.assertEqual(code.count("{Link "), 4)
        for host in ("0xaf50", "0xaf54"):
            self.assertIn('{%s "driver"}' % host, code)
            self.assertIn('{%s "gunner2"}' % host, code)
        # Humvees deploy one at a time, so each needs its own tag.
        for n in (1, 2):
            self.assertEqual(code.count('"attack_mate_hmmwv%d"' % n), 3)

        # Must not disturb the defense pool's ids.
        self.assertNotIn("0xaf0", code)
        self.assertEqual(code.count("{"), code.count("}"))

    def test_all_cwa_maps_include_attack_mate_probe(self) -> None:
        maps = sorted(
            p for p in (ROOT / "resource/map/multi").iterdir()
            if p.is_dir() and p.name.startswith("dcg_[cwa71]_")
        )
        self.assertEqual(len(maps), 14)
        for d in maps:
            mi = (d / "campaign_capture_the_flag.mi").read_text(encoding="utf-8")
            with self.subTest(map=d.name):
                self.assertEqual(mi.count('(include "../attack_mate_retask_probe.inc")'), 1)
                self.assertEqual(mi.count('(include "../allied_support_waves.inc")'), 1)
                # The probe's real-breed pool goes in the ENTITIES section, right
                # after the existing templates include.
                self.assertEqual(
                    mi.count('(include "../attack_mate_probe_templates.inc")'), 1
                )
                # read_text normalises CRLF, so match on \n here.
                self.assertIn(
                    '(include "../allied_support_templates.inc")\n'
                    '\t(include "../attack_mate_probe_templates.inc")',
                    mi,
                )

                # Both entry sides. The dynamic campaign swaps attacker/defender
                # spawns per mission instance - the same map put us on the safe
                # side one run and in enemy territory the next - so a single
                # static entry can never be right. Each map carries one waypoint
                # per side and the probe chooses at runtime.
                self.assertEqual(mi.count('{"allied_support_entry"'), 1)
                self.assertEqual(mi.count('{"attack_mate_entry_a"'), 1)
                self.assertEqual(mi.count('{"attack_mate_entry_b"'), 1)
                # The pre-split name must be fully gone, not merely rare.
                self.assertNotIn('{"attack_mate_entry"', mi)

                entries = {}
                for side in ("a", "b"):
                    wp = re.search(
                        r'\{"attack_mate_entry_%s"\s*\n\s*\{position '
                        r'(-?[\d.]+) (-?[\d.]+) [\d.]+\}\s*\n\s*\{radius 150\}' % side,
                        mi,
                    )
                    self.assertIsNotNone(
                        wp, "malformed attack_mate_entry_%s block" % side
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
                    # Pulled toward the centre, so strictly closer to origin than
                    # the spawn line - units land forward of the spawn markers.
                    self.assertLess(
                        (x * x + y * y) ** 0.5,
                        (own[0] ** 2 + own[1] ** 2) ** 0.5,
                    )

    def test_deployment_patches_exactly_the_cwa_map_family(self) -> None:
        for marker in (
            "$MyInvocation.MyCommand.Path",
            'Join-Path $ScriptDirectory ".."',
            '$ExpectedBranch = "experiment/attack-mate-slot-proof"',
            "git -C $RepoRoot branch --show-current",
            "Never use FirstPlayerId to exclude a",
            "safeRequire",
            'resource\\map\\multi\\dcg_vars.inc',
            'resource\\map\\multi\\attack_mate_retask_probe.inc',
            'resource\\map\\multi\\attack_mate_probe_templates.inc',
            '(include "../attack_mate_probe_templates.inc")',
            '$tplAnchor = \'(include "../allied_support_templates.inc")\'',
            '{Human "mp/nato/2022s/usmc_rifleman"',
            "Expected exactly one probe-templates include in",
            "^dcg_\\[cwa71\\]_",
            "Expected 14 CWA campaign_capture_the_flag.mi files",
            '(include "../attack_mate_retask_probe.inc")',
            "_attack_mate_probe_backups",
            '{var "user_is_defender$"}',
            '{var "attack_mate_wave_cmd$"}',
            "superseded blind startup delay",
        ):
            self.assertIn(marker, self.deploy)

        # Deploy script may mention the bad route only as a rejection check.
        self.assertIn('SimpleMatch "team_a_attack_safe_route"', self.deploy)
        self.assertNotIn("team_a_attack_safe_route =", self.deploy)

    def test_delimiters_are_balanced(self) -> None:
        for text in (self.bot_main, self.attack_mate):
            self.assertEqual(text.count("("), text.count(")"))

        for text in (self.retask, self.templates):
            code = strip_comments(text)
            self.assertEqual(code.count("{"), code.count("}"))
            self.assertEqual(code.count("("), code.count(")"))


if __name__ == "__main__":
    unittest.main()
