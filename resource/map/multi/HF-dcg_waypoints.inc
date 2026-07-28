			{"7"
				{position 6500 6500 0}
				{commands
					{"delay"
						{time 1}
					}
					{"ables"
						{drop orders}
						{add emit_passangers_on_enemy}
					}
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{fire_mode open}
						{move_mode free}
						{ai_move
							{mode enable}
						}
						{movement
							{speed assault}
							{speed_kph 12}
						}
						{ai
							{advance_ratio "0.1"}
							{retreat_ratio 3}
						}
					}
					{"delay"
						{time 0.1}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{tag_add bot_flank_finished}
						{tag_remove bot_flanking}
					}
					{"delay"
						{time 0.1}
					}
				}
			}
			{"8"
				{position -6500 -6500 0}
				{commands
					{"delay"
						{time 1}
					}
					{"ables"
						{drop orders}
						{add emit_passangers_on_enemy}
					}
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{fire_mode open}
						{move_mode free}
						{ai_move
							{mode enable}
						}
						{movement
							{speed assault}
							{speed_kph 12}
						}
						{ai
							{advance_ratio "0.1"}
							{retreat_ratio 3}
						}
					}
					{"delay"
						{time 0.1}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{tag_add bot_flank_finished}
						{tag_remove bot_flanking}
					}
					{"delay"
						{time 0.1}
					}
				}
			}