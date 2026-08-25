; Broken escape: own spawn-side rear. Morale owns movement only while Broken.
; Writers must exclude aio_morale_owned. No player seizure. No POW/surrender.
; One-shot fast retreat: actor_state speed fast -> claim aio_morale_retreat_issued
; -> 0.25s -> dest move to spawn_a/spawn_b from enemy_spawnside$ + current side.
; Arrival at own rear rallies to Shaken and releases ownership.
; Wave tag aio_morale_retreat_wave lets later breaks claim without re-moving runners.
; Hop-in tags aio_morale_needs_resume; hop-out one-shot reclaims wave and re-invokes escape.
			{"conquest_enhanced_mechanics/broken/acquire"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_broken}
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
											{state inactive}
										}
										{state
											{state user_control}
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
							{group
								{select
									{tag
										{tag aio_morale_broken}
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
										{state inactive}
									}
									{state
										{state user_control}
									}
									{tag
										{tag aio_morale_owned}
									}
									{tag
										{tag aio_morale_retreat_issued}
									}
								}
							}
						}
						{drop orders}
						{move_mode free}
						{ai_move
							{mode enable}
						}
						{movement
							{speed fast}
						}
						{ai
							{advance_ratio "0.1"}
							{retreat_ratio 4}
						}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_broken}
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
										{state inactive}
									}
									{state
										{state user_control}
									}
								}
							}
						}
						{tag_add aio_morale_owned}
					}
					{"set_i"
						{var "ce_morale_diag_broken$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/acquire"}
					}
				}
			}
			
			{"conquest_enhanced_mechanics/broken/escape"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_owned}
										}
									}
									{include
										{tag
											{tag aio_morale_broken}
										}
										{prop
											{prop human}
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
											{state user_control}
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
							{group
								{select
									{tag
										{tag aio_morale_owned}
									}
								}
								{include
									{tag
										{tag aio_morale_broken}
									}
									{prop
										{prop human}
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
										{state user_control}
									}
									{tag
										{tag aio_morale_retreat_issued}
									}
								}
							}
						}
						{drop orders}
						{move_mode free}
						{ai_move
							{mode enable}
						}
						{movement
							{speed fast}
						}
						{ai
							{advance_ratio "0.1"}
							{retreat_ratio 4}
						}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_owned}
									}
								}
								{include
									{tag
										{tag aio_morale_broken}
									}
									{prop
										{prop human}
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
										{state user_control}
									}
									{tag
										{tag aio_morale_retreat_issued}
									}
								}
							}
						}
						{tag_add aio_morale_retreat_wave}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_owned}
									}
								}
								{include
									{tag
										{tag aio_morale_broken}
									}
									{prop
										{prop human}
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
										{state user_control}
									}
									{tag
										{tag aio_morale_retreat_issued}
									}
								}
							}
						}
						{tag_add aio_morale_retreat_issued}
					}
					{"delay"
						{time 0.25}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "enemy_spawnside$"} {op "<="} {value 1}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 1}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "1"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "1"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 2}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "2"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "2"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 3}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "3"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "3"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 4}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "4"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "4"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 5}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "5"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "5"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 6}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "6"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "6"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 7}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "7"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "7"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 8}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "8"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "8"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 9}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "9"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "9"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 10}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "10"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "10"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 11}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "11"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "11"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 12}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "12"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "12"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 13}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "13"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "13"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 14}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "14"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "14"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 15}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "15"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "15"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 16}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "16"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "16"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
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
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "1"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "1"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
								}
							}
						}
						{"case"
							{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 1}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "1"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "1"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 2}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "2"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "2"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 3}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "3"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "3"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 4}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "4"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "4"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 5}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "5"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "5"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 6}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "6"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "6"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 7}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "7"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "7"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 8}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "8"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "8"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 9}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "9"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "9"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 10}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "10"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "10"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 11}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "11"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "11"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 12}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "12"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "12"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 13}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "13"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "13"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 14}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "14"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "14"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 15}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "15"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "15"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 16}}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "16"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "16"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
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
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
													}
													{relation
														{relation ally}
														{player "1"}
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
														{state user_control}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_a}
													}
												}
											}
											{amount 1}
										}
									}
									{"action"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_retreat_wave}
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
														{state user_control}
													}
													{relation
														{relation ally}
														{player "1"}
													}
												}
											}
										}
										{drop orders}
										{action move}
										{target
											{source advanced}
											{group
												{select
													{tag
														{tag spawn_b}
													}
												}
											}
											{amount 1}
										}
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
										{tag aio_morale_retreat_wave}
									}
								}
								{include
									{tag
										{tag aio_morale_broken}
									}
									{prop
										{prop human}
									}
									{tag
										{tag aio_morale_retreat_issued}
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
						{tag_remove aio_morale_retreat_wave}
					}
					{"set_i"
						{var "ce_morale_diag_retreat$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/escape"}
					}
				}
			}

			{"conquest_enhanced_mechanics/broken/control_hold"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_owned}
										}
									}
									{include
										{tag
											{tag aio_morale_broken}
										}
										{prop
											{prop human}
										}
										{tag
											{tag aio_morale_retreat_issued}
										}
										{state
											{state user_control}
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
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_owned}
									}
								}
								{include
									{tag
										{tag aio_morale_broken}
									}
									{prop
										{prop human}
									}
									{tag
										{tag aio_morale_retreat_issued}
									}
									{state
										{state user_control}
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
						{tag_add aio_morale_needs_resume}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/control_hold"}
					}
				}
			}

			{"conquest_enhanced_mechanics/broken/resume"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_needs_resume}
										}
									}
									{include
										{tag
											{tag aio_morale_owned}
										}
										{tag
											{tag aio_morale_broken}
										}
										{prop
											{prop human}
										}
										{tag
											{tag aio_morale_retreat_issued}
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
											{state user_control}
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
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_needs_resume}
									}
								}
								{include
									{tag
										{tag aio_morale_owned}
									}
									{tag
										{tag aio_morale_broken}
									}
									{prop
										{prop human}
									}
									{tag
										{tag aio_morale_retreat_issued}
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
										{state user_control}
									}
								}
							}
						}
						{drop orders}
						{move_mode free}
						{ai_move
							{mode enable}
						}
						{movement
							{speed fast}
						}
						{ai
							{advance_ratio "0.1"}
							{retreat_ratio 4}
						}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_needs_resume}
									}
								}
								{include
									{tag
										{tag aio_morale_owned}
									}
									{tag
										{tag aio_morale_broken}
									}
									{prop
										{prop human}
									}
									{tag
										{tag aio_morale_retreat_issued}
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
										{state user_control}
									}
								}
							}
						}
						{tag_add aio_morale_retreat_wave}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_needs_resume}
									}
								}
								{include
									{tag
										{tag aio_morale_owned}
									}
									{tag
										{tag aio_morale_broken}
									}
									{prop
										{prop human}
									}
									{tag
										{tag aio_morale_retreat_issued}
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
										{state user_control}
									}
								}
							}
						}
						{tag_remove aio_morale_needs_resume}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/escape"}
					}
				}
			}

			{"conquest_enhanced_mechanics/broken/arrive_a"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.see_actors"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_owned}
										}
									}
									{include
										{tag
											{tag aio_morale_broken}
										}
										{prop
											{prop human}
										}
										{tag
											{tag aio_morale_retreat_issued}
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
											{state user_control}
										}
									}
								}
							}
							{enemy
								{source advanced}
								{group
									{select
										{tag
											{tag spawn_a}
										}
									}
								}
							}
							{distance
								{mode near_than}
								{meters 25}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_morale_at_rear}
							}
						}
					}
				}
				{actions
					{"switch"
						{"case"
							{condition {type cmp_i} {var "enemy_spawnside$"} {op "<="} {value 1}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 1}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "1"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 2}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "2"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 3}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "3"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 4}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "4"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 5}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "5"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 6}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "6"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 7}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "7"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 8}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "8"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 9}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "9"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 10}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "10"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 11}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "11"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 12}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "12"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 13}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "13"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 14}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "14"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 15}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "15"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 16}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "16"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"default"
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "1"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
							}
						}
						{"case"
							{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 1}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "1"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 2}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "2"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 3}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "3"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 4}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "4"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 5}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "5"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 6}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "6"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 7}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "7"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 8}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "8"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 9}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "9"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 10}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "10"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 11}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "11"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 12}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "12"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 13}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "13"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 14}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "14"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 15}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "15"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 16}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "16"}
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
										{effect aio_morale_rally}
									}
								}
								{"default"
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "1"}
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
										{effect aio_morale_rally}
									}
								}
							}
						}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/arrive_a"}
					}
				}
			}

			{"conquest_enhanced_mechanics/broken/arrive_b"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.see_actors"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_owned}
										}
									}
									{include
										{tag
											{tag aio_morale_broken}
										}
										{prop
											{prop human}
										}
										{tag
											{tag aio_morale_retreat_issued}
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
											{state user_control}
										}
									}
								}
							}
							{enemy
								{source advanced}
								{group
									{select
										{tag
											{tag spawn_b}
										}
									}
								}
							}
							{distance
								{mode near_than}
								{meters 25}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_morale_at_rear}
							}
						}
					}
				}
				{actions
					{"switch"
						{"case"
							{condition {type cmp_i} {var "enemy_spawnside$"} {op "<="} {value 1}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 1}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "1"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 2}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "2"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 3}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "3"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 4}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "4"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 5}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "5"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 6}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "6"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 7}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "7"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 8}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "8"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 9}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "9"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 10}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "10"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 11}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "11"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 12}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "12"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 13}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "13"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 14}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "14"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 15}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "15"}
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
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 16}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "16"}
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
										{effect aio_morale_rally}
									}
								}
								{"default"
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
													{relation
														{relation ally}
														{player "1"}
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
										{effect aio_morale_rally}
									}
								}
							}
						}
						{"case"
							{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 1}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "1"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 2}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "2"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 3}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "3"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 4}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "4"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 5}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "5"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 6}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "6"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 7}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "7"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 8}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "8"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 9}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "9"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 10}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "10"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 11}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "11"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 12}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "12"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 13}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "13"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 14}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "14"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 15}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "15"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"case"
									{condition {type cmp_i} {var "id_1st_player$"} {op "=="} {value 16}}
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "16"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
								{"default"
									{"effect"
										{selector
											{source advanced}
											{group
												{select
													{tag
														{tag aio_morale_owned}
													}
												}
												{include
													{tag
														{tag aio_morale_broken}
													}
													{prop
														{prop human}
													}
													{tag
														{tag aio_morale_retreat_issued}
													}
													{tag
														{tag aio_morale_at_rear}
													}
												}
												{exclude
													{state
														{state dead}
													}
													{state
														{state inactive}
													}
													{relation
														{relation ally}
														{player "1"}
													}
												}
											}
										}
										{effect aio_morale_rally}
									}
								}
							}
						}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/arrive_b"}
					}
				}
			}

			{"conquest_enhanced_mechanics/broken/leader_rally"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.see_actors"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_broken}
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
											{state inactive}
										}
										{tag
											{tag aio_morale_just_broken}
										}
									}
								}
							}
							{enemy
								{source advanced}
								{group
									{select
										{tag
											{tag aio_cmd_junior}
										}
										{tag
											{tag aio_cmd_primary}
										}
										{tag
											{tag aio_cmd_senior}
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
											{state inactive}
										}
									}
								}
							}
							{distance
								{mode near_than}
								{meters 25}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_morale_leader_rally}
							}
						}
					}
				}
				{actions
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_leader_rally}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
						}
						{drop orders}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_leader_rally}
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
						{effect aio_morale_rally}
					}
					{"entity_state"
						{selector
							{tag aio_morale_leader_rally}
						}
						{tag_remove aio_morale_leader_rally}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/leader_rally"}
					}
				}
			}

			{"conquest_enhanced_mechanics/broken/cleanup_dead"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_morale_owned}
										}
									}
									{include
										{state
											{state dead}
										}
									}
								}
								{group
									{select
										{tag
											{tag aio_morale_owned}
										}
									}
									{include
										{state
											{state inactive}
										}
									}
								}
								{group
									{select
										{tag
											{tag aio_morale_broken}
										}
									}
									{include
										{state
											{state dead}
										}
									}
								}
								{group
									{select
										{tag
											{tag aio_morale_broken}
										}
									}
									{include
										{state
											{state inactive}
										}
									}
								}
								{group
									{select
										{tag
											{tag aio_morale_retreat_issued}
										}
									}
									{include
										{state
											{state dead}
										}
									}
								}
								{group
									{select
										{tag
											{tag aio_morale_retreat_issued}
										}
									}
									{include
										{state
											{state inactive}
										}
									}
								}
								{group
									{select
										{tag
											{tag aio_morale_retreat_wave}
										}
									}
									{include
										{state
											{state dead}
										}
									}
								}
								{group
									{select
										{tag
											{tag aio_morale_retreat_wave}
										}
									}
									{include
										{state
											{state inactive}
										}
									}
								}
								{group
									{select
										{tag
											{tag aio_morale_at_rear}
										}
									}
									{include
										{state
											{state dead}
										}
									}
								}
								{group
									{select
										{tag
											{tag aio_morale_at_rear}
										}
									}
									{include
										{state
											{state inactive}
										}
									}
								}
								{group
									{select
										{tag
											{tag aio_morale_needs_resume}
										}
									}
									{include
										{state
											{state dead}
										}
									}
								}
								{group
									{select
										{tag
											{tag aio_morale_needs_resume}
										}
									}
									{include
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
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_owned}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_owned}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_owned}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_broken}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_broken}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_broken}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_retreat_issued}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_retreat_issued}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_retreat_issued}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_retreat_wave}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_retreat_wave}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_retreat_wave}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_at_rear}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_at_rear}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_at_rear}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_needs_resume}
									}
								}
								{include
									{state
										{state dead}
									}
								}
							}
							{group
								{select
									{tag
										{tag aio_morale_needs_resume}
									}
								}
								{include
									{state
										{state inactive}
									}
								}
							}
						}
						{tag_remove aio_morale_needs_resume}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/cleanup_dead"}
					}
				}
			}
