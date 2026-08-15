;// Opening 155/105 and pre-wave mortars. Lua sets arty_prep_open / arty_prep_wave.
;// Spawn MUST use {waypoint}, not {target} — target-select spawn is ignored/dropped.

			{"codex/arty_prep/opening"
				{condition
					{terms
						{"1.cmp_i"
							{var "arty_prep_open$"}
							{op ">"}
							{value 0}
						}
					}
				}
				{actions
					{"delay"
						{time 0.5}
					}
					{"talk"
						{selector
							{ignore_captured_by_user 0}
							{tag _user}
						}
						{text "mission/multi/player_enemy_division_notification_artillery_02"}
					}
					{"delay"
						{time 1.5}
					}
					{"talk"
						{selector
							{ignore_captured_by_user 0}
							{tag _user}
						}
						{text "mission/multi/super_heavy_artillery_incoming_2"}
					}
					{"delay"
						{time 4}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f1$"} {op "=="} {value 1}}
							{"spawn" {entity "codex_arty_150_8"} {waypoint "transport_patrol_flag_1"}}
						}
					}
					{"delay"
						{time 10}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f2$"} {op "=="} {value 1}}
							{"spawn" {entity "codex_arty_150_8"} {waypoint "transport_patrol_flag_2"}}
						}
					}
					{"delay"
						{time 10}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f3$"} {op "=="} {value 1}}
							{"spawn" {entity "codex_arty_150_8"} {waypoint "transport_patrol_flag_3"}}
						}
					}
					{"delay"
						{time 10}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f4$"} {op "=="} {value 1}}
							{"spawn" {entity "codex_arty_150_8"} {waypoint "transport_patrol_flag_4"}}
						}
					}
					{"delay"
						{time 10}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f5$"} {op "=="} {value 1}}
							{"spawn" {entity "codex_arty_150_8"} {waypoint "transport_patrol_flag_5"}}
						}
					}
					{"delay"
						{time 20}
					}
					{"set_i"
						{var "arty_prep_open$"}
						{op "="}
						{value 0}
					}
					{"set_i"
						{var "arty_smoke$"}
						{op "="}
						{value 1}
					}
				}
			}

			{"codex/arty_prep/smoke"
				{condition
					{terms
						{"1.cmp_i"
							{var "arty_smoke$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "user_is_defender$"}
							{op "=="}
							{value 1}
						}
					}
				}
				{actions
					{"set_i"
						{var "arty_smoke$"}
						{op "="}
						{value 0}
					}
					{"delay"
						{time 1}
					}
					{"talk"
						{selector
							{ignore_captured_by_user 0}
							{tag _user}
						}
						{text "mission/multi/calling_smoke_barrage"}
					}
					{"delay"
						{time 3}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
							{"spawn" {entity "codex_smoke_6"} {waypoint "attack_support_flank_a1"}}
							{"delay" {time 3}}
							{"spawn" {entity "codex_smoke_6"} {waypoint "attack_support_flank_a2"}}
							{"delay" {time 3}}
							{"spawn" {entity "codex_smoke_6"} {waypoint "attack_support_air_a1"}}
						}
						{"default"
							{"spawn" {entity "codex_smoke_6"} {waypoint "attack_support_flank_b1"}}
							{"delay" {time 3}}
							{"spawn" {entity "codex_smoke_6"} {waypoint "attack_support_flank_b2"}}
							{"delay" {time 3}}
							{"spawn" {entity "codex_smoke_6"} {waypoint "attack_support_air_b1"}}
						}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "codex/arty_prep/smoke"}
					}
				}
			}

			{"codex/arty_prep/wave_mortar"
				{condition
					{terms
						{"1.cmp_i"
							{var "arty_prep_wave$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "arty_prep_open$"}
							{op "=="}
							{value 0}
						}
					}
				}
				{actions
					{"set_i"
						{var "arty_prep_wave$"}
						{op "="}
						{value 0}
					}
					{"delay"
						{time 0.5}
					}
					{"talk"
						{selector
							{ignore_captured_by_user 0}
							{tag _user}
						}
						{text "mission/multi/player_enemy_division_notification_artillery_01"}
					}
					{"delay"
						{time 1.5}
					}
					{"talk"
						{selector
							{ignore_captured_by_user 0}
							{tag _user}
						}
						{text "mission/multi/calling_light_artillery_barrage"}
					}
					{"delay"
						{time 4}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_wave_slot$"} {op "=="} {value 2}}
							{"spawn" {entity "codex_arty_105_16"} {waypoint "transport_patrol_flag_2"}}
						}
						{"case"
							{condition {type cmp_i} {var "arty_wave_slot$"} {op "=="} {value 3}}
							{"spawn" {entity "codex_arty_105_16"} {waypoint "transport_patrol_flag_3"}}
						}
						{"case"
							{condition {type cmp_i} {var "arty_wave_slot$"} {op "=="} {value 4}}
							{"spawn" {entity "codex_arty_105_16"} {waypoint "transport_patrol_flag_4"}}
						}
						{"case"
							{condition {type cmp_i} {var "arty_wave_slot$"} {op "=="} {value 5}}
							{"spawn" {entity "codex_arty_105_16"} {waypoint "transport_patrol_flag_5"}}
						}
						{"default"
							{"spawn" {entity "codex_arty_105_16"} {waypoint "transport_patrol_flag_1"}}
						}
					}
					{"set_i"
						{var "arty_prep_wave$"}
						{op "="}
						{value 0}
					}
					{"trigger"
						{name "codex/arty_prep/wave_mortar"}
					}
				}
			}

			{"codex/arty_counter/f1"
				{condition
					{terms
						{"1.cmp_i"
							{var "user_is_defender$"}
							{op "=="}
							{value 0}
						}
						{"2.cmp_i"
							{var "prep_inform$"}
							{op "=="}
							{value 1}
						}
						{"3.cmp_i"
							{var "arty_counter_cd$"}
							{op "=="}
							{value 0}
						}
						{"4.near"
							{units
								{ignore_captured_by_user 0}
								{tag _user}
								{type human}
								{state "operatable user_control"}
							}
							{near_to
								{ignore_captured_by_user 0}
								{tag fpc1}
							}
							{distance 45}
						}
					}
				}
				{actions
					{"set_i"
						{var "arty_counter_cd$"}
						{op "="}
						{value 1}
					}
					{"talk"
						{selector
							{ignore_captured_by_user 0}
							{tag _user}
						}
						{text "mission/multi/calling_light_artillery_barrage"}
					}
					{"delay"
						{time 4}
					}
					{"spawn"
						{entity "codex_arty_105_16"}
						{waypoint "transport_patrol_flag_1"}
					}
					{"delay"
						{time 90}
					}
					{"set_i"
						{var "arty_counter_cd$"}
						{op "="}
						{value 0}
					}
					{"trigger"
						{name "codex/arty_counter/f1"}
					}
				}
			}

			{"codex/arty_counter/f2"
				{condition
					{terms
						{"1.cmp_i"
							{var "user_is_defender$"}
							{op "=="}
							{value 0}
						}
						{"2.cmp_i"
							{var "prep_inform$"}
							{op "=="}
							{value 1}
						}
						{"3.cmp_i"
							{var "arty_counter_cd$"}
							{op "=="}
							{value 0}
						}
						{"4.near"
							{units
								{ignore_captured_by_user 0}
								{tag _user}
								{type human}
								{state "operatable user_control"}
							}
							{near_to
								{ignore_captured_by_user 0}
								{tag fpc2}
							}
							{distance 45}
						}
					}
				}
				{actions
					{"set_i"
						{var "arty_counter_cd$"}
						{op "="}
						{value 1}
					}
					{"talk"
						{selector
							{ignore_captured_by_user 0}
							{tag _user}
						}
						{text "mission/multi/calling_light_artillery_barrage"}
					}
					{"delay"
						{time 4}
					}
					{"spawn"
						{entity "codex_arty_105_16"}
						{waypoint "transport_patrol_flag_2"}
					}
					{"delay"
						{time 90}
					}
					{"set_i"
						{var "arty_counter_cd$"}
						{op "="}
						{value 0}
					}
					{"trigger"
						{name "codex/arty_counter/f2"}
					}
				}
			}

			{"codex/arty_counter/f3"
				{condition
					{terms
						{"1.cmp_i"
							{var "user_is_defender$"}
							{op "=="}
							{value 0}
						}
						{"2.cmp_i"
							{var "prep_inform$"}
							{op "=="}
							{value 1}
						}
						{"3.cmp_i"
							{var "arty_counter_cd$"}
							{op "=="}
							{value 0}
						}
						{"4.near"
							{units
								{ignore_captured_by_user 0}
								{tag _user}
								{type human}
								{state "operatable user_control"}
							}
							{near_to
								{ignore_captured_by_user 0}
								{tag fpc3}
							}
							{distance 45}
						}
					}
				}
				{actions
					{"set_i"
						{var "arty_counter_cd$"}
						{op "="}
						{value 1}
					}
					{"talk"
						{selector
							{ignore_captured_by_user 0}
							{tag _user}
						}
						{text "mission/multi/calling_light_artillery_barrage"}
					}
					{"delay"
						{time 4}
					}
					{"spawn"
						{entity "codex_arty_105_16"}
						{waypoint "transport_patrol_flag_3"}
					}
					{"delay"
						{time 90}
					}
					{"set_i"
						{var "arty_counter_cd$"}
						{op "="}
						{value 0}
					}
					{"trigger"
						{name "codex/arty_counter/f3"}
					}
				}
			}
