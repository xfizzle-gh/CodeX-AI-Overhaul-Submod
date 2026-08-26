; Friendly Allied Support multi-faction hybrid pools (player nation).
; Folded from user_nation$ the same way enemy uses bot_army$:
;   1 rusa (+sov/pol)  2 ukr  3 nato (+csa/frg)  4 prc
; Parked L1/L2 only: 20 line (5x4) per faction + UKR/NATO light cars.
; L1 deploys 4-man line. L2 deploys car+6 line (UKR/NATO); RUSA/PRC 6 line.
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

; ===== ALLY RUSA WPN (16=4x4) =====

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

; ===== ALLY UKR WPN (16=4x4) =====

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

; ===== ALLY PRC WPN (16=4x4) =====

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

; ===== ALLY NATO WPN (16=4x4) =====

; ===== HYBRID POOLS (recon/assault/eng/manpad/light veh) =====
; Ids 0xb2a0+, MIDs 9460+. y=-36000/-36200 clear of line/wpn at -35700.
; Depth: recon 5x3, assault 4x4, eng 4x3, manpad 4x2; UKR 3 humvee, NATO 2 fennek.

; ----- ALLY RUSA RECON (15=5x3) -----

; ----- ALLY RUSA ASSAULT (16=4x4) -----

; ----- ALLY RUSA ENG (12=4x3) -----

; ----- ALLY RUSA MANPAD (8=4x2) -----

; ----- ALLY UKR RECON (15=5x3) -----

; ----- ALLY UKR ASSAULT (16=4x4) -----

; ----- ALLY UKR ENG (12=4x3) -----

; ----- ALLY UKR MANPAD (8=4x2) -----

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

; ----- ALLY PRC ASSAULT (16=4x4) -----

; ----- ALLY PRC ENG (12=4x3) -----

; ----- ALLY PRC MANPAD (8=4x2) -----

; ----- ALLY NATO RECON (15=5x3) -----

; ----- ALLY NATO ASSAULT (16=4x4) -----

; ----- ALLY NATO ENG (12=4x3) -----

; ----- ALLY NATO MANPAD (8=4x2) -----

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

; ----- ALLY RUSA LIGHT VEH (1 hull+crew) -----
	{Entity "gaz2975a_nsvt" 0xb2cb
		{Position -6800 -36200}
		{Player 0}
		{MID 9503}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xb2cc
		{Position -6795 -36200}
		{Player 0}
		{MID 9504}
		{Able "-select"}
	}
	{Human "mp/rusa/2022s/rus_vehicleman" 0xb2cd
		{Position -6790 -36200}
		{Player 0}
		{MID 9505}
		{Able "-select"}
	}

; ----- ALLY PRC LIGHT VEH (1 hull+crew) -----
	{Entity "dongfeng__mengshi_sk131" 0xb33a
		{Position -2000 -36200}
		{Player 0}
		{MID 9610}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_vehicleman" 0xb33b
		{Position -1995 -36200}
		{Player 0}
		{MID 9611}
		{Able "-select"}
	}
	{Human "mp/prc/2022s/pla_vehicleman" 0xb33c
		{Position -1990 -36200}
		{Player 0}
		{MID 9612}
		{Able "-select"}
	}

; ----- VEHICLE CREW LINKS -----
	{Link 0xb2cc {0xb2cb "driver"}}
	{Link 0xb2cd {0xb2cb "gunner2"}}
	{Link 0xb33b {0xb33a "driver"}}
	{Link 0xb33c {0xb33a "gunner2"}}
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

; ----- ALLY UKR IFV -----

; ----- ALLY PRC IFV -----

; ----- ALLY NATO IFV -----

; ----- IFV CREW LINKS -----

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

; ----- ALLY UKR MOTORIZED (ural_vsu) -----

; ----- ALLY NATO MOTORIZED (fmtv) -----


; ----- ALLY PRC MOTORIZED (shaanxi_sx2190_passenger) -----

; ----- MOTORIZED CREW + PASSENGER LINKS -----

; BEGIN ENEMY NORMAL TRANSPORT PACKAGES
; ----- ENEMY NORMAL RUSA TRANSPORT -----
; ----- ENEMY NORMAL UKR TRANSPORT -----
; ----- ENEMY NORMAL NATO TRANSPORT -----
; ----- ENEMY NORMAL PRC TRANSPORT -----
; END ENEMY NORMAL TRANSPORT PACKAGES

; ===== E2 AIR PACKAGE POOLS (default-off; West-81 supplies UH-60/C-130) =====
; IDs 0xb401-0xb42f, MIDs 9800-9846, y=-36800.
	}
	}
	}

; ----- E2 CREW + PARATROOPER LINKS -----

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
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_veh" "ally_sup_rusa_veh1" "hidden" 0xb2cb}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_veh" "ally_sup_rusa_veh1" "hidden" 0xb2cc}
	{Tags "ally_sup_tpl" "ally_sup_rusa" "ally_sup_rusa_veh" "ally_sup_rusa_veh1" "hidden" 0xb2cd}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_veh" "ally_sup_prc_veh1" "hidden" 0xb33a}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_veh" "ally_sup_prc_veh1" "hidden" 0xb33b}
	{Tags "ally_sup_tpl" "ally_sup_prc" "ally_sup_prc_veh" "ally_sup_prc_veh1" "hidden" 0xb33c}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh1" "hidden" 0xb306}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh1" "hidden" 0xb307}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh1" "hidden" 0xb308}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh2" "hidden" 0xb309}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh2" "hidden" 0xb30a}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh2" "hidden" 0xb30b}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh3" "hidden" 0xb30c}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh3" "hidden" 0xb30d}
	{Tags "ally_sup_tpl" "ally_sup_ukr" "ally_sup_ukr_veh" "ally_sup_ukr_veh3" "hidden" 0xb30e}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_veh" "ally_sup_nato_veh1" "hidden" 0xb375}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_veh" "ally_sup_nato_veh1" "hidden" 0xb376}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_veh" "ally_sup_nato_veh1" "hidden" 0xb377}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_veh" "ally_sup_nato_veh2" "hidden" 0xb378}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_veh" "ally_sup_nato_veh2" "hidden" 0xb379}
	{Tags "ally_sup_tpl" "ally_sup_nato" "ally_sup_nato_veh" "ally_sup_nato_veh2" "hidden" 0xb37a}


; BEGIN ENEMY NORMAL TRANSPORT TAGS
; END ENEMY NORMAL TRANSPORT TAGS

