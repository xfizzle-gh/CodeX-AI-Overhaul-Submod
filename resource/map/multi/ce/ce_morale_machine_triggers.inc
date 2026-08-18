; PR C: FE-style Shaken/Panic + per-actor entity recovery. Trigger is {state suppressed}.
; All selectors exclude dead/inactive/linked. Do not use operatable as the live filter.
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
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state linked}
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
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state linked}
										}
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
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state linked}
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
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state linked}
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
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state linked}
									}
								}
							}
						}
						{tag_remove aio_morale_recovering}
					}
					{"effect"
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
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state linked}
									}
									{tag
										{tag aio_morale_just_shaken}
									}
									{tag
										{tag aio_morale_panic}
									}
								}
							}
						}
						{effect aio_morale_just_shaken}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/shaken_entry"}
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
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state linked}
										}
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
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state linked}
									}
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
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state linked}
									}
								}
							}
						}
						{tag_remove aio_morale_shaken}
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
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state linked}
									}
								}
							}
						}
						{tag_remove aio_morale_recovering}
					}
					{"if"
						{condition
							{type entities}
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
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state linked}
										}
									}
								}
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
			{"conquest_enhanced_mechanics/morale/refresh_cancel_recover"
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
											{tag aio_morale_recovering}
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
											{state linked}
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
									{prop
										{prop human}
									}
								}
								{include
									{state
										{state suppressed}
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
										{state linked}
									}
								}
							}
						}
						{tag_remove aio_morale_recovering}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/refresh_cancel_recover"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/start_recover"
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
									}
									{exclude
										{state
											{state suppressed}
										}
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state linked}
										}
										{tag
											{tag aio_morale_recovering_from_shaken}
										}
									}
								}
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
											{state suppressed}
										}
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state linked}
										}
										{tag
											{tag aio_morale_recovering_from_panic}
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
										{state suppressed}
									}
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state linked}
									}
									{tag
										{tag aio_morale_recovering_from_shaken}
									}
								}
							}
						}
						{effect recovering_from_shaken}
					}
					{"effect"
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
										{state suppressed}
									}
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{state
										{state linked}
									}
									{tag
										{tag aio_morale_recovering_from_panic}
									}
								}
							}
						}
						{effect recovering_from_panic}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/start_recover"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/observe_recover"
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
											{tag aio_morale_did_recover}
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
											{state linked}
										}
									}
								}
								{group
									{select
										{tag
											{tag aio_morale_did_recover_panic}
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
											{state linked}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_diag_recover$"}
						{op "="}
						{value 1}
					}
					{"if"
						{condition
							{type entities}
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_did_recover_panic}
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
											{state linked}
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
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_did_recover}
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
											{state linked}
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
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/observe_recover"}
					}
				}
			}
