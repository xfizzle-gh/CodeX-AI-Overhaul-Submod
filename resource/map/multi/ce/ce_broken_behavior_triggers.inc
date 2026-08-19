; F0: native retreat_ratio is not how FE/Old Boy move Broken units. Rally move is the authority.
; Actor-scoped ratios are assist only. Writers must exclude aio_morale_owned. No player seizure.
; Surrender: drop + looping walk_giveup_1, then flee to captor entry. 75s delete fail-closed.
; Ally/support invert. Spawnside 0 issues no move. No repeating actor_state.
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
					{"actor_state"
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
								}
							}
						}
						{drop "orders sensor senseless"}
						{fire_mode hold}
						{move_mode hold}
						{weapon_prepare off}
						{ai_move
							{mode disable}
						}
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
						{drop "orders sensor senseless"}
						{operation set}
						{player "0"}
					}
					{"actor_state"
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
						{drop "orders sensor senseless"}
						{control AI}
						{fire_mode hold}
						{weapon_prepare off}
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
					{"animation"
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
						{drop orders}
						{collage walk_giveup_1}
						{cicledelay 0}
						{delay 0}
						{totalTime 99999}
						{flags loop}
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
								}
							}
						}
						{tag_add aio_morale_surrender_evacuating}
					}
					{"delay"
						{time 5}
					}
					{"actor_state"
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
								}
							}
						}
						{drop "orders sensor senseless"}
						{fire_mode hold}
						{move_mode free}
						{weapon_prepare off}
						{ai_move
							{mode enable}
						}
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
										}
									}
								}
								{tag_add aio_morale_surrender_to_b}
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
								{drop "orders sensor senseless"}
								{action move}
								{waypoint "attack_support_entry_a"}
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
										}
									}
								}
								{drop "orders sensor senseless"}
								{action move}
								{waypoint "attack_support_entry_b"}
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
										}
									}
								}
								{tag_add aio_morale_surrender_to_a}
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
								{drop "orders sensor senseless"}
								{action move}
								{waypoint "attack_support_entry_b"}
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
										}
									}
								}
								{drop "orders sensor senseless"}
								{action move}
								{waypoint "attack_support_entry_a"}
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
									}
								}
							}
						}
					}
				}
				{actions
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
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/cleanup_dead"}
					}
				}
			}
