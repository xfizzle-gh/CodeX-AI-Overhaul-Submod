;/-----------ordos++++-------

(define "distance_1"
	{spreading
		{radiusTable {10 0} {25 %r1} {50 %r2} {75 %r3} {100 %r4} {500 (* 5.00 %r4)}}
	}
)

(define "CW_acc_common"
("CW_XP_acc_common" mils( %mils))
)
(define "CW_acc_artillery"
("CW_XP_acc_artillery" mils( %mils))
)
(define "CW_acc_heli"
("CW_XP_acc_heli" mils( %mils 5))
)
(define "CW_acc_guided"
("CW_XP_acc_guided" kvo_meters( %kvo_meters))
)

(define "accuracy_open_rpg"
	{spreading
		{radiusTable
			{0 0.01}
			{20 (* 0.02 %moa)}
			{30 (* 0.07 %moa)}
			{50 (* 0.14 %moa)}
			{65 (* 0.24 %moa)}
			{75 (* 0.43 %moa)}
			{90 (* 0.66 %moa)}
			{100 (* 0.84 %moa)}
			{110 (* 1.27 %moa)}
			{120 (* 1.33 %moa)}
		}
		{BurstAccuracy 100 97 94 89 84 79 74 70 65 60}
		{SpreadPower	1.7}
		{SpreadXYRatio	1.0}
	}
)

(define "accuracy_optic_sight_at_rifle" 
	{spreading 
		{radiusTable 
			{0 0.01} 
			{20 (* 0.3 0.07 %moa)} 
			{50 (* 0.3 0.15 %moa)} 
			{70 (* 0.3 0.29 %moa)} 
			{75 (* 0.3 0.36 %moa)} 
			{100 (* 0.3 0.7 %moa)} 
			{105 (* 0.3 0.88 %moa)} 
			{110 (* 0.3 1.05 %moa)} 
			{115 (* 0.3 1.26 %moa)} 
			{120 (* 0.3 1.39 %moa)}
			{130 (* 0.3 1.75 %moa)}
			{140 (* 0.3 2.09 %moa)}
			{150 (* 0.3 2.8 %moa)}
			{175 (* 0.3 4.2 %moa)}
		}
		{BurstAccuracy 100 96 92 86 80 74 68 62 56 50}
		{SpreadPower	1.7}
		{SpreadXYRatio	1.0}
		{BurstRecoveryTime 1.5}
	}
)

(define "relaxationtime"
(mod "mp"
	{tag "prepare"}
	{Special
		{Mode aim}
		{PrepareTime 0}
		{PrepareCursor "aimed_shot_aa"}
	}
	{RelaxationTime %rt}
)
)
(define "CW_XP_acc_artillery"	;Artillery have large enough spreading, too unaccurate for us on large distances.
							;So, after 1000(70)m it will grow lineary with distance, not exponentially
							;Normalized to old spreading of Msta-S (Vd/Xmax=1/363 means 2.75 mils)
							;Use range spreading Vd, not side spreading
	;{spreading
	;	{radiusTable
	;		{0 0.01}
	;		{50 (* %mils 0.75)}
	;		{70 (* %mils 1.0)}
	;		{105 (* %mils 1.5)}
	;		{140 (* %mils 2.5)}
	;		{210 (* %mils 4.5)}
	;		{280 (* %mils 6.0)}
	;		{350 (* %mils 7.5)}
	;		{420 (* %mils 9.0)}
	;	}
	;	{BurstRecoveryTime 0.5}
	;	{SpreadPower	1.7}
	;	{SpreadXYRatio	1}
	;}

	{spreading
		{radiusTable
			{0 0.01}
			{50 (* %mils 0.05)}
			{70 (* %mils 0.35)}
			{105 (* %mils 0.75)}
			{140 (* %mils 1.0)}
			{210 (* %mils 2.5)}
			{280 (* %mils 3.0)}
			{350 (* %mils 3.5)}
			{420 (* %mils 4.0)}
			{490 (* %mils 4.5)}
			{560 (* %mils 5.0)}
			{630 (* %mils 5.5)}
			{700 (* %mils 6.0)}
			{770 (* %mils 6.5)}
			{840 (* %mils 7.0)}
			{910 (* %mils 7.5)}
			{980 (* %mils 8.0)}
			{1050 (* %mils 9.0)}

		}
		{BurstRecoveryTime 0.5}
		{SpreadPower	1.7}
		{SpreadXYRatio	1}
	}
)

