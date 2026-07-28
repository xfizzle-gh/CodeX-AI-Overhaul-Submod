; Test-only allied support proof using CE's proven hidden cmp_def clone pool.
; Human-defense only. Two equipped infantry clones every 60 seconds.
; Clone at each map's allied_support_entry, assign to DefenderBot, then advance to fpc1.

			{"allied_support/hidden_cmp_def_clone_test"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "id_defenderbot$"} {op ">"} {value 0}}
						{"3.cmp_i" {var "prep_inform$"} {op "=="} {value 1}}
					}
				}
				{actions
					{"delay" {time 60}}

					; Match CE's human-defense source selector exactly: two hidden, off-map,
					; unlinked cmp_def humans. Amount 2 is intentional; CE uses 2 for this role.
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
							{amount 2}
						}
						{tag_add allied_support_clone_source}
					}
					{"delay" {time 0.1}}

					; Use the same placement/clone action as CE's defender setup.
					{"placement"
						{selector
							{source advanced}
							{group
								{select {tag {tag allied_support_clone_source}}}
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
							{amount 2}
						}
						{target_waypoint "allied_support_entry"}
						{clone}
					}
					{"delay" {time 0.5}}

					; Clones inherit allied_support_clone_source. Restrict every live action
					; to tagged humans inside gamezone so hidden source templates are untouched.
					{"entity_state"
						{selector {tag allied_support_clone_source} {zone "gamezone"} {type human}}
						{tag_add allied_wave_fresh}
						{tag_add allied_support_wave_issued}
						{tag_add allied_support}
						{tag_add cmp_def}
						{tag_add _def}
						{tag_add _ai_defender}
						{tag_remove hidden}
						{inactive off}
					}
					{"delay" {time 0.1}}

					; Player fields are literal in MI, so cover DefenderBot IDs 1 through 16.
					{"switch"
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 1}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "1"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_1}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 2}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "2"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_2}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 3}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "3"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_3}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 4}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "4"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_4}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 5}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "5"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_5}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 6}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "6"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_6}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 7}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "7"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_7}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 8}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "8"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_8}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 9}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "9"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_9}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 10}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "10"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_10}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 11}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "11"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_11}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 12}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "12"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_12}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 13}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "13"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_13}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 14}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "14"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_14}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 15}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "15"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_15}}
						}
						{"case"
							{condition {type cmp_i} {var "id_defenderbot$"} {op "=="} {value 16}}
							{"player" {selector {tag allied_wave_fresh}} {operation set} {player "16"}}
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_16}}
						}
						{"default"
							{"entity_state" {selector {tag allied_wave_fresh}} {tag_add allied_support_owner_unsupported}}
						}
					}

					{"actor_state"
						{selector {tag allied_wave_fresh} {zone "gamezone"} {type human}}
						{control AI}
						{weapon_prepare on}
						{fire_mode open}
						{move_mode free}
						{movement {speed assault}}
						{ai {no_retreat on}}
					}
					{"ables"
						{selector {tag allied_wave_fresh} {zone "gamezone"} {type human}}
						{remove select}
					}
					{"action"
						{selector {tag allied_wave_fresh} {zone "gamezone"} {type human} {state operatable}}
						{drop orders}
						{action advance}
						{target {tag fpc1}}
					}

					; Clear the persistent source tag from both clones and hidden templates.
					{"entity_state"
						{selector {tag allied_wave_fresh} {zone "gamezone"} {type human}}
						{tag_remove allied_wave_fresh}
						{tag_remove allied_support_clone_source}
					}
					{"entity_state"
						{selector {tag allied_support_clone_source} {tag hidden} {type human}}
						{tag_remove allied_support_clone_source}
					}
					{"trigger" {name "allied_support/hidden_cmp_def_clone_test"}}
				}
			}
