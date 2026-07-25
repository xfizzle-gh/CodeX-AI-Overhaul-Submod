;// Scripts written by Horten
;// Submod edit (compat audit): auto-loot restricted to AI units only.
;// Restored Horten's original user_control / player exclusions so player-owned
;// units are never auto-commanded to loot weapons from corpses.

(define "looting_weapon"

	{"looting/go loot %weapon" ;// Unit doesn't have weapon in its inventory and is in range of a dead body that has the weapon
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
						{amount 1}
					}
					{near_to
						{tag has_%weapon}
						{type human}
						{state dead}
					}
					{distance 7}
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
					{amount 1}
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
				{name "looting/go loot %weapon"}
			}
		}
	}

	{"looting/body with %weapon in inventory" ;// AI is dead with weapon in his inventory - add tags
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

	{"looting/no more %weapon in body" ;// AI dead body no more has the weapon in it's inventory, de-tag
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
					{amount 1}
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
					{amount 1}
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
					{amount 1}
				}
				{tag_remove has_%weapon}
			}
			{"trigger"
				{name "looting/no more %weapon in body"}
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


;//bazookas (Code:X)
;// === AT / AA LAUNCHERS (all) ===
("looting_weapon" weapon(javelin) type(weapon))
("looting_weapon" weapon(nlaw) type(weapon))
("looting_weapon" weapon(carl_gustaf_v1) type(weapon))
("looting_weapon" weapon(carl_gustaf_m18) type(weapon))
("looting_weapon" weapon(m3_maaws) type(weapon))
("looting_weapon" weapon(maaws) type(weapon))
("looting_weapon" weapon(panzerfaust3) type(weapon))
("looting_weapon" weapon(panzerfaust-3) type(weapon))
("looting_weapon" weapon(SMAW) type(weapon))
("looting_weapon" weapon(rgw90_as) type(weapon))
("looting_weapon" weapon(rgw90_as_he) type(weapon))
("looting_weapon" weapon(trigat) type(weapon))

("looting_weapon" weapon(at-4) type(weapon))
("looting_weapon" weapon(at4_m86) type(weapon))
("looting_weapon" weapon(at4cs_v1) type(weapon))
("looting_weapon" weapon(m136) type(weapon))
("looting_weapon" weapon(m72_law) type(weapon))
("looting_weapon" weapon(m72a7law) type(weapon))
("looting_weapon" weapon(m202) type(weapon))

("looting_weapon" weapon(hj12e) type(weapon))
("looting_weapon" weapon(rpg2_type56) type(weapon))
("looting_weapon" weapon(rpg7) type(weapon))
("looting_weapon" weapon(rpg-7) type(weapon))
("looting_weapon" weapon(rpg-7v) type(weapon))
("looting_weapon" weapon(rpg-7v2) type(weapon))
("looting_weapon" weapon(rpg7vs) type(weapon))
("looting_weapon" weapon(rpg7vshe) type(weapon))
("looting_weapon" weapon(rmg) type(weapon))
("looting_weapon" weapon(rpg_29) type(weapon))
("looting_weapon" weapon(rpg29) type(weapon))
("looting_weapon" weapon(RPG22) type(weapon))
("looting_weapon" weapon(rpg26) type(weapon))
("looting_weapon" weapon(rpg26_fpv) type(weapon))
("looting_weapon" weapon(RPG27) type(weapon))
("looting_weapon" weapon(RPG28) type(weapon))
("looting_weapon" weapon(RPG30) type(weapon))
("looting_weapon" weapon(RShG_1) type(weapon))
("looting_weapon" weapon(RShG_2) type(weapon))
("looting_weapon" weapon(Shmel) type(weapon))
("looting_weapon" weapon(spg-9) type(weapon))

("looting_weapon" weapon(igla) type(weapon))
("looting_weapon" weapon(stinger) type(weapon))
("looting_weapon" weapon(stinger_x4) type(weapon))
("looting_weapon" weapon(spike-er_x1) type(weapon))
("looting_weapon" weapon(spike-lr_x1) type(weapon))
("looting_weapon" weapon(spike-lr_x2) type(weapon))

;// === MAN-PORTABLE MGs / AUTO RIFLES ===
;// === ("looting_weapon" weapon(m249_saw_h) type(weapon))
;// === ("looting_weapon" weapon(m240_v1) type(weapon))
;// === ("looting_weapon" weapon(m240l_a) type(weapon))
;// === ("looting_weapon" weapon(fn_mag_v1) type(weapon))

;// === ("looting_weapon" weapon(mg3_drum) type(weapon))
;// === ("looting_weapon" weapon(mg4_a) type(weapon))
;// === ("looting_weapon" weapon(mg5_a) type(weapon))
;// === ("looting_weapon" weapon(mg338) type(weapon))

;// === ("looting_weapon" weapon(pk) type(weapon))
;// === ("looting_weapon" weapon(pkm) type(weapon))
;// === ("looting_weapon" weapon(pkp_a) type(weapon))
;// === ("looting_weapon" weapon(pkp_m) type(weapon))
;// === ("looting_weapon" weapon(pks) type(weapon))
;// === ("looting_weapon" weapon(6p49) type(weapon))

;// === ("looting_weapon" weapon(aanf1) type(weapon))
;// === ("looting_weapon" weapon(hk21) type(weapon))
;// === ("looting_weapon" weapon(ksp_58) type(weapon))
;// === ("looting_weapon" weapon(ksp_90b) type(weapon))
;// === ("looting_weapon" weapon(ukm2000) type(weapon))

;// === ("looting_weapon" weapon(rpk_74m) type(weapon))
;// === ("looting_weapon" weapon(rpk_74m_h) type(weapon))
;// === ("looting_weapon" weapon(rpk_v2) type(weapon))
;// === ("looting_weapon" weapon(rpk16_h) type(weapon))
;// === ("looting_weapon" weapon(rpl_20) type(weapon))

;// === DMR / MARKSMAN PICKUPS (rifleman-upgrade tier) ===
("looting_weapon" weapon(L129A1) type(weapon))
("looting_weapon" weapon(m110a1_v1) type(weapon))
("looting_weapon" weapon(m38_dmr) type(weapon))
("looting_weapon" weapon(mk14v3) type(weapon))
("looting_weapon" weapon(g28) type(weapon))
("looting_weapon" weapon(svd) type(weapon))
("looting_weapon" weapon(svd_1p69) type(weapon))
("looting_weapon" weapon(svd_mp) type(weapon))
("looting_weapon" weapon(vss_vintorez_v1) type(weapon))

;// === FRAG GRENADES (worth picking up) ===
;// === ("looting_weapon" weapon(m67) type(grenade))
;// === ("looting_weapon" weapon(m26) type(grenade))
;// === ("looting_weapon" weapon(dm51) type(grenade))
;// === ("looting_weapon" weapon(f1m) type(grenade))
;// === ("looting_weapon" weapon(rgd5) type(grenade))
;// === ("looting_weapon" weapon(rgn) type(grenade))
;// === ("looting_weapon" weapon(rgo) type(grenade))

;//Mine clearing
			{"hf inf ai/clear mines"
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
						{name "hf inf ai/clear mines"}
					}
				}
			}
			{"hf inf ai/clear mines tag"
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
						{name "hf inf ai/clear mines tag"}
					}
				}
			}
			{"hf inf ai/clear mines detag"
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
						{name "hf inf ai/clear mines detag"}
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
							{tag bot_arty_move}
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
							{tag bot_arty_move}
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
							{tag bot_arty_move}
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
								}
							}
						}
						{tag_add bot_arty_fire}
						{tag_remove bot_arty_move}
					}
					{"actor_state"
						{selector
							{tag bot_arty_fire}
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
							{tag bot_arty_fire}
						}
						{action rotate}
						{target
							{source advanced}
							{group
								{include
									{state
										{state user_control}
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
							{tag bot_arty_fire}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{prop
										{prop vehicle}
									}
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
								}
							}
						}
						{interval 0}
						{time 210}
						{burst_mode long}
					}
					{"delay"
						{time 30}
					}
					{"actor_fire"
						{selector
							{tag bot_arty_fire}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{prop
										{prop vehicle}
									}
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
								}
							}
						}
						{interval 0}
						{time 210}
						{burst_mode long}
					}
					{"delay"
						{time 30}
					}
					{"actor_fire"
						{selector
							{tag bot_arty_fire}
						}
						{attack enemies}
						{enemies
							{source advanced}
							{group
								{include
									{prop
										{prop vehicle}
									}
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
								}
							}
						}
						{interval 0}
						{time 210}
						{burst_mode long}
					}
					{"delay"
						{time 190}
					}
					{"entity_state"
						{selector
							{tag bot_arty_fire}
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
							{distance 15}
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
			{"hf arty ai/part 6"
				{condition
					{type 1}
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag engine_count_2}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
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
											{tag airstrike_usa_rocket_hvar}
										}										
										{tag
											{tag airstrike_usa_rocket_m8}
										}											
										{tag
											{tag airstrike_ger_strafe}
										}											
										{tag
											{tag airstrike_ger_recon}
										}											
										{tag
											{tag airstrike_usa}
										}											
										{tag
											{tag airstrike_ger}
										}											
										{tag
											{tag airstrike_rus}
										}																	
										{tag
											{tag shotdown}
										}
										{tag
											{tag destroyed}
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
											{tag engine_count_1}
										}
									}
									{exclude
										{state
											{state "dead inactive"}
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
											{tag airstrike_usa_rocket_hvar}
										}										
										{tag
											{tag airstrike_usa_rocket_m8}
										}											
										{tag
											{tag airstrike_ger_strafe}
										}											
										{tag
											{tag airstrike_ger_recon}
										}											
										{tag
											{tag airstrike_usa}
										}											
										{tag
											{tag airstrike_ger}
										}											
										{tag
											{tag airstrike_rus}
										}																	
										{tag
											{tag shotdown}
										}
										{tag
											{tag destroyed}
										}
									}
								}
							}
						}
						{"3.near"
							{units
								{tag bot_arty}
							}
							{near_to
								{tag flare_landed}
							}
							{distance 60}
						}
						{"4.near"
							{units
								{tag bot_arty}
							}
							{near_to
								{tag crosshair}
							}
							{distance 60}
						}
					}
				}
				{actions
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{tag bot_arty}
						}
						{tag_add bot_arty_skedaddle}
						{tag_remove bot_arty_fire}
					}
					{"delay"
						{time 0.1}
					}
					{"entity_state"
						{selector
							{tag bot_arty_skedaddle}
						}
						{tag_remove bot_arty_move}
					}
					{"delay"
						{time 3}
					}
					{"actor_state"
						{selector
							{tag bot_arty_skedaddle}
						}
						{drop orders}
						{movement
							{speed fast}
						}
						{cannon move}
					}
					{"delay"
						{time 3}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.33}
							}
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "1"}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.33}
							}
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "2"}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.33}
							}
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "3"}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.33}
							}
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "4"}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.33}
							}
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "5"}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.33}
							}
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "6"}
							}
						}
						{"default"
							{"action"
								{selector
									{tag bot_arty_skedaddle}
								}
								{drop orders}
								{action move}
								{waypoint "0"}
							}
						}
					}
					{"delay"
						{time 35}
					}
					{"entity_state"
						{selector
							{tag bot_arty_skedaddle}
						}
						{tag_remove bot_arty_skedaddle}
					}
					{"delay"
						{time 1}
					}
					{"trigger"
						{name "hf arty ai/part 6"}
					}
				}
			}
			(define "hf_medic_ai"
				{"hf medic ai/p%player/run towards nearest wounded"
					{condition
						{terms
							{"1.entities"
								{selector
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{include
											{tag
												{tag bleeding}
											}
											{player
												{player "%player"}
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
											{prop
												{prop human}
											}
											{with_item
												{type existance}
												{item "tent_kit"}
											}
											{player
												{player "%player"}
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
											{state
												{state user_control}
											}
											{tag
												{tag _user}
											}
											{tag
												{tag medic_working}
											}
											{tag
												{tag current_medic}
											}
										}
									}
								}
							}
							{"3.near"
								{units
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{include
											{tag
												{tag bleeding}
											}
											{player
												{player "%player"}
											}
										}
										{exclude
											{state
												{state inactive}
											}
										}
									}
								}
								{near_to
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
											{with_item
												{type existance}
												{item "tent_kit"}
											}
											{player
												{player "%player"}
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
											{state
												{state user_control}
											}
											{tag
												{tag _user}
											}
											{tag
												{tag medic_working}
											}
											{tag
												{tag current_medic}
											}
										}
									}
								}
								{distance 100}
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
									{include
										{tag
											{tag bleeding}
										}
										{player
											{player "%player"}
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
												{with_item
													{type existance}
													{item "tent_kit"}
												}
												{player
													{player "%player"}
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
												{state
													{state user_control}
												}
												{tag
													{tag _user}
												}
												{tag
													{tag medic_working}
												}
												{tag
													{tag current_medic}
												}
											}
										}
									}
								}
								{amount 1}
							}
							{tag_add hf_medic_target_p%player}
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
										{with_item
											{type existance}
											{item "tent_kit"}
										}
										{player
											{player "%player"}
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
										{state
											{state user_control}
										}
										{tag
											{tag _user}
										}
										{tag
											{tag medic_working}
										}
										{tag
											{tag current_medic}
										}
									}
								}
								{sort
									{type entity}
									{entity
										{tag hf_medic_target_p%player}
										{player "%player"}
									}
								}
								{amount 1}
							}
							{tag_add hf_current_medic_p%player}
						}
						{"action"
							{selector
								{tag hf_current_medic_p%player}
								{player "%player"}
								{state "not dead"}
							}
							{drop "orders sensor"}
							{action move}
							{target
								{tag hf_medic_target_p%player}
								{player "%player"}
							}
						}
						{"entity_state"
							{selector
								{tag hf_medic_target_p%player}
								{player "%player"}
							}
							{tag_remove hf_medic_target_p%player}
						}
						{"entity_state"
							{selector
								{tag hf_current_medic_p%player}
								{player "%player"}
							}
							{tag_remove hf_current_medic_p%player}
						}
						{"delay"
							{time 5}
						}
						{"trigger"
							{name "hf medic ai/p%player/run towards nearest wounded"}
						}
					}
				}
				{"hf medic ai/p%player/medic smoke"
					{condition
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
											{with_item
												{type existance}
												{item "tent_kit"}
											}
											{player
												{player "%player"}
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
											{state
												{state user_control}
											}
											{tag
												{tag _user}
											}
											{tag
												{tag medic_working}
											}
											{tag
												{tag current_medic}
											}
										}
									}
								}
								{near_to
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{include
											{tag
												{tag bleeding}
											}
											{player
												{player "%player"}
											}
										}
										{exclude
											{state
												{state inactive}
											}
											{state
												{state user_control}
											}
											{tag
												{tag _user}
											}
										}
									}
								}
								{distance 21}
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
									{include
										{tag
											{tag bleeding}
										}
										{player
											{player "%player"}
										}
									}
									{exclude
										{state
											{state inactive}
										}
										{state
											{state user_control}
										}
										{tag
											{tag _user}
										}
									}
								}
								{sort
									{type entity}
									{entity
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
												{with_item
													{type existance}
													{item "tent_kit"}
												}
												{player
													{player "%player"}
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
													{tag medic_working}
												}
												{tag
													{tag current_medic}
												}
											}
										}
									}
								}
								{amount 1}
							}
							{tag_add hf_medic_smoke_target_p%player}
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
										{with_item
											{type existance}
											{item "tent_kit"}
										}
										{player
											{player "%player"}
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
										{state
											{state user_control}
										}
										{tag
											{tag _user}
										}
										{tag
											{tag medic_working}
										}
										{tag
											{tag current_medic}
										}
									}
								}
								{sort
									{type entity}
									{entity
										{tag hf_medic_smoke_target_p%player}
										{player "%player"}
									}
								}
								{amount 1}
							}
							{tag_add hf_medic_smoke_p%player}
						}
						{"actor_state"
							{selector
								{tag hf_medic_smoke_p%player}
								{player "%player"}
							}
							{drop orders}
						}
						{"inventory"
							{selector
								{tag hf_medic_smoke_p%player}
								{player "%player"}
							}
							{action add}
							{item "ai_nbk39" "grenade"}
						}
						{"delay"
							{time 0.2}
						}
						{"inventory"
							{selector
								{tag hf_medic_smoke_p%player}
								{player "%player"}
								{type human}
								{state "not dead"}
							}
							{action take}
							{item "ai_nbk39" "grenade"}
						}
						{"delay"
							{time 1}
						}
						{"actor_state"
							{selector
								{tag hf_medic_smoke_p%player}
								{player "%player"}
							}
							{move_mode hold}
						}
						{"action"
							{selector
								{tag hf_medic_smoke_p%player}
								{player "%player"}
								{state "not dead"}
							}
							{drop "orders sensor senseless"}
							{action attack_ground}
							{target
								{tag hf_medic_smoke_target_p%player}
								{player "%player"}
							}
						}
						{"delay"
							{time 5}
						}
						{"action"
							{selector
								{tag hf_medic_smoke_p%player}
								{player "%player"}
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
								{tag hf_medic_smoke_p%player}
								{player "%player"}
							}
							{drop orders}
						}
						{"entity_state"
							{selector
								{tag hf_medic_smoke_target_p%player}
								{player "%player"}
							}
							{tag_remove hf_medic_smoke_target_p%player}
						}
						{"entity_state"
							{selector
								{tag hf_medic_smoke_p%player}
								{player "%player"}
							}
							{tag_remove hf_medic_smoke_p%player}
						}
						{"delay"
							{time 5}
						}
						{"trigger"
							{name "hf medic ai/p%player/medic smoke"}
						}
					}
				}
			)

			("hf_medic_ai" player(0))
			("hf_medic_ai" player(1))
			("hf_medic_ai" player(2))
			("hf_medic_ai" player(3))
			("hf_medic_ai" player(4))
			("hf_medic_ai" player(5))
			("hf_medic_ai" player(6))
			("hf_medic_ai" player(7))
			("hf_medic_ai" player(8))
