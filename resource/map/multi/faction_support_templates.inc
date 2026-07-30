; Friendly Allied Support multi-faction hybrid pools (player nation).
; Folded from user_nation$ the same way enemy uses bot_army$:
;   1 rusa (+sov/pol)  2 ukr  3 nato (+csa/frg)  4 prc
; Comps: line 6x4, wpn 4x4, recon 5x3, assault 4x4, eng 4x3, manpad 4x2,
;        light veh UKR 3 humvee / NATO 2 fennek (attack edge only).
; Ids 0xb200-0xb29f line/wpn; 0xb2a0+ hybrid. MIDs 9300+.
; y=-35700 line/wpn; y=-36000 hybrid inf; y=-36200 veh.
; Included from attack_support_templates.inc entity block.
; ===== ALLY RUSA LINE (24=6x4) =====
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb200
		{Position -9000 -35700}
		{Player 0}
		{MID 9300}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb201
		{Position -8980 -35700}
		{Player 0}
		{MID 9301}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb202
		{Position -8960 -35700}
		{Player 0}
		{MID 9302}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb203
		{Position -8940 -35700}
		{Player 0}
		{MID 9303}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb204
		{Position -8880 -35700}
		{Player 0}
		{MID 9304}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb205
		{Position -8860 -35700}
		{Player 0}
		{MID 9305}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb206
		{Position -8840 -35700}
		{Player 0}
		{MID 9306}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb207
		{Position -8820 -35700}
		{Player 0}
		{MID 9307}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb208
		{Position -8760 -35700}
		{Player 0}
		{MID 9308}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb209
		{Position -8740 -35700}
		{Player 0}
		{MID 9309}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb20a
		{Position -8720 -35700}
		{Player 0}
		{MID 9310}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb20b
		{Position -8700 -35700}
		{Player 0}
		{MID 9311}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb20c
		{Position -8640 -35700}
		{Player 0}
		{MID 9312}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb20d
		{Position -8620 -35700}
		{Player 0}
		{MID 9313}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb20e
		{Position -8600 -35700}
		{Player 0}
		{MID 9314}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb20f
		{Position -8580 -35700}
		{Player 0}
		{MID 9315}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb210
		{Position -8520 -35700}
		{Player 0}
		{MID 9316}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb211
		{Position -8500 -35700}
		{Player 0}
		{MID 9317}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb212
		{Position -8480 -35700}
		{Player 0}
		{MID 9318}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb213
		{Position -8460 -35700}
		{Player 0}
		{MID 9319}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb214
		{Position -8400 -35700}
		{Player 0}
		{MID 9320}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb215
		{Position -8380 -35700}
		{Player 0}
		{MID 9321}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb216
		{Position -8360 -35700}
		{Player 0}
		{MID 9322}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb217
		{Position -8340 -35700}
		{Player 0}
		{MID 9323}
		{Able "-select"}
	}

; ===== ALLY RUSA WPN (16=4x4) =====
	{Human "mp/rusa/2022s/rus90_seniorrifleman" 0xb218
		{Position -7000 -35700}
		{Player 0}
		{MID 9324}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_antitank" 0xb219
		{Position -6980 -35700}
		{Player 0}
		{MID 9325}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_marksman" 0xb21a
		{Position -6960 -35700}
		{Player 0}
		{MID 9326}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb21b
		{Position -6940 -35700}
		{Player 0}
		{MID 9327}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_seniorrifleman" 0xb21c
		{Position -6880 -35700}
		{Player 0}
		{MID 9328}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_antitank" 0xb21d
		{Position -6860 -35700}
		{Player 0}
		{MID 9329}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_marksman" 0xb21e
		{Position -6840 -35700}
		{Player 0}
		{MID 9330}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb21f
		{Position -6820 -35700}
		{Player 0}
		{MID 9331}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_seniorrifleman" 0xb220
		{Position -6760 -35700}
		{Player 0}
		{MID 9332}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_antitank" 0xb221
		{Position -6740 -35700}
		{Player 0}
		{MID 9333}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_marksman" 0xb222
		{Position -6720 -35700}
		{Player 0}
		{MID 9334}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb223
		{Position -6700 -35700}
		{Player 0}
		{MID 9335}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_seniorrifleman" 0xb224
		{Position -6640 -35700}
		{Player 0}
		{MID 9336}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_antitank" 0xb225
		{Position -6620 -35700}
		{Player 0}
		{MID 9337}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_marksman" 0xb226
		{Position -6600 -35700}
		{Player 0}
		{MID 9338}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb227
		{Position -6580 -35700}
		{Player 0}
		{MID 9339}
		{Able "-select"}
	}

; ===== ALLY UKR LINE (24=6x4) =====
	{Human "mp/ukr/2022s/ter_squadlead" 0xb228
		{Position -5000 -35700}
		{Player 0}
		{MID 9340}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb229
		{Position -4980 -35700}
		{Player 0}
		{MID 9341}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb22a
		{Position -4960 -35700}
		{Player 0}
		{MID 9342}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb22b
		{Position -4940 -35700}
		{Player 0}
		{MID 9343}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb22c
		{Position -4880 -35700}
		{Player 0}
		{MID 9344}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb22d
		{Position -4860 -35700}
		{Player 0}
		{MID 9345}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb22e
		{Position -4840 -35700}
		{Player 0}
		{MID 9346}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb22f
		{Position -4820 -35700}
		{Player 0}
		{MID 9347}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb230
		{Position -4760 -35700}
		{Player 0}
		{MID 9348}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb231
		{Position -4740 -35700}
		{Player 0}
		{MID 9349}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb232
		{Position -4720 -35700}
		{Player 0}
		{MID 9350}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb233
		{Position -4700 -35700}
		{Player 0}
		{MID 9351}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb234
		{Position -4640 -35700}
		{Player 0}
		{MID 9352}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb235
		{Position -4620 -35700}
		{Player 0}
		{MID 9353}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb236
		{Position -4600 -35700}
		{Player 0}
		{MID 9354}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb237
		{Position -4580 -35700}
		{Player 0}
		{MID 9355}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb238
		{Position -4520 -35700}
		{Player 0}
		{MID 9356}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb239
		{Position -4500 -35700}
		{Player 0}
		{MID 9357}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb23a
		{Position -4480 -35700}
		{Player 0}
		{MID 9358}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb23b
		{Position -4460 -35700}
		{Player 0}
		{MID 9359}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb23c
		{Position -4400 -35700}
		{Player 0}
		{MID 9360}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb23d
		{Position -4380 -35700}
		{Player 0}
		{MID 9361}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb23e
		{Position -4360 -35700}
		{Player 0}
		{MID 9362}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb23f
		{Position -4340 -35700}
		{Player 0}
		{MID 9363}
		{Able "-select"}
	}

; ===== ALLY UKR WPN (16=4x4) =====
	{Human "mp/ukr/2022s/ter_squadlead" 0xb240
		{Position -3000 -35700}
		{Player 0}
		{MID 9364}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_antitank" 0xb241
		{Position -2980 -35700}
		{Player 0}
		{MID 9365}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_marksman" 0xb242
		{Position -2960 -35700}
		{Player 0}
		{MID 9366}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb243
		{Position -2940 -35700}
		{Player 0}
		{MID 9367}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb244
		{Position -2880 -35700}
		{Player 0}
		{MID 9368}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_antitank" 0xb245
		{Position -2860 -35700}
		{Player 0}
		{MID 9369}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_marksman" 0xb246
		{Position -2840 -35700}
		{Player 0}
		{MID 9370}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb247
		{Position -2820 -35700}
		{Player 0}
		{MID 9371}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb248
		{Position -2760 -35700}
		{Player 0}
		{MID 9372}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_antitank" 0xb249
		{Position -2740 -35700}
		{Player 0}
		{MID 9373}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_marksman" 0xb24a
		{Position -2720 -35700}
		{Player 0}
		{MID 9374}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb24b
		{Position -2700 -35700}
		{Player 0}
		{MID 9375}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb24c
		{Position -2640 -35700}
		{Player 0}
		{MID 9376}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_antitank" 0xb24d
		{Position -2620 -35700}
		{Player 0}
		{MID 9377}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_marksman" 0xb24e
		{Position -2600 -35700}
		{Player 0}
		{MID 9378}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb24f
		{Position -2580 -35700}
		{Player 0}
		{MID 9379}
		{Able "-select"}
	}

; ===== ALLY PRC LINE (24=6x4) =====
	{Human "mp/prc/2022s/pla_squadlead" 0xb250
		{Position -1000 -35700}
		{Player 0}
		{MID 9380}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb251
		{Position -980 -35700}
		{Player 0}
		{MID 9381}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb252
		{Position -960 -35700}
		{Player 0}
		{MID 9382}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb253
		{Position -940 -35700}
		{Player 0}
		{MID 9383}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_squadlead" 0xb254
		{Position -880 -35700}
		{Player 0}
		{MID 9384}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb255
		{Position -860 -35700}
		{Player 0}
		{MID 9385}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb256
		{Position -840 -35700}
		{Player 0}
		{MID 9386}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb257
		{Position -820 -35700}
		{Player 0}
		{MID 9387}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_squadlead" 0xb258
		{Position -760 -35700}
		{Player 0}
		{MID 9388}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb259
		{Position -740 -35700}
		{Player 0}
		{MID 9389}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb25a
		{Position -720 -35700}
		{Player 0}
		{MID 9390}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb25b
		{Position -700 -35700}
		{Player 0}
		{MID 9391}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_squadlead" 0xb25c
		{Position -640 -35700}
		{Player 0}
		{MID 9392}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb25d
		{Position -620 -35700}
		{Player 0}
		{MID 9393}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb25e
		{Position -600 -35700}
		{Player 0}
		{MID 9394}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb25f
		{Position -580 -35700}
		{Player 0}
		{MID 9395}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_squadlead" 0xb260
		{Position -520 -35700}
		{Player 0}
		{MID 9396}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb261
		{Position -500 -35700}
		{Player 0}
		{MID 9397}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb262
		{Position -480 -35700}
		{Player 0}
		{MID 9398}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb263
		{Position -460 -35700}
		{Player 0}
		{MID 9399}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_squadlead" 0xb264
		{Position -400 -35700}
		{Player 0}
		{MID 9400}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb265
		{Position -380 -35700}
		{Player 0}
		{MID 9401}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb266
		{Position -360 -35700}
		{Player 0}
		{MID 9402}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb267
		{Position -340 -35700}
		{Player 0}
		{MID 9403}
		{Able "-select"}
	}

; ===== ALLY PRC WPN (16=4x4) =====
	{Human "mp/prc/2022s/pla_senior" 0xb268
		{Position 1000 -35700}
		{Player 0}
		{MID 9404}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_antitank_pf98" 0xb269
		{Position 1020 -35700}
		{Player 0}
		{MID 9405}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_marksman" 0xb26a
		{Position 1040 -35700}
		{Player 0}
		{MID 9406}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb26b
		{Position 1060 -35700}
		{Player 0}
		{MID 9407}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_senior" 0xb26c
		{Position 1120 -35700}
		{Player 0}
		{MID 9408}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_antitank_pf98" 0xb26d
		{Position 1140 -35700}
		{Player 0}
		{MID 9409}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_marksman" 0xb26e
		{Position 1160 -35700}
		{Player 0}
		{MID 9410}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb26f
		{Position 1180 -35700}
		{Player 0}
		{MID 9411}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_senior" 0xb270
		{Position 1240 -35700}
		{Player 0}
		{MID 9412}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_antitank_pf98" 0xb271
		{Position 1260 -35700}
		{Player 0}
		{MID 9413}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_marksman" 0xb272
		{Position 1280 -35700}
		{Player 0}
		{MID 9414}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb273
		{Position 1300 -35700}
		{Player 0}
		{MID 9415}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_senior" 0xb274
		{Position 1360 -35700}
		{Player 0}
		{MID 9416}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_antitank_pf98" 0xb275
		{Position 1380 -35700}
		{Player 0}
		{MID 9417}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_marksman" 0xb276
		{Position 1400 -35700}
		{Player 0}
		{MID 9418}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb277
		{Position 1420 -35700}
		{Player 0}
		{MID 9419}
		{Able "-select"}
	}

