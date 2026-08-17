; Allied support command - ownership and Mate handoff.
;
; Reuses the mechanism confirmed working in #110, unchanged: tag the actor, keep it
; human-controlled briefly, run a literal 1-16 {player} switch keyed from
; id_attack_support$, switch to AI, wait 3 seconds, then move. The transfer is a
; tag-driven ownership switch, not a Lua call; the mate bot never issues it.
;
; Human ownership is NOT native Dynamic Conquest registration. Setting the owner to the
; human is an ownership operation only. Whether a cloned actor acquires genuine Conquest
; registration - and therefore terrain fog-of-war illumination - is the open question, and
; the gate below is the only evidence that settles it.
;
; THE GATE IS AN ENFORCED HOLD, NOT A PASS-THROUGH. The actor stops while still
; human-owned and does not advance until allied_support_cmd_fow_continue$ is set. A fast
; automatic transfer would destroy the single most important distinction in the design: if
; the isolated actor is already dark while human-owned, the clone birth has failed and the
; commander is irrelevant. That observation must be made deliberately, by eye, on a
; stationary actor.
;
; The hold is a self-retriggering poll, not a blocking wait: {"wait"} has no precedent
; anywhere in this codebase, while {"trigger" {name ...}} self-invocation is used
; throughout. Polling every 2s also means an operator release takes effect within 2s
; instead of always burning the full backstop. The backstop is 30 ticks x 2.0s = 60s;
; on expiry the hold releases and records allied_support_cmd_gate_auto$ = 1, and an
; auto-release is never counted as a pass.
;
; These actors are never tagged _lua_mi or _lua_ignore. Either tag would place them in the
; commander's exclude set permanently and make them uncommandable.

	{"allied_support_cmd/handoff_human"
		{condition
			{terms
				{"1.cmp_i" {var "allied_support_cmd_stage$"} {op "=="} {value 20}}
			}
		}
		{actions
			{"switch"
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "1"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "2"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "3"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "4"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "5"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "6"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "7"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "8"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "9"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "10"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "11"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "12"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "13"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "14"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "15"}}}
				{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}} {operation set} {player "16"}}}
			}
			{"entity_state"
				{selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}}
				{tag_add allied_support_cmd_human}
			}
			{"entity_state"
				{selector {ignore_captured_by_user 0} {tag allied_support_cmd_fresh}}
				{tag_remove allied_support_cmd_fresh}
			}
			{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 30}}
		}
	}

	{"allied_support_cmd/handoff_gate"
		{condition
			{terms
				{"1.cmp_i" {var "allied_support_cmd_stage$"} {op "=="} {value 30}}
			}
		}
		{actions
			{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 40}}
			{"set_i" {var "allied_support_cmd_gate_ticks$"} {op "="} {value 0}}
			{"switch"
				{"case"
					{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
					{"timer" {mode show} {text "ALLIED SUPPORT: FOW GATE - human-owned, holding, observe now"}}
				}
				{"default"}
			}
			{"trigger" {name "allied_support_cmd/handoff_gate_poll"}}
		}
	}

	{"allied_support_cmd/handoff_gate_poll"
		{condition}
		{actions
			{"delay" {time 2.0}}
			{"switch"
				{"case"
					{condition {type cmp_i} {var "allied_support_cmd_fow_continue$"} {op "=="} {value 1}}
					{"set_i" {var "allied_support_cmd_gate_auto$"} {op "="} {value 0}}
					{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 50}}
				}
				{"case"
					{condition {type cmp_i} {var "allied_support_cmd_gate_ticks$"} {op ">="} {value 30}}
					{"set_i" {var "allied_support_cmd_gate_auto$"} {op "="} {value 1}}
					{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 50}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {mode show} {text "ALLIED SUPPORT: FOW GATE auto-released, NOT a pass"}}
						}
						{"default"}
					}
				}
				{"default"
					{"set_i" {var "allied_support_cmd_gate_ticks$"} {op "+"} {value 1}}
					{"trigger" {name "allied_support_cmd/handoff_gate_poll"}}
				}
			}
		}
	}

	{"allied_support_cmd/handoff_mate"
		{condition
			{terms
				{"1.cmp_i" {var "allied_support_cmd_stage$"} {op "=="} {value 50}}
				{"2.cmp_i" {var "allied_support_cmd_mate_id$"} {op ">"} {value 0}}
			}
		}
		{actions
			{"switch"
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "1"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "2"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "3"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "4"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "5"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "6"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "7"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "8"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "9"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "10"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "11"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "12"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "13"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "14"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "15"}}}
				{"case" {condition {type cmp_i} {var "allied_support_cmd_mate_id$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}} {operation set} {player "16"}}}
			}
			{"actor_state"
				{selector {source standart} {ignore_captured_by_user 0} {tag allied_support_cmd_human}}
				{control AI}
			}
			{"entity_state"
				{selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}}
				{tag_add allied_support_cmd_mate}
			}
			{"entity_state"
				{selector {ignore_captured_by_user 0} {tag allied_support_cmd_human}}
				{tag_remove allied_support_cmd_human}
			}
			{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 60}}
			{"delay" {time 3.0}}
			{"set_i" {var "allied_support_cmd_stage$"} {op "="} {value 70}}
			{"switch"
				{"case"
					{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
					{"timer" {mode show} {text "ALLIED SUPPORT: mate-owned and settled, stage 70"}}
				}
				{"default"}
			}
		}
	}
