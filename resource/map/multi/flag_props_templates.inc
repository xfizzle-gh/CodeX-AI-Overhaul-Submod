; Flag-prop prototype pool (Phase 4 rewrite).
; Parked off-map at player 0; claimed by static tag and MOVEd to flag anchors.
; No runtime spawn. Shared by defense_support + enemy_defense (mutual exclusion).
; MID band 9260-9274. Tags: flag_prop_tpl + pool role + hidden.
;   flag_prop_ammo x3  para_ammo crates (faction-neutral)
;   flag_prop_wpn_rusa/ukr/prc/nato x3 each  crewless weapons (L2+)
; y=-35200, x band -4000..-3700.

	{Entity "para_ammo" 0xb040
		{Position -4000 -35200}
		{Player 0}
		{MID 9260}
		{Able "-select"}
	}

	{Entity "para_ammo" 0xb041
		{Position -3980 -35200}
		{Player 0}
		{MID 9261}
		{Able "-select"}
	}

	{Entity "para_ammo" 0xb042
		{Position -3960 -35200}
		{Player 0}
		{MID 9262}
		{Able "-select"}
	}

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

	{Entity "mg_stand_nsvt_ukr_ai" 0xb046
		{Position -3880 -35200}
		{Player 0}
		{MID 9266}
		{Able "-select"}
	}

	{Entity "mg_stand_nsvt_ukr_ai" 0xb047
		{Position -3860 -35200}
		{Player 0}
		{MID 9267}
		{Able "-select"}
	}

	{Entity "mg_stand_nsvt_ukr_ai" 0xb048
		{Position -3840 -35200}
		{Player 0}
		{MID 9268}
		{Able "-select"}
	}

	{Entity "mg_stand_qjz171" 0xb049
		{Position -3820 -35200}
		{Player 0}
		{MID 9269}
		{Able "-select"}
	}

	{Entity "mg_stand_qjz171" 0xb04a
		{Position -3800 -35200}
		{Player 0}
		{MID 9270}
		{Able "-select"}
	}

	{Entity "mg_stand_qjz171" 0xb04b
		{Position -3780 -35200}
		{Player 0}
		{MID 9271}
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
	{Tags "flag_prop_tpl" "flag_prop_ammo" "hidden" 0xb040}
	{Tags "flag_prop_tpl" "flag_prop_ammo" "hidden" 0xb041}
	{Tags "flag_prop_tpl" "flag_prop_ammo" "hidden" 0xb042}
	{Tags "flag_prop_tpl" "flag_prop_wpn_rusa" "hidden" 0xb043}
	{Tags "flag_prop_tpl" "flag_prop_wpn_rusa" "hidden" 0xb044}
	{Tags "flag_prop_tpl" "flag_prop_wpn_rusa" "hidden" 0xb045}
	{Tags "flag_prop_tpl" "flag_prop_wpn_ukr" "hidden" 0xb046}
	{Tags "flag_prop_tpl" "flag_prop_wpn_ukr" "hidden" 0xb047}
	{Tags "flag_prop_tpl" "flag_prop_wpn_ukr" "hidden" 0xb048}
	{Tags "flag_prop_tpl" "flag_prop_wpn_prc" "hidden" 0xb049}
	{Tags "flag_prop_tpl" "flag_prop_wpn_prc" "hidden" 0xb04a}
	{Tags "flag_prop_tpl" "flag_prop_wpn_prc" "hidden" 0xb04b}
	{Tags "flag_prop_tpl" "flag_prop_wpn_nato" "hidden" 0xb04c}
	{Tags "flag_prop_tpl" "flag_prop_wpn_nato" "hidden" 0xb04d}
	{Tags "flag_prop_tpl" "flag_prop_wpn_nato" "hidden" 0xb04e}

