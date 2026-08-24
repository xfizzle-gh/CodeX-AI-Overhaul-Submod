;// Opening/wave caliber from Lua arty_intensity: 1 mortar120, 2 mortar160, 3 105, 4 150.
;// Spawn MUST use {waypoint}, not {target} — target-select spawn is ignored/dropped.
(define "codex_arty_mark_live"
	{"switch"
		{"case"
			{condition
				{type entities}
				{selector
					{source advanced}
					{group
						{select {name %n}}
						{exclude {state {state inactive}}}
					}
				}
			}
			{"set_i" {var "arty_f%i$"} {op "="} {value 1}}
		}
		{"default"
			{"set_i" {var "arty_f%i$"} {op "="} {value 0}}
		}
	}
)
(define "codex_arty_refresh_live_flags"
	("codex_arty_mark_live" n(f1) i(1))
	("codex_arty_mark_live" n(f2) i(2))
	("codex_arty_mark_live" n(f3) i(3))
	("codex_arty_mark_live" n(f4) i(4))
	("codex_arty_mark_live" n(f5) i(5))
)
(define "codex_arty_by_intensity"
	{"switch"
		{"case"
			{condition {type cmp_i} {var "arty_intensity$"} {op "<="} {value 1}}
			{"spawn" {entity "mortar_barrage_120"} {waypoint "%wp"}}
		}
		{"case"
			{condition {type cmp_i} {var "arty_intensity$"} {op "=="} {value 2}}
			{"spawn" {entity "mortar_barrage_160"} {waypoint "%wp"}}
		}
		{"case"
			{condition {type cmp_i} {var "arty_intensity$"} {op "=="} {value 3}}
			{"spawn" {entity "codex_arty_105_16"} {waypoint "%wp"}}
		}
		{"case"
			{condition {type cmp_i} {var "arty_intensity$"} {op "=="} {value 4}}
			{"spawn" {entity "codex_arty_150_8"} {waypoint "%wp"}}
		}
		{"default"
			{"spawn" {entity "mortar_barrage_120"} {waypoint "%wp"}}
		}
	}
	("codex_arty_own_to_enemy")
)

