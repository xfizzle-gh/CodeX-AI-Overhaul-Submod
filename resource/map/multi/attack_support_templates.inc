; Attack support off-map pool (TEST). Real breeds from conquest squad templates.
; Wave tags:
;   attack_support_w1  = USMC diverse fireteam (squad_usmc_rifle roles, compact)
;   attack_support_w2  = ACAV / 1AD fireteam (squad_tank1_fireteam)
;   attack_support_w3  = German pzgren fireteam (squad_pz10_rifle) + 2 crewed humvees
; All also tagged attack_support_tpl. Parked y=-35100, x band -3200..-2000.

; ===== WAVE 1 — US Marines (7) — squad_usmc_rifle roles =====
	{Human "mp/nato/2022s/usmc_squadlead" 0xaf20
		{Position -3200 -35100}
		{Player 0}
		{MID 9020}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_teamlead" 0xaf21
		{Position -3180 -35100}
		{Player 0}
		{MID 9021}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_mg_m27" 0xaf22
		{Position -3160 -35100}
		{Player 0}
		{MID 9022}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_grenadier" 0xaf23
		{Position -3140 -35100}
		{Player 0}
		{MID 9023}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_marksman" 0xaf24
		{Position -3120 -35100}
		{Player 0}
		{MID 9024}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_rifleman" 0xaf25
		{Position -3100 -35100}
		{Player 0}
		{MID 9025}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_medic" 0xaf26
		{Position -3080 -35100}
		{Player 0}
		{MID 9026}
		{Able "-select"}
	}

; ===== WAVE 2 — US ACAV / 1AD (7) — squad_tank1_fireteam =====
	{Human "mp/nato/2022s/1ad_squadlead" 0xaf30
		{Position -3000 -35100}
		{Player 0}
		{MID 9030}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_teamlead" 0xaf31
		{Position -2980 -35100}
		{Player 0}
		{MID 9031}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_autorifle" 0xaf32
		{Position -2960 -35100}
		{Player 0}
		{MID 9032}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_autorifle" 0xaf33
		{Position -2940 -35100}
		{Player 0}
		{MID 9033}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_rifleman" 0xaf34
		{Position -2920 -35100}
		{Player 0}
		{MID 9034}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_grenadier" 0xaf35
		{Position -2900 -35100}
		{Player 0}
		{MID 9035}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_medic" 0xaf36
		{Position -2880 -35100}
		{Player 0}
		{MID 9036}
		{Able "-select"}
	}

; ===== WAVE 3 — German pzgren (7) — squad_pz10_rifle =====
	{Human "mp/nato/2022s/pzgd_squadlead" 0xaf40
		{Position -2800 -35100}
		{Player 0}
		{MID 9040}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_teamlead" 0xaf41
		{Position -2780 -35100}
		{Player 0}
		{MID 9041}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_rifleman" 0xaf42
		{Position -2760 -35100}
		{Player 0}
		{MID 9042}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_rifleman" 0xaf43
		{Position -2740 -35100}
		{Player 0}
		{MID 9043}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_grenadier" 0xaf44
		{Position -2720 -35100}
		{Player 0}
		{MID 9044}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_mg" 0xaf45
		{Position -2700 -35100}
		{Player 0}
		{MID 9045}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_medic" 0xaf46
		{Position -2680 -35100}
		{Player 0}
		{MID 9046}
		{Able "-select"}
	}

; ===== WAVE 3 — 2 crewed humvees (driver + gunner2) =====
	{Entity "humvee_m2hb_usa" 0xaf50
		{Position -2600 -35100}
		{Player 0}
		{MID 9050}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xaf51
		{Position -2595 -35100}
		{Player 0}
		{MID 9051}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xaf52
		{Position -2590 -35100}
		{Player 0}
		{MID 9052}
		{Able "-select"}
	}
	{Entity "humvee_m2hb_usa" 0xaf54
		{Position -2500 -35100}
		{Player 0}
		{MID 9054}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xaf55
		{Position -2495 -35100}
		{Player 0}
		{MID 9055}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xaf56
		{Position -2490 -35100}
		{Player 0}
		{MID 9056}
		{Able "-select"}
	}
	{Link 0xaf51 {0xaf50 "driver"}}
	{Link 0xaf52 {0xaf50 "gunner2"}}
	{Link 0xaf55 {0xaf54 "driver"}}
	{Link 0xaf56 {0xaf54 "gunner2"}}

; ===== TAGS =====
	{Tags "attack_support_tpl" "attack_support_w1" "hidden" 0xaf20}
	{Tags "attack_support_tpl" "attack_support_w1" "hidden" 0xaf21}
	{Tags "attack_support_tpl" "attack_support_w1" "hidden" 0xaf22}
	{Tags "attack_support_tpl" "attack_support_w1" "hidden" 0xaf23}
	{Tags "attack_support_tpl" "attack_support_w1" "hidden" 0xaf24}
	{Tags "attack_support_tpl" "attack_support_w1" "hidden" 0xaf25}
	{Tags "attack_support_tpl" "attack_support_w1" "hidden" 0xaf26}

	{Tags "attack_support_tpl" "attack_support_w2" "hidden" 0xaf30}
	{Tags "attack_support_tpl" "attack_support_w2" "hidden" 0xaf31}
	{Tags "attack_support_tpl" "attack_support_w2" "hidden" 0xaf32}
	{Tags "attack_support_tpl" "attack_support_w2" "hidden" 0xaf33}
	{Tags "attack_support_tpl" "attack_support_w2" "hidden" 0xaf34}
	{Tags "attack_support_tpl" "attack_support_w2" "hidden" 0xaf35}
	{Tags "attack_support_tpl" "attack_support_w2" "hidden" 0xaf36}

	{Tags "attack_support_tpl" "attack_support_w3" "hidden" 0xaf40}
	{Tags "attack_support_tpl" "attack_support_w3" "hidden" 0xaf41}
	{Tags "attack_support_tpl" "attack_support_w3" "hidden" 0xaf42}
	{Tags "attack_support_tpl" "attack_support_w3" "hidden" 0xaf43}
	{Tags "attack_support_tpl" "attack_support_w3" "hidden" 0xaf44}
	{Tags "attack_support_tpl" "attack_support_w3" "hidden" 0xaf45}
	{Tags "attack_support_tpl" "attack_support_w3" "hidden" 0xaf46}

	{Tags "attack_support_tpl" "attack_support_w3" "attack_support_hmmwv1" "hidden" 0xaf50}
	{Tags "attack_support_tpl" "attack_support_w3" "attack_support_hmmwv1" "hidden" 0xaf51}
	{Tags "attack_support_tpl" "attack_support_w3" "attack_support_hmmwv1" "hidden" 0xaf52}
	{Tags "attack_support_tpl" "attack_support_w3" "attack_support_hmmwv2" "hidden" 0xaf54}
	{Tags "attack_support_tpl" "attack_support_w3" "attack_support_hmmwv2" "hidden" 0xaf55}
	{Tags "attack_support_tpl" "attack_support_w3" "attack_support_hmmwv2" "hidden" 0xaf56}
