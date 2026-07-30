"""Structure pins for the enemy-defender garrison / patrol / reinforcement engine.

Phase B of the attack-mission overhaul: the mirror image of attack_support_waves.inc,
aimed at the AI defender. Everything asserted here is either a hard-won pipeline
constraint inherited from the attack-support engine (no cloning, bare pool selectors,
literal {player} switch, {tag flag} capture points) or a behavioural promise of the
enemy-defender design: garrison on the live flags, ~50/50 flag-cycle-versus-roam
patrols, two asynchronous reinforcement ladders off the defender's own map edge, and
total inertness on a human-DEFENCE mission.
"""

from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VARS = ROOT / "resource/map/multi/dcg_vars.inc"
ENGINE = ROOT / "resource/map/multi/enemy_defense_support.inc"
TEMPLATES = ROOT / "resource/map/multi/enemy_defense_templates.inc"
WAVES = ROOT / "resource/map/multi/attack_support_waves.inc"
ATTACK_TEMPLATES = ROOT / "resource/map/multi/attack_support_templates.inc"
CONQUEST = ROOT / "resource/script/multiplayer/modes/conquest.lua"
DEPLOY = ROOT / "tools/deploy_attack_support_probe.ps1"
BREED_ROOT = ROOT.parent / "3261086933/resource/set/breed"

# faction key -> (enemy_defense_army$ value, breed side folder)
FACTIONS = (
    ("rusa", 1, "rusa"),
    ("ukr", 2, "ukr"),
    ("prc", 4, "prc"),
    ("nato", 3, "nato"),
)

# trigger suffix -> (wave_cmd value, pool role, bodies taken, stage base)
DRAWS = (
    ("light", 1, "line", 3, 10),
    ("line", 2, "line", 4, 20),
    ("wpn", 3, "wpn", 4, 30),
)

# Pool depth per faction. A claim MOVES prototypes out and never returns them, so a
# pool has to carry the whole L3 budget on its own: the garrison (up to three
# fireteams) plus eight reinforcement waves. Running dry is a graceful path - the
# surge steps down to the line pool and then skips the cycle - but it should be the
# exception, not the schedule.
POOL_DEPTH = (("line", 24), ("wpn", 16))
PROTOTYPES = len(FACTIONS) * sum(n for _, n in POOL_DEPTH)  # 160

# The wave budget, shared by both spawners so two ladders do not double the total.
WAVE_BUDGET = ((3, 8), (2, 6))
LIVE_CAP = 16

TRICKLE_LADDER = (45, 60, 75, 90)
SURGE_LADDER = (180, 220, 260, 300)
PATROL_LADDER = (60, 80, 100, 120)
# Weighted {type rand} cascade. 0.25/0.34/0.5 makes the four buckets ~25% each.
LADDER_WEIGHTS = ("0.25", "0.34", "0.5")

# Every breed the pool parks, as mp/<side>/2022s/<name>. Existence-checked below:
# the pool ships no breeds of its own, so a missing base install silently parks 160
# absent entities and the whole engine becomes a no-op.
ROSTERS = {
    ("rusa", "line"): ("rus90_squadlead", "rus90_rifleman", "rus90_rifleman", "rus90_mg"),
    ("rusa", "wpn"): ("rus90_seniorrifleman", "rus90_antitank", "rus90_marksman", "rus90_rifleman"),
    ("ukr", "line"): ("ter_squadlead", "ter_rifleman", "ter_rifleman", "ter_mg"),
    ("ukr", "wpn"): ("ter_squadlead", "ter_antitank", "ter_marksman", "ter_rifleman"),
    ("prc", "line"): ("pla_squadlead", "pla_rifleman", "pla_rifleman", "pla_mg"),
    ("prc", "wpn"): ("pla_senior", "pla_antitank_pf98", "pla_marksman", "pla_rifleman"),
    ("nato", "line"): ("nato_squadlead", "nato_rifleman", "nato_rifleman", "nato_mg"),
    ("nato", "wpn"): ("nato_teamlead", "nato_antitank", "nato_sniper", "nato_rifleman"),
}


