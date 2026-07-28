; CWA Dynamic Conquest ownership proof for the engine-created DefenderBot.
; This is intentionally one-shot and uses a 60-second post-preparation delay.
; Do not generalize into recurring support waves until ownership is proven live.

			{"allied_support/probe_cwa_ownership"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i"
							{var "user_is_defender$"}
							{op "=="}
							{value 1}
						}
						{"2.cmp_i"
							{var "id_defenderbot$"}
							{op ">"}
							{value 0}
						}
						{"3.cmp_i"
							{var "prep_inform$"}
							{op "=="}
							{value 1}
						}
					}
				}
				{actions
					{"delay"
						{time 60}
					}
					; Mark exactly five hidden cmp_def infantry templates. The clone inherits this
					; temporary marker; it is removed from the off-map sources immediately after cloning.
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag cmp_def}
									}
								}
								{include
									{prop
										{prop human}
									}
									{tag
										{tag hidden}
									}
								}
								{exclude
									{zone
										{zone "gamezone"}
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
									{state
										{state user_control}
									}
								}
							}
							{amount 5}
						}
						{tag_add allied_support_probe_source}
					}
					{"delay" {time 0.1}}
					{"placement"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_support_probe_source}
									}
								}
							}
							{amount 5}
						}
						{target_waypoint "allied_support_entry"}
						{clone}
					}
					{"delay" {time 0.5}}
					; Remove the temporary marker from hidden source templates only.
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_support_probe_source}
									}
								}
								{include
									{tag
										{tag hidden}
									}
								}
								{exclude
									{zone
										{zone "gamezone"}
									}
								}
							}
						}
						{tag_remove allied_support_probe_source}
					}
					; The in-game clones retain the inherited marker. Promote only those clones.
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_support_probe_source}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
								}
							}
						}
						{tag_add allied_wave_fresh}
						{tag_add allied_support_probe}
						{tag_add _def}
						{tag_add _ai_defender}
						{tag_remove hidden}
						{tag_remove allied_support_probe_source}
					}
					{"switch"
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 1}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "1"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_1}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 2}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "2"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_2}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 3}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "3"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_3}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 4}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "4"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_4}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 5}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "5"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_5}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 6}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "6"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_6}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 7}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "7"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_7}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 8}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "8"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_8}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 9}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "9"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_9}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 10}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "10"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_10}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 11}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "11"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_11}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 12}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "12"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_12}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 13}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "13"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_13}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 14}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "14"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_14}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 15}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "15"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_15}
					}
				}
				{"case"
					{condition
						{type cmp_i}
						{var "id_defenderbot$"}
						{op "=="}
						{value 16}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{operation set}
						{player "16"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_add allied_support_probe_owner_16}
					}
				}
						{"default"
							{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
								{tag_add allied_support_probe_owner_unsupported}
							}
						}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{control AI}
						{ai
							{no_retreat on}
						}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{drop orders}
						{action advance}
						{target
							{tag fpc1}
						}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_wave_fresh}
									}
								}
								{include
									{prop
										{prop human}
									}
									{state
										{state operatable}
									}
									{zone
										{zone "gamezone"}
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
										{tag hidden}
									}
								}
							}
						}
						{tag_remove allied_wave_fresh}
					}
				}
			}
