; TEST-ONLY attack-mate ownership and mission-script retask proof.
; Human attack missions begin with no player/allied units, so this probe waits
; for attack preparation and live defender sources, clones four defenders at
; allied_support_entry, transfers the clones to AI slot 3, orders fpc1, then fpc2.

			{"attack_mate/probe_init"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "attack_mate_probe_started$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "prep_inform$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"4.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag {tag cmp_def}}
										{tag {tag _def}}
									}
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
									}
								}
							}
							{count {op ">"} {value 0}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_mate_probe_started$"} {op "="} {value 1}}
					{"timer" {time 8} {title "ATTACK MATE PROBE ARMED CLONE TEST"}}
					{"delay" {time 0.5}}

					; Use the ready flag defenders only as clone sources.
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag {tag cmp_def}}
									{tag {tag _def}}
								}
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
									{tag {tag attack_mate_probe}}
									{tag {tag attack_mate_src}}
									{tag {tag hidden}}
								}
							}
							{amount 4}
						}
						{tag_add attack_mate_src}
					}
					{"delay" {time 0.1}}

					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag attack_mate_src} {type human}}
								{count {op ">"} {value 0}}
							}
							{"placement"
								{selector
									{source advanced}
									{group {select {tag {tag attack_mate_src}}}}
									{amount 4}
								}
								{target_waypoint "allied_support_entry"}
								{clone}
							}
							{"delay" {time 0.5}}

							; Originals remain in the defended flag zones. Remove the source tag there.
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select {tag {tag attack_mate_src}}}
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
										{include {tag {tag attack_mate_src}}}
									}
								}
								{tag_remove attack_mate_src}
							}
							{"delay" {time 0.1}}

							; Remaining source-tagged actors are the entry clones.
							{"entity_state"
								{selector
									{source advanced}
									{group
										{select {tag {tag attack_mate_src}}}
										{include {prop {prop human}}}
										{exclude
											{state {state dead}}
											{state {state linked}}
											{state {state user_control}}
										}
									}
								}
								{tag_add attack_mate_probe}
								{tag_add attack_mate_owned}
								{tag_remove attack_mate_src}
								{tag_remove cmp_def}
								{tag_remove _def}
								{tag_remove _ai_defender}
								{tag_remove hidden}
								{tag_remove not_delete}
								{inactive off}
							}
							{"delay" {time 0.1}}

							{"switch"
								{"case"
									{condition
										{type entities}
										{selector {tag attack_mate_probe} {type human}}
										{count {op ">"} {value 0}}
									}
									{"timer" {time 8} {title "ATTACK MATE PROBE 1 CLONES READY"}}
									{"player"
										{selector {tag attack_mate_probe} {type human}}
										{operation set}
										{player "3"}
									}
									{"delay" {time 0.2}}
									{"actor_state"
										{selector {tag attack_mate_probe} {type human} {state "not dead"}}
										{control AI}
										{ai_move {mode enable}}
										{weapon_prepare on}
										{fire_mode open}
										{move_mode free}
										{movement {speed fast} {kind fast} {type normal}}
										{ai {no_retreat on} {advance_ratio 2} {retreat_ratio 0}}
									}
									{"ables"
										{selector {tag attack_mate_probe} {type human}}
										{remove select}
									}
									{"set_i" {var "attack_mate_probe_transferred$"} {op "="} {value 1}}
									{"timer" {time 8} {title "ATTACK MATE PROBE 2 TRANSFERRED"}}
									; First order is intentionally issued after AI ownership is established.
									{"action"
										{selector {tag attack_mate_probe} {type human} {state operatable}}
										{drop orders}
										{action advance}
										{target {tag fpc1}}
									}
									{"timer" {time 10} {title "ATTACK MATE PROBE 3 LEG1 ORDERED"}}
									{"trigger" {name "attack_mate/probe_retask"}}
									{"trigger" {name "attack_mate/probe_timeout"}}
								}
								{"default"
									{"timer" {time 15} {title "ATTACK MATE PROBE FAIL NO CLONES"}}
								}
							}
						}
						{"default"
							{"timer" {time 15} {title "ATTACK MATE PROBE FAIL NO DEFENDER SOURCES"}}
						}
					}
				}
			}

			{"attack_mate/probe_retask"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "attack_mate_probe_transferred$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "attack_mate_probe_retasked$"} {op "=="} {value 0}}
						{"3.entities"
							{selector
								{source advanced}
								{group
									{select {tag {tag attack_mate_probe}}}
									{include
										{prop {prop human}}
										{state {state operatable}}
										{zone {zone "fpc1"}}
									}
									{exclude {state {state dead}}}
								}
							}
							{count {op ">"} {value 0}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_mate_probe_retasked$"} {op "="} {value 1}}
					{"timer" {time 8} {title "ATTACK MATE PROBE FPC1 REACHED"}}
					{"action"
						{selector {tag attack_mate_probe} {type human} {state operatable}}
						{drop orders}
						{action advance}
						{target {tag fpc2}}
					}
					{"timer" {time 15} {title "ATTACK MATE PROBE 4 RETASKED TO FPC2"}}
				}
			}

			{"attack_mate/probe_timeout"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "attack_mate_probe_transferred$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "attack_mate_probe_retasked$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"delay" {time 180}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "attack_mate_probe_retasked$"} {op "=="} {value 0}}
							{"timer" {time 20} {title "ATTACK MATE PROBE TIMEOUT BEFORE FPC1"}}
						}
						{"default"}
					}
				}
			}
