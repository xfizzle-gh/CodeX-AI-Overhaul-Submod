; TMAI-style human-origin -> Mate handoff bridge for friendly attack support.
;
; Lifecycle under test:
;   1. attack_support.lua discovers the real human commander and Mate IDs.
;   2. BEFORE attack-support waves are armed, this bridge reassigns the parked
;      attack_support_tpl pool to the human player while the actors are still
;      hidden/inactive/off-map. This is intentionally different from #98, which
;      first made Player-0 actors human-owned only at the final delivery step.
;   3. Existing wave delivery activates/moves a selected batch. id_attack_support$
;      deliberately points at the HUMAN for this first ownership pass.
;   4. The deployed batch gets a short human/user-control dwell, then is transferred
;      automatically to the Mate AI.
;   5. After the TMAI-style 3s post-transfer settle, MI action move orders take over:
;      small infantry groups and individual vehicles are spread across distinct flags.
;
; This is a focused lifecycle experiment. It does not touch enemy Conquest AI.

			(define "tmai_set_human_owner"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "16"}}}
					{"default"}
				}
			)

			(define "tmai_set_mate_owner"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag %0}} {operation set} {player "16"}}}
					{"default"}
				}
			)

			; Seed the still-inactive support pool to the real human BEFORE waves can arm.
			{"attack_support/tmai_human_seed"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "tmai_handoff_prepare$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "tmai_handoff_prepared$"} {op "=="} {value 0}}
					}
				}
				{actions
					("tmai_set_human_owner" args attack_support_tpl)
					{"set_i" {var "tmai_handoff_prepared$"} {op "="} {value 1}}
				}
			}

			; Once enabled, repeatedly pick up only newly deployed attack_support_src actors.
			; Prior waves retain tmai_handoff_done and are never processed twice.
			{"attack_support/tmai_handoff"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "tmai_handoff_enabled$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "tmai_handoff_busy$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"set_i" {var "tmai_handoff_busy$"} {op "="} {value 1}}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag attack_support_src}}}
								{exclude {tag {tag tmai_handoff_done}}}
								{exclude {tag {tag tmai_handoff_pending}}}
							}
						}
						{tag_add tmai_handoff_pending}
					}

					; Mimic the pre-transfer human-owned phase: these actors are the player's
					; units, under user-control state, before the Mate receives them.
					{"actor_state"
						{selector {ignore_captured_by_user 0} {tag tmai_handoff_pending}}
						{drop orders}
						{control user}
						{ai_move {mode disable}}
					}
					{"delay" {time 3}}

					; Automatic equivalent of the player's TMAI transfer-button handoff.
					("tmai_set_mate_owner" args tmai_handoff_pending)
					{"actor_state"
						{selector {ignore_captured_by_user 0} {tag tmai_handoff_pending}}
						{control AI}
						{ai_move {mode enable}}
						{remove select}
					}

					; TMAI v0.17 settles newly transferred units before the strategy layer moves them.
					{"delay" {time 3}}

					; Clear temporary strategy tags from the previous batch.
					{"entity_state" {selector {tag tmai_support_flag1}} {tag_remove tmai_support_flag1}}
					{"entity_state" {selector {tag tmai_support_flag2}} {tag_remove tmai_support_flag2}}
					{"entity_state" {selector {tag tmai_support_flag3}} {tag_remove tmai_support_flag3}}
					{"entity_state" {selector {tag tmai_move_g1}} {tag_remove tmai_move_g1}}
					{"entity_state" {selector {tag tmai_move_g2}} {tag_remove tmai_move_g2}}
					{"entity_state" {selector {tag tmai_move_v1}} {tag_remove tmai_move_v1}}
					{"entity_state" {selector {tag tmai_move_v2}} {tag_remove tmai_move_v2}}

					; Three distinct active objectives, same target primitive the shipped wave engine uses.
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
						{tag_add tmai_support_flag1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag flag}}}
								{exclude {state {state inactive}}}
								{exclude {tag {tag tmai_support_flag1}}}
							}
							{sort {type shuffle}}
							{amount 1}
						}
						{tag_add tmai_support_flag2}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag flag}}}
								{exclude {state {state inactive}}}
								{exclude {tag {tag tmai_support_flag1}}}
								{exclude {tag {tag tmai_support_flag2}}}
							}
							{sort {type shuffle}}
							{amount 1}
						}
						{tag_add tmai_support_flag3}
					}

					; TMAI-style small infantry groups. Linked vehicle crews are excluded from
					; the infantry selectors and travel with their hulls.
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag tmai_handoff_pending}}}
								{include {prop {prop human}}}
								{exclude {state {state linked}}}
							}
							{amount 4}
						}
						{tag_add tmai_move_g1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag tmai_handoff_pending}}}
								{include {prop {prop human}}}
								{exclude {state {state linked}}}
								{exclude {tag {tag tmai_move_g1}}}
							}
						}
						{tag_add tmai_move_g2}
					}
					{"entity_state"
						{selector {ignore_captured_by_user 0} {tag tmai_handoff_pending} {type vehicle} {amount 1}}
						{tag_add tmai_move_v1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag tmai_handoff_pending}}}
								{include {type {type vehicle}}}
								{exclude {tag {tag tmai_move_v1}}}
							}
							{amount 1}
						}
						{tag_add tmai_move_v2}
					}

					; Use TMAI's MI movement style rather than BotApi CaptureFlag/SeekAndDestroy.
					{"action"
						{selector {ignore_captured_by_user 0} {tag tmai_move_g1}}
						{drop orders}
						{action move}
						{target {ignore_captured_by_user 0} {tag tmai_support_flag1}}
					}
					{"action"
						{selector {ignore_captured_by_user 0} {tag tmai_move_g2}}
						{drop orders}
						{action move}
						{target {ignore_captured_by_user 0} {tag tmai_support_flag2}}
					}
					{"action"
						{selector {ignore_captured_by_user 0} {tag tmai_move_v1}}
						{drop orders}
						{action move}
						{target {ignore_captured_by_user 0} {tag tmai_support_flag2}}
					}
					{"action"
						{selector {ignore_captured_by_user 0} {tag tmai_move_v2}}
						{drop orders}
						{action move}
						{target {ignore_captured_by_user 0} {tag tmai_support_flag3}}
					}

					{"entity_state"
						{selector {ignore_captured_by_user 0} {tag tmai_handoff_pending}}
						{tag_add tmai_handoff_done}
						{tag_remove tmai_handoff_pending}
					}
					{"set_i" {var "tmai_handoff_seq$"} {op "+"} {value 1}}
					{"set_i" {var "tmai_handoff_busy$"} {op "="} {value 0}}
					{"delay" {time 0.5}}
					{"trigger" {name "attack_support/tmai_handoff"}}
				}
			}