;(define "CW_acc_artillery"
;("CW_XP_acc_artillery" mils( %mils))
;)

(define "CW_XP_acc_guided"	;for guided shells - do not grow with distance
	{spreading
		{radiusTable
			{0 0.01}
			{75 (* %kvo_meters 0.6)}
			{100 (* %kvo_meters 0.8)}
			{125 (* %kvo_meters 1.2)}
			{150 (* %kvo_meters 2.0)}
			{250 (* %kvo_meters 2.0)}
			{280 (* %kvo_meters 2.0)}
			{350 (* %kvo_meters 2.0)}
			{420 (* %kvo_meters 2.0)}
			{490 (* %kvo_meters 2.0)}
			{560 (* %kvo_meters 2.0)}
			{630 (* %kvo_meters 2.0)}
			{700 (* %kvo_meters 2.0)}
			{770 (* %kvo_meters 2.0)}
			{840 (* %kvo_meters 2.0)}
			{910 (* %kvo_meters 2.0)}
			{980 (* %kvo_meters 2.0)}
			{1050 (* %kvo_meters 2.0)}
		}
		{BurstRecoveryTime 0.5}
		{SpreadPower	1.7}
		{SpreadXYRatio	1}
	}
)

(define "CW_XP_acc_heli"	;for helicopters unguided aircraft missile

	{spreading
		{radiusTable
			{0 0.01}
			{75 (* 0.5 %mils 0.375)} ; 0.5 ÐºÐ¾Ð¼Ð¿ÐµÐ½ÑÐ°Ñ†Ð¸Ñ Ð²ÐµÑ€Ñ‚Ð¸ÐºÐ°Ð»ÑŒÐ½Ð¾Ð³Ð¾ Ñ€Ð°Ð·Ð±Ñ€Ð¾ÑÐ°
			{100 (* 0.5 %mils 0.5)}
			{125 (* 0.75 %mils 1.0)}
			{150 (* 0.75 %mils 2.5)}
			{250 (* 0.75 %mils 4.5)}

		}
		{BurstRecoveryTime 0.5}
		{SpreadPower	1.7}
		{SpreadXYRatio	1}
	}
)

(define "CW_XP_acc_common"	;experiments show, that we need to set here 0.45 of needed radius in meters
						;and SpreadPower about 1.7 to have a normal-like distribution
						;Experimentally. T-72A (0.27 mils by APFSDS) hits T-90A in 2000(100) in 59%
	{spreading
		{radiusTable
			{0 0.01}
			{75 (* %mils 1.52)}
			{100 (* %mils 2.04)}
			{108 (* %mils 2.56)}
			{115 (* %mils 3.32)}
			{120 (* %mils 4.08)}
			{140 (* %mils 4.61)}
			{145 (* %mils 5.12)}
			{150 (* %mils 6.15)}
			{165 (* %mils 15.21)}
			{185 (* %mils 18.3)}
			{200 (* %mils 20.42)}
			{255 (* %mils 25.61)}
		}
		{BurstRecoveryTime 0.5}
		{SpreadPower	1.7}
		{SpreadXYRatio	1.0}
	}
)

(define "CW_small_AP_damage"  ;steel AP
	{projectileDamageTable
		{60 (* 1.91 %penetr_1000m)}
		{115 (* 1.43 %penetr_1000m)}
		{140 %penetr_1000m}
		{148 (* 0.900 %penetr_1000m)}
		{150 (* 0.770 %penetr_1000m)}
		{160 (* 0.510 %penetr_1000m)}
		{170 (* 0.400 %penetr_1000m)}
		{180 (* 0.300 %penetr_1000m)}
		{185 (* 0.238 %penetr_1000m)}
		{190 (* 0.176 %penetr_1000m)}
		{215 (* 0.104 %penetr_1000m)}
	}
)

(define "relaxationtime"
(mod "mp"
	{tag "prepare"}
	{Special
		{Mode aim}
		{PrepareTime 0}
		{PrepareCursor "aimed_shot_aa"}
	}
	{RelaxationTime %rt}
)
)

