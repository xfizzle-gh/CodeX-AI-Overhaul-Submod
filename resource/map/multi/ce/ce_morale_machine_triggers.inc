; PR C: Steady -> Shaken -> Panic, then Panic -> Shaken -> Steady.
; see_actors+tag pair is a combat-pressure proxy, not a proven suppression signal.
; Players may be Shaken/Panic. No Broken/retreat/surrender/command.
			{"conquest_enhanced_mechanics/morale/observe_suppressed_state"
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
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_diag_suppressed_state$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/observe_suppressed_state"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/pressure_contact"
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
											{state dead}
										}
										{tag
											{tag aio_morale_panic}
										}
									}
								}
							}
							{enemy
								{type human}
								{state "not dead"}
							}
							{distance
								{mode near_than}
								{meters 50}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_morale_saw_enemy}
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
						{var "ce_morale_diag_shaken$"}
						{op "="}
						{value 1}
					}
					{"entity_state"
						{selector
							{tag aio_morale_saw_enemy}
							{type human}
							{state operatable}
						}
						{tag_add aio_morale_shaken}
					}
					{"entity_state"
						{selector
							{tag aio_morale_saw_enemy}
							{type human}
						}
						{tag_add aio_morale_recent_pressure}
					}
					{"entity_state"
						{selector
							{tag aio_morale_saw_enemy}
							{type human}
						}
						{tag_add aio_morale_just_shaken}
					}
					{"entity_state"
						{selector
							{tag aio_morale_saw_enemy}
							{type human}
						}
						{tag_remove aio_morale_saw_enemy}
					}
					{"delay"
						{time 3}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/pressure_contact"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/expire_just_shaken"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{tag aio_morale_just_shaken}
								{type human}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 6}
					}
					{"entity_state"
						{selector
							{tag aio_morale_just_shaken}
							{type human}
						}
						{tag_remove aio_morale_just_shaken}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/expire_just_shaken"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/expire_pressure"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{tag aio_morale_recent_pressure}
								{type human}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 5}
					}
					{"switch"
						{"case"
							{condition
								{type see_actors}
								{selector
									{tag aio_morale_recent_pressure}
									{type human}
									{state operatable}
								}
								{enemy
									{type human}
									{state "not dead"}
								}
								{distance
									{mode near_than}
									{meters 50}
								}
								{detection located}
							}
						}
						{"default"
							{"entity_state"
								{selector
									{tag aio_morale_recent_pressure}
									{type human}
								}
								{tag_remove aio_morale_recent_pressure}
							}
						}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/expire_pressure"}
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
						{"2.see_actors"
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
							{enemy
								{type human}
								{state "not dead"}
							}
							{distance
								{mode near_than}
								{meters 50}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_morale_saw_panic}
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
							{tag aio_morale_saw_panic}
							{type human}
							{state operatable}
						}
						{tag_add aio_morale_panic}
					}
					{"entity_state"
						{selector
							{tag aio_morale_saw_panic}
							{type human}
						}
						{tag_remove aio_morale_shaken}
					}
					{"entity_state"
						{selector
							{tag aio_morale_saw_panic}
							{type human}
						}
						{tag_remove aio_morale_saw_panic}
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
						{time 4}
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
								{tag aio_morale_recovered_panic}
								{type human}
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
								{tag aio_morale_recovered_shaken}
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
						{tag_add aio_morale_shaken}
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
											{tag aio_morale_shaken}
										}
									}
									{exclude
										{tag
											{tag aio_morale_panic}
										}
										{state
											{state user_control}
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
									{tag
										{tag aio_morale_panic}
									}
								}
							}
							{amount 1}
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
