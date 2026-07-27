; // This script defines the VISION FIELDS of the units.
; // Units can have several vision fields. For example, a human has the forward main vision field and the peripheral one.
; //
; // {uncover 15} <- Will spot hidden soldiers in bushes within 15m
; // {h_fov 100}  <- Horizontal field of view arc = 100 degree
; // {v_fov 180}  <- Vertical field of view arc = 180 degree (straight up or down)
; // ("spotting" args 2)  <- Sets the "vision range", defined in generic.inc, so this unit will do spotting twice as far.
; // Or in other terms, the vision distances will be x2


;/*=============== Human Begin ===============*/


{"human"								; // Standard human vision
	{uncover 10}
    	("h_fov" h(100))
    	("v_fov" v(120))
    	("vision_human")
}

{"human_peripheral"						; // Additional short-range situational awareness for humans
	{uncover 5}
    {h_fov 200}
    {v_fov 120}
    ("spotting" r(0.35))
}

{"human_elite"							; // Soldiers with better vision (elites, officers, snipers)
	("uncover" u(20))
    ("h_fov" h(80))
    ("v_fov" v(80))
    ("vision_sniper")
}

{"human_recon"							; // Soldiers with dedicated best vision (recon soldiers)
    ("uncover" u(30))
    ("h_fov" h(90))
    ("v_fov" v(100))
    ("vision_sniper")
}

{"rifle_telescope"						; // Primary vision for sniper scopes
	 ("uncover" u(40))
    ("h_fov" h(360))
    ("v_fov" v(120))
    ("glass" args 7)

}

{"field_glass"							; // Primary vision for binoculars
	  ("uncover" u(20))
    ("h_fov" h(45))
    ("v_fov" v(120))
    ("glass" args 50)

}

{"soldier_focused"						; // Secondary vision when soldier is looking through scope or binoculars
	("uncover" u(20))
   	 ("h_fov" h(20))
   	 ("v_fov" v(50))
   	 ("vision_binocular")

}

{"human_elite_IR_optic"							; // Soldiers with better vision (elites, officers, snipers)
	("uncover" u(40))
    ("h_fov" h(90))
    ("v_fov" v(100))
	("vision_human_IR")
}

{"mp_soldier"
    ("uncover" u(20))
    ("h_fov" h(90))
    ("v_fov" v(100))
    ("vision_human")
	;{wide 10}
}

;/*=============== Human End ===============*/

;/*=============== Cannon Begin ===============*/

{"cannon_main"							; // Standard main gun / gunner vision for cannons
	  ("uncover" u(25))
    ("h_fov" h(60))
    ("v_fov" v(140))
    ("vision_optic")

}

{"cannon_mortar"						; // All mortars
	{uncover 25}
	{h_fov 90}
	{v_fov 170}
	("vision_optic")
}

{"cannon_range_200"						; // Gunner Vision - Gun range 200m or less (direct fire only)
	{uncover 25}
	{h_fov 30}
	{v_fov 170}
	("vision_optic")

}

{"cannon_range_210"						; // Gunner Vision - Gun range 210m (direct fire only)
	{uncover 25}
	{h_fov 30}
	{v_fov 170}
	("vision_optic")

}

{"cannon_range_220"						; // Gunner Vision - Gun range 220m (direct fire only)
	{uncover 25}
	{h_fov 30}
	{v_fov 170}
	("vision_optic")

}

{"cannon_range_230"						; // Gunner Vision - Gun range 230m (direct fire only)
	{uncover 25}
	{h_fov 30}
	{v_fov 170}
	("vision_optic")

}

{"cannon_range_240"						; // Gunner Vision - Gun range 240m (direct fire only)
	{uncover 25}
	{h_fov 30}
	{v_fov 170}
	("vision_optic")

}

{"cannon_range_250"						; // Gunner Vision - Gun range 250m (direct fire only)
	{uncover 25}
	{h_fov 30}
	{v_fov 170}
	("vision_optic")

}

{"atgm_main"							; // Recon gunner vision
	{uncover 25}
	{h_fov 15}
	{v_fov 15}
	("vision_optic")

}

;/*=============== Cannon End ===============*/

;/*=============== Vehicle Begin ===============*/

{"vehicle_main"							; // Standard vehicle / driver vision
	 ("uncover" u(25))
    ("h_fov" h(50))
    ("v_fov" v(140))
    ("vision_driver")

}

