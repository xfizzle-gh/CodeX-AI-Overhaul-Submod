;created by ordos
(include "/map/common_sp_scripts.inc")
(include "/map/ordos's trigger.inc")
			{"[nikral] airstrike script for dcg"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{tag
											{tag ai_cas}
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
											{state inactive}
										}
										{tag
											{tag assigned}
										}
										{tag
											{tag ai_plane}
										}
									}
								}
							}
						}
						{"2.entities"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{prop
											{prop vehicle}
										}
										{state
											{state user_control}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{state
											{state inactive}
										}
										{prop
											{prop airborne}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"music"
						{start "/sound/mission/task_new.wav"}
						{volume narrator}
					}
					{"entity_state"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									{tag
										{tag ai_cas}
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
										{state inactive}
									}
									{tag
										{tag assigned}
									}
									{tag
										{tag ai_plane}
									}
								}
							}
							{amount 1
							}
						}
						{tag_add ai_plane}
					}
					;{"animate"
					;	{selector
					;		{ignore_captured_by_user 0}
					;		{tag ai_plane}
					;	}
					;	{animation wheels}
					;	{speed 10}
					;}
					{"air_state"
						{selector
							{tag ai_plane}
						}
						{altitude 40}
					}
					{"delay"
						{time 0.1}
					}
					{"air_attack"
						{selector
							{ignore_captured_by_user 0}
							{tag ai_plane}
						}
						{drop orders}
						{enemies
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									{prop
										{prop vehicle}
									}
									{state
										{state operatable}
									}
									{state
										{state user_control}
									}
								}
								{exclude
									{state
										{state inactive}
									}
									{prop
										{prop airborne}
									}
								}
							}
							{amount 1
							}
							{sort
								{type shuffle}
							}
						}
						{attack_altitude 30}
					}


					;{"action"

					;    {selector
					;		{ignore_captured_by_user 0}
					;		{tag ai_plane}
					;    }
					 ;   {action move}
					;		{target
					;			{ignore_captured_by_user 0}
					;			{source advanced}
					;			{group
					;			{include
					;				{prop
					;					{prop vehicle}
					;				}
					;				{state
					;					{state operatable}
					;				}
					;				{state
					;					{state user_control}
					;				}
					;			}
					;			{exclude
					;				{state
					;					{state inactive}
					;				}
					;				{prop
					;					{prop airborne}
					;				}
					;			}
					;			}
					;			{amount 1
					;			}
					;
					;		}
					;}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag ai_plane}
						}
						{tag_add assigned}
						{tag_remove ai_plane}
						{tag_remove ai_cas}
						;{drop sensor}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "[nikral] airstrike script for dcg"}
					}
				}
			}
			{"blow_panel"
				{autoreset}
				{condition
					{terms
						{"2.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag blow_panel_active}
							}
						}
					}
				}
				{actions
					{"select"
						{amount 1
						}
						{tag blow_panel_active}
					}
					{"inventory"
						{selector
							{source current}
							{ignore_captured_by_user 0}
						}
						{action remove}
						{item "bullet120_m256" "apfsds" "ammo"}
						{amount 50}
					}
					{"inventory"
						{selector
							{source current}
							{ignore_captured_by_user 0}
						}
						{action remove}
						{item "bullet120_m256" "apfsdsl" "ammo"}
						{amount 50}
					}
					{"inventory"
						{selector
							{source current}
							{ignore_captured_by_user 0}
						}
						{action remove}
						{item "bullet120_m256" "apfsdsa" "ammo"}
						{amount 50}
					}
					{"inventory"
						{selector
							{source current}
							{ignore_captured_by_user 0}
						}
						{action remove}
						{item "bullet120_m256" "heat" "ammo"}
						{amount 50}
					}
					{"inventory"
						{selector
							{source current}
							{ignore_captured_by_user 0}
						}
						{action remove}
						{item "bullet105_m68a1" "apfsds" "ammo"}
						{amount 50}
					}
					{"inventory"
						{selector
							{source current}
							{ignore_captured_by_user 0}
						}
						{action remove}
						{item "bullet105_m68a1" "apfsdsl" "ammo"}
						{amount 50}
					}
					{"inventory"
						{selector
							{source current}
							{ignore_captured_by_user 0}
						}
						{action remove}
						{item "bullet105_m68a1" "apfsdsla" "ammo"}
						{amount 50}
					}
					{"inventory"
						{selector
							{source current}
							{ignore_captured_by_user 0}
						}
						{action remove}
						{item "bullet105_m68a1" "heat" "ammo"}
						{amount 50}
					}
					{"inventory"
						{selector
							{source current}
							{ignore_captured_by_user 0}
						}
						{action remove}
						{item "bullet105_m68a1" "hesh" "ammo"}
						{amount 50}
					}
					{"inventory"
						{selector
							{source current}
							{ignore_captured_by_user 0}
						}
						{action remove}
						{item "bullet120_rh120" "apfsdsl" "ammo"}
						{amount 50}
					}
					{"inventory"
						{selector
							{source current}
							{ignore_captured_by_user 0}
						}
						{action remove}
						{item "bullet120_rh120" "heat" "ammo"}
						{amount 50}
					}
					{"entity_state"
						{selector
							{source current}
						}
						{tag_remove blow_panel_active}
					}
				}
			}
			{"link"
				{condition
					{terms
						{"1.near"
							{units
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										;{prop
										;	{prop sprutsd}
										;}
										{tag
											{tag il76_para}
										}
										{state
											{state "operatable moveable chassis_work user_control"}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state linked}
										}
									}
								}
							}

							{near_to
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{prop
											{prop "il-76td"}
										}
										{state
											{state "operatable moveable chassis_work user_control"}
										}
									}
									{exclude
										{state
											{state dead}
										}
									}
								}
							}
							{distance 150}
						}
					}
				}
				{actions
					{"linker"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									{prop
										{prop "il-76td"}
									}
									{state
										{state "operatable moveable chassis_work user_control"}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									;{prop
									;	{prop sprutsd}
									;}
									{tag
											{tag il76_para}
									}
									{state
										{state "operatable moveable chassis_work user_control"}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
						}

						{place
							{type specific}
							{name vehicle1}
						}
					}
					{"entity_state"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									{prop
										{prop "il-76td"}
									}
									{state
										{state "operatable moveable chassis_work user_control"}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{tag
										{tag place1_busy}
									}
								}
							}
							{amount 1
							}
						}
						{tag_add place1_busy}
					}
					{## "delay"
						{time 1}
					}
					{"effect"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{select
									{tag
										{tag place1_busy}
									}
								}
								{include
									{state
										{state "operatable moveable chassis_work user_control"}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
						}
						{effect check_vdvstate}
					}
					{"trigger"
						{name "link"}
					}
				}
			}


	{"airdrop scripts/auto board"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{tag
											{tag para_board}
										}
									}
									{exclude
										{state
											{state inactive}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"action"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
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
										{state linked}
									}
									{state
										{state inactive}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{include
											{tag
												{tag para_board}
											}
										}
										{exclude
											{state
												{state inactive}
											}
										}
									}
								}
							}
							{amount 1
							}
						}
						{action board}
						{target
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									{tag
										{tag para_board}
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
					{"entity_state"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									{tag
										{tag para_board}
									}
								}
								{exclude
									{state
										{state inactive}
									}
								}
							}
						}
						{"delay"
						{time 5}
						}
						{tag_remove para_board}
					}
					{"trigger"
						{name "airdrop scripts/auto board"}
					}
				}
		}
		(define "check_user"
				{"entity_state"
					{selector
						{ignore_captured_by_user 0}
						{tag wait_para_player_set}
						{player "%0"}
					}
					{tag_add player%0}
				}
		)

		{"check_user"
				{condition
					{terms
						{"1.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag wait_para_player_set}
							}
						}
					}
				}
				{actions
					("check_user" args 0)
					("check_user" args 1)
					("check_user" args 2)
					("check_user" args 3)
					("check_user" args 4)
					("check_user" args 5)
					("check_user" args 6)
					("check_user" args 7)
					("check_user" args 8)
					;("check_user" args 9)
					;("check_user" args 10)
					;("check_user" args 11)
					;("check_user" args 12)
					;("check_user" args 13)
					;("check_user" args 14)
					;("check_user" args 15)
					;("check_user" args 16)
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag wait_para_player_set}
						}
						{tag_remove wait_para_player_set}
					}
					{"trigger"
						{name "check_user"}
					}
				}
			}

		{"flash_board"
				{autoreset}
				{condition
				   {terms
						{"1.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag target_plane}
							}
						}
						{"2.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag transport_crew}
							}
						}
						{"3.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag target_vehicle}
							}
						}
					}
				}
				{actions
					{"linker"
						{selector
							{ignore_captured_by_user 0}
							{tag target_plane}

						}
						{action unlink}
						{entities
							{ignore_captured_by_user 0}
							{tag transport_crew}


						}
					}
					{"delay"
						{time 0.5}
					}
					{"linker"
						{selector
							{ignore_captured_by_user 0}
							{tag target_vehicle}
						}
						{entities
							{ignore_captured_by_user 0}
							{tag transport_crew}
						}
					}
					{"delay"
						{time 0.5}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag transport_crew}
						}
						{tag_remove transport_crew}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag target_vehicle}
						}
						{tag_remove target_vehicle}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag target_plane}
						}
						{tag_remove target_plane}
					}
				   {"trigger"}
				;		{name "flash_board"}
				  ; }
				}
			}



		{"3333"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
						 				{tag
						 					{tag "transport_crew"}
						 				}
									}
								}
							}
						}
					}
				}
				{actions
					{"actor_state"
						{selector
							{ignore_captured_by_user 0}
							{tag "transport_crew"}
						}
						{control user}
						{move_mode hold}
					}
					{"trigger"
						{name "3333"}
					}
				}
			}
			{"4444"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
						 				{tag
						 					{tag "target_vehicle"}
						 				}
									}
								}
							}
						}
					}
				}
				{actions
					{"actor_state"
						{selector
							{ignore_captured_by_user 0}
							{tag "target_vehicle"}
						}
						{control user}
						{move_mode hold}
					}
					{"trigger"
						{name "4444"}
					}
				}
			}


		{tasks
			{"begin"
				{desc "Fuel is running out, prepare to evacuate!"}
				{primary}
				{state hidden}
			}
			{"intro"
				{desc "Fuel is running out, prepare to evacuate!"}
				{primary}
				{state hidden}
			}
			{"exit"
				{desc "Evac Winchester！"}
				{primary}
				{state hidden}
			}
			{"exit_f"
				{desc "Evac Winchester！"}
				{primary}
				{state hidden}
			}
		}

			{"begin"
				{condition
				}
				{actions
					{"task"
						{name "begin"}
					}
					{"action"
						{selector
							{ignore_captured_by_user 0}
							{tag plane_fuel_check}
						}
						{drop orders}
						{action move}
						{target
							{ignore_captured_by_user 0}
							{tag world_center}
						}
					}
					{"delay"
						{time 5}
					}
					{"task"
						{name "intro"}
					}
				}
			}
			{"air_exit_processor"
				{condition
				}
				{actions
					{"if"
						{condition
							{type entities}
							{selector
								{ignore_captured_by_user 0}
								{tag plane_test_to_exit}
							}
						}
						{"entity_state"
							{selector
								{ignore_captured_by_user 0}
								{amount 1
								}
								{tag plane_test_to_exit}
							}
							{tag_add plane_test_to_exit_selected}
						}
						{"ables"
							{selector
								{ignore_captured_by_user 0}
								{tag plane_test_to_exit_selected}
							}
							;{drop orders}
							;{drop sensor}
							{remove select}
						}
						{"actor_state"
							{selector
								{ignore_captured_by_user 0}
								{tag plane_test_to_exit_selected}
							}
							{drop orders}
							;{drop sensor}
							{fire_mode hold}
							{move_mode hold}
							{control AI}
						}
						{"delay"
							{time 0.5}
						}
						{"actor_state"
							{selector
								{ignore_captured_by_user 0}
								{tag plane_test_to_exit_selected}
							}
							{drop sensor}
						}
						{"delay"
							{time 1}
						}
						{"air_state"
							{selector
								{ignore_captured_by_user 0}
								{tag plane_test_to_exit_selected}
							}
							{altitude 75}
						}
						{"delay"
							{time 0.1}
						}
						{"effect"
							{selector
								{ignore_captured_by_user 0}
								{tag plane_test_to_exit_selected}
							}
							{effect air_exit}
						}
						{"delay"
							{time 0.5}
						}
						{"action"
							{selector
								{ignore_captured_by_user 0}
								{tag plane_test_to_exit_selected}
							}
							{drop orders}
							{action move}
							{target
								{ignore_captured_by_user 0}
								{amount 1
								}
								{tag air_exit_position}
							}
						}
						{"entity_state"
							{selector
								{ignore_captured_by_user 0}
								{tag plane_test_to_exit_selected}
							}
							{tag_add plane_test_exiting}
							{tag_remove plane_test_to_exit}
						}
						{"entity_state"
							{selector
								{ignore_captured_by_user 0}
								{tag plane_test_to_exit_selected}
							}
							{tag_remove plane_test_to_exit_selected}
						}
						{"task"
							{name "exit"}
						}
					}
					{"trigger"
						{name "air_exit_processor"}
					}
				}
			}
			{"air_check_fuel"
				{condition
				}
				{actions
					{"if"
						{condition
							{type entities}
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{tag
											{tag plane_fuel_check}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag plane_test_to_exit}
										}
										{tag
											{tag plane_test_exiting}
										}
									}
								}
							}
						}
						{"entity_state"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{tag
											{tag plane_fuel_check}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag plane_test_to_exit}
										}
										{tag
											{tag plane_test_exiting}
										}
									}
								}
								{amount 1
								}
							}
							{tag_add plane_test_check_fuel}
						}
						{"if"
							{condition
								{type inventory}
								{selector
									{ignore_captured_by_user 0}
									{tag plane_test_check_fuel}
								}
								{where fuel}
								{compare less}
								{amount 150}
							}
							{"entity_state"
								{selector
									{ignore_captured_by_user 0}
									{tag plane_test_check_fuel}
								}
								{tag_add plane_test_to_exit}
							}
						}
						{"entity_state"
							{selector
								{ignore_captured_by_user 0}
								{tag plane_test_check_fuel}
							}
							{tag_remove plane_test_check_fuel}
						}
					}
					{"trigger"
						{name "air_check_fuel"}
					}
				}
			}
			{"air_exit_deleter"
				{condition
				}
				{actions
					{"if"
						{condition
							{type near}
							{units
								{ignore_captured_by_user 0}
								{tag plane_test_exiting}
							}
							{near_to
								{ignore_captured_by_user 0}
								{tag air_exit_position}
							}
							{distance 100}
						}
						{"entity_state"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{amount 1
								}
								{group
									{include
										{tag
											{tag plane_test_exiting}
										}
									}
								}
								{sort
									{type entity}
									{entity
										{ignore_captured_by_user 0}
										{tag air_exit_position}
									}
								}
							}
							{tag_add plane_test_del_check}
						}
						{"if"
							{condition
								{type near}
								{units
									{ignore_captured_by_user 0}
									{tag plane_test_del_check}
								}
								{near_to
									{ignore_captured_by_user 0}
									{tag air_exit_position}
								}
								{distance 100}
							}
							{"delete"
								{selector
									{ignore_captured_by_user 0}
									{tag plane_test_del_check}
								}
							}
							{"task"
								{name "exit_f"}
							}
						}
					}
					{"trigger"
						{name "air_exit_deleter"}
					}
				}
			}




	{"ai switch/switch_to_user"
				{condition
					{terms
						{"1.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag switch_to_user}
							}
						}
					}
				}
				{actions
					{"actor_state"
						{selector
							{ignore_captured_by_user 0}
							{tag switch_to_user}
						}
						{drop senseless}
						{control user}
						{able select}
						;{move_mode hold}
						{ai_move
							{mode enable}
						}
						{ai
							{no_advance off}
							{no_retreat off}
						}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag switch_to_user}
						}
						{tag_remove user_ai}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag switch_to_user}
						}
						{tag_remove switch_to_user}
					}
					{"trigger"
						{name "ai switch/switch_to_user"}
					}
				}
}

