
;// Scripts written by Horten
(define "looting_weapon"
			{"hf inf ai/looting/go loot %weapon" ;// AI doesn't have weapon in its inventory and is in range of a dead body that has the weapon
				{condition
					{terms
						{"3.near"
							{units
								{source advanced}
								{group
									{include
										{prop
											{prop human}
										}
									}
									{exclude
										{tag
											{tag go_loot_%weapon}
										}
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{state
											{state user_control}
										}
										{with_item
											{type existance}
											{item "%weapon" "%type"}
											{filled}
										}
									}
								}
								{amount 1
								}
							}
							{near_to
								{tag has_%weapon}
								{type human}
								{state dead}
							}
							{distance 10}
						}
					}
				}
				{actions
					{"delay"
						{time 0.05}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
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
									{state
										{state user_control}
									}
									{tag
										{tag go_loot_%weapon}
									}
									{with_item
										{type existance}
										{item "%weapon" "%type"}
										{filled}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag has_%weapon}
								}
							}
							{amount 1
							}
						}
						{tag_add go_loot_%weapon}
					}
					{"delay"
						{time 0.15}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag go_loot_%weapon}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{tag
										{tag player}
									}
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
						{movement
							{type crawl}
						}
					}
					{"delay"
						{time 0.05}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag go_loot_%weapon}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{tag
										{tag player}
									}
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
						{action take_from}
						{item "%weapon" "%type"}
						{entity
							{tag has_%weapon}
							{type human}
							{state dead}
						}
						{"talk"
							{selector
								{tag bot}
							}
							{say ill_take_it}
							{text ""}
						}
					}
					{"trigger"
						{name "hf inf ai/looting/go loot %weapon"}
					}
				}
			}
			{"hf inf ai/looting/body with %weapon in inventory" ;// AI is dead with weapon in his inventory - add tags
				{condition
					{terms
						{"1.inventory"
							{selector
								{type human}
								{state dead}
							}
							{where "all holders"}
							{item "%weapon"}
						}
					}
				}
				{actions
					{"delay"
						{time 0.25}
					}
					{"entity_state"
						{selector
							{type human}
							{state dead}
						}
						{tag_add loot_with_%weapon}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag loot_with_%weapon}
									}
								}
								{include
									{with_item
										{type existance}
										{item "%weapon" "%type"}
										{filled}
									}
								}
							}
						}
						{tag_add has_%weapon}
					}
				}
			}
			{"hf inf ai/looting/no more %weapon in body" ;// AI dead body no more has the weapon in it's inventory, de-tag
				{condition
					{terms
						{"1.inventory"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag has_%weapon}
										}
									}
									{include
										{prop
											{prop human}
										}
										{state
											{state dead}
										}
									}
								}
							}
							{where "all holders"}
							{item "%weapon"}
							{compare equal}
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
							{group
								{select
									{tag
										{tag has_%weapon}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag go_loot_%weapon}
								}
							}
							{amount 1
							}
						}
						{tag_remove loot}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag has_%weapon}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag go_loot_%weapon}
								}
							}
							{amount 1
							}
						}
						{tag_add looted}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag has_%weapon}
									}
								}
							}
							{sort
								{type entity}
								{entity
									{tag go_loot_%weapon}
								}
							}
							{amount 1
							}
						}
						{tag_remove has_%weapon}
					}
					{"trigger"
						{name "hf inf ai/looting/no more %weapon in body"}
					}
				}
			}
			{"looting/looted the %weapon" ;// AI has taken the weapon, de-tag the guy
				{condition
					{terms
						{"1.inventory"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag go_loot_%weapon}
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
										{state
											{state user_control}
										}
									}
								}
							}
							{where "all holders"}
							{item "%weapon"}
						}
					}
				}
				{actions
					{"delay"
						{time 0.25}
					}
					{"talk"
						{selector
							{tag go_loot_%weapon}
						}
						{say ill_take_it}
						{text ""}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag go_loot_%weapon}
									}
								}
							}
						}
						{tag_remove go_loot_%weapon}

					}
					{"delay"
						{time 0.1}
					}
				}
			}
)

;//signallers
("looting_weapon" weapon(flare_pistol_mk5) type(weapon))
("looting_weapon" weapon(kampfpistole) type(weapon))
("looting_weapon" weapon(osp_30) type(weapon))
("looting_weapon" weapon(leuchtpistole_34) type(weapon))
("looting_weapon" weapon(flaregun_anm8) type(weapon))

;//bazookas
("looting_weapon" weapon(piat) type(weapon))
("looting_weapon" weapon(faustpatrone_30) type(weapon))
("looting_weapon" weapon(panzerfaust_30) type(weapon))
("looting_weapon" weapon(panzerfaust_30_fin) type(weapon))
("looting_weapon" weapon(panzerfaust_60) type(weapon))
("looting_weapon" weapon(panzerfaust_100) type(weapon))
("looting_weapon" weapon(panzershreck_43) type(weapon))
("looting_weapon" weapon(panzershreck_54) type(weapon))
("looting_weapon" weapon(88mm_rakh) type(weapon))
("looting_weapon" weapon(bazooka_m1a1) type(weapon))
("looting_weapon" weapon(bazooka_m9) type(weapon))
("looting_weapon" weapon(bazooka_m9a1) type(weapon))
("looting_weapon" weapon(recoilless_m18) type(weapon))

;//MGuns
("looting_weapon" weapon(bren_mk1) type(weapon))
("looting_weapon" weapon(bren_mk1_drum) type(weapon))
("looting_weapon" weapon(bren_mk2) type(weapon))
("looting_weapon" weapon(bren_mk2_drum) type(weapon))
("looting_weapon" weapon(bren_mk3) type(weapon))
("looting_weapon" weapon(bren_303_mk2) type(weapon))
("looting_weapon" weapon(fm_24_29) type(weapon))
("looting_weapon" weapon(l33_36) type(weapon))
("looting_weapon" weapon(lewis_gun) type(weapon))
("looting_weapon" weapon(dp_bipod) type(weapon))
("looting_weapon" weapon(dt_bipod) type(weapon))
("looting_weapon" weapon(dt) type(weapon))
("looting_weapon" weapon(mg120r_bipod) type(weapon))
("looting_weapon" weapon(m26) type(weapon))
("looting_weapon" weapon(madsen) type(weapon))
("looting_weapon" weapon(mg13_no_bipod) type(weapon))
("looting_weapon" weapon(mg13_bipod) type(weapon))
("looting_weapon" weapon(mg13_no_bipod_drum) type(weapon))
("looting_weapon" weapon(mg13_bipod_drum) type(weapon))
("looting_weapon" weapon(mg15_bipod) type(weapon))
("looting_weapon" weapon(mg26t) type(weapon))
("looting_weapon" weapon(mg34_belt) type(weapon))
("looting_weapon" weapon(mg34_belt_long) type(weapon))
("looting_weapon" weapon(mg42_belt) type(weapon))
("looting_weapon" weapon(mg42_belt_long) type(weapon))
("looting_weapon" weapon(mg34_drum) type(weapon))
("looting_weapon" weapon(mg34_doubledrum) type(weapon))
("looting_weapon" weapon(mg42_drum))
("looting_weapon" weapon(pk21))

;//SMGuns
("looting_weapon" weapon(lanchester_mk1) type(weapon))
("looting_weapon" weapon(lanchester_mk1_2) type(weapon))
("looting_weapon" weapon(patchett) type(weapon))
("looting_weapon" weapon(sten_mk1) type(weapon))
("looting_weapon" weapon(sten_mk2) type(weapon))
("looting_weapon" weapon(sten_mk2s) type(weapon))
("looting_weapon" weapon(sten_mk2_can) type(weapon))
("looting_weapon" weapon(sten_mk3) type(weapon))
("looting_weapon" weapon(sten_mk5) type(weapon))
("looting_weapon" weapon(sten_mk6) type(weapon))
("looting_weapon" weapon(bergmann_m20) type(weapon))
("looting_weapon" weapon(mp28) type(weapon))
("looting_weapon" weapon(mp38_42_20) type(weapon))
("looting_weapon" weapon(mp38_42_40) type(weapon))
("looting_weapon" weapon(mp717r_drum) type(weapon))
("looting_weapon" weapon(mp38_folded) type(weapon))
("looting_weapon" weapon(mp38_unfolded) type(weapon))
("looting_weapon" weapon(mp40_folded) type(weapon))
("looting_weapon" weapon(mp40_unfolded) type(weapon))
("looting_weapon" weapon(mp41) type(weapon))
("looting_weapon" weapon(ppd40) type(weapon))
("looting_weapon" weapon(pps43) type(weapon))
("looting_weapon" weapon(ppsh41_drum) type(weapon))
("looting_weapon" weapon(ppsh41_stick) type(weapon))
("looting_weapon" weapon(suomi_m31_drum) type(weapon))
("looting_weapon" weapon(suomi_m31_sjr_drum) type(weapon))
("looting_weapon" weapon(suomi_m31_sjr_stick) type(weapon))
("looting_weapon" weapon(suomi_m31_stick) type(weapon))
("looting_weapon" weapon(thompson_m1a1_30) type(weapon))
("looting_weapon" weapon(thompson_m1_30) type(weapon))
("looting_weapon" weapon(thompson_m1928a1_30) type(weapon))
("looting_weapon" weapon(thompson_m1928a1_50) type(weapon))
("looting_weapon" weapon(thompson_m1928_30) type(weapon))
("looting_weapon" weapon(thompson_m1928_50) type(weapon))

