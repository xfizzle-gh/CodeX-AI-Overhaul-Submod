; One-shot diagnostic proof for allied DefenderBot reinforcements.
; This is intentionally not a production wave system.
; It clones one CE-prepared hidden cmp_def human to the universal map waypoint "0".
; No gamezone filter is used for promotion, ownership, or AI orders.
; On-screen timers identify the last stage reached.

			{"allied_support/diagnostic_once"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"3.cmp_i" {var "prep_inform$"} {op "=="} {value 1}}
					}
				}
				{actions
					{"timer" {time 8} {title "SUPPORT 1 ARMED"}}
					{"delay" {time 10}}

					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag cmp_def}}}
								{include
									{prop {prop human}}
									{tag {tag hidden}}
								}
								{exclude
									{zone {zone "gamezone"}}
									{state {state linked}}
									{state {state user_control}}
								}
							}
							{amount 1}
						}
						{tag_add allied_support_diag_source}
					}
					{"trigger" {name "allied_support/diag_source_found"}}
					{"timer" {time 8} {title "SUPPORT 2 SOURCE CHECK RAN"}}
					{"delay" {time 1}}

					{"placement"
						{selector
							{source advanced}
							{group
								{select {tag {tag allied_support_diag_source}}}
								{include {prop {prop human}}}
							}
							{amount 1}
						}
						{target_waypoint "0"}
						{clone}
					}
					{"timer" {time 8} {title "SUPPORT 3 CLONE ACTION RAN"}}
					{"delay" {time 1}}
					{"trigger" {name "allied_support/diag_clone_found"}}

					{"entity_state"
						{selector {tag allied_support_diag_source} {type human}}
						{tag_add allied_wave_fresh}
						{tag_add allied_support}
						{tag_add cmp_def}
						{tag_add _def}
						{tag_add _ai_defender}
						{tag_remove hidden}
						{inactive off}
					}
					{"timer" {time 8} {title "SUPPORT 4 PROMOTE ACTION RAN"}}
					{"delay" {time 1}}
					{"trigger" {name "allied_support/diag_promoted_found"}}

					{"switch"
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 1}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "1"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_1} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 2}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "2"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_2} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 3}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "3"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_3} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 4}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "4"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_4} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 5}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "5"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_5} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 6}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "6"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_6} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 7}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "7"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_7} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 8}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "8"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_8} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 9}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "9"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_9} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 10}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "10"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_10} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 11}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "11"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_11} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 12}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "12"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_12} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 13}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "13"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_13} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 14}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "14"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_14} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 15}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "15"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_15} {tag_add allied_support_diag_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 16}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "16"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_16} {tag_add allied_support_diag_owned}}
						}
						{"default"
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_unsupported}}
						}
					}
					{"timer" {time 8} {title "SUPPORT 5 OWNER ACTION RAN"}}
					{"delay" {time 1}}
					{"trigger" {name "allied_support/diag_owner_found"}}

					{"actor_state"
						{selector {tag allied_wave_fresh} {type human}}
						{control AI}
						{weapon_prepare on}
						{fire_mode open}
						{move_mode free}
						{movement {speed assault}}
						{ai {no_retreat on}}
					}
					{"ables"
						{selector {tag allied_wave_fresh} {type human}}
						{remove select}
					}
					{"action"
						{selector {tag allied_wave_fresh} {type human}}
						{drop orders}
						{action advance}
						{target {tag fpc1}}
					}
					{"timer" {time 12} {title "SUPPORT 6 AI ORDER SENT"}}
				}
			}

			{"allied_support/diag_source_found"
				{condition
					{type entities}
					{selector {tag allied_support_diag_source} {type human}}
					{count {op ">="} {value 1}}
				}
				{actions
					{"timer" {time 12} {title "SUPPORT 2A SOURCE FOUND"}}
				}
			}

			{"allied_support/diag_clone_found"
				{condition
					{type entities}
					{selector {tag allied_support_diag_source} {type human}}
					{count {op ">="} {value 2}}
				}
				{actions
					{"timer" {time 12} {title "SUPPORT 3A CLONE FOUND"}}
				}
			}

			{"allied_support/diag_promoted_found"
				{condition
					{type entities}
					{selector {tag allied_wave_fresh} {type human}}
					{count {op ">="} {value 2}}
				}
				{actions
					{"timer" {time 12} {title "SUPPORT 4A PROMOTED FOUND"}}
				}
			}

			{"allied_support/diag_owner_found"
				{condition
					{type entities}
					{selector {tag allied_support_diag_owned} {type human}}
					{count {op ">="} {value 2}}
				}
				{actions
					{"timer" {time 12} {title "SUPPORT 5A OWNER FOUND"}}
				}
			}
