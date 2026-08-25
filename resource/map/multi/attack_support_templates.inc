; Attack support off-map prototype pool (production wave system).
; Real breeds from the conquest squad templates, parked off-map at player 0 and
; claimed by tag when a wave deploys. Nothing here is cloned: attack_support_waves.inc
; MOVES these originals to the attack-side entry waypoint, because a cloned entity's
; provenance is invisible to every selector this format can express.
;
; Pool tags — one per composition, sized so the level budget (L1 4 / L2 6 / L3 8
; waves) cannot starve a wave. A deploy strips the pool tag from the bodies it
; takes, so the pool tag always means "still parked and available" and the wave
; gate can simply count it.
;   attack_support_inf_usmc  20 = 4 x 5-strong USMC rifle team   (composition 1, also the fallback)
;   attack_support_inf_1ad   20 = 4 x 5-strong 1AD rifle team    (compositions 2 and 3)
;   attack_support_inf_arf   20 = 4 x 5-strong ARF/NATO rifle team (composition 5, L1+)
;   attack_support_inf_pzgd  12 = 2 x 6-strong German pzgren team (composition 4)
;   attack_support_hmmwv1..4  4 crewed humvees, taken in order by attack_support_hmmwv_left$
; All also carry attack_support_tpl and hidden. Parked y=-35100, x band -3200..-1320.

; ===== COMPOSITION 1 POOL — USMC rifle teams (20 = 4 x 5) =====
	{Human "mp/nato/2022s/usmc_teamlead" 0xaf20
		{Position -3200 -35100}
		{Player 0}
		{MID 9020}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_mg_m27" 0xaf21
		{Position -3180 -35100}
		{Player 0}
		{MID 9021}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_grenadier" 0xaf22
		{Position -3160 -35100}
		{Player 0}
		{MID 9022}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_rifleman" 0xaf23
		{Position -3140 -35100}
		{Player 0}
		{MID 9023}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_medic" 0xaf24
		{Position -3120 -35100}
		{Player 0}
		{MID 9024}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/usmc_teamlead" 0xaf25
		{Position -3100 -35100}
		{Player 0}
		{MID 9025}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_mg_m27" 0xaf26
		{Position -3080 -35100}
		{Player 0}
		{MID 9026}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_grenadier" 0xaf27
		{Position -3060 -35100}
		{Player 0}
		{MID 9027}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_rifleman" 0xaf28
		{Position -3040 -35100}
		{Player 0}
		{MID 9028}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_medic" 0xaf29
		{Position -3020 -35100}
		{Player 0}
		{MID 9029}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/usmc_teamlead" 0xaf2a
		{Position -3000 -35100}
		{Player 0}
		{MID 9030}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_mg_m27" 0xaf2b
		{Position -2980 -35100}
		{Player 0}
		{MID 9031}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_grenadier" 0xaf2c
		{Position -2960 -35100}
		{Player 0}
		{MID 9032}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_rifleman" 0xaf2d
		{Position -2940 -35100}
		{Player 0}
		{MID 9033}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_medic" 0xaf2e
		{Position -2920 -35100}
		{Player 0}
		{MID 9034}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/usmc_teamlead" 0xaf2f
		{Position -2900 -35100}
		{Player 0}
		{MID 9035}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_mg_m27" 0xaf30
		{Position -2880 -35100}
		{Player 0}
		{MID 9036}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_grenadier" 0xaf31
		{Position -2860 -35100}
		{Player 0}
		{MID 9037}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_rifleman" 0xaf32
		{Position -2840 -35100}
		{Player 0}
		{MID 9038}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_medic" 0xaf33
		{Position -2820 -35100}
		{Player 0}
		{MID 9039}
		{Able "-select"}
	}

; ===== COMPOSITIONS 2 AND 3 POOL — 1AD rifle teams (20 = 4 x 5) =====
	{Human "mp/nato/2022s/1ad_teamlead" 0xaf34
		{Position -2800 -35100}
		{Player 0}
		{MID 9040}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_autorifle" 0xaf35
		{Position -2780 -35100}
		{Player 0}
		{MID 9041}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_grenadier" 0xaf36
		{Position -2760 -35100}
		{Player 0}
		{MID 9042}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_rifleman" 0xaf37
		{Position -2740 -35100}
		{Player 0}
		{MID 9043}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_medic" 0xaf38
		{Position -2720 -35100}
		{Player 0}
		{MID 9044}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/1ad_teamlead" 0xaf39
		{Position -2700 -35100}
		{Player 0}
		{MID 9045}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_autorifle" 0xaf3a
		{Position -2680 -35100}
		{Player 0}
		{MID 9046}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_grenadier" 0xaf3b
		{Position -2660 -35100}
		{Player 0}
		{MID 9047}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_rifleman" 0xaf3c
		{Position -2640 -35100}
		{Player 0}
		{MID 9048}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_medic" 0xaf3d
		{Position -2620 -35100}
		{Player 0}
		{MID 9049}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/1ad_teamlead" 0xaf3e
		{Position -2600 -35100}
		{Player 0}
		{MID 9050}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_autorifle" 0xaf3f
		{Position -2580 -35100}
		{Player 0}
		{MID 9051}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_grenadier" 0xaf40
		{Position -2560 -35100}
		{Player 0}
		{MID 9052}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_rifleman" 0xaf41
		{Position -2540 -35100}
		{Player 0}
		{MID 9053}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_medic" 0xaf42
		{Position -2520 -35100}
		{Player 0}
		{MID 9054}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/1ad_teamlead" 0xaf43
		{Position -2500 -35100}
		{Player 0}
		{MID 9055}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_autorifle" 0xaf44
		{Position -2480 -35100}
		{Player 0}
		{MID 9056}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_grenadier" 0xaf45
		{Position -2460 -35100}
		{Player 0}
		{MID 9057}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_rifleman" 0xaf46
		{Position -2440 -35100}
		{Player 0}
		{MID 9058}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_medic" 0xaf47
		{Position -2420 -35100}
		{Player 0}
		{MID 9059}
		{Able "-select"}
	}

