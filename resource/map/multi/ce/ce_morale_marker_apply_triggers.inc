; PR B: hidden inventory markers -> runtime aio_* tags. FE with_item / tag_add / remove.
			{"conquest_enhanced_mechanics/morale/apply_morale_low"
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
									{include
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "aio_marker_morale_low"}
										}
									}
									{exclude
										{tag
											{tag aio_morale_low}
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
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_morale_low"}
									}
								}
								{exclude
									{tag
										{tag aio_morale_low}
									}
								}
							}
						}
						{tag_add aio_morale_low}
					}
					{"delay"
						{time 0.1}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_low}
									}
								}
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_morale_low"}
									}
								}
							}
						}
						{action remove}
						{item "aio_marker_morale_low"}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/apply_morale_low"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/apply_morale_regular"
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
									{include
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "aio_marker_morale_regular"}
										}
									}
									{exclude
										{tag
											{tag aio_morale_regular}
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
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_morale_regular"}
									}
								}
								{exclude
									{tag
										{tag aio_morale_regular}
									}
								}
							}
						}
						{tag_add aio_morale_regular}
					}
					{"delay"
						{time 0.1}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_regular}
									}
								}
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_morale_regular"}
									}
								}
							}
						}
						{action remove}
						{item "aio_marker_morale_regular"}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/apply_morale_regular"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/apply_morale_trained"
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
									{include
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "aio_marker_morale_trained"}
										}
									}
									{exclude
										{tag
											{tag aio_morale_trained}
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
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_morale_trained"}
									}
								}
								{exclude
									{tag
										{tag aio_morale_trained}
									}
								}
							}
						}
						{tag_add aio_morale_trained}
					}
					{"delay"
						{time 0.1}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_trained}
									}
								}
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_morale_trained"}
									}
								}
							}
						}
						{action remove}
						{item "aio_marker_morale_trained"}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/apply_morale_trained"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/apply_morale_elite"
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
									{include
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "aio_marker_morale_elite"}
										}
									}
									{exclude
										{tag
											{tag aio_morale_elite}
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
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_morale_elite"}
									}
								}
								{exclude
									{tag
										{tag aio_morale_elite}
									}
								}
							}
						}
						{tag_add aio_morale_elite}
					}
					{"delay"
						{time 0.1}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_morale_elite}
									}
								}
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_morale_elite"}
									}
								}
							}
						}
						{action remove}
						{item "aio_marker_morale_elite"}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/apply_morale_elite"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/apply_cmd_junior"
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
									{include
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "aio_marker_cmd_junior"}
										}
									}
									{exclude
										{tag
											{tag aio_cmd_junior}
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
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_cmd_junior"}
									}
								}
								{exclude
									{tag
										{tag aio_cmd_junior}
									}
								}
							}
						}
						{tag_add aio_cmd_junior}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_junior}
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
					{"delay"
						{time 0.1}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_junior}
									}
								}
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_cmd_junior"}
									}
								}
							}
						}
						{action remove}
						{item "aio_marker_cmd_junior"}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/apply_cmd_junior"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/apply_cmd_primary"
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
									{include
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "aio_marker_cmd_primary"}
										}
									}
									{exclude
										{tag
											{tag aio_cmd_primary}
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
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_cmd_primary"}
									}
								}
								{exclude
									{tag
										{tag aio_cmd_primary}
									}
								}
							}
						}
						{tag_add aio_cmd_primary}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_primary}
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
					{"delay"
						{time 0.1}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_primary}
									}
								}
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_cmd_primary"}
									}
								}
							}
						}
						{action remove}
						{item "aio_marker_cmd_primary"}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/apply_cmd_primary"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/apply_cmd_senior"
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
									{include
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "aio_marker_cmd_senior"}
										}
									}
									{exclude
										{tag
											{tag aio_cmd_senior}
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
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_cmd_senior"}
									}
								}
								{exclude
									{tag
										{tag aio_cmd_senior}
									}
								}
							}
						}
						{tag_add aio_cmd_senior}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_senior}
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
					{"delay"
						{time 0.1}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_senior}
									}
								}
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_cmd_senior"}
									}
								}
							}
						}
						{action remove}
						{item "aio_marker_cmd_senior"}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/apply_cmd_senior"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/apply_cmd_independent"
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
									{include
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "aio_marker_cmd_independent"}
										}
									}
									{exclude
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
					{"entity_state"
						{selector
							{source advanced}
							{group
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_cmd_independent"}
									}
								}
								{exclude
									{tag
										{tag aio_cmd_independent}
									}
								}
							}
						}
						{tag_add aio_cmd_independent}
					}
					{"delay"
						{time 0.1}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_cmd_independent}
									}
								}
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_cmd_independent"}
									}
								}
							}
						}
						{action remove}
						{item "aio_marker_cmd_independent"}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/apply_cmd_independent"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/apply_discipline"
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
									{include
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "aio_marker_discipline"}
										}
									}
									{exclude
										{tag
											{tag aio_discipline}
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
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_discipline"}
									}
								}
								{exclude
									{tag
										{tag aio_discipline}
									}
								}
							}
						}
						{tag_add aio_discipline}
					}
					{"delay"
						{time 0.1}
					}
					{"inventory"
						{selector
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
									{with_item
										{type existance}
										{item "aio_marker_discipline"}
									}
								}
							}
						}
						{action remove}
						{item "aio_marker_discipline"}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/apply_discipline"}
					}
				}
			}
			{"conquest_enhanced_mechanics/morale/apply_steadfast"
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
									{include
										{prop
											{prop human}
										}
										{with_item
											{type existance}
											{item "aio_marker_steadfast"}
										}
									}
									{exclude
										{tag
											{tag aio_steadfast}
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
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_steadfast"}
									}
								}
								{exclude
									{tag
										{tag aio_steadfast}
									}
								}
							}
						}
						{tag_add aio_steadfast}
					}
					{"delay"
						{time 0.1}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_steadfast}
									}
								}
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "aio_marker_steadfast"}
									}
								}
							}
						}
						{action remove}
						{item "aio_marker_steadfast"}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "conquest_enhanced_mechanics/morale/apply_steadfast"}
					}
				}
			}