;//Cool rifles
("looting_weapon" weapon(avt40) type(weapon))
("looting_weapon" weapon(bar) type(weapon))
("looting_weapon" weapon(bar_bipod) type(weapon))
("looting_weapon" weapon(fg42) type(weapon))
("looting_weapon" weapon(fg42_scope) type(weapon))
("looting_weapon" weapon(g41) type(weapon))
("looting_weapon" weapon(g43) type(weapon))
("looting_weapon" weapon(mkb42h) type(weapon))
("looting_weapon" weapon(stg44) type(weapon))
("looting_weapon" weapon(svt38) type(weapon))
("looting_weapon" weapon(svt40) type(weapon))

;//Rifle grenades
("looting_weapon" weapon(enfield_no1_mk3_grenade_launcher) type(weapon))
("looting_weapon" weapon(enfield_no4_mk1_grenade_launcher) type(weapon))
("looting_weapon" weapon(k98_antitank) type(weapon))
("looting_weapon" weapon(k98_grenade_launcher) type(weapon))
("looting_weapon" weapon(m1a1_carbine_grenade_launcher) type(weapon))
("looting_weapon" weapon(m1_carbine_grenade_launcher) type(weapon))
("looting_weapon" weapon(m1903_grenade_launcher) type(weapon))
("looting_weapon" weapon(mosin_antitank) type(weapon))
("looting_weapon" weapon(mosin_grenade_launcher) type(weapon))

;//ATR
("looting_weapon" weapon(boys_mk1) type(weapon))
("looting_weapon" weapon(ptrd) type(weapon))
("looting_weapon" weapon(ptrs) type(weapon))
("looting_weapon" weapon(pzb_39) type(weapon))

;//AT Grenades
("looting_weapon" weapon(hafthohlladung) type(grenade))
("looting_weapon" weapon(m7) type(grenade))
("looting_weapon" weapon(m24_geballteladung) type(grenade))
("looting_weapon" weapon(m32_satchel) type(grenade))
("looting_weapon" weapon(no73) type(grenade))
("looting_weapon" weapon(no75) type(grenade))
("looting_weapon" weapon(pwm_l) type(grenade))
("looting_weapon" weapon(rpg40) type(grenade))
("looting_weapon" weapon(rpg43) type(grenade))
("looting_weapon" weapon(molotov_bundle_fin) type(grenade))
("looting_weapon" weapon(molotov_cocktail) type(grenade))
("looting_weapon" weapon(molotov_cocktail_fin) type(grenade))

;//Mine clearing
			{"hf inf ai/mine clearing/clear mines"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag mine_planted}
										}
									}
									{exclude
										{tag
											{tag cmp_def}
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
											{tag ai_using_mine_detector}
										}
									}
								}
							}
						}
						{"3.cmp_i"
							{var "user_is_defender$"}
							{op "=="}
							{value 1}
						}
					}
				}
				{actions
					{"delay"
						{time 0.5}
					}
					{"actor_state"
						{selector
							{tag ai_using_mine_detector}
						}
						{fire_mode hold}
						{ai_move
							{mode disable}
						}
						{movement
							{speed fast}
						}
						{ai
							{follow_commander off}
						}
					}
					{"loop" 6
						{"entity_state"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag mine_planted}
										}
									}
									{exclude
										{tag
											{tag selected_for_clearing}
										}
										{state
											{state inactive}
										}
										{tag
											{tag mine_to_clear}
										}
									}
								}
								{sort
									{type entity}
									{entity
										{source advanced}
										{group
											{select
												{tag
													{tag mine_to_clear}
												}
											}
											{exclude
												{state
													{state inactive}
												}
											}
										}
										{amount 1
										}
									}
								}
								{amount 1
								}
							}
							{tag_add mine_to_clear}
						}
						{"delay"
							{time 0.5}
						}
						{"action"
							{selector
								{amount 1
								}
								{tag ai_using_mine_detector}
							}
							{action advance}
							{target
								{source advanced}
								{group
									{select
										{tag
											{tag mine_to_clear}
										}
									}
									{exclude
										{tag
											{tag selected_for_clearing}
										}
										{state
											{state inactive}
										}
									}
								}
								{amount 1
								}
								{sort
									{type entity}
									{entity
										{amount 1
										}
										{tag ai_using_mine_detector}
									}
								}
							}
						}
						{"delay"
							{time 0.5}
						}
						{"entity_state"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag mine_to_clear}
										}
									}
									{exclude
										{state
											{state inactive}
										}
									}
								}
							}
							{tag_add selected_for_clearing}
							{tag_remove mine_to_clear}
						}
						{"delay"
							{time 0.5}
						}
					}
					{"delay"
						{time 0.5}
					}
					{"entity_state"
						{selector
							{tag selected_for_clearing}
						}
						{tag_remove selected_for_clearing}
					}
					{"delay"
						{time 10}
					}
					{"trigger"
						{name "hf inf ai/mine clearing/clear mines"}
					}
				}
			}
			{"hf inf ai/mine clearing/clear mines tag"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag bot}
										}
									}
									{include
										{with_item
											{type amount}
											{item "mine_detector"}
											{min 1}
										}
									}
									{exclude
										{state
											{state "dead inactive linked user_control"}
										}
									}
								}
							}
						}
						{"2.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag bot}
										}
									}
									{include
										{with_item
											{type amount}
											{item "mine_detector"}
											{min 1}
										}
									}
									{exclude
										{state
											{state "dead inactive user_control"}
										}
										{tag}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{select
										{tag
											{tag mine_planted}
										}
									}
								}
							}
							{distance 140}
						}
						{"3.cmp_i"
							{var "user_is_defender$"}
							{op "=="}
							{value 1}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"select"
						{source advanced}
						{group
							{select
								{tag
									{tag bot}
								}
							}
							{include
								{with_item
									{type amount}
									{item "mine_detector"}
									{min 1}
								}
							}
							{exclude
								{tag
									{tag ai_using_mine_detector}
								}
							}
						}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source current}
						}
						{tag_add ai_mine_detector}
					}
					{"entity_state"
						{selector
							{source current}
						}
						{tag_add _lua_mi}
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
										{tag ai_mine_detector}
									}
								}
								{exclude
									{tag
										{tag ai_using_mine_detector}
									}
								}
							}
						}
						{action take}
						{item "mine_detector"}
					}
					{"delay"
						{time 2}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag ai_mine_detector}
									}
								}
								{include
									{with_item
										{type using}
										{item "mine_detector"}
									}
								}
							}
						}
						{tag_add ai_using_mine_detector}
						{tag_remove ai_mine_detector}
					}
					{"delay"
						{time 10}
					}
					{"trigger"
						{name "hf inf ai/mine clearing/clear mines tag"}
					}
				}
			}
			{"hf inf ai/mine clearing/clear mines detag"
				{condition
					{terms
						{"1.entities"
							{selector
								{tag ai_using_mine_detector}
							}
						}
						{"2.entities"
							{selector
								{tag mine_planted}
							}
							{count
								{op "=="}
								{value 0}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"action"
						{selector
							{tag ai_using_mine_detector}
						}
						{drop orders}
						{action cancel}
					}
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{selector
							{tag ai_using_mine_detector}
						}
						{drop orders}
						{control AI}
						{fire_mode open}
						{move_mode free}
						{ai_move
							{mode enable}
						}
						{movement
							{speed assault}
						}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag ai_mine_detector}
									}
								}
							}
						}
						{tag_remove ai_mine_detector}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag ai_using_mine_detector}
									}
								}
							}
						}
						{tag_remove _lua_mi}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag ai_using_mine_detector}
									}
								}
							}
						}
						{tag_remove ai_using_mine_detector}
					}
					{"delay"
						{time 10}
					}
					{"trigger"
						{name "hf inf ai/mine clearing/clear mines detag"}
					}
				}
			}