{"vehicle_top_mgun"						; // Top mounted Machine gunners vision (jeeps, halftracks, etc)
	  ("h_fov" h(90))
    ("v_fov" v(140))
    ("vision_human")

}

{"vehicle_top_turret"					; // Vision for cars with closed top / turret gunner
	 ("uncover" u(25))
    ("h_fov" h(360))
    ("v_fov" v(20))
    ("vision_turret")

}

{"vehicle_around"						; // Vehicle peripheral / commander vision
	{uncover 25}
	{h_fov 360}
	{v_fov 60}
	("vision_optic")
}

{"vehicle_commander"							; // Commander forward Periscope
	{uncover 25}
	{h_fov 60}
	{v_fov 60}
	("vision_optic")
}

{"vehicle_scout"							; // 360 Scout Vision on American
	 ("uncover" u(25))
    ("h_fov" h(100))
    ("v_fov" v(20))
    ("vision_turret")

}

{"vehicle_main_IR"
    ("uncover" u(25))
    ("h_fov" h(50))
    ("v_fov" v(140))
    ("vision_driver_IR")
}

;/*=============== Vehicle End ===============*/

;/*=============== Tank Begin ===============*/

{"recon_main"							; // Recon gunner vision
	{uncover 25}
	{h_fov 15}
	{v_fov 90}
	("vision_optic_modern_2gen_ground")
}

{"recon_main_2G"							; // Recon gunner vision
	{uncover 25}
	{h_fov 15}
	{v_fov 90}
	("vision_optic_modern_3gen_ground")
}

{"recon_main_3G"							; // Recon gunner vision
	 ("uncover" u(35))
    	("h_fov" h(15))
   	 ("v_fov" v(140))
   	 ("vision_optic_modern_and_IR_3gen_ground_recon")
}

{"recon_open"							; // Open Top Recon gunner vision
	{uncover 25}
	{h_fov 15}
	{v_fov 170}
	("vision_tgunner")
}

{"tank_main_mg"							; // Gunner vision when main gun is MG
	 ("uncover" u(25))
    ("h_fov" h(5))
    ("v_fov" v(90))
    ("vision_tgunner")

}

{"tank_main-l"							; // Light tank gunner vision
	  ("uncover" u(25))
    ("h_fov" h(30))
    ("v_fov" v(140))
    ("vision_driver")

}

{"tank_main-m"							; // Medium tank gunner vision
	 ("uncover" u(25))
    ("h_fov" h(25))
    ("v_fov" v(140))
    ("vision_driver")

}

{"tank_main-h"							; // Heavy tank gunner vision
	  ("uncover" u(25))
    ("h_fov" h(25))
    ("v_fov" v(140))
    ("vision_driver")

}

{"tank_main-aa"							; // Tank's that have a AA gun from main weapon
  ("h_fov" h(90))
    ("v_fov" v(140))
    ("vision_human")

}

{"tank_driver"							; //Tank Driver vision
	 ("uncover" u(25))
    ("h_fov" h(50))
    ("v_fov" v(20))
    ("vision_driver")

}

{"tank_hull-mg"							; // Tank Hull Machine gunner
	 ("uncover" u(25))
    ("h_fov" h(5))
    ("v_fov" v(90))
    ("vision_tgunner")

}

{"tank_back"							; // Tank with rear turret machine gun
	 ("uncover" u(25))
    ("h_fov" h(50))
    ("v_fov" v(120))
    ("vision_driver")

}

{"tank_remote-mg"						; // Remote controlled roof Machine gun (Hetzer, Stug3g, etc)
		 ("uncover" u(25))
    ("h_fov" h(5))
    ("v_fov" v(90))
    ("vision_tgunner")
}

{"tank_shielded-mg"						; // Shielded Machine gun (BA-64)
		 ("uncover" u(25))
    ("h_fov" h(5))
    ("v_fov" v(90))
    ("vision_tgunner")
}

{"tank_around"							; // Tank peripheral / Commander vision
	;{uncover 25}
	;{h_fov 360}
	;{v_fov 170}
	;("spotting" r(0.5))
}

{"tank_around-l"

}

{"tank_around_small"							; // Tank peripheral / Commander vision
	;{uncover 25}
	;{h_fov 360}
	;{v_fov 170}
	;("spotting" r(0.15))
}

