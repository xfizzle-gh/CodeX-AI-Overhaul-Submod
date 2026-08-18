; PR C: Steady -> Shaken -> Panic, then Panic -> Shaken -> Steady.
; Production pressure is proven CE see_enemy. Native {state suppressed} is observe-only;
; phase 0 found zero suppressed hits in local FE/West 81/CE scripts.
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
										{tag aio_morale_panic}
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
					{"set_i"
						{var "ce_morale_diag_panic$"}
						{op "="}
						{value 1}
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
						{tag_add aio_morale_panic}
						{tag_remove aio_morale_shaken}
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
						{time 8}
					}
					{"set_i"
						{var "ce_morale_diag_recover_panic$"}
						{op "="}
						{value 1}
					}
					{"set_i"
						{var "ce_morale_diag_recover$"}
						{op "="}
						{value 1}
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
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
								}
							}
						}
						{tag_add aio_morale_shaken}
						{tag_remove aio_morale_panic}
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
										{state
											{state user_control}
										}
										{tag
											{tag player}
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
									{state
										{state user_control}
									}
									{tag
										{tag player}
									}
									{tag
										{tag aio_morale_panic}
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
						{tag_add aio_morale_panic}
						{tag_remove aio_morale_shaken}
					}
					{"set_i"
						{var "ce_morale_diag_panic$"}
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
						{tag_remove aio_morale_panic}
					}
					{"set_i"
						{var "ce_morale_diag_recover_panic$"}
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
					{"set_i"
						{var "ce_morale_sys_done$"}
						{op "="}
						{value 1}
					}
				}
			}
