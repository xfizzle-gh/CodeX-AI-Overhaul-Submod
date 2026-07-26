; == Code:X skirmish (SG pattern + safe roster) ==
; unitMode/period 2022s matches Code:X period(2022s) tags.
(define "standard_all"
	{common
		{unitset
			{value "2022s"}
		}
		{commandPoints "*=%cp"}
		(include "global_parameters/unitlimits.inc")
		{chargeFactors
			{value "*=1.00"}
		}
		(include "global_parameters/budget_skirmish.inc" s(%start) f(%finish))
	}
	{bots
		(include "global_parameters/bots_generic.inc" period("2022s"))
	}
)

(define "custom_all"
	{common
		{unitset
			{value "2022s"}
			{available "all infantry sniper armor light medium heavy artillery airborne mechanized"}
		}
		{commandPoints "*=%cp"}
		(include "global_parameters/unitlimits.inc")
		{chargeFactors
			{value "*=1.00"}
			{customize "0;0.25;0.5;1;2"}
		}
		(include "global_parameters/budget_skirmish_custom.inc" s(%start) f(%finish))
	}
	{bots
		(include "global_parameters/bots_generic.inc" period("2022s"))
	}
)

{presets
	{"s:2022s_standard_100"
		{gameMode "standard"}
		{unitMode "2022s"}
		{specMode "all_normal"}
		{apply "only_roster_2022s"}
		("standard_all"			start(200) finish(3300) cp(0:100,900:150,1500:200))
		{ranked}
	}

	{"c:2022s_custom_80"
		{gameMode "custom"}
		{unitMode "2022s"}
		{specMode "cp_80"}
		{apply "only_roster_2022s"}
		("custom_all"			start(200) finish(2800) cp(0:80,900:130,1500:180))
	}
	{"c:2022s_custom_100"
		{gameMode "custom"}
		{unitMode "2022s"}
		{specMode "cp_100"}
		{apply "only_roster_2022s"}
		("custom_all"			start(200) finish(2800) cp(0:100,900:150,1500:200))
	}
	{"c:2022s_custom_120"
		{gameMode "custom"}
		{unitMode "2022s"}
		{specMode "cp_120"}
		{apply "only_roster_2022s"}
		("custom_all"			start(200) finish(3300) cp(0:120,900:170,1500:220))
	}
	{"c:2022s_custom_150"
		{gameMode "custom"}
		{unitMode "2022s"}
		{specMode "cp_150"}
		{apply "only_roster_2022s"}
		("custom_all"			start(200) finish(3800) cp(0:150,900:200,1500:250))
	}
}