; ===== ALLY NATO LINE (24=6x4) =====
	{Human "mp/nato/2022s/nato_squadlead" 0xb278
		{Position 3000 -35700}
		{Player 0}
		{MID 9420}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb279
		{Position 3020 -35700}
		{Player 0}
		{MID 9421}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb27a
		{Position 3040 -35700}
		{Player 0}
		{MID 9422}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb27b
		{Position 3060 -35700}
		{Player 0}
		{MID 9423}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_squadlead" 0xb27c
		{Position 3120 -35700}
		{Player 0}
		{MID 9424}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb27d
		{Position 3140 -35700}
		{Player 0}
		{MID 9425}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb27e
		{Position 3160 -35700}
		{Player 0}
		{MID 9426}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb27f
		{Position 3180 -35700}
		{Player 0}
		{MID 9427}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_squadlead" 0xb280
		{Position 3240 -35700}
		{Player 0}
		{MID 9428}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb281
		{Position 3260 -35700}
		{Player 0}
		{MID 9429}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb282
		{Position 3280 -35700}
		{Player 0}
		{MID 9430}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb283
		{Position 3300 -35700}
		{Player 0}
		{MID 9431}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_squadlead" 0xb284
		{Position 3360 -35700}
		{Player 0}
		{MID 9432}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb285
		{Position 3380 -35700}
		{Player 0}
		{MID 9433}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb286
		{Position 3400 -35700}
		{Player 0}
		{MID 9434}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb287
		{Position 3420 -35700}
		{Player 0}
		{MID 9435}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_squadlead" 0xb288
		{Position 3480 -35700}
		{Player 0}
		{MID 9436}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb289
		{Position 3500 -35700}
		{Player 0}
		{MID 9437}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb28a
		{Position 3520 -35700}
		{Player 0}
		{MID 9438}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb28b
		{Position 3540 -35700}
		{Player 0}
		{MID 9439}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_squadlead" 0xb28c
		{Position 3600 -35700}
		{Player 0}
		{MID 9440}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb28d
		{Position 3620 -35700}
		{Player 0}
		{MID 9441}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb28e
		{Position 3640 -35700}
		{Player 0}
		{MID 9442}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb28f
		{Position 3660 -35700}
		{Player 0}
		{MID 9443}
		{Able "-select"}
	}

; ===== ALLY NATO WPN (16=4x4) =====
	{Human "mp/nato/2022s/nato_teamlead" 0xb290
		{Position 5000 -35700}
		{Player 0}
		{MID 9444}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_antitank" 0xb291
		{Position 5020 -35700}
		{Player 0}
		{MID 9445}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_sniper" 0xb292
		{Position 5040 -35700}
		{Player 0}
		{MID 9446}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb293
		{Position 5060 -35700}
		{Player 0}
		{MID 9447}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_teamlead" 0xb294
		{Position 5120 -35700}
		{Player 0}
		{MID 9448}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_antitank" 0xb295
		{Position 5140 -35700}
		{Player 0}
		{MID 9449}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_sniper" 0xb296
		{Position 5160 -35700}
		{Player 0}
		{MID 9450}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb297
		{Position 5180 -35700}
		{Player 0}
		{MID 9451}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_teamlead" 0xb298
		{Position 5240 -35700}
		{Player 0}
		{MID 9452}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_antitank" 0xb299
		{Position 5260 -35700}
		{Player 0}
		{MID 9453}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_sniper" 0xb29a
		{Position 5280 -35700}
		{Player 0}
		{MID 9454}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb29b
		{Position 5300 -35700}
		{Player 0}
		{MID 9455}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_teamlead" 0xb29c
		{Position 5360 -35700}
		{Player 0}
		{MID 9456}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_antitank" 0xb29d
		{Position 5380 -35700}
		{Player 0}
		{MID 9457}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_sniper" 0xb29e
		{Position 5400 -35700}
		{Player 0}
		{MID 9458}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb29f
		{Position 5420 -35700}
		{Player 0}
		{MID 9459}
		{Able "-select"}
	}

; ===== HYBRID POOLS (recon/assault/eng/manpad/light veh) =====
; Ids 0xb2a0+, MIDs 9460+. y=-36000/-36200 clear of line/wpn at -35700.
; Depth: recon 5x3, assault 4x4, eng 4x3, manpad 4x2; UKR 3 humvee, NATO 2 fennek.

; ----- ALLY RUSA RECON (15=5x3) -----
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb2a0
		{Position -9000 -36000}
		{Player 0}
		{MID 9460}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_recon" 0xb2a1
		{Position -8980 -36000}
		{Player 0}
		{MID 9461}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_recon" 0xb2a2
		{Position -8960 -36000}
		{Player 0}
		{MID 9462}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb2a3
		{Position -8880 -36000}
		{Player 0}
		{MID 9463}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_recon" 0xb2a4
		{Position -8860 -36000}
		{Player 0}
		{MID 9464}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_recon" 0xb2a5
		{Position -8840 -36000}
		{Player 0}
		{MID 9465}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb2a6
		{Position -8760 -36000}
		{Player 0}
		{MID 9466}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_recon" 0xb2a7
		{Position -8740 -36000}
		{Player 0}
		{MID 9467}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_marksman" 0xb2a8
		{Position -8720 -36000}
		{Player 0}
		{MID 9468}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb2a9
		{Position -8640 -36000}
		{Player 0}
		{MID 9469}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_recon" 0xb2aa
		{Position -8620 -36000}
		{Player 0}
		{MID 9470}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_recon" 0xb2ab
		{Position -8600 -36000}
		{Player 0}
		{MID 9471}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb2ac
		{Position -8520 -36000}
		{Player 0}
		{MID 9472}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_recon" 0xb2ad
		{Position -8500 -36000}
		{Player 0}
		{MID 9473}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_recon" 0xb2ae
		{Position -8480 -36000}
		{Player 0}
		{MID 9474}
		{Able "-select"}
	}

; ----- ALLY RUSA ASSAULT (16=4x4) -----
	{Human "mp/rusa/2022s/45vdv_squadlead" 0xb2af
		{Position -8200 -36000}
		{Player 0}
		{MID 9475}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_rifleman" 0xb2b0
		{Position -8180 -36000}
		{Player 0}
		{MID 9476}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_lmg" 0xb2b1
		{Position -8160 -36000}
		{Player 0}
		{MID 9477}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_antitank" 0xb2b2
		{Position -8140 -36000}
		{Player 0}
		{MID 9478}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_squadlead" 0xb2b3
		{Position -8080 -36000}
		{Player 0}
		{MID 9479}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_recon" 0xb2b4
		{Position -8060 -36000}
		{Player 0}
		{MID 9480}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_rifleman" 0xb2b5
		{Position -8040 -36000}
		{Player 0}
		{MID 9481}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_antitank" 0xb2b6
		{Position -8020 -36000}
		{Player 0}
		{MID 9482}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_squadlead" 0xb2b7
		{Position -7960 -36000}
		{Player 0}
		{MID 9483}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_rifleman" 0xb2b8
		{Position -7940 -36000}
		{Player 0}
		{MID 9484}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_lmg" 0xb2b9
		{Position -7920 -36000}
		{Player 0}
		{MID 9485}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_rifleman" 0xb2ba
		{Position -7900 -36000}
		{Player 0}
		{MID 9486}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_squadlead" 0xb2bb
		{Position -7840 -36000}
		{Player 0}
		{MID 9487}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_recon" 0xb2bc
		{Position -7820 -36000}
		{Player 0}
		{MID 9488}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_lmg" 0xb2bd
		{Position -7800 -36000}
		{Player 0}
		{MID 9489}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_antitank" 0xb2be
		{Position -7780 -36000}
		{Player 0}
		{MID 9490}
		{Able "-select"}
	}

; ----- ALLY RUSA ENG (12=4x3) -----
	{Human "mp/rusa/2022s/rus_eng" 0xb2bf
		{Position -7400 -36000}
		{Player 0}
		{MID 9491}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_eng" 0xb2c0
		{Position -7380 -36000}
		{Player 0}
		{MID 9492}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb2c1
		{Position -7360 -36000}
		{Player 0}
		{MID 9493}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_eng" 0xb2c2
		{Position -7280 -36000}
		{Player 0}
		{MID 9494}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_eng" 0xb2c3
		{Position -7260 -36000}
		{Player 0}
		{MID 9495}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb2c4
		{Position -7240 -36000}
		{Player 0}
		{MID 9496}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_eng" 0xb2c5
		{Position -7160 -36000}
		{Player 0}
		{MID 9497}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_eng" 0xb2c6
		{Position -7140 -36000}
		{Player 0}
		{MID 9498}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb2c7
		{Position -7120 -36000}
		{Player 0}
		{MID 9499}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_eng" 0xb2c8
		{Position -7040 -36000}
		{Player 0}
		{MID 9500}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_eng" 0xb2c9
		{Position -7020 -36000}
		{Player 0}
		{MID 9501}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb2ca
		{Position -7000 -36000}
		{Player 0}
		{MID 9502}
		{Able "-select"}
	}

; ----- ALLY RUSA MANPAD (8=4x2) -----
	{Human "mp/rusa/2022s/45vdv_manpad" 0xb2cb
		{Position -6800 -36000}
		{Player 0}
		{MID 9503}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_rifleman" 0xb2cc
		{Position -6780 -36000}
		{Player 0}
		{MID 9504}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_manpad" 0xb2cd
		{Position -6720 -36000}
		{Player 0}
		{MID 9505}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_rifleman" 0xb2ce
		{Position -6700 -36000}
		{Player 0}
		{MID 9506}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_manpad" 0xb2cf
		{Position -6640 -36000}
		{Player 0}
		{MID 9507}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_recon" 0xb2d0
		{Position -6620 -36000}
		{Player 0}
		{MID 9508}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_manpad" 0xb2d1
		{Position -6560 -36000}
		{Player 0}
		{MID 9509}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/45vdv_rifleman" 0xb2d2
		{Position -6540 -36000}
		{Player 0}
		{MID 9510}
		{Able "-select"}
	}

; ----- ALLY UKR RECON (15=5x3) -----
	{Human "mp/ukr/2022s/ter_squadlead" 0xb2d3
		{Position -5000 -36000}
		{Player 0}
		{MID 9511}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_recon" 0xb2d4
		{Position -4980 -36000}
		{Player 0}
		{MID 9512}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_recon" 0xb2d5
		{Position -4960 -36000}
		{Player 0}
		{MID 9513}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb2d6
		{Position -4880 -36000}
		{Player 0}
		{MID 9514}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_recon" 0xb2d7
		{Position -4860 -36000}
		{Player 0}
		{MID 9515}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_marksman" 0xb2d8
		{Position -4840 -36000}
		{Player 0}
		{MID 9516}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb2d9
		{Position -4760 -36000}
		{Player 0}
		{MID 9517}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_recon" 0xb2da
		{Position -4740 -36000}
		{Player 0}
		{MID 9518}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_recon" 0xb2db
		{Position -4720 -36000}
		{Player 0}
		{MID 9519}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb2dc
		{Position -4640 -36000}
		{Player 0}
		{MID 9520}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_recon" 0xb2dd
		{Position -4620 -36000}
		{Player 0}
		{MID 9521}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb2de
		{Position -4600 -36000}
		{Player 0}
		{MID 9522}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb2df
		{Position -4520 -36000}
		{Player 0}
		{MID 9523}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_recon" 0xb2e0
		{Position -4500 -36000}
		{Player 0}
		{MID 9524}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_recon" 0xb2e1
		{Position -4480 -36000}
		{Player 0}
		{MID 9525}
		{Able "-select"}
	}