{"tank_commander"							; // Commander forward Periscope
    ("uncover" u(25))
    ("h_fov" h(5))
    ("v_fov" v(90))
    ("vision_tgunner")
}


{"tank_hull-mg"							; // Tank Hull Machine gunner
	{uncover 20}
	{h_fov 30}
	{v_fov 30}
	("spotting" r(0.85))
}

{"tank_rear-mg"							; // Tank with rear turret machine gun
	{uncover 20}
	{h_fov 40}
	{v_fov 50}
	("spotting" r(0.85))
}

{"tank_remote-mg"						; // Remote controlled roof Machine gun (Hetzer, Stug3g, etc)
	{uncover 20}
	{h_fov 8}
	{v_fov 8}
	("spotting" r(0.85))
}

{"tank_shielded-mg"						; // Shielded Machine gun (BA-64)
	{uncover 20}
	{h_fov 8}
	{v_fov 30}
	("spotting" r(1))
}

{"tank_around"							; // Tank peripheral / Commander vision
	{uncover 15}
	{h_fov 360}
	{v_fov 170}
	("spotting" r(0.5))
}



;/*=============== Tank End ===============*/

;/*=============== New Setup Begin ===============*/

{"tank_range_200"							; // Gunner Vision - Gun range 200m or less (direct fire only)
	 ("uncover" u(20))
    ("h_fov" h(15))
    ("v_fov" v(140))
    ("vision_optic_modern_1gen_ground")
}

{"tank_range_210"							; // Gunner Vision - Gun range 210m (direct fire only)
	 ("uncover" u(20))
    ("h_fov" h(15))
    ("v_fov" v(140))
    ("vision_optic_modern_1gen_ground")
}

{"tank_range_220"							; // Gunner Vision - Gun range 220m (direct fire only)
	 ("uncover" u(20))
    ("h_fov" h(15))
    ("v_fov" v(140))
    ("vision_optic_modern_1gen_ground")
}

{"tank_range_230"							; // Gunner Vision - Gun range 230m (direct fire only)
	 ("uncover" u(20))
    ("h_fov" h(15))
    ("v_fov" v(140))
    ("vision_optic_modern_1gen_ground")
}
{"tank_range_240"							; // Gunner Vision - Gun range 240m (direct fire only)
	 ("uncover" u(20))
    ("h_fov" h(15))
    ("v_fov" v(140))
    ("vision_optic_modern_2gen_ground"))
}

{"tank_range_250"							; // Gunner Vision - Gun range 250m (direct fire only)
	 ("uncover" u(20))
    ("h_fov" h(15))
    ("v_fov" v(140))
    ("vision_optic_modern_2gen_ground")
}

{"tank_range_300"							; // Gunner Vision - Gun range 250m (direct fire only)
	 ("uncover" u(20))
    ("h_fov" h(15))
    ("v_fov" v(140))
    ("vision_optic_modern_2gen_ground")
}

;/*===========tank modern===============*/

{"tank_gunner_optic_1g"
    ("uncover" u(25))
    ("h_fov" h(45))
    ("v_fov" v(120))
    ("vision_optic_modern_1gen_ground")
	;{wide 10}
}

{"tank_gunner_optic_2g"
    ("uncover" u(25))
    ("h_fov" h(45))
    ("v_fov" v(140))
    ("vision_optic_modern_2gen_ground")
	;{wide 10}
}


{"tank_gunner_optic_3g"
    ("uncover" u(25))
    ("h_fov" h(45))
    ("v_fov" v(160))
    ("vision_optic_modern_3gen_ground")
	;{wide 10}
}


{"tank_gunner_optic_IR_1g"
    ("uncover" u(25))
    ("h_fov" h(45))
    ("v_fov" v(120))
    ("vision_optic_modern_and_IR_1gen_ground")
	;{wide 10}
}



{"tank_gunner_optic_IR_2g"
    ("uncover" u(35))
    ("h_fov" h(45))
    ("v_fov" v(140))
    ("vision_optic_modern_and_IR_2gen_ground")
	;{wide 10}
}

{"tank_gunner_optic_IR_2g_wide"
    ("uncover" u(35))
    ("h_fov" h(120))
    ("v_fov" v(180))
    ("vision_optic_modern_and_IR_2gen_ground")
	;{wide 10}
}



