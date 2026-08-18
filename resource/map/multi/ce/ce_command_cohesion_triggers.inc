; PR D: command link / weak / lost + Discipline 25m + casualty shock. No movement seizure.
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
										{tag
											{tag aio_cmd_linked}
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
			{"conquest_enhanced_mechanics/command/mark_linked"
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
										{tag
											{tag aio_cmd_linked}
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
								{"for selector" aio_cmd_linked}
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
										{tag aio_cmd_linked}
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
										{tag aio_cmd_linked}
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
					{"set_i"
						{var "ce_morale_diag_cmd_link$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/command/mark_linked"}
					}
				}
			}
			{"conquest_enhanced_mechanics/command/mark_lost"
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
										{tag
											{tag aio_cmd_lost}
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
								{mode far_than}
								{meters 80}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_cmd_lost}
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
										{tag aio_cmd_lost}
									}
								}
								{exclude
									{state
										{state dead}
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
										{tag aio_cmd_lost}
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
					{"set_i"
						{var "ce_morale_diag_cmd_lost$"}
						{op "="}
						{value 1}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/command/mark_lost"}
					}
				}
			}
			{"conquest_enhanced_mechanics/command/mark_weak"
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
										{tag
											{tag aio_cmd_lost}
										}
										{tag
											{tag aio_cmd_weak}
										}
										{tag
											{tag aio_cmd_linked}
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
								{mode far_than}
								{meters 55}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" aio_cmd_weak}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/command/mark_weak"}
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
									{state
										{state inactive}
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