; ----- ALLY UKR ASSAULT (16=4x4) -----
	{Human "mp/ukr/2022s/demon_squadlead" 0xb2e2
		{Position -4200 -36000}
		{Player 0}
		{MID 9526}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_seniorrifleman" 0xb2e3
		{Position -4180 -36000}
		{Player 0}
		{MID 9527}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_rifleman" 0xb2e4
		{Position -4160 -36000}
		{Player 0}
		{MID 9528}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_antitank" 0xb2e5
		{Position -4140 -36000}
		{Player 0}
		{MID 9529}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_squadlead" 0xb2e6
		{Position -4080 -36000}
		{Player 0}
		{MID 9530}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_rifleman" 0xb2e7
		{Position -4060 -36000}
		{Player 0}
		{MID 9531}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_mg_rpk" 0xb2e8
		{Position -4040 -36000}
		{Player 0}
		{MID 9532}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_antitank" 0xb2e9
		{Position -4020 -36000}
		{Player 0}
		{MID 9533}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_squadlead" 0xb2ea
		{Position -3960 -36000}
		{Player 0}
		{MID 9534}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_seniorrifleman" 0xb2eb
		{Position -3940 -36000}
		{Player 0}
		{MID 9535}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_mg_rpk" 0xb2ec
		{Position -3920 -36000}
		{Player 0}
		{MID 9536}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_rifleman" 0xb2ed
		{Position -3900 -36000}
		{Player 0}
		{MID 9537}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_squadlead" 0xb2ee
		{Position -3840 -36000}
		{Player 0}
		{MID 9538}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_rifleman" 0xb2ef
		{Position -3820 -36000}
		{Player 0}
		{MID 9539}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_rifleman" 0xb2f0
		{Position -3800 -36000}
		{Player 0}
		{MID 9540}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/demon_antitank" 0xb2f1
		{Position -3780 -36000}
		{Player 0}
		{MID 9541}
		{Able "-select"}
	}

; ----- ALLY UKR ENG (12=4x3) -----
	{Human "mp/ukr/2022s/ukr_eng" 0xb2f2
		{Position -3400 -36000}
		{Player 0}
		{MID 9542}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_eng" 0xb2f3
		{Position -3380 -36000}
		{Player 0}
		{MID 9543}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb2f4
		{Position -3360 -36000}
		{Player 0}
		{MID 9544}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_eng" 0xb2f5
		{Position -3280 -36000}
		{Player 0}
		{MID 9545}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_eng" 0xb2f6
		{Position -3260 -36000}
		{Player 0}
		{MID 9546}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb2f7
		{Position -3240 -36000}
		{Player 0}
		{MID 9547}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_eng" 0xb2f8
		{Position -3160 -36000}
		{Player 0}
		{MID 9548}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_eng" 0xb2f9
		{Position -3140 -36000}
		{Player 0}
		{MID 9549}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb2fa
		{Position -3120 -36000}
		{Player 0}
		{MID 9550}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_eng" 0xb2fb
		{Position -3040 -36000}
		{Player 0}
		{MID 9551}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_eng" 0xb2fc
		{Position -3020 -36000}
		{Player 0}
		{MID 9552}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb2fd
		{Position -3000 -36000}
		{Player 0}
		{MID 9553}
		{Able "-select"}
	}

; ----- ALLY UKR MANPAD (8=4x2) -----
	{Human "mp/ukr/2022s/ukr_manpad_operator" 0xb2fe
		{Position -2800 -36000}
		{Player 0}
		{MID 9554}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_manpad_supporter" 0xb2ff
		{Position -2780 -36000}
		{Player 0}
		{MID 9555}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_manpad_operator" 0xb300
		{Position -2720 -36000}
		{Player 0}
		{MID 9556}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_manpad_supporter" 0xb301
		{Position -2700 -36000}
		{Player 0}
		{MID 9557}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_manpad_operator" 0xb302
		{Position -2640 -36000}
		{Player 0}
		{MID 9558}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_manpad_supporter" 0xb303
		{Position -2620 -36000}
		{Player 0}
		{MID 9559}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_manpad_operator" 0xb304
		{Position -2560 -36000}
		{Player 0}
		{MID 9560}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_manpad_supporter" 0xb305
		{Position -2540 -36000}
		{Player 0}
		{MID 9561}
		{Able "-select"}
	}

; ----- ALLY UKR LIGHT VEH (3 hull+crew) -----
	{Entity "humvee_m2hb_ukr" 0xb306
		{Position -5000 -36200}
		{Player 0}
		{MID 9562}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xb307
		{Position -4995 -36200}
		{Player 0}
		{MID 9563}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xb308
		{Position -4990 -36200}
		{Player 0}
		{MID 9564}
		{Able "-select"}
	}
	{Entity "humvee_m2hb_ukr" 0xb309
		{Position -4900 -36200}
		{Player 0}
		{MID 9565}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xb30a
		{Position -4895 -36200}
		{Player 0}
		{MID 9566}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xb30b
		{Position -4890 -36200}
		{Player 0}
		{MID 9567}
		{Able "-select"}
	}
	{Entity "humvee_m2hb_ukr" 0xb30c
		{Position -4800 -36200}
		{Player 0}
		{MID 9568}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xb30d
		{Position -4795 -36200}
		{Player 0}
		{MID 9569}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xb30e
		{Position -4790 -36200}
		{Player 0}
		{MID 9570}
		{Able "-select"}
	}

; ----- ALLY PRC RECON (15=5x3) -----
	{Human "mp/prc/2022s/pla_squadlead" 0xb30f
		{Position -1000 -36000}
		{Player 0}
		{MID 9571}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_recon" 0xb310
		{Position -980 -36000}
		{Player 0}
		{MID 9572}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_recon" 0xb311
		{Position -960 -36000}
		{Player 0}
		{MID 9573}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_squadlead" 0xb312
		{Position -880 -36000}
		{Player 0}
		{MID 9574}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_recon" 0xb313
		{Position -860 -36000}
		{Player 0}
		{MID 9575}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_marksman" 0xb314
		{Position -840 -36000}
		{Player 0}
		{MID 9576}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_squadlead" 0xb315
		{Position -760 -36000}
		{Player 0}
		{MID 9577}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_recon" 0xb316
		{Position -740 -36000}
		{Player 0}
		{MID 9578}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb317
		{Position -720 -36000}
		{Player 0}
		{MID 9579}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_squadlead" 0xb318
		{Position -640 -36000}
		{Player 0}
		{MID 9580}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_recon" 0xb319
		{Position -620 -36000}
		{Player 0}
		{MID 9581}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_recon" 0xb31a
		{Position -600 -36000}
		{Player 0}
		{MID 9582}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_squadlead" 0xb31b
		{Position -520 -36000}
		{Player 0}
		{MID 9583}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_recon" 0xb31c
		{Position -500 -36000}
		{Player 0}
		{MID 9584}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb31d
		{Position -480 -36000}
		{Player 0}
		{MID 9585}
		{Able "-select"}
	}

; ----- ALLY PRC ASSAULT (16=4x4) -----
	{Human "mp/prc/2022s/pla_squadlead" 0xb31e
		{Position -200 -36000}
		{Player 0}
		{MID 9586}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_recon" 0xb31f
		{Position -180 -36000}
		{Player 0}
		{MID 9587}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb320
		{Position -160 -36000}
		{Player 0}
		{MID 9588}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_antitank_pf98" 0xb321
		{Position -140 -36000}
		{Player 0}
		{MID 9589}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_squadlead" 0xb322
		{Position -80 -36000}
		{Player 0}
		{MID 9590}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_recon" 0xb323
		{Position -60 -36000}
		{Player 0}
		{MID 9591}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb324
		{Position -40 -36000}
		{Player 0}
		{MID 9592}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb325
		{Position -20 -36000}
		{Player 0}
		{MID 9593}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_senior" 0xb326
		{Position 40 -36000}
		{Player 0}
		{MID 9594}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_recon" 0xb327
		{Position 60 -36000}
		{Player 0}
		{MID 9595}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb328
		{Position 80 -36000}
		{Player 0}
		{MID 9596}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_antitank_pf98" 0xb329
		{Position 100 -36000}
		{Player 0}
		{MID 9597}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_squadlead" 0xb32a
		{Position 160 -36000}
		{Player 0}
		{MID 9598}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_recon" 0xb32b
		{Position 180 -36000}
		{Player 0}
		{MID 9599}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_marksman" 0xb32c
		{Position 200 -36000}
		{Player 0}
		{MID 9600}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb32d
		{Position 220 -36000}
		{Player 0}
		{MID 9601}
		{Able "-select"}
	}

; ----- ALLY PRC ENG (12=4x3) -----
	{Human "mp/prc/2022s/pla_eng" 0xb32e
		{Position 600 -36000}
		{Player 0}
		{MID 9602}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_eng" 0xb32f
		{Position 620 -36000}
		{Player 0}
		{MID 9603}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb330
		{Position 640 -36000}
		{Player 0}
		{MID 9604}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_eng" 0xb331
		{Position 720 -36000}
		{Player 0}
		{MID 9605}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_eng" 0xb332
		{Position 740 -36000}
		{Player 0}
		{MID 9606}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb333
		{Position 760 -36000}
		{Player 0}
		{MID 9607}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_eng" 0xb334
		{Position 840 -36000}
		{Player 0}
		{MID 9608}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_eng" 0xb335
		{Position 860 -36000}
		{Player 0}
		{MID 9609}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb336
		{Position 880 -36000}
		{Player 0}
		{MID 9610}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_eng" 0xb337
		{Position 960 -36000}
		{Player 0}
		{MID 9611}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_eng" 0xb338
		{Position 980 -36000}
		{Player 0}
		{MID 9612}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb339
		{Position 1000 -36000}
		{Player 0}
		{MID 9613}
		{Able "-select"}
	}

; ----- ALLY PRC MANPAD (8=4x2) -----
	{Human "mp/prc/2022s/pla_manpad_operator" 0xb33a
		{Position 1200 -36000}
		{Player 0}
		{MID 9614}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_manpad_supporter" 0xb33b
		{Position 1220 -36000}
		{Player 0}
		{MID 9615}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_manpad_operator" 0xb33c
		{Position 1280 -36000}
		{Player 0}
		{MID 9616}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_manpad_supporter" 0xb33d
		{Position 1300 -36000}
		{Player 0}
		{MID 9617}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_manpad_operator" 0xb33e
		{Position 1360 -36000}
		{Player 0}
		{MID 9618}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_manpad_supporter" 0xb33f
		{Position 1380 -36000}
		{Player 0}
		{MID 9619}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_manpad_operator" 0xb340
		{Position 1440 -36000}
		{Player 0}
		{MID 9620}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_manpad_supporter" 0xb341
		{Position 1460 -36000}
		{Player 0}
		{MID 9621}
		{Able "-select"}
	}

; ----- ALLY NATO RECON (15=5x3) -----
	{Human "mp/nato/2022s/nato_squadlead" 0xb342
		{Position 3000 -36000}
		{Player 0}
		{MID 9622}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_scout" 0xb343
		{Position 3020 -36000}
		{Player 0}
		{MID 9623}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_scout" 0xb344
		{Position 3040 -36000}
		{Player 0}
		{MID 9624}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_squadlead" 0xb345
		{Position 3120 -36000}
		{Player 0}
		{MID 9625}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_scout" 0xb346
		{Position 3140 -36000}
		{Player 0}
		{MID 9626}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_scout" 0xb347
		{Position 3160 -36000}
		{Player 0}
		{MID 9627}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_squadlead" 0xb348
		{Position 3240 -36000}
		{Player 0}
		{MID 9628}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_scout" 0xb349
		{Position 3260 -36000}
		{Player 0}
		{MID 9629}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_sniper" 0xb34a
		{Position 3280 -36000}
		{Player 0}
		{MID 9630}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_squadlead" 0xb34b
		{Position 3360 -36000}
		{Player 0}
		{MID 9631}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_scout" 0xb34c
		{Position 3380 -36000}
		{Player 0}
		{MID 9632}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_scout" 0xb34d
		{Position 3400 -36000}
		{Player 0}
		{MID 9633}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_squadlead" 0xb34e
		{Position 3480 -36000}
		{Player 0}
		{MID 9634}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_scout" 0xb34f
		{Position 3500 -36000}
		{Player 0}
		{MID 9635}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb350
		{Position 3520 -36000}
		{Player 0}
		{MID 9636}
		{Able "-select"}
	}

