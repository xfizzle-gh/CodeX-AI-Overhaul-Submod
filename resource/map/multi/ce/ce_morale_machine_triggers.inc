; Combined morale machine. AI-only. Surrender is last. No player seizure/delete.
			{"conquest_enhanced_mechanics/morale/cmd_present"
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
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_diag_cmd_link$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/cmd_present"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/pressure_see_enemy"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.see_enemy"
							{selector
								{source advanced}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{state
											{state operatable}
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
											{tag _user}
										}
										{tag
											{tag aio_morale_broken}
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
					{"set_i"
						{var "ce_morale_diag_pressure$"}
						{op "="}
						{value 1}
					}
					{"set_i"
						{var "ce_morale_diag_ai_human$"}
						{op "="}
						{value 1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{state
										{state operatable}
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
										{tag _user}
									}
									{tag
										{tag aio_morale_shaken}
									}
									{tag
										{tag aio_morale_panic}
									}
									{tag
										{tag aio_morale_broken}
									}
									{tag
										{tag aio_steadfast}
									}
								}
							}
						}
						{tag_add aio_morale_shaken}
					}
					{"delay"
						{time 3}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/pressure_see_enemy"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/escalate_panic"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.see_enemy"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_shaken}
										}
									}
									{include
										{prop
											{prop human}
										}
										{state
											{state operatable}
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
											{tag aio_morale_panic}
										}
										{tag
											{tag aio_morale_broken}
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
										{tag aio_morale_shaken}
									}
								}
								{include
									{prop
										{prop human}
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
										{tag aio_morale_broken}
									}
								}
							}
						}
						{tag_add aio_morale_panic}
						{tag_remove aio_morale_shaken}
					}
					{"set_i"
						{var "ce_morale_diag_panic$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 4}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/escalate_panic"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/escalate_broken"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.see_enemy"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_panic}
										}
									}
									{include
										{prop
											{prop human}
										}
										{state
											{state operatable}
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
											{tag aio_cmd_linked}
										}
										{tag
											{tag aio_morale_broken}
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
										{tag aio_morale_panic}
									}
								}
								{include
									{prop
										{prop human}
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
										{tag aio_cmd_linked}
									}
								}
							}
						}
						{tag_add aio_morale_broken}
						{tag_remove aio_morale_panic}
					}
					{"set_i"
						{var "ce_morale_diag_broken$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 3}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/escalate_broken"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/broken_retreat"
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
										{state
											{state operatable}
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
											{tag aio_morale_retreating}
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
									{prop
										{prop human}
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
						{tag_add aio_morale_retreating}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_retreating}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state user_control}
									}
								}
							}
						}
						{drop orders}
						{ai
							{advance_ratio 8}
							{retreat_ratio 1}
						}
					}
					{"set_i"
						{var "ce_morale_diag_retreat$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 4}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/broken_retreat"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/retreat_to_surrender"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.see_enemy"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_retreating}
										}
									}
									{include
										{prop
											{prop human}
										}
										{state
											{state operatable}
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
											{tag aio_cmd_linked}
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
										{tag aio_morale_retreating}
									}
								}
								{include
									{prop
										{prop human}
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
										{tag aio_cmd_linked}
									}
								}
							}
						}
						{tag_add aio_morale_surrendering}
						{tag_remove aio_morale_retreating}
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
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state user_control}
									}
								}
							}
						}
						{drop orders}
					}
					{"set_i"
						{var "ce_morale_diag_surrender$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 5}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/retreat_to_surrender"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/recover_shaken"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "ce_morale_diag_pressure$"}
							{op "="}
							{value 0}
						}
						{"3.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_shaken}
										}
									}
									{include
										{prop
											{prop human}
										}
									}
									{exclude
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
					{"delay"
						{time 12}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_shaken}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{tag_remove aio_morale_shaken}
					}
					{"set_i"
						{var "ce_morale_diag_recover$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 5}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/recover_shaken"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/sys_autodemo"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "enable_ce_morale_autodemo$"}
							{op ">"}
							{value 0}
						}
						{"3.cmp_i"
							{var "ce_morale_sys_done$"}
							{op "="}
							{value 0}
						}
						{"4.entities"
							{selector
								{source advanced}
								{group
									{select
										{prop
											{prop human}
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
											{tag _user}
										}
									}
								}
								{group
									{select
										{tag
											{tag bot}
										}
										{tag
											{tag _bot}
										}
										{tag
											{tag _ai}
										}
									}
									{include
										{prop
											{prop human}
										}
									}
									{exclude
										{state
											{state user_control}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_diag_ai_human$"}
						{op "="}
						{value 1}
					}
					{"set_i"
						{var "ce_morale_diag_shaken$"}
						{op "="}
						{value 1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{prop
										{prop human}
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
										{tag _user}
									}
								}
							}
							{amount 1}
						}
						{tag_add aio_morale_shaken}
					}
					{"delay"
						{time 1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_shaken}
									}
								}
								{exclude
									{state
										{state user_control}
									}
								}
							}
							{amount 1}
						}
						{tag_remove aio_morale_shaken}
					}
					{"set_i"
						{var "ce_morale_diag_recover$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 0.5}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{prop
										{prop human}
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
										{tag _user}
									}
								}
							}
							{amount 1}
						}
						{tag_add aio_morale_panic}
					}
					{"set_i"
						{var "ce_morale_diag_panic$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 0.5}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_panic}
									}
								}
								{exclude
									{state
										{state user_control}
									}
								}
							}
							{amount 1}
						}
						{tag_add aio_morale_broken}
						{tag_remove aio_morale_panic}
					}
					{"set_i"
						{var "ce_morale_diag_broken$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 0.5}
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
								{exclude
									{state
										{state user_control}
									}
								}
							}
							{amount 1}
						}
						{tag_add aio_morale_retreating}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_retreating}
									}
								}
								{exclude
									{state
										{state user_control}
									}
								}
							}
							{amount 1}
						}
						{drop orders}
						{ai
							{advance_ratio 8}
							{retreat_ratio 1}
						}
					}
					{"set_i"
						{var "ce_morale_diag_retreat$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_retreating}
									}
								}
								{exclude
									{state
										{state user_control}
									}
								}
							}
							{amount 1}
						}
						{tag_add aio_morale_surrendering}
						{tag_remove aio_morale_retreating}
					}
					{"set_i"
						{var "ce_morale_diag_surrender$"}
						{op "="}
						{value 1}
					}
					{"set_i"
						{var "ce_morale_sys_done$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/sys_autodemo"}
					}
				}
			}
