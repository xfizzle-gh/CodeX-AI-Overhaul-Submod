; TEST-ONLY attack-mate ownership and mission-script retask proof.
; Starts on a fixed delay so it does not depend on Lua-only prep variables.
; Run only in a human attack test. Tags up to six player infantry, orders
; them to fpc1, transfers them to AI ownership slot 3, then retasks to fpc2.

			{"attack_mate/probe_init"
				{condition
					{expression "1"}
					{terms
						{"1.cmp_i" {var "attack_mate_probe_started$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_mate_probe_started$"} {op "="} {value 1}}
					{"timer" {time 8} {title "ATTACK MATE PROBE ARMED FIXED DELAY"}}
					; Map load precedes GameThreadStart by roughly 25-30 seconds.
					; Forty seconds puts selection after the player army exists.
					{"delay" {time 40}}

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