def strip_comments(text: str) -> str:
    """MI comment-stripped view. The headers quote bad forms as cautionary examples,
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
    return block_at(code, code.index('{"enemy_defense/%s"' % name))


def define_body(code: str, name: str) -> str:
    """Return the whole balanced (define "name" ... ) form, nested calls included."""
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


class EnemyDefenseSupportTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.engine = ENGINE.read_text(encoding="utf-8")
        cls.templates = TEMPLATES.read_text(encoding="utf-8")
        cls.conquest = CONQUEST.read_text(encoding="utf-8")
        cls.deploy = DEPLOY.read_text(encoding="utf-8")
        cls.code = strip_comments(cls.engine)
        cls.tpl_code = strip_comments(cls.templates)

    # ---------------------------------------------------------------- structure

    def test_trigger_inventory_is_exactly_what_the_design_calls_for(self) -> None:
        names = re.findall(r'\{"enemy_defense/([a-z0-9_]+)"', self.code)
        self.assertEqual(len(names), len(set(names)), "duplicate trigger name")
        expected = {"init", "trickle", "surge"}
        expected |= {"patrol_%d" % n for n in (1, 2, 3, 4)}
        expected |= {
            "%s_%s" % (key, suffix)
            for key, _army, _side in FACTIONS
            for suffix, _cmd, _role, _take, _stage in DRAWS
        }
        self.assertEqual(set(names), expected)
        self.assertEqual(len(names), 19)

    def test_mi_defines_are_declared_before_they_are_called(self) -> None:
        code = self.code
        for name in (
            "ed_own_to_enemy",
            "ed_place",
            "ed_assign_group",
            "ed_finish",
            "ed_resolve_army",
            "ed_claim_anchors",
            "ed_poke_light",
            "ed_poke_line",
            "ed_poke_wpn",
            "ed_pick_garrison",
            "ed_pick_light",
            "ed_pick_squad",
        ):
            definition = '(define "%s"' % name
            self.assertEqual(code.count(definition), 1, name)
            at_def = code.index(definition)
            self.assertNotIn(
                '("%s")' % name, code[:at_def], "%s called above its define" % name
            )

    def test_engine_state_is_explicitly_declared(self) -> None:
        for name in (
            "enemy_defense_armed",
            "enemy_defense_army",
            "enemy_defense_stage",
            "enemy_defense_transferred",
            "enemy_defense_wave_cmd",
            "enemy_defense_wave_num",
            "enemy_defense_waves_left",
            "enemy_defense_place",
            "enemy_defense_group",
            "enemy_defense_trickle_ok",
            "enemy_defense_trickle_busy",
            "enemy_defense_surge_ok",
            "enemy_defense_surge_busy",
        ):
            self.assertIn('{"%s"}' % name, self.vars)

        # Undeclared-var sweep. defense_level$ is the one exception by design: CE
        # owns it and declares it in ce/ce_vars.inc.
        declared = set(re.findall(r'\{"([a-z0-9_]+)"\}', self.vars))
        declared.add("defense_level")
        for name in sorted(set(re.findall(r'\{var "([a-z0-9_]+)\$"\}', self.code))):
            self.assertIn(name, declared, "undeclared var read: %s$" % name)

    # -------------------------------------------------------------------- gating

    def test_every_trigger_is_gated_to_human_attack_missions(self) -> None:
        # THE inertness proof. A missing gate on any one trigger means this engine
        # reinforces the wrong side on a human-DEFENCE mission.
        code = self.code
        gate = '{var "user_is_defender$"} {op "=="} {value 0}'
        for name in re.findall(r'\{"enemy_defense/([a-z0-9_]+)"', code):
            with self.subTest(trigger=name):
                block = trigger_block(code, name)
                head = block[: block.index("{actions")]
                self.assertIn(gate, head)

        # Nothing in the engine ever reads user_is_defender$ any other way, so no
        # branch can be reached on a defence mission.
        self.assertEqual(
            code.count('{var "user_is_defender$"}'), code.count(gate)
        )

        # id_1st_enemy$ is the ownership target and gates init and both spawners: no
        # live defender bot, nothing deploys.
        for name in ("init", "trickle", "surge"):
            block = trigger_block(code, name)
            self.assertIn(
                '{var "id_1st_enemy$"} {op ">"} {value 0}',
                block[: block.index("{actions")],
                name,
            )
        # It also gates all twelve composition draws.
        for key, _army, _side in FACTIONS:
            for suffix, _cmd, _role, _take, _stage in DRAWS:
                block = trigger_block(code, "%s_%s" % (key, suffix))
                self.assertIn(
                    '{var "id_1st_enemy$"} {op ">"} {value 0}',
                    block[: block.index("{actions")],
                )

        # The defence-side owner var and the friendly system's state are a different
        # world: reaching into either hands units to the wrong player.
        for forbidden in (
            '{var "id_attack_support$"}',
            '{var "attack_support_ready$"}',
            "attack_support_src",
            "attack_support_deploy",
            "allied_support_src",
            '{var "id_defenderbot$"}',
        ):
            self.assertNotIn(forbidden, code)

    def test_init_arms_once_and_waits_for_a_published_perspective(self) -> None:
        code = self.code
        init = trigger_block(code, "init")

        self.assertIn('{"1.cmp_i" {var "enemy_defense_armed$"} {op "=="} {value 0}}', init)
        self.assertIn('{"set_i" {var "enemy_defense_armed$"} {op "="} {value 1}}', init)
        # Never resets its own latch, so it cannot re-run and stack a schedule.
        self.assertNotIn('{"set_i" {var "enemy_defense_armed$"} {op "="} {value 0}}', code)

        # enemy_spawnside$ > 0 is the readiness signal. user_is_defender$ defaults to
        # 0, which reads as "human attacking" before conquest.lua has published
        # anything; enemy_spawnside$ is written immediately after it in the same
        # mission-authority call, so a positive side proves the vars are real.
        self.assertIn('{"4.cmp_i" {var "enemy_spawnside$"} {op ">"} {value 0}}', init)
        define_at = self.conquest.index("local function publishEnemySpawnSide()")
        pub = self.conquest.index("publishEnemySpawnSide()", define_at + 40)
        role = self.conquest.index('BotApi.Scene:SetVar("user_is_defender"')
        self.assertLess(role, pub, "user_is_defender must be published before the spawn side")
        self.assertIn('BotApi.Scene:SetVar("enemy_spawnside", sideNum)', self.conquest)

        # The spawners stay latched shut until the garrison has landed, otherwise
        # their conditions are already true here and they fire alongside init.
        for var in ("trickle", "surge"):
            ok_off = init.index('{"set_i" {var "enemy_defense_%s_ok$"} {op "="} {value 0}}' % var)
            busy_on = init.index('{"set_i" {var "enemy_defense_%s_busy$"} {op "="} {value 1}}' % var)
            ok_on = init.index('{"set_i" {var "enemy_defense_%s_ok$"} {op "="} {value 1}}' % var)
            busy_off = init.index('{"set_i" {var "enemy_defense_%s_busy$"} {op "="} {value 0}}' % var)
            garrison = init.index('("ed_pick_garrison")')
            self.assertLess(ok_off, garrison)
            self.assertLess(busy_on, garrison)
            self.assertLess(garrison, ok_on)
            self.assertLess(garrison, busy_off)
            self.assertIn('{"trigger" {name "enemy_defense/%s"}}' % var, init)

    # ------------------------------------------------------------ spec point 1/2

    def test_garrison_lands_on_the_active_flag_points(self) -> None:
        code = self.code
        anchors = define_body(code, "ed_claim_anchors")

        # A mission activates only ~2 of a map's 2-5 flag_point entities, so all
        # three active picks must exclude inactive, and each must exclude the earlier
        # picks so the tags land on three different flags.
        self.assertEqual(anchors.count("{state {state inactive}}"), 3)
        self.assertEqual(anchors.count("{sort {type shuffle}}"), 5)
        for n in (1, 2, 3):
            anchor = "{tag_add enemy_def_af%d}" % n
            self.assertEqual(code.count(anchor), 1)
            pick = anchors.rindex("{select {tag {tag flag}}}", 0, anchors.index(anchor))
            window = anchors[pick : anchors.index(anchor)]
            self.assertIn("{state {state inactive}}", window)
            for earlier in range(1, n):
                self.assertIn("{tag {tag enemy_def_af%d}}" % earlier, window)

        # Capture points are addressed as {tag flag}: the fpc1..fpc5 tags are absent
        # from one of the fourteen maps entirely, which left units standing still.
        self.assertNotIn("fpc", code)
        self.assertEqual(code.count("{select {tag {tag flag}}}"), 5)

        # The garrison is MOVED onto its flag. {"placement"} takes a plain {target}
        # entity selector - the form dcg_functions.inc uses for spawn helpers.
        # Placement is now staggered: ed_place is a run of single-body placements so
        # a whole fireteam is not crushed into one pile on the flag, and ed_place_one
        # carries the actual per-flag targets.
        wrapper = define_body(code, "ed_place")
        self.assertGreaterEqual(wrapper.count('("ed_place_one")'), 6)
        place = define_body(code, "ed_place_one")
        self.assertIn("{amount 1}", place)
        self.assertIn("{exclude {tag {tag enemy_def_placed}}}", place)
        for n in (1, 2, 3):
            self.assertIn(
                "{target {ignore_captured_by_user 0} {tag enemy_def_af%d}}" % n, place
            )
            self.assertIn(
                '{condition {type cmp_i} {var "enemy_defense_place$"} {op "=="} {value %d}}' % n,
                place,
            )

        # One fireteam per active flag, each guarded on that flag existing, each in
        # its own patrol group, and none of it drawn from the wave budget.
        init = trigger_block(code, "init")
        self.assertEqual(init.count('("ed_pick_garrison")'), 3)
        for n in (1, 2, 3):
            guard = init.index(
                '{condition {type entities} {selector {tag enemy_def_af%d}}}' % n
            )
            # The enclosing {"if"} block, not a fixed window: each diagnostic now
            # carries a support_debug$ gate switch that pushes the rest of the
            # branch past where a fixed slice would end.
            body = block_at(init, init.rindex('{"if"', 0, guard))
            self.assertIn('{"set_i" {var "enemy_defense_place$"} {op "="} {value %d}}' % n, body)
            self.assertIn('{"set_i" {var "enemy_defense_group$"} {op "="} {value %d}}' % n, body)
            self.assertIn('("ed_pick_garrison")', body)
        self.assertNotIn('{"set_i" {var "enemy_defense_waves_left$"} {op "-"}', init)

        # Garrison teams dig in where they land; reinforcements push for a flag.
        finish = define_body(code, "ed_finish")
        cover = finish.index('{"actor_to_cover"')
        gate = finish.rindex(
            '{condition {type cmp_i} {var "enemy_defense_place$"} {op ">"} {value 0}}',
            0,
            cover,
        )
        self.assertLess(gate, cover)
        self.assertIn(
            "{target {ignore_captured_by_user 0} {tag enemy_def_af1}}", finish[cover:]
        )

    def test_patrols_cycle_flags_half_the_time_and_roam_the_rest(self) -> None:
        code = self.code
        for n in (1, 2, 3, 4):
            with self.subTest(group=n):
                patrol = trigger_block(code, "patrol_%d" % n)
                head = patrol[: patrol.index("{actions")]
                body = patrol[patrol.index("{actions") :]
                sel = "{selector {ignore_captured_by_user 0} {tag enemy_def_p%d}}" % n

                # Runs while the group has a live member and stops when it is wiped -
                # counted on the simple selector form live units answer to.
                self.assertIn("{tag enemy_def_p%d}" % n, head)
                self.assertIn('{state "not dead"}', head)
                self.assertIn('{count {op ">"} {value 0}}', head)

                # Its own 60-120s ladder, so the four groups never move in step.
                for weight in LADDER_WEIGHTS:
                    self.assertIn("{condition {type rand} {value %s}}" % weight, body)
                for seconds in PATROL_LADDER:
                    self.assertIn('{"delay" {time %d}}' % seconds, body)

                # Self-re-arming: one cycle always ends by poking itself again.
                self.assertIn('{"trigger" {name "enemy_defense/patrol_%d"}}' % n, body)

                # ~50/50 flag-cycle versus roam. The 0.25/0.34 head of the cascade is
                # 0.25 + 0.75*0.34 = 0.505 on the active flags; everything after it
                # roams. Roam anchors are the spare (inactive) flag points and the two
                # map-edge entry waypoints - the only anchors present on all fourteen
                # maps - with waypoint "0" as the fallback where a map has no spare.
                orders = body[body.index('{"delay" {time 0.1}}') :]
                for target in ("enemy_def_af1", "enemy_def_af2", "enemy_def_r1", "enemy_def_r2"):
                    self.assertIn(
                        "{target {ignore_captured_by_user 0} {tag %s}}" % target, orders
                    )
                self.assertIn('{waypoint "attack_support_entry_a"}', orders)
                self.assertIn('{waypoint "attack_support_entry_b"}', orders)
                self.assertEqual(orders.count('{waypoint "0"}'), 2)
                self.assertIn("{condition {type rand} {value 0.4}}", orders)
                self.assertIn("{condition {type rand} {value 0.6}}", orders)

                # Every order goes to this group only, and drops the previous order.
                # Nine order actions: two active-flag branches (one with an af1
                # fallback), two roam-flag branches (each with a waypoint fallback)
                # and the two map-edge branches.
                self.assertEqual(orders.count(sel), 9)
                self.assertEqual(orders.count("{drop orders}"), 9)
                self.assertEqual(orders.count('{"action"'), 9)

                # Anchors that may be missing on a small map are guarded, so a branch
                # never issues an order at a tag with no entity behind it.
                for anchor, fallback in (
                    ("enemy_def_af2", "enemy_def_af1"),
                    ("enemy_def_r1", None),
                    ("enemy_def_r2", None),
                ):
                    guard = orders.index(
                        "{condition {type entities} {selector {tag %s}}}" % anchor
                    )
                    guarded = block_at(orders, orders.rindex('{"switch"', 0, guard))
                    self.assertIn(
                        "{target {ignore_captured_by_user 0} {tag %s}}" % anchor, guarded
                    )
                    if fallback:
                        self.assertIn(
                            "{target {ignore_captured_by_user 0} {tag %s}}" % fallback,
                            guarded,
                        )
                    else:
                        self.assertIn('{waypoint "0"}', guarded)

        # Roam anchors are the leftovers of the active-flag claim, which is exactly
        # the set of spare flag points on the map.
        anchors = define_body(code, "ed_claim_anchors")
        for n in (1, 2):
            anchor = "{tag_add enemy_def_r%d}" % n
            self.assertEqual(code.count(anchor), 1)
            pick = anchors.rindex("{select {tag {tag flag}}}", 0, anchors.index(anchor))
            window = anchors[pick : anchors.index(anchor)]
            self.assertNotIn("{state {state inactive}}", window)
            for af in (1, 2, 3):
                self.assertIn("{tag {tag enemy_def_af%d}}" % af, window)
            for earlier in range(1, n):
                self.assertIn("{tag {tag enemy_def_r%d}}" % earlier, window)

    def test_patrollers_are_tag_swapped_out_of_the_spawner_pools(self) -> None:
        code = self.code
        assign = define_body(code, "ed_assign_group")
        for n in (1, 2, 3, 4):
            self.assertIn("{tag_add enemy_def_p%d}" % n, assign)
        # Group choice is the caller's, so garrison teams and each spawner's arrivals
        # land in known groups rather than a shuffle.
        for n in (1, 2, 3):
            self.assertIn(
                '{condition {type cmp_i} {var "enemy_defense_group$"} {op "=="} {value %d}}' % n,
                assign,
            )

        # A deployed body loses its pool tag on the claim and never regains one, so a
        # spawner can never re-pick a patroller. Nothing removes the group tags or the
        # live-roster marker either.
        for n in (1, 2, 3, 4):
            self.assertNotIn("{tag_remove enemy_def_p%d}" % n, code)
        self.assertNotIn("{tag_remove enemy_def_src}", code)
        self.assertNotIn("{tag_add enemy_def_rusa_line}", code)
        # The deploy tag IS consumed at the end of every deploy, so the next claim
        # starts from an empty set instead of re-ordering the previous arrivals.
        self.assertIn("{tag_remove enemy_def_deploy}", define_body(code, "ed_finish"))

    # -------------------------------------------------------------- spec point 3

    def test_reinforcements_enter_at_the_defenders_own_map_edge(self) -> None:
        code = self.code
        # The side switch lives in the single-body placement step that ed_place now
        # repeats, so read it from there.
        place = define_body(code, "ed_place_one")
        # Note the reading of enemy_spawnside$: side 1 (a) means the DEFENDER is on
        # side a, so its reinforcements enter at attack_support_entry_a. This is the
        # opposite of the attack-support engine, which deliberately enters from the
        # side the enemy is NOT on.
        side_a = place.index('{var "enemy_spawnside$"} {op "=="} {value 1}')
        side_b = place.index('{var "enemy_spawnside$"} {op "=="} {value 2}')
        self.assertLess(side_a, side_b)
        self.assertIn('{target_waypoint "attack_support_entry_a"}', place[side_a:side_b])
        self.assertIn('{target_waypoint "attack_support_entry_b"}', place[side_b:])
        # An unpublished side falls back to a rather than stalling.
        self.assertEqual(place.count('{target_waypoint "attack_support_entry_a"}'), 2)
        self.assertEqual(place.count('{target_waypoint "attack_support_entry_b"}'), 1)

        # Placement happens before promotion on every one of the twelve draws.
        self.assertEqual(code.count('("ed_place")'), 12)
        self.assertEqual(code.count('("ed_finish")'), 12)
        self.assertLess(code.index('(define "ed_place"'), code.index('(define "ed_finish"'))

    def test_ownership_switch_covers_every_literal_player_slot(self) -> None:
        code = self.code
        own = define_body(code, "ed_own_to_enemy")
        # The engine will not accept a var in the {player} node, so all sixteen slots
        # are spelled out and matched against id_1st_enemy$ - the live defender bot on
        # an attack mission, published by conquest.lua.
        for n in range(1, 17):
            self.assertIn(
                '{condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value %d}}' % n,
                own,
            )
            self.assertIn('{player "%d"}' % n, own)
        self.assertNotIn('{player "id_1st_enemy$"}', code)
        self.assertNotIn('{player "17"}', own)
        self.assertNotIn('{player "0"}', code)
        # Ownership is handed over exactly once per deploy, after placement.
        self.assertEqual(code.count('("ed_own_to_enemy")'), 1)
        finish = define_body(code, "ed_finish")
        self.assertIn('("ed_own_to_enemy")', finish)
        self.assertIn('BotApi.Scene:SetVar("id_1st_enemy", firstEnemyId)', self.conquest)

    # -------------------------------------------------------------- spec point 4

    def test_fireteams_are_small_and_engage_on_contact_without_a_death_ride(self) -> None:
        code = self.code
        finish = define_body(code, "ed_finish")
        for marker in (
            "{tag_add enemy_def_src}",
            "{tag_remove enemy_def_tpl}",
            "{tag_remove hidden}",
            "{inactive off}",
            "{impregnability disabled}",
            "{discovered on}",
            "{control AI}",
            "{ai_move {mode enable}}",
            "{weapon_prepare on}",
            "{fire_mode open}",
            "{move_mode free}",
            # Selection is stripped so the human can never inherit these units.
            "{remove select}",
        ):
            self.assertIn(marker, finish)

        # no_retreat OFF: these are patrolling infantry that may give ground, not a
        # suicide push. The attack-support engine deliberately pins its teammates on.
        self.assertIn("{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}", finish)
        self.assertNotIn("{no_retreat on}", code)

        # Every draw is a 3-4 body fireteam.
        for _suffix, _cmd, _role, take, _stage in DRAWS:
            self.assertGreaterEqual(take, 3)
            self.assertLessEqual(take, 4)

    # ------------------------------------------------------------ spec point 5/6

    def test_two_independent_spawners_run_on_different_random_ladders(self) -> None:
        code = self.code
        for name, ladder, group, pick in (
            ("trickle", TRICKLE_LADDER, 3, "ed_pick_light"),
            ("surge", SURGE_LADDER, 4, "ed_pick_squad"),
        ):
            with self.subTest(spawner=name):
                block = trigger_block(code, name)
                head = block[: block.index("{actions")]
                body = block[block.index("{actions") :]

                # Independent latches: each spawner has its own ok/busy pair, so
                # neither can block or gate the other.
                self.assertIn(
                    '{"1.cmp_i" {var "enemy_defense_%s_ok$"} {op "=="} {value 1}}' % name, head
                )
                self.assertIn(
                    '{"2.cmp_i" {var "enemy_defense_%s_busy$"} {op "=="} {value 0}}' % name, head
                )
                self.assertIn(
                    '{"3.cmp_i" {var "enemy_defense_waves_left$"} {op ">"} {value 0}}', head
                )

                # Its own weighted rand ladder.
                for weight in LADDER_WEIGHTS:
                    self.assertIn("{condition {type rand} {value %s}}" % weight, body)
                for seconds in ladder:
                    self.assertEqual(body.count('{"delay" {time %d}}' % seconds), 1, seconds)

                # Self-re-arming, and it reports when the shared budget runs out.
                self.assertIn(
                    '{"set_i" {var "enemy_defense_%s_busy$"} {op "="} {value 1}}' % name, body
                )
                self.assertIn(
                    '{"set_i" {var "enemy_defense_%s_busy$"} {op "="} {value 0}}' % name, body
                )
                self.assertIn('{"trigger" {name "enemy_defense/%s"}}' % name, body)
                self.assertIn("ENEMY DEFENSE WAVES EXHAUSTED", body)

                # Arrivals land at the map edge in this spawner's patrol group.
                self.assertIn('{"set_i" {var "enemy_defense_place$"} {op "="} {value 0}}', body)
                self.assertIn(
                    '{"set_i" {var "enemy_defense_group$"} {op "="} {value %d}}' % group, body
                )
                self.assertIn('("%s")' % pick, body)

        # The two ladders must not share a single value, or arrivals would drift into
        # phase with each other.
        self.assertFalse(set(TRICKLE_LADDER) & set(SURGE_LADDER))
        self.assertLess(max(TRICKLE_LADDER), min(SURGE_LADDER))

    def test_live_unit_cap_defers_without_consuming_a_wave(self) -> None:
        code = self.code
        for name in ("trickle", "surge"):
            with self.subTest(spawner=name):
                block = trigger_block(code, name)
                # Counted on the simple selector form the mission scripts use for
                # live units: the advanced selector's prop/state decorations zero the
                # match on these entities, and enemy_def_src is never removed.
                self.assertIn(
                    "{selector\n"
                    "\t\t\t\t\t\t\t\t\t{ignore_captured_by_user 0}\n"
                    "\t\t\t\t\t\t\t\t\t{tag enemy_def_src}\n"
                    "\t\t\t\t\t\t\t\t\t{type human}\n"
                    '\t\t\t\t\t\t\t\t\t{state "not dead"}\n'
                    "\t\t\t\t\t\t\t\t}",
                    block,
                )
                self.assertIn('{count {op ">"} {value %d}}' % LIVE_CAP, block)

                # Anchored on the live-count condition, not on the timer title: the
                # nearest {"case"} above a title is now that diagnostic's own gate.
                defer = block_at(
                    block,
                    block.rindex(
                        '{"case"', 0, block.index('{count {op ">"} {value %d}}' % LIVE_CAP)
                    ),
                )
                self.assertIn("ENEMY DEFENSE NEAR CAP DEFER", defer)
                # A defer costs nothing: no wave consumed, no draw dispatched.
                self.assertNotIn('{"set_i" {var "enemy_defense_waves_left$"}', defer)
                self.assertNotIn('{"set_i" {var "enemy_defense_wave_num$"}', defer)
                self.assertNotIn("ed_pick_", defer)

                dispatch = block_at(
                    block, block.index('{"default"', block.index(defer) + len(defer))
                )
                self.assertIn(
                    '{"set_i" {var "enemy_defense_wave_num$"} {op "+"} {value 1}}', dispatch
                )
                self.assertIn(
                    '{"set_i" {var "enemy_defense_waves_left$"} {op "-"} {value 1}}', dispatch
                )

    def test_wave_budget_scales_with_defense_level_and_stays_proportionate(self) -> None:
        code = self.code
        init = trigger_block(code, "init")
        for level, waves in WAVE_BUDGET:
            case = init.index(
                '{condition {type cmp_i} {var "defense_level$"} {op "=="} {value %d}}' % level
            )
            body = init[case : case + 400]
            self.assertIn(
                '{"set_i" {var "enemy_defense_waves_left$"} {op "="} {value %d}}' % waves, body
            )
        # Level 1, and an unpublished level 0, land in the default.
        self.assertIn('{"set_i" {var "enemy_defense_waves_left$"} {op "="} {value 4}}', init)

        # PROPORTIONALITY. The enemy budget must not exceed the friendly attack
        # support system it is balanced against, and its live cap must stay in the
        # same bracket. Read the friendly numbers out of the shipped engine rather
        # than restating them, so the two cannot silently drift apart.
        friendly = strip_comments(WAVES.read_text(encoding="utf-8"))
        for level, waves in WAVE_BUDGET + ((1, 4),):
            self.assertIn(
                '{"set_i" {var "attack_support_waves_left$"} {op "="} {value %d}}' % waves,
                friendly,
                "enemy budget for L%d is not matched by the friendly system" % level,
            )
        friendly_cap = int(
            re.search(r'\{count \{op ">"\} \{value (\d+)\}\}', friendly).group(1)
        )
        self.assertLessEqual(LIVE_CAP - friendly_cap, 2, "enemy live cap outgrew the friendly one")

        # Both spawners draw from the one shared budget, so two ladders do not
        # silently double the total.
        self.assertEqual(
            code.count('{"set_i" {var "enemy_defense_waves_left$"} {op "-"} {value 1}}'), 2
        )

    def test_composition_pools_widen_with_the_campaign_level(self) -> None:
        code = self.code
        pick = define_body(code, "ed_pick_squad")

        def offered(block: str) -> set:
            return set(
                int(m)
                for m in re.findall(
                    r'\{"set_i" \{var "enemy_defense_wave_cmd\$"\} \{op "="\} \{value (\d)\}\}',
                    block,
                )
            )

        level_case = {}
        for level in (3, 2):
            at = pick.index(
                '{condition {type cmp_i} {var "defense_level$"} {op "=="} {value %d}}' % level
            )
            level_case[level] = block_at(pick, pick.rindex('{"case"', 0, at))
        # L3 and L2 can both reach the AT/marksman weapons team; L1 cannot.
        self.assertEqual(offered(level_case[3]), {2, 3})
        self.assertEqual(offered(level_case[2]), {2, 3})
        after_l2 = pick.index(level_case[2]) + len(level_case[2])
        level1 = block_at(pick, pick.index('{"default"', after_l2))
        self.assertEqual(offered(level1), {2})
        # L3 leads with the weapons team, L2 only mixes it in.
        self.assertIn("{condition {type rand} {value 0.6}}", level_case[3])
        self.assertIn("{condition {type rand} {value 0.34}}", level_case[2])

        # The trickle is the same light line team at every level: the escalation is
        # in the budget and the surge, not in an ever-growing drip.
        # (0 is the pool-short tail clearing the command, not a draw.)
        light = define_body(code, "ed_pick_light")
        self.assertEqual(offered(light) - {0}, {1})
        self.assertNotIn('{var "defense_level$"}', light)

        # Pool-short fallback: step down to the deepest pool, then give up on this
        # cycle rather than spin. A draw clears the command as its first action, so a
        # command still standing four seconds later means the pool could not field it.
        self.assertIn("ENEMY DEFENSE POOL SHORT - LINE TEAM INSTEAD", pick)
        self.assertIn("ENEMY DEFENSE POOL EXHAUSTED", pick)
        short = pick.index("ENEMY DEFENSE POOL SHORT - LINE TEAM INSTEAD")
        gaveup = pick.index("ENEMY DEFENSE POOL EXHAUSTED")
        self.assertLess(short, gaveup)
        self.assertIn(
            '{condition {type cmp_i} {var "enemy_defense_wave_cmd$"} {op ">"} {value 2}}',
            pick[:short],
        )
        self.assertIn(
            '{condition {type cmp_i} {var "enemy_defense_wave_cmd$"} {op ">"} {value 0}}',
            pick[short:gaveup],
        )
        for name in ("ed_pick_garrison", "ed_pick_light"):
            self.assertIn("ENEMY DEFENSE POOL EXHAUSTED", define_body(code, name))

    def test_faction_selection_is_a_bot_army_var_switch(self) -> None:
        code = self.code
        resolve = define_body(code, "ed_resolve_army")
        # SetVar is integer-only, so the mapping has to be read off conquest.lua's
        # nationMap: 1 rusa, 2 ukr, 3 nato, 4 csa, 5 sov, 6 prc, 7 frg, 8 pol.
        self.assertIn(
            "local nationMap = { rusa = 1, ukr = 2, nato = 3, csa = 4, sov = 5, prc = 6, frg = 7, pol = 8,",
            self.conquest,
        )
        self.assertIn('BotApi.Scene:SetVar("bot_army", nationMap[botNation] or 0)', self.conquest)
        # bot_army$ is published from the same mission-authority branch as
        # user_is_defender$, so it is in place by the time init runs.
        authority = self.conquest.index("if not isMissionAuthority() then return false end")
        self.assertLess(authority, self.conquest.index('SetVar("bot_army"'))

        for bot_army, army in ((2, 2), (3, 3), (4, 3), (6, 4), (7, 3)):
            at = resolve.index(
                '{condition {type cmp_i} {var "bot_army$"} {op "=="} {value %d}}' % bot_army
            )
            case = block_at(resolve, resolve.rindex('{"case"', 0, at))
            self.assertIn(
                '{"set_i" {var "enemy_defense_army$"} {op "="} {value %d}}' % army, case
            )
        # sov (5), pol (8) and an unpublished 0 fall through to the rusa pool, so the
        # engine never stalls waiting for a var it may not get.
        for absent in (1, 5, 8):
            self.assertNotIn(
                '{condition {type cmp_i} {var "bot_army$"} {op "=="} {value %d}}' % absent,
                resolve,
            )
        # Every support_debug$ gate closes with a bare {"default"}, so the switch's own
        # default is identified by having a body: {"default"} followed by a newline.
        default = block_at(resolve, resolve.rindex('{"default"\n'))
        self.assertIn('{"set_i" {var "enemy_defense_army$"} {op "="} {value 1}}', default)

        # Exactly one faction can answer a poke, so poking all four is safe.
        for suffix, _cmd, _role, _take, _stage in DRAWS:
            poke = define_body(code, "ed_poke_%s" % suffix)
            for key, _army, _side in FACTIONS:
                self.assertIn('{"trigger" {name "enemy_defense/%s_%s"}}' % (key, suffix), poke)

    def test_every_draw_is_command_gated_army_gated_and_pool_gated(self) -> None:
        code = self.code
        for key, army, _side in FACTIONS:
            for suffix, cmd, role, take, stage in DRAWS:
                name = "%s_%s" % (key, suffix)
                pool = "enemy_def_%s_%s" % (key, role)
                with self.subTest(draw=name):
                    block = trigger_block(code, name)
                    head = block[: block.index("{actions")]
                    actions = block[block.index("{actions") :]

                    # COMMAND GATING, inherited from the attack-support engine: waves
                    # keyed on entity presence alone all fired at once. Each draw has
                    # its own command value AND clears it as its first action.
                    self.assertIn(
                        '{"2.cmp_i" {var "enemy_defense_wave_cmd$"} {op "=="} {value %d}}' % cmd,
                        head,
                    )
                    self.assertIn(
                        '{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}',
                        actions[:200],
                    )
                    # ARMY GATING: only the resolved faction can answer.
                    self.assertIn(
                        '{"4.cmp_i" {var "enemy_defense_army$"} {op "=="} {value %d}}' % army,
                        head,
                    )
                    # POOL GATING: a claim strips the pool tag from the bodies it
                    # takes, so counting the tag is exactly "still parked".
                    self.assertIn("{selector {tag %s}}" % pool, head)
                    self.assertIn('{count {op ">="} {value %d}}' % take, head)
                    self.assertIn("{amount %d}" % take, actions)
                    self.assertIn("{group {select {tag {tag %s}}}}" % pool, actions)
                    self.assertIn("{tag_remove %s}" % pool, actions)
                    self.assertIn("{tag_add enemy_def_deploy}", actions)

                    # Placement then the shared deploy flow, in that order.
                    self.assertLess(
                        actions.index('("ed_place")'), actions.index('("ed_finish")')
                    )
                    # Stage reporting: <base>1 entered, <base>2 done.
                    for value in (stage + 1, stage + 2):
                        self.assertIn(
                            '{"set_i" {var "enemy_defense_stage$"} {op "="} {value %d}}' % value,
                            actions,
                        )

    # ------------------------------------------------------- pipeline regression

    def test_engine_never_clones_and_never_decorates_the_pool_selector(self) -> None:
        code = self.code
        # NO CLONING. Three promote designs (runtime tag, gamezone, player-0 identity)
        # each matched zero freshly created entities on the attack-support engine: a
        # new entity's provenance is invisible to every selector this format can
        # express. The pool originals are MOVED, so they keep the tags we put on them.
        self.assertNotIn("{clone}", code)
        self.assertNotIn('{zone {zone "gamezone"}}', code)
        self.assertNotIn("{zone ", code)

        # SELECTOR RULE: decorating the advanced selector that addresses pool units
        # zeroes the match. Live proof in one run: a bare select moved all four; the
        # same select plus a prop/state decoration matched nothing in the very next
        # action. Selecting the deploy set must stay bare.
        self.assertNotIn("{prop {prop human}}", code)
        self.assertNotIn("{include {prop human}}", code)
        self.assertNotIn("{state {state operatable}}", code)
        self.assertNotIn("{include", code)
        self.assertIn("{group {select {tag {tag enemy_def_deploy}}}}", code)
        # Every pool claim uses the bare form and nothing else.
        for key, _army, _side in FACTIONS:
            for role, _n in POOL_DEPTH:
                self.assertIn(
                    "{group {select {tag {tag enemy_def_%s_%s}}}}" % (key, role), code
                )

        # SetVar is integer-only, so every var this engine touches is an integer
        # compare or an integer assignment. A string or float var here is a silent 0.
        self.assertNotIn('{"set_s"', code)
        self.assertNotIn('{"set_f"', code)
        self.assertNotIn("{type cmp_s}", code)
        self.assertNotIn("{type cmp_f}", code)
        for value in re.findall(r'\{"set_i" \{var "[a-z0-9_]+\$"\} \{op "[-+=]"\} \{value (-?[\w.]+)\}\}', code):
            self.assertRegex(value, r"^-?\d+$", "non-integer set_i value: %s" % value)

    def test_engine_does_not_collide_with_the_attack_support_system(self) -> None:
        code = self.code
        tpl = self.tpl_code
        attack_tpl = strip_comments(ATTACK_TEMPLATES.read_text(encoding="utf-8"))

        # Separate namespaces throughout: triggers, defines, tags, state vars.
        self.assertNotIn("attack_support_", "".join(
            re.findall(r"\{tag(?:_add|_remove)? ([a-z0-9_]+)\}", code)
        ))
        for name in ("am_place_at_entry", "am_own_to_support", "am_finish_deploy"):
            self.assertNotIn(name, code)
        # The one deliberate overlap is the two entry waypoints, which are shared
        # per-map geometry rather than state.
        self.assertIn('{target_waypoint "attack_support_entry_a"}', code)

        # Separate off-map parking band, entity ids and MIDs, so the two pools cannot
        # overwrite or shadow each other in a map that includes both.
        self.assertNotIn("-35100", tpl)
        self.assertIn("{Position -9000 -35400}", tpl)
        ours = set(re.findall(r'\{Human "[^"]*" (0x[0-9a-f]+)', tpl))
        theirs = set(re.findall(r'\{(?:Entity|Human) "[^"]*" (0x[0-9a-f]+)', attack_tpl))
        self.assertFalse(ours & theirs)
        our_mids = set(int(m) for m in re.findall(r"\{MID (\d+)\}", tpl))
        their_mids = set(int(m) for m in re.findall(r"\{MID (\d+)\}", attack_tpl))
        self.assertFalse(our_mids & their_mids)
        self.assertEqual(min(our_mids), 9100)

    # ------------------------------------------------------------- template pool

    def test_pool_parks_four_faction_pools_deep_enough_for_the_budget(self) -> None:
        tpl = self.tpl_code
        self.assertEqual(tpl.count('{Able "-select"}'), PROTOTYPES)
        self.assertEqual(tpl.count("{Tags "), PROTOTYPES)
        self.assertEqual(tpl.count("{Player 0}"), PROTOTYPES)
        self.assertEqual(tpl.count('"enemy_def_tpl"'), PROTOTYPES)
        self.assertEqual(tpl.count('"hidden"'), PROTOTYPES)

        for key, _army, _side in FACTIONS:
            self.assertEqual(tpl.count('"enemy_def_%s"' % key), sum(n for _, n in POOL_DEPTH))
            for role, count in POOL_DEPTH:
                self.assertEqual(
                    tpl.count('"enemy_def_%s_%s"' % (key, role)), count, (key, role)
                )
        # The line pool is the deepest, because every short draw ends there and the
        # garrison and the trickle both feed off it.
        for key, _army, _side in FACTIONS:
            self.assertGreater(
                tpl.count('"enemy_def_%s_line"' % key), tpl.count('"enemy_def_%s_wpn"' % key)
            )

        # Deep enough for the worst realistic L3 run: three garrison fireteams plus
        # eight reinforcement waves of at most four bodies each.
        per_faction = sum(n for _, n in POOL_DEPTH)
        self.assertGreaterEqual(per_faction, 3 * 4 + max(WAVE_BUDGET, key=lambda p: p[1])[1] * 3)

        # Real breeds only, no baked loadouts, and no vehicles: the defender bot
        # already buys its own armour through the purchase economy.
        self.assertNotIn('{Human ""', tpl)
        self.assertNotIn("{Inventory", tpl)
        self.assertNotIn("{Entity ", tpl)
        self.assertNotIn("{Vehicle ", tpl)
        self.assertNotIn("{Link ", tpl)
        # Paths that do not exist in Code:X and would park absent entities.
        self.assertNotIn("era1960", self.templates)
        self.assertNotIn("新建文件夹", self.templates)

        for (key, role), roster in ROSTERS.items():
            side = dict((k, s) for k, _a, s in FACTIONS)[key]
            for breed in roster:
                path = "mp/%s/2022s/%s" % (side, breed)
                with self.subTest(breed=path):
                    self.assertIn('{Human "%s"' % path, tpl)
                    self.assertTrue(
                        (BREED_ROOT / (path + ".set")).exists(), "breed not installed: %s" % path
                    )
            # Composition of the team the pool is built from: a lead, a support
            # weapon, and riflemen. L3 flavour is the AT/marksman weapons team.
            self.assertEqual(len(roster), 4)
        for key, _army, _side in FACTIONS:
            wpn = " ".join(ROSTERS[(key, "wpn")])
            self.assertIn("antitank", wpn)
            self.assertTrue("marksman" in wpn or "sniper" in wpn, wpn)

        # Ids and MIDs stay unique, and the ranges are the fresh ones.
        ids = re.findall(r'\{Human "[^"]*" (0x[0-9a-f]+)', tpl)
        self.assertEqual(len(ids), PROTOTYPES)
        self.assertEqual(len(set(ids)), PROTOTYPES)
        mids = re.findall(r"\{MID (\d+)\}", tpl)
        self.assertEqual(len(set(mids)), PROTOTYPES)
        self.assertTrue(all(i.startswith("0xb1") for i in ids))
        self.assertEqual(tpl.count("{"), tpl.count("}"))

    # -------------------------------------------------------------- map wiring

    def test_all_cwa_maps_include_the_enemy_defence_engine_exactly_once(self) -> None:
        maps = sorted(
            p
            for p in (ROOT / "resource/map/multi").iterdir()
            if p.is_dir() and p.name.startswith("dcg_[cwa71]_")
        )
        self.assertEqual(len(maps), 14)
        for d in maps:
            mi = (d / "campaign_capture_the_flag.mi").read_text(encoding="utf-8")
            with self.subTest(map=d.name):
                self.assertEqual(mi.count('(include "../enemy_defense_support.inc")'), 1)
                self.assertEqual(mi.count('(include "../enemy_defense_templates.inc")'), 1)
                # The engine sits with the attack-support engine in the triggers
                # section; the pool sits with the attack-support pool in the entities
                # section. read_text normalises CRLF, so match on \n here.
                self.assertIn(
                    '(include "../attack_support_waves.inc")\n'
                    '\t\t\t(include "../enemy_defense_support.inc")',
                    mi,
                )
                self.assertIn(
                    '(include "../attack_support_templates.inc")\n'
                    '\t(include "../faction_support_templates.inc")\n'
                    '\t(include "../enemy_defense_templates.inc")',
                    mi,
                )
                # Both entry waypoints, which this engine reads as the DEFENDER's own
                # edge, must still be present exactly once each.
                self.assertEqual(mi.count('{"attack_support_entry_a"'), 1)
                self.assertEqual(mi.count('{"attack_support_entry_b"'), 1)
                # Waypoint "0" is the roam fallback where a map has no spare flag.
                self.assertRegex(mi, r'\{"0"\s*\r?\n\s*\{position ')
                # No stray earlier naming for this system.
                self.assertNotIn("enemy_defence", mi)
                self.assertNotIn("enemy_defense_waves.inc", mi)

    def test_deployment_ships_and_guards_the_enemy_defence_engine(self) -> None:
        for marker in (
            'resource\\map\\multi\\enemy_defense_support.inc',
            'resource\\map\\multi\\enemy_defense_templates.inc',
            '(include "../enemy_defense_support.inc")',
            '(include "../enemy_defense_templates.inc")',
            "Expected exactly one enemy-defence include in",
            "Expected exactly one enemy-defence templates include in",
            "Expected 19 enemy_defense triggers",
            "enemy_defense triggers carry the user_is_defender",
            "must park 160 prototypes",
            '{"enemy_defense/init"',
            '{"enemy_defense/patrol_1"',
            '{"enemy_defense/trickle"',
            '{"enemy_defense/surge"',
            '{var "id_1st_enemy$"}',
            '{var "bot_army$"}',
            "ENEMY DEFENSE NEAR CAP DEFER",
            "ENEMY DEFENSE POOL SHORT - LINE TEAM INSTEAD",
            "{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}",
            "must be infantry only",
            "era1960",
            "rusa\\2022s\\rus90_squadlead",
            "ukr\\2022s\\ter_squadlead",
            "prc\\2022s\\pla_squadlead",
            "nato\\2022s\\nato_squadlead",
            '{"enemy_defense_armed"}',
            '{"enemy_defense_surge_busy"}',
        ):
            self.assertIn(marker, self.deploy)

        # The new files must actually be in the copy list, not merely checked.
        copy_list = self.deploy[self.deploy.index("$files = @("):]
        copy_list = copy_list[: copy_list.index("\n)")]
        for relative in (
            "resource\\map\\multi\\enemy_defense_support.inc",
            "resource\\map\\multi\\enemy_defense_templates.inc",
        ):
            self.assertIn(relative, copy_list)
        # Index-based lookups into $files must have moved with the two new entries.
        self.assertIn("$conquestSource = Join-Path $RepoRoot $files[8]", self.deploy)
        self.assertIn("$utilitySource = Join-Path $RepoRoot $files[9]", self.deploy)
        self.assertNotIn("$conquestSource = Join-Path $RepoRoot $files[6]", self.deploy)

    def test_delimiters_are_balanced(self) -> None:
        for text in (self.engine, self.templates):
            code = strip_comments(text)
            self.assertEqual(code.count("{"), code.count("}"))
            self.assertEqual(code.count("("), code.count(")"))
        # Trigger and define bodies each parse as one balanced form.
        for name in re.findall(r'\{"enemy_defense/([a-z0-9_]+)"', self.code):
            block = trigger_block(self.code, name)
            self.assertEqual(block.count("{"), block.count("}"), name)
            self.assertIn("{condition", block)
            self.assertIn("{actions", block)


if __name__ == "__main__":
    unittest.main()