;// Arty AI
			{"hf arty ai/part 1"
				{condition
					{expression "!1 & !2"}
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{include
										{tag
											{tag bot_arty}
										}
									}
									{exclude
										{tag
											{tag bot_arty_move}
										}
										{tag
											{tag bot_arty_fire}
										}
										{state
											{state linked}
										}
										{tag
											{tag bot_arty_skedaddle}
										}
										{tag
											{tag range_less_than_320}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{include
										{state
											{state user_control}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag exploded}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag burned}
										}
										{tag
											{tag no_scavenge}
										}
										{tag
											{tag destroyed}
										}
										{tag
											{tag vehicle_destroyed}
										}
										{prop
											{prop human}
										}
									}
								}
							}
							{distance 320}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{include
										{tag
											{tag bot_arty_fire}
										}
									}
									{exclude
										{tag
											{tag bot_arty_skedaddle}
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
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{include
									{tag
										{tag bot_arty}
									}
								}
								{exclude
									{state
										{state "dead inactive user_control"}
									}
									{tag
										{tag bot_arty_skedaddle}
									}
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_add bot_arty_move}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_320}
									}
								}
							}
						}
						{tag_remove bot_arty_fortified}
					}
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{include
									{tag
										{tag bot_arty_move}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_320}
									}
								}
							}
						}
						{drop "orders sensor"}
						{movement
							{speed fast}
						}
						{cannon move}
					}
					{"delay"
						{time 3.5}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{drop orders}
						{action move}
						{target
							{source advanced}
							{group
								{include
									{state
										{state user_control}
									}
									{prop
										{prop vehicle}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 60}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_remove bot_arty_move}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "hf arty ai/part 1"}
					}
				}
			}
			{"hf arty ai/part 1.1"
				{condition
					{expression "!1 & !2"}
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{include
										{tag
											{tag bot_arty}
										}
										{tag
											{tag range_less_than_320}
										}
									}
									{exclude
										{tag
											{tag bot_arty_move}
										}
										{tag
											{tag bot_arty_fire}
										}
										{state
											{state linked}
										}
										{tag
											{tag bot_arty_skedaddle}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{include
										{state
											{state user_control}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag exploded}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag burned}
										}
										{tag
											{tag no_scavenge}
										}
										{tag
											{tag destroyed}
										}
										{tag
											{tag vehicle_destroyed}
										}
										{prop
											{prop human}
										}
									}
								}
							}
							{distance 260}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{include
										{tag
											{tag bot_arty_fire}
										}
									}
									{exclude
										{tag
											{tag bot_arty_skedaddle}
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
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{include
									{tag
										{tag bot_arty}
									}
									{tag
										{tag range_less_than_320}
									}
								}
								{exclude
									{state
										{state "dead inactive user_control"}
									}
									{tag
										{tag bot_arty_skedaddle}
									}
								}
							}
						}
						{tag_add bot_arty_move}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{tag_remove bot_arty_fortified}
					}
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{drop "orders sensor"}
						{movement
							{speed fast}
						}
						{cannon move}
					}
					{"delay"
						{time 3.5}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{drop orders}
						{action move}
						{target
							{source advanced}
							{group
								{include
									{state
										{state user_control}
									}
									{prop
										{prop vehicle}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 60}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{tag_remove bot_arty_move}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "hf arty ai/part 1.1"}
					}
				}
			}
			{"hf arty ai/part 1.2"
				{condition
					{expression "!1 & !2"}
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{include
										{tag
											{tag bot_arty}
										}
										{tag
											{tag range_more_than_400}
										}
									}
									{exclude
										{tag
											{tag bot_arty_move}
										}
										{tag
											{tag bot_arty_fire}
										}
										{state
											{state linked}
										}
										{tag
											{tag bot_arty_skedaddle}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{include
										{state
											{state user_control}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag exploded}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag burned}
										}
										{tag
											{tag no_scavenge}
										}
										{tag
											{tag destroyed}
										}
										{tag
											{tag vehicle_destroyed}
										}
										{prop
											{prop human}
										}
									}
								}
							}
							{distance 405}
						}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{include
										{tag
											{tag bot_arty_fire}
										}
									}
									{exclude
										{tag
											{tag bot_arty_skedaddle}
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
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{include
									{tag
										{tag bot_arty}
									}
									{tag
										{tag range_more_than_400}
									}
								}
								{exclude
									{state
										{state "dead inactive user_control"}
									}
									{tag
										{tag bot_arty_skedaddle}
									}
								}
							}
						}
						{tag_add bot_arty_move}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_remove bot_arty_fortified}
					}
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{drop "orders sensor"}
						{movement
							{speed fast}
						}
						{cannon move}
					}
					{"delay"
						{time 3.5}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{drop orders}
						{action move}
						{target
							{source advanced}
							{group
								{include
									{state
										{state user_control}
									}
									{prop
										{prop vehicle}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 60}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_move}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_remove bot_arty_move}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "hf arty ai/part 1.2"}
					}
				}
			}
			{"hf arty ai/part 2"
				{condition
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag bot_arty}
										}
									}
									{exclude
										{tag
											{tag bot_arty_skedaddle}
										}
										{tag
											{tag range_less_than_320}
										}
										{tag
											{tag range_more_than_400}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{include
										{state
											{state user_control}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag exploded}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag burned}
										}
										{tag
											{tag no_scavenge}
										}
										{tag
											{tag destroyed}
										}
										{tag
											{tag vehicle_destroyed}
										}
										{prop
											{prop human}
										}
									}
								}
							}
							{distance 310}
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
							{group
								{select
									{tag
										{tag bot_arty}
									}
								}
								{exclude
									{tag
										{tag bot_arty_skedaddle}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_add bot_arty_fire}
						{tag_remove bot_arty_move}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{drop orders}
						{move_mode hold}
						{cannon fire}
					}
					{"delay"
						{time 4}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag bot_arty_fortified}
									}
									{tag
										{tag "+mobilegun"}
									}
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{effect construction_sandbag_cannon_standard}
					}
					{"delay"
						{time 6}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{state
										{state "dead inactive"}
									}
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_add bot_arty_fortified}
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
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{action rotate}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 0.25}
					}
					{"actor_fire"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
						{interval 0}
						{time 30}
					}
					{"delay"
						{time 45}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{action suppression_fire}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 45}
					}
					{"actor_fire"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
						{interval 0}
						{time 30}
					}
					{"delay"
						{time 45}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{action suppression_fire}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 45}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{exclude
									{tag
										{tag range_less_than_320}
									}
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "hf arty ai/part 2"}
					}
				}
			}
			{"hf arty ai/part 2.1"
				{condition
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag bot_arty}
										}
									}
									{include
										{tag
											{tag range_less_than_320}
										}
									}
									{exclude
										{tag
											{tag bot_arty_skedaddle}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{include
										{state
											{state user_control}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag exploded}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag burned}
										}
										{tag
											{tag no_scavenge}
										}
										{tag
											{tag destroyed}
										}
										{tag
											{tag vehicle_destroyed}
										}
										{prop
											{prop human}
										}
									}
								}
							}
							{distance 250}
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
							{group
								{select
									{tag
										{tag bot_arty}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
								{exclude
									{tag
										{tag bot_arty_skedaddle}
									}
									{state
										{state "dead inactive"}
									}
								}
							}
						}
						{tag_add bot_arty_fire}
						{tag_remove bot_arty_move}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{drop orders}
						{move_mode hold}
						{cannon fire}
					}
					{"delay"
						{time 4}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
								{exclude
									{tag
										{tag bot_arty_fortified}
									}
									{tag
										{tag "+mobilegun"}
									}
								}
							}
						}
						{effect construction_sandbag_cannon_standard}
					}
					{"delay"
						{time 6}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
								{exclude
									{state
										{state "dead inactive"}
									}
								}
							}
						}
						{tag_add bot_arty_fortified}
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
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{action rotate}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 0.25}
					}
					{"actor_fire"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
						{interval 0}
						{time 30}
					}
					{"delay"
						{time 45}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{action suppression_fire}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 45}
					}
					{"actor_fire"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
						{interval 0}
						{time 30}
					}
					{"delay"
						{time 45}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{action suppression_fire}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 60}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_less_than_320}
									}
								}
							}
						}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "hf arty ai/part 2.1"}
					}
				}
			}
			{"hf arty ai/part 2.2"
				{condition
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag bot_arty}
										}
									}
									{include
										{tag
											{tag range_more_than_400}
										}
									}
									{exclude
										{tag
											{tag bot_arty_skedaddle}
										}
									}
								}
							}
							{near_to
								{source advanced}
								{group
									{include
										{state
											{state user_control}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag exploded}
										}
										{tag
											{tag ammo_cooked}
										}
										{tag
											{tag burned}
										}
										{tag
											{tag no_scavenge}
										}
										{tag
											{tag destroyed}
										}
										{tag
											{tag vehicle_destroyed}
										}
										{prop
											{prop human}
										}
									}
								}
							}
							{distance 400}
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
							{group
								{select
									{tag
										{tag bot_arty}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
								{exclude
									{tag
										{tag bot_arty_skedaddle}
									}
									{state
										{state "dead inactive"}
									}
								}
							}
						}
						{tag_add bot_arty_fire}
						{tag_remove bot_arty_move}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{drop orders}
						{move_mode hold}
						{cannon fire}
					}
					{"delay"
						{time 4}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
								{exclude
									{tag
										{tag bot_arty_fortified}
									}
									{tag
										{tag "+mobilegun"}
									}
								}
							}
						}
						{effect construction_sandbag_cannon_standard}
					}
					{"delay"
						{time 6}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
								{exclude
									{state
										{state "dead inactive"}
									}
								}
							}
						}
						{tag_add bot_arty_fortified}
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
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{action rotate}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 0.25}
					}
					{"actor_fire"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
						{interval 0}
						{time 30}
					}
					{"delay"
						{time 45}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{action suppression_fire}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 45}
					}
					{"actor_fire"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
						{interval 0}
						{time 30}
					}
					{"delay"
						{time 45}
					}
					{"action"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{action suppression_fire}
						{target
							{source advanced}
							{group
								{include
									{tag
										{tag _user}
									}
								}
								{exclude
									{prop
										{prop human}
									}
									{state
										{state "dead inactive"}
									}
									{tag
										{tag exploded}
									}
									{tag
										{tag ammo_cooked}
									}
									{tag
										{tag burned}
									}
									{tag
										{tag no_scavenge}
									}
									{tag
										{tag destroyed}
									}
									{tag
										{tag vehicle_destroyed}
									}
								}
							}
						}
					}
					{"delay"
						{time 60}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_fire}
									}
								}
								{include
									{tag
										{tag range_more_than_400}
									}
								}
							}
						}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "hf arty ai/part 2.2"}
					}
				}
			}
			{"hf arty ai/part 3"
				{condition
					{expression "1 & 2 & !3"}
					{terms
						{"1.entities"
							{selector
								{tag bot_arty}
							}
						}
						{"2.entities"
							{selector
								{tag bot_aa}
							}
						}
						{"3.entities"
							{selector
								{tag bot_aa_guard}
								{state "not dead"}
							}
							{count
								{op ">="}
								{value 2}
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
							{amount 1
							}
							{tag bot_aa}
						}
						{tag_add bot_aa_guard}
					}
					{"entity_state"
						{selector
							{amount 1
							}
							{tag bot_aa_guard}
						}
						{tag_add bot_aa_guard_move}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{amount 1
							}
							{tag bot_arty}
							{state "not dead"}
						}
						{tag_add bot_arty_guard}
					}
					{"delay"
						{time 2}
					}
					{"actor_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag bot_aa_guard}
									}
								}
								{exclude
									{state
										{state "dead inactive user_control"}
									}
									{tag
										{tag bot_aa_guard_setlled}
									}
								}
							}
						}
						{drop "orders sensor"}
						{move_mode hold}
						{ai_move
							{mode disable}
							{enable_on enemy}
						}
						{movement
							{speed fast}
						}
						{cannon move}
					}
					{"delay"
						{time 2}
					}
					{"action"
						{selector
							{amount 1
							}
							{tag bot_aa_guard_move}
							{state "not dead"}
						}
						{drop orders}
						{action move}
						{target
							{source advanced}
							{group
								{select
									{tag
										{tag bot_arty_guard}
									}
								}
								{exclude
									{state
										{state "dead inactive user_control"}
									}
								}
							}
							{amount 1
							}
						}
					}
					{"delay"
						{time 30}
					}
					{"trigger"
						{name "hf arty ai/part 3"}
					}
				}
			}
			{"hf arty ai/part 4"
				{condition
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag bot_aa_guard}
										}
									}
									{exclude
										{state
											{state "dead inactive user_control"}
										}
									}
								}
								{amount 1
								}
							}
							{near_to
								{amount 1
								}
								{tag bot_arty_guard}
							}
							{distance 30}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{tag bot_aa_guard_move}
						}
						{tag_remove bot_aa_guard_move}
					}
					{"actor_state"
						{selector
							{amount 1
							}
							{tag bot_aa_guard}
							{state "not dead"}
						}
						{drop orders}
						{move_mode hold}
						{ai_move
							{mode disable}
							{enable_on enemy}
						}
						{cannon fire}
					}
					{"delay"
						{time 0.1}
					}
					{"effect"
						{selector
							{amount 1
							}
							{tag bot_aa_guard}
						}
						{effect engine_off}
					}
					{"delay"
						{time 10}
					}
					{"trigger"
						{name "hf arty ai/part 4"}
					}
				}
			}
			{"hf arty ai/part 5"
				{condition
					{expression "!1 & 2"}
					{terms
						{"1.near"
							{units
								{amount 1
								}
								{tag bot_aa_guard}
								{state "not dead"}
							}
							{near_to
								{amount 1
								}
								{tag bot_arty_guard}
							}
							{distance 35}
						}
						{"2.near"
							{units
								{amount 1
								}
								{tag bot_aa_guard}
							}
							{near_to
								{amount 1
								}
								{tag bot_arty_guard}
							}
							{distance 50}
						}
					}
				}
				{actions
					{"delay"
						{time 2}
					}
					{"effect"
						{selector
							{tag bot_aa_guard_move}
						}
						{effect engine_on}
					}
					{"delay"
						{time 1}
					}
					{"action"
						{selector
							{amount 1
							}
							{tag bot_aa_guard_move}
							{state "not dead"}
						}
						{drop orders}
						{action move}
						{target
							{amount 1
							}
							{tag bot_arty_guard}
							{state "not dead"}
						}
					}
					{"delay"
						{time 10}
					}
					{"trigger"
						{name "hf arty ai/part 5"}
					}
				}
			}
			