; ----- ALLY NATO ASSAULT (16=4x4) -----
	{Human "mp/nato/2022s/82nd_squadlead" 0xb351
		{Position 3800 -36000}
		{Player 0}
		{MID 9637}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_rifleman" 0xb352
		{Position 3820 -36000}
		{Player 0}
		{MID 9638}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_mg" 0xb353
		{Position 3840 -36000}
		{Player 0}
		{MID 9639}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_antitank" 0xb354
		{Position 3860 -36000}
		{Player 0}
		{MID 9640}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_squadlead" 0xb355
		{Position 3920 -36000}
		{Player 0}
		{MID 9641}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_scout" 0xb356
		{Position 3940 -36000}
		{Player 0}
		{MID 9642}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_rifleman" 0xb357
		{Position 3960 -36000}
		{Player 0}
		{MID 9643}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_antitank" 0xb358
		{Position 3980 -36000}
		{Player 0}
		{MID 9644}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_squadlead" 0xb359
		{Position 4040 -36000}
		{Player 0}
		{MID 9645}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_rifleman" 0xb35a
		{Position 4060 -36000}
		{Player 0}
		{MID 9646}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_mg" 0xb35b
		{Position 4080 -36000}
		{Player 0}
		{MID 9647}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_rifleman" 0xb35c
		{Position 4100 -36000}
		{Player 0}
		{MID 9648}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_squadlead" 0xb35d
		{Position 4160 -36000}
		{Player 0}
		{MID 9649}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_rifleman" 0xb35e
		{Position 4180 -36000}
		{Player 0}
		{MID 9650}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_marksman" 0xb35f
		{Position 4200 -36000}
		{Player 0}
		{MID 9651}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_antitank" 0xb360
		{Position 4220 -36000}
		{Player 0}
		{MID 9652}
		{Able "-select"}
	}

; ----- ALLY NATO ENG (12=4x3) -----
	{Human "mp/nato/2022s/usmc_eng" 0xb361
		{Position 4600 -36000}
		{Player 0}
		{MID 9653}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_eng" 0xb362
		{Position 4620 -36000}
		{Player 0}
		{MID 9654}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb363
		{Position 4640 -36000}
		{Player 0}
		{MID 9655}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_engineer" 0xb364
		{Position 4720 -36000}
		{Player 0}
		{MID 9656}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_engineer" 0xb365
		{Position 4740 -36000}
		{Player 0}
		{MID 9657}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb366
		{Position 4760 -36000}
		{Player 0}
		{MID 9658}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_eng" 0xb367
		{Position 4840 -36000}
		{Player 0}
		{MID 9659}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_eng" 0xb368
		{Position 4860 -36000}
		{Player 0}
		{MID 9660}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb369
		{Position 4880 -36000}
		{Player 0}
		{MID 9661}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/1ad_engineer" 0xb36a
		{Position 4960 -36000}
		{Player 0}
		{MID 9662}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_eng" 0xb36b
		{Position 4980 -36000}
		{Player 0}
		{MID 9663}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb36c
		{Position 5000 -36000}
		{Player 0}
		{MID 9664}
		{Able "-select"}
	}

; ----- ALLY NATO MANPAD (8=4x2) -----
	{Human "mp/nato/2022s/usmc_manpad" 0xb36d
		{Position 5200 -36000}
		{Player 0}
		{MID 9665}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_manpad_ass" 0xb36e
		{Position 5220 -36000}
		{Player 0}
		{MID 9666}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_manpad" 0xb36f
		{Position 5280 -36000}
		{Player 0}
		{MID 9667}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_manpad_ass" 0xb370
		{Position 5300 -36000}
		{Player 0}
		{MID 9668}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_manpad" 0xb371
		{Position 5360 -36000}
		{Player 0}
		{MID 9669}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_manpad_ass" 0xb372
		{Position 5380 -36000}
		{Player 0}
		{MID 9670}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_manpad" 0xb373
		{Position 5440 -36000}
		{Player 0}
		{MID 9671}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usmc_manpad_ass" 0xb374
		{Position 5460 -36000}
		{Player 0}
		{MID 9672}
		{Able "-select"}
	}

; ----- ALLY NATO LIGHT VEH (2 hull+crew) -----
	{Entity "fennek" 0xb375
		{Position 3000 -36200}
		{Player 0}
		{MID 9673}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_vehicleman" 0xb376
		{Position 3005 -36200}
		{Player 0}
		{MID 9674}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_vehicleman" 0xb377
		{Position 3010 -36200}
		{Player 0}
		{MID 9675}
		{Able "-select"}
	}
	{Entity "fennek" 0xb378
		{Position 3100 -36200}
		{Player 0}
		{MID 9676}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_vehicleman" 0xb379
		{Position 3105 -36200}
		{Player 0}
		{MID 9677}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/pzgd_vehicleman" 0xb37a
		{Position 3110 -36200}
		{Player 0}
		{MID 9678}
		{Able "-select"}
	}

; ----- VEHICLE CREW LINKS -----
	{Link 0xb307 {0xb306 "driver"}}
	{Link 0xb308 {0xb306 "gunner2"}}
	{Link 0xb30a {0xb309 "driver"}}
	{Link 0xb30b {0xb309 "gunner2"}}
	{Link 0xb30d {0xb30c "driver"}}
	{Link 0xb30e {0xb30c "gunner2"}}
	{Link 0xb376 {0xb375 "driver"}}
	{Link 0xb377 {0xb375 "gunner2"}}
	{Link 0xb379 {0xb378 "driver"}}
	{Link 0xb37a {0xb378 "gunner2"}}


; ===== RARE IFV PACKAGES (cmd 17, attack L2+/L3, cap 1) =====
; MID 9700-9731 ids 0xb380+. y=-36400.

; ----- ALLY RUSA IFV -----
	{Entity "bmp2_rus" 0xb380
		{Position -9000 -36400}
		{Player 0}
		{MID 9700}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xb381
		{Position -8995 -36400}
		{Player 0}
		{MID 9701}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xb382
		{Position -8990 -36400}
		{Player 0}
		{MID 9702}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xb383
		{Position -8985 -36400}
		{Player 0}
		{MID 9703}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb384
		{Position -8970 -36400}
		{Player 0}
		{MID 9704}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb385
		{Position -8950 -36400}
		{Player 0}
		{MID 9705}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb386
		{Position -8930 -36400}
		{Player 0}
		{MID 9706}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb387
		{Position -8910 -36400}
		{Player 0}
		{MID 9707}
		{Able "-select"}
	}

; ----- ALLY UKR IFV -----
	{Entity "bmp2_ukr" 0xb388
		{Position -5000 -36400}
		{Player 0}
		{MID 9708}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xb389
		{Position -4995 -36400}
		{Player 0}
		{MID 9709}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xb38a
		{Position -4990 -36400}
		{Player 0}
		{MID 9710}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xb38b
		{Position -4985 -36400}
		{Player 0}
		{MID 9711}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb38c
		{Position -4970 -36400}
		{Player 0}
		{MID 9712}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb38d
		{Position -4950 -36400}
		{Player 0}
		{MID 9713}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb38e
		{Position -4930 -36400}
		{Player 0}
		{MID 9714}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb38f
		{Position -4910 -36400}
		{Player 0}
		{MID 9715}
		{Able "-select"}
	}

; ----- ALLY PRC IFV -----
	{Entity "zbl08" 0xb390
		{Position -1000 -36400}
		{Player 0}
		{MID 9716}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_crew" 0xb391
		{Position -995 -36400}
		{Player 0}
		{MID 9717}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_crew" 0xb392
		{Position -990 -36400}
		{Player 0}
		{MID 9718}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_crew" 0xb393
		{Position -985 -36400}
		{Player 0}
		{MID 9719}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_squadlead" 0xb394
		{Position -970 -36400}
		{Player 0}
		{MID 9720}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb395
		{Position -950 -36400}
		{Player 0}
		{MID 9721}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb396
		{Position -930 -36400}
		{Player 0}
		{MID 9722}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb397
		{Position -910 -36400}
		{Player 0}
		{MID 9723}
		{Able "-select"}
	}

; ----- ALLY NATO IFV -----
	{Entity "m2a3" 0xb398
		{Position 3000 -36400}
		{Player 0}
		{MID 9724}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xb399
		{Position 3005 -36400}
		{Player 0}
		{MID 9725}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xb39a
		{Position 3010 -36400}
		{Player 0}
		{MID 9726}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xb39b
		{Position 3015 -36400}
		{Player 0}
		{MID 9727}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_squadlead" 0xb39c
		{Position 3030 -36400}
		{Player 0}
		{MID 9728}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb39d
		{Position 3050 -36400}
		{Player 0}
		{MID 9729}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb39e
		{Position 3070 -36400}
		{Player 0}
		{MID 9730}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb39f
		{Position 3090 -36400}
		{Player 0}
		{MID 9731}
		{Able "-select"}
	}

; ----- IFV CREW LINKS -----
	{Link 0xb381 {0xb380 "driver"}}
	{Link 0xb382 {0xb380 "gunner"}}
	{Link 0xb383 {0xb380 "commander"}}
	{Link 0xb389 {0xb388 "driver"}}
	{Link 0xb38a {0xb388 "gunner"}}
	{Link 0xb38b {0xb388 "commander"}}
	{Link 0xb391 {0xb390 "driver"}}
	{Link 0xb392 {0xb390 "gunner"}}
	{Link 0xb393 {0xb390 "commander"}}
	{Link 0xb399 {0xb398 "driver"}}
	{Link 0xb39a {0xb398 "gunner"}}
	{Link 0xb39b {0xb398 "commander"}}

;
; ===== MOTORIZED INSERT PACKAGES (cmd 19) =====
; One troop truck per faction, crew and eight dismounts {Link}-baked into the hull's
; own placer slots so the whole squad rides in and nothing has to walk to the pad.
; Seat names come straight from the entity defs: base-game properties/car_crew.ext
; gives ("crew_2_human") the places driver + commander and ("passenger_N_human")
; the places seat1..seatN, all in placer group "passenger" for the seats. That group
; is exactly what {emit {mode passengers}} keys on, which is how the dismount works
; without ejecting the driver.
;   RUSA ural                    crew_2 + 8 pax seats
;   UKR  ural_vsu                crew_2 + 8 pax seats
;   NATO fmtv                    crew_2 + 8 pax seats
;   PRC  shaanxi_sx2190_passenger crew_2 + 8 pax (group passengers — emit-safe)
; PLA IFVs (zbl08 etc.) still put pax in group "crew"; this SX2190 troop truck does not.
; Ids 0xb3a0-0xb3cb, MIDs 9740-9783, y=-36600.
; ----- ALLY RUSA MOTORIZED (ural) -----
	{Entity "ural" 0xb3a0
		{Position -9000 -36600}
		{Player 0}
		{MID 9740}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xb3a1
		{Position -8995 -36600}
		{Player 0}
		{MID 9741}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xb3a2
		{Position -8990 -36600}
		{Player 0}
		{MID 9742}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_squadlead" 0xb3a3
		{Position -8985 -36600}
		{Player 0}
		{MID 9743}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb3a4
		{Position -8980 -36600}
		{Player 0}
		{MID 9744}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb3a5
		{Position -8975 -36600}
		{Player 0}
		{MID 9745}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb3a6
		{Position -8970 -36600}
		{Player 0}
		{MID 9746}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb3a7
		{Position -8965 -36600}
		{Player 0}
		{MID 9747}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xb3a8
		{Position -8960 -36600}
		{Player 0}
		{MID 9748}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_mg" 0xb3a9
		{Position -8955 -36600}
		{Player 0}
		{MID 9749}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_antitank" 0xb3aa
		{Position -8950 -36600}
		{Player 0}
		{MID 9750}
		{Able "-select"}
	}

