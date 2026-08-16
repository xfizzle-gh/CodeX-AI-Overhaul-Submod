; Focused #97/#102 follow-up: prove the automatic handoff itself on a genuine
; Dynamic Conquest human actor. This intentionally does NOT create support actors.
;
; Positive reference already proven by #102:
;   genuine native human actor -> manual transfer to Mate -> bright terrain FoW YES
;
; This probe changes only the transfer mechanism:
;   genuine native human actor -> MI automatic owner handoff -> Mate -> 3s settle -> move
;
; One native human infantry actor is claimed only after attack_support.lua has published
; the real Mate id. The existing {tag player} + {type human} selector is the same canonical
; player-actor selector already used by shipped support announcements. Parked support pools
; are excluded by provenance because they do not carry the player tag.

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
								{ignore_captured_by_user 0}
								{tag player}
								{type human}
								{state "not dead"}
							}
						}
					}
				}
				{actions
					{"set_i" {var "native_handoff_probe_stage$"} {op "="} {value 1}}

					; Claim exactly one genuine player infantry actor. No support-template tags
					; participate in this selector.
					{"entity_state"
						{selector
							{ignore_captured_by_user 0}
							{amount 1}
							{tag player}
							{type human}
							{state "not dead"}
						}
						{tag_add codex_native_handoff_probe}
					}
					{"delay" {time 0.2}}

					; Explicitly begin from the same human/user-controlled state as #102.
					{"actor_state"
						{selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}}
						{drop orders}
						{control user}
						{ai_move {mode disable}}
					}
					{"delay" {time 1}}

					; Automatic equivalent of the manual TMAI ownership handoff.
					("native_handoff_set_mate_owner")
					{"actor_state"
						{selector {ignore_captured_by_user 0} {tag codex_native_handoff_probe}}
						{control AI}
						{ai_move {mode enable}}
						{remove select}
					}
					{"set_i" {var "native_handoff_probe_stage$"} {op "="} {value 2}}

					; Match TMAI's observed post-transfer settle before issuing movement.
					{"delay" {time 3}}

					; Pick one active objective so the transferred native actor leaves the human
					; cluster and produces a clean terrain-FoW observation.
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
