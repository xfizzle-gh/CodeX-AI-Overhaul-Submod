; Freeze parked support-pool prototypes, then delete unused faction pools once
; conquest.lua has published bot_army$. Inactive still leaves bodies in world
; scans; Adams' delete-790 test is the remaining multiplier.
			(define "st_del"
				{"delete" {selector {ignore_captured_by_user 0} {tag %0}}}
			)
			(define "st_del_ally"
				("st_del" args ally_sup_%0)
				("st_del" args support_e2_%0_helo)
				("st_del" args support_e2_%0_helo_crew)
				("st_del" args support_e2_%0_helo_team)
				("st_del" args support_e2_%0_para)
				("st_del" args support_e2_%0_para_crew)
				("st_del" args support_e2_%0_para_pax)
			)
			(define "st_del_enemy"
				("st_del" args enemy_def_%0)
				("st_del" args transport_enemy_%0)
			)

			{"support_templates/inert"
				{condition
					{terms
						{"1.cmp_i" {var "support_templates_inert$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"set_i" {var "support_templates_inert$"} {op "="} {value 1}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_tpl}}}}
						}
						{inactive on}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_tpl}}}}
						}
						{inactive on}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag ally_sup_tpl}}}}
						}
						{inactive on}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag flag_prop_tpl}}}}
						}
						{inactive on}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag transport_enemy_tpl}}}}
						}
						{inactive on}
					}
				}
			}

			{"support_templates/cull"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "support_templates_culled$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "bot_army$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "support_templates_culled$"} {op "="} {value 1}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 2}}
							{"set_i" {var "support_templates_ally$"} {op "="} {value 2}}
						}
						{"case"
							{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 6}}
							{"set_i" {var "support_templates_ally$"} {op "="} {value 4}}
						}
						{"case"
							{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 1}}
							{"set_i" {var "support_templates_ally$"} {op "="} {value 1}}
						}
						{"case"
							{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 5}}
							{"set_i" {var "support_templates_ally$"} {op "="} {value 1}}
						}
						{"case"
							{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 8}}
							{"set_i" {var "support_templates_ally$"} {op "="} {value 1}}
						}
						{"case"
							{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 3}}
							{"set_i" {var "support_templates_ally$"} {op "="} {value 3}}
						}
						{"case"
							{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 4}}
							{"set_i" {var "support_templates_ally$"} {op "="} {value 3}}
						}
						{"case"
							{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 7}}
							{"set_i" {var "support_templates_ally$"} {op "="} {value 3}}
						}
						{"default"
							{"switch"
								{"case"
									{condition {type cmp_i} {var "user_nation_hint$"} {op "=="} {value 1}}
									{"set_i" {var "support_templates_ally$"} {op "="} {value 1}}
								}
								{"default"
									{"set_i" {var "support_templates_ally$"} {op "="} {value 3}}
								}
							}
						}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 2}}
							{"set_i" {var "support_templates_enemy$"} {op "="} {value 2}}
						}
						{"case"
							{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 3}}
							{"set_i" {var "support_templates_enemy$"} {op "="} {value 3}}
						}
						{"case"
							{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 4}}
							{"set_i" {var "support_templates_enemy$"} {op "="} {value 3}}
						}
						{"case"
							{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 6}}
							{"set_i" {var "support_templates_enemy$"} {op "="} {value 4}}
						}
						{"case"
							{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 7}}
							{"set_i" {var "support_templates_enemy$"} {op "="} {value 3}}
						}
						{"default"
							{"set_i" {var "support_templates_enemy$"} {op "="} {value 1}}
						}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_templates_ally$"} {op "=="} {value 1}}
							("st_del_ally" args ukr)
							("st_del_ally" args nato)
							("st_del_ally" args prc)
						}
						{"case"
							{condition {type cmp_i} {var "support_templates_ally$"} {op "=="} {value 2}}
							("st_del_ally" args rusa)
							("st_del_ally" args nato)
							("st_del_ally" args prc)
						}
						{"case"
							{condition {type cmp_i} {var "support_templates_ally$"} {op "=="} {value 4}}
							("st_del_ally" args rusa)
							("st_del_ally" args ukr)
							("st_del_ally" args nato)
						}
						{"default"
							("st_del_ally" args rusa)
							("st_del_ally" args ukr)
							("st_del_ally" args prc)
						}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_templates_enemy$"} {op "=="} {value 1}}
							("st_del_enemy" args ukr)
							("st_del_enemy" args nato)
							("st_del_enemy" args prc)
						}
						{"case"
							{condition {type cmp_i} {var "support_templates_enemy$"} {op "=="} {value 2}}
							("st_del_enemy" args rusa)
							("st_del_enemy" args nato)
							("st_del_enemy" args prc)
						}
						{"case"
							{condition {type cmp_i} {var "support_templates_enemy$"} {op "=="} {value 4}}
							("st_del_enemy" args rusa)
							("st_del_enemy" args ukr)
							("st_del_enemy" args nato)
						}
						{"default"
							("st_del_enemy" args rusa)
							("st_del_enemy" args ukr)
							("st_del_enemy" args prc)
						}
					}
				}
			}

; Runtime human-born support provenance bridge (issue #97).
(include "/map/multi/native_support_birth.inc")