; ----- ALLY UKR MOTORIZED (ural_vsu) -----
	{Entity "ural_vsu" 0xb3ab
		{Position -8800 -36600}
		{Player 0}
		{MID 9751}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xb3ac
		{Position -8795 -36600}
		{Player 0}
		{MID 9752}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xb3ad
		{Position -8790 -36600}
		{Player 0}
		{MID 9753}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_squadlead" 0xb3ae
		{Position -8785 -36600}
		{Player 0}
		{MID 9754}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb3af
		{Position -8780 -36600}
		{Player 0}
		{MID 9755}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb3b0
		{Position -8775 -36600}
		{Player 0}
		{MID 9756}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb3b1
		{Position -8770 -36600}
		{Player 0}
		{MID 9757}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb3b2
		{Position -8765 -36600}
		{Player 0}
		{MID 9758}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xb3b3
		{Position -8760 -36600}
		{Player 0}
		{MID 9759}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_mg" 0xb3b4
		{Position -8755 -36600}
		{Player 0}
		{MID 9760}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_antitank" 0xb3b5
		{Position -8750 -36600}
		{Player 0}
		{MID 9761}
		{Able "-select"}
	}

; ----- ALLY NATO MOTORIZED (fmtv) -----
	{Entity "fmtv" 0xb3b6
		{Position -8600 -36600}
		{Player 0}
		{MID 9762}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xb3b7
		{Position -8595 -36600}
		{Player 0}
		{MID 9763}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xb3b8
		{Position -8590 -36600}
		{Player 0}
		{MID 9764}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_squadlead" 0xb3b9
		{Position -8585 -36600}
		{Player 0}
		{MID 9765}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb3ba
		{Position -8580 -36600}
		{Player 0}
		{MID 9766}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb3bb
		{Position -8575 -36600}
		{Player 0}
		{MID 9767}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb3bc
		{Position -8570 -36600}
		{Player 0}
		{MID 9768}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb3bd
		{Position -8565 -36600}
		{Player 0}
		{MID 9769}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xb3be
		{Position -8560 -36600}
		{Player 0}
		{MID 9770}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_mg" 0xb3bf
		{Position -8555 -36600}
		{Player 0}
		{MID 9771}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_antitank" 0xb3c0
		{Position -8550 -36600}
		{Player 0}
		{MID 9772}
		{Able "-select"}
	}


; ----- ALLY PRC MOTORIZED (shaanxi_sx2190_passenger) -----
	{Entity "shaanxi_sx2190_passenger" 0xb3c1
		{Position -8400 -36600}
		{Player 0}
		{MID 9773}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_crew" 0xb3c2
		{Position -8395 -36600}
		{Player 0}
		{MID 9774}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_crew" 0xb3c3
		{Position -8390 -36600}
		{Player 0}
		{MID 9775}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_squadlead" 0xb3c4
		{Position -8385 -36600}
		{Player 0}
		{MID 9776}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb3c5
		{Position -8380 -36600}
		{Player 0}
		{MID 9777}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb3c6
		{Position -8375 -36600}
		{Player 0}
		{MID 9778}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb3c7
		{Position -8370 -36600}
		{Player 0}
		{MID 9779}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb3c8
		{Position -8365 -36600}
		{Player 0}
		{MID 9780}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xb3c9
		{Position -8360 -36600}
		{Player 0}
		{MID 9781}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_mg" 0xb3ca
		{Position -8355 -36600}
		{Player 0}
		{MID 9782}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_antitank_pf98" 0xb3cb
		{Position -8350 -36600}
		{Player 0}
		{MID 9783}
		{Able "-select"}
	}

; ----- MOTORIZED CREW + PASSENGER LINKS -----
	{Link 0xb3a1 {0xb3a0 "driver"}}
	{Link 0xb3a2 {0xb3a0 "commander"}}
	{Link 0xb3a3 {0xb3a0 "seat1"}}
	{Link 0xb3a4 {0xb3a0 "seat2"}}
	{Link 0xb3a5 {0xb3a0 "seat3"}}
	{Link 0xb3a6 {0xb3a0 "seat4"}}
	{Link 0xb3a7 {0xb3a0 "seat5"}}
	{Link 0xb3a8 {0xb3a0 "seat6"}}
	{Link 0xb3a9 {0xb3a0 "seat7"}}
	{Link 0xb3aa {0xb3a0 "seat8"}}
	{Link 0xb3ac {0xb3ab "driver"}}
	{Link 0xb3ad {0xb3ab "commander"}}
	{Link 0xb3ae {0xb3ab "seat1"}}
	{Link 0xb3af {0xb3ab "seat2"}}
	{Link 0xb3b0 {0xb3ab "seat3"}}
	{Link 0xb3b1 {0xb3ab "seat4"}}
	{Link 0xb3b2 {0xb3ab "seat5"}}
	{Link 0xb3b3 {0xb3ab "seat6"}}
	{Link 0xb3b4 {0xb3ab "seat7"}}
	{Link 0xb3b5 {0xb3ab "seat8"}}
	{Link 0xb3b7 {0xb3b6 "driver"}}
	{Link 0xb3b8 {0xb3b6 "commander"}}
	{Link 0xb3b9 {0xb3b6 "seat1"}}
	{Link 0xb3ba {0xb3b6 "seat2"}}
	{Link 0xb3bb {0xb3b6 "seat3"}}
	{Link 0xb3bc {0xb3b6 "seat4"}}
	{Link 0xb3bd {0xb3b6 "seat5"}}
	{Link 0xb3be {0xb3b6 "seat6"}}
	{Link 0xb3bf {0xb3b6 "seat7"}}
	{Link 0xb3c0 {0xb3b6 "seat8"}}
	{Link 0xb3c2 {0xb3c1 "driver"}}
	{Link 0xb3c3 {0xb3c1 "commander"}}
	{Link 0xb3c4 {0xb3c1 "seat1"}}
	{Link 0xb3c5 {0xb3c1 "seat2"}}
	{Link 0xb3c6 {0xb3c1 "seat3"}}
	{Link 0xb3c7 {0xb3c1 "seat4"}}
	{Link 0xb3c8 {0xb3c1 "seat5"}}
	{Link 0xb3c9 {0xb3c1 "seat6"}}
	{Link 0xb3ca {0xb3c1 "seat7"}}
	{Link 0xb3cb {0xb3c1 "seat8"}}

; ===== E2 AIR PACKAGE POOLS (default-off; West-81 supplies UH-60/C-130) =====
; IDs 0xb401-0xb42f, MIDs 9800-9846, y=-36800.
	{Entity "mi17_b8_rus" 0xb401
		{Position -9000 -36800}
		{Player 0}
		{MID 9800}
		{Able "-select"}
		{Chassis "helicopter"
			{Airborne}
			{EngineStarted}
			{Altitude 22}
		}
	}
	{Human "mp/rusa/2022s/rus_pliot" 0xb402
		{Position -8995 -36800}
		{Player 0}
		{MID 9801}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_pliot" 0xb403
		{Position -8990 -36800}
		{Player 0}
		{MID 9802}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/106vdv_squadlead" 0xb404
		{Position -8985 -36800}
		{Player 0}
		{MID 9803}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/106vdv_mg" 0xb405
		{Position -8980 -36800}
		{Player 0}
		{MID 9804}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/106vdv_rifleman" 0xb406
		{Position -8975 -36800}
		{Player 0}
		{MID 9805}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/106vdv_rifleman" 0xb407
		{Position -8970 -36800}
		{Player 0}
		{MID 9806}
		{Able "-select"}
	}
	{Entity "mi17_b8_ukr" 0xb408
		{Position -8965 -36800}
		{Player 0}
		{MID 9807}
		{Able "-select"}
		{Chassis "helicopter"
			{Airborne}
			{EngineStarted}
			{Altitude 22}
		}
	}
	{Human "mp/ukr/2022s/ukr_pilot" 0xb409
		{Position -8960 -36800}
		{Player 0}
		{MID 9808}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_pilot" 0xb40a
		{Position -8955 -36800}
		{Player 0}
		{MID 9809}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr13_squadlead" 0xb40b
		{Position -8950 -36800}
		{Player 0}
		{MID 9810}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr13_lmg" 0xb40c
		{Position -8945 -36800}
		{Player 0}
		{MID 9811}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr13_rifleman" 0xb40d
		{Position -8940 -36800}
		{Player 0}
		{MID 9812}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr13_rifleman" 0xb40e
		{Position -8935 -36800}
		{Player 0}
		{MID 9813}
		{Able "-select"}
	}
	{Entity "uh-60m_blackhawk_mg" 0xb40f
		{Position -8930 -36800}
		{Player 0}
		{MID 9814}
		{Able "-select"}
		{Chassis "helicopter"
			{Airborne}
			{EngineStarted}
			{Altitude 22}
		}
	}
	{Human "mp/nato/2022s/nato_pilot" 0xb410
		{Position -8925 -36800}
		{Player 0}
		{MID 9815}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_pilot" 0xb411
		{Position -8920 -36800}
		{Player 0}
		{MID 9816}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_squadlead" 0xb412
		{Position -8915 -36800}
		{Player 0}
		{MID 9817}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_mg" 0xb413
		{Position -8910 -36800}
		{Player 0}
		{MID 9818}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_rifleman" 0xb414
		{Position -8905 -36800}
		{Player 0}
		{MID 9819}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_rifleman" 0xb415
		{Position -8900 -36800}
		{Player 0}
		{MID 9820}
		{Able "-select"}
	}
	{Entity "il-76td_para" 0xb416
		{Position -8895 -36800}
		{Player 0}
		{MID 9821}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_pliot" 0xb417
		{Position -8890 -36800}
		{Player 0}
		{MID 9822}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_pliot" 0xb418
		{Position -8885 -36800}
		{Player 0}
		{MID 9823}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_pliot" 0xb419
		{Position -8880 -36800}
		{Player 0}
		{MID 9824}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_pliot" 0xb41a
		{Position -8875 -36800}
		{Player 0}
		{MID 9825}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_pliot" 0xb41b
		{Position -8870 -36800}
		{Player 0}
		{MID 9826}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/106vdv_squadlead" 0xb41c
		{Position -8865 -36800}
		{Player 0}
		{MID 9827}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/106vdv_mg" 0xb41d
		{Position -8860 -36800}
		{Player 0}
		{MID 9828}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/106vdv_rifleman" 0xb41e
		{Position -8855 -36800}
		{Player 0}
		{MID 9829}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/106vdv_rifleman" 0xb41f
		{Position -8850 -36800}
		{Player 0}
		{MID 9830}
		{Able "-select"}
	}
	{Entity "c130_para" 0xb420
		{Position -8845 -36800}
		{Player 0}
		{MID 9831}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_pilot" 0xb421
		{Position -8840 -36800}
		{Player 0}
		{MID 9832}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_pilot" 0xb422
		{Position -8835 -36800}
		{Player 0}
		{MID 9833}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_pilot" 0xb423
		{Position -8830 -36800}
		{Player 0}
		{MID 9834}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr13_squadlead" 0xb424
		{Position -8825 -36800}
		{Player 0}
		{MID 9835}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr13_lmg" 0xb425
		{Position -8820 -36800}
		{Player 0}
		{MID 9836}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr13_rifleman" 0xb426
		{Position -8815 -36800}
		{Player 0}
		{MID 9837}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr13_rifleman" 0xb427
		{Position -8810 -36800}
		{Player 0}
		{MID 9838}
		{Able "-select"}
	}
	{Entity "c130_para" 0xb428
		{Position -8805 -36800}
		{Player 0}
		{MID 9839}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_pilot" 0xb429
		{Position -8800 -36800}
		{Player 0}
		{MID 9840}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_pilot" 0xb42a
		{Position -8795 -36800}
		{Player 0}
		{MID 9841}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_pilot" 0xb42b
		{Position -8790 -36800}
		{Player 0}
		{MID 9842}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_squadlead" 0xb42c
		{Position -8785 -36800}
		{Player 0}
		{MID 9843}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_mg" 0xb42d
		{Position -8780 -36800}
		{Player 0}
		{MID 9844}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_rifleman" 0xb42e
		{Position -8775 -36800}
		{Player 0}
		{MID 9845}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/82nd_rifleman" 0xb42f
		{Position -8770 -36800}
		{Player 0}
		{MID 9846}
		{Able "-select"}
	}