(define "codex_arty_own_to_enemy"
	{"switch"
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 1}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "1"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 2}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "2"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 3}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "3"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 4}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "4"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 5}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "5"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 6}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "6"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 7}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "7"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 8}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "8"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 9}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "9"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 10}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "10"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 11}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "11"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 12}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "12"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 13}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "13"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 14}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "14"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 15}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "15"}}}
		{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 16}} {"player" {selector {source advanced} {ignore_captured_by_user 0} {group {select {tag {tag codex_arty_need_owner}}}}} {operation set} {player "16"}}}
		{"default"}
	}
	{"entity_state"
		{selector
			{source advanced}
			{group
				{select
					{tag
						{tag codex_arty_need_owner}
					}
				}
			}
		}
		{tag_add codex_arty_owned}
	}
	{"entity_state"
		{selector
			{source advanced}
			{group
				{select
					{tag
						{tag codex_arty_need_owner}
					}
				}
			}
		}
		{tag_remove codex_arty_need_owner}
	}
)

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
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_intensity$"} {op "<="} {value 2}}
							{"talk"
								{selector
									{ignore_captured_by_user 0}
									{tag _user}
								}
								{text "mission/multi/calling_light_artillery_barrage"}
							}
						}
						{"default"
							{"talk"
								{selector
									{ignore_captured_by_user 0}
									{tag _user}
								}
								{text "mission/multi/super_heavy_artillery_incoming_2"}
							}
						}
					}
					{"delay"
						{time 4}
					}
					("codex_arty_refresh_live_flags")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f1$"} {op "=="} {value 1}}
							("codex_arty_by_intensity" wp(transport_patrol_flag_1))
						}
					}
					{"delay"
						{time 10}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f2$"} {op "=="} {value 1}}
							("codex_arty_by_intensity" wp(transport_patrol_flag_2))
						}
					}
					{"delay"
						{time 10}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f3$"} {op "=="} {value 1}}
							("codex_arty_by_intensity" wp(transport_patrol_flag_3))
						}
					}
					{"delay"
						{time 10}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f4$"} {op "=="} {value 1}}
							("codex_arty_by_intensity" wp(transport_patrol_flag_4))
						}
					}
					{"delay"
						{time 10}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f5$"} {op "=="} {value 1}}
							("codex_arty_by_intensity" wp(transport_patrol_flag_5))
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
						{time 6}
					}
					("codex_arty_refresh_live_flags")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f1$"} {op "=="} {value 1}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
									{"spawn" {entity "codex_smoke_6"} {waypoint "transport_patrol_flag_1"}}
								}
								{"default"
									{"spawn" {entity "codex_smoke_6b"} {waypoint "transport_patrol_flag_1"}}
								}
							}
							("codex_arty_own_to_enemy")
						}
					}
					{"delay" {time 3}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f2$"} {op "=="} {value 1}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
									{"spawn" {entity "codex_smoke_6"} {waypoint "transport_patrol_flag_2"}}
								}
								{"default"
									{"spawn" {entity "codex_smoke_6b"} {waypoint "transport_patrol_flag_2"}}
								}
							}
							("codex_arty_own_to_enemy")
						}
					}
					{"delay" {time 3}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f3$"} {op "=="} {value 1}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
									{"spawn" {entity "codex_smoke_6"} {waypoint "transport_patrol_flag_3"}}
								}
								{"default"
									{"spawn" {entity "codex_smoke_6b"} {waypoint "transport_patrol_flag_3"}}
								}
							}
							("codex_arty_own_to_enemy")
						}
					}
					{"delay" {time 3}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f4$"} {op "=="} {value 1}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
									{"spawn" {entity "codex_smoke_6"} {waypoint "transport_patrol_flag_4"}}
								}
								{"default"
									{"spawn" {entity "codex_smoke_6b"} {waypoint "transport_patrol_flag_4"}}
								}
							}
							("codex_arty_own_to_enemy")
						}
					}
					{"delay" {time 3}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "arty_f5$"} {op "=="} {value 1}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
									{"spawn" {entity "codex_smoke_6"} {waypoint "transport_patrol_flag_5"}}
								}
								{"default"
									{"spawn" {entity "codex_smoke_6b"} {waypoint "transport_patrol_flag_5"}}
								}
							}
							("codex_arty_own_to_enemy")
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

			{"codex/arty_prep/own"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag codex_arty_need_owner}
										}
									}
								}
							}
						}
					}
				}
				{actions
					("codex_arty_own_to_enemy")
					{"delay"
						{time 0.05}
					}
					{"trigger"
						{name "codex/arty_prep/own"}
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
					("codex_arty_refresh_live_flags")
					{"switch"
						{"case"
							{condition {type cmp_i}
								{var "arty_wave_slot$"} {op "=="} {value 2}
							}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "arty_f2$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_2"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f1$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_1"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f3$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_3"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f4$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_4"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f5$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_5"}}
								}
							}
						}
						{"case"
							{condition {type cmp_i}
								{var "arty_wave_slot$"} {op "=="} {value 3}
							}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "arty_f3$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_3"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f1$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_1"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f2$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_2"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f4$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_4"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f5$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_5"}}
								}
							}
						}
						{"case"
							{condition {type cmp_i}
								{var "arty_wave_slot$"} {op "=="} {value 4}
							}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "arty_f4$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_4"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f1$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_1"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f2$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_2"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f3$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_3"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f5$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_5"}}
								}
							}
						}
						{"case"
							{condition {type cmp_i}
								{var "arty_wave_slot$"} {op "=="} {value 5}
							}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "arty_f5$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_5"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f1$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_1"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f2$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_2"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f3$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_3"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f4$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_4"}}
								}
							}
						}
						{"default"
							{"switch"
								{"case"
									{condition {type cmp_i} {var "arty_f1$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_1"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f2$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_2"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f3$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_3"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f4$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_4"}}
								}
								{"case"
									{condition {type cmp_i} {var "arty_f5$"} {op "=="} {value 1}}
									{"spawn" {entity "mortar_barrage_120"} {waypoint "transport_patrol_flag_5"}}
								}
							}
						}
					}
					("codex_arty_own_to_enemy")
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
						{"4.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag _user}
										}
									}
									{include
										{zone
											{zone "fpc1"}
										}
										{state
											{state "operatable user_control"}
										}
									}
									{exclude
										{state
											{state dead}
										}
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
					("codex_arty_by_intensity" wp(transport_patrol_flag_1))
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
						{"4.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag _user}
										}
									}
									{include
										{zone
											{zone "fpc2"}
										}
										{state
											{state "operatable user_control"}
										}
									}
									{exclude
										{state
											{state dead}
										}
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
					("codex_arty_by_intensity" wp(transport_patrol_flag_2))
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
						{"4.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag _user}
										}
									}
									{include
										{zone
											{zone "fpc3"}
										}
										{state
											{state "operatable user_control"}
										}
									}
									{exclude
										{state
											{state dead}
										}
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
					("codex_arty_by_intensity" wp(transport_patrol_flag_3))
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

			{"codex/arty_counter/f4"
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
						{"4.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag _user}
										}
									}
									{include
										{zone
											{zone "fpc4"}
										}
										{state
											{state "operatable user_control"}
										}
									}
									{exclude
										{state
											{state dead}
										}
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
					("codex_arty_by_intensity" wp(transport_patrol_flag_4))
					{"delay"
						{time 90}
					}
					{"set_i"
						{var "arty_counter_cd$"}
						{op "="}
						{value 0}
					}
					{"trigger"
						{name "codex/arty_counter/f4"}
					}
				}
			}

			{"codex/arty_counter/f5"
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
						{"4.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag _user}
										}
									}
									{include
										{zone
											{zone "fpc5"}
										}
										{state
											{state "operatable user_control"}
										}
									}
									{exclude
										{state
											{state dead}
										}
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
					("codex_arty_by_intensity" wp(transport_patrol_flag_5))
					{"delay"
						{time 90}
					}
					{"set_i"
						{var "arty_counter_cd$"}
						{op "="}
						{value 0}
					}
					{"trigger"
						{name "codex/arty_counter/f5"}
					}
				}
			}
