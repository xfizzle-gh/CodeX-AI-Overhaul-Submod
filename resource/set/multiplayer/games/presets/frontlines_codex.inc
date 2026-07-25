; == Code:X Frontlines ==
(include "global_parameters/frontlines/timers_frontlines.inc")

(define "standard_all"
	{teams
		{a
			{unitset {value "2022s"}}
			{commandPoints "*=%cp_a"}
			(include "global_parameters/frontlines/unitlimits_attacker.inc")
			{chargeFactors ("timers_attacker")}
			(include "global_parameters/frontlines/budget_attacker.inc" s(%start_a) f(%finish_a) r(%reinforcement_a))
		}
		{b
			{unitset {value "2022s"}}
			{commandPoints "*=%cp_b"}
			(include "global_parameters/frontlines/unitlimits_defender.inc")
			{chargeFactors ("timers_defender")}
			(include "global_parameters/frontlines/budget_defender.inc" s(%start_b) f(%finish_b))
		}
	}
	{bots
		(include "global_parameters/frontlines/bots_standard.inc" period("2022s") start_a(%start_a) finish_a(%finish_a) reinforcement_a(%reinforcement_a) start_b(%start_b) finish_b(%finish_b) doctrine_a(0) doctrine_b(0) cp_a(%cp_a) cp_b(%cp_b))
	}
)

(define "custom_all"
	{common}
	{teams
		{a
			{unitset {value "2022s"}}
			{commandPoints "*=%cp_a"}
			(include "global_parameters/frontlines/unitlimits_attacker.inc")
			{chargeFactors ("timers_attacker")}
			(include "global_parameters/frontlines/budget_attacker_custom.inc" s(%start_a) f(%finish_a) r(%reinforcement_a))
		}
		{b
			{unitset {value "2022s"}}
			{commandPoints "*=%cp_b"}
			(include "global_parameters/frontlines/unitlimits_defender.inc")
			{chargeFactors ("timers_defender")}
			(include "global_parameters/frontlines/budget_defender_custom.inc" s(%start_b) f(%finish_b))
		}
	}
	{bots
		(include "global_parameters/frontlines/bots_custom.inc" period("2022s") start_a(%start_a) finish_a(%finish_a) reinforcement_a(%reinforcement_a) start_b(%start_b) finish_b(%finish_b) doctrine_a(0) doctrine_b(0) cp_a(%cp_a) cp_b(%cp_b))
	}
)

{presets
	{"s:2022s_fl_standard"
		{gameMode "standard"}
		{unitMode "2022s"}
		{specMode "all_normal"}
		{apply "only_roster_2022s"}
		("standard_all" start_a(250) finish_a(3500) reinforcement_a(400) start_b(250) finish_b(3500) cp_a(0:100,900:150,1500:200) cp_b(0:100,900:150,1500:200))
		{ranked}
	}
	{"c:2022s_fl_custom_100"
		{gameMode "custom"}
		{unitMode "2022s"}
		{specMode "cp_100"}
		{apply "only_roster_2022s"}
		("custom_all" start_a(250) finish_a(3000) reinforcement_a(400) start_b(250) finish_b(3000) cp_a(0:100,900:150,1500:200) cp_b(0:100,900:150,1500:200))
	}
}
