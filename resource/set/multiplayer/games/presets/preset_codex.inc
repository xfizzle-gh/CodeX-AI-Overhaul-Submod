; Code:X Battle Zones preset.
; Code:X intentionally separates the engine-facing era from the roster period:
; unitMode selects the era2022.<army> Lua purchase module, while unitset/bot
; filters select units tagged 2022s.
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
	{"s:era2022_codex_smoke"
		{gameMode "standard"}
		{unitMode "era2022"}
		{specMode "all_normal"}
		{apply "only_roster_2022s"}
		("codex_bz_standard" start(350) finish(5000) doctrine(0) cp(0:100,900:150,1500:200))
	}
}