; ===== COMPOSITION 4 POOL — German pzgren teams (12 = 2 x 6) =====
	{Human "mp/nato/2022s/pzgd_squadlead" 0xaf48
		{Position -2400 -35100}
		{Player 0}
		{MID 9060}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_teamlead" 0xaf49
		{Position -2380 -35100}
		{Player 0}
		{MID 9061}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_mg" 0xaf4a
		{Position -2360 -35100}
		{Player 0}
		{MID 9062}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_grenadier" 0xaf4b
		{Position -2340 -35100}
		{Player 0}
		{MID 9063}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_rifleman" 0xaf4c
		{Position -2320 -35100}
		{Player 0}
		{MID 9064}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_medic" 0xaf4d
		{Position -2300 -35100}
		{Player 0}
		{MID 9065}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/pzgd_squadlead" 0xaf4e
		{Position -2280 -35100}
		{Player 0}
		{MID 9066}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_teamlead" 0xaf4f
		{Position -2260 -35100}
		{Player 0}
		{MID 9067}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_mg" 0xaf50
		{Position -2240 -35100}
		{Player 0}
		{MID 9068}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_grenadier" 0xaf51
		{Position -2220 -35100}
		{Player 0}
		{MID 9069}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_rifleman" 0xaf52
		{Position -2200 -35100}
		{Player 0}
		{MID 9070}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_medic" 0xaf53
		{Position -2180 -35100}
		{Player 0}
		{MID 9071}
		{Able "-select"}
	}

; ===== COMPOSITION 5 POOL — ARF / NATO rifle teams (20 = 4 x 5) =====
	{Human "mp/nato/2022s/nato_teamlead" 0xaf60
		{Position -1700 -35100}
		{Player 0}
		{MID 9000}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xaf61
		{Position -1680 -35100}
		{Player 0}
		{MID 9001}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_grenadier" 0xaf62
		{Position -1660 -35100}
		{Player 0}
		{MID 9002}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xaf63
		{Position -1640 -35100}
		{Player 0}
		{MID 9003}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_medic" 0xaf64
		{Position -1620 -35100}
		{Player 0}
		{MID 9004}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_teamlead" 0xaf65
		{Position -1600 -35100}
		{Player 0}
		{MID 9005}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xaf66
		{Position -1580 -35100}
		{Player 0}
		{MID 9006}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_grenadier" 0xaf67
		{Position -1560 -35100}
		{Player 0}
		{MID 9007}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xaf68
		{Position -1540 -35100}
		{Player 0}
		{MID 9008}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_medic" 0xaf69
		{Position -1520 -35100}
		{Player 0}
		{MID 9009}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_teamlead" 0xaf6a
		{Position -1500 -35100}
		{Player 0}
		{MID 9010}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xaf6b
		{Position -1480 -35100}
		{Player 0}
		{MID 9011}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_grenadier" 0xaf6c
		{Position -1460 -35100}
		{Player 0}
		{MID 9012}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xaf6d
		{Position -1440 -35100}
		{Player 0}
		{MID 9013}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_medic" 0xaf6e
		{Position -1420 -35100}
		{Player 0}
		{MID 9014}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_teamlead" 0xaf6f
		{Position -1400 -35100}
		{Player 0}
		{MID 9015}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xaf70
		{Position -1380 -35100}
		{Player 0}
		{MID 9016}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_grenadier" 0xaf71
		{Position -1360 -35100}
		{Player 0}
		{MID 9017}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xaf72
		{Position -1340 -35100}
		{Player 0}
		{MID 9018}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_medic" 0xaf73
		{Position -1320 -35100}
		{Player 0}
		{MID 9019}
		{Able "-select"}
	}

