; Focused #97/#102 follow-up: prove the automatic handoff itself on a genuine
; Dynamic Conquest human actor. This intentionally does NOT create support actors.
;
; Positive reference already proven by #102:
;   genuine native human actor -> manual transfer to Mate -> bright terrain FoW YES
;
; This probe changes only the transfer mechanism:
;   genuine native human actor -> MI automatic owner handoff -> Mate -> 3s settle -> move
;
; The first #110 native run proved the normal Mate controller was live and published
; attack_support_ready=1 / id_attack_support=1, while 34 genuine campaign actors moved
; Player 0 -> human Player 3. Yet the handoff never left stage 0. The old selector relied
; on a custom player tag that those Dynamic Conquest actors do not receive in this path.
;
; Select the source by the engine state that actually distinguishes the human's native
; campaign infantry at runtime: human + operatable + user_control, excluding inactive and
; linked actors. This is the same MI selector vocabulary used by working Conquest AI scripts
; to distinguish player-controlled actors from AI-controlled actors. No map-authored tag is
; required.

			(define "native_handoff_set_mate_owner"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}} {operation set} {player "16"}}}
					{"default"}
				}
			)

			{"attack_support/native_human_auto_handoff_probe"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"3.cmp_i" {var "native_handoff_probe_stage$"} {op "=="} {value 0}}
						{"4.entities"
							{selector
								{source advanced}
								{ignore_captured_by_user 0}
								{group
									{include
										{prop {prop human}}
										{state {state operatable}}
										{state {state user_control}}
									}
									{exclude
										{state {state inactive}}
										{state {state linked}}
									}
								}
							}
						}
					}
				}
				{actions
					{"set_i" {var "native_handoff_probe_stage$"} {op "="} {value 1}}
					{"entity_state"
						{selector
							{source advanced}
							{ignore_captured_by_user 0}
							{group
								{include
									{prop {prop human}}
									{state {state operatable}}
									{state {state user_control}}
								}
								{exclude
									{state {state inactive}}
									{state {state linked}}
								}
							}
							{amount 1}
						}
						{tag_add codex_native_handoff_probe}
					}
					{"delay" {time 0.2}}
					{"actor_state"
						{selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}}
						{drop orders}
						{control user}
						{ai_move {mode disable}}
					}
					{"delay" {time 1}}
					("native_handoff_set_mate_owner")
					{"actor_state"
						{selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}}
						{control AI}
						{ai_move {mode enable}}
						{remove select}
					}
					{"set_i" {var "native_handoff_probe_stage$"} {op "="} {value 2}}
					{"delay" {time 3}}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag flag}}}
								{exclude {state {state inactive}}}
							}
							{sort {type shuffle}}
							{amount 1}
						}
						{tag_add codex_native_handoff_target}
					}
					{"action"
						{selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}}
						{drop orders}
						{action move}
						{target
							{selector {ignore_captured_by_user 0} {tag codex_native_handoff_target}}
						}
					}
					{"set_i" {var "native_handoff_probe_stage$"} {op "="} {value 3}}
				}
			}
