; Attack support TEST waves (human ATTACK only).
; Timing: W1 @ 30s, W2 @ 90s, W3 @ 150s — gated by attack_support_wave_cmd$
; (waves must NOT auto-fire on entity presence alone — that stacked all 3).
; Movement: slow + {action move} (path), not sprint advance beeline.
; W3 humvees place one-at-a-time with gaps so they don't clip/flip.

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

			; Promote + own + slow MOVE (not advance sprint). Split targets for scatter.
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
					{tag_add attack_support_probe}
					{tag_add attack_support_owned}
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
				{"set_i" {var "attack_support_probe_transferred$"} {op "="} {value 1}}
			)

			; Wave clock. Arms once per attack mission, then fires W1/W2/W3 by setting
			; attack_support_wave_cmd$ and letting each wave's own condition match it.
			; attack_support_use_mi$ is the enable switch: attack_support.lua sets it
			; to 1 alongside id_attack_support$, because MI delivery is the only path
			; that works on the attack support slot (Lua Spawn reports no units available there).
			{"attack_support/schedule"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "attack_support_probe_started$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
					}
				}
				{actions
					{"set_i" {var "attack_support_probe_started$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_probe_stage$"} {op "="} {value 1}}
					{"timer" {time 10} {title "ATTACK SUPPORT WAVES ARMED (30/90/150)"}}
					{"delay" {time 30}}
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
					{"trigger" {name "attack_support/wave1"}}
					{"delay" {time 60}}
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 2}}
					{"trigger" {name "attack_support/wave2"}}
					{"delay" {time 60}}
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 3}}
					{"trigger" {name "attack_support/wave3"}}
				}
			}

			{"attack_support/wave1"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 1}}
						{"3.entities"
							{selector {tag attack_support_w1}}
							{count {op ">"} {value 0}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					{"timer" {time 8} {title "ATTACK SUPPORT WAVE 1 USMC"}}
					{"set_i" {var "attack_support_probe_stage$"} {op "="} {value 11}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_w1}}}}
						}
						{tag_add attack_support_deploy}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"set_i" {var "attack_support_probe_stage$"} {op "="} {value 12}}
				}
			}

			{"attack_support/wave2"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 2}}
						{"3.entities"
							{selector {tag attack_support_w2}}
							{count {op ">"} {value 0}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					{"timer" {time 8} {title "ATTACK SUPPORT WAVE 2 ACAV"}}
					{"set_i" {var "attack_support_probe_stage$"} {op "="} {value 21}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_w2}}}}
						}
						{tag_add attack_support_deploy}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"set_i" {var "attack_support_probe_stage$"} {op "="} {value 22}}
				}
			}

			{"attack_support/wave3"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 3}}
						{"3.entities"
							{selector {tag attack_support_w3}}
							{count {op ">"} {value 0}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					{"timer" {time 8} {title "ATTACK SUPPORT WAVE 3 GER INF"}}
					{"set_i" {var "attack_support_probe_stage$"} {op "="} {value 31}}
					; Infantry only first (not humvees).
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag attack_support_w3}}}
								{exclude
									{tag {tag attack_support_hmmwv1}}
									{tag {tag attack_support_hmmwv2}}
								}
							}
						}
						{tag_add attack_support_deploy}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"timer" {time 6} {title "ATTACK SUPPORT WAVE 3 HUMVEE 1"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_hmmwv1}}}}
						}
						{tag_add attack_support_deploy}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 6}}
					{"timer" {time 6} {title "ATTACK SUPPORT WAVE 3 HUMVEE 2"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_hmmwv2}}}}
						}
						{tag_add attack_support_deploy}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"set_i" {var "attack_support_probe_stage$"} {op "="} {value 32}}
					{"timer" {time 6} {title "ATTACK SUPPORT WAVES DONE"}}
				}
			}
