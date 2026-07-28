; One-shot diagnostic proof for allied DefenderBot reinforcements.
; Not a production wave system.
;
; Sources CE-reserved templates (allied_support_template / not_delete),
; clones one human to flag waypoint "1" (fpc1), promotes only in-zone clones,
; assigns DefenderBot ownership, AI control, advance to fpc1.
;
; On-screen timers:
;   SUPPORT 1 ARMED
;   SUPPORT 2 SOURCE CHECK RAN / 2A SOURCE FOUND
;   SUPPORT 3 CLONE ACTION RAN / 3A CLONE IN ZONE
;   SUPPORT 4 PROMOTE ACTION RAN / 4A PROMOTED FOUND
;   SUPPORT 5 OWNER ACTION RAN / 5A OWNER FOUND
;   SUPPORT 6 AI ORDER SENT

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

					; Prefer CE-reserved template pool (survives balance delete).
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag {tag allied_support_template}}
									{tag {tag not_delete}}
								}
								{include
									{prop {prop human}}
								}
								{exclude
									{zone {zone "gamezone"}}
									{state {state linked}}
									{state {state user_control}}
									{state {state dead}}
								}
							}
							{amount 1}
						}
						{tag_add allied_support_diag_source}
					}
					; Fallback: any remaining off-zone cmp_def human.
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag cmp_def}}}
								{include
									{prop {prop human}}
								}
								{exclude
									{zone {zone "gamezone"}}
									{state {state linked}}
									{state {state user_control}}
									{state {state dead}}
									{tag {tag allied_support_diag_source}}
								}
							}
							{amount 1}
						}
						{tag_add allied_support_diag_source}
					}
					{"trigger" {name "allied_support/diag_source_found"}}
					{"timer" {time 8} {title "SUPPORT 2 SOURCE CHECK RAN"}}
					{"delay" {time 1}}

					; Clone to CE flag waypoint 1 (fpc1) — known in-fight point.
					{"placement"
						{selector
							{source advanced}
							{group
								{select {tag {tag allied_support_diag_source}}}
								{include {prop {prop human}}}
							}
							{amount 1}
						}
						{target_waypoint "1"}
						{clone}
					}
					{"timer" {time 8} {title "SUPPORT 3 CLONE ACTION RAN"}}
					{"delay" {time 1}}

					; Mark only in-zone copies (clone), never the off-zone template.
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag allied_support_diag_source}}}
								{include
									{prop {prop human}}
									{zone {zone "gamezone"}}
								}
								{exclude
									{state {state dead}}
								}
							}
						}
						{tag_add allied_wave_fresh}
						{tag_add allied_support}
						{tag_add cmp_def}
						{tag_add _def}
						{tag_add _ai_defender}
						{tag_remove hidden}
						{tag_remove allied_support_template}
						{tag_remove not_delete}
						{inactive off}
					}
					{"trigger" {name "allied_support/diag_clone_found"}}
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
					{terms
						{"1.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag allied_support_diag_source}
								{type human}
							}
							{count
								{op ">"}
								{value 0}
							}
						}
					}
				}
				{actions
					{"timer" {time 12} {title "SUPPORT 2A SOURCE FOUND"}}
				}
			}

			{"allied_support/diag_clone_found"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag allied_wave_fresh}
										}
									}
									{include
										{prop
											{prop human}
										}
										{zone
											{zone "gamezone"}
										}
									}
								}
							}
							{count
								{op ">"}
								{value 0}
							}
						}
					}
				}
				{actions
					{"timer" {time 12} {title "SUPPORT 3A CLONE IN ZONE"}}
				}
			}

			{"allied_support/diag_promoted_found"
				{condition
					{terms
						{"1.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag allied_wave_fresh}
								{type human}
							}
							{count
								{op ">"}
								{value 0}
							}
						}
					}
				}
				{actions
					{"timer" {time 12} {title "SUPPORT 4A PROMOTED FOUND"}}
				}
			}

			{"allied_support/diag_owner_found"
				{condition
					{terms
						{"1.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag allied_support_diag_owned}
								{type human}
							}
							{count
								{op ">"}
								{value 0}
							}
						}
					}
				}
				{actions
					{"timer" {time 12} {title "SUPPORT 5A OWNER FOUND"}}
				}
			}
