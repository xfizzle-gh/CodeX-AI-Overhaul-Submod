; POW camp management: opposite-spawn enemy collection + first-hold talk.
; No convert, release, truck, or live count.
; P0+harmless lifecycle stays in surrender_present.
			{"conquest_enhanced_mechanics/broken/spawn_enemy_pow_camp_s1"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "enemy_spawnside$"}
							{op "=="}
							{value 1}
						}
						{"3.cmp_i"
							{var "aio_pow_camp_enemy_ready$"}
							{op "=="}
							{value 0}
						}
						{"4.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag spawn_a}
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
										{tag spawn_a}
									}
								}
							}
							{amount 1
							}
						}
						{tag_add aio_pow_camp_enemy}
					}
					{"set_i"
						{var "aio_pow_camp_enemy_ready$"}
						{op "="}
						{value 1}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/spawn_enemy_pow_camp_s2"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "enemy_spawnside$"}
							{op "=="}
							{value 2}
						}
						{"3.cmp_i"
							{var "aio_pow_camp_enemy_ready$"}
							{op "=="}
							{value 0}
						}
						{"4.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag spawn_b}
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
										{tag spawn_b}
									}
								}
							}
							{amount 1
							}
						}
						{tag_add aio_pow_camp_enemy}
					}
					{"set_i"
						{var "aio_pow_camp_enemy_ready$"}
						{op "="}
						{value 1}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/surrender_arrive_enemy_camp"
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
											{tag aio_pow_camp_enemy}
										}
									}
								}
							}
						}
						{"3.see_actors"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_surrender_to_enemy_camp}
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
										{tag
											{tag prisoner_in_enemy_camp}
										}
									}
								}
							}
							{enemy
								{source advanced}
								{group
									{select
										{tag
											{tag aio_pow_camp_enemy}
										}
									}
								}
							}
							{distance
								{mode near_than}
								{meters 25}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_morale_surrender_at_enemy_camp}
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
										{tag aio_morale_surrender_at_enemy_camp}
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
						{tag_add prisoner_in_enemy_camp}
					}
					{"entity_state"
						{selector
							{tag aio_morale_surrender_at_enemy_camp}
						}
						{tag_remove aio_morale_surrender_evacuating}
					}
					{"entity_state"
						{selector
							{tag aio_morale_surrender_at_enemy_camp}
						}
						{tag_remove aio_morale_surrender_to_enemy_camp}
					}
					{"entity_state"
						{selector
							{tag aio_morale_surrender_at_enemy_camp}
						}
						{tag_remove aio_morale_surrender_expire}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag prisoner_in_enemy_camp}
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
						{move_mode hold}
						{ai_move
							{mode disable}
						}
					}
					{"entity_state"
						{selector
							{tag aio_morale_surrender_at_enemy_camp}
						}
						{tag_remove aio_morale_surrender_at_enemy_camp}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/surrender_arrive_enemy_camp"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/pow_announce"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "aio_pow_camp_announced$"}
							{op "=="}
							{value 0}
						}
						{"3.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag prisoner_in_camp}
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
					{"talk"
						{selector
							{ignore_captured_by_user 0}
							{amount 1
							}
							{tag player}
							{zone "gamezone"}
							{type human}
							{state "not dead"}
						}
						{text "mission/multi/ce_pow_camp_held"}
					}
					{"set_i"
						{var "aio_pow_camp_announced$"}
						{op "="}
						{value 1}
					}
				}
			}
