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

	{Human "mp/rusa/2022s/rus90_squadlead" 0xb114
		{Position -8600 -35400}
		{Player 0}
		{MID 9120}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb115
		{Position -8580 -35400}
		{Player 0}
		{MID 9121}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb116
		{Position -8560 -35400}
		{Player 0}
		{MID 9122}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb117
		{Position -8540 -35400}
		{Player 0}
		{MID 9123}
		{Able "-select"}
	}

; ===== RUSA WPN POOL (16 = 4 x 4) =====
	{Human "mp/rusa/2022s/rus90_seniorrifleman" 0xb118
		{Position -8520 -35400}
		{Player 0}
		{MID 9124}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_antitank" 0xb119
		{Position -8500 -35400}
		{Player 0}
		{MID 9125}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_marksman" 0xb11a
		{Position -8480 -35400}
		{Player 0}
		{MID 9126}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb11b
		{Position -8460 -35400}
		{Player 0}
		{MID 9127}
		{Able "-select"}
	}

	{Human "mp/rusa/2022s/rus90_seniorrifleman" 0xb11c
		{Position -8440 -35400}
		{Player 0}
		{MID 9128}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_antitank" 0xb11d
		{Position -8420 -35400}
		{Player 0}
		{MID 9129}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_marksman" 0xb11e
		{Position -8400 -35400}
		{Player 0}
		{MID 9130}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb11f
		{Position -8380 -35400}
		{Player 0}
		{MID 9131}
		{Able "-select"}
	}

	{Human "mp/rusa/2022s/rus90_seniorrifleman" 0xb120
		{Position -8360 -35400}
		{Player 0}
		{MID 9132}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_antitank" 0xb121
		{Position -8340 -35400}
		{Player 0}
		{MID 9133}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_marksman" 0xb122
		{Position -8320 -35400}
		{Player 0}
		{MID 9134}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb123
		{Position -8300 -35400}
		{Player 0}
		{MID 9135}
		{Able "-select"}
	}

	{Human "mp/rusa/2022s/rus90_seniorrifleman" 0xb124
		{Position -8280 -35400}
		{Player 0}
		{MID 9136}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_antitank" 0xb125
		{Position -8260 -35400}
		{Player 0}
		{MID 9137}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_marksman" 0xb126
		{Position -8240 -35400}
		{Player 0}
		{MID 9138}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb127
		{Position -8220 -35400}
		{Player 0}
		{MID 9139}
		{Able "-select"}
	}

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

	{Human "mp/ukr/2022s/ter_squadlead" 0xb13c
		{Position -7800 -35400}
		{Player 0}
		{MID 9160}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb13d
		{Position -7780 -35400}
		{Player 0}
		{MID 9161}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb13e
		{Position -7760 -35400}
		{Player 0}
		{MID 9162}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb13f
		{Position -7740 -35400}
		{Player 0}
		{MID 9163}
		{Able "-select"}
	}

; ===== UKR WPN POOL (16 = 4 x 4) =====
	{Human "mp/ukr/2022s/ter_squadlead" 0xb140
		{Position -7720 -35400}
		{Player 0}
		{MID 9164}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_antitank" 0xb141
		{Position -7700 -35400}
		{Player 0}
		{MID 9165}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_marksman" 0xb142
		{Position -7680 -35400}
		{Player 0}
		{MID 9166}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb143
		{Position -7660 -35400}
		{Player 0}
		{MID 9167}
		{Able "-select"}
	}

	{Human "mp/ukr/2022s/ter_squadlead" 0xb144
		{Position -7640 -35400}
		{Player 0}
		{MID 9168}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_antitank" 0xb145
		{Position -7620 -35400}
		{Player 0}
		{MID 9169}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_marksman" 0xb146
		{Position -7600 -35400}
		{Player 0}
		{MID 9170}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb147
		{Position -7580 -35400}
		{Player 0}
		{MID 9171}
		{Able "-select"}
	}

	{Human "mp/ukr/2022s/ter_squadlead" 0xb148
		{Position -7560 -35400}
		{Player 0}
		{MID 9172}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_antitank" 0xb149
		{Position -7540 -35400}
		{Player 0}
		{MID 9173}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_marksman" 0xb14a
		{Position -7520 -35400}
		{Player 0}
		{MID 9174}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb14b
		{Position -7500 -35400}
		{Player 0}
		{MID 9175}
		{Able "-select"}
	}

	{Human "mp/ukr/2022s/ter_squadlead" 0xb14c
		{Position -7480 -35400}
		{Player 0}
		{MID 9176}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_antitank" 0xb14d
		{Position -7460 -35400}
		{Player 0}
		{MID 9177}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_marksman" 0xb14e
		{Position -7440 -35400}
		{Player 0}
		{MID 9178}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb14f
		{Position -7420 -35400}
		{Player 0}
		{MID 9179}
		{Able "-select"}
	}

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

	{Human "mp/prc/2022s/pla_squadlead" 0xb164
		{Position -7000 -35400}
		{Player 0}
		{MID 9200}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb165
		{Position -6980 -35400}
		{Player 0}
		{MID 9201}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb166
		{Position -6960 -35400}
		{Player 0}
		{MID 9202}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb167
		{Position -6940 -35400}
		{Player 0}
		{MID 9203}
		{Able "-select"}
	}

