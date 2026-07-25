; Code:X Battle Zones, following Shattered Galaxy's single-token era contract.
(define "codex2022_standard_all"
	{common
		{unitset
			{value "codex2022^conquestonly|doctrineonly"}
		}
		{commandPoints "*=%cp"}
		(include "global_parameters/unitlimits.inc")
		{chargeFactors
			{value "*=1.00"}
		}
		(include "global_parameters/budget.inc" s(%start) f(%finish) d(%doctrine))
	}
	{bots
		(include "global_parameters/bots_generic.inc" period("codex2022"))
	}
)

{presets
	{"s:codex2022_standard_100"
		{gameMode "standard"}
		{unitMode "codex2022"}
		{specMode "all_normal"}
		{apply "only_roster_codex2022"}
		("codex2022_standard_all" start(350) finish(5000) doctrine(0) cp(0:100,900:150,1500:200))
		{ranked}
	}
}
