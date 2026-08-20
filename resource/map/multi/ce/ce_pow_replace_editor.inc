; EDITOR ONLY one-breed surrender replace. Do not include from ce_triggers.inc.
;
; Breed: mp/nato/2022s/nato_rifleman -> generated_pow/mp/nato/2022s/nato_rifleman
; Gate: tag the source human aio_pow_replace_src. Apply stamps aio_pow_need_replace.
; Force commit: this file effects aio_morale_surrender_apply on that tag after 3s.
;
; Proven: {"placement"} MOVE (no clone); reveal; retire only if {"1.near"}
; {units aio_pow_civ} {near_to aio_pow_replacing} {distance 5} (ce_player_triggers
; / ce_ai_logic grammar). Failed MOVE leaves the original. {effect aio_pow_retire}
; uses the entity {delete} primitive (not {on "delete"} / stat_notify die).
; Missing: living-actor breed query; IE spawn-at-human; copy health/facing/player/squad.
; {target} on a tagged human is FE-shaped; pose/heading is Editor-unproven.
; #129 camp is not on this branch. Walk waypoint aio_pow_walk proves scripted move.
;
; Setup: include ce_pow_replace_editor_templates.inc in the entity block and this
; file in the trigger block. Place mp/nato/2022s/nato_rifleman, tag aio_pow_replace_src.
; Optional waypoint name aio_pow_walk.
			{"editor_pow_replace/force_commit"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_pow_replace_src}
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
											{tag aio_morale_surrendering}
										}
										{tag
											{tag aio_pow_need_replace}
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
						{time 3}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_replace_src}
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
										{tag aio_morale_surrendering}
									}
									{tag
										{tag hidden}
									}
								}
							}
						}
						{effect aio_morale_surrender_apply}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "editor_pow_replace/force_commit"}
					}
				}
			}
			{"editor_pow_replace/replace"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_pow_need_replace}
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
											{tag aio_pow_replacing}
										}
										{tag
											{tag aio_pow_hidden}
										}
										{tag
											{tag aio_pow_civ}
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
											{tag aio_pow_tpl}
										}
									}
									{include
										{tag
											{tag hidden}
										}
									}
									{exclude
										{state
											{state dead}
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
										{tag aio_pow_need_replace}
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
										{tag aio_pow_replacing}
									}
									{tag
										{tag aio_pow_hidden}
									}
									{tag
										{tag aio_pow_civ}
									}
									{tag
										{tag hidden}
									}
								}
							}
							{amount 1}
						}
						{tag_add aio_pow_replacing}
					}
					{"delay"
						{time 0.05}
					}
					{"placement"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{select
									{tag
										{tag aio_pow_tpl}
									}
								}
								{include
									{tag
										{tag hidden}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
						}
						{target
							{ignore_captured_by_user 0}
							{amount 1}
							{tag aio_pow_replacing}
						}
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
										{tag aio_pow_tpl}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
						}
						{tag_remove hidden}
						{tag_add aio_pow_civ}
						{tag_remove aio_pow_tpl}
						{inactive off}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "editor_pow_replace/replace"}
					}
				}
			}
			{"editor_pow_replace/retire"
				{condition
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag aio_pow_civ}
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
								{amount 1}
							}
							{near_to
								{source advanced}
								{group
									{select
										{tag
											{tag aio_pow_replacing}
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
											{tag aio_pow_civ}
										}
										{tag
											{tag aio_pow_tpl}
										}
									}
								}
								{amount 1}
							}
							{distance 5}
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
										{tag aio_pow_replacing}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{tag
										{tag aio_pow_tpl}
									}
									{tag
										{tag aio_pow_civ}
									}
								}
							}
						}
						{effect aio_pow_retire}
					}
					{"delay"
						{time 0.05}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_civ}
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
						{effect aio_morale_surrender_apply}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_civ}
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
						{drop "orders sensor senseless"}
						{fire_mode hold}
						{move_mode hold}
						{weapon_prepare off}
						{ai_move
							{mode disable}
						}
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
										{tag aio_pow_civ}
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
						{drop orders}
						{action move}
						{waypoint "aio_pow_walk"}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "editor_pow_replace/retire"}
					}
				}
			}