{"ai switch/switch_to_ai"
				{condition
					{terms
						{"1.entities"
							{selector
								{ignore_captured_by_user 0}
								{state operatable}
								{tag switch_to_ai}
							}
						}
					}
				}
				{actions
					{"actor_state"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{state operatable}
							{group
								{include
									{tag
										{tag switch_to_ai}
									}
								}
							}
						}
						{drop senseless}
						{control AI}
						{fire_mode open}
						;{move_mode hold}
						{ai_move
							{mode enable}
						}
						{ai
							{no_advance off}
							{no_retreat off}
						}
					}

					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag switch_to_ai}
						}

						{tag_remove switch_to_ai}
					}
					{"trigger"
						{name "ai switch/switch_to_ai"}
					}
				}
}

{"altitude_keeper"
				{autoreset}
				{condition
					{terms
						{"1.entities"
							{selector
								{tag altitude_keeper}
								{state operatable}
							}
						}
					}
				}
				{actions
					{"air_state"
						{selector
							{tag altitude_keeper}
							{state operatable}
						}
						{altitude 35}
					}
					{"trigger"
						{name "altitude_keeper"}
					}
				}
			}

{"para_altitude_keeper"
				{autoreset}
				{condition
					{terms
						{"1.entities"
							{selector
								{tag para_altitude_keeper}
								{state operatable}
							}
						}
					}
				}
				{actions
					{"air_state"
						{selector
							{tag para_altitude_keeper}
							{state operatable}
						}
						{altitude 45}
					}
					{"trigger"
						{name "para_altitude_keeper"}
					}
				}
			}

