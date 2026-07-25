; Minimal CE repair/retreat slice for 799
; Restores AI engineer vehicle repair and damaged tank fallback without enabling the full CE AI logic stack.

;============== AI ENGINEER SCRIPTS =============
			{"ai_logic/tag_ai_engineer"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{zone
											{zone "gamezone"}
										}
									}
									{include
										{state
											{state operatable}
										}
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "pickaxe"}
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
											{tag ai_engineer}
										}
										{tag
											{tag player}
										}
										{tag
											{tag cmp_def}
										}
										{tag
											{tag _def}
										}
										{tag
											{tag _user_ally}
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
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{zone
										{zone "gamezone"}
									}
									{zone_ex
										{zone "gamezone"}
									}
								}
								{include
									{state
										{state operatable}
									}
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "pickaxe"}
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
										{tag ai_engineer}
									}
									{tag
										{tag player}
									}
									{tag
										{tag cmp_def}
									}
									{tag
										{tag _def}
									}
									{tag
										{tag _user_ally}
									}
								}
							}
						}
						{effect tag_ai_engineer}
					}
					{"delay"
						{time 0.5}
					}
					{"actor_state"
						{selector
							{tag ai_engineer}
							{type human}
							{state "not dead"}
						}
						{movement
							{speed assault}
							{kind fast}
						}
						{ai
							{follow_commander off}
						}
					}
					{"delay"
						{time 0.5}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag ai_engineer}
									}
								}
								{include
									{with_item
										{type existance}
										{item "pickaxe"}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag player}
									}
									{state
										{state dead}
									}
									{state
										{state user_control}
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
						{action remove}
						{item "pickaxe"}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "ai_logic/tag_ai_engineer"}
					}
				}
			}

			{"ai_logic/ai_repair_vehicles"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag body_break}
										}
										{tag
											{tag engine_brake}
										}
										{tag
											{tag turret_brake}
										}
										{tag
											{tag immobile}
										}
										{tag
											{tag gun_brake}
										}
									}
									{include
										{prop
											{prop vehicle}
										}
										{zone
											{zone "gamezone"}
										}
									}
									{exclude
										{destroyed_components
											{masks
												"body"
											}
										}
										{prop
											{prop airborne}
										}
										{tag
											{tag destroyed}
										}
										{prop
											{prop ammo_point}
										}
										{prop
											{prop parachute_attach}
										}
										{tag
											{tag repair_target}
										}
										{tag
											{tag ignore_repair}
										}
										{tag
											{tag not_selectable}
										}
										{state
											{state user_control}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag _user_ally}
										}
										{tag
											{tag _user}
										}
									}
								}
								{group
									{select
										{tag
											{tag _bot}
										}
									}
									{include
										{broken_components
											{masks
												"trackleft"
											}
										}
										{state
											{state inhabited}
										}
									}
									{exclude
										{destroyed_components
											{masks
												"body"
											}
										}
										{prop
											{prop airborne}
										}
										{tag
											{tag destroyed}
										}
										{prop
											{prop ammo_point}
										}
										{prop
											{prop parachute_attach}
										}
										{tag
											{tag repair_target}
										}
										{tag
											{tag ignore_repair}
										}
										{tag
											{tag not_selectable}
										}
										{state
											{state user_control}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag _user_ally}
										}
										{tag
											{tag _user}
										}
									}
								}
								{group
									{select
										{tag
											{tag _bot}
										}
									}
									{include
										{broken_components
											{masks
												"trackright"
											}
										}
										{state
											{state inhabited}
										}
									}
									{exclude
										{destroyed_components
											{masks
												"body"
											}
										}
										{prop
											{prop airborne}
										}
										{tag
											{tag destroyed}
										}
										{prop
											{prop ammo_point}
										}
										{prop
											{prop parachute_attach}
										}
										{tag
											{tag repair_target}
										}
										{tag
											{tag ignore_repair}
										}
										{tag
											{tag not_selectable}
										}
										{state
											{state user_control}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag _user_ally}
										}
										{tag
											{tag _user}
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
											{tag ai_engineer}
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
											{tag repairing}
										}
										{tag
											{tag player}
										}
										{tag
											{tag engineer_action}
										}
									}
								}
							}
						}
						{"3.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag player_hedgehog}
										}
									}
									{exclude
										{tag
											{tag crushed}
										}
									}
								}
							}
							{count
								{op "<<"}
								{value 4}
							}
						}
					}
				}
				{actions
					{"select"
						{source advanced}
						{group
							{select
								{tag
									{tag ai_engineer}
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
									{tag repairing}
								}
								{tag
									{tag player}
								}
								{tag
									{tag engineer_action}
								}
							}
						}
						{amount 1
						}
					}
					{"delay"
						{time 0.2}
					}
					{"effect"
						{selector
							{source current}
						}
						{effect prep_to_repair_veh}
					}
					{"delay"
						{time 0.2}
					}
					{"select"
						{source advanced}
						{group
							{select
								{tag
									{tag action_repair}
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
									{state dead}
								}
								{tag
									{tag player}
								}
								{state
									{state user_control}
								}
								{state
									{state linked}
								}
								{state
									{state inactive}
								}
							}
						}
						{amount 1
						}
					}
					{"delay"
						{time 0.2}
					}
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector
									{tag action_repair}
									{zone "gamezone"}
									{type human}
									{state "not dead"}
								}
								{count
									{op "=="}
									{value 0}
								}
							}
							{"entity_state"
								{selector
									{tag action_repair}
								}
								{tag_remove action_repair}
							}
							{"delay"
								{time 1}
							}
							{"trigger"
								{name "ai_logic/ai_repair_vehicles"}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag engine_brake}
											}
											{tag
												{tag turret_brake}
											}
											{tag
												{tag immobile}
											}
											{tag
												{tag gun_brake}
											}
										}
										{include
											{prop
												{prop vehicle}
											}
											{zone
												{zone "gamezone"}
											}
											{state
												{state "operatable inhabited"}
											}
										}
										{exclude
											{destroyed_components
												{masks
													"body"
												}
											}
											{prop
												{prop airborne}
											}
											{tag
												{tag destroyed}
											}
											{prop
												{prop ammo_point}
											}
											{prop
												{prop parachute_attach}
											}
											{tag
												{tag repair_target}
											}
											{tag
												{tag ignore_repair}
											}
											{tag
												{tag not_selectable}
											}
											{state
												{state user_control}
											}
											{tag
												{tag ammo_cooked}
											}
											{tag
												{tag _user_ally}
											}
											{tag
												{tag _user}
											}
										}
									}
									{group
										{select
											{tag
												{tag _bot}
											}
										}
										{include
											{broken_components
												{masks
													"trackleft"
												}
											}
											{state
												{state inhabited}
											}
										}
										{exclude
											{destroyed_components
												{masks
													"body"
												}
											}
											{prop
												{prop airborne}
											}
											{tag
												{tag destroyed}
											}
											{prop
												{prop ammo_point}
											}
											{prop
												{prop parachute_attach}
											}
											{tag
												{tag repair_target}
											}
											{tag
												{tag ignore_repair}
											}
											{tag
												{tag not_selectable}
											}
											{state
												{state user_control}
											}
											{tag
												{tag ammo_cooked}
											}
											{tag
												{tag _user_ally}
											}
											{tag
												{tag _user}
											}
										}
									}
									{group
										{select
											{tag
												{tag _bot}
											}
										}
										{include
											{broken_components
												{masks
													"trackright"
												}
											}
											{state
												{state inhabited}
											}
										}
										{exclude
											{destroyed_components
												{masks
													"body"
												}
											}
											{prop
												{prop airborne}
											}
											{tag
												{tag destroyed}
											}
											{prop
												{prop ammo_point}
											}
											{prop
												{prop parachute_attach}
											}
											{tag
												{tag repair_target}
											}
											{tag
												{tag ignore_repair}
											}
											{tag
												{tag not_selectable}
											}
											{state
												{state user_control}
											}
											{tag
												{tag ammo_cooked}
											}
											{tag
												{tag _user_ally}
											}
											{tag
												{tag _user}
											}
										}
									}
								}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag engine_brake}
											}
											{tag
												{tag turret_brake}
											}
											{tag
												{tag immobile}
											}
											{tag
												{tag gun_brake}
											}
										}
										{include
											{prop
												{prop vehicle}
											}
											{zone
												{zone "gamezone"}
											}
											{state
												{state "operatable inhabited"}
											}
										}
										{exclude
											{destroyed_components
												{masks
													"body"
												}
											}
											{prop
												{prop airborne}
											}
											{tag
												{tag destroyed}
											}
											{prop
												{prop ammo_point}
											}
											{prop
												{prop parachute_attach}
											}
											{tag
												{tag repair_target}
											}
											{tag
												{tag ignore_repair}
											}
											{tag
												{tag not_selectable}
											}
											{state
												{state user_control}
											}
											{tag
												{tag ammo_cooked}
											}
											{tag
												{tag _user_ally}
											}
											{tag
												{tag _user}
											}
										}
									}
									{group
										{select
											{tag
												{tag _bot}
											}
										}
										{include
											{broken_components
												{masks
													"trackleft"
												}
											}
											{state
												{state inhabited}
											}
										}
										{exclude
											{destroyed_components
												{masks
													"body"
												}
											}
											{prop
												{prop airborne}
											}
											{tag
												{tag destroyed}
											}
											{prop
												{prop ammo_point}
											}
											{prop
												{prop parachute_attach}
											}
											{tag
												{tag repair_target}
											}
											{tag
												{tag ignore_repair}
											}
											{tag
												{tag not_selectable}
											}
											{state
												{state user_control}
											}
											{tag
												{tag ammo_cooked}
											}
											{tag
												{tag _user_ally}
											}
											{tag
												{tag _user}
											}
										}
									}
									{group
										{select
											{tag
												{tag _bot}
											}
										}
										{include
											{broken_components
												{masks
													"trackright"
												}
											}
											{state
												{state inhabited}
											}
										}
										{exclude
											{destroyed_components
												{masks
													"body"
												}
											}
											{prop
												{prop airborne}
											}
											{tag
												{tag destroyed}
											}
											{prop
												{prop ammo_point}
											}
											{prop
												{prop parachute_attach}
											}
											{tag
												{tag repair_target}
											}
											{tag
												{tag ignore_repair}
											}
											{tag
												{tag not_selectable}
											}
											{state
												{state user_control}
											}
											{tag
												{tag ammo_cooked}
											}
											{tag
												{tag _user_ally}
											}
											{tag
												{tag _user}
											}
										}
									}
									{sort
										{type entity}
										{entity
											{amount 1
											}
											{tag ai_spawn}
											{zone "gamezone"}
										}
									}
									{amount 1
									}
								}
								{tag_add repair_target}
							}
						}
						{"default"
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag body_break}
											}
											{tag
												{tag engine_brake}
											}
											{tag
												{tag turret_brake}
											}
											{tag
												{tag immobile}
											}
											{tag
												{tag gun_brake}
											}
										}
										{include
											{prop
												{prop vehicle}
											}
											{zone
												{zone "gamezone"}
											}
										}
										{exclude
											{destroyed_components
												{masks
													"body"
												}
											}
											{prop
												{prop airborne}
											}
											{tag
												{tag destroyed}
											}
											{prop
												{prop ammo_point}
											}
											{prop
												{prop parachute_attach}
											}
											{tag
												{tag repair_target}
											}
											{tag
												{tag ignore_repair}
											}
											{tag
												{tag not_selectable}
											}
											{state
												{state user_control}
											}
											{tag
												{tag ammo_cooked}
											}
											{tag
												{tag _user_ally}
											}
											{tag
												{tag _user}
											}
										}
									}
									{sort
										{type entity}
										{entity
											{amount 1
											}
											{tag action_repair}
											{zone "gamezone"}
											{type human}
											{state "not dead"}
										}
									}
									{amount 1
									}
								}
								{tag_add repair_target}
							}
						}
					}
					{"delay"
						{time 0.4}
					}
					{"action"
						{selector
							{source current}
						}
						{drop orders}
						{action repair}
						{target
							{amount 1
							}
							{tag repair_target}
							{zone "gamezone"}
							{type vehicle}
						}
					}
					{"delay"
						{time 0.2}
					}
					{"actor_state"
						{selector
							{source current}
						}
						{movement
							{speed assault}
							{kind fast}
						}
					}
					{"delay"
						{time 0.5}
					}
					{"effect"
						{selector
							{tag repair_target}
						}
						{effect ignore_repair_75}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source current}
						}
						{tag_remove action_repair}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "ai_logic/ai_repair_vehicles"}
					}
				}
			}

			{"ai_logic/reverse_tank_to_repair"
				{condition
					{expression "1 & !2"}
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag turret_brake}
										}
										{tag
											{tag gun_brake}
										}
									}
									{include
										{zone
											{zone "gamezone"}
										}
										{prop
											{prop tank}
										}
										{tag
											{tag _bot}
										}
										{state
											{state "operatable inhabited"}
										}
									}
									{exclude
										{state
											{state user_control}
										}
										{state
											{state linked}
										}
										{state
											{state inactive}
										}
										{tag
											{tag _user_ally}
										}
										{broken_components
											{masks
												"trackleft"
											}
										}
										{broken_components
											{masks
												"trackright"
											}
										}
										{tag
											{tag engine_brake}
										}
										{tag
											{tag selected_tank_to_repair}
										}
										{tag
											{tag reversing_to_repair}
										}
										{zone
											{zone "a_1"}
										}
										{zone
											{zone "a_2"}
										}
										{zone
											{zone "a_3"}
										}
										{zone
											{zone "a_4"}
										}
										{zone
											{zone "b_1"}
										}
										{zone
											{zone "b_2"}
										}
										{zone
											{zone "b_3"}
										}
										{zone
											{zone "b_4"}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{select
										{tag
											{tag _user}
										}
										{tag
											{tag _user_ally}
										}
									}
									{include
										{state
											{state operatable}
										}
									}
									{exclude
										{state
											{state inactive}
										}
										{state
											{state dead}
										}
										{prop
											{prop airborne}
										}
									}
								}
							}
							{distance 200}
						}
						{"2.trigger"
							{name "ai_logic/stop_reversing"}
						}
					}
				}
				{actions
					{"delay"
						{time 5}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag turret_brake}
									}
									{tag
										{tag gun_brake}
									}
								}
								{include
									{zone
										{zone "gamezone"}
									}
									{prop
										{prop tank}
									}
									{tag
										{tag _bot}
									}
									{state
										{state "operatable inhabited"}
									}
								}
								{exclude
									{state
										{state user_control}
									}
									{state
										{state linked}
									}
									{state
										{state inactive}
									}
									{tag
										{tag _user_ally}
									}
									{broken_components
										{masks
											"trackleft"
										}
									}
									{broken_components
										{masks
											"trackright"
										}
									}
									{tag
										{tag engine_brake}
									}
									{tag
										{tag selected_tank_to_repair}
									}
									{tag
										{tag reversing_to_repair}
									}
									{zone
										{zone "a_1"}
									}
									{zone
										{zone "a_2"}
									}
									{zone
										{zone "a_3"}
									}
									{zone
										{zone "a_4"}
									}
									{zone
										{zone "b_1"}
									}
									{zone
										{zone "b_2"}
									}
									{zone
										{zone "b_3"}
									}
									{zone
										{zone "b_4"}
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
												{tag _user}
											}
											{tag
												{tag _user_ally}
											}
										}
										{include
											{state
												{state operatable}
											}
											{zone
												{zone "gamezone"}
											}
										}
										{exclude
											{state
												{state inactive}
											}
											{state
												{state dead}
											}
											{prop
												{prop airborne}
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
						{tag_add selected_tank_to_repair}
					}
					{"delay"
						{time 0.2}
					}
					{"select"
						{tag selected_tank_to_repair}
						{type tank}
						{state inhabited}
					}
					{"delay"
						{time 0.2}
					}
					{"actor_state"
						{selector
							{source current}
						}
						{drop orders}
						{movement
							{speed fast}
							{kind fast}
							{forward off}
							{backward on}
						}
					}
					{"delay"
						{time 0.2}
					}
					{"action"
						{selector
							{source current}
						}
						{action move}
						{target
							{source advanced}
							{group
								{select
									{tag
										{tag ai_spawn}
									}
								}
								{include
									{zone
										{zone "gamezone"}
									}
								}
								{exclude
									{state
										{state inactive}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag selected_tank_to_repair}
									{type tank}
									{state inhabited}
								}
							}
						}
					}
					{"delay"
						{time 0.2}
					}
					{"effect"
						{selector
							{source current}
						}
						{effect retreat_to_repair}
					}
					{## "timer"
						{time 120}
						{title "falling back"}
					}
					{"delay"
						{time 0.2}
					}
					{"trigger"
						{name "ai_logic/reverse_tank_to_repair"}
					}
				}
			}
			{"ai_logic/stop_reversing"
				{condition
					{expression "1 & !2"}
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag ai_stop_reversing}
										}
									}
								}
							}
						}
						{"2.trigger"
							{name "ai_logic/reverse_tank_to_repair"}
						}
					}
				}
				{actions
					{## "timer"
						{time 3}
						{title "AI tank stop reversing"}
					}
					{"entity_state"
						{selector
							{amount 1
							}
							{tag ai_stop_reversing}
							{type vehicle}
							{state inhabited}
						}
						{tag_add reversing_ai_selected}
						{tag_remove ai_stop_reversing}
					}
					{"delay"
						{time 0.1}
					}
					{"select"
						{amount 1
						}
						{tag reversing_ai_selected}
						{type vehicle}
						{state inhabited}
					}
					{"delay"
						{time 0.1}
					}
					{"effect"
						{selector
							{source current}
						}
						{effect engine_toggle}
					}
					{"delay"
						{time 0.1}
					}
					{"effect"
						{selector
							{tag reversing_ai_selected}
						}
						{effect tag_ai_for_static_repairng}
					}
					{"delay"
						{time 0.2}
					}
					{"trigger"
						{name "ai_logic/stop_reversing"}
					}
				}
			}
			{"ai_logic/enable_forward_movement"
				{condition
					{expression "1"}
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag ai_ready_to_move}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{## "timer"
						{time 3}
						{title "AI tank stop reversing"}
					}
					{"entity_state"
						{selector
							{amount 1
							}
							{tag ai_ready_to_move}
							{type vehicle}
							;{state inhabited}
						}
						{tag_add ready_to_move_ai_selected}
					}
					{"delay"
						{time 0.1}
					}
					{"select"
						{amount 1
						}
						{tag ready_to_move_ai_selected}
						{type vehicle}
						;{state inhabited}
					}
					{"delay"
						{time 0.1}
					}
					{"effect"
						{selector
							{source current}
						}
						{effect engine_toggle}
					}
					;{"effect"
					;	{selector
					;		{source current}
					;	}
					;	{effect engine_toggle}
					;}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{tag ready_to_move_ai_selected}
						}
						{tag_remove ai_ready_to_move}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{tag ready_to_move_ai_selected}
						}
						{tag_remove ready_to_move_ai_selected}
					}
					{"delay"
						{time 0.2}
					}
					{"trigger"
						{name "ai_logic/stop_reversing"}
					}
				}
			}