; ----- E2 CREW + PARATROOPER LINKS -----
	{Link 0xb402 {0xb401 "driver"}}
	{Link 0xb403 {0xb401 "commander"}}
	{Link 0xb409 {0xb408 "driver"}}
	{Link 0xb40a {0xb408 "commander"}}
	{Link 0xb410 {0xb40f "driver"}}
	{Link 0xb411 {0xb40f "commander"}}
	{Link 0xb417 {0xb416 "driver"}}
	{Link 0xb418 {0xb416 "driver1"}}
	{Link 0xb419 {0xb416 "driver2"}}
	{Link 0xb41a {0xb416 "commander"}}
	{Link 0xb41b {0xb416 "commander1"}}
	{Link 0xb41c {0xb416 "seat01"}}
	{Link 0xb41d {0xb416 "seat02"}}
	{Link 0xb41e {0xb416 "seat03"}}
	{Link 0xb41f {0xb416 "seat04"}}
	{Link 0xb421 {0xb420 "driver"}}
	{Link 0xb422 {0xb420 "driver2"}}
	{Link 0xb423 {0xb420 "commander"}}
	{Link 0xb424 {0xb420 "seat02"}}
	{Link 0xb425 {0xb420 "seat03"}}
	{Link 0xb426 {0xb420 "seat04"}}
	{Link 0xb427 {0xb420 "seat05"}}
	{Link 0xb429 {0xb428 "driver"}}
	{Link 0xb42a {0xb428 "driver2"}}
	{Link 0xb42b {0xb428 "commander"}}
	{Link 0xb42c {0xb428 "seat02"}}
	{Link 0xb42d {0xb428 "seat03"}}
	{Link 0xb42e {0xb428 "seat04"}}
	{Link 0xb42f {0xb428 "seat05"}}

; ===== TAGS =====
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb200}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb201}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb202}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb203}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb204}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb205}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb206}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb207}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb208}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb209}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb20a}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb20b}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb20c}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb20d}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb20e}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb20f}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb210}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb211}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb212}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb213}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb214}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb215}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb216}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb217}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb218}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb219}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb21a}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb21b}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb21c}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb21d}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb21e}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb21f}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb220}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb221}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb222}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb223}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb224}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb225}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb226}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_wpn" "hidden" 0xb227}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb228}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb229}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb22a}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb22b}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb22c}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb22d}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb22e}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb22f}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb230}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb231}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb232}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb233}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb234}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb235}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb236}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb237}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb238}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb239}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb23a}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb23b}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb23c}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb23d}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb23e}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb23f}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb240}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb241}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb242}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb243}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb244}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb245}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb246}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb247}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb248}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb249}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb24a}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb24b}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb24c}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb24d}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb24e}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_wpn" "hidden" 0xb24f}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb250}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb251}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb252}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb253}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb254}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb255}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb256}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb257}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb258}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb259}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb25a}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb25b}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb25c}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb25d}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb25e}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb25f}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb260}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb261}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb262}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb263}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb264}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb265}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb266}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb267}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb268}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb269}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb26a}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb26b}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb26c}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb26d}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb26e}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb26f}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb270}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb271}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb272}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb273}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb274}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb275}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb276}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_wpn" "hidden" 0xb277}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb278}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb279}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb27a}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb27b}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb27c}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb27d}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb27e}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb27f}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb280}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb281}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb282}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb283}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb284}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb285}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb286}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb287}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb288}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb289}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb28a}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb28b}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb28c}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb28d}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb28e}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb28f}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb290}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb291}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb292}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb293}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb294}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb295}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb296}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb297}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb298}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb299}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb29a}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb29b}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb29c}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb29d}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb29e}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_wpn" "hidden" 0xb29f}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2a0}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2a1}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2a2}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2a3}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2a4}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2a5}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2a6}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2a7}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2a8}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2a9}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2aa}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2ab}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2ac}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2ad}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_recon" "hidden" 0xb2ae}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2af}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2b0}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2b1}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2b2}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2b3}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2b4}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2b5}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2b6}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2b7}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2b8}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2b9}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2ba}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2bb}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2bc}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2bd}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_assault" "hidden" 0xb2be}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_eng" "hidden" 0xb2bf}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_eng" "hidden" 0xb2c0}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_eng" "hidden" 0xb2c1}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_eng" "hidden" 0xb2c2}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_eng" "hidden" 0xb2c3}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_eng" "hidden" 0xb2c4}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_eng" "hidden" 0xb2c5}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_eng" "hidden" 0xb2c6}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_eng" "hidden" 0xb2c7}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_eng" "hidden" 0xb2c8}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_eng" "hidden" 0xb2c9}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_eng" "hidden" 0xb2ca}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_manpad" "hidden" 0xb2cb}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_manpad" "hidden" 0xb2cc}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_manpad" "hidden" 0xb2cd}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_manpad" "hidden" 0xb2ce}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_manpad" "hidden" 0xb2cf}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_manpad" "hidden" 0xb2d0}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_manpad" "hidden" 0xb2d1}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_manpad" "hidden" 0xb2d2}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2d3}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2d4}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2d5}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2d6}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2d7}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2d8}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2d9}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2da}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2db}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2dc}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2dd}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2de}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2df}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2e0}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_recon" "hidden" 0xb2e1}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2e2}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2e3}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2e4}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2e5}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2e6}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2e7}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2e8}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2e9}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2ea}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2eb}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2ec}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2ed}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2ee}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2ef}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2f0}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_assault" "hidden" 0xb2f1}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_eng" "hidden" 0xb2f2}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_eng" "hidden" 0xb2f3}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_eng" "hidden" 0xb2f4}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_eng" "hidden" 0xb2f5}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_eng" "hidden" 0xb2f6}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_eng" "hidden" 0xb2f7}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_eng" "hidden" 0xb2f8}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_eng" "hidden" 0xb2f9}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_eng" "hidden" 0xb2fa}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_eng" "hidden" 0xb2fb}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_eng" "hidden" 0xb2fc}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_eng" "hidden" 0xb2fd}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_manpad" "hidden" 0xb2fe}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_manpad" "hidden" 0xb2ff}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_manpad" "hidden" 0xb300}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_manpad" "hidden" 0xb301}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_manpad" "hidden" 0xb302}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_manpad" "hidden" 0xb303}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_manpad" "hidden" 0xb304}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_manpad" "hidden" 0xb305}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh1" "hidden" 0xb306}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh1" "hidden" 0xb307}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh1" "hidden" 0xb308}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh2" "hidden" 0xb309}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh2" "hidden" 0xb30a}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh2" "hidden" 0xb30b}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh3" "hidden" 0xb30c}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh3" "hidden" 0xb30d}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh3" "hidden" 0xb30e}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb30f}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb310}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb311}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb312}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb313}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb314}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb315}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb316}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb317}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb318}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb319}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb31a}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb31b}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb31c}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_recon" "hidden" 0xb31d}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb31e}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb31f}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb320}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb321}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb322}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb323}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb324}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb325}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb326}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb327}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb328}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb329}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb32a}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb32b}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb32c}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_assault" "hidden" 0xb32d}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_eng" "hidden" 0xb32e}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_eng" "hidden" 0xb32f}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_eng" "hidden" 0xb330}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_eng" "hidden" 0xb331}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_eng" "hidden" 0xb332}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_eng" "hidden" 0xb333}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_eng" "hidden" 0xb334}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_eng" "hidden" 0xb335}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_eng" "hidden" 0xb336}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_eng" "hidden" 0xb337}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_eng" "hidden" 0xb338}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_eng" "hidden" 0xb339}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_manpad" "hidden" 0xb33a}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_manpad" "hidden" 0xb33b}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_manpad" "hidden" 0xb33c}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_manpad" "hidden" 0xb33d}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_manpad" "hidden" 0xb33e}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_manpad" "hidden" 0xb33f}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_manpad" "hidden" 0xb340}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_manpad" "hidden" 0xb341}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb342}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb343}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb344}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb345}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb346}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb347}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb348}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb349}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb34a}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb34b}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb34c}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb34d}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb34e}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb34f}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_recon" "hidden" 0xb350}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb351}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb352}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb353}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb354}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb355}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb356}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb357}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb358}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb359}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb35a}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb35b}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb35c}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb35d}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb35e}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb35f}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_assault" "hidden" 0xb360}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_eng" "hidden" 0xb361}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_eng" "hidden" 0xb362}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_eng" "hidden" 0xb363}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_eng" "hidden" 0xb364}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_eng" "hidden" 0xb365}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_eng" "hidden" 0xb366}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_eng" "hidden" 0xb367}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_eng" "hidden" 0xb368}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_eng" "hidden" 0xb369}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_eng" "hidden" 0xb36a}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_eng" "hidden" 0xb36b}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_eng" "hidden" 0xb36c}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_manpad" "hidden" 0xb36d}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_manpad" "hidden" 0xb36e}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_manpad" "hidden" 0xb36f}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_manpad" "hidden" 0xb370}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_manpad" "hidden" 0xb371}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_manpad" "hidden" 0xb372}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_manpad" "hidden" 0xb373}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_manpad" "hidden" 0xb374}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_veh" "ally_sup_nato_veh1" "hidden" 0xb375}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_veh" "ally_sup_nato_veh1" "hidden" 0xb376}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_veh" "ally_sup_nato_veh1" "hidden" 0xb377}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_veh" "ally_sup_nato_veh2" "hidden" 0xb378}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_veh" "ally_sup_nato_veh2" "hidden" 0xb379}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_veh" "ally_sup_nato_veh2" "hidden" 0xb37a}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_ifv" "ally_sup_rusa_ifv1" "hidden" 0xb380}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_ifv" "ally_sup_rusa_ifv1" "hidden" 0xb381}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_ifv" "ally_sup_rusa_ifv1" "hidden" 0xb382}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_ifv" "ally_sup_rusa_ifv1" "hidden" 0xb383}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_ifv_inf" "hidden" 0xb384}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_ifv_inf" "hidden" 0xb385}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_ifv_inf" "hidden" 0xb386}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_ifv_inf" "hidden" 0xb387}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_ifv" "ally_sup_ukr_ifv1" "hidden" 0xb388}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_ifv" "ally_sup_ukr_ifv1" "hidden" 0xb389}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_ifv" "ally_sup_ukr_ifv1" "hidden" 0xb38a}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_ifv" "ally_sup_ukr_ifv1" "hidden" 0xb38b}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_ifv_inf" "hidden" 0xb38c}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_ifv_inf" "hidden" 0xb38d}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_ifv_inf" "hidden" 0xb38e}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_ifv_inf" "hidden" 0xb38f}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_ifv" "ally_sup_prc_ifv1" "hidden" 0xb390}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_ifv" "ally_sup_prc_ifv1" "hidden" 0xb391}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_ifv" "ally_sup_prc_ifv1" "hidden" 0xb392}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_ifv" "ally_sup_prc_ifv1" "hidden" 0xb393}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_ifv_inf" "hidden" 0xb394}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_ifv_inf" "hidden" 0xb395}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_ifv_inf" "hidden" 0xb396}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_ifv_inf" "hidden" 0xb397}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_ifv" "ally_sup_nato_ifv1" "hidden" 0xb398}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_ifv" "ally_sup_nato_ifv1" "hidden" 0xb399}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_ifv" "ally_sup_nato_ifv1" "hidden" 0xb39a}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_ifv" "ally_sup_nato_ifv1" "hidden" 0xb39b}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_ifv_inf" "hidden" 0xb39c}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_ifv_inf" "hidden" 0xb39d}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_ifv_inf" "hidden" 0xb39e}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_ifv_inf" "hidden" 0xb39f}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_hull" "hidden" 0xb3a0}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_crew" "hidden" 0xb3a1}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_crew" "hidden" 0xb3a2}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xb3a3}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xb3a4}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xb3a5}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xb3a6}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xb3a7}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xb3a8}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xb3a9}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xb3aa}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_hull" "hidden" 0xb3ab}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_crew" "hidden" 0xb3ac}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_crew" "hidden" 0xb3ad}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xb3ae}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xb3af}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xb3b0}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xb3b1}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xb3b2}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xb3b3}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xb3b4}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xb3b5}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_hull" "hidden" 0xb3b6}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_crew" "hidden" 0xb3b7}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_crew" "hidden" 0xb3b8}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xb3b9}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xb3ba}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xb3bb}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xb3bc}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xb3bd}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xb3be}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xb3bf}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xb3c0}

	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_hull" "hidden" 0xb3c1}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_crew" "hidden" 0xb3c2}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_crew" "hidden" 0xb3c3}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xb3c4}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xb3c5}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xb3c6}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xb3c7}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xb3c8}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xb3c9}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xb3ca}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xb3cb}

	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_helo" "support_e2_aircraft" "hidden" 0xb401}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_helo_crew" "hidden" 0xb402}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_helo_crew" "hidden" 0xb403}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_helo_team" "hidden" 0xb404}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_helo_team" "hidden" 0xb405}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_helo_team" "hidden" 0xb406}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_helo_team" "hidden" 0xb407}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_helo" "support_e2_aircraft" "hidden" 0xb408}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_helo_crew" "hidden" 0xb409}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_helo_crew" "hidden" 0xb40a}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_helo_team" "hidden" 0xb40b}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_helo_team" "hidden" 0xb40c}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_helo_team" "hidden" 0xb40d}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_helo_team" "hidden" 0xb40e}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_helo" "support_e2_aircraft" "hidden" 0xb40f}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_helo_crew" "hidden" 0xb410}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_helo_crew" "hidden" 0xb411}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_helo_team" "hidden" 0xb412}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_helo_team" "hidden" 0xb413}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_helo_team" "hidden" 0xb414}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_helo_team" "hidden" 0xb415}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_para" "support_e2_aircraft" "hidden" 0xb416}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_para_crew" "hidden" 0xb417}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_para_crew" "hidden" 0xb418}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_para_crew" "hidden" 0xb419}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_para_crew" "hidden" 0xb41a}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_para_crew" "hidden" 0xb41b}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_para_pax" "support_e2_para_pax" "hidden" 0xb41c}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_para_pax" "support_e2_para_pax" "hidden" 0xb41d}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_para_pax" "support_e2_para_pax" "hidden" 0xb41e}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_rusa_para_pax" "support_e2_para_pax" "hidden" 0xb41f}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_para" "support_e2_aircraft" "hidden" 0xb420}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_para_crew" "hidden" 0xb421}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_para_crew" "hidden" 0xb422}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_para_crew" "hidden" 0xb423}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_para_pax" "support_e2_para_pax" "hidden" 0xb424}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_para_pax" "support_e2_para_pax" "hidden" 0xb425}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_para_pax" "support_e2_para_pax" "hidden" 0xb426}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_ukr_para_pax" "support_e2_para_pax" "hidden" 0xb427}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_para" "support_e2_aircraft" "hidden" 0xb428}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_para_crew" "hidden" 0xb429}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_para_crew" "hidden" 0xb42a}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_para_crew" "hidden" 0xb42b}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_para_pax" "support_e2_para_pax" "hidden" 0xb42c}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_para_pax" "support_e2_para_pax" "hidden" 0xb42d}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_para_pax" "support_e2_para_pax" "hidden" 0xb42e}
	{Tags "ally_sup_tpl" "support_e2_tpl" "support_e2_nato_para_pax" "support_e2_para_pax" "hidden" 0xb42f}

