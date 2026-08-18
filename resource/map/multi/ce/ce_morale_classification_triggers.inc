; PR B: marker latch + cleanup + manual/test Shaken/Panic only.
; No automatic Steady/Shaken/Panic escalation. No Broken/retreat/surrender.
			{"conquest_enhanced_mechanics/morale/classify_existing"
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
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_low}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_classified}
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
									}
								}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_regular}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_classified}
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
									}
								}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_trained}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_classified}
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
									}
								}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_elite}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_classified}
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
									}
								}
							}
						}
					}
				}
				{actions
					{"if"
						{condition
							{type cmp_i}
							{var "ce_morale_autodemo_shaken_done$"}
							{op ">"}
							{value 0}
						}
						{"entity_state"
							{selector
								{source advanced}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_trained}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_classified}
										}
										{tag
											{tag aio_morale_fallback}
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
									}
								}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_elite}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_classified}
										}
										{tag
											{tag aio_morale_fallback}
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
									}
								}
							}
							{tag_add aio_morale_late_classified}
						}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{tag
										{tag aio_morale_low}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag aio_morale_classified}
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
								}
							}
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{tag
										{tag aio_morale_regular}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag aio_morale_classified}
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
								}
							}
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{tag
										{tag aio_morale_trained}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag aio_morale_classified}
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
								}
							}
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{tag
										{tag aio_morale_elite}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag aio_morale_classified}
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
								}
							}
						}
						{tag_add aio_morale_classified}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/classify_existing"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/classify_default_regular"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "enable_ce_morale_autodemo$"}
							{op "="}
							{value 0}
						}
						{"3.entities"
							{selector
								{source advanced}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_low}
										}
										{tag
											{tag aio_morale_regular}
										}
										{tag
											{tag aio_morale_trained}
										}
										{tag
											{tag aio_morale_elite}
										}
										{tag
											{tag aio_morale_classified}
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
									{prop
										{prop human}
									}
								}
								{include
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag aio_morale_low}
									}
									{tag
										{tag aio_morale_regular}
									}
									{tag
										{tag aio_morale_trained}
									}
									{tag
										{tag aio_morale_elite}
									}
									{tag
										{tag aio_morale_classified}
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
								}
							}
						}
						{tag_add aio_morale_regular}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{tag
										{tag aio_morale_regular}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag aio_morale_fallback}
									}
									{tag
										{tag aio_morale_low}
									}
									{tag
										{tag aio_morale_trained}
									}
									{tag
										{tag aio_morale_elite}
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
								}
							}
						}
						{tag_add aio_morale_fallback}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{tag
										{tag aio_morale_regular}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag aio_morale_classified}
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
								}
							}
						}
						{tag_add aio_morale_classified}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/classify_default_regular"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/cleanup_dead"
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
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_shaken}
										}
										{state
											{state "dead inactive"}
										}
									}
								}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_panic}
										}
										{state
											{state "dead inactive"}
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
									{prop
										{prop human}
									}
								}
								{include
									{tag
										{tag aio_morale_shaken}
									}
									{state
										{state "dead inactive"}
									}
								}
							}
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{tag
										{tag aio_morale_panic}
									}
									{state
										{state "dead inactive"}
									}
								}
							}
						}
						{tag_remove aio_morale_shaken}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{tag
										{tag aio_morale_panic}
									}
									{state
										{state "dead inactive"}
									}
								}
							}
						}
						{tag_remove aio_morale_panic}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/cleanup_dead"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/force_shaken"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "ce_morale_force_shaken$"}
							{op ">"}
							{value 0}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{tag
										{tag aio_morale_trained}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag aio_morale_fallback}
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
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{tag
										{tag aio_morale_elite}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag aio_morale_fallback}
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
							{amount 3
							}
						}
						{tag_add aio_morale_shaken}
					}
					{"entity_state"
						{selector
							{tag aio_morale_shaken}
							{type human}
							{state operatable}
						}
						{tag_remove aio_morale_panic}
					}
					{"set_i"
						{var "ce_morale_force_shaken$"}
						{op "="}
						{value 0}
					}
					{"if"
						{condition
							{type cmp_i}
							{var "enable_ce_morale_debug$"}
							{op ">"}
							{value 0}
						}
						{"talk"
							{selector
								{ignore_captured_by_user 0}
								{amount 1
								}
								{tag player}
								{zone "gamezone"}
								{type human}
								{state "not dead"}
							}
							{text "mission/multi/ce_morale_force_shaken"}
						}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/force_shaken"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/force_panic"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "ce_morale_force_panic$"}
							{op ">"}
							{value 0}
						}
					}
				}
				{actions
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{tag
										{tag aio_morale_trained}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag aio_morale_fallback}
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
							{group
								{select
									{prop
										{prop human}
									}
								}
								{include
									{tag
										{tag aio_morale_elite}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag aio_morale_fallback}
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
							{amount 3
							}
						}
						{tag_add aio_morale_panic}
					}
					{"entity_state"
						{selector
							{tag aio_morale_panic}
							{type human}
							{state operatable}
						}
						{tag_remove aio_morale_shaken}
					}
					{"set_i"
						{var "ce_morale_force_panic$"}
						{op "="}
						{value 0}
					}
					{"if"
						{condition
							{type cmp_i}
							{var "enable_ce_morale_debug$"}
							{op ">"}
							{value 0}
						}
						{"talk"
							{selector
								{ignore_captured_by_user 0}
								{amount 1
								}
								{tag player}
								{zone "gamezone"}
								{type human}
								{state "not dead"}
							}
							{text "mission/multi/ce_morale_force_panic"}
						}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/force_panic"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/observe_source_profile"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "ce_morale_source_tag_seen$"}
							{op "="}
							{value 0}
						}
						{"3.entities"
							{selector
								{source advanced}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_low}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_fallback}
										}
									}
								}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_regular}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_fallback}
										}
									}
								}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_trained}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_fallback}
										}
									}
								}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_elite}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_fallback}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_source_tag_seen$"}
						{op "="}
						{value 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/observe_source_profile"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/observe_source_quality"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "ce_morale_source_quality_seen$"}
							{op "="}
							{value 0}
						}
						{"3.entities"
							{selector
								{source advanced}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_trained}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_fallback}
										}
									}
								}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_elite}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_fallback}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_source_quality_seen$"}
						{op "="}
						{value 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/observe_source_quality"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/observe_source_command"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "ce_morale_source_command_seen$"}
							{op "="}
							{value 0}
						}
						{"3.entities"
							{selector
								{source advanced}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_cmd_junior}
										}
									}
								}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_cmd_primary}
										}
									}
								}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_cmd_senior}
										}
									}
								}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_cmd_independent}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_source_command_seen$"}
						{op "="}
						{value 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/observe_source_command"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/observe_late"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_autodemo$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "ce_morale_late_seen$"}
							{op "="}
							{value 0}
						}
						{"3.entities"
							{selector
								{source advanced}
								{group
									{select
										{prop
											{prop human}
										}
									}
									{include
										{tag
											{tag aio_morale_late_classified}
										}
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag aio_morale_fallback}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_late_seen$"}
						{op "="}
						{value 1}
					}
					{"talk"
						{selector
							{ignore_captured_by_user 0}
							{amount 1
							}
							{tag player}
							{zone "gamezone"}
							{type human}
							{state "not dead"}
						}
						{text "mission/multi/ce_morale_probe_late_ok"}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/observe_late"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/autodemo_start"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "enable_ce_morale_autodemo$"}
							{op ">"}
							{value 0}
						}
						{"3.cmp_i"
							{var "ce_morale_autodemo_done$"}
							{op "="}
							{value 0}
						}
						{"4.cmp_i"
							{var "ce_morale_autodemo_shaken_done$"}
							{op "="}
							{value 0}
						}
					}
				}
				{actions
					{"delay"
						{time 8}
					}
					{"if"
						{condition
							{type cmp_i}
							{var "ce_morale_source_quality_seen$"}
							{op ">"}
							{value 0}
						}
						{"talk"
							{selector
								{ignore_captured_by_user 0}
								{amount 1
								}
								{tag player}
								{zone "gamezone"}
								{type human}
								{state "not dead"}
							}
							{text "mission/multi/ce_morale_probe_quality_ok"}
						}
						{"set_i"
							{var "ce_morale_force_shaken$"}
							{op "="}
							{value 1}
						}
						{"set_i"
							{var "ce_morale_autodemo_shaken_done$"}
							{op "="}
							{value 1}
						}
					}
					{"if"
						{condition
							{type cmp_i}
							{var "ce_morale_source_quality_seen$"}
							{op "="}
							{value 0}
						}
						{"if"
							{condition
								{type cmp_i}
								{var "ce_morale_source_tag_seen$"}
								{op ">"}
								{value 0}
							}
							{"talk"
								{selector
									{ignore_captured_by_user 0}
									{amount 1
									}
									{tag player}
									{zone "gamezone"}
									{type human}
									{state "not dead"}
								}
								{text "mission/multi/ce_morale_probe_source_regular_only"}
							}
							{"set_i"
								{var "ce_morale_autodemo_done$"}
								{op "="}
								{value 1}
							}
						}
					}
					{"if"
						{condition
							{type cmp_i}
							{var "ce_morale_source_tag_seen$"}
							{op "="}
							{value 0}
						}
						{"talk"
							{selector
								{ignore_captured_by_user 0}
								{amount 1
								}
								{tag player}
								{zone "gamezone"}
								{type human}
								{state "not dead"}
							}
							{text "mission/multi/ce_morale_probe_source_fail"}
						}
						{"set_i"
							{var "ce_morale_autodemo_done$"}
							{op "="}
							{value 1}
						}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/autodemo_panic"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_autodemo$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "ce_morale_autodemo_shaken_done$"}
							{op ">"}
							{value 0}
						}
						{"3.cmp_i"
							{var "ce_morale_autodemo_done$"}
							{op "="}
							{value 0}
						}
					}
				}
				{actions
					{"delay"
						{time 45}
					}
					{"if"
						{condition
							{type cmp_i}
							{var "ce_morale_late_seen$"}
							{op "="}
							{value 0}
						}
						{"talk"
							{selector
								{ignore_captured_by_user 0}
								{amount 1
								}
								{tag player}
								{zone "gamezone"}
								{type human}
								{state "not dead"}
							}
							{text "mission/multi/ce_morale_probe_late_fail"}
						}
					}
					{"set_i"
						{var "ce_morale_force_panic$"}
						{op "="}
						{value 1}
					}
					{"set_i"
						{var "ce_morale_autodemo_done$"}
						{op "="}
						{value 1}
					}
				}
			}