;// Medic AI
			;// One medic claims one downed body so the whole squad does not stack on a single rez.
			{"hf medic ai/run towards nearest wounded"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag bleeding}
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
											{tag medic_claimed}
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
											{tag bot_medic}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{state
											{state user_control}
										}
										{tag
											{tag _user}
										}
										{tag
											{tag medic_busy}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"loop" 8
						{"entity_state"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag bot_medic}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{state
											{state user_control}
										}
										{tag
											{tag _user}
										}
										{tag
											{tag medic_busy}
										}
										{tag
											{tag medic_assign}
										}
									}
								}
								{sort
									{type entity}
									{entity
										{source advanced}
										{group
											{select
												{tag
													{tag bleeding}
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
													{tag medic_claimed}
												}
												{tag
													{tag medic_to_rez}
												}
											}
										}
									}
								}
								{amount 1
								}
							}
							{tag_add medic_assign}
						}
						{"delay"
							{time 0.1}
						}
						{"entity_state"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag bleeding}
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
											{tag medic_claimed}
										}
										{tag
											{tag medic_to_rez}
										}
									}
								}
								{sort
									{type entity}
									{entity
										{amount 1
										}
										{tag medic_assign}
									}
								}
								{amount 1
								}
							}
							{tag_add medic_to_rez}
						}
						{"delay"
							{time 0.1}
						}
						{"entity_state"
							{selector
								{tag medic_assign}
							}
							{tag_add _lua_mi}
						}
						{"actor_state"
							{selector
								{tag medic_assign}
								{type human}
								{state "not dead"}
							}
							{fire_mode return}
							{movement
								{speed fast}
							}
						}
						{"delay"
							{time 0.1}
						}
						{"action"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag medic_assign}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{state
											{state user_control}
										}
									}
								}
								{amount 1
								}
							}
							{drop "orders sensor"}
							{action move}
							{target
								{source advanced}
								{group
									{select
										{tag
											{tag medic_to_rez}
										}
									}
									{exclude
										{state
											{state user_control}
										}
										{tag
											{tag _user}
										}
									}
								}
								{amount 1
								}
								{sort
									{type entity}
									{entity
										{amount 1
										}
										{tag medic_assign}
									}
								}
							}
						}
						{"delay"
							{time 0.1}
						}
						{"entity_state"
							{selector
								{tag medic_to_rez}
							}
							{tag_add medic_claimed}
							{tag_remove medic_to_rez}
						}
						{"entity_state"
							{selector
								{tag medic_assign}
							}
							{tag_add medic_busy}
							{tag_remove medic_assign}
						}
						{"delay"
							{time 0.1}
						}
					}
					{"delay"
						{time 20}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag medic_claimed}
									}
								}
								{exclude
									{tag
										{tag bleeding}
									}
								}
							}
						}
						{tag_remove medic_claimed}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag medic_busy}
									}
								}
								{include
									{state
										{state "dead inactive"}
									}
								}
							}
						}
						{tag_remove medic_busy}
					}
					{"entity_state"
						{selector
							{tag medic_busy}
						}
						{tag_remove medic_busy}
					}
					{"entity_state"
						{selector
							{tag medic_claimed}
						}
						{tag_remove medic_claimed}
					}
					{"entity_state"
						{selector
							{tag medic_assign}
						}
						{tag_remove medic_assign}
					}
					{"entity_state"
						{selector
							{tag medic_to_rez}
						}
						{tag_remove medic_to_rez}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "hf medic ai/run towards nearest wounded"}
					}
				}
			}
			{"hf medic ai/medic smoke"
				{condition
					{terms
						{"1.see_actors"
							{selector
								{tag bot_medic}
							}
							{enemy
								{source advanced}
								{group
									{select
										{tag
											{tag bleeding}
										}
									}
									{exclude
										{state
											{state user_control}
										}
										{tag
											{tag _user}
										}
									}
								}
							}
							{distance
								{mode in_range}
								{min 14}
								{max 21}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" medic_smoke}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{selector
							{tag medic_smoke}
							{state "not dead"}
						}
						{drop orders}
						{fire_mode hold}
					}
					{"inventory"
						{selector
							{tag medic_smoke}
							{state "not dead"}
						}
						{action add}
						{item "nbk39" "grenade"}
					}
					{"delay"
						{time 0.2}
					}
					{"inventory"
						{selector
							{tag medic_smoke}
							{type human}
							{state "not dead"}
						}
						{action take}
						{item "nbk39" "grenade"}
					}
					{"delay"
						{time 1}
					}
					{"actor_state"
						{selector
							{tag medic_smoke}
							{state "not dead"}
						}
						{fire_mode hold}
						{move_mode hold}
					}
					{"action"
						{selector
							{tag medic_smoke}
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
										{tag bleeding}
									}
								}
								{exclude
									{state
										{state user_control}
									}
									{tag
										{tag _user}
									}
								}
							}
						}
					}
					{"delay"
						{time 5}
					}
					{"action"
						{selector
							{tag medic_smoke}
							{state "not dead"}
						}
						{drop orders}
						{action cancel}
						{target
							{ignore_captured_by_user 0}
						}
					}
					{"delay"
						{time 0.1}
					}
					{"actor_state"
						{selector
							{tag medic_smoke}
							{state "not dead"}
						}
						{drop orders}
						{fire_mode return}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{tag bot_medic}
							{state "not dead"}
						}
						{tag_remove medic_smoke}
					}
					{"delay"
						{time 20}
					}
					{"trigger"
						{name "hf medic ai/medic smoke"}
					}
				}
			}

