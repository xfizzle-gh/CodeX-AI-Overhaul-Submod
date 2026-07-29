from __future__ import annotations

import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
CONQUEST_LUA = ROOT / "resource/script/multiplayer/modes/conquest.lua"


class ConquestRuntimeSourceTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.source = CONQUEST_LUA.read_text(encoding="utf-8")

    def test_engine_ids_are_resolved_and_published(self) -> None:
        self.assertIn("resolvePositiveId(conquest.FirstEnemyId", self.source)
        self.assertIn("resolvePositiveId(conquest.DefenderBotId", self.source)
        self.assertIn("resolvePositiveId(conquest.FirstPlayerId", self.source)
        self.assertIn('BotApi.Scene:SetVar("id_defenderbot", defenderBotId)', self.source)
        self.assertIn('"defenderBotPurchaseHost", false', self.source)

    def test_hidden_defender_owner_has_no_purchase_branch(self) -> None:
        forbidden = (
            "isAlliedDefenderBot",
            "DCGWaveOffMin_AlliedSupport",
            "DCGWaveOffMax_AlliedSupport",
            "Min_AlliedSupport",
            "Max_AlliedSupport",
            "DCG allied support",
            "allied-prep-hold",
            "allied-opening",
        )
        for marker in forbidden:
            self.assertNotIn(marker, self.source)

    def test_runtime_bot_cadences_remain_separate(self) -> None:
        self.assertIn('cadence = "enemy-defender-opening"', self.source)
        self.assertIn('cadence = "enemy-attacker-opening"', self.source)
        self.assertIn('cadence = "enemy-defender"', self.source)
        self.assertIn('cadence = "enemy-attacker"', self.source)

    def test_normal_calculated_waves_keep_global_reduction(self) -> None:
        # The scale itself is a tuning knob; what this test protects is that a
        # global reduction is still applied to every calculated wave and that the
        # floor of 3 survives. Update the pin when the knob moves - do not drop it.
        self.assertIn("local NormalWaveSizeScale = 0.765", self.source)
        self.assertIn(
            "rawWaveTotal * ActiveDifficultySettings.waveScale * NormalWaveSizeScale",
            self.source,
        )
        self.assertIn("waveUnitTotal = math.max(3", self.source)

    def test_wave_transition_advances_before_recalculation(self) -> None:
        transition = self.source.index("waveNumber = waveNumber + 1")
        recalculation = self.source.index("calculateWaveUnitTotal()", transition)
        self.assertLess(transition, recalculation)
        self.assertNotIn("if not botDefender or botDefender then", self.source)
        self.assertIn("waveSpawnPossible = true", self.source)

    def test_only_mission_authority_writes_perspective_vars(self) -> None:
        authority_guard = self.source.index(
            "if not isMissionAuthority() then return false end"
        )
        perspective_var = self.source.index(
            'BotApi.Scene:SetVar("user_is_defender"', authority_guard
        )
        ce_vars = self.source.index("SetCEMissionVariables(botDefender)", authority_guard)
        self.assertLess(authority_guard, perspective_var)
        self.assertLess(authority_guard, ce_vars)
        self.assertIn(
            "if wroteMissionVars then setDocVarsInNattorSpeak(currentDivision) end",
            self.source,
        )

    def test_ai_purchase_ownership_and_orders_are_preserved(self) -> None:
        self.assertIn("BotApi.Commands:SpawnAt", self.source)
        self.assertIn("BotApi.Commands:Spawn(unit, maxSquadSize)", self.source)
        self.assertIn("TrySpawnUnit()", self.source)
        self.assertIn("BotApi.Commands:SeekAndDestroy", self.source)
        self.assertIn("BotApi.Commands:CaptureFlag", self.source)
        self.assertNotIn("control user", self.source)

    def test_first_quant_retries_late_conquest_ids_once(self) -> None:
        self.assertIn(
            "firstEnemyId <= 0 or defenderBotId <= 0 or firstPlayerId <= 0",
            self.source,
        )
        self.assertIn("local function retryMissionIdentityOnce()", self.source)
        self.assertIn("missionIdentityRetryPending = false", self.source)

    def test_prep_event_only_updates_mission_and_enemy_attack_release(self) -> None:
        start = self.source.index("function OnPrepTimeOver()")
        end = self.source.index("BotApi.Events:Subscribe", start)
        prep = self.source[start:end]
        self.assertIn('BotApi.Scene:SetVar("prep_inform", 1)', prep)
        self.assertIn("if not botDefender and not ai_attack_started then", prep)
        self.assertNotIn("KillSpawnCooldownTimer()", prep)
        self.assertNotIn("SetSpawnCooldownTimer()", prep)

    def test_prep_inform_helper_is_defined_before_the_quant_that_calls_it(self) -> None:
        # Lua resolves an undeclared local to a nil global: defining
        # ensureAttackPrepInform after OnGameQuant crashes the bot on its first quant.
        definition = self.source.index("local function ensureAttackPrepInform")
        quant = self.source.index("function OnGameQuant")
        self.assertLess(definition, quant)

        body_start = self.source.index("function OnGameQuant")
        body_end = self.source.index("\nfunction ", body_start + 1)
        quant_body = self.source[body_start:body_end]
        self.assertIn("ensureAttackPrepInform()", quant_body)

    def test_enemy_spawn_side_is_published_by_the_mission_authority(self) -> None:
        # The dynamic campaign swaps attacker/defender spawn sides per mission
        # instance, so attack-side scripts cannot rely on a static entry point.
        # utility.lua derives spawnSide from BotApi.Instance.spawnPointName.
        definition = self.source.index("local function publishEnemySpawnSide")
        # Skip past the definition line itself - it also contains "name()".
        call = self.source.index(
            "\tpublishEnemySpawnSide()", definition + 1
        )
        self.assertLess(definition, call)
        self.assertNotIn("publishEnemySpawnSide(", self.source[:definition])

        # It must be a SIBLING of publishConquestIds, not nested inside it. Nested,
        # the mission-script publish resolved to nil and hard-crashed enemy bot
        # init - there must be exactly one `local function` between the two.
        ids = self.source.index("local function publishConquestIds()")
        self.assertLess(ids, definition)
        self.assertEqual(self.source[ids:definition].count("local function"), 1)

        # One writer, and it always writes a NUMBER. Handing Scene:SetVar a nil
        # native-faults, so an unresolvable side publishes 0 rather than nothing.
        self.assertEqual(self.source.count('BotApi.Scene:SetVar("enemy_spawnside"'), 1)
        self.assertIn('BotApi.Scene:SetVar("enemy_spawnside", sideNum)', self.source)
        body = self.source[definition:self.source.index("\nlocal ", definition + 1)]
        self.assertIn("local sideNum = 0", body)
        self.assertIn('if side == "a" or side == "A" then', body)
        self.assertIn('elseif side == "b" or side == "B" then', body)
        self.assertIn("sideNum = 1", body)
        self.assertIn("sideNum = 2", body)
        # The spawn-point fallback is type-guarded before string.sub, which
        # faulted when the engine reported no spawn point at all.
        self.assertIn('if type(sp) == "string" and #sp > 0 then', body)

        # One writer only: it must sit below the mission-authority gate, with the
        # other enemy-perspective vars.
        authority = self.source.index(
            "if not isMissionAuthority() then return false end"
        )
        self.assertLess(authority, call)

    def test_scatter_order_helpers_are_defined_before_use(self) -> None:
        # Same nil-global trap as ensureAttackPrepInform: these are `local
        # function`s, so a call sited above the definition resolves to a nil
        # global and crashes the bot the moment that path runs.
        for helper in (
            "IsSquadReserved",
            "IssueScatterOrder",
            "ScheduleSpawnOrderNudge",
            "IsSquadActive",
        ):
            definition = self.source.index("local function %s" % helper)
            call = self.source.index("%s(" % helper, definition + 1)
            self.assertLess(definition, call, "%s called before definition" % helper)
            self.assertNotIn(
                "%s(" % helper,
                self.source[:definition],
                "%s called before its definition" % helper,
            )

    def test_scatter_helpers_only_use_established_engine_apis(self) -> None:
        # SetQuantTimer/KillQuantTimer are used by utility.lua and bot.ai_logic.lua;
        # Context.SpawnSeekTimer must be initialised or the nudge nil-indexes.
        self.assertIn("Context.SpawnSeekTimer = Context.SpawnSeekTimer or {}", self.source)
        init = self.source.index("Context.SpawnSeekTimer = Context.SpawnSeekTimer or {}")
        first_use = self.source.index("Context.SpawnSeekTimer[squad]")
        self.assertLess(init, first_use)
        self.assertIn("BotApi.Events:SetQuantTimer(", self.source)
        self.assertIn("BotApi.Events:KillQuantTimer(", self.source)

    def test_order_timers_are_registered_on_every_map(self) -> None:
        # Waypoint maps previously got a single move order at spawn and never
        # re-ordered, so squads sat at the spawn line for the rest of the match.
        spawn = self.source.index("function OnGameSpawn(args)")
        body = self.source[spawn:self.source.index("\nfunction ", spawn + 1)]
        self.assertIn("SetSquadOrder(CaptureFlag, squad, OrderRotationPeriod)", body)
        self.assertIn("ScheduleSpawnOrderNudge(squad)", body)
        # Every map now goes through the rotating CaptureFlag loop, so the
        # one-shot waypoint walker is gone rather than merely unused.
        self.assertNotIn("GotoNextWaypoint", self.source)
        # OnWaypoint hands straight into the same loop.
        wp = self.source.index("function OnWaypoint(args)")
        wp_body = self.source[wp:self.source.index("\nfunction ", wp + 1)]
        self.assertIn("SetSquadOrder(CaptureFlag, args.squadId", wp_body)


if __name__ == "__main__":
    unittest.main()
