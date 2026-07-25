; Code:X Battle Zones preset.
; Keep the era token identical across unitMode, unitset, bot period, roster, and AI purchases.
(define "codex_bz_standard"
	{common
		{unitset
			{value "2022s^conquestonly|doctrineonly"}
		}
		{commandPoints "*=%cp"}
		(include "global_parameters/unitlimits.inc")
		{chargeFactors
			{value "*=1.00"}
		}
		(include "global_parameters/budget.inc" s(%start) f(%finish) d(%doctrine))
	}
	{bots
		(include "global_parameters/bots_generic.inc" period("2022s"))
	}
)

{presets
	{"s:2022s_codex_smoke"
		{gameMode "standard"}
		{unitMode "2022s"}
		{specMode "all_normal"}
		{apply "only_roster_2022s"}
		("codex_bz_standard" start(350) finish(5000) doctrine(0) cp(0:100,900:150,1500:200))
	}
}