;// WIP Flanking AI
			{"hf flanking ai/detect and action"
				{condition
					{terms
						{"1.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag _bot}
										}
									}
									{include
										{prop
											{prop human}
										}
									}
									{exclude
										{prop
											{prop vehicle}
										}
										{prop
											{prop tank}
										}
										{prop
											{prop car}
										}
										{state
											{state "dead inactive"}
										}
									}
								}
								{amount 5
								}
							}
							{near_to
								{source advanced}
								{group
									{select
										{tag
											{tag _bot_apc}
										}
									}
									{include
										{state
											{state "operatable inhabited"}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag bot_flank_finished}
										}
									}
								}
							}
							{distance 10}
						}
						{"2.near"
							{units
								{source advanced}
								{group
									{select
										{tag
											{tag _bot}
										}
									}
									{include
										{prop
											{prop human}
										}
									}
									{exclude
										{prop
											{prop car}
										}
										{prop
											{prop vehicle}
										}
										{prop
											{prop tank}
										}
										{state
											{state "dead inactive"}
										}
									}
								}
								{amount 3
								}
							}
							{near_to
								{source advanced}
								{group
									{select
										{tag
											{tag bot_car}
										}
									}
									{include
										{state
											{state "operatable inhabited"}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
										}
										{tag
											{tag bot_flank_finished}
										}
									}
								}
							}
							{distance 10}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag _bot_apc}
									}
								}
								{include
									{state
										{state "operatable inhabited"}
									}
								}
								{exclude
									{state
										{state "dead inactive"}
									}
								}
							}
						}
						{tag_add bot_mounted}
					}
					{"delay"
						{time 0.1}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag bot_mounted}
											}
										}
									}
									{amount 2
									}
								}
								{tag_add bot_flanking}
							}
							{"delay"
								{time 0.1}
							}
							{"entity_state"
								{selector
									{tag bot_flanking}
									{state "not dead"}
								}
								{tag_add _lua_mi}
							}
							{"delay"
								{time 0.1}
							}
							{"actor_state"
								{selector
									{tag bot_flanking}
								}
								{fire_mode return}
								{ai_move
									{mode disable}
									{enable_on "injure enemy"}
								}
								{movement
									{speed fast}
								}
								{ai
									{follow_commander off}
								}
							}
							{"delay"
								{time 0.1}
							}
							{"switch"
								{"case"
									{condition
										{type rand}
										{value 0.25}
									}
									{"actor_to_waypoint"
										{selector
											{amount 1
											}
											{tag bot_flanking}
										}
										{drop orders}
										{waypoint "7"}
									}
								}
								{"case"
									{condition
										{type rand}
										{value 0.25}
									}
									{"actor_to_waypoint"
										{selector
											{tag bot_flanking}
										}
										{drop orders}
										{waypoint "7"}
									}
								}
								{"case"
									{condition
										{type rand}
										{value 0.25}
									}
									{"actor_to_waypoint"
										{selector
											{amount 1
											}
											{tag bot_flanking}
										}
										{drop orders}
										{waypoint "8"}
									}
								}
								{"default"
									{"actor_to_waypoint"
										{selector
											{tag bot_flanking}
										}
										{drop orders}
										{waypoint "8"}
									}
								}
							}
							{"delay"
								{time 0.1}
							}
							{"ables"
								{selector
									{source advanced}
									{group
										{select
											{tag
												{tag bot_flanking}
											}
										}
										{exclude
											{tag
												{tag bot_car}
											}
										}
									}
								}
								{remove emit_passangers_on_enemy}
							}
						}
						{"default"
							{"actor_state"
								{selector
									{tag bot_mounted}
								}
								{movement
									{speed_kph 20}
								}
							}
						}
					}
					{"delay"
						{time 120}
					}
					{"trigger"
						{name "hf flanking ai/detect and action"}
					}
				}
			}

;// Melee AI	
			{"hf inf ai/melee/melee"
				{condition
					{terms
						{"1.see_actors"
							{selector
								{source advanced}
								{group
									{include
										{prop
											{prop human}
										}
										{tag
											{tag bot}
										}
										{with_item
											{type existance}
											{item "cav_sword_m1927"}
										}
									}
									{exclude
										{state
											{state "dead inactive user_control"}
										}
										{tag
											{tag _user}
										}
										{tag
											{tag _user_ally}
										}
									}
								}
							}
							{enemy
								{source advanced}
								{group
									{include
										{prop
											{prop human}
										}
										{tag
											{tag _user}
										}
									}
									{exclude
										{state
											{state "dead inactive linked"}
										}
										{tag
											{tag _bleedout_}
										}
										{tag
											{tag bleeding}
										}
										{tag
											{tag bot}
										}
										{movement_type
											{type swim}
										}
										{movement_type
											{type crawl}
										}
										{movement_type
											{type squat}
										}
									}
								}
							}
							{distance
								{mode near_than}
								{meters 10}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" ai_close_inf}
								{"for enemy" ai_close_inf_target}
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
							{tag ai_close_inf}
						}
						{tag_add _lua_mi}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag ai_close_inf}
									}
								}
								{exclude
									{with_item
										{type using}
										{item "cav_sword_m1927"}
									}
								}
							}
						}
						{action take}
						{item "cav_sword_m1927"}
					}
					{"delay"
						{time 0.25}
					}
					{"actor_state"
						{selector
							{tag ai_close_inf}
						}
						{movement
							{speed fast}
							{kind fast}
							{type normal}
						}
						{ai
							{follow_commander off}
						}
					}
					{"delay"
						{time 0.25}
					}
					{"talk"
						{selector
							{tag ai_close_inf}
						}
						{say attack}
						{text ""}
					}
					{"action"
						{selector
							{tag ai_close_inf}
						}
						{drop orders}
						{action advance}
						{target
							{tag ai_close_inf_target}
						}
					}
					{"delay"
						{time 1}
					}
					{"entity_state"
						{selector
							{tag ai_close_inf}
						}
						{tag_remove _lua_mi}
					}
					{"entity_state"
						{selector
							{tag ai_close_inf}
						}
						{tag_remove ai_close_inf}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{tag ai_close_inf_target}
						}
						{tag_remove ai_close_inf_target}
					}
					{"delay"
						{time 0.5}
					}
					{"trigger"
						{name "hf inf ai/melee/melee"}
					}
				}
			}

