(define "codex_battlezones_standard"
	{common
		{unitset
			{value "2022s"}
		}
		{commandPoints "*=%cp"}
		(include "global_parameters/unitlimits.inc")
		{chargeFactors
			{value "*=1.00"}
		}
		(include "global_parameters/budget.inc" s(%start) f(%finish) d(%doctrine))
	}
	{bots
		(include "global_parameters/bots_generic.inc" period("conquest"))
	}
)

{presets
	{"s:codex_battlezones_standard"
		{gameMode "standard"}
		{unitMode "conquest"}
		{specMode "all_normal"}
		{apply "only_roster_codex_battlezones"}
		("codex_battlezones_standard" start(350) finish(7000) doctrine(100) cp(0:180,900:240,1800:320))
		{ranked}
	}
}
