; Horten artillery AI. Extracted from HF-dcg_script.inc.
; Production include is commented out for FPS. Uncomment in HF-dcg_script.inc to restore.

;// Arty AI
			{"hf arty ai/part 1"
				{condition
					{expression "!1 & !2"}
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{include
										{tag
											{tag bot_arty}
										}
									}
									{exclude
										{tag
											{tag bot_arty_move}
										}
										{tag
											{tag bot_arty_fire}
										}
										{state
											{state linked}
										}
										{tag
											{tag bot_arty_skedaddle}
										}
										{tag
											{tag range_less_than_320}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{include
										{state
											{state user_control}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag exploded}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag burned}
										}
										{tag
											{tag no_scavenge}
										}
										{tag
											{tag destroyed}
										}
										{tag
											{tag vehicle_destroyed}
										}
										{prop
											{prop human}
										}
									}
								}
							}
							{distance 320}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{include
										{tag
											{tag bot_arty_fire}
										}
									}
									{exclude
										{tag
											{tag bot_arty_skedaddle}
										}
										{state
											{state "dead inactive"}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{include
									{tag
										{tag bot_arty}
									}
								}
								{exclude
									{state
										{state "dead inactive user_control"}
									}
									{tag
										{tag bot_arty_skedaddle}
									}
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_add bot_arty_move}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_320}
									}
								}
							}
						}
						{tag_remove bot_arty_fortified}
					}
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{include
									{tag
										{tag bot_arty_move}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_320}
									}
								}
							}
						}
						{drop "orders sensor"}
						{movement
							{speed fast}
						}
						{cannon move}
					}
					{"delay"
						{time 3.5}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{drop orders}
						{action move}
						{target
							{source advanced}
							{group
								{include
									{state
										{state user_control}
									}
									{prop
										{prop vehicle}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 60}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_remove bot_arty_move}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "hf arty ai/part 1"}
					}
				}
			}
			{"hf arty ai/part 1.1"
				{condition
					{expression "!1 & !2"}
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{include
										{tag
											{tag bot_arty}
										}
										{tag
											{tag range_less_than_320}
										}
									}
									{exclude
										{tag
											{tag bot_arty_move}
										}
										{tag
											{tag bot_arty_fire}
										}
										{state
											{state linked}
										}
										{tag
											{tag bot_arty_skedaddle}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{include
										{state
											{state user_control}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag exploded}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag burned}
										}
										{tag
											{tag no_scavenge}
										}
										{tag
											{tag destroyed}
										}
										{tag
											{tag vehicle_destroyed}
										}
										{prop
											{prop human}
										}
									}
								}
							}
							{distance 260}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{include
										{tag
											{tag bot_arty_fire}
										}
									}
									{exclude
										{tag
											{tag bot_arty_skedaddle}
										}
										{state
											{state "dead inactive"}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{include
									{tag
										{tag bot_arty}
									}
									{tag
										{tag range_less_than_320}
									}
								}
								{exclude
									{state
										{state "dead inactive user_control"}
									}
									{tag
										{tag bot_arty_skedaddle}
									}
								}
							}
						}
						{tag_add bot_arty_move}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{tag_remove bot_arty_fortified}
					}
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{drop "orders sensor"}
						{movement
							{speed fast}
						}
						{cannon move}
					}
					{"delay"
						{time 3.5}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{drop orders}
						{action move}
						{target
							{source advanced}
							{group
								{include
									{state
										{state user_control}
									}
									{prop
										{prop vehicle}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 60}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{tag_remove bot_arty_move}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "hf arty ai/part 1.1"}
					}
				}
			}
			{"hf arty ai/part 1.2"
				{condition
					{expression "!1 & !2"}
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{include
										{tag
											{tag bot_arty}
										}
										{tag
											{tag range_more_than_400}
										}
									}
									{exclude
										{tag
											{tag bot_arty_move}
										}
										{tag
											{tag bot_arty_fire}
										}
										{state
											{state linked}
										}
										{tag
											{tag bot_arty_skedaddle}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{include
										{state
											{state user_control}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag exploded}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag burned}
										}
										{tag
											{tag no_scavenge}
										}
										{tag
											{tag destroyed}
										}
										{tag
											{tag vehicle_destroyed}
										}
										{prop
											{prop human}
										}
									}
								}
							}
							{distance 405}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{include
										{tag
											{tag bot_arty_fire}
										}
									}
									{exclude
										{tag
											{tag bot_arty_skedaddle}
										}
										{state
											{state "dead inactive"}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{include
									{tag
										{tag bot_arty}
									}
									{tag
										{tag range_more_than_400}
									}
								}
								{exclude
									{state
										{state "dead inactive user_control"}
									}
									{tag
										{tag bot_arty_skedaddle}
									}
								}
							}
						}
						{tag_add bot_arty_move}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_remove bot_arty_fortified}
					}
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{drop "orders sensor"}
						{movement
							{speed fast}
						}
						{cannon move}
					}
					{"delay"
						{time 3.5}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{drop orders}
						{action move}
						{target
							{source advanced}
							{group
								{include
									{state
										{state user_control}
									}
									{prop
										{prop vehicle}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 60}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_remove bot_arty_move}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "hf arty ai/part 1.2"}
					}
				}
			}
			{"hf arty ai/part 2"
				{condition
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag bot_arty}
										}
									}
									{exclude
										{tag
											{tag bot_arty_skedaddle}
										}
										{tag
											{tag range_less_than_320}
										}
										{tag
											{tag range_more_than_400}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{include
										{state
											{state user_control}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag exploded}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag burned}
										}
										{tag
											{tag no_scavenge}
										}
										{tag
											{tag destroyed}
										}
										{tag
											{tag vehicle_destroyed}
										}
										{prop
											{prop human}
										}
									}
								}
							}
							{distance 310}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty}
									}
								}
								{exclude
									{tag
										{tag bot_arty_skedaddle}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_add bot_arty_fire}
						{tag_remove bot_arty_move}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{drop orders}
						{move_mode hold}
						{cannon fire}
					}
					{"delay"
						{time 4}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag bot_arty_fortified}
									}
									{tag
										{tag "+mobilegun"}
									}
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{effect construction_sandbag_cannon_standard}
					}
					{"delay"
						{time 6}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{state
										{state "dead inactive"}
									}
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_add bot_arty_fortified}
					}
					{"delay"
						{time 1}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{action rotate}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 0.25}
					}
					{"actor_fire"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
						{interval 0}
						{time 30}
					}
					{"delay"
						{time 45}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{action suppression_fire}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 45}
					}
					{"actor_fire"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
						{interval 0}
						{time 30}
					}
					{"delay"
						{time 45}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{action suppression_fire}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 45}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "hf arty ai/part 2"}
					}
				}
			}
			{"hf arty ai/part 2.1"
				{condition
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag bot_arty}
										}
									}
									{include
										{tag
											{tag range_less_than_320}
										}
									}
									{exclude
										{tag
											{tag bot_arty_skedaddle}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{include
										{state
											{state user_control}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag exploded}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag burned}
										}
										{tag
											{tag no_scavenge}
										}
										{tag
											{tag destroyed}
										}
										{tag
											{tag vehicle_destroyed}
										}
										{prop
											{prop human}
										}
									}
								}
							}
							{distance 250}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
								{exclude
									{tag
										{tag bot_arty_skedaddle}
									}
									{state
										{state "dead inactive"}
									}
								}
							}
						}
						{tag_add bot_arty_fire}
						{tag_remove bot_arty_move}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{drop orders}
						{move_mode hold}
						{cannon fire}
					}
					{"delay"
						{time 4}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
								{exclude
									{tag
										{tag bot_arty_fortified}
									}
									{tag
										{tag "+mobilegun"}
									}
								}
							}
						}
						{effect construction_sandbag_cannon_standard}
					}
					{"delay"
						{time 6}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
								{exclude
									{state
										{state "dead inactive"}
									}
								}
							}
						}
						{tag_add bot_arty_fortified}
					}
					{"delay"
						{time 1}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{action rotate}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 0.25}
					}
					{"actor_fire"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
						{interval 0}
						{time 30}
					}
					{"delay"
						{time 45}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{action suppression_fire}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 45}
					}
					{"actor_fire"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
						{interval 0}
						{time 30}
					}
					{"delay"
						{time 45}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{action suppression_fire}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 60}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "hf arty ai/part 2.1"}
					}
				}
			}
			{"hf arty ai/part 2.2"
				{condition
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag bot_arty}
										}
									}
									{include
										{tag
											{tag range_more_than_400}
										}
									}
									{exclude
										{tag
											{tag bot_arty_skedaddle}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{include
										{state
											{state user_control}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag exploded}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag burned}
										}
										{tag
											{tag no_scavenge}
										}
										{tag
											{tag destroyed}
										}
										{tag
											{tag vehicle_destroyed}
										}
										{prop
											{prop human}
										}
									}
								}
							}
							{distance 400}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
								{exclude
									{tag
										{tag bot_arty_skedaddle}
									}
									{state
										{state "dead inactive"}
									}
								}
							}
						}
						{tag_add bot_arty_fire}
						{tag_remove bot_arty_move}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{drop orders}
						{move_mode hold}
						{cannon fire}
					}
					{"delay"
						{time 4}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
								{exclude
									{tag
										{tag bot_arty_fortified}
									}
									{tag
										{tag "+mobilegun"}
									}
								}
							}
						}
						{effect construction_sandbag_cannon_standard}
					}
					{"delay"
						{time 6}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
								{exclude
									{state
										{state "dead inactive"}
									}
								}
							}
						}
						{tag_add bot_arty_fortified}
					}
					{"delay"
						{time 1}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{action rotate}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 0.25}
					}
					{"actor_fire"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
						{interval 0}
						{time 30}
					}
					{"delay"
						{time 45}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{action suppression_fire}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 45}
					}
					{"actor_fire"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
						{interval 0}
						{time 30}
					}
					{"delay"
						{time 45}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{action suppression_fire}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 60}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "hf arty ai/part 2.2"}
					}
				}
			}
			{"hf arty ai/part 3"
				{condition
					{expression "1 & 2 & !3"}
					{terms
						{"1.entities"
							{selector
								{tag bot_arty}
							}
						}
						{"2.entities"
							{selector
								{tag bot_aa}
							}
						}
						{"3.entities"
							{selector
								{tag bot_aa_guard}
								{state "not dead"}
							}
							{count
								{op ">="}
								{value 2}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{amount 1
							}
							{tag bot_aa}
						}
						{tag_add bot_aa_guard}
					}
					{"entity_state"
						{selector
							{amount 1
							}
							{tag bot_aa_guard}
						}
						{tag_add bot_aa_guard_move}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{amount 1
							}
							{tag bot_arty}
							{state "not dead"}
						}
						{tag_add bot_arty_guard}
					}
					{"delay"
						{time 2}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_aa_guard}
									}
								}
								{exclude
									{state
										{state "dead inactive user_control"}
									}
									{tag
										{tag bot_aa_guard_setlled}
									}
								}
							}
						}
						{drop "orders sensor"}
						{move_mode hold}
						{ai_move
							{mode disable}
							{enable_on enemy}
						}
						{movement
							{speed fast}
						}
						{cannon move}
					}
					{"delay"
						{time 2}
					}
					{"action"
						{selector
							{amount 1
							}
							{tag bot_aa_guard_move}
							{state "not dead"}
						}
						{drop orders}
						{action move}
						{target
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_guard}
									}
								}
								{exclude
									{state
										{state "dead inactive user_control"}
									}
								}
							}
							{amount 1
							}
						}
					}
					{"delay"
						{time 30}
					}
					{"trigger"
						{name "hf arty ai/part 3"}
					}
				}
			}
			{"hf arty ai/part 4"
				{condition
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag bot_aa_guard}
										}
									}
									{exclude
										{state
											{state "dead inactive user_control"}
										}
									}
								}
								{amount 1
								}
							}
							{near_to
								{amount 1
								}
								{tag bot_arty_guard}
							}
							{distance 30}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{tag bot_aa_guard_move}
						}
						{tag_remove bot_aa_guard_move}
					}
					{"actor_state"
						{selector
							{amount 1
							}
							{tag bot_aa_guard}
							{state "not dead"}
						}
						{drop orders}
						{move_mode hold}
						{ai_move
							{mode disable}
							{enable_on enemy}
						}
						{cannon fire}
					}
					{"delay"
						{time 0.1}
					}
					{"effect"
						{selector
							{amount 1
							}
							{tag bot_aa_guard}
						}
						{effect engine_off}
					}
					{"delay"
						{time 10}
					}
					{"trigger"
						{name "hf arty ai/part 4"}
					}
				}
			}
			{"hf arty ai/part 5"
				{condition
					{expression "!1 & 2"}
					{terms
						{"1.near"
							{units
								{amount 1
								}
								{tag bot_aa_guard}
								{state "not dead"}
							}
							{near_to
								{amount 1
								}
								{tag bot_arty_guard}
							}
							{distance 35}
						}
						{"2.near"
							{units
								{amount 1
								}
								{tag bot_aa_guard}
							}
							{near_to
								{amount 1
								}
								{tag bot_arty_guard}
							}
							{distance 50}
						}
					}
				}
				{actions
					{"delay"
						{time 2}
					}
					{"effect"
						{selector
							{tag bot_aa_guard_move}
						}
						{effect engine_on}
					}
					{"delay"
						{time 1}
					}
					{"action"
						{selector
							{amount 1
							}
							{tag bot_aa_guard_move}
							{state "not dead"}
						}
						{drop orders}
						{action move}
						{target
							{amount 1
							}
							{tag bot_arty_guard}
							{state "not dead"}
						}
					}
					{"delay"
						{time 10}
					}
					{"trigger"
						{name "hf arty ai/part 5"}
					}
				}
			}
			