{"fuel_reduction"
				{autoreset}
				{condition
					{terms
						{"2.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag fuel_tank_damaged}
							}
						}
					}
				}
				{actions
					{"select"
						{tag fuel_tank_damaged}
					}
					{"inventory"
						{selector
							{source current}
							{ignore_captured_by_user 0}
						}
						{action fuel}
						{amount -3}
					}

					{"delay"
						{time 0.1}
					}

					{"trigger"
						{name "fuel_reduction"}
					}
				}
			}

	(define "check_user_veh"
				{"entity_state"
					{selector
						{ignore_captured_by_user 0}
						{player "%0"}
						{type vehicle}
					}
					{tag_add player%0_veh}
				}
			)
			(define "capture_check"
				{"entity_state"

					{selector
						{source advanced}
						{ignore_captured_by_user 0}
						{group
							{include
								{tag
									{tag player%0_veh}
								}
							}
							{exclude

								{player
									{player "%0"}
								}
							}
						}

					}

					{tag_remove player%0_veh}

			     }
			)
			{"check_user_veh"
				{condition
					{terms
						{"1.entities"
							{selector
								{ignore_captured_by_user 0}
								{type vehicle}
							}
						}
					}
				}
				{actions
					("check_user_veh" args 1)
					("check_user_veh" args 2)
					("check_user_veh" args 3)
					("check_user_veh" args 4)
					("check_user_veh" args 5)
					("check_user_veh" args 6)
					("check_user_veh" args 7)
					("check_user_veh" args 8)

					("capture_check" args 1)
					("capture_check" args 2)
					("capture_check" args 3)
					("capture_check" args 4)
					("capture_check" args 5)
					("capture_check" args 6)
					("capture_check" args 7)
					("capture_check" args 8)

					{"trigger"
						{name "check_user_veh"}
					}

				}


			}


			{"[ordos] airborne paratrooper script for dcg"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{tag
											{tag airborne_ai_para}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{state
											{state "user_control inactive"}
										}

										{tag
											{tag ai_para_assigned}
										}
										{tag
											{tag ai_para_plane}
										}
									}
								}
							}
						}
						{"2.entities"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{prop
											{prop vehicle}
										}
										{state
											{state "user_control operatable"}
										}

									}
									{exclude
										{state
											{state inactive}
										}
										{prop
											{prop airborne}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"music"
						{start "/sound/mission/task_new.wav"}
						{volume narrator}
					}
					{"entity_state"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									{tag
										{tag airborne_ai_para}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{state
										{state "user_control inactive"}
									}
									{tag
										{tag ai_para_assigned}
									}
									{tag
										{tag ai_para_plane}
									}
								}
							}
							{amount 1
							}
						}
						{tag_add ai_para_plane}
					}

					{"entity_state"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									{prop
										{prop vehicle}
									}
									{state
										{state "user_control operatable"}
									}

								}
								{exclude
									{state
										{state inactive}
									}
									{prop
										{prop airborne}
									}
								}
							}
							{amount 1
							}
						}
						{tag_add paradrop_target}
					}

					{"delay"
						{time 0.1}
					}
					{"air_attack"
						{selector
							{ignore_captured_by_user 0}
							{tag ai_para_plane}
						}
						{drop orders}
						{enemies
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									{prop
										{prop vehicle}
									}
									{state
										{state "user_control operatable"}
									}

								}
								{exclude
									{state
										{state inactive}
									}
									{prop
										{prop airborne}
									}
								}
							}
							{amount 1
							}
							{sort
								{type shuffle}
							}
						}
						{attack_altitude 45}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag ai_para_plane}
						}
						{tag_add ai_para_assigned}
						{tag_remove ai_para_plane}
						{tag_remove airborne_ai_para}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "[ordos] airborne paratrooper script for dcg"}
					}
				}
			}
{"[ordos] paratrooper detector script for dcg"

				{condition
					{terms
						{"1.near"
							{units
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{tag
											{tag ai_para_assigned}
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
											{state inactive}
										}
										{tag
											{tag ai_para_assigned2}
										}

									}
								}
								{amount 1
								}
							}

							{near_to
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{tag
											{tag paradrop_target}
										}
									}
								}
							}
							{distance 180}
						}
					}
				}
				{actions
					{"effect"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{select
									{tag
										{tag ai_para_assigned}
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
									{tag
										{tag ai_para_assigned2}
									}
									{tag
										{tag codex_para_plane}
									}
									{tag
										{tag codex_drop_plane}
									}
								}
							}
							{amount 1
							}
						}
						{effect drop_paratrooper}
					}

					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag ai_para_assigned}
						}
						{tag_add ai_para_assigned2}
						{tag_remove ai_para_assigned}


					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag paradrop_target}
						}

						{tag_remove paradrop_target}

					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "[ordos] paratrooper detector script for dcg"}
					}

				}


			}




