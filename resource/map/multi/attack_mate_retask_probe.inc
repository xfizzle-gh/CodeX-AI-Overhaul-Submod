; Attack-side ownership and mission-script retask proof.
; Human attack only. Tags up to six player infantry, orders them to fpc1,
; transfers them to the proven Team A AI ownership slot, then retasks them
; to fpc2 only after at least one tagged actor enters fpc1.

			{"attack_mate/probe_init"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "prep_inform$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_mate_probe_started$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_mate_probe_started$"} {op "="} {value 1}}
					{"timer" {time 8} {title "ATTACK MATE PROBE ARMED"}}
					{"delay" {time 10}}

					; Prefer the normal player tag.
					{"entity_state"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{select {tag {tag player}}}
								{include
									{prop {prop human}}
									{state {state operatable}}
									{zone {zone "gamezone"}}
								}
								{exclude
									{state {state dead}}
									{state {state inactive}}
									{state {state linked}}
									{tag {tag attack_mate_probe}}
								}
							}
							{amount 6}
						}
						{tag_add attack_mate_probe}
					}
					{"delay" {time 0.1}}

					; Some conquest builds identify player infantry with _user instead.
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag attack_mate_probe} {type human} {state operatable}}
								{count {op ">"} {value 0}}
							}
						}
						{"default"
							{"entity_state"
								{selector
									{source advanced}
									{ignore_captured_by_user 0}
									{group
										{select {tag {tag _user}}}
										{include
											{prop {prop human}}
											{state {state operatable}}
											{zone {zone "gamezone"}}
										}
										{exclude
											{state {state dead}}
											{state {state inactive}}
											{state {state linked}}
											{tag {tag attack_mate_probe}}
										}
									}
									{amount 6}
								}
								{tag_add attack_mate_probe}
							}
						}
					}
					{"delay" {time 0.1}}

					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag attack_mate_probe} {type human} {state operatable}}
								{count {op ">"} {value 0}}
							}
							{"timer" {time 8} {title "ATTACK MATE PROBE 1 SELECTED"}}
							; Critical order: issue before ownership transfer.
							{"action"
								{selector {ignore_captured_by_user 0} {tag attack_mate_probe} {type human} {state operatable}}
								{drop orders}
								{action advance}
								{target {tag fpc1}}
							}
							{"timer" {time 8} {title "ATTACK MATE PROBE 2 LEG1 ORDERED"}}
							{"delay" {time 0.2}}
							; Live manual-transfer proof identified this ownership slot as player "3".
							{"player"
								{selector {ignore_captured_by_user 0} {tag attack_mate_probe} {type human}}
								{operation set}
								{player "3"}
							}
							{"delay" {time 0.2}}
							{"actor_state"
								{selector {ignore_captured_by_user 0} {tag attack_mate_probe} {type human} {state "not dead"}}
								{control AI}
								{ai_move {mode enable}}
								{weapon_prepare on}
								{fire_mode open}
								{move_mode free}
								{movement {speed fast} {kind fast} {type normal}}
								{ai {no_retreat on} {advance_ratio 2} {retreat_ratio 0}}
							}
							{"ables"
								{selector {ignore_captured_by_user 0} {tag attack_mate_probe} {type human}}
								{remove select}
							}
							{"entity_state"
								{selector {tag attack_mate_probe}}
								{tag_add attack_mate_owned}
							}
							{"set_i" {var "attack_mate_probe_transferred$"} {op "="} {value 1}}
							{"timer" {time 10} {title "ATTACK MATE PROBE 3 TRANSFERRED"}}
							{"trigger" {name "attack_mate/probe_retask"}}
							{"trigger" {name "attack_mate/probe_timeout"}}
						}
						{"default"
							{"timer" {time 15} {title "ATTACK MATE PROBE FAIL NO PLAYER INFANTRY"}}
						}
					}
				}
			}

			{"attack_mate/probe_retask"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_mate_probe_transferred$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_mate_probe_retasked$"} {op "=="} {value 0}}
						{"4.entities"
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
					; Critical second order: issue after ownership has already transferred.
					{"action"
						{selector {ignore_captured_by_user 0} {tag attack_mate_probe} {type human} {state operatable}}
						{drop orders}
						{action advance}
						{target {tag fpc2}}
					}
					{"timer" {time 15} {title "ATTACK MATE PROBE 4 RETASKED TO FPC2"}}
				}
			}

			{"attack_mate/probe_timeout"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_mate_probe_transferred$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_mate_probe_retasked$"} {op "=="} {value 0}}
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