(define "CW_acc_grlaunch"	;Grenade launchers are too unaccurate for us when using atillery scale.
							;So, they have separate accuracy scale, four times more accurate then common weapons
							;Use side spreading
	{spreading
		{radiusTable
			{0 0.01}
			{75 (* %mils 0.38)}
			{100 (* %mils 0.75)}
			{108 (* %mils 1.2)}
			{120 (* %mils 1.50)}
			{140 (* %mils 1.9)}
			{145 (* %mils 2.28)}
			{150 (* %mils 3.03)}
			{155 (* %mils 3.8)}
			{165 (* %mils 6.08)}
			{185 (* %mils 7.6)}
			{255 (* %mils 11.4)}
		}
		{BurstRecoveryTime 0.5}
		{SpreadPower	1.7}
		{SpreadXYRatio	1}
	}
)

(define "accuracy_open_sight"
	{spreading
		{radiusTable
			{0 0.01}
			{20 (* 0.07 %moa)}
			{70 (* 0.18 %moa)}
			{70 (* 0.35 %moa)}
			{75 (* 0.43 %moa)}
			{100 (* 0.84 %moa)}
			{105 (* 1.05 %moa)}
			{110 (* 1.26 %moa)}
			{115 (* 1.47 %moa)}
			{120 (* 1.67 %moa)}
			{140 (* 2.51 %moa)}
			{150 (* 3.33 %moa)}
			{175 (* 5.0 %moa)}
		}
		{BurstAccuracy 100 97 94 89 84 79 74 70 65 60}
		{SpreadPower	1.7}
		{SpreadXYRatio	1.0}
		{BurstRecoveryTime 1.5}
	}
)

(define "accuracy_open_sight_at_rifle"
	{spreading
		{radiusTable
			{0 0.01}
			{20 (* 0.07 %moa)}
			{70 (* 0.18 %moa)}
			{70 (* 0.35 %moa)}
			{75 (* 0.43 %moa)}
			{100 (* 0.84 %moa)}
			{105 (* 1.05 %moa)}
			{110 (* 1.26 %moa)}
			{115 (* 1.47 %moa)}
			{120 (* 1.67 %moa)}
			{140 (* 2.51 %moa)}
			{150 (* 3.33 %moa)}
			{175 (* 5.0 %moa)}
		}
		{BurstAccuracy 100 97 94 89 84 79 74 70 65 60}
		{SpreadPower	1.7}
		{SpreadXYRatio	1.0}
		{BurstRecoveryTime 1.5}
	}
)



;(define "CW_acc_guided"
;("CW_XP_acc_guided" kvo_meters( %kvo_meters 5 ))
;)


(define "accuracy_optic_rpg"
	{spreading
		{radiusTable
			{0 0.01}
			{20 (* 0.02 %moa)}
			{30 (* 0.07 %moa)}
			{50 (* 0.14 %moa)}
			{65 (* 0.24 %moa)}
			{75 (* 0.36 %moa)}
			{90 (* 0.55 %moa)}
			{100 (* 0.70 %moa)}
			{110 (* 1.06 %moa)}
			{120 (* 1.10 %moa)}
			{150 (* 1.60 %moa)}
		}
		{BurstAccuracy 100 96 92 86 80 74 68 62 56 50}
		{SpreadPower	1.7}
		{SpreadXYRatio	1.0}
	}
)
(define "accuracy_open_rpg"
	{spreading
		{radiusTable
			{0 0.01}
			{20 (* 0.02 %moa)}
			{30 (* 0.07 %moa)}
			{50 (* 0.14 %moa)}
			{65 (* 0.24 %moa)}
			{75 (* 0.43 %moa)}
			{90 (* 0.66 %moa)}
			{100 (* 0.84 %moa)}
			{110 (* 1.27 %moa)}
			{120 (* 1.33 %moa)}
		}
		{BurstAccuracy 100 97 94 89 84 79 74 70 65 60}
		{SpreadPower	1.7}
		{SpreadXYRatio	1.0}
	}
)


