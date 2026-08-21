; Interim collection: tag captor-rear spawn_a/spawn_b as aio_pow_camp.
; Not a visible prison camp. Ordinary POWs walk there if the live tag exists.
; Ally/support keep waypoint egress. Spawnside 0 uses the same mapping as
; spawnside 1 (Conquest default / bots_spawnside fallback: enemy-on-A,
; player-rear-B). enemy_spawnside$ <= 1 arms spawn_b; == 2 arms spawn_a.
; P0+harmless lifecycle stays in surrender_present; this file only dest+hold.
			{"conquest_enhanced_mechanics/broken/spawn_pow_camp_s1"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "enemy_spawnside$"}
							{op "<="}
							{value 1}
						}
						{"3.cmp_i"
							{var "aio_pow_camp_ready$"}
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
						{tag_add aio_pow_camp}
					}
					{"set_i"
						{var "aio_pow_camp_ready$"}
						{op "="}
						{value 1}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/spawn_pow_camp_s2"
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
							{var "aio_pow_camp_ready$"}
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
						{tag_add aio_pow_camp}
					}
					{"set_i"
						{var "aio_pow_camp_ready$"}
						{op "="}
						{value 1}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/surrender_arrive_camp"
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
											{tag aio_pow_camp}
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
											{tag aio_morale_surrender_to_camp}
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
											{tag prisoner_in_camp}
										}
									}
								}
							}
							{enemy
								{source advanced}
								{group
									{select
										{tag
											{tag aio_pow_camp}
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
								{"for selector" aio_morale_surrender_at_camp}
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
										{tag aio_morale_surrender_at_camp}
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
						{tag_add prisoner_in_camp}
					}
					{"set_i"
						{var "ce_morale_diag_held$"}
						{op "="}
						{value 1}
					}
					{"entity_state"
						{selector
							{tag aio_morale_surrender_at_camp}
						}
						{tag_remove aio_morale_surrender_evacuating}
					}
					{"entity_state"
						{selector
							{tag aio_morale_surrender_at_camp}
						}
						{tag_remove aio_morale_surrender_to_camp}
					}
					{"entity_state"
						{selector
							{tag aio_morale_surrender_at_camp}
						}
						{tag_remove aio_morale_surrender_to_a}
					}
					{"entity_state"
						{selector
							{tag aio_morale_surrender_at_camp}
						}
						{tag_remove aio_morale_surrender_to_b}
					}
					{"entity_state"
						{selector
							{tag aio_morale_surrender_at_camp}
						}
						{tag_remove aio_morale_surrender_expire}
					}
					{"actor_state"
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
						{move_mode hold}
						{ai_move
							{mode disable}
						}
					}
					{"entity_state"
						{selector
							{tag aio_morale_surrender_at_camp}
						}
						{tag_remove aio_morale_surrender_at_camp}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/surrender_arrive_camp"}
					}
				}
			}
