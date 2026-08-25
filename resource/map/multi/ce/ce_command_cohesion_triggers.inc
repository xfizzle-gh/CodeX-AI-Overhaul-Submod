; PR D/E: command heartbeat + resistance. see_actors {enemy} is FE/Old Boy grammar for the other party.
; Linked = commander near_than 50. In-range = near_than 80. Weak/Lost derived from missing those pulses.
; Commanders/independent never expire. No movement seizure.
			{"conquest_enhanced_mechanics/command/self_link"
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
											{tag aio_cmd_junior}
										}
										{tag
											{tag aio_cmd_primary}
										}
										{tag
											{tag aio_cmd_senior}
										}
										{tag
											{tag aio_cmd_independent}
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
										{tag aio_cmd_junior}
									}
									{tag
										{tag aio_cmd_primary}
									}
									{tag
										{tag aio_cmd_senior}
									}
									{tag
										{tag aio_cmd_independent}
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
						{tag_add aio_cmd_linked}
					}
					{"entity_state"
						{selector
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
									{tag
										{tag aio_cmd_independent}
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
						{tag_add aio_cmd_had_command}
					}
					{"entity_state"
						{selector
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
									{tag
										{tag aio_cmd_independent}
									}
								}
								{include
									{tag
										{tag aio_cmd_lost}
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
						{tag_add aio_icon_refresh}
					}
					{"entity_state"
						{selector
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
									{tag
										{tag aio_cmd_independent}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
						}
						{tag_remove aio_cmd_lost}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_icon_refresh}
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
						{effect aio_morale_refresh_icons}
					}
					{"entity_state"
						{selector
							{tag aio_icon_refresh}
						}
						{tag_remove aio_icon_refresh}
					}
					{"set_i"
						{var "ce_morale_diag_cmd_link$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/command/self_link"}
					}
				}
			}
			{"conquest_enhanced_mechanics/command/pulse_near"
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
											{state linked}
										}
										{tag
											{tag aio_cmd_junior}
										}
										{tag
											{tag aio_cmd_primary}
										}
										{tag
											{tag aio_cmd_senior}
										}
										{tag
											{tag aio_cmd_independent}
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
								{meters 50}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_cmd_seen}
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
										{tag aio_cmd_seen}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
						}
						{tag_add aio_cmd_linked}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_seen}
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
						{tag_add aio_cmd_had_command}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_seen}
									}
								}
								{include
									{tag
										{tag aio_cmd_lost}
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
						{tag_add aio_icon_refresh}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_seen}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
						}
						{tag_remove aio_cmd_weak}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_seen}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
						}
						{tag_remove aio_cmd_lost}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_icon_refresh}
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
						{effect aio_morale_refresh_icons}
					}
					{"entity_state"
						{selector
							{tag aio_icon_refresh}
						}
						{tag_remove aio_icon_refresh}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_seen}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
						}
						{tag_remove aio_cmd_miss}
					}
					{"set_i"
						{var "ce_morale_diag_cmd_link$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/command/pulse_near"}
					}
				}
			}
			{"conquest_enhanced_mechanics/command/pulse_range"
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
											{state linked}
										}
										{tag
											{tag aio_cmd_junior}
										}
										{tag
											{tag aio_cmd_primary}
										}
										{tag
											{tag aio_cmd_senior}
										}
										{tag
											{tag aio_cmd_independent}
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
								{meters 80}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_cmd_in_range}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/command/pulse_range"}
					}
				}
			}
			{"conquest_enhanced_mechanics/command/recompute"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_mechanic$"}
							{op ">"}
							{value 0}
						}
					}
				}
				{actions
					{"delay"
						{time 1.2}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_miss}
									}
								}
								{exclude
									{tag
										{tag aio_cmd_seen}
									}
									{tag
										{tag aio_cmd_junior}
									}
									{tag
										{tag aio_cmd_primary}
									}
									{tag
										{tag aio_cmd_senior}
									}
									{tag
										{tag aio_cmd_independent}
									}
								}
							}
						}
						{tag_remove aio_cmd_linked}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_linked}
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
										{tag aio_cmd_seen}
									}
									{tag
										{tag aio_cmd_junior}
									}
									{tag
										{tag aio_cmd_primary}
									}
									{tag
										{tag aio_cmd_senior}
									}
									{tag
										{tag aio_cmd_independent}
									}
								}
							}
						}
						{tag_add aio_cmd_miss}
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
										{tag aio_cmd_in_range}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{tag
										{tag aio_cmd_linked}
									}
									{tag
										{tag aio_cmd_junior}
									}
									{tag
										{tag aio_cmd_primary}
									}
									{tag
										{tag aio_cmd_senior}
									}
									{tag
										{tag aio_cmd_independent}
									}
								}
							}
						}
						{tag_add aio_cmd_weak}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_weak}
									}
								}
								{include
									{tag
										{tag aio_cmd_lost}
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
						{tag_add aio_icon_refresh}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_weak}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
						}
						{tag_remove aio_cmd_lost}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_icon_refresh}
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
						{effect aio_morale_refresh_icons}
					}
					{"entity_state"
						{selector
							{tag aio_icon_refresh}
						}
						{tag_remove aio_icon_refresh}
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
										{tag aio_cmd_had_command}
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
										{tag aio_cmd_in_range}
									}
									{tag
										{tag aio_cmd_seen}
									}
									{tag
										{tag aio_cmd_junior}
									}
									{tag
										{tag aio_cmd_primary}
									}
									{tag
										{tag aio_cmd_senior}
									}
									{tag
										{tag aio_cmd_independent}
									}
									{tag
										{tag aio_cmd_lost}
									}
								}
							}
						}
						{tag_add aio_icon_refresh}
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
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{tag
										{tag aio_cmd_in_range}
									}
									{tag
										{tag aio_cmd_seen}
									}
									{tag
										{tag aio_cmd_junior}
									}
									{tag
										{tag aio_cmd_primary}
									}
									{tag
										{tag aio_cmd_senior}
									}
									{tag
										{tag aio_cmd_independent}
									}
								}
							}
						}
						{tag_add aio_cmd_lost}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_icon_refresh}
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
						{effect aio_morale_refresh_icons}
					}
					{"entity_state"
						{selector
							{tag aio_icon_refresh}
						}
						{tag_remove aio_icon_refresh}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_lost}
									}
								}
							}
						}
						{tag_remove aio_cmd_weak}
					}
					{"if"
						{condition
							{type entities}
							{selector
								{tag aio_cmd_lost}
							}
						}
						{"set_i"
							{var "ce_morale_diag_cmd_lost$"}
							{op "="}
							{value 1}
						}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_seen}
									}
								}
							}
						}
						{tag_remove aio_cmd_seen}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_in_range}
									}
								}
							}
						}
						{tag_remove aio_cmd_in_range}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/command/recompute"}
					}
				}
			}
			{"conquest_enhanced_mechanics/command/discipline_aura"
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
											{tag aio_cmd_encouraged}
										}
									}
								}
							}
							{enemy
								{source advanced}
								{group
									{select
										{tag
											{tag aio_discipline}
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
								{"for selector" aio_cmd_encouraged}
							}
						}
					}
				}
				{actions
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_encouraged}
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
						{effect aio_cmd_encouraged}
					}
					{"set_i"
						{var "ce_morale_diag_cmd_encourage$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/command/discipline_aura"}
					}
				}
			}
			{"conquest_enhanced_mechanics/command/casualty_shock"
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
											{tag aio_cmd_shock}
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
										{state
											{state dead}
										}
									}
									{exclude
										{tag
											{tag aio_cmd_shock_spent}
										}
									}
								}
							}
							{distance
								{mode near_than}
								{meters 30}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_cmd_shock}
								{"for enemy" aio_cmd_shock_spent}
							}
						}
					}
				}
				{actions
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_shock}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
						}
						{effect aio_cmd_shock}
					}
					{"set_i"
						{var "ce_morale_diag_cmd_shock$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/command/casualty_shock"}
					}
				}
			}
			{"conquest_enhanced_mechanics/command/probe_vet"
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
											{tag veterancy_lvl_1}
										}
										{tag
											{tag veterancy_lvl_2}
										}
										{tag
											{tag veterancy_lvl_3}
										}
										{tag
											{tag veterancy_lvl_4}
										}
										{tag
											{tag veterancy_lvl_5}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i"
						{var "ce_morale_diag_vet_live$"}
						{op "="}
						{value 1}
					}
				}
			}
