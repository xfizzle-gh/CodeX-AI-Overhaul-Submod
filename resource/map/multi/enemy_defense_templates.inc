; Enemy defender off-map prototype pool (MI-driven garrison / patrol / reinforcement).
; Mirrors attack_support_templates.inc exactly: real breeds parked off-map at
; player 0, claimed by tag, MOVED (never cloned) to their delivery point by
; enemy_defense_support.inc and promoted in place. A cloned entity's provenance is
; invisible to every selector this format can express, so the originals travel.
;
; FOUR FACTION POOLS, because the CWA enemy army varies per mission. conquest.lua
; publishes bot_army$ (nationMap: 1 rusa, 2 ukr, 3 nato, 4 csa, 5 sov, 6 prc,
; 7 frg, 8 pol) and the wave engine folds that to enemy_defense_army$ 1-4, then
; claims only from the matching pool. Every faction carries the same two roles:
;   enemy_def_<f>_line  24 = 6 x 4  squadlead + 2 rifleman + mg   (garrison, trickle, surge L1/L2)
;   enemy_def_<f>_wpn   16 = 4 x 4  senior + AT + marksman + rifleman (surge flavour, L2/L3)
; 40 prototypes per faction, 160 in total. A claim MOVES bodies out and never
; returns them, so each pool has to carry the whole L3 budget on its own: garrison
; (up to 3 fireteams) plus 8 reinforcement waves. Running a pool dry is a proven
; graceful path - the engine steps down to the line pool and then skips the cycle.
;
; Every prototype carries enemy_def_tpl (stripped at promotion), its faction tag,
; its pool tag (stripped by the claim, so the pool tag means "still parked") and
; hidden. Parked y=-35400, x band -9000..-5820: a band of its own, clear of the
; attack-support pool at y=-35100 / x -3200..-1790. Ids 0xb100-0xb19f, MIDs
; 9100-9259, both ranges unused anywhere else in the fourteen maps.
;
; NO VEHICLES here on purpose: the defender bot already buys its own armour
; through the normal purchase economy, and a second armour source unbalances it.

; ===== RUSA LINE POOL (24 = 6 x 4) =====
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb100
		{Position -9000 -35400}
		{Player 0}
		{MID 9100}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb101
		{Position -8980 -35400}
		{Player 0}
		{MID 9101}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb102
		{Position -8960 -35400}
		{Player 0}
		{MID 9102}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb103
		{Position -8940 -35400}
		{Player 0}
		{MID 9103}
		{Able "-select"}
	}

	{Human "mp/rusa/2022s/rus90_squadlead" 0xb104
		{Position -8920 -35400}
		{Player 0}
		{MID 9104}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb105
		{Position -8900 -35400}
		{Player 0}
		{MID 9105}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb106
		{Position -8880 -35400}
		{Player 0}
		{MID 9106}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb107
		{Position -8860 -35400}
		{Player 0}
		{MID 9107}
		{Able "-select"}
	}

	{Human "mp/rusa/2022s/rus90_squadlead" 0xb108
		{Position -8840 -35400}
		{Player 0}
		{MID 9108}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb109
		{Position -8820 -35400}
		{Player 0}
		{MID 9109}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb10a
		{Position -8800 -35400}
		{Player 0}
		{MID 9110}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb10b
		{Position -8780 -35400}
		{Player 0}
		{MID 9111}
		{Able "-select"}
	}

	{Human "mp/rusa/2022s/rus90_squadlead" 0xb10c
		{Position -8760 -35400}
		{Player 0}
		{MID 9112}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb10d
		{Position -8740 -35400}
		{Player 0}
		{MID 9113}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb10e
		{Position -8720 -35400}
		{Player 0}
		{MID 9114}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb10f
		{Position -8700 -35400}
		{Player 0}
		{MID 9115}
		{Able "-select"}
	}

	{Human "mp/rusa/2022s/rus90_squadlead" 0xb110
		{Position -8680 -35400}
		{Player 0}
		{MID 9116}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb111
		{Position -8660 -35400}
		{Player 0}
		{MID 9117}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb112
		{Position -8640 -35400}
		{Player 0}
		{MID 9118}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb113
		{Position -8620 -35400}
		{Player 0}
		{MID 9119}
		{Able "-select"}
	}


; ===== RUSA WPN POOL (16 = 4 x 4) =====