{"tank_gunner_optic_IR_3g"
    ("uncover" u(35))
    ("h_fov" h(45))
    ("v_fov" v(160))
    ("vision_optic_modern_and_IR_3gen_ground")
	;{wide 10}
}

{"moto"
    ("uncover" u(5))
    ("h_fov" h(30))
    ("v_fov" v(150))
    ("vision_driver")
}
{"moto_crew"
    ("uncover" u(5))
    ("h_fov" h(90))
    ("v_fov" v(60))
    ("vision_driver")
}
;/*=============== New Setup End ===============*/

;/*=============== Airborne Begin ===============*/

{"airborne_main"						; // Standard plane pilot vision
    ("uncover" u(5))
    ("h_fov" h(35))
    ("v_fov" v(140))
    ("vision_optic_modern_1gen")
}

{"airborne_main_mp"						; // Standard plane pilot vision for multiplayer
("uncover" u(5))
    ("h_fov" h(35))
    ("v_fov" v(140))
    ("vision_optic_modern_1gen")
}

{"airborne_around"						; // Pilot peripheral vision
	
}

{"airborne_scout"						; // Scout plane vision (better than normal plane)
    ("uncover" u(85))
    ("h_fov" h(180))
    ("v_fov" v(240))
    ("vision_optic_modern_1gen_air")
}

{"aircraft_optic_2g"
    ("uncover" u(35))
    ("h_fov" h(35))
    ("v_fov" v(140))
    ("vision_optic_modern_2gen")
}
{"aircraft_optic_3g"
    ("uncover" u(35))
    ("h_fov" h(35))
    ("v_fov" v(140))
    ("vision_optic_modern_3gen")
}
{"aircraft_optic_3g_wide"
    ("uncover" u(55))
    ("h_fov" h(120))
    ("v_fov" v(340))
    ("vision_optic_modern_3gen")
}


{"aircraft_optic_IR_1g"
    ("uncover" u(35))
    ("h_fov" h(35))
    ("v_fov" v(140))
    ("vision_optic_modern_and_IR_1gen")
}


{"aircraft_optic_IR_2g"
    ("uncover" u(35))
    ("h_fov" h(35))
    ("v_fov" v(180))
    ("vision_optic_modern_and_IR_2gen")
}
{"aircraft_optic_IR_2g_wide"
    ("uncover" u(35))
    ("h_fov" h(120))
    ("v_fov" v(180))
    ("vision_optic_modern_and_IR_2gen")
}


{"aircraft_optic_IR_3g"     ;
    {uncover 35}
    {h_fov 35}
    {v_fov 180}
    ("vision_optic_modern_and_IR_3gen")
}

{"aircraft_optic_IR_3g_wide"
    ("uncover" u(35))
    ("h_fov" h(120))
    ("v_fov" v(180))
    ("vision_optic_modern_and_IR_3gen")
}

{"aircraft_optic_IR_4g"     ;
    {uncover 45}
    {h_fov 35}
    {v_fov 180}
    ("vision_optic_modern_and_IR_4gen")
}

{"aircraft_optic_IR_4g_wide"
    ("uncover" u(45))
    ("h_fov" h(120))
    ("v_fov" v(180))
    ("vision_optic_modern_and_IR_4gen")
}

;/*=============== Airborne End ===============*/

;/*=============== Marine Begin ===============*/

{"marine_main"
	{h_fov 120}
	{v_fov 180}
	("spotting" r(0.85))
}

{"marine_around"
	{h_fov 360}
	{v_fov 180}
	("spotting" r(0.85))
}

{"marine_cannon"
	{h_fov 110}
	{v_fov 110}
	("spotting" r(1.3))
}

{"marine_torpedo"
	{h_fov 40}
	{v_fov 40}
	("spotting" r(0.85))
}

;/*=============== Marine End ===============*/

;/*=============== Other Begin ===============*/

{"noise_detection"								; // short-range movement and omnidirectional gunfire awareness
	{uncover 10}
	{h_fov 360}
	{v_fov 180}
	("hearing" args 0.2)
}

;{"cover-detector"
;	{uncover 5}
;	{h_fov 60}
;	{v_fov 120}
;	("spotting" r(1))
;}

{"animal"
	{uncover 2}
	{h_fov 360}
	{v_fov 120}
}