; ===== VEHICLE POOL — 4 crewed humvees, one per instance tag =====
	{Entity "humvee_m2hb_usa" 0xaf54
		{Position -2100 -35100}
		{Player 0}
		{MID 9072}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xaf55
		{Position -2095 -35100}
		{Player 0}
		{MID 9073}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xaf56
		{Position -2090 -35100}
		{Player 0}
		{MID 9074}
		{Able "-select"}
	}

	{Entity "humvee_m2hb_usa" 0xaf57
		{Position -2000 -35100}
		{Player 0}
		{MID 9075}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xaf58
		{Position -1995 -35100}
		{Player 0}
		{MID 9076}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xaf59
		{Position -1990 -35100}
		{Player 0}
		{MID 9077}
		{Able "-select"}
	}

	{Entity "humvee_m2hb_usa" 0xaf5a
		{Position -1900 -35100}
		{Player 0}
		{MID 9078}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xaf5b
		{Position -1895 -35100}
		{Player 0}
		{MID 9079}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xaf5c
		{Position -1890 -35100}
		{Player 0}
		{MID 9080}
		{Able "-select"}
	}

	{Entity "humvee_m2hb_usa" 0xaf5d
		{Position -1800 -35100}
		{Player 0}
		{MID 9081}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xaf5e
		{Position -1795 -35100}
		{Player 0}
		{MID 9082}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xaf5f
		{Position -1790 -35100}
		{Player 0}
		{MID 9083}
		{Able "-select"}
	}

	{Link 0xaf55 {0xaf54 "driver"}}
	{Link 0xaf56 {0xaf54 "gunner2"}}
	{Link 0xaf58 {0xaf57 "driver"}}
	{Link 0xaf59 {0xaf57 "gunner2"}}
	{Link 0xaf5b {0xaf5a "driver"}}
	{Link 0xaf5c {0xaf5a "gunner2"}}
	{Link 0xaf5e {0xaf5d "driver"}}
	{Link 0xaf5f {0xaf5d "gunner2"}}

; ===== TAGS =====
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf20}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf21}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf22}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf23}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf24}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf25}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf26}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf27}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf28}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf29}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf2a}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf2b}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf2c}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf2d}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf2e}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf2f}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf30}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf31}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf32}
	{Tags "attack_support_tpl" "attack_support_inf_usmc" "hidden" 0xaf33}

	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf34}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf35}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf36}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf37}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf38}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf39}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf3a}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf3b}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf3c}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf3d}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf3e}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf3f}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf40}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf41}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf42}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf43}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf44}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf45}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf46}
	{Tags "attack_support_tpl" "attack_support_inf_1ad" "hidden" 0xaf47}

	{Tags "attack_support_tpl" "attack_support_inf_pzgd" "hidden" 0xaf48}
	{Tags "attack_support_tpl" "attack_support_inf_pzgd" "hidden" 0xaf49}
	{Tags "attack_support_tpl" "attack_support_inf_pzgd" "hidden" 0xaf4a}
	{Tags "attack_support_tpl" "attack_support_inf_pzgd" "hidden" 0xaf4b}
	{Tags "attack_support_tpl" "attack_support_inf_pzgd" "hidden" 0xaf4c}
	{Tags "attack_support_tpl" "attack_support_inf_pzgd" "hidden" 0xaf4d}
	{Tags "attack_support_tpl" "attack_support_inf_pzgd" "hidden" 0xaf4e}
	{Tags "attack_support_tpl" "attack_support_inf_pzgd" "hidden" 0xaf4f}
	{Tags "attack_support_tpl" "attack_support_inf_pzgd" "hidden" 0xaf50}
	{Tags "attack_support_tpl" "attack_support_inf_pzgd" "hidden" 0xaf51}
	{Tags "attack_support_tpl" "attack_support_inf_pzgd" "hidden" 0xaf52}
	{Tags "attack_support_tpl" "attack_support_inf_pzgd" "hidden" 0xaf53}

	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf60}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf61}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf62}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf63}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf64}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf65}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf66}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf67}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf68}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf69}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf6a}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf6b}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf6c}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf6d}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf6e}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf6f}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf70}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf71}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf72}
	{Tags "attack_support_tpl" "attack_support_inf_arf" "hidden" 0xaf73}

	{Tags "attack_support_tpl" "attack_support_hmmwv1" "hidden" 0xaf54}
	{Tags "attack_support_tpl" "attack_support_hmmwv1" "hidden" 0xaf55}
	{Tags "attack_support_tpl" "attack_support_hmmwv1" "hidden" 0xaf56}

	{Tags "attack_support_tpl" "attack_support_hmmwv2" "hidden" 0xaf57}
	{Tags "attack_support_tpl" "attack_support_hmmwv2" "hidden" 0xaf58}
	{Tags "attack_support_tpl" "attack_support_hmmwv2" "hidden" 0xaf59}

	{Tags "attack_support_tpl" "attack_support_hmmwv3" "hidden" 0xaf5a}
	{Tags "attack_support_tpl" "attack_support_hmmwv3" "hidden" 0xaf5b}
	{Tags "attack_support_tpl" "attack_support_hmmwv3" "hidden" 0xaf5c}

	{Tags "attack_support_tpl" "attack_support_hmmwv4" "hidden" 0xaf5d}
	{Tags "attack_support_tpl" "attack_support_hmmwv4" "hidden" 0xaf5e}
	{Tags "attack_support_tpl" "attack_support_hmmwv4" "hidden" 0xaf5f}