; ===== PRC WPN POOL (16 = 4 x 4) =====
	{Human "mp/prc/2022s/pla_senior" 0xb168
		{Position -6920 -35400}
		{Player 0}
		{MID 9204}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_antitank_pf98" 0xb169
		{Position -6900 -35400}
		{Player 0}
		{MID 9205}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_marksman" 0xb16a
		{Position -6880 -35400}
		{Player 0}
		{MID 9206}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb16b
		{Position -6860 -35400}
		{Player 0}
		{MID 9207}
		{Able "-select"}
	}

	{Human "mp/prc/2022s/pla_senior" 0xb16c
		{Position -6840 -35400}
		{Player 0}
		{MID 9208}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_antitank_pf98" 0xb16d
		{Position -6820 -35400}
		{Player 0}
		{MID 9209}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_marksman" 0xb16e
		{Position -6800 -35400}
		{Player 0}
		{MID 9210}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb16f
		{Position -6780 -35400}
		{Player 0}
		{MID 9211}
		{Able "-select"}
	}

	{Human "mp/prc/2022s/pla_senior" 0xb170
		{Position -6760 -35400}
		{Player 0}
		{MID 9212}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_antitank_pf98" 0xb171
		{Position -6740 -35400}
		{Player 0}
		{MID 9213}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_marksman" 0xb172
		{Position -6720 -35400}
		{Player 0}
		{MID 9214}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb173
		{Position -6700 -35400}
		{Player 0}
		{MID 9215}
		{Able "-select"}
	}

	{Human "mp/prc/2022s/pla_senior" 0xb174
		{Position -6680 -35400}
		{Player 0}
		{MID 9216}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_antitank_pf98" 0xb175
		{Position -6660 -35400}
		{Player 0}
		{MID 9217}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_marksman" 0xb176
		{Position -6640 -35400}
		{Player 0}
		{MID 9218}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb177
		{Position -6620 -35400}
		{Player 0}
		{MID 9219}
		{Able "-select"}
	}

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

	{Human "mp/nato/2022s/nato_squadlead" 0xb18c
		{Position -6200 -35400}
		{Player 0}
		{MID 9240}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb18d
		{Position -6180 -35400}
		{Player 0}
		{MID 9241}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb18e
		{Position -6160 -35400}
		{Player 0}
		{MID 9242}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb18f
		{Position -6140 -35400}
		{Player 0}
		{MID 9243}
		{Able "-select"}
	}

; ===== NATO WPN POOL (16 = 4 x 4) =====
	{Human "mp/nato/2022s/nato_teamlead" 0xb190
		{Position -6120 -35400}
		{Player 0}
		{MID 9244}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_antitank" 0xb191
		{Position -6100 -35400}
		{Player 0}
		{MID 9245}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_sniper" 0xb192
		{Position -6080 -35400}
		{Player 0}
		{MID 9246}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb193
		{Position -6060 -35400}
		{Player 0}
		{MID 9247}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_teamlead" 0xb194
		{Position -6040 -35400}
		{Player 0}
		{MID 9248}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_antitank" 0xb195
		{Position -6020 -35400}
		{Player 0}
		{MID 9249}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_sniper" 0xb196
		{Position -6000 -35400}
		{Player 0}
		{MID 9250}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb197
		{Position -5980 -35400}
		{Player 0}
		{MID 9251}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_teamlead" 0xb198
		{Position -5960 -35400}
		{Player 0}
		{MID 9252}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_antitank" 0xb199
		{Position -5940 -35400}
		{Player 0}
		{MID 9253}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_sniper" 0xb19a
		{Position -5920 -35400}
		{Player 0}
		{MID 9254}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb19b
		{Position -5900 -35400}
		{Player 0}
		{MID 9255}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_teamlead" 0xb19c
		{Position -5880 -35400}
		{Player 0}
		{MID 9256}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_antitank" 0xb19d
		{Position -5860 -35400}
		{Player 0}
		{MID 9257}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_sniper" 0xb19e
		{Position -5840 -35400}
		{Player 0}
		{MID 9258}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb19f
		{Position -5820 -35400}
		{Player 0}
		{MID 9259}
		{Able "-select"}
	}

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
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb114}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb115}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb116}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_line" "hidden" 0xb117}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb118}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb119}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb11a}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb11b}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb11c}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb11d}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb11e}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb11f}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb120}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb121}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb122}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb123}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb124}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb125}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb126}
	{Tags "enemy_def_tpl" "enemy_def_rusa" "enemy_def_rusa_wpn" "hidden" 0xb127}
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
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb13c}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb13d}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb13e}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_line" "hidden" 0xb13f}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb140}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb141}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb142}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb143}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb144}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb145}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb146}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb147}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb148}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb149}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb14a}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb14b}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb14c}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb14d}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb14e}
	{Tags "enemy_def_tpl" "enemy_def_ukr" "enemy_def_ukr_wpn" "hidden" 0xb14f}
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
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb164}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb165}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb166}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_line" "hidden" 0xb167}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb168}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb169}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb16a}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb16b}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb16c}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb16d}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb16e}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb16f}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb170}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb171}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb172}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb173}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb174}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb175}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb176}
	{Tags "enemy_def_tpl" "enemy_def_prc" "enemy_def_prc_wpn" "hidden" 0xb177}
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
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb18a}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb18b}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb18c}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb18d}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb18e}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_line" "hidden" 0xb18f}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb190}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb191}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb192}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb193}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb194}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb195}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb196}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb197}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb198}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb199}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb19a}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb19b}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb19c}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb19d}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb19e}
	{Tags "enemy_def_tpl" "enemy_def_nato" "enemy_def_nato_wpn" "hidden" 0xb19f}
