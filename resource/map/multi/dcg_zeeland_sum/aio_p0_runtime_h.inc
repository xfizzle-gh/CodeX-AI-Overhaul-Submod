; Isolation H — AI-owned T-62 HE splash. EDITOR ONLY.
; Included only by aio_p0_runtime_h.mi. Do not include from
; ce_triggers.inc / dcg_script.inc. F and G missions stay unused.
; No production POW / surrender_present / human_ce changes.
; No select 0, fight 0, hold-fire, control AI, expire, evac, delete.
;
; Isolation G proved runtime Player 0 alone is native-AI-ignore.
; Direct {action attack} on P0 was refused. G did not test AI-owned
; damage into P0. H is collateral/splash only.
;
; After 2s: P0-ONLY bystander gets runtime player 0. FIVE-STEP
; bystander gets the literal Old Boy 5-step from F/G. Then the
; same AI T-62 fires HE at dummy 1, then dummy 2, then dummy 3.
; Never attack a bystander tag.
			{"aio_p0_runtime_h/prepare"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_p0_h_dummy_1}
										}
										{tag
											{tag aio_p0_h_dummy_2}
										}
										{tag
											{tag aio_p0_h_dummy_3}
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
											{tag aio_p0_h_ready}
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
											{tag aio_p0_h_ai}
										}
									}
									{include
										{state
											{state operatable}
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
						{time 2}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_p0_h_bystander_p0}
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
										{tag aio_p0_h_ready}
									}
								}
							}
						}
						{operation set}
						{player "0"}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_p0_h_bystander_ob}
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
										{tag aio_p0_h_ready}
									}
								}
							}
						}
						{effect start_white_flag}
					}
					{"delay"
						{time 0.05}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_p0_h_bystander_ob}
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
										{tag aio_p0_h_ready}
									}
								}
							}
						}
						{operation set}
						{player "0"}
					}
					{"delay"
						{time 0.05}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_p0_h_bystander_ob}
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
										{tag aio_p0_h_ready}
									}
								}
							}
						}
						{tag_remove enemy}
					}
					{"delay"
						{time 0.05}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_p0_h_bystander_ob}
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
										{tag aio_p0_h_ready}
									}
								}
							}
						}
						{action drop}
						{volume in_hands}
					}
					{"delay"
						{time 0.05}
					}
					{"animation"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_p0_h_bystander_ob}
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
										{tag aio_p0_h_ready}
									}
								}
							}
						}
						{collage stand_giveup_1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_p0_h_bystander_1}
									}
									{tag
										{tag aio_p0_h_bystander_p0}
									}
									{tag
										{tag aio_p0_h_bystander_ob}
									}
									{tag
										{tag aio_p0_h_dummy_1}
									}
									{tag
										{tag aio_p0_h_dummy_2}
									}
									{tag
										{tag aio_p0_h_dummy_3}
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
						{tag_add aio_p0_h_ready}
					}
					{"delay"
						{time 1}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_p0_h_ai}
									}
								}
								{include
									{state
										{state operatable}
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
						{action attack}
						{target
							{source advanced}
							{group
								{select
									{tag
										{tag aio_p0_h_dummy_1}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{tag
										{tag aio_p0_h_bystander_1}
									}
									{tag
										{tag aio_p0_h_bystander_p0}
									}
									{tag
										{tag aio_p0_h_bystander_ob}
									}
								}
							}
						}
					}
					{"delay"
						{time 8}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_p0_h_ai}
									}
								}
								{include
									{state
										{state operatable}
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
						{action attack}
						{target
							{source advanced}
							{group
								{select
									{tag
										{tag aio_p0_h_dummy_2}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{tag
										{tag aio_p0_h_bystander_1}
									}
									{tag
										{tag aio_p0_h_bystander_p0}
									}
									{tag
										{tag aio_p0_h_bystander_ob}
									}
								}
							}
						}
					}
					{"delay"
						{time 8}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_p0_h_ai}
									}
								}
								{include
									{state
										{state operatable}
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
						{action attack}
						{target
							{source advanced}
							{group
								{select
									{tag
										{tag aio_p0_h_dummy_3}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{tag
										{tag aio_p0_h_bystander_1}
									}
									{tag
										{tag aio_p0_h_bystander_p0}
									}
									{tag
										{tag aio_p0_h_bystander_ob}
									}
								}
							}
						}
					}
				}
			}
