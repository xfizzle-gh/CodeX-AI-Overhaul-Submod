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
DEPLOY = ROOT / "tools/deploy_attack_mate_probe.ps1"


class AttackMateSlotProofTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.game_set = GAME_SET.read_text(encoding="utf-8")
        cls.bot_main = BOT_MAIN.read_text(encoding="utf-8")
        cls.attack_mate = ATTACK_MATE.read_text(encoding="utf-8")
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.retask = RETASK.read_text(encoding="utf-8")
        cls.deploy = DEPLOY.read_text(encoding="utf-8")

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

    def test_lua_probe_remains_read_only(self) -> None:
        for marker in (
            'local PREFIX = "CODEX_ATTACK_MATE_PROBE"',
            'sc:SetVar("id_attacker_mate", id.playerId)',
            'sc:SetVar("attacker_mate_ready", 1)',
            '"scene_squads"',
            '"scene_flags"',
            '"diagnostics_only"',
            '"orders", "disabled"',
            '"stage", readVar("attack_mate_probe_stage")',
        ):
            self.assertIn(marker, self.attack_mate)

        forbidden = (
            "CaptureFlag(",
            "SeekAndDestroy(",
            ":Spawn(",
            ":SpawnAt(",
            ":Purchase(",
            "GameModeSpawnUnit(",
        )
        for marker in forbidden:
            self.assertNotIn(marker, self.attack_mate)

    def test_probe_state_is_explicitly_declared(self) -> None:
        for marker in (
            '{"attack_mate_probe_started"}',
            '{"attack_mate_probe_transferred"}',
            '{"attack_mate_probe_retasked"}',
            '{"attack_mate_probe_stage"}',
            '{"id_attacker_mate"}',
            '{"attacker_mate_ready"}',
        ):
            self.assertIn(marker, self.vars)

    def test_mission_probe_waits_for_ready_sources_then_retasks(self) -> None:
        for marker in (
            '{expression "1 & 2 & 3"}',
            '{var "user_is_defender$"}',
            'ATTACK MATE PROBE BOOT ATTACK SIDE',
            'ATTACK MATE PROBE ARMED CLONE TEST',
            '{tag_add attack_mate_src}',
            '{target_waypoint "attack_mate_entry"}',
            'ATTACK MATE PROBE 1 CLONES READY',
            '{player "3"}',
            'id_attacker_mate$',
            'ATTACK MATE PROBE OWNER FALLBACK P3',
            'ATTACK MATE PROBE 2 TRANSFERRED',
            'ATTACK MATE PROBE 3 LEG1 ORDERED',
            '"attack_mate/probe_retask"',
            'ATTACK MATE PROBE FLAG 1 REACHED',
            'ATTACK MATE PROBE 4 RETASKED TO FLAG 2',
            'ATTACK MATE PROBE FAIL NO CLONES',
            'ATTACK MATE PROBE FAIL NO POOL',
            # Real capture points. fpc1..fpc5 is an Indomitus naming convention:
            # 13 of 14 CWA maps carry those tags but outback carries none, which
            # is why {tag fpc1} left the units standing still on the live run.
            # flag_point_campaign entities exist on all 14 and the engine tags
            # them `flag`, which is how dcg_script.inc addresses them throughout.
            '{group {select {tag {tag flag}}}}',
            '{tag_add attack_mate_flag1}',
            '{tag_add attack_mate_flag2}',
            '{target {ignore_captured_by_user 0} {tag attack_mate_flag1}}',
            '{target {ignore_captured_by_user 0} {tag attack_mate_flag2}}',
            # Source is the probe's OWN real-breed off-map pool. Live map-garrison
            # defenders are map-state-dependent (never armed on border), and the
            # breed-less defense pool is where every selector anomaly showed up.
            '{select {tag {tag attack_mate_tpl}}}',
            '{tag_add attack_mate_pool}',
        ):
            self.assertIn(marker, self.retask)

        self.assertNotIn('ATTACK MATE PROBE FAIL NO DEFENDER SOURCES', self.retask)

        # Pipeline stage reporting: 1 claimed, 2 moved, 21 pre-promote, 3 promoted,
        # 4 transferred, 8 no pool (terminal), 9 promote matched none.
        for value in (1, 2, 21, 3, 4, 8, 9):
            self.assertIn(
                '{"set_i" {var "attack_mate_probe_stage$"} {op "="} {value %d}}' % value,
                self.retask,
            )

        # NO CLONING. Three promote designs (runtime tag, gamezone, player-0
        # identity) each matched zero freshly created entities. A new entity's
        # provenance is invisible to every selector we can express on this engine,
        # so the probe moves the pool originals instead of copying them.
        self.assertNotIn("{clone}", self.retask)
        self.assertNotIn('{zone {zone "gamezone"}}', self.retask)
        self.assertNotIn('{player "0"}', self.retask)

        # Slices are taken over the comment-stripped view: the header and inline
        # notes deliberately quote the bad forms as cautionary examples.
        code = "\n".join(l.split(";", 1)[0] for l in self.retask.splitlines())

        # The move is the placement verb with the clone sub-node dropped.
        move = code.index('{target_waypoint "attack_mate_entry"}')
        promote = code.index("{tag_add attack_mate_probe}")
        self.assertLess(move, promote)
        move_block = code[code.index('{"placement"'):move]
        self.assertIn("{select {tag {tag attack_mate_src}}}", move_block)
        self.assertIn("{amount 4}", move_block)

        # SELECTOR RULE: on these breed-less templates, decorating an advanced
        # selector zeroes the match. Live proof in one run: bare select moved all
        # 4; the same select plus {include {prop {prop human}}} and state excludes
        # matched nothing in the very next action. So promote's selector must be
        # byte-for-byte the placement's proven form - bare select, nothing else.
        self.assertNotIn("{prop {prop human}}", code)
        self.assertNotIn("{state {state operatable}}", code)
        promote_sel = code[move:promote]
        self.assertIn("{select {tag {tag attack_mate_src}}}", promote_sel)
        for decoration in ("{include", "{exclude"):
            self.assertNotIn(decoration, promote_sel)

        payload = code[promote:promote + 500]
        for marker in (
            "{tag_remove attack_mate_pool}",
            "{tag_remove attack_mate_tpl}",
            "{tag_remove hidden}",
            "{inactive off}",
            "{impregnability disabled}",
            "{discovered on}",
        ):
            self.assertIn(marker, payload)

        # The probe owns its pool outright: it never selects, tags or consumes
        # the breed-less defense pool.
        self.assertNotIn("allied_support_template", code)
        # attack_mate_src is never removed: the whole downstream chain keys on it.
        self.assertNotIn("{tag_remove attack_mate_src}", self.retask)

        # Nothing gates on attack_mate_probe any more - it is a best-effort marker
        # applied by promote and referenced nowhere else.
        self.assertEqual(self.retask.count("{tag_add attack_mate_probe}"), 1)
        self.assertEqual(self.retask.count("attack_mate_probe}"), 1)
        # Ownership, actor_state, ables, orders and retask all key on the proven
        # tag: 16 ownership cases + P3 default + actor_state + ables + the flag-1
        # advance + the flag-2 advance in probe_retask = 21.
        self.assertEqual(
            self.retask.count(
                "{selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}}"
            ),
            21,
        )
        # Nothing anywhere still keys on the unproven marker tag.
        self.assertNotIn("{tag attack_mate_probe}", code)

        # No fpc reference may survive in probe code. dcg_functions.mi keeps its
        # own fpc_* helpers and other tests may legitimately pin those; this
        # assertion is scoped to the probe only.
        self.assertNotIn("fpc", code)

        # Nearest-flag pick: sort the candidate set by distance to a reference
        # entity then take one (the ai_enhance_dcg.inc:78-101 idiom), and the
        # retask must exclude the first pick so it is a genuinely new target.
        self.assertEqual(code.count("{type entity}"), 2)
        self.assertEqual(code.count("{tag_add attack_mate_flag1}"), 1)
        self.assertEqual(code.count("{tag_add attack_mate_flag2}"), 1)
        self.assertIn("{exclude {tag {tag attack_mate_flag1}}}", code)

        # Arrival is the near/near_to/distance idiom, not a zone test.
        self.assertIn('{"3.near"', code)
        self.assertIn("{near_to", code)
        self.assertIn("{distance 60}", code)
        self.assertNotIn("{zone ", code)

        # The move consumes the originals, so the no-pool path must terminate
        # rather than spin once the 8-strong pool is empty.
        stage8 = self.retask.index(
            '{"set_i" {var "attack_mate_probe_stage$"} {op "="} {value 8}}'
        )
        self.assertNotIn(
            '{"trigger" {name "attack_mate/probe_init"}}', self.retask[stage8:]
        )
        self.assertNotIn(
            '{"set_i" {var "attack_mate_probe_started$"} {op "="} {value 0}}',
            self.retask,
        )

        # All indices below are in the same comment-stripped coordinate space.
        boot = code.index("ATTACK MATE PROBE BOOT ATTACK SIDE")
        transfer = code.index('{player "3"}')
        leg1 = code.index("ATTACK MATE PROBE 3 LEG1 ORDERED")
        retask = code.index("ATTACK MATE PROBE 4 RETASKED TO FLAG 2")
        self.assertLess(boot, move)
        self.assertLess(move, transfer)
        self.assertLess(transfer, leg1)
        self.assertLess(leg1, retask)
        self.assertNotIn('{"delay" {time 8}}', self.retask)
        # The prep_inform gate was removed on purpose: an attack mission often
        # never raises PrepTimeOver, so gating on it would stall the probe.
        self.assertNotIn('{var "prep_inform$"}', self.retask)
        # The probe only ever touches the 4 pool originals it tagged itself, so it
        # has no reason to name player- or user-owned entities at all.
        self.assertNotIn('{tag {tag player}}', self.retask)
        self.assertNotIn('{tag {tag _user}}', self.retask)

    def test_probe_templates_are_real_breed_prototypes(self) -> None:
        tpl = (ROOT / "resource/map/multi/attack_mate_probe_templates.inc").read_text(
            encoding="utf-8"
        )
        # Breed path is verified twice: the .set file exists in the Code:X base mod,
        # and the same string is used by this mod's own purchase list
        # (resource/set/multiplayer/units/conquest/inf_nato.set).
        # The header documents the form, so structural counts run on code only.
        code = "\n".join(l.split(";", 1)[0] for l in tpl.splitlines())

        breed = "mp/nato/2022s/1ad_rifleman"
        self.assertEqual(code.count('{Human "%s"' % breed), 4)
        units = (
            ROOT / "resource/set/multiplayer/units/conquest/inf_nato.set"
        ).read_text(encoding="utf-8")
        self.assertIn('"%s"' % breed, units)

        for n in range(1, 5):
            self.assertIn("0xaf1%d" % n, code)
            self.assertIn("{MID 901%d}" % n, code)
            self.assertIn('{Tags "attack_mate_tpl" "hidden" 0xaf1%d}' % n, code)

        # DOTD's parked-prototype form: {Able "-select"}, and NO baked Inventory,
        # so the breed's own loadout stands. This is the point of the switch away
        # from the breed-less {Human ""} + {Inventory {box {clear}}} pool.
        self.assertEqual(code.count('{Able "-select"}'), 4)
        self.assertNotIn("{Inventory", code)
        self.assertNotIn('{Human ""', code)
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

                # Attack-side entry. allied_support_entry is authored at the
                # spawn_a centroid (the DEFENDER's rear) on 13 of 14 maps, so on an
                # attack mission it drops the units in enemy territory - confirmed
                # live on outback. Each map gets its own waypoint at that map's
                # spawn_b centroid, pulled toward the map centre.
                self.assertEqual(mi.count('{"attack_mate_entry"'), 1)
                self.assertEqual(mi.count('{"allied_support_entry"'), 1)
                wp = re.search(
                    r'\{"attack_mate_entry"\s*\n\s*\{position '
                    r'(-?[\d.]+) (-?[\d.]+) [\d.]+\}\s*\n\s*\{radius 150\}',
                    mi,
                )
                self.assertIsNotNone(wp, "malformed attack_mate_entry block")
                x, y = float(wp.group(1)), float(wp.group(2))

                # It must sit on the B side: nearer the spawn_b centroid than the
                # spawn_a centroid, and strictly inside the spawn_b line.
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

                ax, ay = centroid("spawn_a")
                bx, by = centroid("spawn_b")
                d_a = ((x - ax) ** 2 + (y - ay) ** 2) ** 0.5
                d_b = ((x - bx) ** 2 + (y - by) ** 2) ** 0.5
                self.assertLess(d_b, d_a)
                # Pulled toward the centre, so strictly closer to origin than the
                # spawn line itself - units land forward of the spawn markers.
                self.assertLess(
                    (x * x + y * y) ** 0.5, (bx * bx + by * by) ** 0.5
                )

    def test_deployment_patches_exactly_the_cwa_map_family(self) -> None:
        for marker in (
            "$MyInvocation.MyCommand.Path",
            'Join-Path $ScriptDirectory ".."',
            '$ExpectedBranch = "experiment/attack-mate-slot-proof"',
            "git -C $RepoRoot branch --show-current",
            "Never use FirstPlayerId to exclude a",
            "safeRequire",
            "diagnostics_only",
            'resource\\map\\multi\\dcg_vars.inc',
            'resource\\map\\multi\\attack_mate_retask_probe.inc',
            'resource\\map\\multi\\attack_mate_probe_templates.inc',
            '(include "../attack_mate_probe_templates.inc")',
            '$tplAnchor = \'(include "../allied_support_templates.inc")\'',
            '{Human "mp/nato/2022s/1ad_rifleman" 0xaf11',
            "Expected exactly one probe-templates include in",
            "^dcg_\\[cwa71\\]_",
            "Expected 14 CWA campaign_capture_the_flag.mi files",
            '(include "../attack_mate_retask_probe.inc")',
            "_attack_mate_probe_backups",
            '{var "user_is_defender$"}',
            "superseded blind startup delay",
        ):
            self.assertIn(marker, self.deploy)

        # Deploy script may mention the bad route only as a rejection check.
        self.assertIn('SimpleMatch "team_a_attack_safe_route"', self.deploy)
        self.assertNotIn("team_a_attack_safe_route =", self.deploy)

    def test_delimiters_are_balanced(self) -> None:
        for text in (self.bot_main, self.attack_mate):
            self.assertEqual(text.count("("), text.count(")"))

        code = "\n".join(line.split(";", 1)[0] for line in self.retask.splitlines())
        self.assertEqual(code.count("{"), code.count("}"))
        self.assertEqual(code.count("("), code.count(")"))


if __name__ == "__main__":
    unittest.main()
