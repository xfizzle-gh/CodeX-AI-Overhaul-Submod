; EDITOR ONLY killer-ownership damage fixture. Do not include from
; ce_triggers.inc / dcg_script.inc. No CWA-map / production changes.
; No fight/select/hold/control-AI. No civilian-mirror. No Conquest.
; No CE surrender tags / expire / evac / delete on these victims.
;
; Isolation F PASS: owner-controlled T-62 HE killed CONTROL, runtime
; P0-only, and exact five-step P0 with no AV. Conquest owner/player
; damage to P0 POWs was also safe. Production AVs were AI-owned
; damage (T-62/HE scene.quant.dmg and AI M4/rifle scene.quant.bullets),
; both read 0x00000158. Do not treat nearby deleted logs as the cause.
;
; Place three Player-1 combat humans (nato_rifleman or StormV rifleman):
;   aio_pow_dmg_ctrl — normal control, no transfer
;   aio_pow_dmg_p0   — runtime {"player"} {operation set} {player "0"} only
;   aio_pow_dmg_ob   — exact Old Boy five-step (white flag, P0, strip
;                      enemy, {"action"} drop {volume in_hands}, stand_giveup_1)
; Place one ordinary AI-owned shooter (prefer T-62 to match F HE),
; Player 2 / not user_control, tag aio_pow_dmg_ai. After 2s P0 and
; five-step apply, then scripted {action attack} ctrl -> p0 -> ob.
; Preserve dump if AV.
;
; control safe + P0-only AV => abandon Player 0.
; P0-only safe + five-step AV => narrow the five-step.
; all three safe => killer ownership alone is not sufficient.
; Stop. No Conquest. No select 0.
			{"editor_pow_dmg/prepare"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_pow_dmg_p0}
										}
										{tag
											{tag aio_pow_dmg_ob}
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
											{tag aio_pow_dmg_ready}
										}
										{tag
											{tag hidden}
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
											{tag aio_pow_dmg_ai}
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
										{tag aio_pow_dmg_p0}
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
										{tag aio_pow_dmg_ready}
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
										{tag aio_pow_dmg_ob}
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
										{tag aio_pow_dmg_ready}
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
										{tag aio_pow_dmg_ob}
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
										{tag aio_pow_dmg_ready}
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
										{tag aio_pow_dmg_ob}
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
										{tag aio_pow_dmg_ready}
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
										{tag aio_pow_dmg_ob}
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
										{tag aio_pow_dmg_ready}
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
										{tag aio_pow_dmg_ob}
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
										{tag aio_pow_dmg_ready}
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
										{tag aio_pow_dmg_ctrl}
									}
									{tag
										{tag aio_pow_dmg_p0}
									}
									{tag
										{tag aio_pow_dmg_ob}
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
						{tag_add aio_pow_dmg_ready}
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
										{tag aio_pow_dmg_ai}
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
										{tag aio_pow_dmg_ctrl}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
							}
						}
					}
					{"delay"
						{time 6}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_dmg_ai}
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
										{tag aio_pow_dmg_p0}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
							}
						}
					}
					{"delay"
						{time 6}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_dmg_ai}
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
										{tag aio_pow_dmg_ob}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
							}
						}
					}
				}
			}
