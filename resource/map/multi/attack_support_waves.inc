; Attack support waves — the human attacker's AI teammate reinforcement engine.
; Included by all fourteen CWA campaign_capture_the_flag.mi maps. Runs ONLY when the
; human is attacking; attack_support.lua publishes id_attack_support$ /
; attack_support_ready$ / attack_support_use_mi$ and nothing here moves until it has.
;
; DELIVERY (proven, do not deviate): off-map real-breed prototypes in
; attack_support_templates.inc are MOVED to the attack-side entry waypoint with
; {"placement"} and promoted in place. Never cloned: three promote designs each
; matched zero freshly created entities, because a new entity's provenance is
; invisible to every selector this format can express. Ownership is a literal 1-16
; {player} switch on id_attack_support$ — the engine will not accept a var there.
;
; SCHEDULE
;   opening wave  30/38/45s after the identity is published, one small USMC rifle team
;   recurring     self-re-arming clock, randomized 150/190/225/260/300s cadence
;                 (weighted {type rand} cascade, ~20% per bucket)
;   budget        defense_level$ (CE campaign progression, 1-3): L1 4 waves,
;                 L2 6, L3 8 — including the opening wave. Level 0 falls back to L1.
;
; COMPOSITION POOL, one drawn at random per wave, widening with the level:
;   1 usmc     5 USMC infantry                        L1 L2 L3   (also the pool-short fallback)
;   2 1ad      5 1AD infantry                         L1 L2 L3
;   3 acav     4 1AD infantry + 1 crewed humvee          L2 L3
;   4 pzgren   6 German pzgren + 2 crewed humvees           L3
; Waves stay deliberately small: this is a fireteam of support, not a second army.
;
; SAFETY GATES on every wave: the readiness gates above, the wave budget, and a
; live-unit cap (defer without consuming a wave while more than 14 attack-support
; infantry are alive). A composition whose pool cannot field it clears no command,
; so the pick falls back to the USMC rifle team and then gives up for that cycle.
;
; Vehicles deploy one at a time with a gap, because two humvees placed together
; clip and flip. Movement is {action advance} on a claimed flag after a slow
; {"placement"}, never a beeline from a raw coordinate.

			(define "am_place_at_entry"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"placement"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_deploy}}}}
							}
							{target_waypoint "attack_support_entry_b"}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_deploy}}}}
							}
							{target_waypoint "attack_support_entry_a"}
						}
					}
					{"default"
						{"placement"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_deploy}}}}
							}
							{target_waypoint "attack_support_entry_b"}
						}
					}
				}
			)

			(define "am_own_to_support"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "16"}}}
					{"default"
						{"player"
							{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
							{operation set}
							{player "3"}
						}
					}
				}
			)

			; Promote + own + slow MOVE (not a sprint advance). Split into staggered
			; fireteams so a wave does not walk one line. attack_support_src is the live
			; attack-support roster marker and is never removed: the live-unit cap counts it.
			(define "am_finish_deploy"
				{"delay" {time 0.5}}
				; Fresh scatter picks each deploy (clear previous wave flag tags).
				{"entity_state"
					{selector {tag attack_support_flag1}}
					{tag_remove attack_support_flag1}
				}
				{"entity_state"
					{selector {tag attack_support_flag2}}
					{tag_remove attack_support_flag2}
				}
				{"entity_state"
					{selector {tag attack_support_flag3}}
					{tag_remove attack_support_flag3}
				}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag attack_support_deploy}}}}
					}
					{tag_add attack_support_src}
					{tag_remove attack_support_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
				{"delay" {time 0.2}}
				("am_own_to_support")
				{"delay" {time 0.4}}
				{"actor_state"
					{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
					{control AI}
					{ai_move {mode enable}}
					{weapon_prepare on}
					{fire_mode open}
					{move_mode free}
					{movement {speed normal} {kind normal} {type normal}}
					{ai {no_retreat on} {advance_ratio 1} {retreat_ratio 0}}
				}
				{"ables"
					{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
					{remove select}
				}
				; Settle (vehicles especially).
				{"delay" {time 2}}
				; Up to 3 active flags, shuffled — different elements peel different ways.
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
					{tag_add attack_support_flag1}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
								{tag {tag attack_support_flag1}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add attack_support_flag2}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
								{tag {tag attack_support_flag1}}
								{tag {tag attack_support_flag2}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add attack_support_flag3}
				}
				{"delay" {time 0.1}}
				; Split deploy into staggered fireteams so they do not laser as one blob.
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag attack_support_deploy}}}}
						{amount 2}
					}
					{tag_add attack_support_g1}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag attack_support_deploy}}}
							{exclude {tag {tag attack_support_g1}}}
						}
						{amount 2}
					}
					{tag_add attack_support_g2}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag attack_support_deploy}}}
							{exclude
								{tag {tag attack_support_g1}}
								{tag {tag attack_support_g2}}
							}
						}
						{amount 2}
					}
					{tag_add attack_support_g3}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag attack_support_deploy}}}
							{exclude
								{tag {tag attack_support_g1}}
								{tag {tag attack_support_g2}}
								{tag {tag attack_support_g3}}
							}
						}
					}
					{tag_add attack_support_g4}
				}
				; G1: straight to flag1
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_g1}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag1}}
				}
				{"delay" {time 3}}
				; G2: peel to flag2 (or flag1 if only one)
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_g2}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag2}}
				}
				{"delay" {time 3}}
				; G3: take cover briefly, then push (breaks the line)
				{"actor_to_cover"
					{selector {ignore_captured_by_user 0} {tag attack_support_g3}}
				}
				{"delay" {time 5}}
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_g3}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag1}}
				}
				{"delay" {time 2}}
				; G4: third flag if present, else flag2, with extra lag
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_g4}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag3}}
				}
				{"delay" {time 0.2}}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag attack_support_deploy}}}}
					}
					{tag_remove attack_support_deploy}
					{tag_remove attack_support_g1}
					{tag_remove attack_support_g2}
					{tag_remove attack_support_g3}
					{tag_remove attack_support_g4}
				}
				{"entity_state"
					{selector {tag attack_support_flag3}}
					{tag_remove attack_support_flag3}
				}
				{"set_i" {var "attack_support_transferred$"} {op "="} {value 1}}
			)

			; Vehicle instances are consumed in declaration order, tracked by
			; attack_support_hmmwv_left$. Each humvee's crew is {Link}ed to that specific
			; hull, so an instance has to move as a whole - never select a count of bodies
			; out of a shared vehicle pool, or a hull arrives without its driver.
			(define "am_deploy_next_hmmwv"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "attack_support_hmmwv_left$"} {op "=="} {value 4}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_hmmwv1}}}}
							}
							{tag_add attack_support_deploy}
						}
						{"set_i" {var "attack_support_hmmwv_left$"} {op "-"} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "attack_support_hmmwv_left$"} {op "=="} {value 3}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_hmmwv2}}}}
							}
							{tag_add attack_support_deploy}
						}
						{"set_i" {var "attack_support_hmmwv_left$"} {op "-"} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "attack_support_hmmwv_left$"} {op "=="} {value 2}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_hmmwv3}}}}
							}
							{tag_add attack_support_deploy}
						}
						{"set_i" {var "attack_support_hmmwv_left$"} {op "-"} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "attack_support_hmmwv_left$"} {op "=="} {value 1}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_hmmwv4}}}}
							}
							{tag_add attack_support_deploy}
						}
						{"set_i" {var "attack_support_hmmwv_left$"} {op "-"} {value 1}}
					}
					{"default"}
				}
				{"delay" {time 0.1}}
				("am_place_at_entry")
				("am_finish_deploy")
			)

			; One composition per wave, drawn at random from the pool this campaign
			; level unlocks. The command IS the composition id; the matching trigger
			; clears it as its first action, so a command still standing a few seconds
			; later means that composition's pool could not field the wave.
			(define "am_pick_composition"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
						{"switch"
							{"case"
								{condition {type rand} {value 0.25}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 4}}
								{"trigger" {name "attack_support/comp_pzgren"}}
							}
							{"case"
								{condition {type rand} {value 0.34}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 3}}
								{"trigger" {name "attack_support/comp_acav"}}
							}
							{"case"
								{condition {type rand} {value 0.5}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 2}}
								{"trigger" {name "attack_support/comp_1ad"}}
							}
							{"default"
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
								{"trigger" {name "attack_support/comp_usmc"}}
							}
						}
					}
					{"case"
						{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
						{"switch"
							{"case"
								{condition {type rand} {value 0.34}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 3}}
								{"trigger" {name "attack_support/comp_acav"}}
							}
							{"case"
								{condition {type rand} {value 0.5}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 2}}
								{"trigger" {name "attack_support/comp_1ad"}}
							}
							{"default"
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
								{"trigger" {name "attack_support/comp_usmc"}}
							}
						}
					}
					; Level 1, and level 0 when CE has not published a level yet:
					; infantry-only fireteams.
					{"default"
						{"switch"
							{"case"
								{condition {type rand} {value 0.5}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 2}}
								{"trigger" {name "attack_support/comp_1ad"}}
							}
							{"default"
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
								{"trigger" {name "attack_support/comp_usmc"}}
							}
						}
					}
				}
				; Pool-short fallback: step down to the always-deepest USMC rifle team,
				; then give up on this cycle rather than spin.
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "attack_support_wave_cmd$"} {op ">"} {value 1}}
						{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
						{"trigger" {name "attack_support/comp_usmc"}}
						{"timer" {time 6} {title "ATTACK SUPPORT POOL SHORT - RIFLE TEAM INSTEAD"}}
					}
					{"default"}
				}
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "attack_support_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
						{"timer" {time 6} {title "ATTACK SUPPORT POOL EXHAUSTED"}}
					}
					{"default"}
				}
			)

			; Arms once per attack mission. attack_support_use_mi$ is the enable switch:
			; attack_support.lua sets it alongside id_attack_support$, because MI delivery
			; is the only path that works on the attack support slot (Lua Spawn reports no
			; units available there). The clock stays parked behind attack_support_busy$ /
			; attack_support_next_ok$ until the opening wave has been issued, otherwise its
			; condition is already true here and it fires alongside init.
			{"attack_support/init"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "attack_support_armed$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"5.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_armed$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_busy$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_next_ok$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_wave_num$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_hmmwv_left$"} {op "="} {value 4}}
					{"set_i" {var "attack_support_stage$"} {op "="} {value 1}}
					; Wave budget by campaign progression. defense_level$ is computed by the
					; CE mission setup (1-3, roughly campaign games 0 / 6 / 12).
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
							{"set_i" {var "attack_support_waves_left$"} {op "="} {value 8}}
							{"timer" {time 10} {title "ATTACK SUPPORT ARMED - L3, 8 WAVES"}}
						}
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
							{"set_i" {var "attack_support_waves_left$"} {op "="} {value 6}}
							{"timer" {time 10} {title "ATTACK SUPPORT ARMED - L2, 6 WAVES"}}
						}
						{"default"
							{"set_i" {var "attack_support_waves_left$"} {op "="} {value 4}}
							{"timer" {time 10} {title "ATTACK SUPPORT ARMED - L1, 4 WAVES"}}
						}
					}
					; Opening wave: one small USMC rifle team, 30-45s in.
					{"switch"
						{"case"
							{condition {type rand} {value 0.34}}
							{"delay" {time 30}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 38}}
						}
						{"default"
							{"delay" {time 45}}
						}
					}
					{"set_i" {var "attack_support_wave_num$"} {op "+"} {value 1}}
					{"set_i" {var "attack_support_waves_left$"} {op "-"} {value 1}}
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
					{"trigger" {name "attack_support/comp_usmc"}}
					; Hand the cadence over only once the opening wave has finished landing.
					{"delay" {time 20}}
					{"set_i" {var "attack_support_next_ok$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_busy$"} {op "="} {value 0}}
					{"trigger" {name "attack_support/clock"}}
				}
			}

			; Self-re-arming cadence. One cycle = randomized wait, then either a
			; live-unit-cap defer (no wave consumed) or one randomly picked wave.
			; When the budget runs out the condition stops matching and the loop ends.
			{"attack_support/clock"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "attack_support_next_ok$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "attack_support_busy$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "attack_support_waves_left$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_busy$"} {op "="} {value 1}}
					; Randomized 150-300s cadence: weighted cascade, ~20% per bucket.
					{"switch"
						{"case"
							{condition {type rand} {value 0.2}}
							{"delay" {time 150}}
						}
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 190}}
						}
						{"case"
							{condition {type rand} {value 0.33}}
							{"delay" {time 225}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 260}}
						}
						{"default"
							{"delay" {time 300}}
						}
					}
					{"delay" {time 0.1}}
					{"switch"
						; LIVE-UNIT CAP. Skip this cycle without consuming a wave while the
						; teammate still has a platoon on the field. Counted on the simple
						; selector form the mission scripts use for live units - the advanced
						; selector's prop/state decorations zero the match on these entities.
						{"case"
							{condition
								{type entities}
								{selector
									{ignore_captured_by_user 0}
									{tag attack_support_src}
									{type human}
									{state "not dead"}
								}
								{count {op ">"} {value 14}}
							}
							{"timer" {time 8} {title "ATTACK SUPPORT NEAR CAP DEFER"}}
						}
						{"default"
							{"set_i" {var "attack_support_wave_num$"} {op "+"} {value 1}}
							{"set_i" {var "attack_support_waves_left$"} {op "-"} {value 1}}
							("am_pick_composition")
						}
					}
					{"delay" {time 0.1}}
					{"set_i" {var "attack_support_busy$"} {op "="} {value 0}}
					{"trigger" {name "attack_support/clock"}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "attack_support_waves_left$"} {op "=="} {value 0}}
							{"timer" {time 10} {title "ATTACK SUPPORT WAVES EXHAUSTED"}}
						}
						{"default"}
					}
				}
			}

			; COMPOSITION 1 — 5 USMC infantry. Also the pool-short fallback, so this pool
			; is the deepest. Taking the bodies strips the pool tag, so the gate below can
			; simply count the tag to know what is still parked and available.
			{"attack_support/comp_usmc"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_usmc}}
							{count {op ">="} {value 5}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_stage$"} {op "="} {value 11}}
					{"timer" {time 8} {title "ATTACK SUPPORT WAVE - USMC RIFLE TEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_usmc}}}}
							{amount 5}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_usmc}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 12}}
				}
			}

			; COMPOSITION 2 — 5 1AD infantry.
			{"attack_support/comp_1ad"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_1ad}}
							{count {op ">="} {value 5}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_stage$"} {op "="} {value 21}}
					{"timer" {time 8} {title "ATTACK SUPPORT WAVE - 1AD RIFLE TEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_1ad}}}}
							{amount 5}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_1ad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 22}}
				}
			}

			; COMPOSITION 3 (level 2+) — 4 1AD infantry, then one crewed humvee behind
			; them. Infantry first: a vehicle placed into the same spot clips and flips.
			{"attack_support/comp_acav"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 3}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_1ad}}
							{count {op ">="} {value 4}}
						}
						{"5.cmp_i" {var "attack_support_hmmwv_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_stage$"} {op "="} {value 31}}
					{"timer" {time 8} {title "ATTACK SUPPORT WAVE - 1AD TEAM + HUMVEE"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_1ad}}}}
							{amount 4}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_1ad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"timer" {time 6} {title "ATTACK SUPPORT HUMVEE MOVING UP"}}
					("am_deploy_next_hmmwv")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 32}}
				}
			}

			; COMPOSITION 4 (level 3) — 6 German pzgren, then two crewed humvees one at a
			; time with a gap between them.
			{"attack_support/comp_pzgren"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 4}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_pzgd}}
							{count {op ">="} {value 6}}
						}
						{"5.cmp_i" {var "attack_support_hmmwv_left$"} {op ">"} {value 1}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_stage$"} {op "="} {value 41}}
					{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PZGREN + 2 HUMVEES"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_pzgd}}}}
							{amount 6}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_pzgd}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"timer" {time 6} {title "ATTACK SUPPORT HUMVEE 1 MOVING UP"}}
					("am_deploy_next_hmmwv")
					{"delay" {time 6}}
					{"timer" {time 6} {title "ATTACK SUPPORT HUMVEE 2 MOVING UP"}}
					("am_deploy_next_hmmwv")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 42}}
				}
			}
