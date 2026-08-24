; Flag-prop prototype pool.
; Two blocs to match support: rusa (NK/east) + nato (ARF/west). Parked player 0.
; MID 9263-9265 rusa NSVT, 9272-9274 nato TOW. Tags: flag_prop_tpl + pool role + hidden.

	{Entity "mg_stand_nsvt_rus_ai" 0xb043
		{Position -3940 -35200}
		{Player 0}
		{MID 9263}
		{Able "-select"}
	}

	{Entity "mg_stand_nsvt_rus_ai" 0xb044
		{Position -3920 -35200}
		{Player 0}
		{MID 9264}
		{Able "-select"}
	}

	{Entity "mg_stand_nsvt_rus_ai" 0xb045
		{Position -3900 -35200}
		{Player 0}
		{MID 9265}
		{Able "-select"}
	}

	{Entity "bgm71_tow_ai" 0xb04c
		{Position -3760 -35200}
		{Player 0}
		{MID 9272}
		{Able "-select"}
	}

	{Entity "bgm71_tow_ai" 0xb04d
		{Position -3740 -35200}
		{Player 0}
		{MID 9273}
		{Able "-select"}
	}

	{Entity "bgm71_tow_ai" 0xb04e
		{Position -3720 -35200}
		{Player 0}
		{MID 9274}
		{Able "-select"}
	}

; ===== STATIC TAGS =====
	{Tags "flag_prop_tpl" "flag_prop_wpn_rusa" "hidden" 0xb043}
	{Tags "flag_prop_tpl" "flag_prop_wpn_rusa" "hidden" 0xb044}
	{Tags "flag_prop_tpl" "flag_prop_wpn_rusa" "hidden" 0xb045}
	{Tags "flag_prop_tpl" "flag_prop_wpn_nato" "hidden" 0xb04c}
	{Tags "flag_prop_tpl" "flag_prop_wpn_nato" "hidden" 0xb04d}
	{Tags "flag_prop_tpl" "flag_prop_wpn_nato" "hidden" 0xb04e}
