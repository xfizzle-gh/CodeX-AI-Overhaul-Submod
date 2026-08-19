			{"codex/para/drop"
				{condition
					{terms
						{"1.cmp_i"
							{var "codex_para$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{tag codex_drop_plane}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "codex_para$"}
						{op "="}
						{value 0}
					}
					{"delay" {time 2}}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag codex_drop_plane}}}
								{exclude {tag {tag codex_para_plane}}}
							}
							{amount 1}
						}
						{tag_add codex_para_plane}
						{tag_add ai_para_assigned2}
						{tag_remove plane_test_to_exit}
						{tag_remove wait_para_player_set}
						{tag_remove para_altitude_keeper}
						{tag_remove dont_drop}
					}
					{"air_state"
						{selector {tag codex_para_plane}}
						{altitude 80}
					}
					{"actor_state"
						{selector {tag codex_para_plane}}
						{drop orders}
						{control AI}
						{ai_move {mode enable}}
						{movement {speed fast}}
					}
					{"switch"
						{"case" {condition {type cmp_i} {var "codex_para_lz$"} {op "=="} {value 1}}
							{"action" {selector {tag codex_para_plane}} {drop orders} {action move} {waypoint "transport_patrol_flag_1"}}}
						{"case" {condition {type cmp_i} {var "codex_para_lz$"} {op "=="} {value 2}}
							{"action" {selector {tag codex_para_plane}} {drop orders} {action move} {waypoint "transport_patrol_flag_2"}}}
						{"case" {condition {type cmp_i} {var "codex_para_lz$"} {op "=="} {value 3}}
							{"action" {selector {tag codex_para_plane}} {drop orders} {action move} {waypoint "transport_patrol_flag_3"}}}
						{"case" {condition {type cmp_i} {var "codex_para_lz$"} {op "=="} {value 4}}
							{"action" {selector {tag codex_para_plane}} {drop orders} {action move} {waypoint "transport_patrol_flag_4"}}}
						{"case" {condition {type cmp_i} {var "codex_para_lz$"} {op "=="} {value 5}}
							{"action" {selector {tag codex_para_plane}} {drop orders} {action move} {waypoint "transport_patrol_flag_5"}}}
						{"case" {condition {type cmp_i} {var "codex_para_lz$"} {op "=="} {value 7}}
							{"action" {selector {tag codex_para_plane}} {drop orders} {action move} {waypoint "attack_support_flank_a2"}}}
						{"case" {condition {type cmp_i} {var "codex_para_lz$"} {op "=="} {value 8}}
							{"action" {selector {tag codex_para_plane}} {drop orders} {action move} {waypoint "attack_support_flank_b1"}}}
						{"case" {condition {type cmp_i} {var "codex_para_lz$"} {op "=="} {value 9}}
							{"action" {selector {tag codex_para_plane}} {drop orders} {action move} {waypoint "attack_support_flank_b2"}}}
						{"default"
							{"action" {selector {tag codex_para_plane}} {drop orders} {action move} {waypoint "attack_support_flank_a1"}}}
					}
					{"delay" {time 12}}
					{"effect"
						{selector {tag codex_para_plane}}
						{effect drop_paratroopers}
					}
					{"delay" {time 0.3}}
					{"effect"
						{selector {tag codex_para_plane}}
						{effect codex_force_drop}
					}
					{"delay" {time 12}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
							{"action" {selector {tag codex_para_plane}} {drop orders} {action move} {waypoint "attack_support_entry_b"}}
						}
						{"default"
							{"action" {selector {tag codex_para_plane}} {drop orders} {action move} {waypoint "attack_support_entry_a"}}
						}
					}
					{"delay" {time 22}}
					{"delete"
						{selector {tag codex_para_plane}}
					}
					{"entity_state"
						{selector {tag codex_para_plane}}
						{tag_remove codex_para_plane}
					}
					{"trigger"
						{name "codex/para/drop"}
					}
				}
			}
