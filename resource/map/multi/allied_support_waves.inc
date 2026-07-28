; Test-only allied support proof using real, equipped live DefenderBot infantry as clone sources.
; No empty Human "" template shells. Every wave clones exactly five live cmp_def humans.
; Human-defense only. First wave 60 seconds after preparation; repeats every 60 seconds.
; Sources are limited to fpc1..fpc5, so source markers can be removed without touching entry clones.

			{"allied_support/live_clone_test"
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

					; Select five real defenders at capture points. Never reuse prior support waves.
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag cmp_def}}}
								{include
									{prop {prop human}}
									{state {state operatable}}
									{zone {zone "gamezone"}}
								}
								{exclude
									{state {state dead}}
									{state {state inactive}}
									{state {state linked}}
									{state {state user_control}}
									{tag {tag hidden}}
									{tag {tag _user}}
									{tag {tag player}}
									{tag {tag allied_support_wave_issued}}
								}
							}
							{group
								{select
									{zone {zone "fpc1"}}
									{zone {zone "fpc2"}}
									{zone {zone "fpc3"}}
									{zone {zone "fpc4"}}
									{zone {zone "fpc5"}}
								}
								{include {tag {tag cmp_def}}}
							}
							{amount 5}
						}
						{tag_add allied_support_source}
					}
					{"delay" {time 0.1}}

					; Clones inherit the temporary source tag.
					{"placement"
						{selector {tag allied_support_source} {amount 5}}
						{target_waypoint "allied_support_entry"}
						{clone}
					}
					{"delay" {time 0.5}}

					; Remove the marker from originals, all selected inside FPC zones.
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag allied_support_source}}}
								{include {prop {prop human}}}
							}
							{group
								{select
									{zone {zone "fpc1"}}
									{zone {zone "fpc2"}}
									{zone {zone "fpc3"}}
									{zone {zone "fpc4"}}
									{zone {zone "fpc5"}}
								}
								{include {tag {tag allied_support_source}}}
							}
						}
						{tag_remove allied_support_source}
					}
					{"delay" {time 0.1}}

					; Remaining source-tagged humans are the five entry clones.
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag allied_support_source}}}
								{include
									{prop {prop human}}
									{state {state operatable}}
									{zone {zone "gamezone"}}
								}
								{exclude
									{state {state dead}}
									{state {state linked}}
									{state {state user_control}}
								}
							}
						}
						{tag_add allied_wave_fresh}
						{tag_add allied_support_wave_issued}
						{tag_add allied_support}
						{tag_add cmp_def}
						{tag_add _def}
						{tag_add _ai_defender}
						{tag_remove hidden}
						{tag_remove allied_support_source}
						{inactive off}
					}
					{"delay" {time 0.1}}

					; Player fields are literal in MI, so cover IDs 1 through 16.
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
						{selector {tag allied_wave_fresh}}
						{control AI}
						{ai {no_retreat on}}
					}
					{"action"
						{selector {tag allied_wave_fresh}}
						{drop orders}
						{action advance}
						{target {tag fpc1}}
					}
					{"entity_state" {selector {tag allied_wave_fresh}} {tag_remove allied_wave_fresh}}
					{"trigger" {name "allied_support/live_clone_test"}}
				}
			}