; ===== FLAG-WEAPON CREW TOP-UP (Phase 4 crewed rewrite, 2026-07-30) =====
; defense_support_waves.inc crews each flag emplacement with two bodies from this
; faction's line pool at garrison time. Six extra draws per mission, so the line
; pool is deepened by exactly six per faction to fund them: the wave budget below
; is unchanged. 24 -> 30 per faction, 91 -> 97 overall.
; Ids 0xb430-0xb447, MIDs 9847-9870, parked y=-36900: all three bands swept clear
; against every .inc and .mi in resource/map/multi.
; Pairs are gunner + loader by breed (mg + rifleman) because the NATO emplacement
; is bgm71_tow_ai, whose Weaponry names "gunner" as gunner and "commander" as
; charger - a one-man TOW fires its loaded round and can never reload.
; ===== RUSA FLAG-WEAPON CREW TOP-UP (6 = 3 x 2) =====
	{Human "mp/rusa/2022s/rus90_mg" 0xb430
		{Position -9000 -36900}
		{Player 0}
		{MID 9847}
		{Able "-select"}
	}

	{Human "mp/rusa/2022s/rus90_rifleman" 0xb431
		{Position -8980 -36900}
		{Player 0}
		{MID 9848}
		{Able "-select"}
	}

	{Human "mp/rusa/2022s/rus90_mg" 0xb432
		{Position -8960 -36900}
		{Player 0}
		{MID 9849}
		{Able "-select"}
	}

	{Human "mp/rusa/2022s/rus90_rifleman" 0xb433
		{Position -8940 -36900}
		{Player 0}
		{MID 9850}
		{Able "-select"}
	}

	{Human "mp/rusa/2022s/rus90_mg" 0xb434
		{Position -8920 -36900}
		{Player 0}
		{MID 9851}
		{Able "-select"}
	}

	{Human "mp/rusa/2022s/rus90_rifleman" 0xb435
		{Position -8900 -36900}
		{Player 0}
		{MID 9852}
		{Able "-select"}
	}

; ===== UKR FLAG-WEAPON CREW TOP-UP (6 = 3 x 2) =====
	{Human "mp/ukr/2022s/ter_mg" 0xb436
		{Position -8880 -36900}
		{Player 0}
		{MID 9853}
		{Able "-select"}
	}

	{Human "mp/ukr/2022s/ter_rifleman" 0xb437
		{Position -8860 -36900}
		{Player 0}
		{MID 9854}
		{Able "-select"}
	}

	{Human "mp/ukr/2022s/ter_mg" 0xb438
		{Position -8840 -36900}
		{Player 0}
		{MID 9855}
		{Able "-select"}
	}

	{Human "mp/ukr/2022s/ter_rifleman" 0xb439
		{Position -8820 -36900}
		{Player 0}
		{MID 9856}
		{Able "-select"}
	}

	{Human "mp/ukr/2022s/ter_mg" 0xb43a
		{Position -8800 -36900}
		{Player 0}
		{MID 9857}
		{Able "-select"}
	}

	{Human "mp/ukr/2022s/ter_rifleman" 0xb43b
		{Position -8780 -36900}
		{Player 0}
		{MID 9858}
		{Able "-select"}
	}

; ===== PRC FLAG-WEAPON CREW TOP-UP (6 = 3 x 2) =====
	{Human "mp/prc/2022s/pla_mg" 0xb43c
		{Position -8760 -36900}
		{Player 0}
		{MID 9859}
		{Able "-select"}
	}

	{Human "mp/prc/2022s/pla_rifleman" 0xb43d
		{Position -8740 -36900}
		{Player 0}
		{MID 9860}
		{Able "-select"}
	}

	{Human "mp/prc/2022s/pla_mg" 0xb43e
		{Position -8720 -36900}
		{Player 0}
		{MID 9861}
		{Able "-select"}
	}

	{Human "mp/prc/2022s/pla_rifleman" 0xb43f
		{Position -8700 -36900}
		{Player 0}
		{MID 9862}
		{Able "-select"}
	}

	{Human "mp/prc/2022s/pla_mg" 0xb440
		{Position -8680 -36900}
		{Player 0}
		{MID 9863}
		{Able "-select"}
	}

	{Human "mp/prc/2022s/pla_rifleman" 0xb441
		{Position -8660 -36900}
		{Player 0}
		{MID 9864}
		{Able "-select"}
	}

; ===== NATO FLAG-WEAPON CREW TOP-UP (6 = 3 x 2) =====
	{Human "mp/nato/2022s/nato_mg" 0xb442
		{Position -8640 -36900}
		{Player 0}
		{MID 9865}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_rifleman" 0xb443
		{Position -8620 -36900}
		{Player 0}
		{MID 9866}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_mg" 0xb444
		{Position -8600 -36900}
		{Player 0}
		{MID 9867}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_rifleman" 0xb445
		{Position -8580 -36900}
		{Player 0}
		{MID 9868}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_mg" 0xb446
		{Position -8560 -36900}
		{Player 0}
		{MID 9869}
		{Able "-select"}
	}

	{Human "mp/nato/2022s/nato_rifleman" 0xb447
		{Position -8540 -36900}
		{Player 0}
		{MID 9870}
		{Able "-select"}
	}

; ===== FLAG-WEAPON CREW TOP-UP TAGS =====
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb430}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb431}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb432}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb433}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb434}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_line" "hidden" 0xb435}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb436}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb437}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb438}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb439}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb43a}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_line" "hidden" 0xb43b}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb43c}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb43d}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb43e}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb43f}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb440}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_line" "hidden" 0xb441}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb442}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb443}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb444}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb445}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb446}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_line" "hidden" 0xb447}