; ===== UKR LINE POOL (24 = 6 x 4) =====
	{Human "mp/ukr/2022s/ter_squadlead" 0xb128
		{Position -8200 -35400}
		{Player 0}
		{MID 9140}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb129
		{Position -8180 -35400}
		{Player 0}
		{MID 9141}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb12a
		{Position -8160 -35400}
		{Player 0}
		{MID 9142}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb12b
		{Position -8140 -35400}
		{Player 0}
		{MID 9143}
		{Able "-select"}
	}

	{Human "mp/ukr/2022s/ter_squadlead" 0xb12c
		{Position -8120 -35400}
		{Player 0}
		{MID 9144}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb12d
		{Position -8100 -35400}
		{Player 0}
		{MID 9145}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb12e
		{Position -8080 -35400}
		{Player 0}
		{MID 9146}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb12f
		{Position -8060 -35400}
		{Player 0}
		{MID 9147}
		{Able "-select"}
	}

	{Human "mp/ukr/2022s/ter_squadlead" 0xb130
		{Position -8040 -35400}
		{Player 0}
		{MID 9148}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb131
		{Position -8020 -35400}
		{Player 0}
		{MID 9149}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb132
		{Position -8000 -35400}
		{Player 0}
		{MID 9150}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb133
		{Position -7980 -35400}
		{Player 0}
		{MID 9151}
		{Able "-select"}
	}

	{Human "mp/ukr/2022s/ter_squadlead" 0xb134
		{Position -7960 -35400}
		{Player 0}
		{MID 9152}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb135
		{Position -7940 -35400}
		{Player 0}
		{MID 9153}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb136
		{Position -7920 -35400}
		{Player 0}
		{MID 9154}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb137
		{Position -7900 -35400}
		{Player 0}
		{MID 9155}
		{Able "-select"}
	}

	{Human "mp/ukr/2022s/ter_squadlead" 0xb138
		{Position -7880 -35400}
		{Player 0}
		{MID 9156}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb139
		{Position -7860 -35400}
		{Player 0}
		{MID 9157}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb13a
		{Position -7840 -35400}
		{Player 0}
		{MID 9158}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb13b
		{Position -7820 -35400}
		{Player 0}
		{MID 9159}
		{Able "-select"}
	}


; ===== UKR WPN POOL (16 = 4 x 4) =====




; ===== PRC LINE POOL (24 = 6 x 4) =====
	{Human "mp/prc/2022s/pla_squadlead" 0xb150
		{Position -7400 -35400}
		{Player 0}
		{MID 9180}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb151
		{Position -7380 -35400}
		{Player 0}
		{MID 9181}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb152
		{Position -7360 -35400}
		{Player 0}
		{MID 9182}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb153
		{Position -7340 -35400}
		{Player 0}
		{MID 9183}
		{Able "-select"}
	}

	{Human "mp/prc/2022s/pla_squadlead" 0xb154
		{Position -7320 -35400}
		{Player 0}
		{MID 9184}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb155
		{Position -7300 -35400}
		{Player 0}
		{MID 9185}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb156
		{Position -7280 -35400}
		{Player 0}
		{MID 9186}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb157
		{Position -7260 -35400}
		{Player 0}
		{MID 9187}
		{Able "-select"}
	}

	{Human "mp/prc/2022s/pla_squadlead" 0xb158
		{Position -7240 -35400}
		{Player 0}
		{MID 9188}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb159
		{Position -7220 -35400}
		{Player 0}
		{MID 9189}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb15a
		{Position -7200 -35400}
		{Player 0}
		{MID 9190}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb15b
		{Position -7180 -35400}
		{Player 0}
		{MID 9191}
		{Able "-select"}
	}

	{Human "mp/prc/2022s/pla_squadlead" 0xb15c
		{Position -7160 -35400}
		{Player 0}
		{MID 9192}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb15d
		{Position -7140 -35400}
		{Player 0}
		{MID 9193}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb15e
		{Position -7120 -35400}
		{Player 0}
		{MID 9194}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb15f
		{Position -7100 -35400}
		{Player 0}
		{MID 9195}
		{Able "-select"}
	}

	{Human "mp/prc/2022s/pla_squadlead" 0xb160
		{Position -7080 -35400}
		{Player 0}
		{MID 9196}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb161
		{Position -7060 -35400}
		{Player 0}
		{MID 9197}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb162
		{Position -7040 -35400}
		{Player 0}
		{MID 9198}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb163
		{Position -7020 -35400}
		{Player 0}
		{MID 9199}
		{Able "-select"}
	}


; ===== PRC WPN POOL (16 = 4 x 4) =====




; ===== NATO LINE POOL (24 = 6 x 4) =====
	{Human "mp/nato/2022s/nato_squadlead" 0xb178
		{Position -6600 -35400}
		{Player 0}
		{MID 9220}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb179
		{Position -6580 -35400}
		{Player 0}
		{MID 9221}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb17a
		{Position -6560 -35400}
		{Player 0}
		{MID 9222}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb17b
		{Position -6540 -35400}
		{Player 0}
		{MID 9223}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_squadlead" 0xb17c
		{Position -6520 -35400}
		{Player 0}
		{MID 9224}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb17d
		{Position -6500 -35400}
		{Player 0}
		{MID 9225}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb17e
		{Position -6480 -35400}
		{Player 0}
		{MID 9226}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb17f
		{Position -6460 -35400}
		{Player 0}
		{MID 9227}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_squadlead" 0xb180
		{Position -6440 -35400}
		{Player 0}
		{MID 9228}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb181
		{Position -6420 -35400}
		{Player 0}
		{MID 9229}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb182
		{Position -6400 -35400}
		{Player 0}
		{MID 9230}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb183
		{Position -6380 -35400}
		{Player 0}
		{MID 9231}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_squadlead" 0xb184
		{Position -6360 -35400}
		{Player 0}
		{MID 9232}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb185
		{Position -6340 -35400}
		{Player 0}
		{MID 9233}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb186
		{Position -6320 -35400}
		{Player 0}
		{MID 9234}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb187
		{Position -6300 -35400}
		{Player 0}
		{MID 9235}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_squadlead" 0xb188
		{Position -6280 -35400}
		{Player 0}
		{MID 9236}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb189
		{Position -6260 -35400}
		{Player 0}
		{MID 9237}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb18a
		{Position -6240 -35400}
		{Player 0}
		{MID 9238}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb18b
		{Position -6220 -35400}
		{Player 0}
		{MID 9239}
		{Able "-select"}
	}