;{"searchlight"
;	{uncover 100}
;	{h_fov 12}
;	{v_fov 90}
;	("spotting" r(3.0))
;}

;{"watchtower"
;	{h_fov 90}
;	{v_fov 90}
;	("spotting" r(1))
;}

(define "ultra_vision"
	{radius 20}
	{h_fov 360}
	{v_fov 180}
	{uncover %range}
)


{"start_point"	("ultra_vision" range(20))}
{"map_point"	("ultra_vision" range(20))}
{"flag_point"	("ultra_vision" range(20))}

{"none"}

;/*=============== west81 setting Begin ===============*/

;/*===========missile ===============*/
{"missile"
;    ("radius" r(315))
    ("radius" r(415))
    ("uncover" u(350))
    ("h_fov" h(90))
    ("v_fov" v(90))
    ("vision_missile")
}
{"antimissile"
;    ("radius" r(315))
    ("radius" r(415))
    ("uncover" u(350))
    ("h_fov" h(90))
    ("v_fov" v(90))
    ("vision_antimissile")
}
{"semiLR_missile"
    ("radius" r(550))
    ("uncover" u(350))
    ("h_fov" h(70))
    ("v_fov" v(70))
    ("vision_missile")
}
{"vertical_missile"
    ("radius" r(315))
    ("uncover" u(350))
    ("h_fov" h(360))
    ("v_fov" v(180))
    ("vision_missile")
}
{"LRmissile"	;perhaps dont work
    ("radius" r(690))
    ("uncover" u(420))
;    ("radius" r(520))
;    ("uncover" u(120))
    ("h_fov" h(90))
    ("v_fov" v(90))
    ("vision_missile")
}
{"vertical_LRmissile"
    ("radius" r(690))
    ("uncover" u(420))
    ("h_fov" h(360))
    ("v_fov" v(180))
    ("vision_missile")
}
{"vertical_antimissile"
    ("radius" r(690))
    ("uncover" u(350))
    ("h_fov" h(90))
    ("v_fov" v(90))
    ("vision_antimissile")
}
{"vertical_antimissile_side"
    ("radius" r(690))
;    ("radius" r(420))
;    ("uncover" u(350))
    ("uncover" u(10))
    ("h_fov" h(360))
;    ("v_fov" v(360))
    ("v_fov" v(60))
   ("vision_antimissile")
}

{"missile_sam"
;    ("radius" r(113))
    ("radius" r(250))
    ("uncover" u(350))
    ("h_fov" h(180))
    ("v_fov" v(180))
    ("vision_missile")
}
{"antimissile_sam"
;    ("radius" r(113))
    ("radius" r(250))
    ("uncover" u(350))
    ("h_fov" h(180))
    ("v_fov" v(180))
    ("vision_antimissile")
}
{"missile_close"
    ("radius" r(58))
    ("uncover" u(350))
    ("h_fov" h(240))
    ("v_fov" v(240))
    ("vision_missile")
}
{"antimissile_close"
    ("radius" r(38))
    ("uncover" u(350))
    ("h_fov" h(240))
    ("v_fov" v(240))
    ("vision_antimissile")
}
{"harm_passive_radar"
    ("radius" r(240))
    ("uncover" u(350))
    ("h_fov" h(60))
    ("v_fov" v(90))
    ("vision_antirad")
}


;/*===========radar ===============*/

{"radar_around"	;16km, Тунгуска, Шилка, т.п.
    ("uncover" u(10))
    ("h_fov" h(360))
    ("v_fov" v(180))
    ("radius" r(54))
    ("vision_radar_short")
}


{"radar_wide"	;m163 7km
    ("uncover" u(1))
    ("h_fov" h(150))
    ("v_fov" v(150))
    ("radius" r(41))
    ("vision_radar_short")
}

{"radar_90grad"		;Mi-28, Apache, approx 15 km
    {uncover 10}
    {h_fov 90}
    {v_fov 90}
    ("vision_radar_ground")
}

{"radar_pantsir_s_front"	; 30km Панцирь-С (станция подсвета 1РС2-1Е) малый сектор
    ("uncover" u(10))
    ("h_fov" h(25))
    ("v_fov" v(180))
    ("radius" r(81))
    ("vision_radar_short")
}

{"radar_hawx_cwar"	;70km
    ("uncover" u(10))
    ("h_fov" h(360))
    ("v_fov" v(20))
    ("radius" r(104))
    ("vision_radar_X_band")
}