;/*----------------------*/

	(define "velocity"				;// muzzle velocity in meters per second
		{speed (* 0.70 %mps)}
	)
	(define "fire rate"				;// rate of fire in rounds per minute
		{recoveryTime (/ 60 (* %rpm 1.07))}
	)
	(define "reload time"			;// reload time for magazine or shell
		{rechargeTime %seconds}
	)
	(define "human health"
		{healthDamage (* 1.0 %damage)}
	)
	(define "armor health"
		{minimumDamageModifier (* 1.0 %damage)}
	)

;/*----------------------*/

	(define "accuracy_pistol"
		("pistol_range"  r1(0.4) r2(1.0) globalaccuracy(1.10))
	)

;/*----------------------*/

	(define "accuracy_smg"
		("smg_range"  r1(0.9) r2(2.2) globalaccuracy(1.10))
	)

;/*----------------------*/

	(define "accuracy_rifle"
		("rifle_range"	r0(0.2) r1(0.4) r2(0.8) r3(1.00) globalaccuracy(1.10))
	)
	(define "accuracy_rifle_sniper"
		("sniper_range" r0(0.2) r1(0.4) r2(0.8) r3(1.00) globalaccuracy(1.10))
	)
	(define "accuracy_rifle_auto_noscope"
		("rifle_range"	r0(0.4) r1(0.9) r2(1.5) r3(2.30) globalaccuracy(1.10))
	)
	(define "accuracy_rifle_auto_urgi"
		("rifle_range"	r0(0.2) r1(0.4) r2(0.9) r3(1.6) globalaccuracy(1.10))
	)
	(define "accuracy_rifle_ar15"
		("rifle_range"	r0(0.3) r1(0.6) r2(1.1) r3(1.8) globalaccuracy(1.10))
	)		
	(define "accuracy_rifle_auto"
		("rifle_range"	r0(0.3) r1(1.2) r2(1.5) r3(2.8) globalaccuracy(1.10))
	)
	(define "accuracy_rifle_ddm4"
		("rifle_range"	r0(0.3) r1(0.9) r2(1.2) r3(2.2) globalaccuracy(1.10))
	)
	(define "accuracy_rifle_sai"
		("rifle_range"	r0(0.3) r1(0.7) r2(1.0) r3(1.9) globalaccuracy(1.10))
	)	
	(define "accuracy_rifle_auto_xm157"
		("rifle_range"	r0(0.1) r1(0.2) r2(0.5) r3(0.7) globalaccuracy(1.10))
	)
	(define "accuracy_rifle_auto_qbz191"
		("rifle_range"	r0(0.2) r1(0.8) r2(1.2) r3(1.5) globalaccuracy(1.10))
	)	
	(define "accuracy_rifle_auto_sniper"
		("sniper_range" r0(0.4) r1(0.5) r2(1.0) r3(1.50) globalaccuracy(1.10))
	)
	(define "accuracy_assault_rifle"
		("rifle_range"	r0(0.2) r1(1.0) r2(1.5) r3(2.5) globalaccuracy(1.10))
	)
	(define "accuracy_shotgun"
		("rifle_range"	r0(0.2) r1(1.4) r2(0.9) r3(1.25) globalaccuracy(1.10))
	)
	(define "accuracy_rifle_at"
		("rifle_range"	r0(0.4) r1(0.8) r2(1.2) r3(1.50) globalaccuracy(1.10))
	)

;/*----------------------*/

	(define "accuracy_mg"
		("mg_range" r1(0.5) r2(1.3) r3(1.5) globalaccuracy(1.10))
	)
	(define "accuracy_hmg"
		("hmg_range" r1(0.5) r2(1.3) r3(1.5) globalaccuracy(1.10))
	)
	(define "accuracy_hmg_xm157"
		("hmg_range" r1(0.1) r2(0.2) r3(0.9) globalaccuracy(1.10))
	)
	(define "accuracy_hmg_holo"
		("hmg_range" r1(0.4) r2(0.9) r3(1.6) globalaccuracy(1.10))
	)		
	(define "accuracy_hmg_tank"
		("hmg_range" r1(0.4) r2(1.2) r3(1.4) globalaccuracy(1.10))
	)

	(define "accuracy_mg_plane"
		("plane_range" r1(1.0) globalaccuracy(1.10))
	)

	(define "accuracy_grenade_launcher"
		("distance_3" r1(2.90) r2(3.90) r3(5.15) r4(9.50))
	)


