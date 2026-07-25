;// Scripts written by Horten
;// Minimal edits: allow user-controlled units to loot too
;//Mine clearing
			{"hf inf ai/clear mines"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag mine_planted}
										}
									}
									{exclude
										{tag
											{tag cmp_def}
										}
									}
								}
							}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag ai_using_mine_detector}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 0.5}
					}
					{"actor_state"
						{selector
							{tag ai_using_mine_detector}
						}
						{fire_mode hold}
						{ai_move
							{mode disable}
						}
						{movement
							{speed fast}
						}
						{ai
							{follow_commander off}
						}
					}
					{"loop" 6
						{"entity_state"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag mine_planted}
										}
									}
									{exclude
										{tag
											{tag selected_for_clearing}
										}
										{state
											{state inactive}
										}
										{tag
											{tag mine_to_clear}
										}
									}
								}
								{sort
									{type entity}
									{entity
										{source advanced}
										{group
											{select
												{tag
													{tag mine_to_clear}
												}
											}
											{exclude
												{state
													{state inactive}
												}
											}
										}
										{amount 1
										}
									}
								}
								{amount 1
								}
							}
							{tag_add mine_to_clear}
						}
						{"delay"
							{time 0.5}
						}
						{"action"
							{selector
								{amount 1
								}
								{tag ai_using_mine_detector}
							}
							{action advance}
							{target
								{source advanced}
								{group
									{select
										{tag
											{tag mine_to_clear}
										}
									}
									{exclude
										{tag
											{tag selected_for_clearing}
										}
										{state
											{state inactive}
										}
									}
								}
								{amount 1
								}
								{sort
									{type entity}
									{entity
										{amount 1
										}
										{tag ai_using_mine_detector}
									}
								}
							}
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
											{tag mine_to_clear}
										}
									}
									{exclude
										{state
											{state inactive}
										}
									}
								}
							}
							{tag_add selected_for_clearing}
							{tag_remove mine_to_clear}
						}
						{"delay"
							{time 0.5}
						}
					}
					{"delay"
						{time 0.5}
					}
					{"entity_state"
						{selector
							{tag selected_for_clearing}
						}
						{tag_remove selected_for_clearing}
					}
					{"delay"
						{time 10}
					}
					{"trigger"
						{name "hf inf ai/clear mines"}
					}
				}
			}
			{"hf inf ai/clear mines tag"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag bot}
										}
									}
									{include
										{with_item
											{type amount}
											{item "mine_detector"}
											{min 1}
										}
									}
									{exclude
										{state
											{state "dead inactive linked user_control"}
										}
									}
								}
							}
						}
						{"2.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag bot}
										}
									}
									{include
										{with_item
											{type amount}
											{item "mine_detector"}
											{min 1}
										}
									}
									{exclude
										{state
											{state "dead inactive user_control"}
										}
										{tag}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{select
										{tag
											{tag mine_planted}
										}
									}
								}
							}
							{distance 140}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"select"
						{source advanced}
						{group
							{select
								{tag
									{tag bot}
								}
							}
							{include
								{with_item
									{type amount}
									{item "mine_detector"}
									{min 1}
								}
							}
							{exclude
								{tag
									{tag ai_using_mine_detector}
								}
							}
						}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source current}
						}
						{tag_add ai_mine_detector}
					}
					{"entity_state"
						{selector
							{source current}
						}
						{tag_add _lua_mi}
					}
					{"delay"
						{time 0.1}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag ai_mine_detector}
									}
								}
								{exclude
									{tag
										{tag ai_using_mine_detector}
									}
								}
							}
						}
						{action take}
						{item "mine_detector"}
					}
					{"delay"
						{time 2}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag ai_mine_detector}
									}
								}
								{include
									{with_item
										{type using}
										{item "mine_detector"}
									}
								}
							}
						}
						{tag_add ai_using_mine_detector}
						{tag_remove ai_mine_detector}
					}
					{"delay"
						{time 10}
					}
					{"trigger"
						{name "hf inf ai/clear mines tag"}
					}
				}
			}
			{"hf inf ai/clear mines detag"
				{condition
					{terms
						{"1.entities"
							{selector
								{tag ai_using_mine_detector}
							}
						}
						{"2.entities"
							{selector
								{tag mine_planted}
							}
							{count
								{op "=="}
								{value 0}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"action"
						{selector
							{tag ai_using_mine_detector}
						}
						{drop orders}
						{action cancel}
					}
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{selector
							{tag ai_using_mine_detector}
						}
						{drop orders}
						{control AI}
						{fire_mode open}
						{move_mode free}
						{ai_move
							{mode enable}
						}
						{movement
							{speed assault}
						}
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
										{tag ai_mine_detector}
									}
								}
							}
						}
						{tag_remove ai_mine_detector}
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
										{tag ai_using_mine_detector}
									}
								}
							}
						}
						{tag_remove _lua_mi}
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
										{tag ai_using_mine_detector}
									}
								}
							}
						}
						{tag_remove ai_using_mine_detector}
					}
					{"delay"
						{time 10}
					}
					{"trigger"
						{name "hf inf ai/clear mines detag"}
					}
				}
			}

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
							{tag bot_arty_move}
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
							{tag bot_arty_move}
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
							{tag bot_arty_move}
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
								}
							}
						}
						{tag_add bot_arty_fire}
						{tag_remove bot_arty_move}
					}
					{"actor_state"
						{selector
							{tag bot_arty_fire}
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
							{tag bot_arty_fire}
						}
						{action rotate}
						{target
							{source advanced}
							{group
								{include
									{state
										{state user_control}
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
							{tag bot_arty_fire}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{prop
										{prop vehicle}
									}
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
								}
							}
						}
						{interval 0}
						{time 210}
						{burst_mode long}
					}
					{"delay"
						{time 30}
					}
					{"actor_fire"
						{selector
							{tag bot_arty_fire}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{prop
										{prop vehicle}
									}
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
								}
							}
						}
						{interval 0}
						{time 210}
						{burst_mode long}
					}
					{"delay"
						{time 30}
					}
					{"actor_fire"
						{selector
							{tag bot_arty_fire}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{prop
										{prop vehicle}
									}
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
								}
							}
						}
						{interval 0}
						{time 210}
						{burst_mode long}
					}
					{"delay"
						{time 190}
					}
					{"entity_state"
						{selector
							{tag bot_arty_fire}
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
							{distance 15}
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
			{"hf arty ai/part 6"
				{condition
					{type 1}
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag engine_count_2}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag airstrike_rus_500}
										}
										{tag
											{tag airstrike_rus_50}
										}										
										{tag
											{tag airstrike_rus_rocket}
										}										
										{tag
											{tag airstrike_usa_rocket_hvar}
										}										
										{tag
											{tag airstrike_usa_rocket_m8}
										}											
										{tag
											{tag airstrike_ger_strafe}
										}											
										{tag
											{tag airstrike_ger_recon}
										}											
										{tag
											{tag airstrike_usa}
										}											
										{tag
											{tag airstrike_ger}
										}											
										{tag
											{tag airstrike_rus}
										}																	
										{tag
											{tag shotdown}
										}
										{tag
											{tag destroyed}
										}
									}
								}
							}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag engine_count_1}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag airstrike_rus_500}
										}
										{tag
											{tag airstrike_rus_50}
										}										
										{tag
											{tag airstrike_rus_rocket}
										}										
										{tag
											{tag airstrike_usa_rocket_hvar}
										}										
										{tag
											{tag airstrike_usa_rocket_m8}
										}											
										{tag
											{tag airstrike_ger_strafe}
										}											
										{tag
											{tag airstrike_ger_recon}
										}											
										{tag
											{tag airstrike_usa}
										}											
										{tag
											{tag airstrike_ger}
										}											
										{tag
											{tag airstrike_rus}
										}																	
										{tag
											{tag shotdown}
										}
										{tag
											{tag destroyed}
										}
									}
								}
							}
						}
						{"3.near"
							{units
								{tag bot_arty}
							}
							{near_to
								{tag flare_landed}
							}
							{distance 60}
						}
						{"4.near"
							{units
								{tag bot_arty}
							}
							{near_to
								{tag crosshair}
							}
							{distance 60}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{tag bot_arty}
						}
						{tag_add bot_arty_skedaddle}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{tag bot_arty_skedaddle}
						}
						{tag_remove bot_arty_move}
					}
					{"delay"
						{time 3}
					}
					{"actor_state"
						{selector
							{tag bot_arty_skedaddle}
						}
						{drop orders}
						{movement
							{speed fast}
						}
						{cannon move}
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
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "1"}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.33}
							}
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "2"}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.33}
							}
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "3"}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.33}
							}
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "4"}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.33}
							}
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "5"}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.33}
							}
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "6"}
							}
						}
						{"default"
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "0"}
							}
						}
					}
					{"delay"
						{time 35}
					}
					{"entity_state"
						{selector
							{tag bot_arty_skedaddle}
						}
						{tag_remove bot_arty_skedaddle}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "hf arty ai/part 6"}
					}
				}
			}
			
			{"hf medic ai/medic smoke"
				{condition
					{terms
						{"1.see_actors"
							{selector
								{tag medic}
							}
							{enemy
								{source advanced}
								{group
									{select
										{tag
											{tag bleeding}
										}
									}
									{exclude
										{state
											{state user_control}
										}
									}
								}
							}
							{distance
								{mode in_range}
								{min 16}
								{max 21}
							}
							{detection visible}
							{tag
								{"tag pair"}
								{"for selector" medic_smoke}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{selector
							{tag medic_smoke}
						}
						{drop orders}
					}
					{"inventory"
						{selector
							{tag medic_smoke}
						}
						{action add}
						{item "nbk39" "grenade"}
					}
					{"delay"
						{time 0.2}
					}
					{"inventory"
						{selector
							{tag medic_smoke}
							{type human}
							{state "not dead"}
						}
						{action take}
						{item "nbk39" "grenade"}
					}
					{"delay"
						{time 1}
					}
					{"actor_state"
						{selector
							{tag medic_smoke}
						}
						{move_mode hold}
					}
					{"action"
						{selector
							{tag medic_smoke}
							{state "not dead"}
						}
						{drop "orders sensor senseless"}
						{action attack_ground}
						{target
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{select
									{tag
										{tag bleeding}
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
					{"delay"
						{time 5}
					}
					{"action"
						{selector
							{tag medic_smoke}
							{state "not dead"}
						}
						{drop orders}
						{action cancel}
						{target
							{ignore_captured_by_user 0}
						}
					}
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{selector
							{tag medic_smoke}
						}
						{drop orders}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{tag medic}
						}
						{tag_remove medic_smoke}
					}
					{"delay"
						{time 5}
					}
					{"trigger"
						{name "hf medic ai/medic smoke"}
					}
				}
			}