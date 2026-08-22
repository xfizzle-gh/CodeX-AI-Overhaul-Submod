; EDITOR ONLY AI-owned HE splash/collateral fixture. Do not include
; from ce_triggers.inc / dcg_script.inc. No CWA-map / production
; changes. No fight/select/hold/control-AI. No civilian-mirror.
; No Conquest. No CE surrender tags / expire / evac / delete on
; these victims. Never {action attack} the P0 subjects themselves.
;
; Isolation F PASS: owner-controlled T-62 HE killed CONTROL, runtime
; P0-only, and exact five-step P0 with no AV. Conquest owner/player
; damage to P0 POWs was also safe.
;
; Isolation G: AI-owned T-62 killed CONTROL, would not target
; runtime-P0-only or five-step P0 (native ignore). Owner then
; manually killed both P0 subjects, no AV. ProcDump was
; SetThreadName/startup, not a GoH AV. G does not test AI-owned
; damage into P0. Runtime P0 alone is enough for native ignore.
;
; Production AVs were AI-owned damage (T-62/HE scene.quant.dmg
; and AI M4/rifle scene.quant.bullets), both read 0x00000158.
; Do not treat nearby deleted logs as the cause unless the crash
; victim ID enters that transition on the same tick.
;
; Place three stations. Each station is one ordinary targetable
; dummy human plus one subject in that dummy's HE blast radius:
;   aio_pow_dmg_dummy_ctrl + aio_pow_dmg_ctrl (ordinary control)
;   aio_pow_dmg_dummy_p0   + aio_pow_dmg_p0   (runtime P0 only)
;   aio_pow_dmg_dummy_ob   + aio_pow_dmg_ob   (exact Old Boy
;                      five-step: white flag, P0, strip enemy,
;                      {"action"} drop {volume in_hands},
;                      stand_giveup_1)
; Place one AI-owned T-62/HE, never user_control, tag
; aio_pow_dmg_ai. After P0 states settle, scripted {action attack}
; dummy_ctrl -> dummy_p0 -> dummy_ob. Place each P0 subject close
; enough that the blast damages/kills it. Preserve dump if AV.
;
; ordinary collateral safe + P0-only splash AV => abandon Player 0.
; P0-only splash safe + five-step splash AV => narrow the five-step.
; both P0 splash cases safe => AI ownership/collateral alone is
; not sufficient; then one production-state fixture.
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
						{"3.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_pow_dmg_dummy_ctrl}
										}
										{tag
											{tag aio_pow_dmg_dummy_p0}
										}
										{tag
											{tag aio_pow_dmg_dummy_ob}
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
										{tag aio_pow_dmg_dummy_ctrl}
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
										{tag aio_pow_dmg_dummy_p0}
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
										{tag aio_pow_dmg_dummy_ob}
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
