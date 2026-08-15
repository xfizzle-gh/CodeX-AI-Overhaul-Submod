			{"codex/ai_smoke/infantry"
				{condition
					{type 1}
					{terms
						{"1.see_actors"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{select
										{zone
											{zone "gamezone"}
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
											{state user_control}
										}
										{state
											{state linked}
										}
										{tag
											{tag _user}
										}
										{tag
											{tag _cx_smoke_busy}
										}
										{tag
											{tag _lua_mi}
										}
									}
								}
							}
							{enemy
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{select
										{tag
											{tag _user}
										}
									}
									{include
										{state
											{state operatable}
										}
									}
									{exclude
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
							{tag
								{"tag pair"}
								{"for selector" _cx_smoke_seen}
								{"for enemy" _cx_smoke_target}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{select
									{tag
										{tag _cx_smoke_seen}
									}
								}
								{exclude
									{tag
										{tag _cx_smoke_busy}
									}
									{state
										{state "dead inactive"}
									}
								}
							}
							{amount 1
							}
						}
						{tag_add _cx_smoke_thrower}
						{tag_add _cx_smoke_busy}
					}
					{"inventory"
						{selector
							{tag _cx_smoke_thrower}
							{state "not dead"}
						}
						{action add}
						{item "ai_anm8_smoke" "grenade"}
					}
					{"delay"
						{time 0.25}
					}
					{"inventory"
						{selector
							{tag _cx_smoke_thrower}
							{state "not dead"}
						}
						{action take}
						{item "ai_anm8_smoke" "grenade"}
					}
					{"delay"
						{time 0.4}
					}
					{"actor_state"
						{selector
							{tag _cx_smoke_thrower}
							{state "not dead"}
						}
						{fire_mode hold}
					}
					{"action"
						{selector
							{ignore_captured_by_user 0}
							{tag _cx_smoke_thrower}
							{type human}
							{state "not dead"}
						}
						{drop "orders sensor senseless"}
						{action attack_ground}
						{target
							{ignore_captured_by_user 0}
							{tag _cx_smoke_target}
						}
					}
					{"delay"
						{time 5}
					}
					{"entity_state"
						{selector
							{tag _cx_smoke_thrower}
						}
						{tag_remove _cx_smoke_busy}
						{tag_remove _cx_smoke_thrower}
						{tag_remove _cx_smoke_seen}
					}
					{"entity_state"
						{selector
							{tag _cx_smoke_target}
						}
						{tag_remove _cx_smoke_target}
					}
					{"delay"
						{time 8}
					}
					{"trigger"
						{name "codex/ai_smoke/infantry"}
					}
				}
			}

			{"codex/ai_smoke/assault_flag"
				{condition
					{type 1}
					{terms
						{"1.near"
							{units
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{select
										{zone
											{zone "gamezone"}
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
											{state user_control}
										}
										{state
											{state linked}
										}
										{tag
											{tag _user}
										}
										{tag
											{tag _cx_smoke_busy}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{select
										{tag
											{tag fpc1}
										}
										{tag
											{tag fpc2}
										}
										{tag
											{tag fpc3}
										}
										{tag
											{tag fpc4}
										}
										{tag
											{tag fpc5}
										}
										{tag
											{tag flag}
										}
									}
									{exclude
										{state
											{state inactive}
										}
									}
								}
							}
							{distance 45}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{select
									{zone
										{zone "gamezone"}
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
										{state user_control}
									}
									{tag
										{tag _user}
									}
									{tag
										{tag _cx_smoke_busy}
									}
								}
							}
							{amount 1
							}
						}
						{tag_add _cx_smoke_thrower}
						{tag_add _cx_smoke_busy}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag fpc1}
									}
									{tag
										{tag fpc2}
									}
									{tag
										{tag fpc3}
									}
									{tag
										{tag fpc4}
									}
									{tag
										{tag fpc5}
									}
									{tag
										{tag flag}
									}
								}
								{exclude
									{state
										{state inactive}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag _cx_smoke_thrower}
								}
							}
							{amount 1
							}
						}
						{tag_add _cx_smoke_target}
					}
					{"inventory"
						{selector
							{tag _cx_smoke_thrower}
							{state "not dead"}
						}
						{action add}
						{item "ai_anm8_smoke" "grenade"}
					}
					{"delay"
						{time 0.25}
					}
					{"inventory"
						{selector
							{tag _cx_smoke_thrower}
							{state "not dead"}
						}
						{action take}
						{item "ai_anm8_smoke" "grenade"}
					}
					{"delay"
						{time 0.4}
					}
					{"action"
						{selector
							{tag _cx_smoke_thrower}
							{type human}
							{state "not dead"}
						}
						{drop "orders sensor senseless"}
						{action attack_ground}
						{target
							{tag _cx_smoke_target}
						}
					}
					{"delay"
						{time 5}
					}
					{"entity_state"
						{selector
							{tag _cx_smoke_thrower}
						}
						{tag_remove _cx_smoke_busy}
						{tag_remove _cx_smoke_thrower}
					}
					{"entity_state"
						{selector
							{tag _cx_smoke_target}
						}
						{tag_remove _cx_smoke_target}
					}
					{"delay"
						{time 12}
					}
					{"trigger"
						{name "codex/ai_smoke/assault_flag"}
					}
				}
			}
