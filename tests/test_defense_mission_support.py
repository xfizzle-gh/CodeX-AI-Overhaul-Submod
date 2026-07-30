"""Structure pins for the two human-DEFENCE mission engines.

Phase C, the quadrant that completes the support parity:

    quadrant  mission                engine                        owner
    Q1        human attacks          attack_support_waves.inc      id_attack_support$
    Q4        human attacks          enemy_defense_support.inc     id_1st_enemy$
    Q2        human DEFENDS          defense_support_waves.inc     id_defenderbot$
    Q3        human DEFENDS          enemy_attack_support.inc      id_1st_enemy$

Q2 and Q3 are the two files this module pins. Everything asserted here is either a
hard-won pipeline constraint inherited from the attack-mission pair (no cloning, bare
pool selectors, literal {player} switch, {tag flag} capture points, integer-only vars) or
a behavioural promise of the defence-mission design: nothing deploys until the real 480s
preparation phase is over, the friendly waves hold flags and dig in, the hostile waves
push them, both claim from the pools the attack-mission engines already park, and the
whole pair is provably inert on a human-ATTACK mission.

The inertness proof runs in BOTH directions here on purpose: Q2/Q3 must be inert when
user_is_defender$ == 0 and Q1/Q4 must stay inert when it is 1, because that mutual
exclusion is also what makes the pool sharing safe.
"""

from __future__ import annotations

import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
MULTI = ROOT / "resource/map/multi"
VARS = MULTI / "dcg_vars.inc"
DS = MULTI / "defense_support_waves.inc"            # Q2
EA = MULTI / "enemy_attack_support.inc"             # Q3
Q1 = MULTI / "attack_support_waves.inc"
Q4 = MULTI / "enemy_defense_support.inc"
Q1_TEMPLATES = MULTI / "attack_support_templates.inc"
Q4_TEMPLATES = MULTI / "enemy_defense_templates.inc"
CE_SETUP = MULTI / "ce/map_setup/ce_map_setup_triggers.inc"
CONQUEST = ROOT / "resource/script/multiplayer/modes/conquest.lua"
GAME_SET = ROOT / "resource/set/multiplayer/games/campaign_capture_the_flag.set"
DEPLOY = ROOT / "tools/deploy_attack_support_probe.ps1"
WORKSHOP = Path("E:/Steam/steamapps/workshop/content/400750/3636883799")

# Every include the four quadrants need in a patched map, in the order the deploy script
# lays them down. THREE entities-section pools and four triggers-section engines: Q2 and
# Q3 park nothing of their own, and faction_support_templates.inc is the shared
# player-nation pool that Q1 and Q2 both draw from.
MAP_INCLUDES = (
    '(include "../attack_support_templates.inc")',
    '(include "../faction_support_templates.inc")',
    '(include "../enemy_defense_templates.inc")',
    '(include "../attack_support_waves.inc")',
    '(include "../enemy_defense_support.inc")',
    '(include "../defense_support_waves.inc")',
    '(include "../enemy_attack_support.inc")',
)

# Faction-aware pools shared by Q1 (attack support) and Q2 (defence support). The two
# never run on the same mission - Q1 gates on user_is_defender$ == 0 and Q2 on == 1 - so
# each pool only ever has to cover ONE engine's worst case, not the sum of both.
# faction_support_army$ folds user_nation$: sov/pol join rusa, csa/frg join nato.
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
# The published fold, mapping every user_nation$ the CE setup can emit onto a pool.
# Anything else falls through to NATO, so an unpublished nation cannot stall a wave.
NATION_FOLD = {1: 1, 5: 1, 8: 1, 2: 2, 3: 3, 4: 3, 7: 3, 6: 4}

# Q2 draws: trigger suffix -> (wave_cmd, shared attack-support pool, bodies, stage base)
DS_DRAWS = (
    ("usmc", 1, "attack_support_inf_usmc", 4, 10),
    ("1ad", 2, "attack_support_inf_1ad", 4, 20),
    ("pzgd", 3, "attack_support_inf_pzgd", 6, 30),
    ("arf", 5, "attack_support_inf_arf", 4, 50),
)
# Depth of the shared NATO pool, read back below from the shipped template file.
DS_POOL_DEPTH = {"attack_support_inf_usmc": 20, "attack_support_inf_1ad": 20,
                 "attack_support_inf_pzgd": 12, "attack_support_inf_arf": 20}

# Q3 faction key -> enemy_attack_army$ value. Same fold as enemy_defense_support.inc,
# resolved into its own var so the two pool-sharing engines stay decoupled.
EA_FACTIONS = (("rusa", 1), ("ukr", 2), ("prc", 4), ("nato", 3))
# Q3 draws: trigger suffix -> (wave_cmd, shared enemy-defence pool role, bodies, stage)
EA_DRAWS = (("line", 1, "line", 4, 10), ("wpn", 2, "wpn", 4, 20))
EA_POOL_DEPTH = {"line": 24, "wpn": 16}

# Budgets. Both mirror the attack-mission pair: L1 4 / L2 6 / L3 8, level 0 -> L1.
WAVE_BUDGET = ((3, 8), (2, 6))
DS_LIVE_CAP = 14   # parity with the friendly attack-support engine
EA_LIVE_CAP = 16   # parity with the hostile enemy-defence engine

# Tightened with the faction pools, the same ~20% trim the attack-support clock took,
# so the defender is not left waiting five minutes between reinforcements. The first
# bucket is 115 rather than 110 because 110 is a DS_HOLD_LADDER value and the
# anti-synchronisation pin below forbids a recurring delay shared by the two.
DS_CLOCK_LADDER = (115, 145, 170, 200, 230)
DS_HOLD_LADDER = (90, 110, 130, 150)
DS_OPENING = (25, 32, 40)
EA_CLOCK_LADDER = (125, 165, 205, 240, 280)
EA_OPENING = (65, 80, 95)
# Weighted {type rand} cascade. 0.2/0.25/0.33/0.5 makes five buckets ~20% each;
# 0.25/0.34/0.5 makes four buckets ~25% each.
FIVE_WEIGHTS = ("0.2", "0.25", "0.33", "0.5")
FOUR_WEIGHTS = ("0.25", "0.34", "0.5")

# The simple selector form live units answer to. The advanced selector's prop/state
# decorations zero the match on these entities, which is why the caps use this shape.
def live_selector(tag: str, depth: int) -> str:
    pad = "\t" * depth
    return (
        "{selector\n"
        "%s{ignore_captured_by_user 0}\n"
        "%s{tag %s}\n"
        "%s{type human}\n"
        '%s{state "not dead"}\n'
        "%s}" % (pad, pad, tag, pad, pad, "\t" * (depth - 1))
    )


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
    return block_at(code, code.index('{"%s"' % name))


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


def trigger_names(code: str, prefix: str) -> list:
    return re.findall(r'\{"%s/([a-z0-9_]+)"' % prefix, code)


class DefenceMissionSupportTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.vars = VARS.read_text(encoding="utf-8")
        cls.ds_raw = DS.read_text(encoding="utf-8")
        cls.ea_raw = EA.read_text(encoding="utf-8")
        cls.ds = strip_comments(cls.ds_raw)
        cls.ea = strip_comments(cls.ea_raw)
        cls.q1 = strip_comments(Q1.read_text(encoding="utf-8"))
        cls.q4 = strip_comments(Q4.read_text(encoding="utf-8"))
        cls.q1_tpl = strip_comments(Q1_TEMPLATES.read_text(encoding="utf-8"))
        cls.q4_tpl = strip_comments(Q4_TEMPLATES.read_text(encoding="utf-8"))
        cls.conquest = CONQUEST.read_text(encoding="utf-8")
        cls.deploy = DEPLOY.read_text(encoding="utf-8")
        cls.maps = sorted(
            p for p in MULTI.iterdir()
            if p.is_dir() and p.name.startswith("dcg_[cwa71]_")
        )

    # ---------------------------------------------------------------- structure

    def test_trigger_inventory_is_exactly_what_the_design_calls_for(self) -> None:
        ds = trigger_names(self.ds, "defense_support")
        self.assertEqual(len(ds), len(set(ds)), "duplicate defence-support trigger")
        # The original NATO comps survive; the faction-aware pools add one trigger
        # per faction per comp, plus the one-shot flag garrison.
        self.assertEqual(
            set(ds),
            {"init", "clock", "garrison_init", "hold_1", "hold_2", "hold_3",
             "comp_usmc", "comp_1ad", "comp_pzgd", "comp_arf"} | {
                "ally_%s_%s" % (key, suffix)
                for key, _army in FACTION_ARMIES
                for suffix, _cmd, _take, _depth in FACTION_COMPS
            },
        )
        self.assertEqual(len(ds), 10 + len(FACTION_ARMIES) * len(FACTION_COMPS))
        self.assertEqual(len(ds), 34)
        # Light vehicles are attack-only: the defence engine must not grow a veh
        # trigger, and must not reference the vehicle pools at all.
        self.assertFalse([n for n in ds if "veh" in n])
        for key, _army in FACTION_ARMIES:
            self.assertNotIn("ally_sup_%s_veh" % key, self.ds)

        ea = trigger_names(self.ea, "enemy_attack")
        self.assertEqual(len(ea), len(set(ea)), "duplicate enemy-attack trigger")
        self.assertEqual(
            set(ea),
            {"init", "clock"} | {
                "%s_%s" % (key, suffix)
                for key, _army in EA_FACTIONS
                for suffix, _cmd, _role, _take, _stage in EA_DRAWS
            },
        )
        self.assertEqual(len(ea), 10)

        # Trigger namespaces are disjoint from each other and from the two
        # attack-mission engines, so no {"trigger"} poke can cross systems.
        for code in (self.ds, self.ea, self.q1, self.q4):
            spaces = set(re.findall(r'\{"([a-z_]+)/[a-z0-9_]+"', code))
            self.assertEqual(len(spaces), 1, spaces)
        self.assertEqual(
            len({"defense_support", "enemy_attack", "attack_support", "enemy_defense"}), 4
        )

    def test_mi_defines_are_declared_before_they_are_called(self) -> None:
        for code, names in (
            (self.ds, ("ds_place_at_entry", "ds_place_one", "ds_own_to_defenderbot",
                       "ds_report_owner", "ds_claim_anchors", "ds_assign_group",
                       "ds_finish", "ds_pick_composition", "ds_pick_garrison",
                       "ds_resolve_army", "ds_pick_hybrid_non_nato",
                       "ds_poke_faction_line", "ds_poke_faction_wpn",
                       "ds_poke_faction_recon", "ds_poke_faction_assault",
                       "ds_poke_faction_eng", "ds_poke_faction_manpad")),
            (self.ea, ("ea_place_at_entry", "ea_place_one", "ea_own_to_enemy",
                       "ea_resolve_army", "ea_finish", "ea_poke_line", "ea_poke_wpn",
                       "ea_pick_wave")),
        ):
            for name in names:
                with self.subTest(define=name):
                    definition = '(define "%s"' % name
                    self.assertEqual(code.count(definition), 1, name)
                    at_def = code.index(definition)
                    self.assertNotIn(
                        '("%s")' % name, code[:at_def],
                        "%s called above its define" % name,
                    )
            # No define is dead weight, and no call has no define behind it.
            defined = set(re.findall(r'\(define "([a-z0-9_]+)"', code))
            called = set(re.findall(r'\("([a-z0-9_]+)"\)', code))
            self.assertEqual(defined, set(names))
            self.assertEqual(called, defined)

    def test_defence_faction_fold_matches_the_attack_side_exactly(self) -> None:
        """Q1 and Q2 draw from the same faction pools, so they must fold user_nation$
        the same way. A disagreement would have the defence engine reach into a
        different nation's pool than the attack engine does on the same save."""
        resolve = define_body(self.ds, "ds_resolve_army")
        for nation, army in sorted(NATION_FOLD.items()):
            with self.subTest(user_nation=nation):
                at = resolve.index(
                    '{condition {type cmp_i} {var "user_nation$"} {op "=="} '
                    "{value %d}}" % nation
                )
                self.assertIn(
                    '{"set_i" {var "faction_support_army$"} {op "="} {value %d}}' % army,
                    resolve[at : at + 200],
                )
        handled = set(
            int(m) for m in re.findall(
                r'\{condition \{type cmp_i\} \{var "user_nation\$"\} \{op "=="\} '
                r"\{value (\d+)\}\}",
                resolve,
            )
        )
        self.assertEqual(handled, set(NATION_FOLD))
        # Fail closed to NATO, same as the attack engine.
        self.assertIn(
            '{"set_i" {var "faction_support_army$"} {op "="} {value 3}}',
            resolve[resolve.rindex('{"default"'):],
        )
        # Both consumers resolve before they read, so neither can pick on a zero.
        for holder in ("ds_pick_composition", "ds_pick_garrison"):
            body = define_body(self.ds, holder)
            self.assertIn('("ds_resolve_army")', body)
            self.assertLess(body.index('("ds_resolve_army")'),
                            body.index('{"switch"'), holder)
        # The garrison is the earliest consumer on a defence mission. It waits well
        # past the ~1s at which dcg/player_nation publishes user_nation$: the arming
        # trigger delays before it reaches the first pick.
        garrison = trigger_block(self.ds, "defense_support/garrison_init")
        before = garrison[: garrison.index('("ds_pick_garrison")')]
        waits = [float(m) for m in re.findall(r'\{"delay" \{time ([\d.]+)\}\}', before)]
        self.assertGreaterEqual(sum(waits), 5.0,
                                "garrison could read user_nation$ before it is published")

    def test_engine_state_is_explicitly_declared(self) -> None:
        for name in (
            "defense_support_armed",
            "defense_support_transferred",
            "defense_support_stage",
            "defense_support_wave_cmd",
            "defense_support_wave_num",
            "defense_support_waves_left",
            "defense_support_busy",
            "defense_support_next_ok",
            "defense_support_group",
            "defense_support_owner_fail",
            "enemy_attack_armed",
            "enemy_attack_army",
            "enemy_attack_stage",
            "enemy_attack_transferred",
            "enemy_attack_wave_cmd",
            "enemy_attack_wave_num",
            "enemy_attack_waves_left",
            "enemy_attack_busy",
            "enemy_attack_next_ok",
            "enemy_attack_owner_fail",
        ):
            self.assertIn('{"%s"}' % name, self.vars)

        # UNDECLARED-VAR SWEEP. An undeclared read is a silent zero, which on an
        # *_armed$ latch means the engine re-arms every tick. defense_level$ is the one
        # exception by design: CE owns it and declares it in ce/ce_vars.inc.
        declared = set(re.findall(r'\{"([a-z0-9_]+)"\}', self.vars))
        declared.add("defense_level")
        for code in (self.ds, self.ea):
            for name in sorted(set(re.findall(r'\{var "([a-z0-9_]+)\$"\}', code))):
                self.assertIn(name, declared, "undeclared var read: %s$" % name)

        # No var is declared for these engines and then never read.
        read = set(re.findall(r'\{var "([a-z0-9_]+)\$"\}', self.ds + "\n" + self.ea))
        for prefix in ("defense_support_", "enemy_attack_"):
            for name in sorted(re.findall(r'\{"(%s[a-z0-9_]+)"\}' % prefix, self.vars)):
                self.assertIn(name, read, "declared but never read: %s$" % name)

    # -------------------------------------------------------- inertness, both ways

    def test_every_defence_engine_trigger_is_gated_to_defence_missions(self) -> None:
        # THE inertness proof, direction one. A missing gate on any one trigger means
        # these engines fire on a human-ATTACK mission, where they would reinforce the
        # wrong side AND contend with the attack-mission engines for the same pools.
        gate = '{var "user_is_defender$"} {op "=="} {value 1}'
        for code, prefix in ((self.ds, "defense_support"), (self.ea, "enemy_attack")):
            for name in trigger_names(code, prefix):
                with self.subTest(trigger="%s/%s" % (prefix, name)):
                    block = trigger_block(code, "%s/%s" % (prefix, name))
                    self.assertIn(gate, block[: block.index("{actions")])
            # Nothing reads user_is_defender$ any other way, so no branch inside a
            # trigger can be reached on an attack mission either.
            self.assertEqual(code.count('{var "user_is_defender$"}'), code.count(gate))
            self.assertNotIn('{var "user_is_defender$"} {op "=="} {value 0}', code)

    def test_attack_mission_engines_stay_gated_the_other_way(self) -> None:
        # THE inertness proof, direction two - and the regression that keeps the pool
        # sharing safe. Q2 claims from the pool Q1 owns and Q3 from the pools Q4 owns,
        # which is only sound because a mission has exactly one value of
        # user_is_defender$ and each pair of claimants sits on opposite sides of it.
        gate = '{var "user_is_defender$"} {op "=="} {value 0}'
        for code, prefix in ((self.q1, "attack_support"), (self.q4, "enemy_defense")):
            names = trigger_names(code, prefix)
            self.assertTrue(names)
            for name in names:
                with self.subTest(trigger="%s/%s" % (prefix, name)):
                    block = trigger_block(code, "%s/%s" % (prefix, name))
                    self.assertIn(gate, block[: block.index("{actions")])
            self.assertEqual(code.count('{var "user_is_defender$"}'), code.count(gate))
            self.assertNotIn('{var "user_is_defender$"} {op "=="} {value 1}', code)

    def test_neither_defence_engine_reaches_into_another_systems_state(self) -> None:
        # Q2 owns to the friendly defender bot and Q3 to the enemy attacker. Reading
        # each other's state, or an attack-mission engine's state, is how a wave ends up
        # handed to the wrong player or counted against the wrong cap.
        for forbidden in (
            '{var "id_1st_enemy$"}',
            '{var "id_attack_support$"}',
            '{var "attack_support_ready$"}',
            "attack_support_src",
            "attack_support_deploy",
            "enemy_def_src",
            "enemy_def_deploy",
            "enemy_attack_",
            "allied_support",
        ):
            self.assertNotIn(forbidden, self.ds, forbidden)
        for forbidden in (
            '{var "id_defenderbot$"}',
            '{var "id_attack_support$"}',
            "attack_support_src",
            "attack_support_deploy",
            "enemy_def_src",
            "enemy_def_deploy",
            "enemy_def_p1",
            "enemy_defense_",
            "defense_support_",
            "def_sup_",
            "allied_support",
        ):
            self.assertNotIn(forbidden, self.ea, forbidden)

        # Runtime tag namespaces are disjoint across all four engines except for the
        # deliberate reads into the shared pools, which are claims, not state.
        def tags(code: str) -> set:
            return set(re.findall(r"\{tag(?:_add|_remove)? ([a-z0-9_]+)\}", code))

        shared = {"flag", "hidden"}
        # Pool tags the defence engine is ALLOWED to claim from: the original NATO
        # comps plus the player-nation faction pools it now shares with Q1. These are
        # claims against parked prototypes, not another engine's runtime state - and
        # Q1 and Q2 never run on the same mission, so the claim cannot race.
        faction_pools = {"ally_sup_tpl"} | {
            "ally_sup_%s" % key for key, _army in FACTION_ARMIES
        } | {
            "ally_sup_%s_%s" % (key, suffix)
            for key, _army in FACTION_ARMIES
            for suffix, _cmd, _take, _depth in FACTION_COMPS
        }
        # Vehicle pools are attack-only, so they are deliberately NOT allow-listed
        # here: if the defence engine ever names one this test fails.
        ds_shared = set(DS_POOL_DEPTH) | {"attack_support_tpl"} | faction_pools
        ds_own = tags(self.ds) - shared - ds_shared
        ea_own = tags(self.ea) - shared - {"enemy_def_tpl"} - {
            "enemy_def_%s_%s" % (key, role)
            for key, _army in EA_FACTIONS for role in EA_POOL_DEPTH
        }
        self.assertTrue(all(t.startswith("def_sup_") for t in ds_own), ds_own)
        self.assertTrue(all(t.startswith("ea_") for t in ea_own), ea_own)
        # Every faction pool the defence engine claims must be one of the allowed
        # ones - a typo'd or invented pool tag would silently never match.
        claimed = {t for t in tags(self.ds) if t.startswith("ally_sup_")}
        self.assertTrue(claimed)
        self.assertFalse(claimed - faction_pools, claimed - faction_pools)
        self.assertFalse(ds_own & ea_own)
        self.assertFalse(ds_own & tags(self.q1))
        self.assertFalse(ea_own & tags(self.q4))

    # ------------------------------------------------------------- the prep phase

    def test_both_engines_wait_for_the_real_preparation_phase(self) -> None:
        gate = '{var "prep_inform$"} {op "=="} {value 1}'
        for code, prefix in ((self.ds, "defense_support"), (self.ea, "enemy_attack")):
            init = trigger_block(code, "%s/init" % prefix)
            self.assertIn(gate, init[: init.index("{actions")])
            # Exactly one reader: the gate. Nothing branches on it later.
            self.assertEqual(code.count('{var "prep_inform$"}'), 1)
            # And nothing pre-places: no delivery happens outside a spawner.
            self.assertNotIn('{"placement"', trigger_block(code, "%s/init" % prefix))

        # A defence mission genuinely has a prep phase to wait for.
        self.assertRegex(
            GAME_SET.read_text(encoding="utf-8"),
            r"\{preparationTime\s+480\}",
        )
        # prep_inform$ is published when it ends.
        self.assertIn("function OnPrepTimeOver()", self.conquest)
        prep_over = self.conquest.index("function OnPrepTimeOver()")
        self.assertIn(
            'BotApi.Scene:SetVar("prep_inform", 1)',
            self.conquest[prep_over : prep_over + 400],
        )
        self.assertIn(
            "BotApi.Events:Subscribe(BotApi.Events.PrepTimeOver, OnPrepTimeOver)",
            self.conquest,
        )

    def test_the_early_prep_inform_shortcut_is_attack_only(self) -> None:
        # ensureAttackPrepInform exists because human-ATTACK missions often never raise
        # PrepTimeOver. botDefender is THIS BOT's role, so "human attacks" is
        # botDefender == true - the same reading the two lines below prove. The gate was
        # inverted, which published prep_inform on the first quant of every DEFENCE
        # mission: prep then read as already over at t=0, which fires dcg_script's
        # dcg2/userdefend/prep_end during the player's own placement and would let both
        # engines in this module deploy into the preparation phase they gate on.
        self.assertIn(
            'BotApi.Scene:SetVar("user_is_defender", botDefender and 0 or 1)',
            self.conquest,
        )
        self.assertIn(
            "-- When player was defending, bot is attacker", self.conquest
        )
        body_at = self.conquest.index("local function ensureAttackPrepInform()")
        body = self.conquest[body_at : self.conquest.index("\nend", body_at)]
        self.assertIn("if not botDefender then return end", body)
        self.assertNotIn("if botDefender then return end", body)
        # Must stay above OnGameQuant: a local defined after its caller resolves to a
        # nil global and hard-crashes the bot on its first quant.
        self.assertLess(body_at, self.conquest.index("function OnGameQuant()"))

    # ------------------------------------------------------------ side resolution

    def test_each_engine_enters_from_the_side_its_own_force_holds(self) -> None:
        # enemy_spawnside$ is published from conquest.lua's mission-authority branch,
        # and the mission authority is the ENEMY bot (myId == firstEnemyId). On a
        # defence mission the enemy bot is the ATTACKER, so enemy_spawnside$ names the
        # attacker's physical side.
        self.assertIn("local function isMissionAuthority()", self.conquest)
        self.assertIn(
            "return firstEnemyId > 0 and myId == firstEnemyId", self.conquest
        )
        role = self.conquest.index('BotApi.Scene:SetVar("user_is_defender"')
        authority = self.conquest.index(
            "if not isMissionAuthority() then return false end"
        )
        pub = self.conquest.index("publishEnemySpawnSide()", role)
        self.assertLess(authority, role)
        self.assertLess(role, pub)
        self.assertIn('BotApi.Scene:SetVar("enemy_spawnside", sideNum)', self.conquest)

        # Q3 reinforces the attacker, so side 1 (a) -> entry_a. Same reading as Q4,
        # which delivers to the enemy defender's own edge on an attack mission.
        # Both engines stagger arrivals one body at a time now, so the side switch
        # lives in the single-body step that the wrapper repeats.
        place = define_body(self.ea, "ea_place_one")
        side_a = place.index('{var "enemy_spawnside$"} {op "=="} {value 1}')
        side_b = place.index('{var "enemy_spawnside$"} {op "=="} {value 2}')
        self.assertLess(side_a, side_b)
        self.assertIn('{target_waypoint "attack_support_entry_a"}', place[side_a:side_b])
        self.assertIn('{target_waypoint "attack_support_entry_b"}', place[side_b:])
        # Unpublished side falls back to a rather than stalling.
        self.assertEqual(place.count('{target_waypoint "attack_support_entry_a"}'), 2)
        self.assertEqual(place.count('{target_waypoint "attack_support_entry_b"}'), 1)

        # Q2 reinforces the player/defender, which is the side the attacker is NOT on,
        # so side 1 (a) -> entry_b. Same reading as Q1.
        place = define_body(self.ds, "ds_place_one")
        side_a = place.index('{var "enemy_spawnside$"} {op "=="} {value 1}')
        side_b = place.index('{var "enemy_spawnside$"} {op "=="} {value 2}')
        self.assertLess(side_a, side_b)
        self.assertIn('{target_waypoint "attack_support_entry_b"}', place[side_a:side_b])
        self.assertIn('{target_waypoint "attack_support_entry_a"}', place[side_b:])
        self.assertEqual(place.count('{target_waypoint "attack_support_entry_b"}'), 2)
        self.assertEqual(place.count('{target_waypoint "attack_support_entry_a"}'), 1)

        # The two defence-mission engines therefore enter from OPPOSITE edges, and each
        # matches the attack-mission engine that serves the same side.
        for a, b in ((self.ds, self.q1), (self.ea, self.q4)):
            for value, side in ((1, None), (2, None)):
                pat = '{var "enemy_spawnside$"} {op "=="} {value %d}' % value
                mine = a[a.index(pat) : a.index(pat) + 400]
                theirs = b[b.index(pat) : b.index(pat) + 400]
                for wp in ("attack_support_entry_a", "attack_support_entry_b"):
                    self.assertEqual(
                        '{target_waypoint "%s"}' % wp in mine,
                        '{target_waypoint "%s"}' % wp in theirs,
                        (value, wp),
                    )

        # Both engines also depend on enemy_spawnside$ > 0 as their readiness proof:
        # user_is_defender$ has no "unpublished" value of its own, but a positive spawn
        # side proves the whole perspective block really was written.
        for code, prefix in ((self.ds, "defense_support"), (self.ea, "enemy_attack")):
            init = trigger_block(code, "%s/init" % prefix)
            self.assertIn(
                '{var "enemy_spawnside$"} {op ">"} {value 0}',
                init[: init.index("{actions")],
            )

    # --------------------------------------------------------- ownership handover

    def test_defence_support_owns_to_the_defender_bot_and_never_guesses(self) -> None:
        own = define_body(self.ds, "ds_own_to_defenderbot")
        for n in range(1, 17):
            self.assertIn(
                '{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value %d}}' % n,
                own,
            )
            self.assertIn('{player "%d"}' % n, own)
        self.assertNotIn('{player "17"}', own)
        self.assertNotIn('{player "0"}', self.ds)
        self.assertNotIn('{player "id_defenderbot$"}', self.ds)

        # THE non-guess. Unlike the other three engines, the default branch transfers
        # nothing: on a defence mission a guessed slot could be the attacker's. It
        # records the failure and leaves the wave at player 0, where it can do no harm.
        # Every support_debug$ gate closes with a bare {"default"}, so a branch default
        # is identified by having a body: {"default"} followed by a newline.
        default = block_at(own, own.rindex('{"default"\n'))
        self.assertNotIn('{"player"', default)
        self.assertIn(
            '{"set_i" {var "defense_support_owner_fail$"} {op "="} {value 1}}', default
        )
        self.assertIn("DEFENSE SUPPORT OWNER UNRESOLVED - NO TRANSFER", default)
        self.assertEqual(own.count('{"player"'), 16)

        # id_defenderbot$ gates init, the clock and every draw, so nothing deploys
        # before the id is published.
        for name in ("init", "clock") + tuple(
            "comp_%s" % suffix for suffix, _c, _p, _t, _s in DS_DRAWS
        ):
            block = trigger_block(self.ds, "defense_support/%s" % name)
            self.assertIn(
                '{var "id_defenderbot$"} {op ">"} {value 0}',
                block[: block.index("{actions")],
                name,
            )

        # Diagnostic timer, fired once from init: timer titles cannot interpolate a var,
        # so the resolved slot is reported by spelling out all sixteen possibilities. A
        # live run reads the real number off this line, which is what the switch above
        # has to be checked against.
        report = define_body(self.ds, "ds_report_owner")
        for n in range(1, 17):
            self.assertIn("DEFENSE SUPPORT OWNER - SLOT %d" % n, report)
        self.assertIn("DEFENSE SUPPORT OWNER - UNRESOLVED", report)
        # Reported once, from garrison_init rather than init: the garrison arms as
        # soon as the defender bot and the spawn side are known, whereas init waits
        # for prep to end, so garrison_init is the first place the slot is resolved.
        garrison = trigger_block(self.ds, "defense_support/garrison_init")
        self.assertIn('("ds_report_owner")', garrison)
        self.assertEqual(self.ds.count('("ds_report_owner")'), 1)

        # id_defenderbot$ is conquest.lua's DefenderBotId, published by every bot
        # because it is perspective-neutral - the only published id that can name a
        # friendly AI defender on a defence mission.
        self.assertIn(
            "defenderBotId = resolvePositiveId(conquest.DefenderBotId, "
            "BotApi.Instance.CampaignDefenderBotId)",
            self.conquest,
        )
        self.assertIn(
            'if defenderBotId > 0 then BotApi.Scene:SetVar("id_defenderbot", '
            "defenderBotId) end",
            self.conquest,
        )
        publish = self.conquest.index("local function publishConquestIds()")
        self.assertLess(
            publish,
            self.conquest.index("if not isMissionAuthority() then return false end"),
        )

    def test_enemy_attack_owns_to_the_attacker_bot_and_never_guesses(self) -> None:
        own = define_body(self.ea, "ea_own_to_enemy")
        for n in range(1, 17):
            self.assertIn(
                '{condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value %d}}' % n,
                own,
            )
            self.assertIn('{player "%d"}' % n, own)
        self.assertNotIn('{player "0"}', self.ea)
        self.assertNotIn('{player "id_1st_enemy$"}', self.ea)
        default = block_at(own, own.rindex('{"default"\n'))
        self.assertNotIn('{"player"', default)
        self.assertIn(
            '{"set_i" {var "enemy_attack_owner_fail$"} {op "="} {value 1}}', default
        )
        self.assertIn("ENEMY ATTACK OWNER UNRESOLVED - NO TRANSFER", default)
        self.assertEqual(own.count('{"player"'), 16)

        for name in ("init", "clock") + tuple(
            "%s_%s" % (key, suffix)
            for key, _army in EA_FACTIONS
            for suffix, _c, _r, _t, _s in EA_DRAWS
        ):
            block = trigger_block(self.ea, "enemy_attack/%s" % name)
            self.assertIn(
                '{var "id_1st_enemy$"} {op ">"} {value 0}',
                block[: block.index("{actions")],
                name,
            )
        self.assertIn(
            'if firstEnemyId > 0 then BotApi.Scene:SetVar("id_1st_enemy", '
            "firstEnemyId) end",
            self.conquest,
        )

        # Ownership is handed over exactly once per deploy, after placement.
        for code, own_name, place, finish in (
            (self.ds, "ds_own_to_defenderbot", "ds_place_at_entry", "ds_finish"),
            (self.ea, "ea_own_to_enemy", "ea_place_at_entry", "ea_finish"),
        ):
            self.assertEqual(code.count('("%s")' % own_name), 1)
            self.assertIn('("%s")' % own_name, define_body(code, finish))
            self.assertLess(
                code.index('(define "%s"' % place), code.index('(define "%s"' % finish)
            )

    # ------------------------------------------------------------- pool sharing

    def test_both_engines_claim_shared_pools_and_park_nothing(self) -> None:
        # Neither file declares an entity. That is the whole point of the sharing: 224
        # prototypes are already parked for the attack-mission pair and the engines that
        # own them are inert on exactly the missions these two run on.
        for raw in (self.ds_raw, self.ea_raw):
            self.assertFalse(re.search(r"^\s*\{(Human|Entity|Vehicle) ", raw, re.M))
            self.assertNotIn("{Tags ", raw)
            self.assertNotIn("{MID ", raw)
            self.assertNotIn("{Position ", raw)
            self.assertNotIn("{Link ", raw)

        # Q2 claims the attack-support NATO pool, one draw per pool, and strips the pool
        # tag it took from - so the pool tag still means "still parked" for whichever
        # engine is live.
        for suffix, _cmd, pool, take, _stage in DS_DRAWS:
            actions = trigger_block(self.ds, "defense_support/comp_%s" % suffix)
            self.assertIn("{group {select {tag {tag %s}}}}" % pool, actions)
            self.assertIn("{tag_remove %s}" % pool, actions)
            self.assertIn("{amount %d}" % take, actions)
            self.assertEqual(self.q1_tpl.count('"%s"' % pool), DS_POOL_DEPTH[pool])
        self.assertIn("{tag_remove attack_support_tpl}", define_body(self.ds, "ds_finish"))

        # Q3 claims the four enemy-defence faction pools the same way.
        for key, _army in EA_FACTIONS:
            for suffix, _cmd, role, take, _stage in EA_DRAWS:
                pool = "enemy_def_%s_%s" % (key, role)
                actions = trigger_block(self.ea, "enemy_attack/%s_%s" % (key, suffix))
                self.assertIn("{group {select {tag {tag %s}}}}" % pool, actions)
                self.assertIn("{tag_remove %s}" % pool, actions)
                self.assertIn("{amount %d}" % take, actions)
                self.assertEqual(self.q4_tpl.count('"%s"' % pool), EA_POOL_DEPTH[role])
        self.assertIn("{tag_remove enemy_def_tpl}", define_body(self.ea, "ea_finish"))

        # DEPTH. A claim MOVES prototypes out and never returns them, so each engine's
        # reachable pools together have to field the whole L3 budget of eight waves.
        max_waves = max(w for _l, w in WAVE_BUDGET)
        ds_draws = sum(
            DS_POOL_DEPTH[pool] // take for _s, _c, pool, take, _st in DS_DRAWS
        )
        self.assertGreaterEqual(ds_draws, max_waves)
        # And the fallback pool alone has to carry a decent share, because every short
        # draw ends there.
        self.assertGreaterEqual(DS_POOL_DEPTH["attack_support_inf_usmc"] // 4, 5)
        for key, _army in EA_FACTIONS:
            ea_draws = sum(
                EA_POOL_DEPTH[role] // take for _s, _c, role, take, _st in EA_DRAWS
            )
            self.assertGreaterEqual(ea_draws, max_waves, key)
        # Q3's line pool is the fallback, so it must be the deeper of the two.
        self.assertGreater(EA_POOL_DEPTH["line"], EA_POOL_DEPTH["wpn"])

        # The pool-sharing rationale is written down where the next reader will look.
        for raw in (self.ds_raw, self.ea_raw):
            self.assertIn("POOL SHARING", raw)

    # ------------------------------------------------------- budgets and cadence

    def test_wave_budget_matches_the_attack_mission_pair(self) -> None:
        for code, prefix, var in (
            (self.ds, "defense_support", "defense_support_waves_left"),
            (self.ea, "enemy_attack", "enemy_attack_waves_left"),
        ):
            init = trigger_block(code, "%s/init" % prefix)
            for level, waves in WAVE_BUDGET:
                at = init.index(
                    '{condition {type cmp_i} {var "defense_level$"} {op "=="} '
                    "{value %d}}" % level
                )
                body = init[at : at + 400]
                self.assertIn(
                    '{"set_i" {var "%s$"} {op "="} {value %d}}' % (var, waves), body
                )
            # Level 1, and an unpublished level 0, land in the default.
            self.assertIn('{"set_i" {var "%s$"} {op "="} {value 4}}' % var, init)
            # One budget, one spawner: exactly one place decrements it per cycle, plus
            # the opening wave in init.
            self.assertEqual(code.count('{"set_i" {var "%s$"} {op "-"} {value 1}}' % var), 2)

        # PROPORTIONALITY. Read the attack-mission numbers out of the shipped engines
        # rather than restating them, so the four quadrants cannot silently drift apart.
        for level, waves in WAVE_BUDGET + ((1, 4),):
            self.assertIn(
                '{"set_i" {var "attack_support_waves_left$"} {op "="} {value %d}}' % waves,
                self.q1,
            )
            self.assertIn(
                '{"set_i" {var "enemy_defense_waves_left$"} {op "="} {value %d}}' % waves,
                self.q4,
            )

    def test_defense_level_is_published_on_defence_missions_too(self) -> None:
        # The budget above is worthless if CE only computes defense_level$ when the
        # human attacks. Its trigger is gated on defense_level$ == 0 and nothing else,
        # and it carries its own user_is_defender$ == 1 branch, which is direct evidence
        # the author expected it to run on a defence mission.
        setup = CE_SETUP.read_text(encoding="utf-8")
        block = block_at(
            setup,
            setup.index(
                '{"conquest_enhanced_mechanics/ai_defenders/set_defense_level"'
            ),
        )
        condition = block_at(block, block.index("{condition"))
        self.assertIn('{var "defense_level$"}', condition)
        self.assertNotIn("user_is_defender", condition)
        self.assertIn('{var "user_is_defender$"}', block)
        flat = re.sub(r"\s+", " ", block)
        for value in (1, 2, 3):
            self.assertIn(
                '{"set_i" {var "defense_level$"} {op "="} {value %d} }' % value, flat
            )
        # And CE declares it, which is why it is the one exception in the sweep above.
        self.assertIn(
            '{"defense_level"}', (MULTI / "ce/ce_vars.inc").read_text(encoding="utf-8")
        )

    def test_live_unit_caps_defer_without_consuming_a_wave(self) -> None:
        for code, prefix, tag, cap, marker in (
            (self.ds, "defense_support", "def_sup_src", DS_LIVE_CAP,
             "DEFENSE SUPPORT NEAR CAP DEFER"),
            (self.ea, "enemy_attack", "ea_src", EA_LIVE_CAP,
             "ENEMY ATTACK NEAR CAP DEFER"),
        ):
            with self.subTest(engine=prefix):
                block = trigger_block(code, "%s/clock" % prefix)
                self.assertIn(live_selector(tag, 9), block)
                self.assertIn('{count {op ">"} {value %d}}' % cap, block)

                # Anchored on the live-count condition, not on the timer title: the
                # nearest {"case"} above a title is now that diagnostic's own gate.
                defer = block_at(
                    block,
                    block.rindex(
                        '{"case"', 0, block.index('{count {op ">"} {value %d}}' % cap)
                    ),
                )
                self.assertIn(marker, defer)
                self.assertNotIn('{"set_i" {var "%s_waves_left$"}' % prefix, defer)
                self.assertNotIn('{"set_i" {var "%s_wave_num$"}' % prefix, defer)
                self.assertNotIn('_pick_', defer)

                dispatch = block_at(
                    block, block.index('{"default"', block.index(defer) + len(defer))
                )
                self.assertIn(
                    '{"set_i" {var "%s_wave_num$"} {op "+"} {value 1}}' % prefix, dispatch
                )
                self.assertIn(
                    '{"set_i" {var "%s_waves_left$"} {op "-"} {value 1}}' % prefix,
                    dispatch,
                )
                # The roster marker is never removed, or the cap would stop counting.
                self.assertNotIn("{tag_remove %s}" % tag, code)

        # Cap parity with the quadrant each engine mirrors: friendly caps match the
        # friendly system, hostile caps the hostile one.
        self.assertIn('{count {op ">"} {value %d}}' % DS_LIVE_CAP, self.q1)
        self.assertIn('{count {op ">"} {value %d}}' % EA_LIVE_CAP, self.q4)

    def test_cadences_are_randomized_self_rearming_and_never_synchronise(self) -> None:
        for code, prefix, ladder, opening in (
            (self.ds, "defense_support", DS_CLOCK_LADDER, DS_OPENING),
            (self.ea, "enemy_attack", EA_CLOCK_LADDER, EA_OPENING),
        ):
            with self.subTest(engine=prefix):
                clock = trigger_block(code, "%s/clock" % prefix)
                head = clock[: clock.index("{actions")]
                self.assertIn(
                    '{"1.cmp_i" {var "%s_next_ok$"} {op "=="} {value 1}}' % prefix, head
                )
                self.assertIn(
                    '{"2.cmp_i" {var "%s_busy$"} {op "=="} {value 0}}' % prefix, head
                )
                self.assertIn(
                    '{"3.cmp_i" {var "%s_waves_left$"} {op ">"} {value 0}}' % prefix, head
                )
                for weight in FIVE_WEIGHTS:
                    self.assertIn("{condition {type rand} {value %s}}" % weight, clock)
                for seconds in ladder:
                    self.assertEqual(clock.count('{"delay" {time %d}}' % seconds), 1)
                # Self-re-arming, and it reports when the budget runs out.
                self.assertIn('{"trigger" {name "%s/clock"}}' % prefix, clock)
                self.assertIn(
                    '{"set_i" {var "%s_busy$"} {op "="} {value 1}}' % prefix, clock
                )
                self.assertIn(
                    '{"set_i" {var "%s_busy$"} {op "="} {value 0}}' % prefix, clock
                )
                self.assertIn("WAVES EXHAUSTED", clock)

                # The clock stays latched shut until init has issued the opening wave,
                # otherwise its condition is already true at arming time and it fires
                # alongside init.
                init = trigger_block(code, "%s/init" % prefix)
                ok_off = init.index(
                    '{"set_i" {var "%s_next_ok$"} {op "="} {value 0}}' % prefix
                )
                ok_on = init.index(
                    '{"set_i" {var "%s_next_ok$"} {op "="} {value 1}}' % prefix
                )
                hand_over = init.index('{"trigger" {name "%s/clock"}}' % prefix)
                self.assertLess(ok_off, ok_on)
                self.assertLess(ok_on, hand_over)
                # Arms exactly once and never resets its own latch.
                self.assertIn(
                    '{"1.cmp_i" {var "%s_armed$"} {op "=="} {value 0}}' % prefix, init
                )
                self.assertIn(
                    '{"set_i" {var "%s_armed$"} {op "="} {value 1}}' % prefix, init
                )
                self.assertNotIn(
                    '{"set_i" {var "%s_armed$"} {op "="} {value 0}}' % prefix, code
                )
                for seconds in opening:
                    self.assertIn('{"delay" {time %d}}' % seconds, init)

        # Attacker pressure builds: Q3's opening lags Q2's entirely.
        self.assertLess(max(DS_OPENING), min(EA_OPENING))

        # THE anti-synchronisation pin. Q2 and Q3 are the only two engines live on the
        # same mission, so no recurring delay value may appear in both - otherwise
        # friendly and hostile arrivals would drift into phase with each other.
        def recurring(code: str) -> set:
            return set(
                int(m) for m in re.findall(r'\{"delay" \{time (\d+)\}\}', code)
            ) - set(range(0, 60))

        self.assertFalse(recurring(self.ds) & recurring(self.ea))
        self.assertFalse(set(DS_CLOCK_LADDER) & set(EA_CLOCK_LADDER))
        self.assertFalse(set(DS_CLOCK_LADDER) & set(DS_HOLD_LADDER))

    # ------------------------------------------------------- Q2 behaviour: hold

    def test_defence_support_advances_on_active_flags_and_digs_in(self) -> None:
        code = self.ds
        anchors = define_body(code, "ds_claim_anchors")
        # A mission activates only ~2 of a map's 2-5 flag_point entities, so all three
        # picks exclude inactive, and each excludes the earlier picks so the tags land
        # on three different flags.
        # Three active-flag anchors, each excluding inactive, plus two roam anchors
        # that are allowed to be any flag - five shuffled picks, three of which are
        # the inactive-excluding ones.
        self.assertEqual(anchors.count("{state {state inactive}}"), 3)
        self.assertEqual(anchors.count("{sort {type shuffle}}"), 5)
        self.assertEqual(anchors.count("{select {tag {tag flag}}}"), 5)
        for n in (1, 2):
            self.assertEqual(code.count("{tag_add def_sup_r%d}" % n), 1, n)
        for n in (1, 2, 3):
            anchor = "{tag_add def_sup_af%d}" % n
            self.assertEqual(code.count(anchor), 1)
            pick = anchors.rindex("{select {tag {tag flag}}}", 0, anchors.index(anchor))
            window = anchors[pick : anchors.index(anchor)]
            self.assertIn("{state {state inactive}}", window)
            for earlier in range(1, n):
                self.assertIn("{tag {tag def_sup_af%d}}" % earlier, window)
        # Flag activation is fixed for the mission, so the anchors are claimed once -
        # by garrison_init, which arms as soon as the defender bot and spawn side are
        # known. init carries a guarded re-claim for the case where the identity
        # arrived too late for the garrison to have run, and that branch fires only
        # when no anchor exists yet, so the anchors are never re-rolled mid-mission.
        self.assertEqual(code.count('("ds_claim_anchors")'), 2)
        self.assertIn('("ds_claim_anchors")',
                      trigger_block(code, "defense_support/garrison_init"))
        init = trigger_block(code, "defense_support/init")
        self.assertIn('("ds_claim_anchors")', init)
        reclaim = init.index('("ds_claim_anchors")')
        guard = block_at(init, init.rindex('{"switch"', 0, reclaim))
        self.assertIn(
            "{condition {type entities} {selector {tag def_sup_af1}}}", guard
        )
        # The re-claim sits in the default arm: anchors present means do nothing.
        self.assertLess(guard.index("{selector {tag def_sup_af1}}"),
                        guard.index('("ds_claim_anchors")'))
        self.assertIn('{"default"', guard[:guard.index('("ds_claim_anchors")')])

        finish = define_body(code, "ds_finish")
        for marker in (
            "{tag_add def_sup_src}",
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

        # no_retreat OFF: a defence may give ground. Q1 deliberately pins its
        # teammates on instead, and Q3 below is the assault.
        self.assertIn("{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}", finish)
        self.assertNotIn("{no_retreat on}", code)

        # Arrivals advance on the flag their hold group owns and THEN take cover -
        # arriving is not the end of the order, digging in is.
        advance = finish.rindex('{"action"')
        cover = finish.index('{"actor_to_cover"', advance)
        self.assertLess(advance, cover)
        self.assertIn("{target {ignore_captured_by_user 0} {tag def_sup_af1}}", finish)
        # Group choice is the caller's, and each group has a flag with an af1 fallback
        # for a map where the mission activated fewer flags than there are groups.
        for n in (2, 3):
            self.assertIn(
                '{condition {type cmp_i} {var "defense_support_group$"} {op "=="} '
                "{value %d}}" % n,
                finish,
            )
            guard = finish.index(
                "{condition {type entities} {selector {tag def_sup_af%d}}}" % n
            )
            guarded = block_at(finish, finish.rindex('{"switch"', 0, guard))
            self.assertIn(
                "{target {ignore_captured_by_user 0} {tag def_sup_af%d}}" % n, guarded
            )
            self.assertIn(
                "{target {ignore_captured_by_user 0} {tag def_sup_af1}}", guarded
            )

        # The deploy tag is consumed at the end of every deploy, so the next claim
        # starts from an empty set instead of re-ordering the previous arrivals.
        self.assertIn("{tag_remove def_sup_deploy}", finish)

    def test_hold_groups_redistribute_across_the_flags_on_a_modest_ladder(self) -> None:
        code = self.ds
        assign = define_body(code, "ds_assign_group")
        for n in (1, 2, 3):
            self.assertIn("{tag_add def_sup_h%d}" % n, assign)
            # A deployed body loses its pool tag on the claim and never regains one, so
            # no spawner can re-pick a holder, and nothing removes the group tags.
            self.assertNotIn("{tag_remove def_sup_h%d}" % n, code)

        for n in (1, 2, 3):
            with self.subTest(group=n):
                hold = trigger_block(code, "defense_support/hold_%d" % n)
                head = hold[: hold.index("{actions")]
                body = hold[hold.index("{actions") :]
                sel = "{selector {ignore_captured_by_user 0} {tag def_sup_h%d}}" % n

                # Runs while the group has a live member, stops when it is wiped, and
                # matches again if a later wave joins the group.
                self.assertIn(live_selector("def_sup_h%d" % n, 8), head)
                self.assertIn('{count {op ">"} {value 0}}', head)

                # Its own modest 90-150s ladder, so the three groups never move in step.
                for weight in FOUR_WEIGHTS:
                    self.assertIn("{condition {type rand} {value %s}}" % weight, body)
                for seconds in DS_HOLD_LADDER:
                    self.assertEqual(body.count('{"delay" {time %d}}' % seconds), 1)
                self.assertIn('{"trigger" {name "defense_support/hold_%d"}}' % n, body)

                # Five re-order branches, all on this group only, all dropping the
                # previous order. The per-branch entity guards were replaced by a
                # weighted cascade over five anchors: the three active-flag anchors
                # plus two roam anchors. def_sup_r1/r2 are claimed WITHOUT the
                # inactive exclusion, so they always resolve to a real flag and give
                # the cascade a target that cannot be empty on a two-flag mission.
                orders = body[body.index('{"delay" {time 0.1}}') :]
                self.assertEqual(orders.count('{"action"'), 5)
                self.assertEqual(orders.count(sel), 6)  # 5 orders + the cover beat
                self.assertEqual(orders.count("{drop orders}"), 5)
                for weight in ("0.25", "0.34", "0.5"):
                    self.assertIn("{condition {type rand} {value %s}}" % weight, orders)
                # One branch per anchor, every one advancing this group only.
                for anchor in ("def_sup_af1", "def_sup_af2", "def_sup_af3",
                               "def_sup_r1", "def_sup_r2"):
                    self.assertEqual(
                        orders.count(
                            "{target {ignore_captured_by_user 0} {tag %s}}" % anchor
                        ),
                        1,
                        anchor,
                    )
                self.assertEqual(orders.count("{action advance}"), 5)
                # Every re-order ends in cover, because this is a defence.
                self.assertIn('{"actor_to_cover"', orders)
                self.assertLess(orders.rindex('{"action"'), orders.index('{"actor_to_cover"'))

        # Successive waves rotate through the groups so the force spreads over the
        # flags rather than stacking on one.
        clock = trigger_block(code, "defense_support/clock")
        for n in (1, 2, 3):
            self.assertIn(
                '{"set_i" {var "defense_support_group$"} {op "="} {value %d}}' % n, clock
            )

    def test_defence_support_compositions_widen_with_the_campaign_level(self) -> None:
        """Mirrors the attack-support pick: a non-NATO defender draws from its own
        faction pools, NATO keeps its specialty comps with the hybrid comps injected
        at L2/L3. Neither branch may ever reach a vehicle - those are attack-only."""
        code = self.ds
        pick = define_body(code, "ds_pick_composition")
        hybrid = define_body(code, "ds_pick_hybrid_non_nato")

        def offered(block: str) -> set:
            return set(
                int(m)
                for m in re.findall(
                    r'\{"set_i" \{var "defense_support_wave_cmd\$"\} \{op "="\} '
                    r"\{value (\d+)\}\}",
                    block,
                )
            )

        def levels(body: str) -> dict:
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

        # Non-NATO: L1 line + recon, L2 adds wpn/assault/eng, L3 adds the MANPAD team.
        nn = levels(hybrid)
        self.assertEqual(offered(nn[1]), {10, 12})
        self.assertEqual(offered(nn[2]), {10, 11, 12, 13, 14})
        self.assertEqual(offered(nn[3]), {10, 11, 12, 13, 14, 15})
        # NATO: specialty comps survive, hybrids injected from L2.
        na = levels(pick)
        self.assertEqual(offered(na[1]), {1, 2, 5, 12})
        self.assertEqual(offered(na[2]), {1, 2, 3, 12, 13, 14})
        self.assertEqual(offered(na[3]), {1, 2, 3, 12, 13, 14, 15})
        # MANPAD is the L3-only unlock on both branches.
        for branch in (nn, na):
            self.assertNotIn(15, offered(branch[1]))
            self.assertNotIn(15, offered(branch[2]))
            self.assertIn(15, offered(branch[3]))
        # THE attack-only pin: no level of either branch may offer the vehicle comp,
        # and the whole engine must never name a vehicle pool.
        for branch in (nn, na):
            for lvl in (1, 2, 3):
                self.assertNotIn(16, offered(branch[lvl]))
        self.assertNotIn(16, offered(code))
        self.assertNotIn("_veh", code)

        # Pool-short fallback: step down to the player's own line pool, then give up
        # on this cycle rather than spin. A draw clears the command as its first
        # action, so a command still standing means the pool could not field it.
        short = pick.index("DEFENSE SUPPORT POOL SHORT - FACTION LINE")
        gaveup = pick.index("DEFENSE SUPPORT POOL EXHAUSTED")
        self.assertLess(short, gaveup)
        self.assertIn(
            '{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 10}}',
            pick[:short],
        )
        self.assertIn('("ds_poke_faction_line")', pick[:short])
        self.assertIn(
            '{condition {type cmp_i} {var "defense_support_wave_cmd$"} {op ">"} {value 0}}',
            pick[short:gaveup],
        )

        # The opening wave is no longer hardcoded to the rifle team: init clears the
        # command and goes through the ordinary faction-aware pick, so a non-NATO
        # defender's first arrival is its own nation's line or recon team.
        init = trigger_block(code, "defense_support/init")
        self.assertIn(
            '{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}', init
        )
        self.assertIn('("ds_pick_composition")', init)

        # The flag garrison is line-or-recon ONLY - never a weapons team, never a
        # vehicle - and it resolves the faction before it picks.
        garrison = define_body(code, "ds_pick_garrison")
        self.assertEqual(offered(garrison) - {0}, {10, 12})
        self.assertTrue(garrison.index('("ds_resolve_army")') < garrison.index('{"switch"'))
        self.assertIn('("ds_poke_faction_line")', garrison)
        self.assertIn('("ds_poke_faction_recon")', garrison)
        for banned in ("wpn", "assault", "eng", "manpad", "veh"):
            self.assertNotIn('("ds_poke_faction_%s")' % banned, garrison, banned)

    # ----------------------------------------------------- Q3 behaviour: assault

    def test_enemy_attack_mirrors_the_attack_support_order_flow(self) -> None:
        code = self.ea
        finish = define_body(code, "ea_finish")
        for marker in (
            "{tag_add ea_src}",
            "{tag_remove hidden}",
            "{inactive off}",
            "{impregnability disabled}",
            "{discovered on}",
            "{control AI}",
            "{ai_move {mode enable}}",
            "{weapon_prepare on}",
            "{fire_mode open}",
            "{move_mode free}",
            "{remove select}",
        ):
            self.assertIn(marker, finish)
        # no_retreat ON: this is the assault half of the defence mission.
        self.assertIn("{ai {no_retreat on} {advance_ratio 1} {retreat_ratio 0}}", finish)
        self.assertNotIn("{no_retreat off}", code)

        # Fresh flag picks every deploy, so successive waves do not all converge on the
        # same objective - the Q1 idiom, and the reason the tags are cleared first.
        for n in (1, 2, 3):
            self.assertIn("{tag_remove ea_flag%d}" % n, finish)
            self.assertLess(
                finish.index("{tag_remove ea_flag%d}" % n),
                finish.index("{tag_add ea_flag%d}" % n),
            )
        self.assertEqual(finish.count("{state {state inactive}}"), 3)
        self.assertEqual(finish.count("{sort {type shuffle}}"), 3)
        self.assertEqual(code.count("{select {tag {tag flag}}}"), 3)

        # Two staggered fireteams. Every draw is four bodies, so two pairs is the whole
        # wave and a third group would only ever order an empty selector.
        for n in (1, 2):
            self.assertIn("{tag_add ea_g%d}" % n, finish)
            self.assertIn("{tag_remove ea_g%d}" % n, finish)
        self.assertNotIn("ea_g3", code)
        self.assertEqual(finish.count("{amount 2}"), 1)
        for _s, _c, _r, take, _st in EA_DRAWS:
            self.assertEqual(take, 4)
        # The retired cover beat is gone. The line is broken by sending the two
        # groups to DIFFERENT flags instead: G1 always takes flag1, and G2 rolls
        # between flag3 and flag2, so the two halves of a wave never converge on the
        # same point and pile up on each other.
        self.assertNotIn('{"actor_to_cover"', finish)
        # Read each order as a whole: which group it selects, and which flag it sends
        # them to. G1's set and G2's set must be disjoint.
        by_group = {"ea_g1": set(), "ea_g2": set()}
        for m in re.finditer(r'\{"action"', finish):
            order = block_at(finish, m.start())
            grp = re.search(r"\{selector \{ignore_captured_by_user 0\} \{tag (ea_g\d)\}\}",
                            order)
            tgt = re.search(r"\{target \{ignore_captured_by_user 0\} \{tag (ea_flag\d)\}\}",
                            order)
            if grp and tgt:
                by_group[grp.group(1)].add(tgt.group(1))
        self.assertEqual(by_group["ea_g1"], {"ea_flag1"})
        self.assertEqual(by_group["ea_g2"], {"ea_flag2", "ea_flag3"})
        self.assertFalse(by_group["ea_g1"] & by_group["ea_g2"])
        # Both groups advance rather than beelining, and each drops its prior order.
        self.assertEqual(finish.count("{action advance}"), 3)
        self.assertEqual(finish.count("{drop orders}"), 3)
        self.assertIn("{tag_remove ea_deploy}", finish)

    def test_enemy_attack_faction_selection_is_its_own_bot_army_switch(self) -> None:
        code = self.ea
        resolve = define_body(code, "ea_resolve_army")
        # SetVar is integer-only, so the mapping has to be read off conquest.lua's
        # nationMap: 1 rusa, 2 ukr, 3 nato, 4 csa, 5 sov, 6 prc, 7 frg, 8 pol. On a
        # defence mission the bot it describes is the ATTACKER, which is the side this
        # engine reinforces.
        self.assertIn(
            "local nationMap = { rusa = 1, ukr = 2, nato = 3, csa = 4, sov = 5, "
            "prc = 6, frg = 7, pol = 8,",
            self.conquest,
        )
        self.assertIn(
            'BotApi.Scene:SetVar("bot_army", nationMap[botNation] or 0)', self.conquest
        )
        authority = self.conquest.index(
            "if not isMissionAuthority() then return false end"
        )
        self.assertLess(authority, self.conquest.index('SetVar("bot_army"'))

        for bot_army, army in ((2, 2), (3, 3), (4, 3), (6, 4), (7, 3)):
            at = resolve.index(
                '{condition {type cmp_i} {var "bot_army$"} {op "=="} {value %d}}' % bot_army
            )
            case = block_at(resolve, resolve.rindex('{"case"', 0, at))
            self.assertIn(
                '{"set_i" {var "enemy_attack_army$"} {op "="} {value %d}}' % army, case
            )
        # sov (5), pol (8) and an unpublished 0 fall through to the rusa pool, so the
        # engine never stalls waiting for a var it may not get.
        for absent in (1, 5, 8):
            self.assertNotIn(
                '{condition {type cmp_i} {var "bot_army$"} {op "=="} {value %d}}' % absent,
                resolve,
            )
        default = block_at(resolve, resolve.rindex('{"default"\n'))
        self.assertIn('{"set_i" {var "enemy_attack_army$"} {op "="} {value 1}}', default)

        # Its own var, resolved from the same source as enemy_defense_army$ but never
        # reading it: the two pool-sharing engines must stay decoupled.
        self.assertNotIn("enemy_defense_army", code)
        self.assertIn(
            '{"set_i" {var "enemy_defense_army$"} {op "="} {value 1}}', self.q4
        )

        # Exactly one faction can answer a poke, so poking all four is safe.
        for suffix, _cmd, _role, _take, _stage in EA_DRAWS:
            poke = define_body(code, "ea_poke_%s" % suffix)
            for key, _army in EA_FACTIONS:
                self.assertIn(
                    '{"trigger" {name "enemy_attack/%s_%s"}}' % (key, suffix), poke
                )

    def test_enemy_attack_draws_widen_with_the_level_and_fall_back_gracefully(self) -> None:
        code = self.ea
        pick = define_body(code, "ea_pick_wave")

        def offered(block: str) -> set:
            return set(
                int(m)
                for m in re.findall(
                    r'\{"set_i" \{var "enemy_attack_wave_cmd\$"\} \{op "="\} '
                    r"\{value (\d)\}\}",
                    block,
                )
            )

        level_case = {}
        for level in (3, 2):
            at = pick.index(
                '{condition {type cmp_i} {var "defense_level$"} {op "=="} {value %d}}'
                % level
            )
            level_case[level] = block_at(pick, pick.rindex('{"case"', 0, at))
        self.assertEqual(offered(level_case[3]), {1, 2})
        self.assertEqual(offered(level_case[2]), {1, 2})
        after_l2 = pick.index(level_case[2]) + len(level_case[2])
        level1 = block_at(pick, pick.index('{"default"', after_l2))
        self.assertEqual(offered(level1), {1})
        # L3 leads with the weapons team, L2 only mixes it in - same shape as Q4.
        self.assertIn("{condition {type rand} {value 0.6}}", level_case[3])
        self.assertIn("{condition {type rand} {value 0.34}}", level_case[2])
        self.assertIn("{condition {type rand} {value 0.6}}", self.q4)

        short = pick.index("ENEMY ATTACK POOL SHORT - LINE TEAM INSTEAD")
        gaveup = pick.index("ENEMY ATTACK POOL EXHAUSTED")
        self.assertLess(short, gaveup)
        self.assertIn(
            '{condition {type cmp_i} {var "enemy_attack_wave_cmd$"} {op ">"} {value 1}}',
            pick[:short],
        )
        self.assertIn(
            '{condition {type cmp_i} {var "enemy_attack_wave_cmd$"} {op ">"} {value 0}}',
            pick[short:gaveup],
        )
        # Both the opening wave and every cycle go through the one level-aware pick.
        self.assertEqual(code.count('("ea_pick_wave")'), 2)

    def test_every_draw_is_command_gated_army_gated_and_pool_gated(self) -> None:
        # COMMAND GATING, inherited from the attack-support engine: waves keyed on
        # entity presence alone all fired at once. Each draw has its own command value
        # AND clears it as its first action.
        for suffix, cmd, pool, take, stage in DS_DRAWS:
            name = "defense_support/comp_%s" % suffix
            with self.subTest(draw=name):
                block = trigger_block(self.ds, name)
                head = block[: block.index("{actions")]
                actions = block[block.index("{actions") :]
                self.assertIn(
                    '{"2.cmp_i" {var "defense_support_wave_cmd$"} {op "=="} '
                    "{value %d}}" % cmd,
                    head,
                )
                self.assertIn(
                    '{"set_i" {var "defense_support_wave_cmd$"} {op "="} {value 0}}',
                    actions[:200],
                )
                # POOL GATING: a claim strips the pool tag from the bodies it takes,
                # so counting the tag is exactly "still parked".
                self.assertIn("{selector {tag %s}}" % pool, head)
                self.assertIn('{count {op ">="} {value %d}}' % take, head)
                self.assertIn("{tag_add def_sup_deploy}", actions)
                self.assertLess(
                    actions.index('("ds_place_at_entry")'), actions.index('("ds_finish")')
                )
                for value in (stage + 1, stage + 2):
                    self.assertIn(
                        '{"set_i" {var "defense_support_stage$"} {op "="} '
                        "{value %d}}" % value,
                        actions,
                    )

        for key, army in EA_FACTIONS:
            for suffix, cmd, role, take, stage in EA_DRAWS:
                name = "enemy_attack/%s_%s" % (key, suffix)
                pool = "enemy_def_%s_%s" % (key, role)
                with self.subTest(draw=name):
                    block = trigger_block(self.ea, name)
                    head = block[: block.index("{actions")]
                    actions = block[block.index("{actions") :]
                    self.assertIn(
                        '{"2.cmp_i" {var "enemy_attack_wave_cmd$"} {op "=="} '
                        "{value %d}}" % cmd,
                        head,
                    )
                    self.assertIn(
                        '{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 0}}',
                        actions[:200],
                    )
                    # ARMY GATING: only the resolved faction can answer.
                    self.assertIn(
                        '{"4.cmp_i" {var "enemy_attack_army$"} {op "=="} '
                        "{value %d}}" % army,
                        head,
                    )
                    self.assertIn("{selector {tag %s}}" % pool, head)
                    self.assertIn('{count {op ">="} {value %d}}' % take, head)
                    self.assertIn("{tag_add ea_deploy}", actions)
                    self.assertLess(
                        actions.index('("ea_place_at_entry")'),
                        actions.index('("ea_finish")'),
                    )
                    for value in (stage + 1, stage + 2):
                        self.assertIn(
                            '{"set_i" {var "enemy_attack_stage$"} {op "="} '
                            "{value %d}}" % value,
                            actions,
                        )

    # ------------------------------------------------------- pipeline regression

    def test_engines_never_clone_and_never_decorate_the_pool_selector(self) -> None:
        for code, deploy_tag in ((self.ds, "def_sup_deploy"), (self.ea, "ea_deploy")):
            with self.subTest(deploy_tag=deploy_tag):
                # NO CLONING. Three promote designs (runtime tag, gamezone, player-0
                # identity) each matched zero freshly created entities on the
                # attack-support engine: a new entity's provenance is invisible to every
                # selector this format can express. The pool originals are MOVED, so
                # they keep the tags the template file put on them.
                self.assertNotIn("{clone}", code)
                self.assertNotIn('{zone {zone "gamezone"}}', code)
                self.assertNotIn("{zone ", code)

                # SELECTOR RULE: decorating the advanced selector that addresses pool
                # units zeroes the match. Live proof in one run: a bare select moved all
                # four; the same select plus a prop/state decoration matched nothing in
                # the very next action.
                self.assertNotIn("{prop {prop human}}", code)
                self.assertNotIn("{include {prop human}}", code)
                self.assertNotIn("{state {state operatable}}", code)
                self.assertNotIn("{include", code)
                self.assertIn("{group {select {tag {tag %s}}}}" % deploy_tag, code)

                # Capture points are addressed as {tag flag}: the fpc1..fpc5 tags are
                # absent from one of the fourteen maps entirely, which left units
                # standing still on a live run.
                self.assertNotIn("fpc", code)

                # SetVar is integer-only, so every var these engines touch is an integer
                # compare or an integer assignment. A string or float var is a silent 0.
                self.assertNotIn('{"set_s"', code)
                self.assertNotIn('{"set_f"', code)
                self.assertNotIn("{type cmp_s}", code)
                self.assertNotIn("{type cmp_f}", code)
                for value in re.findall(
                    r'\{"set_i" \{var "[a-z0-9_]+\$"\} \{op "[-+=]"\} \{value (-?[\w.]+)\}\}',
                    code,
                ):
                    self.assertRegex(value, r"^-?\d+$", "non-integer set_i: %s" % value)

                # {"placement"} happens before promotion, and nothing is placed at a raw
                # coordinate.
                self.assertNotIn("{position ", code)

    # --------------------------------------------------------------- map wiring

    def test_all_cwa_maps_wire_all_four_quadrants_exactly_once(self) -> None:
        self.assertEqual(len(self.maps), 14)
        for d in self.maps:
            mi = (d / "campaign_capture_the_flag.mi").read_text(encoding="utf-8")
            with self.subTest(map=d.name):
                for include in MAP_INCLUDES:
                    self.assertEqual(mi.count(include), 1, include)
                # read_text normalises CRLF, so match on \n here. The four engines sit
                # together in a fixed order in the triggers section, attack-mission pair
                # first, and the two pools sit together in the entities section.
                self.assertIn(
                    '(include "../attack_support_waves.inc")\n'
                    '\t\t\t(include "../enemy_defense_support.inc")\n'
                    '\t\t\t(include "../defense_support_waves.inc")\n'
                    '\t\t\t(include "../enemy_attack_support.inc")',
                    mi,
                )
                self.assertIn(
                    '(include "../attack_support_templates.inc")\n'
                    '\t(include "../faction_support_templates.inc")\n'
                    '\t(include "../enemy_defense_templates.inc")',
                    mi,
                )
                # ZERO legacy allied-support references: both .inc files are deleted, so
                # a surviving include is a dangling reference the engine cannot resolve,
                # and the entry waypoint it cloned into has no readers left.
                self.assertNotIn("allied_support", mi)
                # Both entry waypoints, which the four engines read in two different
                # directions, must be present exactly once each.
                self.assertEqual(mi.count('{"attack_support_entry_a"'), 1)
                self.assertEqual(mi.count('{"attack_support_entry_b"'), 1)
                # Waypoint "0" is the enemy-defence patrol roam fallback.
                self.assertRegex(mi, r'\{"0"\s*\r?\n\s*\{position ')
                # No stray naming for the two new systems.
                self.assertNotIn("defense_support_templates", mi)
                self.assertNotIn("enemy_attack_templates", mi)

    def test_border_declares_engine_state_via_the_shared_var_block(self) -> None:
        # CLOSED GAP. border used to be the one map that declared no engine state: it
        # carried an inline eleven-var block instead of dcg_vars.inc, an undeclared MI
        # var read is a silent zero, so every engine gate failed (user_is_defender$
        # fails Q2/Q3's == 1 gate at 0, the owner ids fail Q1/Q4's > 0 gate at 0) and
        # all four quadrants were inert there. The fix swaps the inline block for the
        # shared include on border too, so all fourteen maps now declare the same
        # engine state.
        for d in self.maps:
            mi = (d / "campaign_capture_the_flag.mi").read_text(encoding="utf-8")
            with self.subTest(map=d.name):
                self.assertEqual(mi.count('(include "../dcg_vars.inc")'), 1)

        border = (
            MULTI / "dcg_[cwa71]_border/campaign_capture_the_flag.mi"
        ).read_text(encoding="utf-8")
        # The whole vars block, byte for byte: the shared include plus "balance", the
        # one inline var dcg_vars.inc does not declare. Keeping balance inline next to
        # the include means nothing is ever declared twice, which keeps us off the
        # untested question of whether the MI parser tolerates duplicate declarations.
        # (read_text normalises CRLF, so match on \n here.)
        self.assertIn(
            "\t\t{vars\n"
            '\t\t\t(include "../dcg_vars.inc")\n'
            '\t\t\t{"balance"}\n'
            "\t\t}",
            border,
        )
        self.assertEqual(border.count('{"balance"}'), 1)
        # Every name the shared block declares comes in via the include and ONLY via
        # the include: no survivor from the old inline block may remain as a literal.
        shared = re.findall(r'\{"([a-z0-9_]+)"\}', VARS.read_text(encoding="utf-8"))
        self.assertIn("user_is_defender", shared)
        self.assertNotIn("balance", shared)
        for name in shared:
            self.assertNotIn('{"%s"}' % name, border, name)
        # Deliberately NOT dcg_script.inc: border runs nikral's trigger set where the
        # other maps run the DCG one, and the four engines depend on nothing in
        # dcg_script.inc - their perspective gates (user_is_defender$, the owner ids,
        # prep_inform$, enemy_spawnside$) are published from conquest.lua, they arm
        # themselves, and defense_level$ comes from CE's dummy entity. All of that is
        # map-agnostic, so the var declarations alone are what border was missing.
        self.assertNotIn('(include "../dcg_script.inc")', border)
        self.assertEqual(border.count('(include "/map/nikral\'s trigger.mi")'), 1)
        for include in MAP_INCLUDES:
            self.assertEqual(border.count(include), 1, include)

    def test_no_orphans_in_the_repo_or_the_workshop(self) -> None:
        # The retired experiment is gone from the repo, includes and all.
        for retired in ("allied_support_waves.inc", "allied_support_templates.inc"):
            self.assertFalse((MULTI / retired).exists(), retired)
        # And nothing anywhere in the mod still names it, except the deploy script,
        # which has to keep the names in order to strip them.
        # The only surviving mentions anywhere are the retirement machinery itself: the
        # deploy script has to keep the names in order to strip them, and the tests have
        # to keep them in order to forbid them. No shipped .inc may name it. CE's own
        # allied_support_template tag is unrelated - CE tag_adds it to ten of its own
        # off-map reserve entities and then only ever excludes them from a cleanup
        # sweep, so it is self-contained and stays untouched.
        for path in sorted(MULTI.glob("*.inc")):
            self.assertNotIn(
                "allied_support", path.read_text(encoding="utf-8", errors="ignore"),
                str(path),
            )
        ce_setup = CE_SETUP.read_text(encoding="utf-8")
        self.assertEqual(ce_setup.count("{tag_add allied_support_template}"), 1)
        for m in re.finditer(r"\{tag\s+\{tag\s+allied_support_template\}\s*\}", ce_setup):
            self.assertIn("{exclude", ce_setup[max(0, m.start() - 400) : m.start()])
        # Every .inc the maps include exists, and every engine .inc is included.
        for include in MAP_INCLUDES:
            name = re.search(r'\.\./([\w.]+)', include).group(1)
            self.assertTrue((MULTI / name).exists(), name)

        if WORKSHOP.exists():
            wmulti = WORKSHOP / "resource/map/multi"
            for retired in ("allied_support_waves.inc", "allied_support_templates.inc"):
                self.assertFalse((wmulti / retired).exists(), "workshop orphan: " + retired)
            for include in MAP_INCLUDES:
                name = re.search(r'\.\./([\w.]+)', include).group(1)
                self.assertTrue((wmulti / name).exists(), "workshop missing: " + name)
            wmaps = sorted(
                p for p in wmulti.iterdir()
                if p.is_dir() and p.name.startswith("dcg_[cwa71]_")
            )
            self.assertEqual(len(wmaps), 14)
            for d in wmaps:
                mi = (d / "campaign_capture_the_flag.mi").read_text(encoding="utf-8")
                with self.subTest(workshop_map=d.name):
                    for include in MAP_INCLUDES:
                        self.assertEqual(mi.count(include), 1, include)
                    # Engine state declaration, border included: the deploy script
                    # converts border's inline vars block into the shared include.
                    self.assertEqual(mi.count('(include "../dcg_vars.inc")'), 1)
                    self.assertNotIn("allied_support", mi)

    def test_deployment_ships_and_guards_both_defence_mission_engines(self) -> None:
        for marker in (
            "resource\\map\\multi\\defense_support_waves.inc",
            "resource\\map\\multi\\enemy_attack_support.inc",
            '(include "../defense_support_waves.inc")',
            '(include "../enemy_attack_support.inc")',
            '{"defense_support/init"',
            '{"defense_support/hold_1"',
            '{"enemy_attack/init"',
            '{"enemy_attack/nato_wpn"',
            '{var "id_defenderbot$"}',
            '{var "prep_inform$"}',
            "DEFENSE SUPPORT NEAR CAP DEFER",
            "DEFENSE SUPPORT OWNER UNRESOLVED - NO TRANSFER",
            "ENEMY ATTACK NEAR CAP DEFER",
            "ENEMY ATTACK POOL SHORT - LINE TEAM INSTEAD",
            '{"defense_support_armed"}',
            '{"defense_support_owner_fail"}',
            '{"enemy_attack_armed"}',
            '{"enemy_attack_owner_fail"}',
            # Inertness, prep gating, no-park and no-cross-read guards.
            "triggers carry the user_is_defender$ == 1 gate",
            "does not gate on prep_inform$ == 1",
            "It must claim from the existing parked pools, not park its own",
            "reaches into other-system state",
            "share cadence values",
            # Legacy retirement.
            "resource\\map\\multi\\allied_support_waves.inc",
            "resource\\map\\multi\\allied_support_templates.inc",
            "LEGACY-STRIPPED retired allied support from",
            "Workshop still carries the retired allied-support file",
            "Map still references the retired allied-support experiment",
            # The corrected prep-inform gate has to ship with the engines.
            "if not botDefender then return end",
            "still carries the inverted ensureAttackPrepInform gate",
            # Border vars-block conversion: the one map whose engine state was inline
            # gets the shared include, keeping its map-local balance var, and every
            # map must end up with the include exactly once.
            "BORDER-VARS converted inline vars block in",
            "Inline vars block is missing the map-local balance var",
            "Expected exactly one dcg_vars.inc include in",
            '(include "../dcg_vars.inc")',
        ):
            self.assertIn(marker, self.deploy)

        # The new files must actually be in the copy list, not merely checked, and the
        # index-based lookups must still point at the same files as before.
        copy_list = self.deploy[self.deploy.index("$files = @("):]
        copy_list = copy_list[: copy_list.index("\n)")]
        for relative in (
            "resource\\map\\multi\\defense_support_waves.inc",
            "resource\\map\\multi\\enemy_attack_support.inc",
        ):
            self.assertIn(relative, copy_list)
        self.assertIn("$conquestSource = Join-Path $RepoRoot $files[8]", self.deploy)
        self.assertIn("$utilitySource = Join-Path $RepoRoot $files[9]", self.deploy)
        self.assertIn("$dsSource = Join-Path $RepoRoot $files[10]", self.deploy)
        self.assertIn("$eaSource = Join-Path $RepoRoot $files[11]", self.deploy)
        # Neither engine gets a templates include: they claim from the shared pools.
        self.assertNotIn("defense_support_templates", self.deploy)
        self.assertNotIn("enemy_attack_templates", self.deploy)

    def test_delimiters_are_balanced(self) -> None:
        for raw in (self.ds_raw, self.ea_raw):
            code = strip_comments(raw)
            self.assertEqual(code.count("{"), code.count("}"))
            self.assertEqual(code.count("("), code.count(")"))
        # Trigger and define bodies each parse as one balanced form.
        for code, prefix in ((self.ds, "defense_support"), (self.ea, "enemy_attack")):
            for name in trigger_names(code, prefix):
                block = trigger_block(code, "%s/%s" % (prefix, name))
                self.assertEqual(block.count("{"), block.count("}"), name)
                self.assertIn("{condition", block)
                self.assertIn("{actions", block)
            for name in re.findall(r'\(define "([a-z0-9_]+)"', code):
                body = define_body(code, name)
                self.assertEqual(body.count("{"), body.count("}"), name)
                self.assertEqual(body.count("("), body.count(")"), name)


if __name__ == "__main__":
    unittest.main()
