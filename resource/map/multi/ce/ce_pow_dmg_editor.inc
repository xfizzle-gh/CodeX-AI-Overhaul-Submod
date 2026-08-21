; EDITOR ONLY combined tank/HE damage fixture. Do not include from
; ce_triggers.inc / dcg_script.inc. No CWA-map / production changes.
; No fight/select/hold/control-AI. No civilian-mirror. No Conquest.
;
; Exact b95f3cdc five-step P0 AVed under T-62/HE: scene.quant.dmg
; read 0x00000158. Rifle A-E PASS does not cover this. Next cycle is
; this one fixture, not another shooting matrix.
;
; Place three Player-1 combat humans (nato_rifleman or StormV rifleman):
;   aio_pow_dmg_ctrl — normal control, no transfer
;   aio_pow_dmg_p0   — runtime {"player"} {operation set} {player "0"} only
;   aio_pow_dmg_ob   — exact Old Boy five-step (white flag, P0, strip
;                      enemy, {"action"} drop {volume in_hands}, stand_giveup_1)
; Place a player-controlled T-62 (or same HE source). After 2s P0 and
; five-step apply. Then hit ctrl, then p0, then five-step with the SAME
; tank/HE shot. Preserve dump if AV.
;
; If p0 already AVs: Player 0 damage registration is the seam.
; If only five-step AVs: narrow inside that sequence.
; Stop. No Conquest.
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
				}
			}