{"radar_sharp_hq7b"	; 40km малый сектор
    ("uncover" u(1))
    ("h_fov" h(25))
    ("v_fov" v(45))
    ("radius" r(93))
    ("vision_radar_short")
}

{"radar_around_apache"	;Apache, approx 15 km
    ("uncover" u(1))
    ("h_fov" h(360))
    ("v_fov" v(180))
    ("vision_radar_air")
}
{"radar_air_mi28n"	;Mi-28, approx 15 km
    ("uncover" u(1))
    ("h_fov" h(180))
    ("v_fov" v(60))
    ("vision_radar_air")
}

{"radar_around_roland"	;25km
    ("uncover" u(1))
    ("h_fov" h(360))
    ("v_fov" v(180))
    ("radius" r(75))
    ("vision_radar_short")
}

{"pelengator_strela10"	;182м по самолету
    ("uncover" u(1))
    ("h_fov" h(360))
    ("v_fov" v(180))
	("radius" r(42.3))
    ("vision_pelengator")	;видит авиацию и РЛС ПВО, не видит ракеты и наземные цели
}


{"pelengator_strela10_side"	;182м по самолету
    {uncover  5}
    {h_fov 90}
    {v_fov 180}
    ("radius" r(42.3))
    ("vision_pelengator")	;видит авиацию и РЛС ПВО, не видит ракеты и наземные цели
}



{"radarX_buk_85km"	;Бук-М1 СОУ
    ("uncover" u(1))
    ("h_fov" h(60))
    ("v_fov" v(90))
    ("radius" r(110))
    ("vision_radar_shipSAM")
}
{"radarX_buk_120km"	;Бук-М2 СОЦ
    ("uncover" u(1))
    ("h_fov" h(90))
    ("v_fov" v(90))
    ("radius" r(112))
    ("vision_radar_shipSAM")
}
{"radarX_kupol_160km"	;Купол М1-3 СОУ
    ("uncover" u(1))
    ("h_fov" h(30))
    ("v_fov" v(90))
    ("radius" r(115))
    ("vision_radar_shipSAM")
}

{"aircraft_radarX_80km"
    ("uncover" u(1))
    ("h_fov" h(120))
    ("v_fov" v(120))
    ("radius" r(53))
    ("vision_radar_X_band")
}
{"aircraft_radarX_60km"
    ("uncover" u(1))
    ("h_fov" h(120))
    ("v_fov" v(120))
    ("radius" r(41))
    ("vision_radar_X_band")
}
{"aircraft_radarL_150km"
    ("uncover" u(1))
    ("h_fov" h(210))
    ("v_fov" v(90))
    ("radius" r(129))
    ("vision_radar_L_band")
}
{"aircraft_radar_X_side"	;better on ground targets, but small
    ("uncover" u(1))
    ("h_fov" h(90))
    ("v_fov" v(90))
    ("radius" r(41))
    ("vision_radar_X_band_small")
}

{"aircraft_radarX_100km"
    ("uncover" u(1))
    ("h_fov" h(120))
    ("v_fov" v(120))
    ("radius" r(66))
    ("vision_radar_X_band")
}


{"shtora_visor"
    {uncover 10}
    {h_fov 360}
    {v_fov 90}
    ("radius" r(100))
    ("vision_shtora")
}

{"shtora90_visor"
    ("uncover" u(10))
    ("h_fov" h(100))
    ("v_fov" v(120))
    ("radius" r(100))
    ("vision_shtora")
}

{"radar_around_shorad"	;30km
    ("uncover" u(1))
    ("h_fov" h(360))
    ("v_fov" v(180))
    ("radius" r(76))
    ("vision_radar_short")
}

{"radar_pnsr5m"
    ("uncover" u(100))
    ("h_fov" h(108))
    ("v_fov" v(90))
    ("radius" r(220))
    ("vision_optic_modern_and_IR_3gen_ground_recon")
}

{"antiship_missile"
    ("radius" r(250))
    ("uncover" u(10))
    ("h_fov" h(90))
    ("v_fov" v(150))
    ("vision_antiship_missile")
}


{"antiship_missile_close"
    ("radius" r(250))
    ("uncover" u(10))
    ("h_fov" h(90))
    ("v_fov" v(150))
    ("vision_antiship_missile")
}