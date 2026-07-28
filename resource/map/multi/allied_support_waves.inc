; Allied support waves for human-defense Dynamic Conquest (CWA).
; Indomitus-style FSM: init -> chance -> decide (target FPC / near-cap) -> spawn.
; Clones permanent allied_support_template pool at waypoint allied_support_entry,
; assigns DefenderBot, {control AI}, advances to neediest fpc.
;
; Budget by defense_level$ (CE): L1 2x4, L2 3x5, L3 4x6. Level 0 defaults to L1.
; TEST DELAY: chance uses 60s. Restore PRODUCTION 4-8 min block below when proven.

			{"allied_support/init"
				{condition
					{expression "1 & 2 & 3 & 4"}
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
						{"4.cmp_i"
							{var "allied_support_initialized$"}
							{op "=="}
							{value 0}
						}
					}
				}
				{actions
					{"delay" {time 1}}
					{"switch"
						{"case"
							{condition
								{type cmp_i}
								{var "defense_level$"}
								{op "=="}
								{value 3}
							}
							{"set_i" {var "allied_support_waves_left$"} {op "="} {value 4}}
							{"set_i" {var "allied_support_wave_size$"} {op "="} {value 6}}
							{"timer"
								{time 8}
								{title "allied_support init L3 waves=4 size=6"}
							}
						}
						{"case"
							{condition
								{type cmp_i}
								{var "defense_level$"}
								{op "=="}
								{value 2}
							}
							{"set_i" {var "allied_support_waves_left$"} {op "="} {value 3}}
							{"set_i" {var "allied_support_wave_size$"} {op "="} {value 5}}
							{"timer"
								{time 8}
								{title "allied_support init L2 waves=3 size=5"}
							}
						}
						{"default"
							{"set_i" {var "allied_support_waves_left$"} {op "="} {value 2}}
							{"set_i" {var "allied_support_wave_size$"} {op "="} {value 4}}
							{"timer"
								{time 8}
								{title "allied_support init L1 waves=2 size=4"}
							}
						}
					}
					{"set_i" {var "allied_support_target$"} {op "="} {value 1}}
					{"set_i" {var "allied_support_initialized$"} {op "="} {value 1}}
					; Ensure off-map clone pool exists (CWA maps often have no hidden cmp_def reserve)
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag allied_support_template}
											}
										}
										{include
											{prop
												{prop human}
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
												{state linked}
											}
											{state
												{state user_control}
											}
										}
									}
								}
								{count
									{op ">="}
									{value 4}
								}
							}
							{"timer"
								{time 2}
								{title "allied_support pool ok"}
							}
						}
						{"default"
							{"timer"
								{time 3}
								{title "allied_support pool bootstrap"}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag cmp_def}
											}
											{tag
												{tag _def}
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
												{tag _user}
											}
											{tag
												{tag player}
											}
											{tag
												{tag allied_support}
											}
										}
									}
									{amount 8}
								}
								{tag_add allied_support_boot}
							}
							{"delay" {time 0.1}}
							{"placement"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag allied_support_boot}
											}
										}
									}
									{amount 8}
								}
								{target_waypoint "allied_support_entry"}
								{clone}
							}
							{"delay" {time 0.5}}
							; Live sources stay on flags — strip boot tag from anyone still in FPC zones
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag allied_support_boot}
											}
										}
										{include
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
										}
									}
									{group
										{select
											{zone
												{zone "fpc1"}
											}
											{zone
												{zone "fpc2"}
											}
											{zone
												{zone "fpc3"}
											}
											{zone
												{zone "fpc4"}
											}
											{zone
												{zone "fpc5"}
											}
										}
										{include
											{tag
												{tag allied_support_boot}
											}
										}
									}
								}
								{tag_remove allied_support_boot}
							}
							{"delay" {time 0.1}}
							; Remaining boot units (clones at entry, not on flags) = permanent pool
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag allied_support_boot}
											}
										}
										{include
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{zone
												{zone "fpc1"}
											}
											{zone
												{zone "fpc2"}
											}
											{zone
												{zone "fpc3"}
											}
											{zone
												{zone "fpc4"}
											}
											{zone
												{zone "fpc5"}
											}
										}
									}
								}
								{tag_add allied_support_template}
								{tag_add cmp_def}
								{tag_add not_delete}
								{tag_add hidden}
								{tag_remove allied_support_boot}
							}
							; Safety: never leave boot on live troops
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag allied_support_boot}
											}
										}
									}
								}
								{tag_remove allied_support_boot}
							}
						}
					}
					{"timer"
						{time 3}
						{title "allied_support init done"}
					}
					{"delay" {time 0.1}}
					{"trigger" {name "allied_support/chance"}}
				}
			}

			{"allied_support/chance"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i"
							{var "allied_support_initialized$"}
							{op "=="}
							{value 1}
						}
						{"2.cmp_i"
							{var "allied_support_waves_left$"}
							{op ">"}
							{value 0}
						}
						{"3.cmp_i"
							{var "allied_support_busy$"}
							{op "=="}
							{value 0}
						}
					}
				}
				{actions
					{"set_i" {var "allied_support_busy$"} {op "="} {value 1}}
					{"timer"
						{time 2}
						{title "allied_support chance start (TEST 60s)"}
					}
					; TEST delay 60s — restore PRODUCTION 4-8 min block below after log proof
					{"delay" {time 60}}
					; PRODUCTION inter-wave delay (Indomitus-style weighted switch):
					; {"switch"
					; 	{"case"
					; 		{condition {type rand} {value 0.2}}
					; 		{"delay" {time 240}}
					; 	}
					; 	{"case"
					; 		{condition {type rand} {value 0.25}}
					; 		{"delay" {time 300}}
					; 	}
					; 	{"case"
					; 		{condition {type rand} {value 0.33}}
					; 		{"delay" {time 360}}
					; 	}
					; 	{"case"
					; 		{condition {type rand} {value 0.5}}
					; 		{"delay" {time 420}}
					; 	}
					; 	{"default"
					; 		{"delay" {time 480}}
					; 	}
					; }
					{"set_i" {var "allied_support_busy$"} {op "="} {value 0}}
					{"delay" {time 0.1}}
					{"trigger" {name "allied_support/decide"}}
				}
			}

			{"allied_support/decide"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i"
							{var "user_is_defender$"}
							{op "=="}
							{value 1}
						}
						{"2.cmp_i"
							{var "allied_support_waves_left$"}
							{op ">"}
							{value 0}
						}
						{"3.cmp_i"
							{var "allied_support_busy$"}
							{op "=="}
							{value 0}
						}
					}
				}
				{actions
					{"set_i" {var "allied_support_busy$"} {op "="} {value 1}}
					{"delay" {time 0.1}}
					; Near-cap: too many live AI defenders already — wait 60s, do not consume a wave
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag _ai_defender}
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
											{tag
												{tag hidden}
											}
										}
									}
								}
								{count
									{op ">="}
									{value 16}
								}
							}
							{"timer"
								{time 2}
								{title "allied_support near-cap defer"}
							}
							{"delay" {time 60}}
							{"set_i" {var "allied_support_busy$"} {op "="} {value 0}}
							{"trigger" {name "allied_support/chance"}}
						}
						{"default"
							; Pick neediest existing FPC (bot pressure, thin defenders). Default fpc1.
							{"set_i" {var "allied_support_target$"} {op "="} {value 1}}
							{"delay" {time 0.1}}
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{tag fpc5}
											{zone "gamezone"}
										}
									}
									{"switch"
										{"case"
											{condition
												{type entities}
												{selector
													{ignore_captured_by_user 0}
													{tag bot}
													{zone "fpc5"}
													{type human}
													{state "not dead"}
												}
												{count {op ">"} {value 0}}
											}
											{"set_i" {var "allied_support_target$"} {op "="} {value 5}}
										}
										{"default"}
									}
								}
								{"default"}
							}
							{"delay" {time 0.1}}
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{tag fpc4}
											{zone "gamezone"}
										}
									}
									{"switch"
										{"case"
											{condition
												{type entities}
												{selector
													{ignore_captured_by_user 0}
													{tag bot}
													{zone "fpc4"}
													{type human}
													{state "not dead"}
												}
												{count {op ">"} {value 0}}
											}
											{"set_i" {var "allied_support_target$"} {op "="} {value 4}}
										}
										{"default"}
									}
								}
								{"default"}
							}
							{"delay" {time 0.1}}
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{tag fpc3}
											{zone "gamezone"}
										}
									}
									{"switch"
										{"case"
											{condition
												{type entities}
												{selector
													{ignore_captured_by_user 0}
													{tag bot}
													{zone "fpc3"}
													{type human}
													{state "not dead"}
												}
												{count {op ">"} {value 0}}
											}
											{"set_i" {var "allied_support_target$"} {op "="} {value 3}}
										}
										{"default"}
									}
								}
								{"default"}
							}
							{"delay" {time 0.1}}
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{tag fpc2}
											{zone "gamezone"}
										}
									}
									{"switch"
										{"case"
											{condition
												{type entities}
												{selector
													{ignore_captured_by_user 0}
													{tag bot}
													{zone "fpc2"}
													{type human}
													{state "not dead"}
												}
												{count {op ">"} {value 0}}
											}
											{"set_i" {var "allied_support_target$"} {op "="} {value 2}}
										}
										{"default"}
									}
								}
								{"default"}
							}
							{"delay" {time 0.1}}
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{tag fpc1}
											{zone "gamezone"}
										}
									}
									{"switch"
										{"case"
											{condition
												{type entities}
												{selector
													{ignore_captured_by_user 0}
													{tag bot}
													{zone "fpc1"}
													{type human}
													{state "not dead"}
												}
												{count {op ">"} {value 0}}
											}
											{"set_i" {var "allied_support_target$"} {op "="} {value 1}}
										}
										{"default"}
									}
								}
								{"default"}
							}
							{"delay" {time 0.1}}
							{"set_i" {var "allied_support_busy$"} {op "="} {value 0}}
							{"trigger" {name "allied_support/spawn"}}
						}
					}
				}
			}

			{"allied_support/spawn"
				{condition
					{expression "1 & 2 & 3 & 4"}
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
							{var "allied_support_waves_left$"}
							{op ">"}
							{value 0}
						}
						{"4.cmp_i"
							{var "allied_support_busy$"}
							{op "=="}
							{value 0}
						}
					}
				}
				{actions
					{"set_i" {var "allied_support_busy$"} {op "="} {value 1}}
					{"delay" {time 0.1}}
					; Prefer permanent template pool; fall back to cloning live flag defenders.
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag allied_support_template}
											}
										}
										{include
											{prop
												{prop human}
											}
										}
										{exclude
											{state
												{state dead}
											}
											{tag
												{tag allied_support}
											}
										}
									}
								}
								{count
									{op ">="}
									{value 1}
								}
							}
							{"switch"
								{"case"
									{condition
										{type cmp_i}
										{var "allied_support_wave_size$"}
										{op "=="}
										{value 5}
									}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag allied_support_template}
													}
												}
												{include
													{prop
														{prop human}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{tag
														{tag allied_support}
													}
												}
											}
											{amount 5}
										}
										{tag_add allied_support_src}
									}
								}
								{"case"
									{condition
										{type cmp_i}
										{var "allied_support_wave_size$"}
										{op "=="}
										{value 6}
									}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag allied_support_template}
													}
												}
												{include
													{prop
														{prop human}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{tag
														{tag allied_support}
													}
												}
											}
											{amount 6}
										}
										{tag_add allied_support_src}
									}
								}
								{"default"
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag allied_support_template}
													}
												}
												{include
													{prop
														{prop human}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{tag
														{tag allied_support}
													}
												}
											}
											{amount 4}
										}
										{tag_add allied_support_src}
									}
								}
							}
						}
						{"default"
							{"timer"
								{time 2}
								{title "allied_support live-clone fallback"}
							}
							{"switch"
								{"case"
									{condition
										{type cmp_i}
										{var "allied_support_wave_size$"}
										{op "=="}
										{value 5}
									}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag cmp_def}
													}
													{tag
														{tag _def}
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
														{tag _user}
													}
													{tag
														{tag player}
													}
													{tag
														{tag allied_support}
													}
													{tag
														{tag allied_support_template}
													}
													{tag
														{tag hidden}
													}
												}
											}
											{amount 5}
										}
										{tag_add allied_support_src}
									}
								}
								{"case"
									{condition
										{type cmp_i}
										{var "allied_support_wave_size$"}
										{op "=="}
										{value 6}
									}
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag cmp_def}
													}
													{tag
														{tag _def}
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
														{tag _user}
													}
													{tag
														{tag player}
													}
													{tag
														{tag allied_support}
													}
													{tag
														{tag allied_support_template}
													}
													{tag
														{tag hidden}
													}
												}
											}
											{amount 6}
										}
										{tag_add allied_support_src}
									}
								}
								{"default"
									{"entity_state"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag cmp_def}
													}
													{tag
														{tag _def}
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
														{tag _user}
													}
													{tag
														{tag player}
													}
													{tag
														{tag allied_support}
													}
													{tag
														{tag allied_support_template}
													}
													{tag
														{tag hidden}
													}
												}
											}
											{amount 4}
										}
										{tag_add allied_support_src}
									}
								}
							}
						}
					}
					{"delay" {time 0.1}}
					{"switch"
						{"case"
							{condition
								{type cmp_i}
								{var "allied_support_wave_size$"}
								{op "=="}
								{value 5}
							}
							{"placement"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag allied_support_src}
											}
										}
									}
									{amount 5}
								}
								{target_waypoint "allied_support_entry"}
								{clone}
							}
						}
						{"case"
							{condition
								{type cmp_i}
								{var "allied_support_wave_size$"}
								{op "=="}
								{value 6}
							}
							{"placement"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag allied_support_src}
											}
										}
									}
									{amount 6}
								}
								{target_waypoint "allied_support_entry"}
								{clone}
							}
						}
						{"default"
							{"placement"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag allied_support_src}
											}
										}
									}
									{amount 4}
								}
								{target_waypoint "allied_support_entry"}
								{clone}
							}
						}
					}
					{"delay" {time 0.5}}
					; Drop src from flag-zone originals so only entry clones keep it
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_support_src}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{zone
										{zone "fpc1"}
									}
									{zone
										{zone "fpc2"}
									}
									{zone
										{zone "fpc3"}
									}
									{zone
										{zone "fpc4"}
									}
									{zone
										{zone "fpc5"}
									}
								}
								{include
									{tag
										{tag allied_support_src}
									}
								}
							}
						}
						{tag_remove allied_support_src}
					}
					; Drop src from hidden permanent templates
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_support_src}
									}
								}
								{include
									{tag
										{tag allied_support_template}
									}
									{tag
										{tag hidden}
									}
								}
							}
						}
						{tag_remove allied_support_src}
					}
					{"delay" {time 0.1}}
					; Promote remaining src = clones at entry
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_support_src}
									}
								}
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
										{state user_control}
									}
								}
							}
						}
						{tag_add allied_wave_fresh}
						{tag_add allied_support}
						{tag_add cmp_def}
						{tag_add _def}
						{tag_add _ai_defender}
						{tag_remove hidden}
						{tag_remove not_delete}
						{tag_remove allied_support_template}
						{tag_remove allied_support_src}
						{inactive off}
					}
					{"delay" {time 0.1}}
					; Fail closed if no clones were promoted
					{"switch"
						{"case"
							{condition
								{type entities}
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
										}
										{exclude
											{state
												{state dead}
											}
										}
									}
								}
								{count
									{op ">"}
									{value 0}
								}
							}
							{"timer"
								{time 3}
								{title "allied_support spawn ok"}
							}
						}
						{"default"
							{"timer"
								{time 5}
								{title "allied_support spawn FAIL no clone"}
							}
						}
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
								{tag_add allied_support_owner_1}
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
								{tag_add allied_support_owner_2}
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
								{tag_add allied_support_owner_3}
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
								{tag_add allied_support_owner_4}
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
								{tag_add allied_support_owner_5}
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
								{tag_add allied_support_owner_6}
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
								{tag_add allied_support_owner_7}
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
								{tag_add allied_support_owner_8}
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
								{tag_add allied_support_owner_9}
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
								{tag_add allied_support_owner_10}
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
								{tag_add allied_support_owner_11}
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
								{tag_add allied_support_owner_12}
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
								{tag_add allied_support_owner_13}
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
								{tag_add allied_support_owner_14}
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
								{tag_add allied_support_owner_15}
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
								{tag_add allied_support_owner_16}
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
								{tag_add allied_support_owner_unsupported}
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
					{"switch"
						{"case"
							{condition
								{type cmp_i}
								{var "allied_support_target$"}
								{op "=="}
								{value 1}
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
						}
						{"case"
							{condition
								{type cmp_i}
								{var "allied_support_target$"}
								{op "=="}
								{value 2}
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
									{tag fpc2}
								}
							}
						}
						{"case"
							{condition
								{type cmp_i}
								{var "allied_support_target$"}
								{op "=="}
								{value 3}
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
									{tag fpc3}
								}
							}
						}
						{"case"
							{condition
								{type cmp_i}
								{var "allied_support_target$"}
								{op "=="}
								{value 4}
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
									{tag fpc4}
								}
							}
						}
						{"case"
							{condition
								{type cmp_i}
								{var "allied_support_target$"}
								{op "=="}
								{value 5}
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
									{tag fpc5}
								}
							}
						}

						{"default"
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
						}
					}
					{"switch"
						{"case"
							{condition
								{type entities}
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
											{zone
												{zone "gamezone"}
											}
										}
									}
								}
								{count
									{op ">"}
									{value 0}
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
									}
								}
								{tag_remove allied_wave_fresh}
							}
							{"set_i" {var "allied_support_waves_left$"} {op "-"} {value 1}}
							{"set_i" {var "allied_support_wave_num$"} {op "+"} {value 1}}
							{"timer"
								{time 4}
								{title "allied_support wave consumed"}
							}
						}
						{"default"
							{"timer"
								{time 4}
								{title "allied_support no consume (fail)"}
							}
						}
					}
					{"delay" {time 1}}
					{"set_i" {var "allied_support_busy$"} {op "="} {value 0}}
					{"switch"
						{"case"
							{condition
								{type cmp_i}
								{var "allied_support_waves_left$"}
								{op ">"}
								{value 0}
							}
							{"trigger" {name "allied_support/chance"}}
						}
						{"default"
							{"timer"
								{time 4}
								{title "allied_support waves exhausted"}
							}
						}
					}
				}
			}