{"link_c130"
				{condition
					{terms
						{"1.near"
							{units
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{tag
											{tag c130_para_veh}
										}
										{state
											{state "operatable moveable chassis_work user_control"}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state linked}
										}
									}
								}
							}

							{near_to
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{prop
											{prop "c130"}
										}
										{state
											{state "operatable moveable chassis_work user_control"}
										}
									}
									{exclude
										{state
											{state dead}
										}
									}
								}
							}
							{distance 150}
						}
					}
				}
				{actions
					{"linker"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									{prop
										{prop "c130"}
									}
									{state
										{state "operatable moveable chassis_work user_control"}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
							{amount 1
							}
						}
						{entities
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									{tag
											{tag c130_para_veh}
									}
									{state
										{state "operatable moveable chassis_work user_control"}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state linked}
									}
								}
							}
							{amount 1
							}
						}

						{place
							{type specific}
							{name vehicle1}
						}
					}
					{"entity_state"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									{prop
										{prop "c130"}
									}
									{state
										{state "operatable moveable chassis_work user_control"}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{tag
										{tag place1_busy}
									}
								}
							}
							{amount 1
							}
						}
						{tag_add place1_busy}
					}
					{## "delay"
						{time 1}
					}
					{"effect"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{select
									{tag
										{tag place1_busy}
									}
								}
								{include
									{state
										{state "operatable moveable chassis_work user_control"}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
						}
						{effect check_vdvstate}
					}
					{"trigger"
						{name "link_c130"}
					}
				}
			}
{"metis_usage"
				{autoreset}
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{include
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "metis_hand"}
										}
									}
									{exclude
										{tag
											{tag setuping}
										}
										{state
											{state "dead inactive"}
										}
										{state
											{state linked}
										}
										{state
											{state user_control}
										}
										{tag
											{tag setuped}
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
							{ignore_captured_by_user 0}
							{group
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "metis_hand"}
									}
								}
								{exclude
									{tag
										{tag setuping}
									}
									{state
										{state "dead inactive"}
									}
									{state
										{state linked}
									}
									{state
										{state user_control}
									}
									{tag
										{tag setuped}
									}
								}
							}
							{amount 1
							}
						}
						{tag_add setuping}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag setuping}
						}
						{tag_add repairing}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag setuping}
						}
						{tag_add _mi_temp1}
					}
					{"delay"
						{time 0.2}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag _mi_temp1}
						}
						{tag_add _lua_mi}
					}								
					{"delay"
						{time 0.2}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{include
									{tag
										{tag setuping}
									}
								}
								{exclude
									{state
										{state "dead inactive"}
									}
									{state
										{state linked}
									}
								}
							}
						}
						{drop orders}
						{action portable_atgm_metis}
						{target
							{tag "[self]"}
						}
					}
					{"delay"
						{time 1}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag setuping}
						}
						{tag_add setuped}
						{tag_remove setuping}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag repairing}
						}
						{tag_remove repairing}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag _mi_temp1}
						}
						{tag_remove _mi_temp1}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag _lua_mi}
						}
						{tag_remove _lua_mi}
					}
					{"trigger"
						{name "metis_usage"}
					}
				}
			}

{"hawk_system_set"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										
										{state
											{state operatable}
										}
										{state
											{state user_control}
										}
										{tag
											{tag hawk_system}
										}
										
										{state "operatable moveable chassis_work "}
									}
									{exclude
										{state
											{state inactive}
										}
										{tag
											{tag hawk_setuped}
										}
									}
								}

							}
						}
					}
				}
				{actions
					{"actor_state"
						{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										
										{state
											{state operatable}
										}
										{state
											{state user_control}
										}
										{tag
											{tag hawk_system}
										}
										
										{state "operatable moveable chassis_work "}
									}
									{exclude
										{state
											{state inactive}
										}
										{tag
											{tag hawk_setuped}
										}
									}
								}

							}
						
						{control AI}
						{fire_mode open}
						{ai_move
							{mode disable}
						}
					}

					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag hawk_system}
						}

						{tag_add hawk_setuped}
					}
					{"trigger"
						{name "hawk_system_set"}
					}
				}
}