;// Modified airstrike script, moved from dcg_script.inc
			{"dcg/ai_air_decide"
				{condition
					{expression "(1 & 2 & !3 & !4 & !7 & 5 & 6) | (1 & 2 & !3 & !4 & !5 & 7)"}
					{terms
						{"1.event"
							{id "ai_air_possible"}
						}
						{"2.cmp_i"
							{var "ai_air_on$"}
							{op "=="}
							{value 0}
						}
						{"3.cmp_i"
							{var "bot_difficulty$"}
							{op "=="}
							{value 1}
						}
						{"4.cmp_i"
							{var "bot_difficulty$"}
							{op "=="}
							{value 2}
						}
						{"5.cmp_i"
							{var "bot_difficulty$"}
							{op "=="}
							{value 3}
						}
						{"6.cmp_i"
							{var "tier2$"}
							{op "=="}
							{value 1}
						}
						{"7.cmp_i"
							{var "bot_difficulty$"}
							{op "=="}
							{value 4}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector
									{ignore_captured_by_user 0}
									{tag fpc1}
									{zone "gamezone"}
								}
							}
							{"delay"
								{time 0.1}
							}
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{tag player}
											{zone "fpc1"}
											{type human}
											{state "not dead"}
										}
									}
									{"delay"
										{time 0.1}
									}
									{"set_i"
										{var "ai_air_on$"}
										{op "="}
										{value 1}
									}
								}
								{"default"}
							}
						}
						{"default"}
					}
					{"delay"
						{time 2}
					}
					{"switch"
						{"case"
							{condition
								{type cmp_i}
								{var "ai_air_on$"}
								{op "=="}
								{value 0}
							}
							{"delay"
								{time 0.1}
							}
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{tag fpc2}
											{zone "gamezone"}
										}
									}
									{"delay"
										{time 0.1}
									}
									{"switch"
										{"case"
											{condition
												{type entities}
												{selector
													{ignore_captured_by_user 0}
													{tag player}
													{zone "fpc2"}
													{type human}
													{state "not dead"}
												}
											}
											{"delay"
												{time 0.1}
											}
											{"set_i"
												{var "ai_air_on$"}
												{op "="}
												{value 2}
											}
										}
										{"default"}
									}
								}
								{"default"}
							}
						}
						{"default"}
					}
					{"delay"
						{time 2}
					}
					{"switch"
						{"case"
							{condition
								{type cmp_i}
								{var "ai_air_on$"}
								{op "=="}
								{value 0}
							}
							{"delay"
								{time 0.1}
							}
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{tag fpc3}
											{zone "gamezone"}
										}
									}
									{"delay"
										{time 0.1}
									}
									{"switch"
										{"case"
											{condition
												{type entities}
												{selector
													{ignore_captured_by_user 0}
													{tag player}
													{zone "fpc3"}
													{type human}
													{state "not dead"}
												}
											}
											{"delay"
												{time 0.1}
											}
											{"set_i"
												{var "ai_air_on$"}
												{op "="}
												{value 3}
											}
										}
										{"default"}
									}
								}
								{"default"}
							}
						}
						{"default"}
					}
					{"delay"
						{time 2}
					}
					{"switch"
						{"case"
							{condition
								{type cmp_i}
								{var "ai_air_on$"}
								{op "=="}
								{value 0}
							}
							{"delay"
								{time 0.1}
							}
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{tag fpc4}
											{zone "gamezone"}
										}
									}
									{"delay"
										{time 0.1}
									}
									{"switch"
										{"case"
											{condition
												{type entities}
												{selector
													{ignore_captured_by_user 0}
													{tag player}
													{zone "fpc4"}
													{type human}
													{state "not dead"}
												}
											}
											{"delay"
												{time 0.1}
											}
											{"set_i"
												{var "ai_air_on$"}
												{op "="}
												{value 4}
											}
										}
										{"default"}
									}
								}
								{"default"}
							}
						}
						{"default"}
					}
					{"delay"
						{time 2}
					}
					{"switch"
						{"case"
							{condition
								{type cmp_i}
								{var "ai_air_on$"}
								{op "=="}
								{value 0}
							}
							{"delay"
								{time 0.1}
							}
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{tag fpc5}
											{zone "gamezone"}
										}
									}
									{"delay"
										{time 0.1}
									}
									{"switch"
										{"case"
											{condition
												{type entities}
												{selector
													{ignore_captured_by_user 0}
													{tag player}
													{zone "fpc5"}
													{type human}
													{state "not dead"}
												}
											}
											{"delay"
												{time 0.1}
											}
											{"set_i"
												{var "ai_air_on$"}
												{op "="}
												{value 5}
											}
										}
										{"default"}
									}
								}
								{"default"}
							}
						}
						{"default"}
					}
					{"delay"
						{time 50}
					}
					{"trigger"
						{name "dcg/ai_air_decide"}
					}
				}
			}
			{"dcg/ai_airstrike"
				{condition
					{expression "(1 & !2 & !3 & !6 & 4 & 5) | (1 & !2 & !3 & !4 & 6)"}
					{terms
						{"1.event"
							{id "ai_air_on"}
						}
						{"2.cmp_i"
							{var "bot_difficulty$"}
							{op "=="}
							{value 1}
						}
						{"3.cmp_i"
							{var "bot_difficulty$"}
							{op "=="}
							{value 2}
						}
						{"4.cmp_i"
							{var "bot_difficulty$"}
							{op "=="}
							{value 3}
						}
						{"5.cmp_i"
							{var "tier2$"}
							{op "=="}
							{value 1}
						}
						{"6.cmp_i"
							{var "bot_difficulty$"}
							{op "=="}
							{value 4}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag enemy_air}
						}
						{tag_remove enemy_air}
					}
					{"delay"
						{time 0.1}
					}
					{"switch"
						{"case"
							{condition
								{type cmp_i}
								{var "user_team$"}
								{op "=="}
								{value 1}
							}
							{"delay"
								{time 0.1}
							}
							{"actor_to_waypoint"
								{selector
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{select
											{tag
												{tag airstrike_ger_250}
											}													
											{tag
												{tag airstrike_rus}
											}
											{tag
												{tag airstrike_rus_500}
											}
											{tag
												{tag airstrike_rus_50}
											}
											{tag
												{tag airstrike_rus_rocket}
											}											
											{tag
												{tag airstrike_fin}
											}
											{tag
												{tag airstrike_usa}
											}
											{tag
												{tag airstrike_usa_rocket_hvar}
											}
											{tag
												{tag airstrike_usa_rocket_m8}
											}
											{tag
												{tag airstrike_eng_250}
											}
											{tag
												{tag airstrike_eng_250_2}
											}	
											{tag
												{tag airstrike_eng_100}
											}	
										}
										{include
											{tag
												{tag hidden}
											}
										}
									}
									{amount 1
									}
								}
								{waypoint "0"}
								{clone}
								{approach "safe teleport & rotate"}
							}
						}
						{"default"
							{"delay"
								{time 0.1}
							}
							{"actor_to_waypoint"
								{selector
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{select
											{tag
												{tag airstrike_ger_250}
											}													
											{tag
												{tag airstrike_rus}
											}
											{tag
												{tag airstrike_rus_500}
											}
											{tag
												{tag airstrike_rus_50}
											}
											{tag
												{tag airstrike_rus_rocket}
											}											
											{tag
												{tag airstrike_fin}
											}
											{tag
												{tag airstrike_usa}
											}
											{tag
												{tag airstrike_usa_rocket_hvar}
											}
											{tag
												{tag airstrike_usa_rocket_m8}
											}		
											{tag
												{tag airstrike_eng_250}
											}
											{tag
												{tag airstrike_eng_250_2}
											}	
											{tag
												{tag airstrike_eng_100}
											}												
										}
										{include
											{tag
												{tag hidden}
											}
										}
									}
									{amount 1
									}
								}
								{waypoint "6"}
								{clone}
								{approach "safe teleport & rotate"}
							}
						}
					}
				}
			}
			{"dcg/ai_airstrike_do"
				{condition
					{expression "(1 & 2 & 4) | (1 & 3 & !4)"}
					{terms
						{"1.event"
							{id "ai_air_on"}
						}
						{"2.entities"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{select
										{zone
											{zone "a_1"}
										}
										{zone
											{zone "a_2"}
										}
										{zone
											{zone "a_3"}
										}
										{zone
											{zone "a_4"}
										}
									}
									{include
										{tag
											{tag enemy_air}
										}
										{prop
											{prop vehicle}
										}
										{state
											{state operatable}
										}
									}
								}
							}
						}
						{"3.entities"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{select
										{zone
											{zone "b_1"}
										}
										{zone
											{zone "b_2"}
										}
										{zone
											{zone "b_3"}
										}
										{zone
											{zone "b_4"}
										}
									}
									{include
										{tag
											{tag enemy_air}
										}
										{prop
											{prop vehicle}
										}
										{state
											{state operatable}
										}
									}
								}
							}
						}
						{"4.cmp_i"
							{var "user_team$"}
							{op "=="}
							{value 1}
						}
					}
				}
				{actions
					{"delay"
						{time 4.5}
					}
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector
									{ignore_captured_by_user 0}
									{tag ai_air_target}
									{zone "gamezone"}
								}
							}
							{"delay"
								{time 0.1}
							}
						}
						{"default"
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
												{tag _user}
											}
										}
										{include
											{zone
												{zone "gamezone"}
											}
											{state
												{state "operatable user_control"}
											}
										}
										{exclude
											{state
												{state inactive}
											}
											{state
												{state linked}
											}
										}
									}
								}
								{tag_add ai_air_target}
							}
						}
					}
					{"delay"
						{time 1.1}
					}
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{select
											{tag
												{tag enemy_air}
											}
										}
										{include
											{tag
												{tag altitude_high}
											}
										}
										{exclude
											{tag
												{tag recon_plane}
											}
										}
									}
								}
							}
							{"delay"
								{time 0.1}
							}
							{"event"
								{event "ai_air_on"}
								{mode reset}
								{delay 0}
							}
							{"air_attack"
								{selector
									{ignore_captured_by_user 0}
									{tag enemy_air}
									{type vehicle}
									{state inhabited}
								}
								{drop "orders sensor senseless"}
								{enemies
									{ignore_captured_by_user 0}
									{tag ai_air_target}
								}
								{attack_altitude 50}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{select
											{tag
												{tag enemy_air}
											}
										}
										{include
											{tag
												{tag altitude_low}
											}
										}
										{exclude
											{tag
												{tag altitude_high}
											}
											{tag
												{tag recon_plane}
											}
										}
									}
								}
							}
							{"delay"
								{time 0.1}
							}
							{"air_attack"
								{selector
									{ignore_captured_by_user 0}
									{tag enemy_air}
									{type vehicle}
									{state inhabited}
								}
								{drop "orders sensor senseless"}
								{enemies
									{ignore_captured_by_user 0}
									{tag ai_air_target}
								}
								{attack_altitude 45}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{select
											{tag
												{tag enemy_air}
											}
										}
										{include
											{tag
												{tag recon_plane}
											}
										}
										{exclude
											{tag
												{tag altitude_high}
											}
											{tag
												{tag altitude_low}
											}											
										}
									}
								}
							}
							{"delay"
								{time 0.2}
							}							
							{"action"
								{selector
									{ignore_captured_by_user 0}
									{tag enemy_air}
									{type vehicle}
									{state inhabited}
								}
								{drop "orders sensor senseless"}
								{action move}
							}
						}
						{"default"
							{"delay"
								{time 0.1}
							}
							{"air_attack"
								{selector
									{ignore_captured_by_user 0}
									{tag enemy_air}
									{type vehicle}
									{state inhabited}
								}
								{drop "orders sensor senseless"}
								{enemies
									{ignore_captured_by_user 0}
									{tag ai_air_target}
								}
								{attack_altitude 50}
							}
						}
					}
				}
			}
			{"dcg/ai_air_chance"
				{condition
					{expression "(1 & !3 & !4 & !7 & 5 & 6) | (1 & !3 & !4 & !5 & 7)"}
					{terms
						{"1.event"
							{id "airplane_set"}
						}
						{"3.cmp_i"
							{var "bot_difficulty$"}
							{op "=="}
							{value 1}
						}
						{"4.cmp_i"
							{var "bot_difficulty$"}
							{op "=="}
							{value 2}
						}
						{"5.cmp_i"
							{var "bot_difficulty$"}
							{op "=="}
							{value 3}
						}
						{"6.cmp_i"
							{var "tier2$"}
							{op "=="}
							{value 1}
						}
						{"7.cmp_i"
							{var "bot_difficulty$"}
							{op "=="}
							{value 4}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"event"
						{event "ai_air_possible"}
						{mode reset}
						{delay 0}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.10}
							}
							{"delay"
								{time 450}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.10}
							}
							{"delay"
								{time 450}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.10}
							}
							{"delay"
								{time 450}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.10}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 450}
							}
						}
					}
					{"delay"
						{time 1}
					}
					{"switch"
						{##"case"
							{condition
								{type rand}
								{value 0.25}
							}
							{"delay"
								{time 0.1}
							}
							{"trigger"
								{name "dcg/ai_air_chance"}
							}
						}
						{"default"
							{"delay"
								{time 0.1}
							}
							{"event"
								{event "ai_air_possible"}
								{mode set}
								{delay 0}
							}
						}
					}
				}
			}
			{"dcg/ai_air_on"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.event"
							{id "dcg"}
						}
						{"2.cmp_i"
							{var "enemyid$"}
							{op ">"}
							{value 0}
						}
						{"3.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag _user}
								{zone "gamezone"}
								{state operatable}
							}
						}
						{"4.cmp_i"
							{var "ai_air_on$"}
							{op ">"}
							{value 0}
						}
						{"5.cmp_i"
							{var "dcg_on$"}
							{op ">"}
							{value 0}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"event"
						{event "ai_air_on"}
						{mode set}
						{delay 0}
					}
					{"delay"
						{time 1}
					}
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{tag ai_air_target}
						}
						{tag_remove ai_air_target}
					}
					{"delay"
						{time 5}
					}
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{select
											{tag
												{tag _user}
											}
										}
										{include
											{zone
												{zone "combat"}
											}
											{prop
												{prop vehicle}
											}											
											{state
												{state operatable}
											}
											{state
												{state user_control}
											}
										}
										{exclude
											{tag
												{tag destroyed}
											}
										}
									}
								}
							}
							{"delay"
								{time 0.1}
							}
							{"switch"
								{"case"
									{condition
										{type cmp_i}
										{var "ai_air_on$"}
										{op "=="}
										{value 5}
									}
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
														{tag _user}
													}
												}
												{include
													{zone
														{zone "gamezone"}
													}
													{prop
														{prop vehicle}
													}												
													{state
														{state operatable}
													}
													{state
														{state user_control}
													}
												}
												{exclude
													{tag
														{tag destroyed}
													}
												}
											}
											{sort
												{type waypoint}
												{wpid "5"}
											}
											{amount 1
											}
										}
										{tag_add ai_air_target}
									}
								}
								{"case"
									{condition
										{type cmp_i}
										{var "ai_air_on$"}
										{op "=="}
										{value 2}
									}
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
														{tag _user}
													}
												}
												{include
													{zone
														{zone "gamezone"}
													}
													{prop
														{prop vehicle}
													}													
													{state
														{state operatable}
													}
													{state
														{state user_control}
													}
												}
												{exclude
													{tag
														{tag destroyed}
													}
												}
											}
											{sort
												{type waypoint}
												{wpid "2"}
											}
											{amount 1
											}
										}
										{tag_add ai_air_target}
									}
								}
								{"case"
									{condition
										{type cmp_i}
										{var "ai_air_on$"}
										{op "=="}
										{value 3}
									}
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
														{tag _user}
													}
												}
												{include
													{zone
														{zone "gamezone"}
													}
													{prop
														{prop vehicle}
													}											
													{state
														{state operatable}
													}
													{state
														{state user_control}
													}
												}
												{exclude
													{tag
														{tag destroyed}
													}
												}
											}
											{sort
												{type waypoint}
												{wpid "3"}
											}
											{amount 1
											}
										}
										{tag_add ai_air_target}
									}
								}
								{"case"
									{condition
										{type cmp_i}
										{var "ai_air_on$"}
										{op "=="}
										{value 4}
									}
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
														{tag _user}
													}
												}
												{include
													{zone
														{zone "gamezone"}
													}
													{prop
														{prop vehicle}
													}													
													{state
														{state operatable}
													}
													{state
														{state user_control}
													}
												}
												{exclude
													{tag
														{tag destroyed}
													}
												}
											}
											{sort
												{type waypoint}
												{wpid "4"}
											}
											{amount 1
											}
										}
										{tag_add ai_air_target}
									}
								}
								{"default"
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
														{tag _user}
													}
												}
												{include
													{zone
														{zone "gamezone"}
													}
													{prop
														{prop vehicle}
													}													
													{state
														{state operatable}
													}
													{state
														{state user_control}
													}
												}
												{exclude
													{tag
														{tag destroyed}
													}
												}
											}
											{sort
												{type waypoint}
												{wpid "1"}
											}
											{amount 1
											}
										}
										{tag_add ai_air_target}
									}
								}
							}
						}
						{"default"
							{"delay"
								{time 0.1}
							}
							{"switch"
								{"case"
									{condition
										{type cmp_i}
										{var "ai_air_on$"}
										{op "=="}
										{value 5}
									}
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
														{tag _user}
													}
												}
												{include
													{zone
														{zone "gamezone"}
													}
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
											{sort
												{type waypoint}
												{wpid "5"}
											}
											{amount 1
											}
										}
										{tag_add ai_air_target}
									}
								}
								{"case"
									{condition
										{type cmp_i}
										{var "ai_air_on$"}
										{op "=="}
										{value 2}
									}
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
														{tag _user}
													}
												}
												{include
													{zone
														{zone "gamezone"}
													}
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
											{sort
												{type waypoint}
												{wpid "2"}
											}
											{amount 1
											}
										}
										{tag_add ai_air_target}
									}
								}
								{"case"
									{condition
										{type cmp_i}
										{var "ai_air_on$"}
										{op "=="}
										{value 3}
									}
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
														{tag _user}
													}
												}
												{include
													{zone
														{zone "gamezone"}
													}
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
											{sort
												{type waypoint}
												{wpid "3"}
											}
											{amount 1
											}
										}
										{tag_add ai_air_target}
									}
								}
								{"case"
									{condition
										{type cmp_i}
										{var "ai_air_on$"}
										{op "=="}
										{value 4}
									}
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
														{tag _user}
													}
												}
												{include
													{zone
														{zone "gamezone"}
													}
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
											{sort
												{type waypoint}
												{wpid "4"}
											}
											{amount 1
											}
										}
										{tag_add ai_air_target}
									}
								}
								{"default"
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
														{tag _user}
													}
												}
												{include
													{zone
														{zone "gamezone"}
													}
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
											{sort
												{type waypoint}
												{wpid "1"}
											}
											{amount 1
											}
										}
										{tag_add ai_air_target}
									}
								}
							}
						}
					}
				}
			}
			{"dcg/warning_air"
				{condition
					{expression "!1 & 2"}
					{terms
						{"1.event"
							{id "talk_on"}
						}
						{"2.trigger"
							{name "dcg/ai_airstrike"}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"event"
						{event "talk_on"}
						{mode set}
						{delay 0}
					}
					{"delay"
						{time 6}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 0.1}
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
								{text "mission/multi/enemy_plane_alert2"}
							}
						}
						{"default"
							{"delay"
								{time 0.1}
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
								{text "mission/multi/enemy_plane_alert1"}
							}
						}
					}
					{"delay"
						{time 8}
					}
					{"event"
						{event "talk_on"}
						{mode reset}
						{delay 0}
					}
					{"delay"
						{time 1200}
					}
					{"trigger"
						{name "dcg/ai_air_chance"}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "dcg/ai_airstrike"}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "dcg/ai_airstrike_do"}
					}
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "dcg/warning_air"}
					}	
					{"delay"
						{time 0.1}
					}
					{"trigger"
						{name "dcg/ai_air_on"}
					}						
					{"set_i"
						{var "ai_air_on$"}
						{op "="}
						{value 0}
					}	
					{"event"
						{event "airplane_set"}
						{mode set}
						{delay 0}
					}					
				}
			}

