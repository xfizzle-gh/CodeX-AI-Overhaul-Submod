; Enemy attacker pressure waves - MI-driven reinforcements for the AI attacker.
; Quadrant 3 of the four-way support parity, and the mirror image of
; attack_support_waves.inc pointed at the player's own flags: on a HUMAN DEFENCE
; mission (campaign_capture_the_flag, user_is_defender$ == 1) it gives the AI attacker
; the infantry its purchase economy is too slow to field, walking in off its own map edge
; and advancing on the flags the human is defending.
;
; Included by all fourteen CWA campaign_capture_the_flag.mi maps. Provably inert on a
; human-ATTACK mission: every single trigger carries user_is_defender$ == 1, and
; enemy_attack/init additionally waits for id_1st_enemy$ > 0, enemy_spawnside$ > 0 and
; prep_inform$ == 1. enemy_spawnside$ is the readiness proof - conquest.lua writes it
; immediately after user_is_defender$ inside the same mission-authority call, so a
; positive spawn side proves the perspective vars have actually been published rather
; than merely defaulting to 0.
;
; DELIVERY (identical to the other three engines, do not deviate): off-map real-breed
; prototypes are MOVED to the attacker-side entry waypoint with {"placement"} and
; promoted in place. Never cloned - three promote designs each matched zero freshly
; created entities, because a new entity's provenance is invisible to every selector this
; format can express. Ownership is a literal 1-16 {player} switch on id_1st_enemy$ (the
; live attacker bot on a defence mission); the engine will not accept a var there.
;
; POOL SHARING, deliberate and safe. This engine claims from the SAME four faction pools
; enemy_defense_templates.inc parks for enemy_defense_support.inc. The two can never
; contend: enemy_defense_support.inc gates every trigger on user_is_defender$ == 0 and
; this engine gates every trigger on user_is_defender$ == 1, and a mission has exactly
; one value of that var. One mission therefore only ever has one claimant, so no budget
; arithmetic has to be shared and no second set of 160 prototypes has to be parked. A
; claim strips the pool tag it took from, so the pool tag still means "still parked and
; available" for whichever engine is live.
;
; FACTION SELECTION is a var switch, not a cascade, and it uses its own var so the two
; pool-sharing engines stay decoupled. conquest.lua publishes bot_army$ from its
; nationMap (1 rusa, 2 ukr, 3 nato, 4 csa, 5 sov, 6 prc, 7 frg, 8 pol) - and on a defence
; mission the bot it describes is the ATTACKER, which is exactly the side this engine
; reinforces. ea_resolve_army folds that onto the four pools this mod ships
; (enemy_attack_army$ 1 rusa / 2 ukr / 3 nato / 4 prc), routing sov+pol to the rusa pool
; and csa+frg to the nato pool. An unpublished bot_army$ lands in the default and draws
; rusa, so the system never stalls waiting for a var.
;
; PREP PHASE. A defence mission runs a real preparation phase (preparationTime 480 in
; campaign_capture_the_flag.set) and prep_inform$ goes to 1 when it ends (conquest.lua
; OnPrepTimeOver). Nothing here pre-places anything: the attacker is not supposed to be
; on the map while the player is still laying out a defence.
;
; WHAT ARRIVES, AND WHEN
;   opening wave  65/80/95s after prep ends - deliberately behind the friendly
;                 defensive-support engine's 25/40s opening, so pressure builds rather
;                 than landing on the player at the same moment as their own help
;   recurring     self-re-arming clock, randomized 125/165/205/240/280s cadence
;                 (weighted {type rand} cascade, ~20% per bucket). Every value is
;                 distinct from defense_support_waves.inc's 140-290s ladder, which is
;                 the only other engine live on this mission, so the two never
;                 synchronise.
;   budget        defense_level$ (CE campaign progression, 1-3): L1 4 waves, L2 6, L3 8,
;                 including the opening wave. Level 0 falls back to L1. defense_level$ is
;                 computed on defence missions too - CE's set_defense_level trigger is
;                 gated only on defense_level$ == 0 and carries its own
;                 user_is_defender$ == 1 branch, so it is not attack-only.
;   live cap      more than 16 live ea_src infantry defers the cycle WITHOUT consuming a
;                 wave, exactly as the other three engines' caps do.
;
; COMPOSITION POOL, one drawn at random per wave, widening with the level:
;   1 line     4 bodies from the faction line pool   L1 L2 L3  (also the pool-short fallback)
;   2 weapons  4 bodies from the faction wpn pool       L2 L3  (AT + marksman flavour)
; Infantry only, on purpose: the attacker bot already buys its own armour through the
; purchase economy and a second armour source unbalances it.
;
; WHAT THEY DO. This is Q1's order flow aimed the other way. Every wave re-shuffles up to
; three ACTIVE flag points, splits into staggered fireteams and advances - one straight
; in, one peeling to the second flag, one taking a cover beat before it pushes - so a
; wave does not walk the map as one line. no_retreat is ON: these are an assault, and the
; friendly defensive-support engine is the one that gives ground.

			; Delivery point: the ATTACKER's own map edge. enemy_spawnside$ is published
			; from conquest.lua's mission-authority branch and on a defence mission the
			; mission authority is the enemy bot, i.e. the ATTACKER - so side 1 (a) means
			; the attacker spawns on a and enters at attack_support_entry_a. That is the
			; same reading enemy_defense_support.inc uses when it delivers to the enemy's
			; own edge, and the opposite of attack_support_waves.inc, which deliberately
			; enters from the side the enemy is NOT on.
			(define "ea_place_at_entry"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"placement"
							{selector
								{source advanced}
								{group {select {tag {tag ea_deploy}}}}
							}
							{target_waypoint "attack_support_entry_a"}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement"
							{selector
								{source advanced}
								{group {select {tag {tag ea_deploy}}}}
							}
							{target_waypoint "attack_support_entry_b"}
						}
					}
					{"default"
						{"placement"
							{selector
								{source advanced}
								{group {select {tag {tag ea_deploy}}}}
							}
							{target_waypoint "attack_support_entry_a"}
						}
					}
				}
			)

			; Ownership. The engine will not accept a var in a {player} node, so all
			; sixteen slots are spelled out against id_1st_enemy$. The default branch
			; deliberately does NOT transfer: on a defence mission a guessed slot could
			; be a friendly one, which would hand the player free infantry and put an
			; attacker's orders on it. enemy_attack_owner_fail$ records that instead and
			; the wave stays at player 0, where it can do no harm. Every trigger gates on
			; id_1st_enemy$ > 0, so the default is unreachable in practice.
			(define "ea_own_to_enemy"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag ea_deploy}} {operation set} {player "16"}}}
					{"default"
						{"set_i" {var "enemy_attack_owner_fail$"} {op "="} {value 1}}
						{"timer" {time 10} {title "ENEMY ATTACK OWNER UNRESOLVED - NO TRANSFER"}}
					}
				}
			)

			; bot_army$ -> enemy_attack_army$. conquest.lua publishes bot_army$ from its
			; nationMap (1 rusa, 2 ukr, 3 nato, 4 csa, 5 sov, 6 prc, 7 frg, 8 pol) in the
			; same mission-authority call that writes user_is_defender$, so it is already
			; there by the time init runs. sov and pol draw the rusa pool, csa and frg the
			; nato pool, and an unpublished 0 lands in the default rather than stalling.
			; Its own var on purpose: enemy_defense_support.inc resolves the same mapping
			; into enemy_defense_army$, and the two engines must not read each other's
			; state even though they share the prototypes.
			(define "ea_resolve_army"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 2}}
						{"set_i" {var "enemy_attack_army$"} {op "="} {value 2}}
						{"timer" {time 8} {title "ENEMY ATTACK ARMY - UKR"}}
					}
					{"case"
						{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 3}}
						{"set_i" {var "enemy_attack_army$"} {op "="} {value 3}}
						{"timer" {time 8} {title "ENEMY ATTACK ARMY - NATO"}}
					}
					{"case"
						{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 4}}
						{"set_i" {var "enemy_attack_army$"} {op "="} {value 3}}
						{"timer" {time 8} {title "ENEMY ATTACK ARMY - NATO (csa)"}}
					}
					{"case"
						{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 6}}
						{"set_i" {var "enemy_attack_army$"} {op "="} {value 4}}
						{"timer" {time 8} {title "ENEMY ATTACK ARMY - PRC"}}
					}
					{"case"
						{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 7}}
						{"set_i" {var "enemy_attack_army$"} {op "="} {value 3}}
						{"timer" {time 8} {title "ENEMY ATTACK ARMY - NATO (frg)"}}
					}
					{"default"
						{"set_i" {var "enemy_attack_army$"} {op "="} {value 1}}
						{"timer" {time 8} {title "ENEMY ATTACK ARMY - RUSA"}}
					}
				}
			)

			; Promote + own + staggered advance on the player's flags. ea_src is the live
			; roster marker and is never removed: the live-unit cap counts it. Fresh flag
			; picks every deploy, so successive waves do not all converge on one
			; objective. Capture points are addressed as {tag flag}: the fpc1..fpc5 tags
			; are missing from one of the fourteen maps entirely.
			(define "ea_finish"
				{"delay" {time 0.5}}
				; Fresh scatter picks each deploy (clear the previous wave's flag tags).
				{"entity_state"
					{selector {tag ea_flag1}}
					{tag_remove ea_flag1}
				}
				{"entity_state"
					{selector {tag ea_flag2}}
					{tag_remove ea_flag2}
				}
				{"entity_state"
					{selector {tag ea_flag3}}
					{tag_remove ea_flag3}
				}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag ea_deploy}}}}
					}
					{tag_add ea_src}
					{tag_remove enemy_def_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
				{"delay" {time 0.2}}
				("ea_own_to_enemy")
				{"delay" {time 0.4}}
				{"actor_state"
					{selector {ignore_captured_by_user 0} {tag ea_deploy}}
					{control AI}
					{ai_move {mode enable}}
					{weapon_prepare on}
					{fire_mode open}
					{move_mode free}
					{movement {speed normal} {kind normal} {type normal}}
					{ai {no_retreat on} {advance_ratio 1} {retreat_ratio 0}}
				}
				{"ables"
					{selector {ignore_captured_by_user 0} {tag ea_deploy}}
					{remove select}
				}
				; Settle before anything is ordered anywhere.
				{"delay" {time 2}}
				; Up to 3 active flags, shuffled - a mission activates only ~2 of a map's
				; 2-5 flag_point entities and a squad sent to a dead objective just
				; stands there, so every pick excludes inactive.
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude {state {state inactive}}}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add ea_flag1}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
								{tag {tag ea_flag1}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add ea_flag2}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
								{tag {tag ea_flag1}}
								{tag {tag ea_flag2}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add ea_flag3}
				}
				{"delay" {time 0.1}}
				; Split the deploy into two staggered fireteams so a wave does not laser
				; in as one blob. Every draw here is four bodies, so two pairs is the
				; whole wave: a third group would only ever be an order on an empty
				; selector.
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag ea_deploy}}}}
						{amount 2}
					}
					{tag_add ea_g1}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag ea_deploy}}}
							{exclude {tag {tag ea_g1}}}
						}
					}
					{tag_add ea_g2}
				}
				; G1: straight in at the first flag.
				{"action"
					{selector {ignore_captured_by_user 0} {tag ea_g1}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag ea_flag1}}
				}
				{"delay" {time 3}}
				; G2: cover beat first, which breaks the line, then peel to the second
				; flag - or the third if the mission activated three.
				{"actor_to_cover"
					{selector {ignore_captured_by_user 0} {tag ea_g2}}
				}
				{"delay" {time 5}}
				{"switch"
					{"case"
						{condition {type rand} {value 0.34}}
						{"action"
							{selector {ignore_captured_by_user 0} {tag ea_g2}}
							{drop orders}
							{action advance}
							{target {ignore_captured_by_user 0} {tag ea_flag3}}
						}
					}
					{"default"
						{"action"
							{selector {ignore_captured_by_user 0} {tag ea_g2}}
							{drop orders}
							{action advance}
							{target {ignore_captured_by_user 0} {tag ea_flag2}}
						}
					}
				}
				{"delay" {time 0.2}}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag ea_deploy}}}}
					}
					{tag_remove ea_deploy}
					{tag_remove ea_g1}
					{tag_remove ea_g2}
				}
				{"entity_state"
					{selector {tag ea_flag3}}
					{tag_remove ea_flag3}
				}
				{"set_i" {var "enemy_attack_transferred$"} {op "="} {value 1}}
			)

			; Command 1 - line fireteam, 4 bodies from the faction line pool. Only the
			; trigger whose enemy_attack_army$ matches can clear the command, so poking
			; all four factions is safe.
			(define "ea_poke_line"
				{"trigger" {name "enemy_attack/rusa_line"}}
				{"trigger" {name "enemy_attack/ukr_line"}}
				{"trigger" {name "enemy_attack/prc_line"}}
				{"trigger" {name "enemy_attack/nato_line"}}
			)

			; Command 2 - weapons fireteam, 4 bodies from the faction wpn pool. Only the
			; trigger whose enemy_attack_army$ matches can clear the command, so poking
			; all four factions is safe.
			(define "ea_poke_wpn"
				{"trigger" {name "enemy_attack/rusa_wpn"}}
				{"trigger" {name "enemy_attack/ukr_wpn"}}
				{"trigger" {name "enemy_attack/prc_wpn"}}
				{"trigger" {name "enemy_attack/nato_wpn"}}
			)

			; The wave draw, widening with the campaign level: L1 fields plain line
			; fireteams, L2 mixes in the AT/marksman weapons team a third of the time, L3
			; leads with it. Level 0 - CE has not published a level yet - lands in the
			; default and behaves as L1. A draw clears the command as its first action, so
			; a command still standing four seconds later means the pool came up short.
			(define "ea_pick_wave"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
						{"switch"
							{"case"
								{condition {type rand} {value 0.6}}
								{"timer" {time 8} {title "ENEMY ATTACK WAVE - WEAPONS FIRETEAM"}}
								{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 2}}
								("ea_poke_wpn")
							}
							{"default"
								{"timer" {time 8} {title "ENEMY ATTACK WAVE - LINE FIRETEAM"}}
								{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 1}}
								("ea_poke_line")
							}
						}
					}
					{"case"
						{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
						{"switch"
							{"case"
								{condition {type rand} {value 0.34}}
								{"timer" {time 8} {title "ENEMY ATTACK WAVE - WEAPONS FIRETEAM"}}
								{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 2}}
								("ea_poke_wpn")
							}
							{"default"
								{"timer" {time 8} {title "ENEMY ATTACK WAVE - LINE FIRETEAM"}}
								{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 1}}
								("ea_poke_line")
							}
						}
					}
					{"default"
						{"timer" {time 8} {title "ENEMY ATTACK WAVE - LINE FIRETEAM"}}
						{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 1}}
						("ea_poke_line")
					}
				}
				; Pool-short fallback: step down to the always-deepest line pool, then
				; give up on this cycle rather than spin.
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_attack_wave_cmd$"} {op ">"} {value 1}}
						{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 1}}
						("ea_poke_line")
						{"timer" {time 6} {title "ENEMY ATTACK POOL SHORT - LINE TEAM INSTEAD"}}
					}
					{"default"}
				}
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_attack_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 0}}
						{"timer" {time 6} {title "ENEMY ATTACK POOL EXHAUSTED"}}
					}
					{"default"}
				}
			)

			; Arms once per defence mission. prep_inform$ == 1 is the prep-phase gate: the
			; attacker is not supposed to be on the map while the player is still laying
			; out a defence. enemy_spawnside$ > 0 is the readiness proof. The clock stays
			; parked behind enemy_attack_busy$ / enemy_attack_next_ok$ until the opening
			; wave has been issued, otherwise its condition is already true here and it
			; fires alongside init.
			{"enemy_attack/init"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "enemy_attack_armed$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "prep_inform$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "enemy_spawnside$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "enemy_attack_armed$"} {op "="} {value 1}}
					{"set_i" {var "enemy_attack_busy$"} {op "="} {value 1}}
					{"set_i" {var "enemy_attack_next_ok$"} {op "="} {value 0}}
					{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_attack_wave_num$"} {op "="} {value 0}}
					{"set_i" {var "enemy_attack_owner_fail$"} {op "="} {value 0}}
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 1}}
					{"delay" {time 2}}
					("ea_resolve_army")
					; Wave budget by campaign progression. defense_level$ is computed by
					; the CE mission setup (1-3, roughly campaign games 0 / 6 / 12) on
					; defence missions as well as attack ones.
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
							{"set_i" {var "enemy_attack_waves_left$"} {op "="} {value 8}}
							{"timer" {time 10} {title "ENEMY ATTACK ARMED - L3, 8 WAVES"}}
						}
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
							{"set_i" {var "enemy_attack_waves_left$"} {op "="} {value 6}}
							{"timer" {time 10} {title "ENEMY ATTACK ARMED - L2, 6 WAVES"}}
						}
						{"default"
							{"set_i" {var "enemy_attack_waves_left$"} {op "="} {value 4}}
							{"timer" {time 10} {title "ENEMY ATTACK ARMED - L1, 4 WAVES"}}
						}
					}
					; Opening wave, deliberately behind the friendly defensive-support
					; engine's opening so attacker pressure builds instead of arriving
					; alongside the player's own help.
					{"switch"
						{"case"
							{condition {type rand} {value 0.34}}
							{"delay" {time 65}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 80}}
						}
						{"default"
							{"delay" {time 95}}
						}
					}
					{"set_i" {var "enemy_attack_wave_num$"} {op "+"} {value 1}}
					{"set_i" {var "enemy_attack_waves_left$"} {op "-"} {value 1}}
					("ea_pick_wave")
					; Hand the cadence over only once the opening wave has been issued.
					{"delay" {time 20}}
					{"set_i" {var "enemy_attack_next_ok$"} {op "="} {value 1}}
					{"set_i" {var "enemy_attack_busy$"} {op "="} {value 0}}
					{"trigger" {name "enemy_attack/clock"}}
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 2}}
				}
			}

			; Self-re-arming cadence. One cycle = randomized wait, then either a
			; live-unit-cap defer (no wave consumed) or one level-scaled wave. When the
			; budget runs out the condition stops matching and the loop ends.
			{"enemy_attack/clock"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "enemy_attack_next_ok$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "enemy_attack_busy$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "enemy_attack_waves_left$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"5.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "enemy_attack_busy$"} {op "="} {value 1}}
					; Randomized 125-280s cadence: weighted cascade, ~20% per bucket.
					; Every value is distinct from the friendly defensive-support ladder.
					{"switch"
						{"case"
							{condition {type rand} {value 0.2}}
							{"delay" {time 125}}
						}
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 165}}
						}
						{"case"
							{condition {type rand} {value 0.33}}
							{"delay" {time 205}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 240}}
						}
						{"default"
							{"delay" {time 280}}
						}
					}
					{"delay" {time 0.1}}
					{"switch"
						; LIVE-UNIT CAP. Skip this cycle without consuming a wave while
						; the attacker still has a platoon of MI infantry on the field.
						; Counted on the simple selector form the mission scripts use for
						; live units - the advanced selector's prop/state decorations zero
						; the match on these entities.
						{"case"
							{condition
								{type entities}
								{selector
									{ignore_captured_by_user 0}
									{tag ea_src}
									{type human}
									{state "not dead"}
								}
								{count {op ">"} {value 16}}
							}
							{"timer" {time 8} {title "ENEMY ATTACK NEAR CAP DEFER"}}
						}
						{"default"
							{"set_i" {var "enemy_attack_wave_num$"} {op "+"} {value 1}}
							{"set_i" {var "enemy_attack_waves_left$"} {op "-"} {value 1}}
							("ea_pick_wave")
						}
					}
					{"delay" {time 0.1}}
					{"set_i" {var "enemy_attack_busy$"} {op "="} {value 0}}
					{"trigger" {name "enemy_attack/clock"}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "enemy_attack_waves_left$"} {op "=="} {value 0}}
							{"timer" {time 10} {title "ENEMY ATTACK WAVES EXHAUSTED"}}
						}
						{"default"}
					}
				}
			}

			; COMPOSITION TRIGGERS - four faction pools x two draws, claiming from the
			; prototypes enemy_defense_templates.inc parks. Command gated the way the
			; other engines' compositions are: the command IS the draw id and the trigger
			; clears it as its first action, so a command still standing means no pool
			; could field it. Each also gates on enemy_attack_army$, so exactly one
			; faction can ever answer a poke, and on its own pool being stocked - a claim
			; strips the pool tag, so the tag count is precisely "still parked".
			{"enemy_attack/rusa_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "enemy_attack_wave_cmd$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_attack_army$"} {op "=="} {value 1}}
						{"5.entities"
							{selector {tag enemy_def_rusa_line}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 11}}
					{"timer" {time 8} {title "ENEMY ATTACK RUSA LINE FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_rusa_line}}}}
							{amount 4}
						}
						{tag_add ea_deploy}
						{tag_remove enemy_def_rusa_line}
					}
					{"delay" {time 0.1}}
					("ea_place_at_entry")
					("ea_finish")
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 12}}
				}
			}

			{"enemy_attack/rusa_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "enemy_attack_wave_cmd$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_attack_army$"} {op "=="} {value 1}}
						{"5.entities"
							{selector {tag enemy_def_rusa_wpn}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 21}}
					{"timer" {time 8} {title "ENEMY ATTACK RUSA WEAPONS FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_rusa_wpn}}}}
							{amount 4}
						}
						{tag_add ea_deploy}
						{tag_remove enemy_def_rusa_wpn}
					}
					{"delay" {time 0.1}}
					("ea_place_at_entry")
					("ea_finish")
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 22}}
				}
			}

			{"enemy_attack/ukr_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "enemy_attack_wave_cmd$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_attack_army$"} {op "=="} {value 2}}
						{"5.entities"
							{selector {tag enemy_def_ukr_line}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 11}}
					{"timer" {time 8} {title "ENEMY ATTACK UKR LINE FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_ukr_line}}}}
							{amount 4}
						}
						{tag_add ea_deploy}
						{tag_remove enemy_def_ukr_line}
					}
					{"delay" {time 0.1}}
					("ea_place_at_entry")
					("ea_finish")
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 12}}
				}
			}

			{"enemy_attack/ukr_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "enemy_attack_wave_cmd$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_attack_army$"} {op "=="} {value 2}}
						{"5.entities"
							{selector {tag enemy_def_ukr_wpn}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 21}}
					{"timer" {time 8} {title "ENEMY ATTACK UKR WEAPONS FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_ukr_wpn}}}}
							{amount 4}
						}
						{tag_add ea_deploy}
						{tag_remove enemy_def_ukr_wpn}
					}
					{"delay" {time 0.1}}
					("ea_place_at_entry")
					("ea_finish")
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 22}}
				}
			}

			{"enemy_attack/prc_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "enemy_attack_wave_cmd$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_attack_army$"} {op "=="} {value 4}}
						{"5.entities"
							{selector {tag enemy_def_prc_line}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 11}}
					{"timer" {time 8} {title "ENEMY ATTACK PRC LINE FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_prc_line}}}}
							{amount 4}
						}
						{tag_add ea_deploy}
						{tag_remove enemy_def_prc_line}
					}
					{"delay" {time 0.1}}
					("ea_place_at_entry")
					("ea_finish")
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 12}}
				}
			}

			{"enemy_attack/prc_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "enemy_attack_wave_cmd$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_attack_army$"} {op "=="} {value 4}}
						{"5.entities"
							{selector {tag enemy_def_prc_wpn}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 21}}
					{"timer" {time 8} {title "ENEMY ATTACK PRC WEAPONS FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_prc_wpn}}}}
							{amount 4}
						}
						{tag_add ea_deploy}
						{tag_remove enemy_def_prc_wpn}
					}
					{"delay" {time 0.1}}
					("ea_place_at_entry")
					("ea_finish")
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 22}}
				}
			}

			{"enemy_attack/nato_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "enemy_attack_wave_cmd$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_attack_army$"} {op "=="} {value 3}}
						{"5.entities"
							{selector {tag enemy_def_nato_line}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 11}}
					{"timer" {time 8} {title "ENEMY ATTACK NATO LINE FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_nato_line}}}}
							{amount 4}
						}
						{tag_add ea_deploy}
						{tag_remove enemy_def_nato_line}
					}
					{"delay" {time 0.1}}
					("ea_place_at_entry")
					("ea_finish")
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 12}}
				}
			}

			{"enemy_attack/nato_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "enemy_attack_wave_cmd$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_attack_army$"} {op "=="} {value 3}}
						{"5.entities"
							{selector {tag enemy_def_nato_wpn}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_attack_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 21}}
					{"timer" {time 8} {title "ENEMY ATTACK NATO WEAPONS FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_nato_wpn}}}}
							{amount 4}
						}
						{tag_add ea_deploy}
						{tag_remove enemy_def_nato_wpn}
					}
					{"delay" {time 0.1}}
					("ea_place_at_entry")
					("ea_finish")
					{"set_i" {var "enemy_attack_stage$"} {op "="} {value 22}}
				}
			}
