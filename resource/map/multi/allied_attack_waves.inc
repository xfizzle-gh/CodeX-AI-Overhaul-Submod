; Allied ATTACK waves for human-attack Dynamic Conquest (CWA).
;
; Purpose: reinforce the friendly Attacker AI ("attack support") with periodic
; infantry waves cloned from the permanent allied_support_templates.inc pool,
; handed to the attack support's player slot, and pushed onto the flag chain.
;
; INERT BY DEFAULT. Nothing in this file runs until allied_attack_enabled$ == 1,
; and no map .mi includes this file yet. It is the production-shaped skeleton for
; the mechanism proven by attack_support_probe.inc; the probe stays the only
; live attack-side experiment until this engine is deployed deliberately.
;
; Ownership never falls back to a hardcoded player. If id_attack_support$ does not
; resolve to a slot in 1..16 the wave is abandoned in place (allied_attack_owner_fail$)
; rather than being gifted to some arbitrary player.
;
; Tag contract:
;   allied_support_template — permanent off-map pool (parked outside gamezone).
;   allied_attack_unit      — transient tag: a freshly cloned, not-yet-dispatched wave.
;   allied_attack_active    — dispatched wave; never re-selected by the spawner.
; The defense engine's own working tags are never reused here, so the two wave
; engines can never select each other's units.

			{"allied_attack/init"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i"
							{var "allied_attack_enabled$"}
							{op "=="}
							{value 1}
						}
						{"2.cmp_i"
							{var "user_is_defender$"}
							{op "=="}
							{value 0}
						}
						{"3.cmp_i"
							{var "attack_support_ready$"}
							{op "=="}
							{value 1}
						}
						{"4.cmp_i"
							{var "id_attack_support$"}
							{op ">"}
							{value 0}
						}
						{"5.cmp_i"
							{var "allied_attack_started$"}
							{op "=="}
							{value 0}
						}
					}
				}
				{actions
					{"timer"
						{time 10}
						{title "ALLIED ATTACK INIT"}
					}
					{"set_i" {var "allied_attack_started$"} {op "="} {value 1}}
					{"delay" {time 0.1}}
					{"trigger" {name "allied_attack/wave_clock"}}
				}
			}

			; Self-re-arming pacing loop. One cycle = randomized wait, then either
			; exhaustion (stop), near-cap defer (skip, re-arm) or dispatch (spawn, re-arm).
			{"allied_attack/wave_clock"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i"
							{var "allied_attack_started$"}
							{op "=="}
							{value 1}
						}
						{"2.cmp_i"
							{var "allied_attack_enabled$"}
							{op "=="}
							{value 1}
						}
						{"3.cmp_i"
							{var "user_is_defender$"}
							{op "=="}
							{value 0}
						}
					}
				}
				{actions
					; Randomized inter-wave delay (weighted cascade, 240..480s).
					{"switch"
						{"case"
							{condition {type rand} {value 0.2}}
							{"delay" {time 240}}
						}
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 300}}
						}
						{"case"
							{condition {type rand} {value 0.33}}
							{"delay" {time 360}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 420}}
						}
						{"default"
							{"delay" {time 480}}
						}
					}
					{"delay" {time 0.1}}
					{"switch"
						; Budget spent — stop the loop for good (no re-arm).
						{"case"
							{condition
								{type cmp_i}
								{var "allied_attack_wave_num$"}
								{op ">="}
								{value 6}
							}
							{"timer"
								{time 6}
								{title "ALLIED ATTACK WAVES EXHAUSTED"}
							}
						}
						; Live-unit cap: too many dispatched attackers still alive.
						; Skip this cycle without consuming a wave, then re-arm.
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag allied_attack_active}
											}
										}
										{include
											{prop
												{prop human}
											}
											{state
												{state operatable}
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
									{value 12}
								}
							}
							{"timer"
								{time 4}
								{title "ALLIED ATTACK NEAR CAP DEFER"}
							}
							{"delay" {time 0.1}}
							{"trigger" {name "allied_attack/wave_clock"}}
						}
						{"default"
							{"trigger" {name "allied_attack/wave_spawn"}}
							{"delay" {time 0.1}}
							{"trigger" {name "allied_attack/wave_clock"}}
						}
					}
				}
			}

			{"allied_attack/wave_spawn"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i"
							{var "allied_attack_started$"}
							{op "=="}
							{value 1}
						}
						{"2.cmp_i"
							{var "id_attack_support$"}
							{op ">"}
							{value 0}
						}
						{"3.cmp_i"
							{var "allied_attack_wave_num$"}
							{op "<"}
							{value 6}
						}
						{"4.entities"
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
										{state
											{state linked}
										}
										{state
											{state user_control}
										}
										{tag
											{tag allied_attack_unit}
										}
										{tag
											{tag allied_attack_active}
										}
									}
								}
							}
							{count
								{op ">="}
								{value 1}
							}
						}
					}
				}
				{actions
					; 1) Mark 4 pool prototypes as this wave's clone sources.
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
									{state
										{state linked}
									}
									{state
										{state user_control}
									}
									{tag
										{tag allied_attack_unit}
									}
									{tag
										{tag allied_attack_active}
									}
								}
							}
							{amount 4}
						}
						{tag_add allied_attack_unit}
					}
					{"delay" {time 0.1}}

					; 2) Clone them onto the map at the shared entry waypoint.
					{"placement"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_attack_unit}
									}
								}
							}
							{amount 4}
						}
						{target_waypoint "allied_support_entry"}
						{clone}
					}
					{"delay" {time 0.5}}

					; 3) The parked prototypes sit outside gamezone; the clones are inside it.
					;    Strip the wave tag off everything still off-map so the pool stays intact
					;    and only the fresh clones carry allied_attack_unit.
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_attack_unit}
									}
								}
								{exclude
									{zone
										{zone "gamezone"}
									}
								}
							}
						}
						{tag_remove allied_attack_unit}
					}
					{"delay" {time 0.1}}

					; 4) State fixups on the clones: visible, active, deletable, no pool tags.
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag allied_attack_unit}
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
						{tag_remove allied_support_template}
						{tag_remove hidden}
						{tag_remove not_delete}
						{tag_remove cmp_def}
						{tag_remove _def}
						{inactive off}
						{impregnability disabled}
						{discovered on}
					}
					{"delay" {time 0.1}}

					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag allied_attack_unit} {type human} {state "not dead"}}
								{count {op ">"} {value 0}}
							}
							{"timer"
								{time 6}
								{title "ALLIED ATTACK CLONES READY"}
							}

							; 5) Ownership. NO hardcoded fallback: the default case only reports.
							{"switch"
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "1"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "2"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "3"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "4"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "5"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "6"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "7"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "8"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "9"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "10"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "11"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "12"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "13"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "14"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "15"}}}
								{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}} {operation set} {player "16"}}}
								{"default"
									{"timer"
										{time 15}
										{title "ALLIED ATTACK OWNER UNRESOLVED"}
									}
									{"set_i" {var "allied_attack_owner_fail$"} {op "="} {value 1}}
								}
							}
							{"delay" {time 0.3}}

							; 6) AI control + aggressive stance for the transferred clones.
							{"actor_state"
								{selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human} {state "not dead"}}
								{control AI}
								{ai_move {mode enable}}
								{weapon_prepare on}
								{fire_mode open}
								{move_mode free}
								{movement {speed fast} {kind fast} {type normal}}
								{ai {no_retreat on} {advance_ratio 2} {retreat_ratio 0}}
							}
							{"ables"
								{selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human}}
								{remove select}
							}
							{"delay" {time 0.1}}

							; 7) Tag swap: unit -> active. Idempotence guarantee — once a wave is
							;    dispatched it carries allied_attack_active only, so the spawner's
							;    allied_attack_unit selectors can never pick it up again.
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag allied_attack_unit}
											}
										}
									}
								}
								{tag_add allied_attack_active}
							}
							{"delay" {time 0.1}}
							{"set_i" {var "allied_attack_wave_num$"} {op "+"} {value 1}}
							{"timer"
								{time 8}
								{title "ALLIED ATTACK WAVE DISPATCHED"}
							}

							; 8) Order the fresh wave onto the first flag (two-step, as proven
							;    by attack_support_probe.inc: raw advance, then the shared
							;    dcg_functions.mi stance/approach function).
							{"action"
								{selector {ignore_captured_by_user 0} {tag allied_attack_unit} {type human} {state operatable}}
								{drop orders}
								{action advance}
								{target {ignore_captured_by_user 0} {tag fpc1}}
							}
							{"delay" {time 0.1}}
							{"call"
								{function "fpc_inf_to_flag1"}
								{context
									{specified}
									{selector {source standart} {tag allied_attack_unit}}
								}
							}
							{"delay" {time 0.1}}

							; 9) Retire the transient tag — dispatched units are now allied_attack_active only.
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag allied_attack_unit}
											}
										}
									}
								}
								{tag_remove allied_attack_unit}
							}
							{"timer"
								{time 6}
								{title "ALLIED ATTACK WAVE ORDERED"}
							}
							{"trigger" {name "allied_attack/retask"}}
						}
						{"default"
							{"timer"
								{time 15}
								{title "ALLIED ATTACK SPAWN FAIL NO CLONES"}
							}
							; Release any half-marked prototypes so the pool is reusable next cycle.
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag allied_attack_unit}
											}
										}
									}
								}
								{tag_remove allied_attack_unit}
							}
						}
					}
				}
			}

			; FALLBACK PATH ONLY. Final target selection is intended to live in the Lua
			; strategic brain (resource/script/multiplayer/modes/attack_support_brain.lua),
			; which issues BotApi CaptureFlag orders per squad. This MI retask exists so the
			; wave still advances if the brain is disarmed or the engine hides the squads.
			{"allied_attack/retask"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i"
							{var "allied_attack_started$"}
							{op "=="}
							{value 1}
						}
						{"2.cmp_i"
							{var "allied_attack_retasked$"}
							{op "=="}
							{value 0}
						}
						{"3.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag allied_attack_active}
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
											{zone "fpc1"}
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
					}
				}
				{actions
					{"set_i" {var "allied_attack_retasked$"} {op "="} {value 1}}
					{"timer"
						{time 8}
						{title "ALLIED ATTACK FPC1 REACHED"}
					}
					{"action"
						{selector {ignore_captured_by_user 0} {tag allied_attack_active} {type human} {state operatable}}
						{drop orders}
						{action advance}
						{target {ignore_captured_by_user 0} {tag fpc2}}
					}
					{"timer"
						{time 10}
						{title "ALLIED ATTACK RETASKED TO FPC2"}
					}
				}
			}
