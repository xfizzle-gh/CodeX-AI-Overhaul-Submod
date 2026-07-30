; Flag-prop prototype pool (Phase 4 rewrite).
; Parked off-map at player 0; claimed by static tag and MOVEd to flag anchors.
; No runtime spawn. Shared by defense_support + enemy_defense (mutual exclusion).
; MID band 9263-9274. Tags: flag_prop_tpl + pool role + hidden.
;   flag_prop_wpn_rusa/ukr/prc/nato x3 each  flag emplacements (every level)
; y=-35200, x band -3940..-3720.
;
; CREWED, not crewless (user decision 2026-07-30). The entities here are still the
; _ai emplacement variants and they are still parked unowned at player 0, but the
; two garrison engines no longer leave them that way: each one is claimed, MOVEd to
; its flag, stamped flag_prop_afN, activated, transferred to the defending bot and
; then boarded by two bodies from that faction's line pool. Three per faction is
; still the right depth - a mission activates at most three flag points, and the
; two engines are mutually exclusive, so one mission can consume at most three of
; the twelve.
;
; RETIRED: the three ammo-crate prototypes that used to open this pool (MIDs
; 9260-9262, ids 0xb040-0xb042) are gone and must not come back. Ammo supply on a
; flag is now the vanilla mechanism - a flagpoint_ammo entity linked into the
; flag's own built-in "ammo" placer slot by the deploy script, drawing Code:X's
; modern resupply table through our shadow flagpoint_ammo.def. That follows the
; flag when it changes hands, costs no pool depth and needs no engine step, so a
; crate prop alongside it was strictly worse.

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