;/*=========================Curves======================*/


 	(define "pistol_range"
		{spreading
			{radiusTable {0 0.01} {30 (* %globalaccuracy %r1)} {60 (* %globalaccuracy %r2)}}
		}
	)
	(define "smg_range"
		{spreading
			{radiusTable {0 0.01} {60 (* %globalaccuracy %r1)} {110 (* %globalaccuracy %r2)}}
		}
	)
 	(define "rifle_range"
		{spreading
			{radiusTable {0 0.01} {30 %r0} {60 (* %globalaccuracy %r1)} {95 (* %globalaccuracy %r2)} {120 (* %globalaccuracy %r3)}}
		}
	)
	(define "sniper_range"
		{spreading
			{radiusTable {0 0.01} {30 %r0} {60 (* %globalaccuracy %r1)} {95 (* %globalaccuracy %r2)} {120 (* %globalaccuracy %r3)} {130 (* %globalaccuracy %r3)} {180 5}}
		}
	)
	(define "mg_range"
		{spreading
			{radiusTable {0 0.02} {40 (* %globalaccuracy %r1)} {80 (* %globalaccuracy %r2)} {110 (* %globalaccuracy %r3)} {120 (* %globalaccuracy %r3)} {180 5}}
		}
	)
	(define "hmg_range"
		{spreading
			{radiusTable {0 0.02} {40 (* %globalaccuracy %r1)} {80 (* %globalaccuracy %r2)} {110 (* %globalaccuracy %r3)} {140 (* %globalaccuracy %r3)} {200 5}}
		}
	)
	(define "plane_range"
		{spreading
			{radiusTable {0 1} {100 (* 3 (* %globalaccuracy %r1))} {200 (* 5 (* %globalaccuracy %r1))} {300 (* 8 (* %globalaccuracy %r1))} {400 (* 10 (* %globalaccuracy %r1))}}
		}
	)

	(define "distance_3"
		{spreading
			{radiusTable {10 0} {25 %r1} {50 %r2} {70 %r3} {90 %r4} {500 (* 2.25 %r4)}}
		}
	)


