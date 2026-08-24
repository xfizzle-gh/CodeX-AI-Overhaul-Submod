; Friendly liberation of captured allies (FE ally_released + our pre-P0 provenance).
; Detect: player-side combat human within 2m of an aio_pow_captor_enemy POW
; (walking or held). Side comes from provenance stamped BEFORE P0, never from
; _user_ally / support tags / current P0 / uniform / breed.
; Apply: FE {player {operation set}} from aio_pow_orig_pN, stop_white_flag,
; {impregnability disabled}, strip prisoner state, withdraw to own rear.
; No {control user}, no re-arm, no teleport, no auto-recruit.
; aio_pow_withdraw lasts until rear arrival. aio_pow_liberated is a separate
; 20s anti-resurrender timer. Clear orig-owner tags after restore.
			(define "pow_liberate_see"
				{"conquest_enhanced_mechanics/broken/pow_liberate_p%slot"
					{condition
						{terms
							{"1.cmp_i"
								{var "enable_ce_morale_mechanic$"}
								{op ">"}
								{value 0}
							}
							{"2.cmp_i"
								{var "id_1st_player$"}
								{op "=="}
								{value %slot}
							}
							{"3.see_actors"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_captor_enemy}
											}
										}
										{include
											{tag
												{tag aio_morale_surrendering}
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
											{tag
												{tag player}
											}
											{tag
												{tag aio_pow_liberated}
											}
											{tag
												{tag aio_pow_liberating}
											}
											{tag
												{tag aio_pow_liberate_cand}
											}
										}
									}
								}
								{enemy
									{source advanced}
									{group
										{select
											{prop
												{prop human}
											}
										}
										{include
											{relation
												{relation ally}
												{player "%slot"}
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
												{tag aio_morale_surrendering}
											}
											{tag
												{tag aio_pow_captor_enemy}
											}
											{tag
												{tag aio_pow_captor_player}
											}
										}
									}
								}
								{distance
									{mode near_than}
									{meters 2}
								}
								{detection located}
								{tag
									{"tag pair"}
									{"for selector" aio_pow_liberate_cand}
								}
							}
						}
					}
					{actions
						{"delay"
							{time 0.2}
						}
						{"trigger"
							{name "conquest_enhanced_mechanics/broken/pow_liberate"}
						}
						{"delay"
							{time 1}
						}
						{"trigger"
							{name "conquest_enhanced_mechanics/broken/pow_liberate_p%slot"}
						}
					}
				}
			)
			("pow_liberate_see" slot(1))
			("pow_liberate_see" slot(2))
			("pow_liberate_see" slot(3))
			("pow_liberate_see" slot(4))
			("pow_liberate_see" slot(5))
			("pow_liberate_see" slot(6))
			("pow_liberate_see" slot(7))
			("pow_liberate_see" slot(8))
			("pow_liberate_see" slot(9))
			("pow_liberate_see" slot(10))
			("pow_liberate_see" slot(11))
			("pow_liberate_see" slot(12))
			("pow_liberate_see" slot(13))
			("pow_liberate_see" slot(14))
			("pow_liberate_see" slot(15))
			("pow_liberate_see" slot(16))
			(define "pow_restore_orig"
				{"player"
					{selector
						{source advanced}
						{group
							{select
								{tag
									{tag aio_pow_liberating}
								}
							}
							{include
								{tag
									{tag aio_pow_orig_p%slot}
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
							}
						}
					}
					{operation set}
					{player "%slot"}
				}
			)
			{"conquest_enhanced_mechanics/broken/pow_liberate"
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
											{tag aio_pow_liberate_cand}
										}
									}
									{include
										{tag
											{tag aio_pow_captor_enemy}
										}
										{tag
											{tag aio_morale_surrendering}
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
										{tag
											{tag player}
										}
										{tag
											{tag aio_pow_liberated}
										}
										{tag
											{tag aio_pow_liberating}
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
										{tag aio_pow_liberate_cand}
									}
								}
								{include
									{tag
										{tag aio_pow_captor_enemy}
									}
									{tag
										{tag aio_morale_surrendering}
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
										{tag aio_pow_liberated}
									}
									{tag
										{tag aio_pow_liberating}
									}
								}
							}
						}
						{tag_add aio_pow_liberating}
					}
					("pow_restore_orig" slot(1))
					("pow_restore_orig" slot(2))
					("pow_restore_orig" slot(3))
					("pow_restore_orig" slot(4))
					("pow_restore_orig" slot(5))
					("pow_restore_orig" slot(6))
					("pow_restore_orig" slot(7))
					("pow_restore_orig" slot(8))
					("pow_restore_orig" slot(9))
					("pow_restore_orig" slot(10))
					("pow_restore_orig" slot(11))
					("pow_restore_orig" slot(12))
					("pow_restore_orig" slot(13))
					("pow_restore_orig" slot(14))
					("pow_restore_orig" slot(15))
					("pow_restore_orig" slot(16))
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_liberating}
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
						{effect aio_pow_clear_orig}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_liberating}
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
						{impregnability disabled}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_liberating}
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
						{effect stop_white_flag}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_add aio_pow_liberated}
						{tag_remove aio_pow_move_issued}
						{tag_remove aio_pow_fast_release_pending}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_add aio_pow_withdraw}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
							{"entity_state"
								{selector
									{tag aio_pow_liberating}
								}
								{tag_add aio_pow_withdraw_a}
							}
							{"action"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_withdraw}
											}
										}
										{include
											{tag
												{tag aio_pow_liberating}
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
										}
									}
								}
								{action move}
								{waypoint "attack_support_entry_a"}
							}
						}
						{"case"
							{condition {type cmp_i} {var "enemy_spawnside$"} {op "<="} {value 1}}
							{"entity_state"
								{selector
									{tag aio_pow_liberating}
								}
								{tag_add aio_pow_withdraw_b}
							}
							{"action"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag aio_pow_withdraw}
											}
										}
										{include
											{tag
												{tag aio_pow_liberating}
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
										}
									}
								}
								{action move}
								{waypoint "attack_support_entry_b"}
							}
						}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_liberating}
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
						{effect aio_pow_liberate_guard}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove aio_morale_surrendering}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove aio_morale_surrender_fx}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove aio_morale_surrender_presenting}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove aio_morale_surrender_evacuating}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove aio_morale_surrender_to_camp}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove aio_morale_surrender_to_enemy_camp}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove aio_morale_surrender_to_a}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove aio_morale_surrender_to_b}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove aio_morale_surrender_expire}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove prisoner_in_camp}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove prisoner_in_enemy_camp}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove aio_pow_captor_enemy}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove aio_pow_captor_player}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove aio_pow_liberate_cand}
					}
					{"entity_state"
						{selector
							{tag aio_pow_liberating}
						}
						{tag_remove aio_pow_liberating}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/pow_liberate"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/pow_withdraw_arrive_a"
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
											{tag aio_pow_withdraw_a}
										}
									}
									{include
										{tag
											{tag aio_pow_withdraw}
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
										{tag
											{tag player}
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
								{meters 200}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_pow_withdraw_at_rear}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{tag aio_pow_withdraw_at_rear}
						}
						{tag_remove aio_pow_withdraw}
					}
					{"entity_state"
						{selector
							{tag aio_pow_withdraw_at_rear}
						}
						{tag_remove aio_pow_withdraw_a}
					}
					{"entity_state"
						{selector
							{tag aio_pow_withdraw_at_rear}
						}
						{tag_remove aio_pow_withdraw_at_rear}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/pow_withdraw_arrive_a"}
					}
				}
			}
			{"conquest_enhanced_mechanics/broken/pow_withdraw_arrive_b"
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
											{tag aio_pow_withdraw_b}
										}
									}
									{include
										{tag
											{tag aio_pow_withdraw}
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
										{tag
											{tag player}
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
								{meters 200}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_pow_withdraw_at_rear}
							}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{tag aio_pow_withdraw_at_rear}
						}
						{tag_remove aio_pow_withdraw}
					}
					{"entity_state"
						{selector
							{tag aio_pow_withdraw_at_rear}
						}
						{tag_remove aio_pow_withdraw_b}
					}
					{"entity_state"
						{selector
							{tag aio_pow_withdraw_at_rear}
						}
						{tag_remove aio_pow_withdraw_at_rear}
					}
					{"delay"
						{time 2}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/broken/pow_withdraw_arrive_b"}
					}
				}
			}
