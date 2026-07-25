			{"ai_logic/ai_patrol_logic"
				{condition
					{expression "(1 | 2 | 3 | 4 | 5) & 6"}
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{zone
											{zone "fpc1"}
										}
									}
									{include
										{prop
											{prop human}
										}
										{state
											{state operatable}
										}
										{tag
											{tag cmp_def}
										}
									}
									{exclude
										{state
											{state user_control}
										}
										{state
											{state inactive}
										}
										{state
											{state linked}
										}
										{tag
											{tag patrolling}
										}
										{tag
											{tag go_patrol}
										}
										{tag
											{tag _bot}
										}
									}
								}
							}
							{count
								{op ">"}
								{value "max_ai_defender_at_flag$"}
							}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{zone
											{zone "fpc2"}
										}
									}
									{include
										{prop
											{prop human}
										}
										{state
											{state operatable}
										}
										{tag
											{tag cmp_def}
										}
									}
									{exclude
										{state
											{state user_control}
										}
										{state
											{state inactive}
										}
										{state
											{state linked}
										}
										{tag
											{tag patrolling}
										}
										{tag
											{tag go_patrol}
										}
										{tag
											{tag _bot}
										}
									}
								}
							}
							{count
								{op ">"}
								{value "max_ai_defender_at_flag$"}
							}
						}
						{"3.entities"
							{selector
								{source advanced}
								{group
									{select
										{zone
											{zone "fpc3"}
										}
									}
									{include
										{prop
											{prop human}
										}
										{state
											{state operatable}
										}
										{tag
											{tag cmp_def}
										}
									}
									{exclude
										{state
											{state user_control}
										}
										{state
											{state inactive}
										}
										{state
											{state linked}
										}
										{tag
											{tag patrolling}
										}
										{tag
											{tag go_patrol}
										}
										{tag
											{tag _bot}
										}
									}
								}
							}
							{count
								{op ">"}
								{value "max_ai_defender_at_flag$"}
							}
						}
						{"4.entities"
							{selector
								{source advanced}
								{group
									{select
										{zone
											{zone "fpc4"}
										}
									}
									{include
										{prop
											{prop human}
										}
										{state
											{state operatable}
										}
										{tag
											{tag cmp_def}
										}
									}
									{exclude
										{state
											{state user_control}
										}
										{state
											{state inactive}
										}
										{state
											{state linked}
										}
										{tag
											{tag patrolling}
										}
										{tag
											{tag go_patrol}
										}
										{tag
											{tag _bot}
										}
									}
								}
							}
							{count
								{op ">"}
								{value "max_ai_defender_at_flag$"}
							}
						}
						{"5.entities"
							{selector
								{source advanced}
								{group
									{select
										{zone
											{zone "fpc5"}
										}
									}
									{include
										{prop
											{prop human}
										}
										{state
											{state operatable}
										}
										{tag
											{tag cmp_def}
										}
									}
									{exclude
										{state
											{state user_control}
										}
										{state
											{state inactive}
										}
										{state
											{state linked}
										}
										{tag
											{tag patrolling}
										}
										{tag
											{tag go_patrol}
										}
										{tag
											{tag _bot}
										}
									}
								}
							}
							{count
								{op ">"}
								{value "max_ai_defender_at_flag$"}
							}
						}
						{"6.event"
							{id "ai_defender_inf_balanced"}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{group
										{select
											{zone
												{zone "fpc1"}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
											}
											{tag
												{tag cmp_def}
											}
										}
										{exclude
											{state
												{state user_control}
											}
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
											{tag
												{tag patrolling}
											}
											{tag
												{tag go_patrol}
											}
											{tag
												{tag _bot}
											}
										}
									}
								}
								{count
									{op ">"}
									{value "max_ai_defender_at_flag$"}
								}
							}
							{"effect"
								{selector
									{source advanced}
									{group
										{select
											{zone
												{zone "fpc1"}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
											}
											{tag
												{tag cmp_def}
											}
										}
										{exclude
											{state
												{state user_control}
											}
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
											{tag
												{tag patrolling}
											}
											{tag
												{tag go_patrol}
											}
											{tag
												{tag _bot}
											}
										}
									}
									{amount 4}
								}
								{effect send_ai_to_partol}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{group
										{select
											{zone
												{zone "fpc2"}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
											}
											{tag
												{tag cmp_def}
											}
										}
										{exclude
											{state
												{state user_control}
											}
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
											{tag
												{tag patrolling}
											}
											{tag
												{tag go_patrol}
											}
											{tag
												{tag _bot}
											}
										}
									}
								}
								{count
									{op ">"}
									{value "max_ai_defender_at_flag$"}
								}
							}
							{"effect"
								{selector
									{source advanced}
									{group
										{select
											{zone
												{zone "fpc2"}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
											}
											{tag
												{tag cmp_def}
											}
										}
										{exclude
											{state
												{state user_control}
											}
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
											{tag
												{tag patrolling}
											}
											{tag
												{tag go_patrol}
											}
											{tag
												{tag _bot}
											}
										}
									}
									{amount 4}
								}
								{effect send_ai_to_partol}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{group
										{select
											{zone
												{zone "fpc3"}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
											}
											{tag
												{tag cmp_def}
											}
										}
										{exclude
											{state
												{state user_control}
											}
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
											{tag
												{tag patrolling}
											}
											{tag
												{tag go_patrol}
											}
											{tag
												{tag _bot}
											}
										}
									}
								}
								{count
									{op ">"}
									{value "max_ai_defender_at_flag$"}
								}
							}
							{"effect"
								{selector
									{source advanced}
									{group
										{select
											{zone
												{zone "fpc3"}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
											}
											{tag
												{tag cmp_def}
											}
										}
										{exclude
											{state
												{state user_control}
											}
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
											{tag
												{tag patrolling}
											}
											{tag
												{tag go_patrol}
											}
											{tag
												{tag _bot}
											}
										}
									}
									{amount 4}
								}
								{effect send_ai_to_partol}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{group
										{select
											{zone
												{zone "fpc4"}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
											}
											{tag
												{tag cmp_def}
											}
										}
										{exclude
											{state
												{state user_control}
											}
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
											{tag
												{tag patrolling}
											}
											{tag
												{tag go_patrol}
											}
											{tag
												{tag _bot}
											}
										}
									}
								}
								{count
									{op ">"}
									{value "max_ai_defender_at_flag$"}
								}
							}
							{"effect"
								{selector
									{source advanced}
									{group
										{select
											{zone
												{zone "fpc4"}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
											}
											{tag
												{tag cmp_def}
											}
										}
										{exclude
											{state
												{state user_control}
											}
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
											{tag
												{tag patrolling}
											}
											{tag
												{tag go_patrol}
											}
											{tag
												{tag _bot}
											}
										}
									}
									{amount 4}
								}
								{effect send_ai_to_partol}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{group
										{select
											{zone
												{zone "fpc5"}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
											}
											{tag
												{tag cmp_def}
											}
										}
										{exclude
											{state
												{state user_control}
											}
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
											{tag
												{tag patrolling}
											}
											{tag
												{tag go_patrol}
											}
											{tag
												{tag _bot}
											}
										}
									}
								}
								{count
									{op ">"}
									{value "max_ai_defender_at_flag$"}
								}
							}
							{"effect"
								{selector
									{source advanced}
									{group
										{select
											{zone
												{zone "fpc5"}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
											}
											{tag
												{tag cmp_def}
											}
										}
										{exclude
											{state
												{state user_control}
											}
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
											{tag
												{tag patrolling}
											}
											{tag
												{tag go_patrol}
											}
											{tag
												{tag _bot}
											}
										}
									}
									{amount 4}
								}
								{effect send_ai_to_partol}
							}
						}
					}
					{"delay"
						{time 0.5}
					}
					{"call"
						{function "recon_random_flag"}
						{context
							{specified}
							{selector
								{tag go_patrol}
								{type human}
								{state operatable}
							}
						}
					}
					{"delay"
						{time 3}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.33}
							}
							{"actor_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag go_patrol}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
											}
											{tag
												{tag cmp_def}
											}
										}
										{exclude
											{state
												{state user_control}
											}
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
											{tag
												{tag patrolling}
											}
										}
									}
								}
								{fire_mode open}
								{weapon_prepare on}
								{ai_move
									{mode enable}
								}
								{movement
									{speed spot}
								}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.33}
							}
							{"actor_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag go_patrol}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
											}
											{tag
												{tag cmp_def}
											}
										}
										{exclude
											{state
												{state user_control}
											}
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
											{tag
												{tag patrolling}
											}
										}
									}
								}
								{fire_mode open}
								{weapon_prepare on}
								{ai_move
									{mode enable}
								}
								{movement
									{speed walk}
								}
							}
						}
						{"default"
							{"actor_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag go_patrol}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
											}
											{tag
												{tag cmp_def}
											}
										}
										{exclude
											{state
												{state user_control}
											}
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
											{tag
												{tag patrolling}
											}
										}
									}
								}
								{fire_mode open}
								{weapon_prepare on}
								{ai_move
									{mode enable}
								}
								{movement
									{speed assault}
								}
							}
						}
					}
					{"delay"
						{time 0.5}
					}
					{"entity_state"
						{selector
							{tag go_patrol}
							{type human}
							{state operatable}
						}
						{tag_add patrolling}
						{tag_remove go_patrol}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "ai_logic/ai_patrol_logic"}
					}
				}
			}
