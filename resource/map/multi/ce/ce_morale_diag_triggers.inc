; PR B architecture diagnostic. Independent bits. No Shaken/Panic gameplay path.
			{"conquest_enhanced_mechanics/morale/diag_mi_alive"
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
					{"set_i"
						{var "ce_morale_diag_mi_alive$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/diag_mi_alive"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/diag_human_add"
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
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_diag_human$"}
						{op "="}
						{value 1}
					}
					{"set_i"
						{var "ce_morale_diag_add_action_ran$"}
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
							}
						}
						{tag_add aio_morale_diag_roundtrip}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/diag_human_add"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/diag_added_tag_read"
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
											{tag aio_morale_diag_roundtrip}
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
						{var "ce_morale_diag_added_tag_read$"}
						{op "="}
						{value 1}
					}
					{"set_i"
						{var "ce_morale_diag_tag_roundtrip$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/diag_added_tag_read"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/diag_known_tag"
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
											{state user_control}
										}
									}
								}
								{group
									{select
										{tag
											{tag player}
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
						{var "ce_morale_diag_known_tag$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/diag_known_tag"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/diag_pr_a_source"
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
											{tag aio_morale_low}
										}
										{tag
											{tag aio_morale_regular}
										}
										{tag
											{tag aio_morale_trained}
										}
										{tag
											{tag aio_morale_elite}
										}
									}
									{include
										{prop
											{prop human}
										}
									}
									{exclude
										{tag
											{tag aio_morale_fallback}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_diag_pr_a_source$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/diag_pr_a_source"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/diag_canary_present"
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
									{include
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "secret_doc_bag2"}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_diag_canary_present$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/diag_canary_present"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/diag_inventory_canary"
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
									{include
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "aio_morale_marker"}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_diag_inventory_canary$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/diag_inventory_canary"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/diag_shaken_apply"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "ce_morale_diag_shaken$"}
							{op "="}
							{value 0}
						}
						{"3.entities"
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
						}
						{tag_add aio_morale_shaken}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/diag_shaken_apply"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/diag_panic_apply"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "ce_morale_diag_panic$"}
							{op "="}
							{value 0}
						}
						{"3.entities"
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
						{var "ce_morale_diag_panic$"}
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
						}
						{tag_add aio_morale_panic}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/diag_panic_apply"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/diag_player_hit"
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
										{tag
											{tag aio_morale_panic}
										}
									}
									{include
										{prop
											{prop human}
										}
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
						{var "ce_morale_diag_player_hit$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/diag_player_hit"}
					}
				}
			}
