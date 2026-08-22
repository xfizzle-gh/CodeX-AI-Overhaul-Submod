; F0: native retreat_ratio is not how FE/Old Boy move Broken units. Rally move is the authority.
; Actor-scoped ratios are assist only. Writers must exclude aio_morale_owned. No player seizure.
; Surrender present is the Old Boy Player-0 5-step only (start_white_flag,
; runtime player 0, impregnability harmless, tag_remove enemy,
; inventory drop of the currently-used weapon (9fb440b double drop), stand_giveup_1).
; After P0, entity_state {impregnability harmless} (vanilla token from
; {Impregnability harmless}; do not use {impregnability full}). Keep for POW life.
; Evac stays tag-driven after a 3s pose, then {action move}. Ordinary POWs
; retarget to a live {tag aio_pow_camp} collection anchor; otherwise captor-rear
; waypoint. Ally/support keep waypoint egress. No drop-sensor/fight/select restack.
; 100s expire stays as a backstop. Do not restack fight/select/drop-sensor/control AI.
; ID assign uses evac switch/cmp_i on aio_pow_next_id$. Do not use entities+count in that switch.
			{"conquest_enhanced_mechanics/broken/acquire"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_broken}
										}
									}
									{include
										{prop
											{prop human}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state user_control}
										}
										{tag
											{tag player}
										}
										{tag
											{tag aio_morale_surrendering}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_broken}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
									{tag
										{tag aio_morale_owned}
									}
								}
							}
						}
						{drop orders}
						{move_mode free}
						{ai_move
							{mode enable}
						}
						{ai
							{advance_ratio "0.1"}
							{retreat_ratio 4}
						}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_broken}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{tag_add aio_morale_owned}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_owned}
									}
								}
								{include
									{tag
										{tag aio_morale_broken}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_add aio_morale_regrouping}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_owned}
									}
								}
								{include
									{tag
										{tag aio_morale_broken}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
								}
							}
						}
						{move_mode free}
						{ai_move
							{mode enable}
						}
						{ai
							{advance_ratio "0.1"}
							{retreat_ratio 4}
						}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_owned}
									}
								}
								{include
									{tag
										{tag aio_morale_broken}
									}
								}
							{exclude
								{state
									{state dead}
								}
								{state
									{state inactive}
								}
								{state
									{state user_control}
								}
								{tag
									{tag aio_morale_watching_regroup}
								}
							}
							}
						}
						{effect aio_morale_watch_regroup}
					}
					{"set_i"
						{var "ce_morale_diag_broken$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/acquire"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/rally"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_owned}
										}
									}
									{include
										{tag
											{tag aio_morale_broken}
										}
										{prop
											{prop human}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state user_control}
										}
										{tag
											{tag aio_morale_surrendering}
										}
										{tag
											{tag aio_morale_moving_to_rally}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_owned}
									}
								}
								{include
									{tag
										{tag aio_morale_broken}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag aio_morale_moving_to_rally}
									}
								}
							}
						}
						{drop "orders sensor senseless"}
						{action move}
						{target
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_junior}
									}
									{tag
										{tag aio_cmd_primary}
									}
									{tag
										{tag aio_cmd_senior}
									}
								}
								{include
									{prop
										{prop human}
									}
									{relation
										{relation ally}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
								}
							}
							{sort
								{type entity}
								{mode nearest}
							}
							{amount 1}
						}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_owned}
									}
								}
								{include
									{tag
										{tag aio_morale_broken}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
								}
							}
						}
						{effect aio_morale_moving_to_rally}
					}
					{"set_i"
						{var "ce_morale_diag_retreat$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 8}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/rally"}
					}
				}
			}

			{"conquest_enhanced_mechanics/broken/recover"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_broken}
										}
									}
									{include
										{tag
											{tag aio_cmd_linked}
										}
										{prop
											{prop human}
										}
									}
									{exclude
										{state
											{state suppressed}
										}
										{state
											{state dead}
										}
										{tag
											{tag aio_morale_recovering_from_broken}
										}
										{tag
											{tag aio_morale_surrendering}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_broken}
									}
								}
								{include
									{tag
										{tag aio_cmd_linked}
									}
								}
							{exclude
								{state
									{state dead}
								}
								{state
									{state inactive}
								}
								{state
									{state suppressed}
								}
								{tag
									{tag aio_morale_recovering_from_broken}
								}
								{tag
									{tag aio_morale_surrendering}
								}
							}
							}
						}
						{effect recovering_from_broken}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/recover"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/reacquire"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_broken}
										}
									}
									{include
										{tag
											{tag aio_cmd_linked}
										}
										{tag
											{tag aio_morale_regroup_failed}
										}
										{prop
											{prop human}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state user_control}
										}
										{tag
											{tag player}
										}
										{tag
											{tag aio_morale_surrendering}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_broken}
									}
								}
								{include
									{tag
										{tag aio_cmd_linked}
									}
									{tag
										{tag aio_morale_regroup_failed}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
									{tag
										{tag aio_morale_surrendering}
									}
								}
							}
						}
						{tag_remove aio_morale_regroup_failed}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_cand}
									}
								}
								{include
									{tag
										{tag aio_morale_broken}
									}
									{tag
										{tag aio_cmd_linked}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
									{tag
										{tag aio_morale_surrendering}
									}
								}
							}
						}
						{tag_remove aio_morale_surrender_cand}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/reacquire"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/surrender"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.see_actors"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_broken}
										}
									}
									{include
										{tag
											{tag aio_morale_regroup_failed}
										}
										{prop
											{prop human}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state user_control}
										}
										{tag
											{tag player}
										}
									{tag
										{tag aio_morale_surrendering}
									}
									{tag
										{tag aio_cmd_linked}
									}
								}
							}
						}
						{enemy
							{source advanced}
							{group
								{select
									{prop
										{prop human}
									}
								}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
									}
								}
							}
							{distance
								{mode near_than}
								{meters 30}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_morale_surrender_cand}
							}
						}
					}
				}
				{actions
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_cand}
									}
								}
							{include
								{tag
									{tag aio_morale_broken}
								}
								{tag
									{tag aio_morale_regroup_failed}
								}
							}
							{exclude
								{state
									{state dead}
								}
								{state
									{state inactive}
								}
								{state
									{state user_control}
								}
								{tag
									{tag player}
								}
								{tag
									{tag aio_morale_surrendering}
								}
								{tag
									{tag aio_cmd_linked}
								}
							}
							}
						}
						{effect aio_morale_surrender}
					}
					{"delay"
						{time 0.2}
					}
					{"entity_state"
						{selector
							{tag aio_morale_surrender_cand}
						}
						{tag_remove aio_morale_surrender_cand}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/surrender"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/surrender_diag_assign"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_pow_need_id}
										}
									}
									{include
										{tag
											{tag aio_morale_surrendering}
										}
										{prop
											{prop human}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{tag
											{tag aio_pow_did}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_diag_assign$"}
						{op "="}
						{value 1}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 0}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d01}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 1}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d02}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 2}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 2}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d03}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 3}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 3}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d04}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 4}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 4}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d05}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 5}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 5}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d06}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 6}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 6}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d07}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 7}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 7}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d08}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 8}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 8}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d09}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 9}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 9}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d10}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 10}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 10}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d11}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 11}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 11}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d12}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 12}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 12}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d13}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 13}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 13}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d14}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 14}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 14}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d15}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 15}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "aio_pow_next_id$"} {op "=="} {value 15}}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_d16}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_last_id$"}
								{op "="}
								{value 16}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
						{"default"
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_need_id}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{tag
												{tag aio_pow_did}
											}
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
									{amount 1}
								}
								{tag_add aio_pow_assigning}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_overflow}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_add aio_pow_did}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_need_id}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_assigning}
											}
										}
									}
								}
								{tag_remove aio_pow_assigning}
							}
							{"set_i"
								{var "aio_pow_next_id$"}
								{op "+"}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_last_evt$"}
								{op "="}
								{value 1}
							}
							{"set_i"
								{var "aio_pow_seq$"}
								{op "+"}
								{value 1}
							}
						}
					}
					{"delay"
						{time 0.05}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/surrender_diag_assign"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/surrender_present"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_surrendering}
										}
									}
									{include
										{prop
											{prop human}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state user_control}
										}
										{tag
											{tag player}
										}
										{tag
											{tag aio_morale_surrender_fx}
										}
										{tag
											{tag aio_morale_surrender_presenting}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_diag_present$"}
						{op "="}
						{value 1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
									{tag
										{tag aio_morale_surrender_fx}
									}
									{tag
										{tag aio_morale_surrender_presenting}
									}
								}
							}
						}
						{tag_add aio_morale_surrender_presenting}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_presenting}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{effect start_white_flag}
					}
					{"delay"
						{time 0.05}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_presenting}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{operation set}
						{player "0"}
					}
					{"set_i"
						{var "ce_morale_diag_p0$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 0.05}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_presenting}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{tag_add aio_pow_post_p0}
						{impregnability harmless}
					}
					{"set_i"
						{var "ce_morale_diag_impregnable$"}
						{op "="}
						{value 1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_presenting}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{tag_add aio_pow_evt_p0}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_presenting}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{tag_remove enemy}
					}
					{"delay"
						{time 0.05}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_presenting}
									}
								}
								{include
									{with_item
										{type using}
										{item "weapon"}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
									{tag
										{tag aio_morale_surrender_fx}
									}
								}
							}
						}
						{action drop}
					}
					{"delay"
						{time 0.2}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_presenting}
									}
								}
								{include
									{with_item
										{type using}
										{item "weapon"}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
									{tag
										{tag aio_morale_surrender_fx}
									}
								}
							}
						}
						{action drop}
					}
					{"set_i"
						{var "ce_morale_diag_drop$"}
						{op "="}
						{value 1}
					}
					{"set_i"
						{var "aio_pow_last_evt$"}
						{op "="}
						{value 2}
					}
					{"set_i"
						{var "aio_pow_seq$"}
						{op "+"}
						{value 1}
					}
					{"delay"
						{time 0.05}
					}
					{"animation"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_presenting}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{collage stand_giveup_1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_presenting}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{tag_add aio_morale_surrender_fx}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_fx}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_add aio_pow_evt_present_done}
					}
					{"set_i"
						{var "aio_pow_last_evt$"}
						{op "="}
						{value 3}
					}
					{"set_i"
						{var "aio_pow_seq$"}
						{op "+"}
						{value 1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_presenting}
									}
								}
							}
						}
						{tag_remove aio_morale_surrender_presenting}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/surrender_present"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/surrender_evacuate"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_surrender_fx}
										}
									}
									{include
										{tag
											{tag aio_morale_surrendering}
										}
										{prop
											{prop human}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state user_control}
										}
										{tag
											{tag player}
										}
										{tag
											{tag aio_morale_surrender_evacuating}
										}
										{tag
											{tag prisoner_in_camp}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_fx}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
									{tag
										{tag aio_morale_surrender_evacuating}
									}
									{tag
										{tag prisoner_in_camp}
									}
								}
							}
						}
						{tag_add aio_morale_surrender_evacuating}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_evacuating}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_add aio_pow_evt_evac}
					}
					{"set_i"
						{var "aio_pow_last_evt$"}
						{op "="}
						{value 4}
					}
					{"set_i"
						{var "aio_pow_seq$"}
						{op "+"}
						{value 1}
					}
					{"delay"
						{time 3}
					}
					{"switch"
							{"case"
								{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
								{"entity_state"
									{selector
										{source advanced}
									{group
										{select
											{tag
												{tag aio_morale_surrender_evacuating}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{tag
												{tag _user_ally}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{state
												{state user_control}
											}
											{tag
												{tag player}
											}
										}
									}
									{group
										{select
											{tag
												{tag aio_morale_surrender_evacuating}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{tag
												{tag def_sup_src}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{state
												{state user_control}
											}
											{tag
												{tag player}
											}
										}
									}
									}
									{tag_add aio_morale_surrender_to_a}
								}
								{"entity_state"
									{selector
										{source advanced}
										{group
											{select
												{tag
													{tag aio_morale_surrender_to_a}
												}
											}
											{include
												{tag
													{tag aio_morale_surrender_evacuating}
												}
												{tag
													{tag aio_morale_surrendering}
												}
											}
											{exclude
												{state
													{state dead}
												}
												{state
													{state inactive}
												}
											}
										}
									}
									{tag_add aio_pow_evt_move_a}
								}
								{"set_i"
									{var "aio_pow_last_evt$"}
									{op "="}
									{value 5}
								}
								{"set_i"
									{var "aio_pow_seq$"}
									{op "+"}
									{value 1}
								}
								{"action"
									{selector
										{source advanced}
									{group
										{select
											{tag
												{tag aio_morale_surrender_evacuating}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{tag
												{tag _user_ally}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{state
												{state user_control}
											}
											{tag
												{tag player}
											}
										}
									}
									{group
										{select
											{tag
												{tag aio_morale_surrender_evacuating}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{tag
												{tag def_sup_src}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{state
												{state user_control}
											}
											{tag
												{tag player}
											}
										}
									}
									}
									{action move}
									{waypoint "attack_support_entry_a"}
								}
								{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_pow_camp}
													}
												}
											}
										}
										{count
											{op ">="}
											{value 1}
										}
									}
									{"entity_state"
										{selector
											{source advanced}
										{group
											{select
												{tag
													{tag aio_morale_surrender_evacuating}
												}
											}
											{include
												{tag
													{tag aio_morale_surrendering}
												}
											}
											{exclude
												{state
													{state dead}
												}
												{state
													{state inactive}
												}
												{state
													{state user_control}
												}
												{tag
													{tag player}
												}
												{tag
													{tag _user_ally}
												}
												{tag
													{tag def_sup_src}
												}
												{tag
													{tag prisoner_in_camp}
												}
											}
										}
										}
										{tag_add aio_morale_surrender_to_camp}
									}
									{"action"
										{selector
											{source advanced}
										{group
											{select
												{tag
													{tag aio_morale_surrender_to_camp}
												}
											}
											{include
												{tag
													{tag aio_morale_surrendering}
												}
												{tag
													{tag aio_morale_surrender_evacuating}
												}
											}
											{exclude
												{state
													{state dead}
												}
												{state
													{state inactive}
												}
												{state
													{state user_control}
												}
												{tag
													{tag player}
												}
												{tag
													{tag prisoner_in_camp}
												}
											}
										}
										}
										{action move}
										{target
										{source advanced}
										{amount 1
										}
										{group
											{select
												{tag
													{tag aio_pow_camp}
												}
											}
										}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
									{"entity_state"
										{selector
											{source advanced}
										{group
											{select
												{tag
													{tag aio_morale_surrender_evacuating}
												}
											}
											{include
												{tag
													{tag aio_morale_surrendering}
												}
											}
											{exclude
												{state
													{state dead}
												}
												{state
													{state inactive}
												}
												{state
													{state user_control}
												}
												{tag
													{tag player}
												}
												{tag
													{tag _user_ally}
												}
												{tag
													{tag def_sup_src}
												}
												{tag
													{tag prisoner_in_camp}
												}
											}
										}
										}
										{tag_add aio_morale_surrender_to_b}
									}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_surrender_to_b}
													}
												}
												{include
													{tag
														{tag aio_morale_surrender_evacuating}
													}
													{tag
														{tag aio_morale_surrendering}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
												}
											}
										}
										{tag_add aio_pow_evt_move_b}
									}
									{"set_i"
										{var "aio_pow_last_evt$"}
										{op "="}
										{value 6}
									}
									{"set_i"
										{var "aio_pow_seq$"}
										{op "+"}
										{value 1}
									}
									{"action"
										{selector
											{source advanced}
										{group
											{select
												{tag
													{tag aio_morale_surrender_evacuating}
												}
											}
											{include
												{tag
													{tag aio_morale_surrendering}
												}
											}
											{exclude
												{state
													{state dead}
												}
												{state
													{state inactive}
												}
												{state
													{state user_control}
												}
												{tag
													{tag player}
												}
												{tag
													{tag _user_ally}
												}
												{tag
													{tag def_sup_src}
												}
												{tag
													{tag prisoner_in_camp}
												}
											}
										}
										}
										{action move}
										{waypoint "attack_support_entry_b"}
									}
								}
								}
							}
							{"case"
								{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
								{"entity_state"
									{selector
										{source advanced}
									{group
										{select
											{tag
												{tag aio_morale_surrender_evacuating}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{tag
												{tag _user_ally}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{state
												{state user_control}
											}
											{tag
												{tag player}
											}
										}
									}
									{group
										{select
											{tag
												{tag aio_morale_surrender_evacuating}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{tag
												{tag def_sup_src}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{state
												{state user_control}
											}
											{tag
												{tag player}
											}
										}
									}
									}
									{tag_add aio_morale_surrender_to_b}
								}
								{"entity_state"
									{selector
										{source advanced}
										{group
											{select
												{tag
													{tag aio_morale_surrender_to_b}
												}
											}
											{include
												{tag
													{tag aio_morale_surrender_evacuating}
												}
												{tag
													{tag aio_morale_surrendering}
												}
											}
											{exclude
												{state
													{state dead}
												}
												{state
													{state inactive}
												}
											}
										}
									}
									{tag_add aio_pow_evt_move_b}
								}
								{"set_i"
									{var "aio_pow_last_evt$"}
									{op "="}
									{value 6}
								}
								{"set_i"
									{var "aio_pow_seq$"}
									{op "+"}
									{value 1}
								}
								{"action"
									{selector
										{source advanced}
									{group
										{select
											{tag
												{tag aio_morale_surrender_evacuating}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{tag
												{tag _user_ally}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{state
												{state user_control}
											}
											{tag
												{tag player}
											}
										}
									}
									{group
										{select
											{tag
												{tag aio_morale_surrender_evacuating}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
											}
											{tag
												{tag def_sup_src}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{state
												{state inactive}
											}
											{state
												{state user_control}
											}
											{tag
												{tag player}
											}
										}
									}
									}
									{action move}
									{waypoint "attack_support_entry_b"}
								}
								{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_pow_camp}
													}
												}
											}
										}
										{count
											{op ">="}
											{value 1}
										}
									}
									{"entity_state"
										{selector
											{source advanced}
										{group
											{select
												{tag
													{tag aio_morale_surrender_evacuating}
												}
											}
											{include
												{tag
													{tag aio_morale_surrendering}
												}
											}
											{exclude
												{state
													{state dead}
												}
												{state
													{state inactive}
												}
												{state
													{state user_control}
												}
												{tag
													{tag player}
												}
												{tag
													{tag _user_ally}
												}
												{tag
													{tag def_sup_src}
												}
												{tag
													{tag prisoner_in_camp}
												}
											}
										}
										}
										{tag_add aio_morale_surrender_to_camp}
									}
									{"action"
										{selector
											{source advanced}
										{group
											{select
												{tag
													{tag aio_morale_surrender_to_camp}
												}
											}
											{include
												{tag
													{tag aio_morale_surrendering}
												}
												{tag
													{tag aio_morale_surrender_evacuating}
												}
											}
											{exclude
												{state
													{state dead}
												}
												{state
													{state inactive}
												}
												{state
													{state user_control}
												}
												{tag
													{tag player}
												}
												{tag
													{tag prisoner_in_camp}
												}
											}
										}
										}
										{action move}
										{target
										{source advanced}
										{amount 1
										}
										{group
											{select
												{tag
													{tag aio_pow_camp}
												}
											}
										}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
									{"entity_state"
										{selector
											{source advanced}
										{group
											{select
												{tag
													{tag aio_morale_surrender_evacuating}
												}
											}
											{include
												{tag
													{tag aio_morale_surrendering}
												}
											}
											{exclude
												{state
													{state dead}
												}
												{state
													{state inactive}
												}
												{state
													{state user_control}
												}
												{tag
													{tag player}
												}
												{tag
													{tag _user_ally}
												}
												{tag
													{tag def_sup_src}
												}
												{tag
													{tag prisoner_in_camp}
												}
											}
										}
										}
										{tag_add aio_morale_surrender_to_a}
									}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_surrender_to_a}
													}
												}
												{include
													{tag
														{tag aio_morale_surrender_evacuating}
													}
													{tag
														{tag aio_morale_surrendering}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
												}
											}
										}
										{tag_add aio_pow_evt_move_a}
									}
									{"set_i"
										{var "aio_pow_last_evt$"}
										{op "="}
										{value 5}
									}
									{"set_i"
										{var "aio_pow_seq$"}
										{op "+"}
										{value 1}
									}
									{"action"
										{selector
											{source advanced}
										{group
											{select
												{tag
													{tag aio_morale_surrender_evacuating}
												}
											}
											{include
												{tag
													{tag aio_morale_surrendering}
												}
											}
											{exclude
												{state
													{state dead}
												}
												{state
													{state inactive}
												}
												{state
													{state user_control}
												}
												{tag
													{tag player}
												}
												{tag
													{tag _user_ally}
												}
												{tag
													{tag def_sup_src}
												}
												{tag
													{tag prisoner_in_camp}
												}
											}
										}
										}
										{action move}
										{waypoint "attack_support_entry_a"}
									}
								}
								}
							}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/surrender_evacuate"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/surrender_arrive_a"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.see_actors"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_surrender_to_a}
										}
									}
									{include
										{tag
											{tag aio_morale_surrendering}
										}
										{tag
											{tag aio_morale_surrender_evacuating}
										}
										{prop
											{prop human}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state user_control}
										}
										{tag
											{tag player}
										}
									}
								}
							}
							{enemy
								{source advanced}
								{group
									{select
										{tag
											{tag spawn_a}
										}
									}
								}
							}
							{distance
								{mode near_than}
								{meters 200}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_morale_surrender_at_egress}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_at_egress}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{tag_add aio_pow_evt_arrive}
					}
					{"set_i"
						{var "aio_pow_last_evt$"}
						{op "="}
						{value 7}
					}
					{"set_i"
						{var "aio_pow_seq$"}
						{op "+"}
						{value 1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_at_egress}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{tag_add aio_pow_evt_delete}
					}
					{"set_i"
						{var "aio_pow_last_evt$"}
						{op "="}
						{value 9}
					}
					{"set_i"
						{var "aio_pow_seq$"}
						{op "+"}
						{value 1}
					}
					{"delete"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_at_egress}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag aio_morale_surrender_at_egress}
						}
						{tag_remove aio_morale_surrender_at_egress}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/surrender_arrive_a"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/surrender_arrive_b"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.see_actors"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_surrender_to_b}
										}
									}
									{include
										{tag
											{tag aio_morale_surrendering}
										}
										{tag
											{tag aio_morale_surrender_evacuating}
										}
										{prop
											{prop human}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state user_control}
										}
										{tag
											{tag player}
										}
									}
								}
							}
							{enemy
								{source advanced}
								{group
									{select
										{tag
											{tag spawn_b}
										}
									}
								}
							}
							{distance
								{mode near_than}
								{meters 200}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_morale_surrender_at_egress}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_at_egress}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{tag_add aio_pow_evt_arrive}
					}
					{"set_i"
						{var "aio_pow_last_evt$"}
						{op "="}
						{value 7}
					}
					{"set_i"
						{var "aio_pow_seq$"}
						{op "+"}
						{value 1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_at_egress}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{tag_add aio_pow_evt_delete}
					}
					{"set_i"
						{var "aio_pow_last_evt$"}
						{op "="}
						{value 9}
					}
					{"set_i"
						{var "aio_pow_seq$"}
						{op "+"}
						{value 1}
					}
					{"delete"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_at_egress}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
					}
					{"entity_state"
						{selector
							{tag aio_morale_surrender_at_egress}
						}
						{tag_remove aio_morale_surrender_at_egress}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/surrender_arrive_b"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/surrender_expire"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_surrender_expire}
										}
									}
									{include
										{tag
											{tag aio_morale_surrendering}
										}
										{prop
											{prop human}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state user_control}
										}
										{tag
											{tag player}
										}
										{tag
											{tag prisoner_in_camp}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_expire}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
									{tag
										{tag prisoner_in_camp}
									}
								}
							}
						}
						{tag_add aio_pow_evt_expire}
					}
					{"set_i"
						{var "aio_pow_last_evt$"}
						{op "="}
						{value 8}
					}
					{"set_i"
						{var "aio_pow_seq$"}
						{op "+"}
						{value 1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_expire}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
									{tag
										{tag prisoner_in_camp}
									}
								}
							}
						}
						{tag_add aio_pow_evt_delete}
					}
					{"set_i"
						{var "aio_pow_last_evt$"}
						{op "="}
						{value 9}
					}
					{"set_i"
						{var "aio_pow_seq$"}
						{op "+"}
						{value 1}
					}
					{"delete"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_expire}
									}
								}
								{include
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
									{tag
										{tag prisoner_in_camp}
									}
								}
							}
						}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/surrender_expire"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/observe_surrender"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_surrendering}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_diag_surrender$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/observe_surrender"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/cleanup_dead"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_owned}
										}
										{tag
											{tag aio_morale_broken}
										}
										{tag
											{tag aio_morale_surrendering}
										}
										{tag
											{tag aio_morale_surrender_cand}
										}
										{tag
											{tag aio_morale_watching_regroup}
										}
										{tag
											{tag aio_morale_surrender_fx}
										}
										{tag
											{tag aio_morale_surrender_presenting}
										}
										{tag
											{tag aio_morale_surrender_evacuating}
										}
									}
									{include
										{state
											{state dead}
										}
									}
								}
								{group
									{select
										{tag
											{tag aio_morale_owned}
										}
										{tag
											{tag aio_morale_broken}
										}
										{tag
											{tag aio_morale_surrendering}
										}
										{tag
											{tag aio_morale_surrender_cand}
										}
										{tag
											{tag aio_morale_watching_regroup}
										}
										{tag
											{tag aio_morale_surrender_fx}
										}
										{tag
											{tag aio_morale_surrender_presenting}
										}
										{tag
											{tag aio_morale_surrender_evacuating}
										}
									}
									{include
										{state
											{state inactive}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_owned}
									}
									{tag
										{tag aio_morale_broken}
									}
									{tag
										{tag aio_morale_surrendering}
									}
									{tag
										{tag aio_morale_surrender_cand}
									}
									{tag
										{tag aio_morale_regrouping}
									}
									{tag
										{tag aio_morale_regroup_failed}
									}
									{tag
										{tag aio_morale_moving_to_rally}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_owned}
									}
									{tag
										{tag aio_morale_broken}
									}
									{tag
										{tag aio_morale_surrendering}
									}
									{tag
										{tag aio_morale_surrender_cand}
									}
									{tag
										{tag aio_morale_regrouping}
									}
									{tag
										{tag aio_morale_regroup_failed}
									}
									{tag
										{tag aio_morale_moving_to_rally}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_owned}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_broken}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_broken}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_broken}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_surrendering}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_surrendering}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_cand}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_surrender_cand}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_surrender_cand}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_regrouping}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_regrouping}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_regrouping}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_regroup_failed}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_regroup_failed}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_regroup_failed}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_moving_to_rally}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_moving_to_rally}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_moving_to_rally}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_watching_regroup}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_watching_regroup}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_watching_regroup}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_fx}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_surrender_fx}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_surrender_fx}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_presenting}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_surrender_presenting}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_surrender_presenting}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_expire}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_surrender_expire}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_surrender_expire}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_evacuating}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_surrender_evacuating}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_surrender_evacuating}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_at_egress}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_surrender_at_egress}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_surrender_at_egress}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_to_a}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_surrender_to_a}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_surrender_to_a}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_to_b}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_surrender_to_b}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_surrender_to_b}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_surrender_to_camp}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_surrender_to_camp}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_surrender_to_camp}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag prisoner_in_camp}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag prisoner_in_camp}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove prisoner_in_camp}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/cleanup_dead"}
					}
				}
			}
