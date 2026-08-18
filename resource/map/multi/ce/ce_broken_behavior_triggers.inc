; F0: native retreat_ratio is not how FE/Old Boy move Broken units. Rally move is the authority.
; Actor-scoped ratios are assist only. Writers must exclude aio_morale_owned. No player seizure.
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
									}
									{include
										{state
											{state dead}
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
						}
						{tag_remove aio_morale_regroup_failed}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/cleanup_dead"}
					}
				}
			}