; ===== ENEMY L2 LIGHT CARS (1 hull+crew per faction) =====
	{Entity "gaz2975a_nsvt" 0xb1c0
		{Position -9000 -35600}
		{Player 0}
		{MID 9260}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xb1c1
		{Position -8995 -35600}
		{Player 0}
		{MID 9261}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xb1c2
		{Position -8990 -35600}
		{Player 0}
		{MID 9262}
		{Able "-select"}
	}
	{Entity "humvee_m2hb_ukr" 0xb1c3
		{Position -8200 -35600}
		{Player 0}
		{MID 9263}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xb1c4
		{Position -8195 -35600}
		{Player 0}
		{MID 9264}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xb1c5
		{Position -8190 -35600}
		{Player 0}
		{MID 9265}
		{Able "-select"}
	}
	{Entity "dongfeng__mengshi_sk131" 0xb1c6
		{Position -7400 -35600}
		{Player 0}
		{MID 9266}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_vehicleman" 0xb1c7
		{Position -7395 -35600}
		{Player 0}
		{MID 9267}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_vehicleman" 0xb1c8
		{Position -7390 -35600}
		{Player 0}
		{MID 9268}
		{Able "-select"}
	}
	{Entity "fennek" 0xb1c9
		{Position -6600 -35600}
		{Player 0}
		{MID 9269}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_vehicleman" 0xb1ca
		{Position -6595 -35600}
		{Player 0}
		{MID 9270}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_vehicleman" 0xb1cb
		{Position -6590 -35600}
		{Player 0}
		{MID 9271}
		{Able "-select"}
	}
	{Link 0xb1c1 {0xb1c0 "driver"}}
	{Link 0xb1c2 {0xb1c0 "gunner2"}}
	{Link 0xb1c4 {0xb1c3 "driver"}}
	{Link 0xb1c5 {0xb1c3 "gunner2"}}
	{Link 0xb1c7 {0xb1c6 "driver"}}
	{Link 0xb1c8 {0xb1c6 "gunner2"}}
	{Link 0xb1ca {0xb1c9 "driver"}}
	{Link 0xb1cb {0xb1c9 "gunner2"}}

; ===== NATO WPN POOL (16 = 4 x 4) =====




; ===== TAGS =====
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb100}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb101}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb102}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb103}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb104}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb105}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb106}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb107}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb108}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb109}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb10a}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb10b}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb10c}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb10d}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb10e}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb10f}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb110}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb111}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb112}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb113}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb128}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb129}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb12a}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb12b}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb12c}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb12d}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb12e}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb12f}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb130}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb131}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb132}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb133}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb134}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb135}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb136}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb137}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb138}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb139}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb13a}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb13b}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb150}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb151}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb152}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb153}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb154}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb155}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb156}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb157}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb158}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb159}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb15a}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb15b}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb15c}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb15d}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb15e}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb15f}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb160}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb161}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb162}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb163}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb178}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb179}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb17a}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb17b}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb17c}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb17d}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb17e}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb17f}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb180}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb181}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb182}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb183}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb184}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb185}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb186}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb187}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb188}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb189}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_veh" "enemy_def_rusa_veh1" "hidden" 0xb1c0}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_veh" "enemy_def_rusa_veh1" "hidden" 0xb1c1}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_veh" "enemy_def_rusa_veh1" "hidden" 0xb1c2}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_veh" "enemy_def_ukr_veh1" "hidden" 0xb1c3}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_veh" "enemy_def_ukr_veh1" "hidden" 0xb1c4}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_veh" "enemy_def_ukr_veh1" "hidden" 0xb1c5}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_veh" "enemy_def_prc_veh1" "hidden" 0xb1c6}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_veh" "enemy_def_prc_veh1" "hidden" 0xb1c7}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_veh" "enemy_def_prc_veh1" "hidden" 0xb1c8}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_veh" "enemy_def_nato_veh1" "hidden" 0xb1c9}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_veh" "enemy_def_nato_veh1" "hidden" 0xb1ca}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_veh" "enemy_def_nato_veh1" "hidden" 0xb1cb}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb18a}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb18b}
