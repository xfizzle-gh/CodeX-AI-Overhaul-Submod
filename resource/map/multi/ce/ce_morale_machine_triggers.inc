; PR C: Steady -> Shaken -> Panic, then Panic -> Shaken -> Steady.
; Production pressure is native {state suppressed}, proven in Conquest game.log.
; Age tags tick everyone together, oldest first. Players may be Shaken/Panic.
			{"conquest_enhanced_mechanics/morale/observe_ai"
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
											{tag _bot}
										}
										{tag
											{tag bot}
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
								}
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
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/observe_ai"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/shaken_entry"
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
										{prop
											{prop human}
										}
									}
									{include
										{state
											{state suppressed}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{tag
											{tag aio_morale_shaken}
										}
										{tag
											{tag aio_morale_panic}
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
						{var "ce_morale_diag_suppressed_state$"}
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
								{include
									{state
										{state suppressed}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag aio_morale_shaken}
									}
									{tag
										{tag aio_morale_panic}
									}
								}
							}
						}
						{tag_add aio_morale_shaken}
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
										{state suppressed}
									}
									{tag
										{tag aio_morale_shaken}
									}
								}
								{exclude
									{tag
										{tag aio_morale_panic}
									}
									{tag
										{tag aio_morale_just_shaken}
									}
								}
							}
						}
						{tag_add aio_morale_just_shaken}
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
										{state suppressed}
									}
									{tag
										{tag aio_morale_just_shaken}
									}
								}
							}
						}
						{tag_add aio_morale_j0}
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
										{state suppressed}
									}
									{tag
										{tag aio_morale_shaken}
									}
								}
								{exclude
									{tag
										{tag aio_morale_panic}
									}
								}
							}
						}
						{tag_add aio_morale_recent_pressure}
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
										{state suppressed}
									}
									{tag
										{tag aio_morale_shaken}
									}
								}
								{exclude
									{tag
										{tag aio_morale_panic}
									}
								}
							}
						}
						{tag_add aio_morale_p0}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/shaken_entry"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/refresh_pressure"
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
										{prop
											{prop human}
										}
									}
									{include
										{state
											{state suppressed}
										}
										{state
											{state operatable}
										}
										{tag
											{tag aio_morale_shaken}
										}
									}
								}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{state
											{state suppressed}
										}
										{state
											{state operatable}
										}
										{tag
											{tag aio_morale_panic}
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
										{state suppressed}
									}
									{tag
										{tag aio_morale_shaken}
									}
								}
							}
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{state
										{state suppressed}
									}
									{tag
										{tag aio_morale_panic}
									}
								}
							}
						}
						{tag_add aio_morale_recent_pressure}
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
										{state suppressed}
									}
									{tag
										{tag aio_morale_recent_pressure}
									}
								}
							}
						}
						{tag_remove aio_morale_p1}
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
										{state suppressed}
									}
									{tag
										{tag aio_morale_recent_pressure}
									}
								}
							}
						}
						{tag_remove aio_morale_p2}
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
										{state suppressed}
									}
									{tag
										{tag aio_morale_recent_pressure}
									}
								}
							}
						}
						{tag_remove aio_morale_p3}
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
										{state suppressed}
									}
									{tag
										{tag aio_morale_recent_pressure}
									}
								}
							}
						}
						{tag_add aio_morale_p0}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/refresh_pressure"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/age_tick"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
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
										{tag aio_morale_p3}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state suppressed}
									}
								}
							}
						}
						{tag_remove aio_morale_recent_pressure}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_p3}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state suppressed}
									}
								}
							}
						}
						{tag_remove aio_morale_p3}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_p3}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state suppressed}
									}
								}
							}
						}
						{tag_remove aio_morale_p3}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_recent_pressure}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state suppressed}
									}
								}
								{exclude
									{tag
										{tag aio_morale_p0}
									}
									{tag
										{tag aio_morale_p1}
									}
									{tag
										{tag aio_morale_p2}
									}
									{tag
										{tag aio_morale_p3}
									}
								}
							}
						}
						{tag_add aio_morale_p0}
					}
					{"entity_state"
						{selector
							{tag aio_morale_p2}
							{type human}
						}
						{tag_add aio_morale_p3}
					}
					{"entity_state"
						{selector
							{tag aio_morale_p2}
							{type human}
						}
						{tag_remove aio_morale_p2}
					}
					{"entity_state"
						{selector
							{tag aio_morale_p1}
							{type human}
						}
						{tag_add aio_morale_p2}
					}
					{"entity_state"
						{selector
							{tag aio_morale_p1}
							{type human}
						}
						{tag_remove aio_morale_p1}
					}
					{"entity_state"
						{selector
							{tag aio_morale_p0}
							{type human}
						}
						{tag_add aio_morale_p1}
					}
					{"entity_state"
						{selector
							{tag aio_morale_p0}
							{type human}
						}
						{tag_remove aio_morale_p0}
					}
					{"entity_state"
						{selector
							{tag aio_morale_j2}
							{type human}
						}
						{tag_remove aio_morale_just_shaken}
					}
					{"entity_state"
						{selector
							{tag aio_morale_j2}
							{type human}
						}
						{tag_remove aio_morale_j2}
					}
					{"entity_state"
						{selector
							{tag aio_morale_j1}
							{type human}
						}
						{tag_add aio_morale_j2}
					}
					{"entity_state"
						{selector
							{tag aio_morale_j1}
							{type human}
						}
						{tag_remove aio_morale_j1}
					}
					{"entity_state"
						{selector
							{tag aio_morale_j0}
							{type human}
						}
						{tag_add aio_morale_j1}
					}
					{"entity_state"
						{selector
							{tag aio_morale_j0}
							{type human}
						}
						{tag_remove aio_morale_j0}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/age_tick"}
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
						{"2.entities"
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
											{state suppressed}
										}
										{tag
											{tag aio_morale_shaken}
										}
										{tag
											{tag aio_morale_recent_pressure}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_just_shaken}
										}
										{tag
											{tag aio_morale_panic}
										}
									}
								}
							}
						}
					}
				}
				{actions
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
								{include
									{state
										{state suppressed}
									}
									{tag
										{tag aio_morale_shaken}
									}
									{tag
										{tag aio_morale_recent_pressure}
									}
								}
								{exclude
									{tag
										{tag aio_morale_just_shaken}
									}
									{tag
										{tag aio_morale_panic}
									}
								}
							}
						}
						{tag_add aio_morale_panic}
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
								{include
									{prop
										{prop human}
									}
								}
							}
						}
						{tag_remove aio_morale_shaken}
					}
					{"if"
						{condition
							{type entities}
							{selector
								{tag aio_morale_panic}
								{type human}
							}
						}
						{"set_i"
							{var "ce_morale_diag_panic$"}
							{op "="}
							{value 1}
						}
					}
					{"delay"
						{time 3}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/escalate_panic"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/recover_panic"
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
										{tag
											{tag aio_morale_recent_pressure}
										}
										{state
											{state suppressed}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 8}
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
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{tag
										{tag aio_morale_recent_pressure}
									}
									{state
										{state suppressed}
									}
								}
							}
						}
						{tag_add aio_morale_shaken}
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
								{include
									{tag
										{tag aio_morale_shaken}
									}
								}
								{exclude
									{tag
										{tag aio_morale_recent_pressure}
									}
								}
							}
						}
						{tag_add aio_morale_recovered_panic}
					}
					{"entity_state"
						{selector
							{tag aio_morale_recovered_panic}
							{type human}
						}
						{tag_remove aio_morale_panic}
					}
					{"if"
						{condition
							{type entities}
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_recovered_panic}
										}
									}
									{exclude
										{state
											{state suppressed}
										}
									}
								}
							}
						}
						{"set_i"
							{var "ce_morale_diag_recover_panic$"}
							{op "="}
							{value 1}
						}
					}
					{"if"
						{condition
							{type entities}
							{selector
								{tag aio_morale_recovered_panic}
								{type human}
							}
						}
						{"set_i"
							{var "ce_morale_diag_recover$"}
							{op "="}
							{value 1}
						}
					}
					{"entity_state"
						{selector
							{tag aio_morale_recovered_panic}
							{type human}
						}
						{tag_remove aio_morale_recovered_panic}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/recover_panic"}
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
						{"2.entities"
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
										{tag
											{tag aio_morale_panic}
										}
										{tag
											{tag aio_morale_recent_pressure}
										}
										{tag
											{tag aio_morale_just_shaken}
										}
										{state
											{state suppressed}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 8}
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
									{tag
										{tag aio_morale_panic}
									}
									{tag
										{tag aio_morale_recent_pressure}
									}
									{state
										{state suppressed}
									}
								}
							}
						}
						{tag_add aio_morale_recovered_shaken}
					}
					{"entity_state"
						{selector
							{tag aio_morale_recovered_shaken}
							{type human}
						}
						{tag_remove aio_morale_shaken}
					}
					{"if"
						{condition
							{type entities}
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_recovered_shaken}
										}
									}
									{exclude
										{state
											{state suppressed}
										}
									}
								}
							}
						}
						{"set_i"
							{var "ce_morale_diag_recover$"}
							{op "="}
							{value 1}
						}
					}
					{"if"
						{condition
							{type entities}
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_recovered_shaken}
										}
									}
									{exclude
										{state
											{state suppressed}
										}
									}
								}
							}
						}
						{"set_i"
							{var "ce_morale_diag_recover_clear$"}
							{op "="}
							{value 1}
						}
					}
					{"entity_state"
						{selector
							{tag aio_morale_recovered_shaken}
							{type human}
						}
						{tag_remove aio_morale_recovered_shaken}
					}
					{"delay"
						{time 4}
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
						{var "ce_morale_diag_pressure$"}
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
						{tag_add aio_morale_autodemo_actor}
					}
					{"entity_state"
						{selector
							{tag aio_morale_autodemo_actor}
							{type human}
						}
						{tag_add aio_morale_shaken}
					}
					{"if"
						{condition
							{type entities}
							{selector
								{tag aio_morale_autodemo_actor}
								{type human}
							}
						}
						{"set_i"
							{var "ce_morale_diag_shaken$"}
							{op "="}
							{value 1}
						}
					}
					{"delay"
						{time 1}
					}
					{"entity_state"
						{selector
							{tag aio_morale_autodemo_actor}
							{type human}
						}
						{tag_add aio_morale_panic}
					}
					{"entity_state"
						{selector
							{tag aio_morale_autodemo_actor}
							{type human}
						}
						{tag_remove aio_morale_shaken}
					}
					{"if"
						{condition
							{type entities}
							{selector
								{tag aio_morale_panic}
								{type human}
							}
						}
						{"set_i"
							{var "ce_morale_diag_panic$"}
							{op "="}
							{value 1}
						}
					}
					{"delay"
						{time 1}
					}
					{"entity_state"
						{selector
							{tag aio_morale_autodemo_actor}
							{type human}
						}
						{tag_add aio_morale_shaken}
					}
					{"entity_state"
						{selector
							{tag aio_morale_autodemo_actor}
							{type human}
						}
						{tag_remove aio_morale_panic}
					}
					{"if"
						{condition
							{type entities}
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_autodemo_actor}
										}
									}
									{include
										{tag
											{tag aio_morale_shaken}
										}
									}
									{exclude
										{tag
											{tag aio_morale_panic}
										}
									}
								}
							}
						}
						{"set_i"
							{var "ce_morale_diag_recover_panic$"}
							{op "="}
							{value 1}
						}
					}
					{"delay"
						{time 1}
					}
					{"entity_state"
						{selector
							{tag aio_morale_autodemo_actor}
							{type human}
						}
						{tag_remove aio_morale_shaken}
					}
					{"if"
						{condition
							{type entities}
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_autodemo_actor}
										}
									}
									{exclude
										{tag
											{tag aio_morale_shaken}
										}
										{tag
											{tag aio_morale_panic}
										}
									}
								}
							}
						}
						{"set_i"
							{var "ce_morale_diag_recover$"}
							{op "="}
							{value 1}
						}
					}
					{"set_i"
						{var "ce_morale_sys_done$"}
						{op "="}
						{value 1}
					}
				}
			}
