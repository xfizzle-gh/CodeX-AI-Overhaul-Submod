; Minimal standalone Code:X Battle Zones preset.
; This follows the same preset -> apply -> roster chain used by the working mods.
(define "codex_bz_standard"
	{common
		{unitset
			{value "era2022^conquestonly|doctrineonly"}
		}
		{commandPoints "*=%cp"}
		(include "global_parameters/unitlimits.inc")
		{chargeFactors
			{value "*=1.00"}
		}
		(include "global_parameters/budget.inc" s(%start) f(%finish) d(%doctrine))
	}
	{bots
		(include "global_parameters/bots_generic.inc" period("era2022"))
	}
)

{presets
	{"s:era2022_codex_smoke"
		{gameMode "standard"}
		{unitMode "era2022"}
		{specMode "all_normal"}
		{apply "only_roster_era2022"}
		("codex_bz_standard" start(350) finish(5000) doctrine(0) cp(0:100,900:150,1500:200))
	}
}
