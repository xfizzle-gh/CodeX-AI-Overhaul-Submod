; One-shot Indomitus-style proof using one explicit named actor embedded in every CWA map.
; Test only. Human defense, DefenderBot ownership, waypoint 1 / fpc1.
; No CE template-pool fallback and no repeating loop.

			{"allied_support/explicit_actor_once"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"3.cmp_i" {var "prep_inform$"} {op "=="} {value 1}}
					}
				}
				{actions
					{"delay" {time 10}}
					{"actor_to_waypoint"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{select {tag {tag allied_support_explicit_template}}}
								{include
									{prop {prop human}}
									{tag {tag hidden}}
								}
								{exclude
									{state {state dead}}
									{zone {zone "gamezone"}}
								}
							}
							{amount 1}
						}
						{waypoint "1"}
						{clone}
						{approach "safe teleport & rotate"}
					}
					{"delay" {time 0.5}}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag allied_support_explicit_template}}}
								{include
									{prop {prop human}}
									{zone {zone "gamezone"}}
								}
								{exclude {state {state dead}}}
							}
						}
						{tag_add allied_wave_fresh}
						{tag_add allied_support}
						{tag_add cmp_def}
						{tag_add _def}
						{tag_add _ai_defender}
						{tag_add allied_support_explicit_clone}
						{tag_remove allied_support_explicit_template}
						{tag_remove not_delete}
						{tag_remove hidden}
						{inactive off}
					}
					{"delay" {time 0.1}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 1}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "1"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_1} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 2}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "2"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_2} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 3}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "3"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_3} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 4}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "4"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_4} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 5}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "5"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_5} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 6}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "6"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_6} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 7}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "7"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_7} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 8}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "8"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_8} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 9}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "9"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_9} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 10}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "10"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_10} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 11}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "11"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_11} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 12}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "12"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_12} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 13}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "13"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_13} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 14}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "14"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_14} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 15}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "15"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_15} {tag_add allied_support_explicit_owned}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 16}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "16"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_16} {tag_add allied_support_explicit_owned}}
						}
						{"default"
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_unsupported}}
						}
					}
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
					{"entity_state" {selector {tag allied_wave_fresh}} {tag_remove allied_wave_fresh}}
				}
			}