; ----- MOTOR POOL TOP-UP: sustains four trucks per mission per engine -----
; A truck claims 1 hull + 2 crew + 6 pax, so each faction parks 4 / 8 / 24.
; Ids 0xc100-0xc163, MIDs 9900-9999, y=-37200. Swept against every pool and base map.
	{Entity "ural" 0xc100
		{Position -9000 -37200}
		{Player 0}
		{MID 9900}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xc101
		{Position -8995 -37200}
		{Player 0}
		{MID 9901}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xc102
		{Position -8994 -37200}
		{Player 0}
		{MID 9902}
		{Able "-select"}
	}
	{Entity "ural" 0xc103
		{Position -8960 -37200}
		{Player 0}
		{MID 9903}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xc104
		{Position -8955 -37200}
		{Player 0}
		{MID 9904}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xc105
		{Position -8954 -37200}
		{Player 0}
		{MID 9905}
		{Able "-select"}
	}
	{Entity "ural" 0xc106
		{Position -8920 -37200}
		{Player 0}
		{MID 9906}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xc107
		{Position -8915 -37200}
		{Player 0}
		{MID 9907}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xc108
		{Position -8914 -37200}
		{Player 0}
		{MID 9908}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc109
		{Position -8800 -37200}
		{Player 0}
		{MID 9909}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc10a
		{Position -8796 -37200}
		{Player 0}
		{MID 9910}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc10b
		{Position -8792 -37200}
		{Player 0}
		{MID 9911}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc10c
		{Position -8788 -37200}
		{Player 0}
		{MID 9912}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc10d
		{Position -8784 -37200}
		{Player 0}
		{MID 9913}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc10e
		{Position -8780 -37200}
		{Player 0}
		{MID 9914}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc10f
		{Position -8776 -37200}
		{Player 0}
		{MID 9915}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc110
		{Position -8772 -37200}
		{Player 0}
		{MID 9916}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc111
		{Position -8768 -37200}
		{Player 0}
		{MID 9917}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc112
		{Position -8764 -37200}
		{Player 0}
		{MID 9918}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc113
		{Position -8760 -37200}
		{Player 0}
		{MID 9919}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc114
		{Position -8756 -37200}
		{Player 0}
		{MID 9920}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc115
		{Position -8752 -37200}
		{Player 0}
		{MID 9921}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc116
		{Position -8748 -37200}
		{Player 0}
		{MID 9922}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc117
		{Position -8744 -37200}
		{Player 0}
		{MID 9923}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus90_rifleman" 0xc118
		{Position -8740 -37200}
		{Player 0}
		{MID 9924}
		{Able "-select"}
	}
	{Entity "ural_vsu" 0xc119
		{Position -8100 -37200}
		{Player 0}
		{MID 9925}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xc11a
		{Position -8095 -37200}
		{Player 0}
		{MID 9926}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xc11b
		{Position -8094 -37200}
		{Player 0}
		{MID 9927}
		{Able "-select"}
	}
	{Entity "ural_vsu" 0xc11c
		{Position -8060 -37200}
		{Player 0}
		{MID 9928}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xc11d
		{Position -8055 -37200}
		{Player 0}
		{MID 9929}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xc11e
		{Position -8054 -37200}
		{Player 0}
		{MID 9930}
		{Able "-select"}
	}
	{Entity "ural_vsu" 0xc11f
		{Position -8020 -37200}
		{Player 0}
		{MID 9931}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xc120
		{Position -8015 -37200}
		{Player 0}
		{MID 9932}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ukr_vehicleman" 0xc121
		{Position -8014 -37200}
		{Player 0}
		{MID 9933}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc122
		{Position -7900 -37200}
		{Player 0}
		{MID 9934}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc123
		{Position -7896 -37200}
		{Player 0}
		{MID 9935}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc124
		{Position -7892 -37200}
		{Player 0}
		{MID 9936}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc125
		{Position -7888 -37200}
		{Player 0}
		{MID 9937}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc126
		{Position -7884 -37200}
		{Player 0}
		{MID 9938}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc127
		{Position -7880 -37200}
		{Player 0}
		{MID 9939}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc128
		{Position -7876 -37200}
		{Player 0}
		{MID 9940}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc129
		{Position -7872 -37200}
		{Player 0}
		{MID 9941}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc12a
		{Position -7868 -37200}
		{Player 0}
		{MID 9942}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc12b
		{Position -7864 -37200}
		{Player 0}
		{MID 9943}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc12c
		{Position -7860 -37200}
		{Player 0}
		{MID 9944}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc12d
		{Position -7856 -37200}
		{Player 0}
		{MID 9945}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc12e
		{Position -7852 -37200}
		{Player 0}
		{MID 9946}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc12f
		{Position -7848 -37200}
		{Player 0}
		{MID 9947}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc130
		{Position -7844 -37200}
		{Player 0}
		{MID 9948}
		{Able "-select"}
	}
	{Human "mp/ukr/2022s/ter_rifleman" 0xc131
		{Position -7840 -37200}
		{Player 0}
		{MID 9949}
		{Able "-select"}
	}
	{Entity "fmtv" 0xc132
		{Position -7200 -37200}
		{Player 0}
		{MID 9950}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xc133
		{Position -7195 -37200}
		{Player 0}
		{MID 9951}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xc134
		{Position -7194 -37200}
		{Player 0}
		{MID 9952}
		{Able "-select"}
	}
	{Entity "fmtv" 0xc135
		{Position -7160 -37200}
		{Player 0}
		{MID 9953}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xc136
		{Position -7155 -37200}
		{Player 0}
		{MID 9954}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xc137
		{Position -7154 -37200}
		{Player 0}
		{MID 9955}
		{Able "-select"}
	}
	{Entity "fmtv" 0xc138
		{Position -7120 -37200}
		{Player 0}
		{MID 9956}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xc139
		{Position -7115 -37200}
		{Player 0}
		{MID 9957}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/usarmy_crew" 0xc13a
		{Position -7114 -37200}
		{Player 0}
		{MID 9958}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc13b
		{Position -7000 -37200}
		{Player 0}
		{MID 9959}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc13c
		{Position -6996 -37200}
		{Player 0}
		{MID 9960}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc13d
		{Position -6992 -37200}
		{Player 0}
		{MID 9961}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc13e
		{Position -6988 -37200}
		{Player 0}
		{MID 9962}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc13f
		{Position -6984 -37200}
		{Player 0}
		{MID 9963}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc140
		{Position -6980 -37200}
		{Player 0}
		{MID 9964}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc141
		{Position -6976 -37200}
		{Player 0}
		{MID 9965}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc142
		{Position -6972 -37200}
		{Player 0}
		{MID 9966}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc143
		{Position -6968 -37200}
		{Player 0}
		{MID 9967}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc144
		{Position -6964 -37200}
		{Player 0}
		{MID 9968}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc145
		{Position -6960 -37200}
		{Player 0}
		{MID 9969}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc146
		{Position -6956 -37200}
		{Player 0}
		{MID 9970}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc147
		{Position -6952 -37200}
		{Player 0}
		{MID 9971}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc148
		{Position -6948 -37200}
		{Player 0}
		{MID 9972}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc149
		{Position -6944 -37200}
		{Player 0}
		{MID 9973}
		{Able "-select"}
	}
	{Human "mp/nato/2022s/nato_rifleman" 0xc14a
		{Position -6940 -37200}
		{Player 0}
		{MID 9974}
		{Able "-select"}
	}
	{Entity "shaanxi_sx2190_passenger" 0xc14b
		{Position -6300 -37200}
		{Player 0}
		{MID 9975}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_crew" 0xc14c
		{Position -6295 -37200}
		{Player 0}
		{MID 9976}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_crew" 0xc14d
		{Position -6294 -37200}
		{Player 0}
		{MID 9977}
		{Able "-select"}
	}
	{Entity "shaanxi_sx2190_passenger" 0xc14e
		{Position -6260 -37200}
		{Player 0}
		{MID 9978}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_crew" 0xc14f
		{Position -6255 -37200}
		{Player 0}
		{MID 9979}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_crew" 0xc150
		{Position -6254 -37200}
		{Player 0}
		{MID 9980}
		{Able "-select"}
	}
	{Entity "shaanxi_sx2190_passenger" 0xc151
		{Position -6220 -37200}
		{Player 0}
		{MID 9981}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_crew" 0xc152
		{Position -6215 -37200}
		{Player 0}
		{MID 9982}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_crew" 0xc153
		{Position -6214 -37200}
		{Player 0}
		{MID 9983}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc154
		{Position -6100 -37200}
		{Player 0}
		{MID 9984}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc155
		{Position -6096 -37200}
		{Player 0}
		{MID 9985}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc156
		{Position -6092 -37200}
		{Player 0}
		{MID 9986}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc157
		{Position -6088 -37200}
		{Player 0}
		{MID 9987}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc158
		{Position -6084 -37200}
		{Player 0}
		{MID 9988}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc159
		{Position -6080 -37200}
		{Player 0}
		{MID 9989}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc15a
		{Position -6076 -37200}
		{Player 0}
		{MID 9990}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc15b
		{Position -6072 -37200}
		{Player 0}
		{MID 9991}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc15c
		{Position -6068 -37200}
		{Player 0}
		{MID 9992}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc15d
		{Position -6064 -37200}
		{Player 0}
		{MID 9993}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc15e
		{Position -6060 -37200}
		{Player 0}
		{MID 9994}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc15f
		{Position -6056 -37200}
		{Player 0}
		{MID 9995}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc160
		{Position -6052 -37200}
		{Player 0}
		{MID 9996}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc161
		{Position -6048 -37200}
		{Player 0}
		{MID 9997}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc162
		{Position -6044 -37200}
		{Player 0}
		{MID 9998}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_rifleman" 0xc163
		{Position -6040 -37200}
		{Player 0}
		{MID 9999}
		{Able "-select"}
	}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_hull" "hidden" 0xc100}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_crew" "hidden" 0xc101}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_crew" "hidden" 0xc102}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_hull" "hidden" 0xc103}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_crew" "hidden" 0xc104}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_crew" "hidden" 0xc105}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_hull" "hidden" 0xc106}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_crew" "hidden" 0xc107}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_crew" "hidden" 0xc108}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc109}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc10a}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc10b}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc10c}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc10d}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc10e}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc10f}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc110}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc111}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc112}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc113}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc114}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc115}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc116}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc117}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_motor" "ally_sup_rusa_motor_pax" "hidden" 0xc118}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_hull" "hidden" 0xc119}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_crew" "hidden" 0xc11a}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_crew" "hidden" 0xc11b}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_hull" "hidden" 0xc11c}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_crew" "hidden" 0xc11d}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_crew" "hidden" 0xc11e}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_hull" "hidden" 0xc11f}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_crew" "hidden" 0xc120}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_crew" "hidden" 0xc121}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc122}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc123}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc124}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc125}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc126}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc127}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc128}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc129}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc12a}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc12b}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc12c}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc12d}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc12e}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc12f}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc130}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_motor" "ally_sup_ukr_motor_pax" "hidden" 0xc131}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_hull" "hidden" 0xc132}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_crew" "hidden" 0xc133}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_crew" "hidden" 0xc134}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_hull" "hidden" 0xc135}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_crew" "hidden" 0xc136}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_crew" "hidden" 0xc137}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_hull" "hidden" 0xc138}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_crew" "hidden" 0xc139}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_crew" "hidden" 0xc13a}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc13b}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc13c}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc13d}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc13e}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc13f}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc140}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc141}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc142}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc143}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc144}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc145}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc146}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc147}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc148}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc149}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_motor" "ally_sup_nato_motor_pax" "hidden" 0xc14a}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_hull" "hidden" 0xc14b}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_crew" "hidden" 0xc14c}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_crew" "hidden" 0xc14d}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_hull" "hidden" 0xc14e}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_crew" "hidden" 0xc14f}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_crew" "hidden" 0xc150}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_hull" "hidden" 0xc151}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_crew" "hidden" 0xc152}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_crew" "hidden" 0xc153}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc154}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc155}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc156}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc157}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc158}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc159}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc15a}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc15b}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc15c}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc15d}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc15e}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc15f}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc160}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc161}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc162}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_motor" "ally_sup_prc_motor_pax" "hidden" 0xc163}
	{Link 0xc101 {0xc100 "driver"}}
	{Link 0xc102 {0xc100 "commander"}}
	{Link 0xc104 {0xc103 "driver"}}
	{Link 0xc105 {0xc103 "commander"}}
	{Link 0xc107 {0xc106 "driver"}}
	{Link 0xc108 {0xc106 "commander"}}
	{Link 0xc11a {0xc119 "driver"}}
	{Link 0xc11b {0xc119 "commander"}}
	{Link 0xc11d {0xc11c "driver"}}
	{Link 0xc11e {0xc11c "commander"}}
	{Link 0xc120 {0xc11f "driver"}}
	{Link 0xc121 {0xc11f "commander"}}
	{Link 0xc133 {0xc132 "driver"}}
	{Link 0xc134 {0xc132 "commander"}}
	{Link 0xc136 {0xc135 "driver"}}
	{Link 0xc137 {0xc135 "commander"}}
	{Link 0xc139 {0xc138 "driver"}}
	{Link 0xc13a {0xc138 "commander"}}
	{Link 0xc14c {0xc14b "driver"}}
	{Link 0xc14d {0xc14b "commander"}}
	{Link 0xc14f {0xc14e "driver"}}
	{Link 0xc150 {0xc14e "commander"}}
	{Link 0xc152 {0xc151 "driver"}}
	{Link 0xc153 {0xc151 "commander"}}
