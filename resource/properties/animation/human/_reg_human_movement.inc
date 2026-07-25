; This file contains animations and poses for overall infantry movement when not part of vehicle crews




;==============================================================================================================================
;===================================MOCAP ANIMS================================================================================
;==============================================================================================================================




;========================================================CINEMATICS========================================================================
;==========================================================================================================================================


	{sequence "cinematics\lst_attn_01" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\lst_attn_02a" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\lst_attn_02b" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\lst_attn_03" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\lst_exhaust_01" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\lst_exhaust_02" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\lst_irrit_01" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\lst_irrit_02" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\lst_irrit_03" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\lst_neut_01" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\lst_neut_02" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\lst_neut_03" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\sit_lst_exhaust_01_sitdown" 				{smooth 0.01}{speed 1}}
	{sequence "cinematics\sit_lst_exhaust_01" 						{smooth 0.01}{speed 1}}
	{sequence "cinematics\sit_lst_neut_01" 						{smooth 0.01}{speed 1}}
	{sequence "cinematics\sit_lst_neut_01_getup" 					{smooth 0.01}{speed 1}}
	{sequence "cinematics\sit_lst_neut_01_sitdown" 				{smooth 0.01}{speed 1}}
	{sequence "cinematics\sit_lst_neut_02" 						{smooth 0.01}{speed 1}}
	{sequence "cinematics\sit_lst_neut_02_getup" 					{smooth 0.01}{speed 1}}
	{sequence "cinematics\sit_lst_neut_02_sitdown" 				{smooth 0.01}{speed 1}}
	{sequence "cinematics\sit_spk_exhaust_01" 						{smooth 0.01}{speed 1}}
	{sequence "cinematics\sit_spk_exhaust_01_getup" 				{smooth 0.01}{speed 1}}
	{sequence "cinematics\sit_spk_exhaust_01_sitdown" 				{smooth 0.01}{speed 1}}
	{sequence "cinematics\sit_spk_neut_01" 						{smooth 0.01}{speed 1}}
	{sequence "cinematics\sit_spk_neut_02" 						{smooth 0.01}{speed 1}}
	{sequence "cinematics\spk_angry_01" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\spk_angry_02" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\spk_angry_03" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\spk_calm_01" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\spk_calm_02" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\spk_calm_03" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\spk_nervous_01" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\spk_nervous_02" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\spk_nervous_03" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\spk_neut_01" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\spk_neut_02" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\spk_neut_03" 							{smooth 0.01}{speed 1}}
	{sequence "cinematics\varsity_intro_01a" 						{smooth 0.01}{speed 1}}
	{sequence "cinematics\varsity_intro_01b" 						{smooth 0.01}{speed 1}}
	{sequence "cinematics\varsity_intro_01c" 						{smooth 0.01}{speed 1}}
	{sequence "cinematics\varsity_intro_02a" 						{smooth 0.01}{speed 1}}
	{sequence "cinematics\varsity_intro_02b" 						{smooth 0.01}{speed 1}}
	{sequence "cinematics\varsity_intro_02c" 						{smooth 0.01}{speed 1}}
	{sequence "cinematics\varsity_intro_02d" 						{smooth 0.01}{speed 1}}

;==============================================================================================================================
;==============================================================================================================================
;==============================================================================================================================





;============ flinching ================
	{sequence "flinch/flinch_arm_l_01" {additive}}
	{sequence "flinch/flinch_arm_r_01" {additive}}
	{sequence "flinch/flinch_chest_01" {additive}}
	{sequence "flinch/flinch_chest_l_01" {additive}}
	{sequence "flinch/flinch_chest_r_01" {additive}}
	{sequence "flinch/flinch_head_01" {additive}}
	{sequence "flinch/flinch_leg_l_01" {additive}}
	{sequence "flinch/flinch_leg_r_01" {additive}}

	{smooth 0}
	{sequence "flinch/flinch_head_01_set" {file "flinch/flinch_head_01.anm"}}
	{smooth 0.2}




	; run normal
	{sequence "run_down" 			{file "locomotion\jog_gun_down"} 				{speed 0.55} {resume}
																						{events {11 "leg_right"} {31 "leg_left"}}}
	{sequence "run_empty_up" 		{file "locomotion\anim_run_empty_up.anm"} 		{speed 0.55} {resume}
																						{events {11 "leg_right"} {31 "leg_left"}}}
	{sequence "run_strap_up" 		{file "locomotion\anim_run_empty_up.anm"}		{speed 0.55} {resume}
																						{events {11 "leg_right"} {31 "leg_left"}}}
	{sequence "run_gun_up" 			{file "locomotion\jog_gun_up"}					{speed 0.55} {resume}
																						{events {11 "leg_left"} {31 "leg_right"}}}
	{sequence "run_flag_up" 														{speed 0.60} {resume}}
	{sequence "run_flag_down" 														{speed 0.60} {resume}}
	
	
	
;==========================================================================================================================================
;============================================DIRECT CONTROL (STANDARD)=====================================================================
; This is the STABLE version. MOCAP is disabled to prevent sliding/wandering bugs.

	{sequence "walk_normal_pistol_down"	{speed 0.27} {smooth 0.08} {resume} {events {10 "leg_right"} {22 "leg_left"} {34 "leg_right"} {47 "leg_left"} {60 "leg_right"} {72 "leg_left"}}}
	{sequence "walk_normal_pistol_up"	{speed 0.27} {smooth 0.08} {resume}}
	{sequence "walk_normal_mgun_down"	{speed 0.27} {smooth 0.08} {resume} {events {10 "leg_right"} {23 "leg_left"} {35 "leg_right"} {47 "leg_left"} {60 "leg_right"} {72 "leg_left"}}}
	{sequence "walk_normal_mgun_up"		{speed 0.27} {smooth 0.08} {resume}}
	{sequence "walk_normal_gun_down"	{speed 0.27} {smooth 0.08} {resume} {events {11 "leg_right"} {22 "leg_left"} {36 "leg_right"} {47 "leg_left"} {61 "leg_right"} {72 "leg_left"}}}
	{sequence "walk_normal_gun_up"		{speed 0.27} {smooth 0.08} {resume}}
	{sequence "walk_normal_rifle_down"	{speed 0.27} {smooth 0.08} {resume} {events {9 "leg_right"} {21 "leg_left"} {34 "leg_right"} {47 "leg_left"} {59 "leg_right"} {72 "leg_left"}}}
	{sequence "walk_normal_rifle_up"	{speed 0.27} {smooth 0.08} {resume}}
	{sequence "walk_normal_gun_down_2"	{speed 0.27} {resume} {events {7 "leg_left"} {33 "leg_right"}}}
	{sequence "walk_normal_gun_up_2"	{speed 0.27} {resume}}
	{sequence "walk_normal_gun_short_down"	{speed 0.40} {resume} {events {36 "leg_left"} {13 "leg_right"}}}
	{sequence "walk_normal_gun_short_up"	{speed 0.40} {resume}}

;=============================================DIRECT CONTROL (MOCAP - DISABLED)============================================================
; Disabled for stability. To enable, uncomment these and comment out the block above.
;	{sequence "walk_normal_pistol_down"		{file "locomotion\anim_run_dc_down.anm"}	{speed 0.55} {smooth 0.04} {resume} {events {11 "leg_right"}{27 "leg_left"}{42 "leg_right"}{59 "leg_left"}{74 "leg_right"}{88 "leg_left"}}}
;	{sequence "walk_normal_pistol_up"		{speed 0.30} {smooth 0.04} {resume}}
;	{sequence "walk_normal_mgun_down"		{file "locomotion\anim_run_empty_down.anm"}	{speed 0.55} {smooth 0.04} {resume} {events {11 "leg_right"}{27 "leg_left"}{42 "leg_right"}{59 "leg_left"}{74 "leg_right"}{88 "leg_left"}}}
;	{sequence "walk_normal_mgun_up"			{speed 0.27} {smooth 0.04} {resume}}
;	{sequence "walk_normal_gun_down"		{file "locomotion\anim_run_dc_down.anm"}	{speed 0.55} {smooth 0.04} {resume} {events {11 "leg_right"}{27 "leg_left"}{42 "leg_right"}{59 "leg_left"}{74 "leg_right"}{88 "leg_left"}}}
;	{sequence "walk_normal_gun_up"			{file "locomotion\anim_run_dc_gun_up.anm"}	{speed 0.55} {smooth 0.04} {resume}}
;	{sequence "walk_normal_rifle_down"		{file "locomotion\anim_run_dc_down.anm"}	{speed 0.55} {smooth 0.04} {resume} {events {11 "leg_right"}{27 "leg_left"}{42 "leg_right"}{59 "leg_left"}{74 "leg_right"}{88 "leg_left"}}}
;	{sequence "walk_normal_rifle_up"		{file "locomotion\anim_run_dc_gun_up.anm"}	{speed 0.55} {smooth 0.04} {resume}}
;	{sequence "walk_normal_gun_down_2"		{file "locomotion\anim_run_dc_down.anm"}	{speed 0.55} {resume} {events {11 "leg_right"}{27 "leg_left"}{42 "leg_right"}{59 "leg_left"}{74 "leg_right"}{88 "leg_left"}}}
;	{sequence "walk_normal_gun_up_2"		{file "locomotion\anim_run_dc_gun_up.anm"}	{speed 0.55} {resume}}
;	{sequence "walk_normal_gun_short_down"	{file "locomotion\anim_run_dc_down.anm"}	{speed 0.55} {resume} {events {11 "leg_right"}{27 "leg_left"}{42 "leg_right"}{59 "leg_left"}{74 "leg_right"}{88 "leg_left"}}}
;	{sequence "walk_normal_gun_short_up"	{file "locomotion\anim_run_dc_gun_up.anm"}	{speed 0.55} {resume}}
;==========================================================================================================================================


; 	run backward - for manual control
;	{sequence "run_back_down"}
;	{sequence "run_back_up"}
;	{sequence "run_strap_back_down"}
;	{sequence "run_strap_back_up"}

	; run fast
	{sequence "run_fast_down"				{file "locomotion\anim_sprint_empty_down.anm"} 	{speed 0.3}{smooth 0.2}{resume}
											{events {11 "leg_right"}{27 "leg_left"}{42 "leg_right"}
											{59 "leg_left"}{74 "leg_right"}{88 "leg_left"}}}
	{sequence "run_fast_empty_up"			{file "locomotion\anim_sprint_empty_up.anm"} 	{speed 0.3}{smooth 0.2}{resume}}
	{sequence "run_fast_gun_rhand_up"		{file "locomotion\sprint_gun_up\anim_sprint_gun_1hand_up.anm"} 				{speed 0.40}{smooth 0.2}{resume}}
	{sequence "run_fast_gun_up"				{file "locomotion\sprint_gun_up\anim_sprint_gun_1hand_up.anm"} 				{speed 0.40}{smooth 0.2}{resume}}
	{sequence "run_fast_strap_up"			{file "locomotion\anim_sprint_empty_up.anm"} 	{speed 0.3}{smooth 0.2}{resume}}
	; bow
	{sequence "run_bow_gun_down"			{file "locomotion\sprint_gun_down"} 			{speed 0.3}{smooth 0.2}{resume}
											{events {11 "leg_right"}{27 "leg_left"}{42 "leg_right"}
											{59 "leg_left"}{74 "leg_right"}{88 "leg_left"}}}

	{sequence "run_bow_gun_up"				{file "locomotion\sprint_gun_up"} 				{speed 0.3}{smooth 0.2}{resume}}
	{sequence "run_bow_gun_tocover_down"	{file "locomotion\sprint_gun_down"} 			{speed 0.3}{smooth 0.2}{resume}
											{events {11 "leg_right"}{27 "leg_left"}{42 "leg_right"}
											{59 "leg_left"}{74 "leg_right"}{88 "leg_left"}}}
	{sequence "run_bow_gun_tocover_up"		{file "locomotion\sprint_gun_up"} 				{speed 0.3}{smooth 0.2}{resume}}
	{sequence "run_bow_tocover_down"		{file "locomotion\sprint_gun_down"} 			{speed 0.3}{smooth 0.2}{resume}
											{events {11 "leg_right"}{27 "leg_left"}{42 "leg_right"}
											{59 "leg_left"}{74 "leg_right"}{88 "leg_left"}}}
	{sequence "run_bow_tocover_up"			{file "locomotion\sprint_gun_up"} 				{speed 0.3}{smooth 0.2}{resume}}
;	{sequence "run_bow_flag"				{speed 0.55} {resume}}


	{sequence "lie_from_stand" 				{file "locomotion\mantle\anim_stand_to_lie_gun_alt.anm"} 		{speed 2}{smooth 0.75}
												{events {12 "stand_to_prone"}}}
	{sequence "lie_from_stand_1" 			{file "locomotion\mantle\anim_stand_to_lie_gun_alt.anm"} 		{speed 2}{smooth 0.75}
												{events {12 "prone_to_stand"}}}
	{sequence "lie_from_squat_empty" 		{file "locomotion\mantle\anim_crouch_to_lie_empty.anm"} 	{speed 2}{smooth 0.75}
												{events {32 "prone_to_stand"}}}
	{sequence "lie_from_squat_gun" 			{file "locomotion\mantle\anim_crouch_to_lie_gun.anm"} 		{speed 2.5}{smooth 0.75}
												{events {0 "stand_to_prone"}}}
	{sequence "lie_from_squat_mgun" 		{file "locomotion\mantle\anim_crouch_to_lie_gun.anm"} 		{speed 2.5}{smooth 0.75}
												{events {0 "stand_to_prone"}}}
	{sequence "lie_from_squat_bazooka" 		{file "locomotion\mantle\anim_crouch_to_lie_gun.anm"} 		{speed 2.5}{smooth 0.75}
												{events {0 "stand_to_prone"}}}
	{sequence "lie_from_squat_pistol" 		{file "locomotion\mantle\anim_crouch_to_lie_empty.anm"} 	{speed 2.5}{smooth 0.75}
												{events {1 "stand_to_prone"}}}
	{sequence "squat_from_stand_empty" 		{file "locomotion\mantle\anim_stand_to_crouch_empty.anm"} 	{speed 2.5}{smooth 0.75}
												{events {12 "crouch"}}}
	{sequence "squat_from_stand_gun" 		{file "locomotion\mantle\anim_stand_to_crouch_gun.anm"} 	{speed 2.5}{smooth 0.75}
												{events {12 "crouch"}}}
	{sequence "squat_from_stand_bazooka"	{file "locomotion\mantle\anim_stand_to_crouch_gun.anm"} 	{speed 2.5}{smooth 0.75}
												{events {12 "crouch"}}}
												
												
												
	{sequence "stand_idle_1hand"			{file "idle\empty\idle_empty_01.anm"} 	{speed 0.8}{smooth 0.1}{resume}}
	{sequence "stand_idle_empty_1" 			{file "idle\empty\idle_empty_02.anm"} 	{speed 0.8}{smooth 0.2}}
	{sequence "stand_idle_empty_2" 			{file "idle\empty\idle_empty_03.anm"} 	{speed 0.8}{smooth 0.2}}
	{sequence "stand_idle_empty_3" 			{file "idle\empty\idle_empty_02.anm"} 	{speed 0.8}{smooth 0.2}}
	{sequence "stand_idle_empty_4" 			{file "idle\empty\idle_empty_03.anm"} 	{speed 0.8}{smooth 0.2}}
	{sequence "stand_idle_strap_1" 			{file "idle\empty\idle_empty_02.anm"} 	{speed 0.5}{smooth 0.2}}
	{sequence "stand_idle_strap_2" 			{file "idle\empty\idle_empty_03.anm"} 	{speed 0.8}{smooth 0.2}}
	{sequence "stand_idle_strap_3" 			{file "idle\empty\idle_empty_01.anm"} 	{speed 0.8}{smooth 0.2}}
	{sequence "stand_look_around_gun"		{file "idle\gun"} 	{speed 1}{smooth 0.05}{resume}}
	{sequence "stand_look_around_gun_1"		{file "idle\gun"} 	{speed 1}{smooth 0.05}{resume} {events {145 "rest_gun"} {759 "rest_gun"}}}
	{sequence "stand_look_around_gun_2"		{file "idle\gun"} 	{speed 1}{smooth 0.05}{resume}}

	{sequence "crawl" 						{file "locomotion\anim_lie_crawl_empty.anm"}{resume} {speed 1.4} {events {10 "leg_left"} {32 "leg_right"}}}
	{sequence "crawl_gun" 					{file "locomotion\lie_crawl_gun"}{resume} {speed 1.4} {events {10 "leg_left"} {32 "leg_right"}}}


	;--- crouching idles ---
	{sequence "idle_crouch_empty"		{file "idle\empty_crouch"}	{speed 0.8}{smooth 0.2}}
	{sequence "idle_crouch_gun"			{file "idle\gun_crouch"}	{speed 0.8}{smooth 0.2}}

	;--- lying idles ---
	{sequence "idle_lie_empty"			{file "idle_lie_empty"}		{speed 1.5}{smooth 0.2}}
	{sequence "idle_lie_gun"			{file "idle_lie_gun"}		{speed 1.5}{smooth 0.2}}














;==============================================================================================================================
;==============================================================================================================================
;==============================================================================================================================














;=================paratroopers begin==============================

	{sequence "para_drop_idle_1" {file "paratroopers\para_drop_idle_1.anm"} {speed 0.7} {smooth 0}}
	{sequence "para_drop_idle_2" {file "paratroopers\para_drop_idle_2.anm"} {speed 0.7} {smooth 0}}
	{sequence "para_drop_idle_3" {file "paratroopers\para_drop_idle_3.anm"} {speed 0.7} {smooth 0}}

	{sequence "paratroopers/para_dead_1"		{speed 0.7} {smooth 0}}
	{sequence "paratroopers/para_dead_2"		{speed 0.7} {smooth 0}}


;=================paratroopers end==============================


;======================Supplement for animations to work properly==============

	{sequence "fingers/fingers_smg_l"}
	{sequence "fingers/fingers_smg_r"}
	{sequence "fingers/fingers_bare_l"}
	{sequence "fingers/fingers_bare_r"}
	{sequence "fingers/fingers_box_l"}
	{sequence "fingers/fingers_box_r"}
	{sequence "fingers/fingers_item_l"}
	{sequence "fingers/fingers_item_r"}
	{sequence "fingers/fingers_pistol_l"}
	{sequence "fingers/fingers_pistol_r"}
	{sequence "fingers/fingers_rifle_l"}
	{sequence "fingers/fingers_rifle_r"}


;=================BOLTING Animations==============================

;=================LIE Bolt==========

	{sequence "lie_bolt_rifle"			{file "weapon\anim_rifle_lie_bolt_01.anm"}						{speed 0.8}{smooth 0.15}{events {7 "open_bolt"}{22 "close_bolt"}}}
	{sequence "lie_bolt_rifle_open"		{file "weapon\anim_rifle_lie_bolt_open_01.anm"}					{speed 0.8}{smooth 0.15}{events {7 "open_bolt"}}}
	{sequence "lie_bolt_rifle_close"	{file "weapon\anim_rifle_lie_bolt_close_01.anm"}				{speed 0.8}{smooth 0.15}{events {9 "close_bolt"}}}

;=================STAND Bolt==========

	{sequence "stand_bolt_rifle_up"			{file "weapon\anim_bolt_rifle_stand_bolt_up.anm"}			{speed 1.5}{smooth 0.05}{events {10 "open_bolt"} {35 "close_bolt"}}}
	{sequence "stand_bolt_rifle_down"		{file "weapon\anim_bolt_rifle_stand_bolt_down.anm"}			{speed 1.5}{smooth 0.05}}
	{sequence "stand_bolt_rifle_open_up"	{file "weapon\anim_bolt_rifle_stand_bolt_open_up.anm"}		{speed 1.5}{smooth 0.05}{events {10 "open_bolt"}}}
	{sequence "stand_bolt_rifle_open_down"	{file "weapon\anim_bolt_rifle_stand_bolt_open_down.anm"}	{speed 1.5}{smooth 0.05}}
	{sequence "stand_bolt_rifle_close_up"	{file "weapon\anim_bolt_rifle_stand_bolt_close_up.anm"}		{speed 1.5}{smooth 0.05}}
	{sequence "stand_bolt_rifle_close_down"	{file "weapon\anim_bolt_rifle_stand_bolt_close_down.anm"}	{speed 1.5}{smooth 0.05}}

;=================SQUAT Bolt==========

	{sequence "squat_bolt_rifle"		{file "weapon\anim_bolt_rifle_crouch_bolt_all.anm"}{speed 1.35} {smooth 0.15} {events {10 "open_bolt"} {35 "close_bolt"}}}
	{sequence "squat_bolt_rifle_open"	{file "weapon\anim_bolt_rifle_crouch_bolt_open_all.anm"}{speed 0.9} {smooth 0}}
	{sequence "squat_bolt_rifle_close"	{file "weapon\anim_bolt_rifle_crouch_bolt_close_all.anm"}{speed 0.9} {smooth 0}}

;=================experiments begin==============================
	{smooth 0.5}

	{sequence "squat_death_TEST"	{speed 1} {smooth 0}}
	{sequence "crouch_cover"		{speed 1} {smooth 0}}
	{sequence "stand_idle_02"		{speed 1} {smooth 0}}
;	{sequence "stand_die_03"		{movement} {speed 1} {smooth 0.1} {events {16 "body_fall_heavy"}}}
;	{sequence "stand_die_04"		{movement} {speed 0.7} {smooth 0.1} {events {20 "body_fall_heavy"} {43 "death_breathe"}}}
;	{sequence "stand_die_05"		{movement} {speed 0.65} {smooth 0.1} {events {18 "body_fall_heavy"}}}
;	{sequence "stand_die_06"		{movement} {speed 1} {smooth 0.1} {events {26 "body_fall_light"}}}
	{sequence "stand_idle_gun_01"	{speed 1} {smooth 0}}
	{sequence "dig_pickaxe_01"		{speed 1} {smooth 0} {events {28 "leg_left"} {35 "leg_right"} {38 "pickaxe"}}}
	{sequence "squat_dig_01"		{speed 1} {smooth 0} {events {17 "pickaxe"}}}
	{sequence "stand_dig_01"		{speed 1} {smooth 0} {events {27 "leg_left"} {32 "shovel"}}}
;	{sequence "stand_die_back_5"	{movement} {speed 0.65} {smooth 0.1} {events {22 "body_fall_light"} {41 "death_breathe"}}}

	{sequence "squat_construct_sandbag"		{speed 0.8} {smooth 0.3} {events {48 "construct_sandbag"}}}
	{sequence "squat_construct_mine"		{speed 0.8} {smooth 0.3} {events {46 "construct_mine"}}}

	{sequence "squat_medic_revive"		{speed 0.5} {smooth 0.07}}
	{sequence "lie_wounded"		{speed 0.5} {smooth 0.3}}
	{sequence "lie_wounded_1"		{speed 0.5} {smooth 0.3}}
	{sequence "lie_wounded_2"		{speed 0.5} {smooth 0.3}}

	{sequence "stand_at_attention"		{speed 0.6} {smooth 0.1} {events {1 "stand_at_attention"}}}

;=================surrender begin================================

	{sequence "stand_surrender"			{speed 1} {smooth 0.7}}
	{sequence "stand_surrender_idle"	{speed 1} {smooth 0.7}}
	{sequence "stand_handcuff"			{speed 1} {smooth 0.7}}
	{sequence "stand_handcuff_idle"		{speed 1} {smooth 0.7}}
	{sequence "stand_handcuff_walk"		{speed 1} {smooth 0.7}}


;=================Wire Cutting==========

	{sequence "stand_cut_wire"	{speed 0.65} {smooth 0}}

;=================Wire Cutting==========


;=================POSES begin==============================

	{smooth 0.5}
;	{blend "pose_die_water" 		{pose <now uses no pose> end}}
	{blend "pose_lie_empty" 		{pose "weapon\anim_item_lie_take_end_01" end}}
	{blend "pose_lie_pistol"		{pose "weapon\anim_pistol_lie_shoot_01" end}}
	{blend "pose_lie_gun" 			{pose "weapon\anim_smg_lie_shoot_01" end}}
	{blend "pose_lie_mgun" 			{pose "weapon\anim_mg_lie_shoot_01" begin}}
	{blend "pose_lie_bazooka"		{pose "lie_reload_bazooka" end}}

	{blend "pose_snipe_pistol" 		{pose "stand_fire_pistol_up_1" end}}
	{blend "pose_snipe_gun" 		{pose "weapon\anim_pose_stand_rifle" begin}}

	{blend "pose_stand_pistol_up" 	{pose "stand_take_pistol_up_end" end}}
	{blend "pose_stand_empty_1" 	{pose "stand_idle_empty_1" begin}}
	{blend "pose_stand_strap_1" 	{pose "weapon\anim_stand_take_empty_start_01" begin}}
	{blend "pose_stand_bazooka" 	{pose "weapon\anim_bazooka_stand_shoot_all" begin}}
	{blend "pose_stand_1hand" 		{pose "walk_1hand" begin}}
	{blend "pose_stand_2hand" 		{pose "walk_2hand" begin}}
	{blend "pose_stand_corpse" 		{pose "stand_drop_corpse" begin}}
	{blend "pose_stand_flag" 		{pose "stand_flag" end}}
	{blend "pose_stand_down" 		{pose "stand_down" begin}}
	{blend "pose_stand_gun_down" 	{pose "stand_take_gun_down" end}}

	{blend "pose_squat_empty" 		{pose "locomotion\mantle\anim_stand_to_crouch_empty" end}}
	{blend "pose_squat_bazooka" 	{pose "squat_take_bazooka" end}}
	{blend "pose_stand_giveup_1" 	{pose "stand_giveup_1" end}}
	{blend "pose_stand_giveup_2" 	{pose "stand_giveup_2" end}}

	{blend "pose_watch_binocular" 		{pose "stand_watch_binocular" begin}}
	{blend "pose_squat_watch_binocular" {pose "squat_watch_binocular" begin}}
	{blend "pose_lie_watch_binocular" 	{pose "squat_watch_binocular" end}}

	; new poses from fire animations

	{blend "pose_sit_chair"					{pose "pose_sit_chair"  begin}}
	{blend "pose_stand_look_around_gun"		{pose "stand_look_around_gun"  end}}
	{blend "pose_stand_look_around_gun_1"	{pose "pose_stand_gun_aside_1_2"  end}}
	{blend "pose_stand_gun_aside"			{pose "pose_stand_gun_aside"  begin}}
	{blend "pose_stand_gun_aside_1"   	    {pose "pose_stand_gun_aside_1_2"  begin}}
	{blend "pose_stand_gun_aside_2"     	{pose "pose_stand_gun_aside_1_2"  end}}

	{blend "pose_stand_fire_gun_up_1"		{pose "weapon\anim_smg_stand_shoot_01_up"   begin}}
	{blend "pose_stand_fire_gun_down_1"		{pose "weapon\anim_smg_stand_shoot_01_down" begin}}
	{blend "pose_stand_fire_gun_up"    		{pose "weapon\anim_smg_stand_shoot_01_up"     begin}}
	{blend "pose_stand_fire_gun_down"   	{pose "weapon\anim_smg_stand_shoot_01_down"   begin}}

;	{blend "pose_stand_fire_gun_short_up"	{pose "weapon\anim_smg_stand_shoot_01_up" begin}}
	{blend "pose_stand_fire_gun_short_down"	{pose "weapon\anim_smg_stand_shoot_01_down" begin}}

	{blend "pose_stand_fire_rifle_up"			{pose "weapon\anim_rifle_stand_shoot_01_up" begin}}
	{blend "pose_stand_fire_rifle_down"			{pose "weapon\anim_rifle_stand_shoot_01_down" begin}}
	{blend "pose_stand_assault-fire_rifle_down"	{pose "weapon\anim_smg_stand_shoot_01_down" begin}}
	{blend "pose_stand_assault-fire_rifle_up"	{pose "weapon\anim_smg_stand_shoot_01_up" begin}}
	{blend "pose_stand_assault-fire_gun_down"	{pose "weapon\anim_smg_stand_shoot_01_down" begin}}
	{blend "pose_stand_assault-fire_gun_up"		{pose "weapon\anim_smg_stand_shoot_01_up" begin}}
	{blend "pose_stand_assault-fire_gun_down_1"	{pose "weapon\anim_smg_stand_shoot_01_down" begin}}
	{blend "pose_stand_assault-fire_gun_up_1"	{pose "weapon\anim_smg_stand_shoot_01_up" begin}}
	{blend "pose_stand_fire_mgun_down"			{pose "weapon\anim_mg_stand_shoot_01_down" begin}}
	{blend "pose_stand_fire_mgun_down_1"		{pose "weapon\anim_mg_stand_shoot_01_down" begin}}
	{blend "pose_stand_fire_mgun_up"			{pose "weapon\anim_mg_stand_shoot_01_up" begin}}
	{blend "pose_stand_fire_mgun_up_1"			{pose "weapon\anim_mg_stand_shoot_01_up" begin}}

	{blend "pose_stand_fire_pistol_down" 	{pose "weapon\anim_pistol_stand_shoot_01_down"	begin}}
	{blend "pose_stand_fire_pistol_up"   	{pose "weapon\anim_pistol_stand_shoot_01_up" 	begin}}
	{blend "pose_stand_fire_pistol_down_1" 	{pose "weapon\anim_pistol_stand_shoot_01_down"	begin}}
	{blend "pose_stand_fire_pistol_up_1"   	{pose "weapon\anim_pistol_stand_shoot_01_up" 	begin}}
	{blend "pose_stand_fire_pistol_down_2" 	{pose "weapon\anim_pistol_stand_shoot_01_down"	begin}}
	{blend "pose_stand_fire_pistol_up_2"   	{pose "weapon\anim_pistol_stand_shoot_01_up" 	begin}}

	{blend "pose_squat_assault-fire_gun"	{pose "weapon\anim_smg_crouch_shoot_01"		begin}}
	{blend "pose_squat_assault-fire_rifle"	{pose "weapon\anim_rifle_crouch_shoot_01"	begin}}
	{blend "pose_squat_fire_gun"			{pose "weapon\anim_smg_crouch_shoot_01"		begin}}
	{blend "pose_squat_fire_gun_short"		{pose "weapon\anim_smg_crouch_shoot_01" 		begin}}
	{blend "pose_squat_fire_rifle"			{pose "weapon\anim_rifle_crouch_shoot_01"	begin}}
	{blend "pose_squat_fire_pistol"			{pose "squat_fire_pistol"						begin}}
	{blend "pose_squat_fire_pistol_1"		{pose "squat_fire_pistol_1"						begin}}

	{blend "pose_stand_fire_gun_up_old"		{pose "weapon\anim_smg_stand_shoot_01_up" begin}}
	{blend "pose_stand_fire_mgun_up_old"	{pose "weapon\anim_mg_stand_shoot_01_up" begin}}

	{blend "pose_corpse" 					{pose "corpse_move" begin}}
	{blend "pose_test" 						{pose "pose_test" begin}}

;==================POSES end ==============================

;=================IDLE ANIMATIONS (of any stance) begin ===


	{sequence "stand_smoke_1"}
	{sequence "stand_smoke_2"}
	{sequence "stand_smoke_3"}
	{sequence "stand_climb_stairs"}
	{sequence "stand_upstairs"}
	{smooth 0.1}
	{sequence "stand_clime_up_rope_begin" {speed 0.5}}
	{sequence "stand_clime_up_rope_end" {speed 0.5}}
	{sequence "stand_move_up_rope" {speed 0.5}}
	{sequence "stand_giveup_1" {speed 1}}
	{sequence "stand_giveup_2" {speed 1}}



	; cinematic-only animations begin
	{sequence "cinematics\cin_idle_01"}
	{sequence "cinematics\cin_idle_02"}
	{sequence "cinematics\cin_idle_02_lean"}
	{sequence "cinematics\cin_idle_sit_01"}
	{sequence "cinematics\cin_idle_sit_02"}
	{sequence "cinematics\cin_idle_sit_03"}
	{sequence "cinematics\cin_idle_sit_crate_01"}
	{sequence "cinematics\cin_piano_play" {events {10 "play_piano"}}}
	
	{sequence "cinematics\idle_attention"}
	{sequence "cinematics\idle_to_attention"}
	{sequence "cinematics\idle_attention_salute"}
	{sequence "cinematics\idle_attention_eyes_left"}
	{sequence "cinematics\idle_attention_eyes_right"}

	; mission-only gamedesign idle animations begin
	{sequence "idle_mission\idle_passive_squat_ground_gun_strap_simple1"}
	{sequence "idle_mission\idle_passive_squat_ground_gun_strap_scratch"}
	{sequence "idle_mission\idle_passive_squat_ground_gun_strap_simple2"}
	{sequence "idle_mission\idle_passive_squat_ground_gun_strap_simple3"}
	{sequence "idle_mission\idle_passive_squat_ground_gun_strap_sleep"}
	{sequence "idle_mission\idle_passive_squat_ground_gun_strap_smoke"}
	{sequence "idle_mission\idle_passive_squat_gun_aside_drop"}
	{sequence "idle_mission\idle_passive_squat_gun_aside_fals_alarm"}
	{sequence "idle_mission\idle_passive_squat_gun_aside_ground_test"}
	{sequence "idle_mission\idle_passive_squat_gun_aside_gun_test1"}
	{sequence "idle_mission\idle_passive_squat_gun_aside_gun_test2"}
	{sequence "idle_mission\idle_passive_squat_gun_aside_look_around"}
	{sequence "idle_mission\idle_passive_squat_wall_gun_aside_simple1"}
	{sequence "idle_mission\idle_passive_squat_wall_gun_aside_simple2"}
	{sequence "idle_mission\idle_passive_squat_wall_gun_aside_sleep"}
	{sequence "idle_mission\idle_passive_stand_gun_strap_gun_test"}
	{sequence "idle_mission\idle_passive_stand_gun_strap_lace"}
	{sequence "idle_mission\idle_passive_stand_gun_strap_self_clean"}
	{sequence "idle_mission\idle_passive_stand_gun_strap_simple_long"}
	{sequence "idle_mission\idle_passive_stand_gun_strap_simple3"}
	{sequence "idle_mission\idle_squat_from_stand_ground_gun_strap"}
	{sequence "idle_mission\idle_squat_from_stand_gun_aside"}
	{sequence "idle_mission\idle_squat_from_stand_wall_gun_aside"}
	{sequence "idle_mission\idle_stand_from_squat_ground_gun_strap"}
	{sequence "idle_mission\idle_stand_gun_aside_test"}
	{sequence "idle_mission\idle_stand_listen_gun_aside_1"}
	{sequence "idle_mission\idle_stand_listen_gun_aside_2"}
	{sequence "idle_mission\idle_stand_listen_gun_aside_3"}
	{sequence "idle_mission\idle_stand_listen_gun_aside_4"}
	{sequence "idle_mission\idle_stand_listen_gun_strap_1"}
	{sequence "idle_mission\idle_stand_listen_gun_strap_2"}
	{sequence "idle_mission\idle_stand_listen_gun_strap_3"}
	{sequence "idle_mission\idle_stand_listen_gun_strap_4"}
	{sequence "idle_mission\idle_stand_listen_gun_strap_5"}
	{sequence "idle_mission\idle_stand_listen_gun_strap_6"}
	{sequence "idle_mission\idle_stand_talk_gun_aside_1"}
	{sequence "idle_mission\idle_stand_talk_gun_aside_2"}
	{sequence "idle_mission\idle_passive_squat_ground_gun_strap_ground_test"}



	{sequence "idle_mission\idle_passive_stand_gun_strap_warming"}
	{sequence "idle_mission\idle_passive_stand_gun_strap_watch_test"}
	{sequence "idle_mission\idle_passive_stand_wall_gun_aside_1"}
	{sequence "idle_mission\idle_passive_stand_wall_gun_aside_2"}
	{sequence "idle_mission\idle_stand_talk_gun_aside_3"}
	{sequence "idle_mission\idle_stand_talk_gun_aside_4"}
	{sequence "idle_mission\idle_stand_talk_gun_aside_5"}
	{sequence "idle_mission\idle_stand_talk_gun_aside_6"}
	{sequence "idle_mission\idle_stand_talk_gun_strap_1"}
	{sequence "idle_mission\idle_stand_talk_gun_strap_2"}
	{sequence "idle_mission\idle_stand_talk_gun_strap_3"}
	{sequence "idle_mission\idle_stand_talk_gun_strap_4"}
	{sequence "idle_mission\idle_walk_listen_left_gun_strap_1"}
	{sequence "idle_mission\idle_walk_listen_left_gun_strap_2"}
	{sequence "idle_mission\idle_walk_listen_left_gun_strap_3"}
	{sequence "idle_mission\idle_walk_listen_left_gun_strap_4"}
	{sequence "idle_mission\idle_walk_listen_right_gun_strap_1"}
	{sequence "idle_mission\idle_walk_listen_right_gun_strap_2"}
	{sequence "idle_mission\idle_walk_listen_right_gun_strap_3"}
	{sequence "idle_mission\idle_walk_listen_right_gun_strap_4"}
	{sequence "idle_mission\idle_walk_talk_left_gun_strap_1"}
	{sequence "idle_mission\idle_walk_talk_left_gun_strap_2"}
	{sequence "idle_mission\idle_walk_talk_left_gun_strap_3"}
	{sequence "idle_mission\idle_walk_talk_left_gun_strap_4"}
	{sequence "idle_mission\idle_walk_talk_right_gun_strap_1"}
	{sequence "idle_mission\idle_walk_talk_right_gun_strap_2"}
	{sequence "idle_mission\idle_walk_talk_right_gun_strap_3"}
	{sequence "idle_mission\idle_walk_talk_right_gun_strap_4"}

	{sequence "lie_sleep_idle_1" {smooth 0.1}}
	{sequence "lie_sleep_idle_2" {smooth 0.1}}
	{sequence "idle_passive_squat_ground_gun_strap_sleep" {smooth 0.1}}

	; mission-only gamedesign idle animations end

;==================IDLE ANIMATIONS end ==============================

;================= STANDING UP ANIMATIONS begin ===========


	{sequence "stand_assault-fire_gun_up"		{file "weapon\anim_smg_stand_shoot_01_up.anm"}	{speed 4}}
	{sequence "stand_assault-fire_gun_down"		{file "weapon\anim_smg_stand_shoot_01_down.anm"}{speed 4}}
	{sequence "stand_assault-fire_gun_up_1"		{file "weapon\anim_smg_stand_shoot_01_up.anm"}	{speed 4}}
	{sequence "stand_assault-fire_gun_down_1"	{file "weapon\anim_smg_stand_shoot_01_down.anm"}{speed 4}}
	{sequence "stand_assault-fire_rifle_up"		{file "weapon\anim_smg_stand_shoot_01_up.anm"}	{speed 2}}
	{sequence "stand_assault-fire_rifle_down"	{file "weapon\anim_smg_stand_shoot_01_down.anm"}{speed 2}}

	{sequence "stand_fire_gun_down_1" 		{file "weapon\anim_smg_stand_shoot_01_down.anm"}	{speed 1}{smooth 0.1}}
	{sequence "stand_fire_gun_down"  		{file "weapon\anim_smg_stand_shoot_01_down.anm"}	{resume}{speed 1}{smooth 0.1}}
	{sequence "stand_fire_gun_up"  			{file "weapon\anim_smg_stand_shoot_01_up.anm"}		{resume}{speed 1}{smooth 0.1}}
	{sequence "stand_fire_gun_up_1" 		{file "weapon\anim_smg_stand_shoot_01_up.anm"}		{resume}{speed 1}{smooth 0.1}}
	{sequence "stand_fire_gun_up_old" 		{file "weapon\anim_smg_stand_shoot_01_up.anm"}		{resume}{speed 1}}
	{sequence "stand_fire_mgun_up_old" 		{file "weapon\anim_mg_stand_shoot_01_up.anm"}		{resume}{speed 1}}
	{sequence "stand_fire_rifle_up"			{file "weapon\anim_rifle_stand_shoot_01_up.anm"}	{resume}{speed 2}} ;// if too slow, semi autos won't play animation everytime
	{sequence "stand_fire_rifle_down"		{file "weapon\anim_rifle_stand_shoot_01_down.anm"}	{resume}{speed 2}} ;// if too slow, semi autos won't play animation everytime
	{sequence "stand_fire_gun_short_down"	{file "weapon\anim_smg_stand_shoot_01_down.anm"}	{speed 1}}
	{sequence "stand_fire_gun_short_up"		{file "weapon\anim_smg_stand_shoot_01_up.anm"}		{speed 1}}
	{sequence "stand_fire_mgun_down"  		{file "weapon\anim_mg_stand_shoot_01_down.anm"}		{speed 1}}
	{sequence "stand_fire_mgun_down_1"  	{file "weapon\anim_mg_stand_shoot_01_down.anm"}		{speed 4}}
	{sequence "stand_fire_mgun_up"			{file "weapon\anim_mg_stand_shoot_01_up.anm"}		{speed 1}}
	{sequence "stand_fire_mgun_up_1" 		{file "weapon\anim_mg_stand_shoot_01_up.anm"}		{speed 4}}

	{sequence "stand_fire_pistol" 			{file "weapon\anim_pistol_stand_shoot_01_up.anm"}	{speed 2.1}}
	{sequence "stand_fire_bazooka" 			{file "weapon\anim_bazooka_stand_shoot_all.anm"}	{speed 1}}

	{sequence "stand_fire_pistol_down"		{file "weapon\anim_pistol_stand_shoot_01_down.anm"}	{speed 1}{events {1 "prone_to_stand"}}}
	{sequence "stand_fire_pistol_up"		{file "weapon\anim_pistol_stand_shoot_01_up.anm"}	{speed 1} {events {1 "stand_to_prone"}}}
	{sequence "stand_fire_pistol_down_1"	{file "weapon\anim_pistol_stand_shoot_02_down.anm"}}
	{sequence "stand_fire_pistol_down_2"	{file "weapon\anim_pistol_stand_shoot_01_down.anm"}}
	{sequence "stand_fire_pistol_up_1"		{file "weapon\anim_pistol_stand_shoot_02_up.anm"}}
	{sequence "stand_fire_pistol_up_2"		{file "weapon\anim_pistol_stand_shoot_01_up.anm"}}

	{sequence "stand_from_lie_back"  		{speed 0.5}  {smooth 0.2}}
	{sequence "stand_from_lie_back_1" 		{speed 0.5}  {smooth 0.2}}
	{sequence "stand_down"}
	{sequence "stand_put_small" 			{file "weapon\anim_stand_take_empty_start_01.anm"}		{speed 1.33}{smooth 0.1}}
	{sequence "stand_take_small" 			{file "weapon\anim_stand_take_empty_end_01.anm"}		{speed 1.33}{smooth 0.1} {events {1 "pick_up_small"}}}
	{sequence "stand_take_gun_down" 		{file "weapon\stand_take_gun_down"}						{speed 1}{smooth 0.2}}
	{sequence "stand_take_gun_up" 			{file "weapon\stand_take_gun_up"}						{speed 1}{smooth 0.2}}
	{sequence "stand_take_mgun_up"			{file "weapon\stand_take_gun_up"}						{speed 1}{smooth 0.2}}
	{sequence "stand_take_mgun_down"		{file "weapon\stand_take_gun_down"}						{speed 1}{smooth 0.2}}
	{sequence "stand_take_pistol_up_begin" 	{file "weapon\anim_pistol_stand_equip_begin_up.anm"}	{speed 1.2}}
	{sequence "stand_take_pistol_up_end" 	{file "weapon\anim_pistol_stand_equip_end_up.anm"}		{speed 1.2}}
	{sequence "stand_take_bazooka" 			{file "weapon\anim_bazooka_stand_take_01.anm"}			{speed 1}}
	{sequence "stand_reload_piat" 			{speed 1}}
	{sequence "stand_reload_mgun" 			{speed 0.8}} ;{file "weapon\anim_mg_stand_reload_up.anm"} @note: commented out because of diff wep speeds.
	{sequence "stand_reload_gun" 			{file "weapon\anim_smg_stand_reload_up.anm"}{speed 1} {smooth 0.2} {events {1 "open_bolt"} {2 "close_bolt"}}}
	{sequence "stand_reload_pistol"			{file "weapon\anim_pistol_stand_reload_01_up.anm"}{speed 1} {smooth 0.2}}
	{sequence "stand_reload_bazooka" 		{speed 0.35}}
	{sequence "stand_pick_2hand_begin" 		{speed 0.35}}
	{sequence "stand_pick_2hand_end" 		{speed 0.7}}
	{sequence "stand_throw-off_tarpaulin" 	{speed 0.3} {file "stand_pick_2hand_end.anm"}}
	{sequence "stand_pick_small_begin" 		{file "weapon\anim_stand_take_ground_start_01.anm"}{speed 1.1} {events {1 "pick_up_small"}}}
	{sequence "stand_pick_small_end" 		{file "weapon\anim_stand_take_ground_end_01.anm"}{speed 1.25}}
	{sequence "stand_holdback_gun_up"				{file "weapon\anim_rifle_stand_holdback_01_up.anm"}		{speed 1}{smooth 0.2}}
	{sequence "stand_holdback_mgun_up"				{file "weapon\anim_rifle_stand_holdback_01_up.anm"}		{speed 1}{smooth 0.2}}
	{sequence "stand_holdback_pistol_up"			{file "weapon\pistol_holdback_up"}						{speed 1}{smooth 0.2}}
	{sequence "stand_holdback_gun_down"				{file "weapon\anim_rifle_stand_holdback_01_down.anm"}	{speed 1}{smooth 0.2}}
	{sequence "stand_holdback_mgun_down"			{file "weapon\anim_rifle_stand_holdback_01_down.anm"}	{speed 1}{smooth 0.2}}
	{sequence "stand_holdback_pistol_down"			{file "weapon\pistol_holdback_down"}					{speed 1}{smooth 0.2}}
	{sequence "stand_flag"}
	{sequence "stand_ring_gong_up"}
	{sequence "stand_pourin_barrel"}
	{sequence "stand_pourin_can"}
	{sequence "stand_pourout_barrel"}
	{sequence "stand_pourout_can"}
	{sequence "stand_turn_railswitch" {movement} {smooth 0}}
	{sequence "stand_dig_trench" {speed 0.75} {events {1 "crouch"} {6 "shovel"}}}

	{sequence "stand_drop_corpse"}
	{sequence "stand_lift_corpse"}

	{sequence "corpse_drop" {smooth 0} {dynamicSpeed {102 103 1000}}}
	{sequence "corpse_lift"}

	{sequence "getover_middle" {movement} {speed 0.6} {smooth 0.2} {events {3 "crouch"} {7 "leg_right"} {21 "crouch"} {22 "leg_left"} {29 "leg_right"}}}      ; JUMPING FENCE

	{smooth 0.025}
	;new mapped to gun
	{sequence "stand_reload_rifle"			{file "stand_reload_rifle.anm"} {speed 2} {events {4 "open_bolt"} {32 "close_bolt"}}}
	{sequence "stand_holdback_rifle_up"		{file "stand_holdback_gun_up.anm"}{speed 2}}
	{sequence "stand_holdback_rifle_down" 	{file "stand_holdback_gun_down.anm"}{speed 2}}
	{sequence "stand_take_rifle_up"			{file "stand_take_gun_up.anm"}{speed 2}}
	{sequence "stand_holdback_bazooka_up"	{file "stand_holdback_gun_up.anm"}{speed 2}}
	{sequence "stand_holdback_bazooka_down" {file "stand_holdback_gun_down.anm"}{speed 2}}

;====================== FIRING ANIMATIONS end ==========================

;================= SQUATING ANIMATIONS begin ==============

	{smooth 0.5}

	{sequence "squat_hide"						{file "cover_crouch"}{speed 1.5}}
	{sequence "squat_take_gun" 					{file "weapon\anim_crouch_take_gun_01.anm"}{speed 1.35}}
	{sequence "squat_take_pistol"				{speed 0.6}}
	{sequence "squat_take_small" 				{file "weapon\anim_crouch_take_empty_end_01.anm"}{speed 1.1}{events {1 "pick_up_small"}}}
	{sequence "squat_put_small" 				{file "weapon\anim_crouch_take_empty_start_01.anm"}{speed 1.1}{events {1 "pick_up_small"}}}
	{sequence "squat_reload_gun" 				{file "weapon\anim_smg_crouch_reload.anm"}{speed 2.3} {smooth 0.1}}
	{sequence "squat_reload_piat" 				{speed 1}}
	{sequence "squat_reload_pistol"				{file "weapon\anim_pistol_crouch_reload_01_all.anm"}{speed 1}}
	{sequence "squat_take_bazooka" 				{speed 0.6}}
	{sequence "squat_reload_bazooka" 			{speed 0.35}}
	{sequence "squat_throw_small_begin"}
	{sequence "squat_throw_small_end"}
	{sequence "squat_throw_grenade_begin" 			{file "throw\anim_crouch_throw_begin.anm"}	{speed 1.9} {events {40 "arm_right"}}}
	{sequence "squat_throw_grenade_end" 			{file "throw\anim_crouch_throw_end.anm"}	{speed 1.5}}
	{sequence "squat_throw_grenade_1_begin" 		{file "throw\anim_crouch_throw_begin.anm"}	{speed 1.9} {events {50 "arm_right"}}}
	{sequence "squat_throw_grenade_1_end"   		{file "throw\anim_crouch_throw_end.anm"}	{speed 1.5}}
	{sequence "squat_throw_knife_begin" {speed 1.4}}
	{sequence "squat_throw_knife_end"   {speed 1.2}}
	{sequence "squat_repair_1" {speed 0.65} {smooth 0.1} {events {18 "toolbox_open"} {40 "tool_equip"} {83 "wrench_turn"} {100 "wrench_turn"} {117 "wrench_turn"} {135 "wrench_turn"} {151 "wrench_turn"} {168 "wrench_turn"} {196 "toolbox_close"}}}
	{sequence "squat_repair_2" {speed 0.45} {smooth 0.1} {events {25 "toolbox_open"} {58 "tool_equip"} {82 "screw_twist"} {93 "screw_twist"} {105 "screw_twist"} {118 "screw_twist"} {133 "screw_twist"} {161 "screw_twist"} {175 "screw_twist"} {187 "screw_twist"} {198 "screw_twist"} {226 "screw_twist"} {238 "screw_twist"} {251 "screw_twist"} {270 "toolbox_close"}}}
	{sequence "squat_repair_3" {speed 0.95} {smooth 0.1} {events {18 "toolbox_open"} {40 "tool_equip"} {83 "wrench_turn"} {100 "wrench_turn"} {117 "wrench_turn"} {135 "wrench_turn"} {151 "wrench_turn"} {168 "wrench_turn"} {196 "toolbox_close"}}}
	{sequence "squat_holdback_gun"						{file "weapon\anim_rifle_crouch_holdback_01.anm"}{speed 1.3}}
	{sequence "squat_holdback_pistol"					{file "weapon\anim_pistol_crouch_holdback_01_up.anm"}{speed 1.3}}
	{sequence "squat_self-treatment_hand" {speed 0.4}}
	{sequence "squat_self-treatment_hand_begin" {speed 0.4}}
	{sequence "squat_self-treatment_hand_end" {speed 0.4}}
	{sequence "squat_self-treatment_head" {speed 0.4}}
	{sequence "squat_self-treatment_head_begin" {speed 0.4}}
	{sequence "squat_self-treatment_head_end" {speed 0.4}}
	{sequence "squat_self-treatment_leg" {speed 0.4}}
	{sequence "squat_self-treatment_leg_begin" {speed 0.4}}
	{sequence "squat_self-treatment_leg_end" {speed 0.4}}

	
	{sequence "squat_dig_trench" {speed 0.75} {events {22 "shovel"}}}

	; squat fire old
	{sequence "squat_fire_bazooka" {speed 2.1}}


	; squat fire new
	{sequence "squat_assault-fire_gun"					{file "weapon\anim_smg_crouch_shoot_01.anm"}		{speed 1}}
	{sequence "squat_assault-fire_rifle"				{file "weapon\anim_rifle_crouch_shoot_01.anm"}	{speed 2}}

	{sequence "squat_fire_gun"							{file "weapon\anim_smg_crouch_shoot_01.anm"}		{speed 1}}
	{sequence "squat_fire_gun_short"					{file "weapon\anim_smg_crouch_shoot_01.anm"}		{speed 1}}
	{sequence "squat_fire_rifle" 						{file "weapon\anim_rifle_crouch_shoot_01.anm"}	{speed 2}}
	{sequence "squat_fire_mgun" 						{file "weapon\anim_smg_crouch_shoot_01_up.anm"}		{speed 1}}

	{sequence "squat_fire_pistol"						{file "weapon\anim_pistol_crouch_shoot_01_up.anm"}		{speed 1}}
	{sequence "squat_fire_pistol_1"						{file "weapon\anim_pistol_crouch_shoot_01_up.anm"}		{speed 1}}

	{smooth 0.025}
	;new mapped to gun
	{sequence "squat_holdback_rifle" 	{file "weapon\anim_rifle_crouch_holdback_01.anm"}}
	{sequence "squat_holdback_bazooka" 	{file "squat_holdback_gun.anm"}}
	{sequence "squat_reload_rifle" 		{file "weapon\anim_smg_crouch_reload.anm"}{speed 0.35}}
	{sequence "squat_from_stand_rifle" 	{file "locomotion\mantle\anim_stand_to_crouch_gun.anm"} {speed 0.55}}
	{blend	  "pose_squat_fire_mgun"	{pose "squat_fire_gun" begin}}

	{sequence "squat_reload_mgun" 		{file "squat_reload_gun.anm"} {speed 0.5}}
	{sequence "squat_from_stand_mgun" 	{file "squat_from_stand_gun.anm"} {speed 0.5}}
	{sequence "squat_from_stand_pistol"	{file "squat_from_stand_pistol.anm"} {speed 0.5}}
	{smooth 0.1}

;================== SQUATING ANIMATIONS end ==========================

;================= LYING DOWN ANIMATIONS begin ============

	{smooth 0.5}
	
	{sequence "lie_medic_revive"		{smooth 0.3} {speed 0.5}}
	
	
	{sequence "lie_self-treatment_hand" 		{speed 0.9}}
	{sequence "lie_self-treatment_hand_begin" 	{speed 0.65}}
	{sequence "lie_self-treatment_hand_end" 	{speed 0.65}}
	{sequence "lie_self-treatment_head" 		{speed 0.9}}
	{sequence "lie_self-treatment_head_begin" 	{speed 0.65}}
	{sequence "lie_self-treatment_head_end" 	{speed 0.65}}
	{sequence "lie_self-treatment_leg" 			{speed 0.9}}
	{sequence "lie_self-treatment_leg_begin" 	{speed 0.65}}
	{sequence "lie_self-treatment_leg_end" 		{speed 0.65}}
	

	

	{sequence "lie_reload_gun" 			{file "weapon\lie_reload_gun"}{speed 1.3} {smooth 0.5}}
	{sequence "lie_take_gun" 			{speed 0.25}}
	{sequence "lie_fire_gun" 			{file "weapon\anim_smg_lie_shoot_01.anm"}{speed 1}}
	{sequence "lie_holdback_gun"		{file "weapon\anim_rifle_lie_holdback_01.anm"}{speed 1}}
	{sequence "lie_holdback_bazooka" 	{file "lie_holdback_gun.anm"}}

	{sequence "lie_reload_mgun" 			{file "weapon\anim_mg_lie_reload_01.anm"}{speed 0.4}{smooth 0.1} {dynamicSpeed {0 10 0.6} {10 15 0.3} {25 27 0.2}}}
	{sequence "lie_reload_bazooka" 			{speed 1} {smooth 0}}
	{sequence "lie_reload_piat" 			{speed 1}}
	{sequence "lie_fire_bazooka" 			{speed 1}}
	{sequence "lie_take_mgun" 				{speed 0.25}}
	{sequence "lie_fire_mgun" 				{file "weapon\anim_mg_lie_shoot_01.anm"}}

	{sequence "lie_take_pistol" 				{file "weapon\anim_item_lie_take_01.anm"}	{speed 1}}
	{sequence "lie_fire_pistol"					{file "weapon\anim_pistol_lie_shoot_01.anm"}	{speed 1}}
	{sequence "lie_reload_pistol" 				{file "weapon\anim_pistol_lie_reload_01.anm"}	{speed 1}{dynamicSpeed {0 10 0.6} {10 15 0.3}}}
	{sequence "lie_holdback_pistol"				{file "weapon\anim_pistol_lie_holdback_01.anm"}	{speed 1}}
	{sequence "lie_take_small" 					{file "weapon\anim_item_lie_take_end_01.anm"}	{speed 1}{events {1 "pick_up_small"}}}
	{sequence "lie_put_small" 					{file "weapon\anim_item_lie_take_start_01.anm"}	{speed 1}}
	{sequence "lie_take_item_begin" 				{speed 0.7} {events {0 "pick_up_small"}}}
	{sequence "lie_take_item_end" 					{speed 0.7}}
	{sequence "lie_throw_small_begin" 				{speed 1.3}}
	{sequence "lie_throw_small_end" 				{speed 0.9}}
	{sequence "lie_throw_grenade_begin" 			{file "throw\anim_lie_throw_begin.anm"}		{speed 1.8}{events {16 "arm_right"}}}
	{sequence "lie_throw_grenade_end"   			{file "throw\anim_lie_throw_end.anm"}		{speed 1.5}}
	{sequence "lie_throw_grenade_1_begin" 			{file "throw\anim_lie_throw_begin.anm"}		{speed 1.8}{events {16 "arm_right"}}}
	{sequence "lie_throw_grenade_1_end"   			{file "throw\anim_lie_throw_end.anm"}		{speed 1.5}}
;Old grenade throw
	{sequence "lie_throw_grenade_1_old_begin" 	{speed 1.5} {events {60 "arm_right"}}}
	{sequence "lie_throw_grenade_1_old_end"   	{speed 1.25}}

	{sequence "lie_rotate_l45" 					{movement} {speed 0.75}}
	{sequence "lie_rotate_r45" 					{movement} {speed 0.75}}
	{sequence "lie_rotate_l45_gun" 				{movement} {speed 0.75}}
	{sequence "lie_rotate_r45_gun" 				{movement} {speed 0.75}}
	{sequence "lie_grenade_alert"				{smooth 0.1} {speed 0.8} {movement}}
	{smooth 0.025}
	{sequence "lie_from_squat_rifle" 	{file "lie_from_squat_gun.anm"} {speed 0.5}}
	{blend "pose_lie_rifle"				{pose "weapon\anim_smg_lie_shoot_01" begin}}
	{blend "pose_snipe_rifle" 			{pose "weapon\anim_smg_lie_shoot_01" begin}}
	{sequence "lie_take_rifle" 			{file "lie_take_gun.anm"} {speed 0.25}}
	{sequence "lie_reload_rifle"		{file "weapon\lie_reload_gun"}{speed 0.4} {dynamicSpeed {0 10 0.6} {10 15 0.3}}}
	{sequence "lie_fire_rifle"			{file "weapon\anim_rifle_lie_shoot_01.anm"}{speed 1}}
	{smooth 0.1}

;================== LYING DOWN ANIMATIONS end ==========================

;================= STEP ASIDE (STRAFE) begin ==============

	{blend "pose_step_aside_left"  		{pose "stand_step_aside_left" end}}
	{blend "pose_step_aside_right" 		{pose "stand_step_aside_right" end}}
	{blend "pose_step_aside_left_gun" 	{pose "stand_step_aside_left_gun" end}}
	{blend "pose_step_aside_right_gun"	{pose "stand_step_aside_right_gun" end}}

	{sequence "stand_step_aside_left"  		{speed 1.0}}
	{sequence "stand_step_aside_right" 		{speed 1.0}}
	{sequence "stand_step_aside_left_gun" 	{speed 1.0}}
	{sequence "stand_step_aside_right_gun"	{speed 1.0}}

;====================== STEP ASIDE (STRAFE) end =========================

;================= WALK AND RUN begin =====================
	{smooth 0.25}
	; walk
	{sequence "walk_gun_up" 	{file "locomotion\walk_up"}						{speed 0.3} {resume}{events {6 "leg_left"} {31 "leg_right"}}}
	{sequence "walk_down"		{file "locomotion\walk_down"}					{speed 0.3} {resume}{events {6 "leg_left"} {31 "leg_right"}}}
	{sequence "walk_empty_up" 	{file "locomotion\anim_walk_empty_up.anm"}		{speed 0.3} {resume}{events {6 "leg_left"} {31 "leg_right"}}}
	{sequence "walk_strap_up" 	{file "locomotion\anim_walk_empty_up.anm"}		{speed 0.3} {resume}{events {6 "leg_left"} {31 "leg_right"}}}
	{sequence "walk_1hand" 		{file "locomotion\anim_walk_1hand.anm"}			{speed 0.3} {resume}{events {6 "leg_left"} {31 "leg_right"}}}
	{sequence "walk_2hand" 		{file "locomotion\anim_walk_2hand.anm"}			{speed 0.3} {resume}{events {6 "leg_left"} {31 "leg_right"}}}
	{sequence "walk_corpse"														{speed 0.4} {resume}{events {20 "leg_left"}{47 "leg_right"}}}
	{sequence "corpse_move" 													{speed 0.4} {resume}}

	; spot
	{sequence "walk_spot_gun_down" 				{speed 0.45} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_gun_up" 				{speed 0.45} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_gun_up_1" 				{speed 0.37} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_gun_down_1" 			{speed 0.37} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_gun_up_2" 				{speed 0.45} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_gun_down_2" 			{speed 0.45} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_gun_belt_down" 		{speed 0.35} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_gun_belt_up" 			{speed 0.35} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_gun_half-mast_down"	{speed 0.45} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_gun_half-mast_up" 		{speed 0.45} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_mgun_down" 			{speed 0.45} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_mgun_up" 				{speed 0.45} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_mgun_down_1"			{speed 0.45} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_mgun_up_1"				{speed 0.45} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_pistol_down" 			{speed 0.37} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_spot_pistol_up" 			{speed 0.37} {resume}}
	{sequence "walk_giveup_1" 					{speed 1} {resume} {events {10 "leg_left"} {40 "leg_right"}}}
	{sequence "walk_giveup_2" 					{speed 1} {resume} {events {10 "leg_left"} {40 "leg_right"}}}

	; run slow
	{sequence "run_slow_down"		{file "locomotion\jog_gun_down"}{speed 0.3} {resume} {events {15 "leg_left"} {36 "leg_right"}}}
	{sequence "run_slow_down_1"		{file "locomotion\jog_gun_down"}{speed 0.3} {resume} {events {15 "leg_left"} {36 "leg_right"}}}
	{sequence "run_slow_empty_up"	{file "locomotion\anim_run_empty_up.anm"}{speed 0.8} {resume}}
	{sequence "run_slow_gun_up"		{file "locomotion\jog_gun_up"}{speed 0.3} {resume}}
	{sequence "run_slow_strap_up"	{file "locomotion\anim_run_empty_up.anm"}{speed 0.3} {resume}}



	; strafe
	{sequence "run_back_spot_gun_down" 	 	 	{resume}}
	{sequence "run_back_spot_gun_up" 			{resume}}
	{sequence "run_spot_gun_down"				{resume}}
	{sequence "run_spot_gun_up"  				{resume}}
	{sequence "run_spot_back-strafe_gun_down"	{resume} {rotate "body" 0 0 1 -90}}
	{sequence "run_spot_back-strafe_gun_up"  	{resume} {rotate "body" 0 0 1 -90}}
	{sequence "run_spot_left-strafe_gun_down"	{resume} {rotate "body" 0 0 1 -90}}
	{sequence "run_spot_left-strafe_gun_up"  	{resume} {rotate "body" 0 0 1 -90}}

	; Bazooka mapped to gun
	{smooth 0.025}
	{sequence "run_bazooka_up"				{file "locomotion\sprint_bazooka_up"} 			{speed 0.42} {resume}}
	{sequence "run_fast_bazooka_rhand_up" 	{file "locomotion\sprint_bazooka_up"} 			{speed 0.42} {resume}}
	{sequence "run_bow_bazooka_up" 			{file "locomotion\sprint_bazooka_up"} 			{speed 0.42} {resume}}
	{sequence "run_bow_bazooka_down" 		{file "locomotion\sprint_bazooka_down"} 		{speed 0.42} {resume}}

;====================== WALK AND RUN end =====================

;================= THROWING begin =========================


	{smooth 0.2}
	{sequence "stand_throw_1hand_begin" 			{file "throw\anim_stand_throw_begin.anm"}{speed 1.35}}
	{sequence "stand_throw_1hand_end" 				{file "throw\anim_stand_throw_end.anm"}{speed 1}}
	{sequence "stand_throw_small_begin" 			{file "throw\anim_stand_throw_begin.anm"}{speed 1.35}}
	{sequence "stand_throw_small_end" 				{file "throw\anim_stand_throw_end.anm"}{speed 1}}
	{sequence "stand_front_throw_knife_1_begin" 	{speed 1.3}}
	{sequence "stand_front_throw_knife_1_end"   	{speed 0.9}}
	{sequence "stand_back_throw_knife_begin" 		{speed 1.3}}
	{sequence "stand_back_throw_knife_end"   		{speed 0.9}}


;	{sequence "stand_throw-side_grenade_begin" {file "stand_throw-side_grenade_mp_begin.anm"} {smooth 0.5} {speed 0.8} {events {5 "arm_right"}}}
;	{sequence "stand_throw-side_grenade_end"   {file "stand_throw-side_grenade_end.anm"} {speed 0.8}}

	{sequence "stand_throw-side_grenade_begin" {file "throw\anim_stand_throw_begin.anm"}	{speed 1.8} {smooth 0.5} {events {5 "arm_right"}}}
	{sequence "stand_throw-side_grenade_end"   {file "throw\anim_stand_throw_end.anm"}		{speed 1.5}}
	
	{sequence "stand_throw_grenade_begin"      {file "stand_throw_grenade_begin.anm"}  {smooth 0.5} {speed 2.0} {events {50 "arm_right"}}}
	{sequence "stand_throw_grenade_end"        {file "stand_throw_grenade_end.anm"} {smooth 0.5} {speed 1.50}}
	{sequence "stand_throw_grenade_1_begin"    {file "stand_throw_grenade_1_mp_begin.anm"} {smooth 0.5} {speed 1.3} {events {5 "arm_right"}}}
	{sequence "stand_throw_grenade_1_end"      {file "stand_throw_grenade_1_end.anm"} {speed 1.50}}
	{sequence "stand_throw_grenade_2_begin"    {file "stand_throw_grenade_2_mp_begin.anm"} {smooth 0.5} {speed 1.3} {events {10 "arm_right"}}}
	{sequence "stand_throw_grenade_2_end"      {file "stand_throw_grenade_2_end.anm"} {speed 1.50}}

;================== THROWING end =====================

;================= DEATH ANIMATIONS begin =================
	; DYING


;=================||Die and fall backward||	
	{sequence "death/die_bwd/die_bwd_01"  }
	{sequence "death/die_bwd/die_bwd_02"  }
	{sequence "death/die_bwd/die_bwd_03a"  }
	{sequence "death/die_bwd/die_bwd_03b"  }
	{sequence "death/die_bwd/die_bwd_03c"  }
	{sequence "death/die_bwd/die_bwd_03d"  }
	{sequence "death/die_bwd/die_bwd_04a"  }
	{sequence "death/die_bwd/die_bwd_04b"  }
	{sequence "death/die_bwd/die_bwd_04c"  }
	{sequence "death/die_bwd/die_bwd_05a"  }
	{sequence "death/die_bwd/die_bwd_05b"  }
	{sequence "death/die_bwd/die_bwd_05c"  }
	{sequence "death/die_bwd/die_bwd_06"  }
	{sequence "death/die_bwd/die_bwd_07a"  }
	{sequence "death/die_bwd/die_bwd_07b"  }
	{sequence "death/die_bwd/die_bwd_07c"  }
	{sequence "death/die_bwd/die_bwd_07d"  }
	{sequence "death/die_bwd/die_bwd_08"  }
	{sequence "death/die_bwd/die_bwd_09a"  }
	{sequence "death/die_bwd/die_bwd_09b"  }
	{sequence "death/die_bwd/die_bwd_10"  }
	{sequence "death/die_bwd/die_bwd_11a"  }
	{sequence "death/die_bwd/die_bwd_11b"  }
	{sequence "death/die_bwd/die_bwd_12"  }
	{sequence "death/die_bwd/die_bwd_13"  }
	{sequence "death/die_bwd/die_bwd_14a"  }
	{sequence "death/die_bwd/die_bwd_14b"  }
	{sequence "death/die_bwd/die_bwd_14c"  }
	{sequence "death/die_bwd/die_bwd_14d"  }
	{sequence "death/die_bwd/die_bwd_14e"  }
	{sequence "death/die_bwd/die_bwd_15"  }
	{sequence "death/die_bwd/die_bwd_16"  }
	
;=================||Die and fall forward||	
	{sequence "death/die_fwd/die_fwd_01a"    }
	{sequence "death/die_fwd/die_fwd_01b"    }
	{sequence "death/die_fwd/die_fwd_01c"    }
	{sequence "death/die_fwd/die_fwd_01d"    }
	{sequence "death/die_fwd/die_fwd_02a"    }
	{sequence "death/die_fwd/die_fwd_02b"    }
	{sequence "death/die_fwd/die_fwd_03a"    }
	{sequence "death/die_fwd/die_fwd_03b"    }
	{sequence "death/die_fwd/die_fwd_04a"    }
	{sequence "death/die_fwd/die_fwd_04b"    }
	{sequence "death/die_fwd/die_fwd_05a"    }
	{sequence "death/die_fwd/die_fwd_05b"    }
	{sequence "death/die_fwd/die_fwd_05c"    }
	{sequence "death/die_fwd/die_fwd_06"    }
	{sequence "death/die_fwd/die_fwd_07"    }
	
;=================||Die and crouch||	
	{sequence "death/die_crouch/die_crouch_01a"    }
	{sequence "death/die_crouch/die_crouch_01b"    }
	{sequence "death/die_crouch/die_crouch_01c"    }
	{sequence "death/die_crouch/die_crouch_02"    }
	{sequence "death/die_crouch/die_crouch_03"    }
	
;=================||Die long||	
	{sequence "death/die_long/die_long_01"   }
	{sequence "death/die_long/die_long_03"    }
	
;=================||Die prone||	
	{sequence "death/die_prone/die_prone" {smooth 0.1}{speed 3.0}{movement} }

	{sequence "stand_die_03" 				{file "death/die_long/die_long_01.anm"} {movement}{speed 0.8}{smooth 0.06} }
	{sequence "stand_die_04" 				{file "death/die_long/die_long_03.anm"} {movement}{speed 0.8}{smooth 0.06} }
	{sequence "stand_die_05" 				{file "death/die_bwd/die_bwd_04c.anm"} {movement}{speed 0.8}{smooth 0.06} }
	{sequence "stand_die_06"				{file "death/die_bwd/die_bwd_12.anm"} {movement}{speed 0.8}{smooth 0.06} }
	{sequence "stand_die_back_1" 			{file "death/die_bwd/die_bwd_05a.anm"} {movement}{speed 0.8}{smooth 0.06} }
	{sequence "stand_die_back_2" 			{file "death/die_bwd/die_bwd_14a.anm"} {movement}{speed 0.65}{smooth 0.06} }
	{sequence "stand_die_back_3" 			{file "death/die_bwd/die_bwd_08.anm"} {movement}{speed 0.65}{smooth 0.06} }
	{sequence "stand_die_back_4" 			{file "death/die_bwd/die_bwd_03c.anm"} {movement}{speed 0.8}{smooth 0.06} }
	{sequence "stand_die_back_5" 			{file "death/die_bwd/die_bwd_07a.anm"} {movement}{speed 0.65}{smooth 0.06} }
	{sequence "stand_die_front_1" 			{file "death/die_run/die_run_05.anm"} {movement}{speed 0.8}{smooth 0.06} }
	{sequence "stand_die_front_2" 			{file "death/die_fwd/die_fwd_01c.anm"} {movement}{speed 0.65}{smooth 0.06} }
	{sequence "stand_die_front_3" 			{file "death/die_run/die_run_02.anm"} {movement}{speed 0.65}{smooth 0.06} }
	{sequence "stand_die_front_4" 			{file "death/die_fwd/die_fwd_05b.anm"} {movement}{speed 0.65}{smooth 0.06} }
	{sequence "stand_scenic-die_front_1" 	{file "death/die_run/die_run_06.anm"} {movement}{speed 0.65}{smooth 0.06} }
	{sequence "stand_scenic-die_front_2" 	{file "death/die_fwd/die_fwd_03b.anm"} {movement}{speed 0.65}{smooth 0.06} }
	{sequence "stand_scenic-die_back_1" 	{file "death/die_fwd/die_fwd_06.anm"} {movement}{speed 0.8}{smooth 0.06} }
	{sequence "stand_scenic-die_back_2" 	{file "death/die_bwd/die_bwd_06.anm"} {movement}{speed 0.7}{smooth 0.06} }
	{sequence "stand_die_spot" 				{file "death/die_bwd/die_bwd_02.anm"} {movement}{speed 0.7}{smooth 0.06} }
	{sequence "stand_die_spot_1" 			{file "death/die_bwd/die_bwd_16.anm"} {movement}{speed 0.8}{smooth 0.06} }
	{sequence "squat_die" 					{file "death/die_crouch/die_crouch_01a.anm"} {movement}{speed 0.7}{smooth 0.06} }
	{sequence "squat_die_front" 			{file "death/die_crouch/die_crouch_01b.anm"} {movement}{speed 0.7}{smooth 0.06} }
	{sequence "squat_die_back_1" 			{file "death/die_crouch/die_crouch_01c.anm"} {movement}{speed 0.7}{smooth 0.06} }
	{sequence "squat_die_back_2" 			{file "death/die_crouch/die_crouch_02.anm"} {movement}{speed 0.7}{smooth 0.06} }
	{sequence "squat_die_force-back" 		{file "death/die_crouch/die_crouch_03.anm"} {movement}{speed 0.7}{smooth 0.06} }

	{sequence "stand_die_back_4_blast"  		{movement} {smooth 0.1} {speed 0.8}	{events {31 "body_fall_heavy"}}}
	{sequence "stand_die_back_5_blast"  		{movement} {smooth 0.1} {speed 0.8} {events {34 "body_fall_heavy"}}}
	{sequence "stand_die_front_1_blast"  		{movement} {smooth 0.1} {speed 0.8} {events {30 "body_fall_heavy"}}}
	{sequence "stand_die_front_2_blast"  		{movement} {smooth 0.1} {speed 0.8} {events {32 "body_fall_heavy"}}}
	{sequence "stand_die-force_back_1"     		{movement} {speed 0.8} {smooth 0.1} {events {30 "body_fall_heavy"} {60 "death_breathe"}}}
	{sequence "stand_die-force_back_2_far" 		{movement} {speed 0.8} {smooth 0.1} {events {33 "body_fall_heavy"}}}
	{sequence "stand_die-force_back_3_far" 		{movement} {speed 0.8} {smooth 0.1} {events {26 "body_fall_heavy"}}}
	{sequence "stand_die-force_front_1"     	{movement} {speed 0.8} {smooth 0.1} {events {16 "body_fall_heavy"}}}
	{sequence "stand_die-force_front_2"     	{movement} {speed 0.8} {smooth 0.1} {events {22 "body_fall_heavy"} {93 "body_fall_light"} {49 "death_breathe"}}}
	{sequence "stand_die-force_front_3_far" 	{movement} {speed 0.7} {smooth 0.1} {events {31 "body_fall_heavy"}}}
	{sequence "stand_die-force_front_4_far" 	{movement} {speed 0.7} {smooth 0.1} {events {32 "body_fall_heavy"}}}
	{sequence "stand_die_drowin" 				{events {1 "swim"} {224 "splash"} {144 "burn_scream"}}}
	{sequence "stand_die_trench_1"				{speed 0.9} {smooth 0.1} {events {22 "body_fall_heavy"}}}
	{sequence "stand_die_trench_2"				{speed 0.9} {smooth 0.1} {events {34 "body_fall_heavy"}}}
	{sequence "stand_die_trench_3"				{speed 1.2} {events {29 "body_fall_light"} {40 "death_breathe"}}}
	{sequence "stand_die_burn_1"				{speed 0.9} {smooth 0} {events {38 "leg_right"} {44 "burn_scream"} {48 "leg_left"} {61 "leg_right"} {80 "leg_left"} {89 "leg_right"} {102 "leg_left"} {115 "leg_right"} {128 "body_fall_heavy"} {288 "body_fall_light"}}}
	{sequence "stand_die_burn_2"				{speed 0.9} {smooth 0} {events {38 "leg_right"} {44 "burn_scream"} {48 "leg_left"} {61 "leg_right"} {80 "leg_left"} {89 "leg_right"} {102 "leg_left"} {115 "leg_right"} {128 "body_fall_heavy"} {288 "body_fall_light"}}}
	
	{sequence "lie_die_drowin" 	{events {1 "swim"} {130 "splash"} {75 "burn_scream"}}}
	{sequence "lie_die" 		{movement}{speed 0.7}{smooth 0.06}{events {12 "body_fall_light"}} {file "death/die_prone/die_prone.anm"}}
	{sequence "lie_die_burn" 	{movement} {events {1 "burn_scream"} {12 "body_fall_light"}}}

	{sequence "stand_fly_forward"}
	{sequence "stand_fly_forward_end" {events {1 "body_fall_heavy"}}}
	{sequence "stand_fly_back" 	   {smooth 0}}
	{sequence "stand_fly_back_end" {smooth 0} {events {1 "body_fall_heavy"}}}
	{sequence "stand_fly_tumble_back"}
	{sequence "stand_fly_tumble_back_end" {events {1 "body_fall_heavy"}}}
	{sequence "stand_fly_back_far" {speed 0.8}}
	{sequence "stand_fly_back_far_end" {speed 0.8} {events {1 "body_fall_heavy"}}}
	{sequence "stand_fly_tumble_back_far" {speed 0.8}}
	{sequence "stand_fly_tumble_back_far_end" {speed 0.8} {events {1 "body_fall_heavy"}}}
	{sequence "stand_fly_forward_far" {speed 0.8}}
	{sequence "stand_fly_forward_far_end" {speed 0.8} {events {1 "body_fall_heavy"}}}
	{sequence "stand_fly_tumble_forward_far" {speed 0.8}}
	{sequence "stand_fly_tumble_forward_far_end" {speed 0.8} {events {1 "body_fall_heavy"}}}

	{smooth 0.2}

;================== DEATH ANIMATIONS end =====================

;================= LOOK L/R begin =========================

	{smooth 0.5}
	{sequence "rlook_gun"		{smooth 0.3} {speed 0.9} {movement} {file "cornerpeekright"}}
	{sequence "llook_gun" 		{smooth 0.3} {speed 0.9} {movement} {file "cornerpeekright"}}
	{sequence "rlook_pistol"	{movement}}
	{sequence "llook_pistol" 	{movement}}
	{sequence "rlook_item" 		{movement}}
	{sequence "rlook_throw_item_begin" {pinback 2}}
	{sequence "rlook_throw_item_end" {pinback 2}}
	{sequence "llook_item" 		{movement}}
	{sequence "llook_throw_item_begin" {pinback 2}}
	{sequence "llook_throw_item_end" {pinback 2}}

	{sequence "rlook_peep" {movement}}
	{sequence "llook_peep_gun" {movement}}
	{sequence "llook_stealth_gun" {movement}}
	{sequence "rlook_stealth_gun" {movement}}
	{sequence "llook_stealth_item" {movement}}
	{sequence "llook_stealth_throw_item_begin" {pinback 2}}
	{sequence "llook_stealth_throw_item_end" {pinback 2}}
	{sequence "rlook_stealth_item" {movement}}
	{sequence "rlook_stealth_throw_item_begin" {pinback 2}}
	{sequence "rlook_stealth_throw_item_end" {pinback 2}}
	{smooth 0.2}

;================== LOOK L/R end =====================

;================= SNIPE COVER begin ======================

	{smooth 0.5}
	{sequence "snipe_take_pistol"}
	{sequence "cover_from_stand_empty"				{file "cover_stand_empty"}{speed 1.2} {movement} {smooth 0.2}}
	{sequence "cover_from_snipe_gun" 				{file "cover_stand"}{speed 1.2}{movement}{smooth 0.2}}
	{sequence "cover_from_snipe_gun_for_wind" 		{file "cover_stand"}{speed 1.2}{movement}{smooth 0.2}}
;	{sequence "snipe_from_cover_gun" 				{movement}}
;	{sequence "snipe_from_cover_gun_for_wind" 		{file "snipe_from_cover_gun.anm"}}
;	{sequence "snipe_put_gun"}
;	{sequence "snipe_reload_gun"}
	{sequence "cover_grenade_alert_left" 			{speed 0.70} {smooth 0.1}}
	{sequence "cover_grenade_alert_left_gun"		{speed 0.70} {smooth 0.1}}
	{sequence "cover_grenade_alert_right_gun"		{speed 0.70} {smooth 0.1}}
	{smooth 0.2}

;================== SNIPE COVER end =====================

;================= SWIM begin ======================

	{smooth 0.5}
	{sequence "swim_from_stand_gun" {movement} {speed 0.7} {smooth 0.3}}
	{sequence "swim_gun"	  {pinback} {speed 0.9}}
	{sequence "swim_spot_gun" {pinback}}
	{sequence "swim_from_stand_empty" {movement} {smooth 0.3}}
	{sequence "swim_on_the_spot_empty" {pinback}}
	{sequence "swim_empty" {pinback}}
	{sequence "swim_fast_empty" {pinback}}
	{smooth 0.2}

;================== SWIM end =====================

;================= MELEE begin =============================

	{smooth 0.2}
	{sequence "stand_bayonet_stab" {speed 4.0} {dynamicSpeed {90 120 4}}}
	{sequence "fight_front_knife" {movement}}
	{sequence "fight_front_club_begin" {speed 1.2} {dynamicSpeed {0 2 0.25} {10 12 0.2}}}
	{sequence "fight_club_finish"}
	{sequence "fight_kick_lying_begin"  {dynamicSpeed {11.5 12 0.05}}}

	{sequence "fight_hit_right_hand" {speed 1.5} {dynamicSpeed {0 10 0.8} {16 20 0.4}}}
	{sequence "fight_hit_left_hand"  {speed 1.5}  {dynamicSpeed {0 10 0.8} {15 16 0.08}}}
	{sequence "fight_straight_hit_rhand" {speed 1.5}  {dynamicSpeed {0 10 0.8} {16 20 0.4}}}
	{sequence "fight_straight_hit_lhand" {speed 1.5}  {dynamicSpeed {0 10 0.8} {15 16 0.08}}}
	{sequence "fight_upper-cut_rhand"    {speed 1.5}  {dynamicSpeed {0 10 0.8} {16 20 0.4}}}
	{sequence "fight_body-hit_rhand" {speed 2.4} {dynamicSpeed {0 1 0.08} {16 20 0.2}}}

	{sequence "stand_after_straight_hit"}
	{sequence "stand_after_hit"   {speed 0.85}  {dynamicSpeed {10 11 0.09}}}
	{sequence "stand_after_hit_1" {speed 0.85}  {dynamicSpeed {10 11 0.09}}}
	{sequence "stand_after_body-hit" {dynamicSpeed {9 10 0.12}}}
	{sequence "step_back_after_hit"    {movement}}
	{sequence "step_back_after_hit_1"  {movement}}
	{sequence "step_back_after_body-hit"   {movement} {dynamicSpeed {34 35 0.15}}}
	{sequence "step_back_after_body-hit_1" {movement} {dynamicSpeed {34 35 0.15}}}
	
	{sequence "fight_front_sword"{speed 2.0}}
	{sequence "fight_front_sword_2"{speed 2.0}}
	{sequence "fight_front_sword_3"{movement}{speed 1.5}}

;================== MELEE end =====================

;=============These are not the anims you are looking for===============
	{sequence "anim_that_yoinky_sploinky_01" 					{smooth 0.01}{speed 1}}
	{sequence "anim_that_yoinky_sploinky_02" 					{smooth 0.01}{speed 1}}
	{sequence "anim_that_yoinky_sploinky_03" 					{smooth 0.01}{speed 1}}
	{sequence "anim_that_yoinky_sploinky_04" 					{smooth 0.01}{speed 1}}
;=============These are not the anims you are looking for===============
