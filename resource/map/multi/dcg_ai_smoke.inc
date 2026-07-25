;// Clean DCG-only AI infantry smoke response.
;// Uses attack_ground because throw_sk_grenade is unreliable in scripted AI flows.

			{"codex/ai_smoke/infantry"
				{condition
					{type 1}
					{terms
						{"1.cmp_i"
							{var "dcg_on$"}
							{op ">"}
							{value 2}
						}
						{"2.see_actors"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{tag
											{tag bot}
										}
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
											{tag _cx_smoke_busy}
										}
										{tag
											{tag _lua_mi}
										}
										{tag
											{tag _lua_alert}
										}
										{tag
											{tag _e_active}
										}
										{cover}
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
											{state "operatable user_control"}
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
								{meters 75}
							}
							{detection visible}
							{tag
								{"tag pair"}
								{"for selector" _cx_smoke_seen}
								{"for enemy" _cx_smoke_target}
							}
						}
					}
				}
				{actions
					{"switch"
						{"case"
							{condition
								{expression "1 & 2"}
								{terms
									{"1.entities"
										{selector
											{source advanced}
											{ignore_captured_by_user 0}
											{group
												{select
													{tag
														{tag _cx_smoke_seen}
													}
												}
												{include
													{with_item
														{type amount}
														{item "nbk39" "grenade"}
														{min 1}
													}
												}
											}
											{group
												{select
													{tag
														{tag _cx_smoke_seen}
													}
												}
												{include
													{with_item
														{type amount}
														{item "ai_nbk39" "grenade"}
														{min 1}
													}
												}
											}
											{group
												{select
													{tag
														{tag _cx_smoke_seen}
													}
												}
												{include
													{with_item
														{type amount}
														{item "rdg1_smoke" "grenade"}
														{min 1}
													}
												}
											}
											{group
												{select
													{tag
														{tag _cx_smoke_seen}
													}
												}
												{include
													{with_item
														{type amount}
														{item "ai_rdg1_smoke" "grenade"}
														{min 1}
													}
												}
											}
											{group
												{select
													{tag
														{tag _cx_smoke_seen}
													}
												}
												{include
													{with_item
														{type amount}
														{item "m24_smoke" "grenade"}
														{min 1}
													}
												}
											}
											{group
												{select
													{tag
														{tag _cx_smoke_seen}
													}
												}
												{include
													{with_item
														{type amount}
														{item "ai_m24_smoke" "grenade"}
														{min 1}
													}
												}
											}
											{group
												{select
													{tag
														{tag _cx_smoke_seen}
													}
												}
												{include
													{with_item
														{type amount}
														{item "rdgm_smoke" "grenade"}
														{min 1}
													}
												}
											}
											{group
												{select
													{tag
														{tag _cx_smoke_seen}
													}
												}
												{include
													{with_item
														{type amount}
														{item "anm8_smoke" "grenade"}
														{min 1}
													}
												}
											}
											{group
												{select
													{tag
														{tag _cx_smoke_seen}
													}
												}
												{include
													{with_item
														{type amount}
														{item "ai_anm8_smoke" "grenade"}
														{min 1}
													}
												}
											}
											{group
												{select
													{tag
														{tag _cx_smoke_seen}
													}
												}
												{include
													{with_item
														{type amount}
														{item "m43_smoke" "grenade"}
														{min 1}
													}
												}
											}
											{group
												{select
													{tag
														{tag _cx_smoke_seen}
													}
												}
												{include
													{with_item
														{type amount}
														{item "ai_m43_smoke" "grenade"}
														{min 1}
													}
												}
											}
										}
									}
									{"2.rand"
										{value 1.0}
									}
								}
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
										{include
											{with_item
												{type amount}
												{item "nbk39" "grenade"}
												{min 1}
											}
										}
									}
									{group
										{select
											{tag
												{tag _cx_smoke_seen}
											}
										}
										{include
											{with_item
												{type amount}
												{item "ai_nbk39" "grenade"}
												{min 1}
											}
										}
									}
									{group
										{select
											{tag
												{tag _cx_smoke_seen}
											}
										}
										{include
											{with_item
												{type amount}
												{item "rdg1_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{group
										{select
											{tag
												{tag _cx_smoke_seen}
											}
										}
										{include
											{with_item
												{type amount}
												{item "ai_rdg1_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{group
										{select
											{tag
												{tag _cx_smoke_seen}
											}
										}
										{include
											{with_item
												{type amount}
												{item "m24_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{group
										{select
											{tag
												{tag _cx_smoke_seen}
											}
										}
										{include
											{with_item
												{type amount}
												{item "ai_m24_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{group
										{select
											{tag
												{tag _cx_smoke_seen}
											}
										}
										{include
											{with_item
												{type amount}
												{item "rdgm_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{group
										{select
											{tag
												{tag _cx_smoke_seen}
											}
										}
										{include
											{with_item
												{type amount}
												{item "anm8_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{group
										{select
											{tag
												{tag _cx_smoke_seen}
											}
										}
										{include
											{with_item
												{type amount}
												{item "ai_anm8_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{group
										{select
											{tag
												{tag _cx_smoke_seen}
											}
										}
										{include
											{with_item
												{type amount}
												{item "m43_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{group
										{select
											{tag
												{tag _cx_smoke_seen}
											}
										}
										{include
											{with_item
												{type amount}
												{item "ai_m43_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{sort
										{type entity}
										{entity
											{tag _cx_smoke_target}
										}
									}
									{amount 1}
								}
								{tag_add _cx_smoke_thrower}
								{tag_add _cx_smoke_busy}
								{tag_add _lua_mi}
							}
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state operatable}
											{with_item
												{type amount}
												{item "nbk39" "grenade"}
												{min 1}
											}
										}
									}
									{"inventory"
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state "not dead"}
										}
										{action take}
										{item "nbk39" "grenade"}
									}
								}
								{"case"
									{condition
										{type entities}
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state operatable}
											{with_item
												{type amount}
												{item "ai_nbk39" "grenade"}
												{min 1}
											}
										}
									}
									{"inventory"
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state "not dead"}
										}
										{action take}
										{item "ai_nbk39" "grenade"}
									}
								}
								{"case"
									{condition
										{type entities}
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state operatable}
											{with_item
												{type amount}
												{item "rdg1_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{"inventory"
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state "not dead"}
										}
										{action take}
										{item "rdg1_smoke" "grenade"}
									}
								}
								{"case"
									{condition
										{type entities}
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state operatable}
											{with_item
												{type amount}
												{item "ai_rdg1_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{"inventory"
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state "not dead"}
										}
										{action take}
										{item "ai_rdg1_smoke" "grenade"}
									}
								}
								{"case"
									{condition
										{type entities}
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state operatable}
											{with_item
												{type amount}
												{item "m24_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{"inventory"
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state "not dead"}
										}
										{action take}
										{item "m24_smoke" "grenade"}
									}
								}
								{"case"
									{condition
										{type entities}
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state operatable}
											{with_item
												{type amount}
												{item "ai_m24_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{"inventory"
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state "not dead"}
										}
										{action take}
										{item "ai_m24_smoke" "grenade"}
									}
								}
								{"case"
									{condition
										{type entities}
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state operatable}
											{with_item
												{type amount}
												{item "rdgm_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{"inventory"
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state "not dead"}
										}
										{action take}
										{item "rdgm_smoke" "grenade"}
									}
								}
								{"case"
									{condition
										{type entities}
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state operatable}
											{with_item
												{type amount}
												{item "anm8_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{"inventory"
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state "not dead"}
										}
										{action take}
										{item "anm8_smoke" "grenade"}
									}
								}
								{"case"
									{condition
										{type entities}
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state operatable}
											{with_item
												{type amount}
												{item "ai_anm8_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{"inventory"
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state "not dead"}
										}
										{action take}
										{item "ai_anm8_smoke" "grenade"}
									}
								}
								{"case"
									{condition
										{type entities}
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state operatable}
											{with_item
												{type amount}
												{item "m43_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{"inventory"
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state "not dead"}
										}
										{action take}
										{item "m43_smoke" "grenade"}
									}
								}
								{"case"
									{condition
										{type entities}
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state operatable}
											{with_item
												{type amount}
												{item "ai_m43_smoke" "grenade"}
												{min 1}
											}
										}
									}
									{"inventory"
										{selector
											{tag _cx_smoke_thrower}
											{type human}
											{state "not dead"}
										}
										{action take}
										{item "ai_m43_smoke" "grenade"}
									}
								}
								{"default"}
							}
							{"delay"
								{time 0.35}
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
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{select
											{tag
												{tag _cx_smoke_target}
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
							}
							{"delay"
								{time 4}
							}
							{"entity_state"
								{selector
									{ignore_captured_by_user 0}
									{tag _cx_smoke_thrower}
								}
								{tag_remove _lua_mi}
								{tag_remove _cx_smoke_busy}
								{tag_remove _cx_smoke_thrower}
							}
							{"entity_state"
								{selector
									{ignore_captured_by_user 0}
									{tag _cx_smoke_seen}
								}
								{tag_remove _cx_smoke_seen}
							}
							{"entity_state"
								{selector
									{ignore_captured_by_user 0}
									{tag _cx_smoke_target}
								}
								{tag_remove _cx_smoke_target}
							}
							{"delay"
								{time 2}
							}
							{"trigger"
								{name "codex/ai_smoke/infantry"}
							}
						}
						{"default"
							{"entity_state"
								{selector
									{ignore_captured_by_user 0}
									{tag _cx_smoke_seen}
								}
								{tag_remove _cx_smoke_seen}
							}
							{"entity_state"
								{selector
									{ignore_captured_by_user 0}
									{tag _cx_smoke_target}
								}
								{tag_remove _cx_smoke_target}
							}
							{"delay"
								{time 2.5}
							}
							{"trigger"
								{name "codex/ai_smoke/infantry"}
							}
						}
					}
				}
			}