;// Bot tags
			{"lua/HF_bot_tags"
				{condition
					{terms
						{"1.cmp_i"
							{var "dcg_on$"}
							{op ">"}
							{value 2}
						}
						{"2.entities"
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
										{state
											{state operatable}
										}
									}
									{exclude
										{tag
											{tag _bot}
										}
										{tag
											{tag _animal}
										}
										{tag
											{tag cmp_def}
										}
										{tag
											{tag air_vehicle}
										}
										{player
											{player "0"}
										}
										{state
											{state user_control}
										}
									}
								}
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
							{group
								{select
									{tag
										{tag _ai}
									}
								}
								{include
									{prop
										{prop human}
									}
									{with_item
										{type existance}
										{item "tent_kit"}
									}
								}
								{exclude
									{tag
										{tag bot_medic}
									}
									{state
										{state user_control}
									}
								}
							}
						}
						{tag_add bot_medic}
					}
					{"delay"
						{time 0.5}
					}
					{"entity_state"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{select
									{tag
										{tag _ai}
									}
								}
								{include
									{prop
										{prop detect_artillery}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag bot_arty}
									}
									{state
										{state user_control}
									}
								}
							}
						}
						{tag_add bot_arty}
					}
					{"delay"
						{time 0.5}
					}
					{"entity_state"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{select
									{tag
										{tag _ai}
									}
								}
								{include
									{tag
										{tag anti_air}
									}
									{state
										{state operatable}
									}
								}
								{exclude
									{tag
										{tag bot_aa}
									}
									{prop
										{prop cannon}
									}
									{prop
										{prop cannon_small}
									}
									{state
										{state user_control}
									}
								}
							}
						}
						{tag_add bot_aa}
					}
					{"delay"
						{time 6}
					}
					{"trigger"
						{name "lua/HF_bot_tags"}
					}
				}
			}