;/*====================Slope Multiplier==================*/


	;/*====================Slope Multiplier==================*/


	(define "ap_slope_multiplier"	;use for AP and for any other Projectiles not specifically defined below
		{overmatchCalibre
			{0		0.96		0.0637	}
			{5		0.96297		0.0637	}
			{10		0.98297		0.0637	}
			{15		1.00066		0.0969	}
			{20		1.0361		0.13561	}
			{25		1.1116		0.16164	}
			{30		1.2195		0.19702	}
			{35		1.3771		0.22546	}
			{40		1.6263		0.26313	}
			{45		2.0033		0.34717	}
			{50		2.6447		0.57353	}
			{55		3.231		0.69075	}
			{60		4.0708		0.81826	}
			{65		6.2655		0.9192	}
			{70		8.6492		1.00539	}
			{75		13.7512		1.074	}
			{80		21.8713		1.17973	}
			{85		34.4862		1.28631	}
			{90		99.0000		0.00000	}
		}
	)
	(define "ap_7_62_slope_multiplier"	;	7.62mm - 8mm bullets, MG, rifles
		{overmatchCalibre
			{0		1			0.00	}
			{10		1.037		0.01	}
			{20		1.10		0.02	}
			{30		1.435		0.15	}
			{40		2.19		0.10	}
			{45		2.79		0.12	}
			{50		3.86		0.12	}
			{60		6.23		0.15	}
			{65		7.70		0.15	}
			{70		9.93		0.15	}
			{75		14.3		0.17	}
			{80		22.9		0.17	}
			{85		62.7		0.242	}
			{90		99.0000		0.00000	}
		}
	)
	(define "ap_12_slope_multiplier"	;	12.7mm bullets, HMG
		{overmatchCalibre
			{0		1			0.00	}
			{20		1.24		-0.015	}
			{30		1.59		0.015	}
			{40		1.885		0.11	}
			{45		2.395		0.15	}
			{50		3.32		0.14	}
			{60		5.43		0.185	}
			{65		6.78		0.192	}
			{70		8.85		0.193	}
			{75		12.75		0.21	}
			{80		22.17		0.245	}
			{85		69.6		0.36	}
			{90		99.0000		0.00000	}
		}
	)

	(define "at_rifle_slope_multiplier"	;use for AP and for any other Projectiles not specifically defined below
		{overmatchCalibre
			{0		0.1			0.0637	}
			{10		1.00066		0.0969	}
			{15		1.0361		0.13561	}
			{20		1.1116		0.16164	}
			{25		1.2195		0.19702	}
			{30		1.3771		0.22546	}
			{40		1.6263		0.26313	}
			{65		2.0033		0.34717	}
			{70		2.6447		0.57353	}
			{80		8.6492		1.00539	}
			{85		21.8713		1.17973	}
			{90		99.0000		0.00000	}
		}
	)

	(define "apcr_slope_multiplier"	;use for APCR & HVAP Projectiles 76mm Caliber or below
		{overmatchFixed
			{0  1.00	}
			{10	1.01	}
			{10	1.03	}
			{15	1.07	}
			{20	1.13	}
			{25	1.23	}
			{30	1.36	}
			{35	1.59	}
			{40	1.90	}
			{45	2.28	}
			{50	2.78	}
			{55	3.41	}
			{60	4.23	}
			{65	5.29	}
			{70	6.69	}
			{75	8.52	}
			{80	10.93	}
			{85	14.15	}
			{90 99		}
		}
	)

	(define "sway_pistol"
		{swayparams
			{bounds -0.25 0.2 -0.35 0.3}		; x -> negativeYaw, y -> positiveYaw, z -> negativePitch, w -> positive Pitch (in degree)
			{speed 0.1}						; sway speed coef
		}
	)

	(define "sway_smg"
		{swayparams
			{bounds -0.25 0.2 -0.35 0.3}		; x -> negativeYaw, y -> positiveYaw, z -> negativePitch, w -> positive Pitch (in degree)
			{speed 0.1}						; sway speed coef
		}
	)

	(define "sway_mg"
		{swayparams
			{bounds -0.3 0.2 -0.4 0.3}		; x -> negativeYaw, y -> positiveYaw, z -> negativePitch, w -> positive Pitch (in degree)
			{speed 0.3}						; sway speed coef
		}
	)

	(define "sway_rifle"
		{swayparams
			{bounds -0.3 0.2 -0.4 0.3}		; x -> negativeYaw, y -> positiveYaw, z -> negativePitch, w -> positive Pitch (in degree)
			{speed 0.2}						; sway speed coef
		}
	)

	(define "sway_sniper"
		{swayparams
			{bounds -0.35 0.25 -0.45 0.35}		; x -> negativeYaw, y -> positiveYaw, z -> negativePitch, w -> positive Pitch (in degree)
			{speed 0.2}						; sway speed coef
		}
	)

	(define "recoil_shotgun"
		{recoil
			{shoot {2.50 0.75} {1 0.75}}		; pitch / +- random and yaw / +- random, per shot, degree
			{maxAngle 8 8}						; max recoil angle (pitch/yaw), degree
			{restoreTime 1.00 0.025}			; recoil restore time, first -> minimum time, second -> additional time per shot, sec
			{restoreDelay 0.20}					; delay before restore, sec
		}
	)

	(define "recoil_rifle_at"
		{recoil
			{shoot {2.50 0.75} {1 0.75}}		; pitch / +- random and yaw / +- random, per shot, degree
			{maxAngle 8 8}						; max recoil angle (pitch/yaw), degree
			{restoreTime 1.10 0.025}			; recoil restore time, first -> minimum time, second -> additional time per shot, sec
			{restoreDelay 0.20}					; delay before restore, sec
		}
	)

	(define "recoil_pistol"
		{recoil
			{shoot {0.40 0.15} {0 0.45}}		; pitch / +- random and yaw / +- random, per shot, degree
			{maxAngle 8 8}						; max recoil angle (pitch/yaw), degree
			{restoreTime 0.25 0.005}			; recoil restore time, first -> minimum time, second -> additional time per shot, sec
			{restoreDelay 0.2}					; delay before restore, sec
		}
	)

	(define "recoil_stg"
		{recoil
			{shoot {0.45 0.20} {0 0.50}}		; pitch / +- random and yaw / +- random, per shot, degree
			{maxAngle 8 8}						; max recoil angle (pitch/yaw), degree
			{restoreTime 0.25 0.005}			; recoil restore time, first -> minimum time, second -> additional time per shot, sec
			{restoreDelay 0.2}					; delay before restore, sec
		}
	)

	(define "recoil_fg42"
		{recoil
			{shoot {0.55 0.20} {0.10 0.50}}		; pitch / +- random and yaw / +- random, per shot, degree
			{maxAngle 8 8}						; max recoil angle (pitch/yaw), degree
			{restoreTime 0.5 0.025}				; recoil restore time, first -> minimum time, second -> additional time per shot, sec
			{restoreDelay 0.20}					; delay before restore, sec
		}
	)

	(define "recoil_rifle_semi"
		{recoil
			{shoot {0.75 0.25} {0 0.60}}		; pitch / +- random and yaw / +- random, per shot, degree
			{maxAngle 8 8}						; max recoil angle (pitch/yaw), degree
			{restoreTime 0.5 0.025}				; recoil restore time, first -> minimum time, second -> additional time per shot, sec
			{restoreDelay 0.20}					; delay before restore, sec
		}
	)

	(define "recoil_rifle_bolt"
		{recoil
			{shoot {0.90 0.25} {0 0.60}}		; pitch / +- random and yaw / +- random, per shot, degree
			{maxAngle 8 8}						; max recoil angle (pitch/yaw), degree
			{restoreTime 0.5 0.025}				; recoil restore time, first -> minimum time, second -> additional time per shot, sec
			{restoreDelay 0.20}					; delay before restore, sec
		}
	)

	(define "recoil_sniper_semi"
		{recoil
			{shoot {1.20 0.25} {0 0.75}}		; pitch / +- random and yaw / +- random, per shot, degree
			{maxAngle 8 8}						; max recoil angle (pitch/yaw), degree
			{restoreTime 1.00 0.025}			; recoil restore time, first -> minimum time, second -> additional time per shot, sec
			{restoreDelay 0.20}					; delay before restore, sec
		}
	)

	(define "recoil_sniper_bolt"
		{recoil
			{shoot {1.50 0.25} {0 0.75}}		; pitch / +- random and yaw / +- random, per shot, degree
			{maxAngle 8 8}						; max recoil angle (pitch/yaw), degree
			{restoreTime 1.00 0.025}			; recoil restore time, first -> minimum time, second -> additional time per shot, sec
			{restoreDelay 0.20}					; delay before restore, sec
		}
	)

	(define "recoil_smg_low"
		{recoil
			{shoot {0.325 0.10} {0 0.2}}		; pitch / +- random and yaw / +- random, per shot, degree
			{maxAngle 8 8}						; max recoil angle (pitch/yaw), degree
			{restoreTime 0.15 0.005}				; recoil restore time, first -> minimum time, second -> additional time per shot, sec
			{restoreDelay 0.2}					; delay before restore, sec
		}
	)

	(define "recoil_smg_medium"
		{recoil
			{shoot {0.325 0.15} {0 0.25}}		; pitch / +- random and yaw / +- random, per shot, degree
			{maxAngle 8 8}						; max recoil angle (pitch/yaw), degree
			{restoreTime 0.15 0.005}				; recoil restore time, first -> minimum time, second -> additional time per shot, sec
			{restoreDelay 0.2}					; delay before restore, sec
		}
	)

	(define "recoil_mg_medium"
		{recoil
			{shoot {0.45 0.25} {0 0.515}}		; pitch / +- random and yaw / +- random, per shot, degree
			{maxAngle 8 8}						; max recoil angle (pitch/yaw), degree
			{restoreTime 0.25 0.005}				; recoil restore time, first -> minimum time, second -> additional time per shot, sec
			{restoreDelay 0.2}					; delay before restore, sec
		}
	)

	(define "recoil_hmg_medium"
		{recoil
			{shoot {0.50 0.25} {0 0.55}}		; pitch / +- random and yaw / +- random, per shot, degree
			{maxAngle 8 8}						; max recoil angle (pitch/yaw), degree
			{restoreTime 0.25 0.005	}			; recoil restore time, first -> minimum time, second -> additional time per shot, sec
			{restoreDelay 0.2}					; delay before restore, sec
		}
	)