;// Tank AI
			{"hf bot tank/finish off"
				{condition
					{terms
						{"1.see_actors"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag bot_tank}
										}
									}
									{include
										{state
											{state inhabited}
										}
									}
									{exclude
										{prop
											{prop human}
										}
										{state
											{state user_control}
										}
										{tag
											{tag ai_start_finishing_off}
										}
										{tag
											{tag ai_enemy_near_cooldown}
										}
										{tag
											{tag "+mobilegun"}
										}
									}
								}
								{amount 1
								}
							}
							{enemy
								{source advanced}
								{group
									{include
										{state
											{state user_control}
										}
										{prop
											{prop tank}
										}
									}
									{exclude
										{prop
											{prop human}
										}
										{destroyed_components
											{masks
												"body"
											}
										}
										{destroyed_components
											{masks
												"ammo"
											}
										}
										{destroyed_components
											{masks
												"engine"
											}
										}
									}
								}
							}
							{distance
								{mode in_range}
								{min 0}
								{max 220}
							}
							{detection located}
							{tag
								{"tag pair"}
								{"for selector" ai_start_finishing_off}
								{"for enemy" ai_target_finishing_off}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 0.01}
					}
					{"switch"
						{"case"
							{condition
								{type see_actors}
								{selector
									{tag ai_start_finishing_off}
								}
								{enemy
									{source advanced}
									{group
										{include
											{state
												{state "inhabited user_control"}
											}
											{prop
												{prop vehicle}
											}
										}
									}
								}
								{distance
									{mode in_range}
									{min 0}
									{max 200}
								}
								{detection located}
							}
							{"delay"
								{time 0.01}
							}
						}
						{"default"
							{"actor_fire"
								{selector
									{tag ai_start_finishing_off}
								}
								{attack enemies}
								{enemies
									{tag ai_target_finishing_off}
								}
								{interval 0}
								{time 20}
								{burst_mode long}
							}
							{"delay"
								{time 10}
							}
						}
					}
					{"entity_state"
						{selector
							{tag ai_start_finishing_off}
						}
						{tag_add ai_enemy_near_cooldown}
					}
					{"entity_state"
						{selector
							{tag ai_target_finishing_off}
						}
						{tag_remove ai_target_finishing_off}
					}
					{"delay"
						{time 0.01}
					}
					{"effect"
						{selector
							{tag ai_start_finishing_off}
						}
						{effect cooldown_reset}
					}
					{"trigger"
						{name "hf bot tank/finish off"}
					}
				}
			}