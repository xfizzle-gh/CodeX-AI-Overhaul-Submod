; Native allied-support inactive-clone bridge, issue #97.
;
; #110 proved:
;   genuine human-native Conquest actor -> automatic human->Mate handoff
;   -> Mate AI control -> bright shared terrain FoW.
;
; #111 exposed one concrete engine requirement:
;   placement {clone} rejects a source that is not inactive.
;
; This revision isolates that exact boundary. The parked support prototype stays
; INACTIVE while its ownership is temporarily changed to the real human player.
; The engine then clones that inactive, human-owned prototype at the attack entry.
; Only after the clone exists inside gamezone do we activate it, hand it to Mate,
; and apply the already-proven #110 control sequence.
;
; Telemetry is fail-closed: stage 11 means clone requested/waiting. Stage 12 is
; reached only if an actual human actor exists inside gamezone for the probe to claim.
; Do not deploy a normal player stage before this one-wave diagnostic finishes.

			(define "native_support_set_human_owner"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 1}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 2}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 3}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 4}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 5}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 6}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 7}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 8}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 9}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 10}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 11}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 12}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 13}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 14}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 15}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 16}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "16"}}}
					{"default"}
				}
			)

			(define "native_support_set_mate_owner"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "16"}}}
					{"default"}
				}
			)

			(define "native_support_claim_line" ; %0 = rusa/ukr/prc/nato
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag ally_sup_%0_line}}}
							{include
								{prop {prop human}}
								{tag {tag ally_sup_tpl}}
							}
							{exclude
								{zone {zone "gamezone"}}
								{state {state linked}}
							}
						}
						{amount 5}
					}
					{tag_add native_support_source}
				}
			)

			(define "native_support_claim_current_line"
				{"switch"
					{"case" {condition {type cmp_i} {var "support_templates_ally$"} {op "=="} {value 1}} ("native_support_claim_line" args rusa)}
					{"case" {condition {type cmp_i} {var "support_templates_ally$"} {op "=="} {value 2}} ("native_support_claim_line" args ukr)}
					{"case" {condition {type cmp_i} {var "support_templates_ally$"} {op "=="} {value 4}} ("native_support_claim_line" args prc)}
					{"default" ("native_support_claim_line" args nato)}
				}
			)

			(define "native_support_prepare_source"
				("native_support_set_human_owner")
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag native_support_source}}}
							{exclude {zone {zone "gamezone"}}}
						}
					}
					{inactive on}
					{impregnability enabled}
				}
			)

			(define "native_support_clone_to_entry"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement"
							{selector {source advanced} {group {select {tag {tag native_support_source}}} {include {prop {prop human}}} {exclude {zone {zone "gamezone"}}}}}
							{target_waypoint "attack_support_entry_a1"}
							{clone}
						}
					}
					{"default"
						{"placement"
							{selector {source advanced} {group {select {tag {tag native_support_source}}} {include {prop {prop human}}} {exclude {zone {zone "gamezone"}}}}}
							{target_waypoint "attack_support_entry_b1"}
							{clone}
						}
					}
				}
			)

			(define "native_support_restore_source"
				{"player"
					{selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}}
					{operation set}
					{player "0"}
				}
				{"entity_state"
					{selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}}
					{inactive on}
					{impregnability enabled}
					{tag_remove native_support_source}
				}
			)

			(define "native_support_mark_runtime_clone"
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {zone {zone "gamezone"}}}
							{include {prop {prop human}}}
							{exclude {state {state linked}}}
						}
						{amount 5}
					}
					{tag_add native_support_new}
					{tag_add native_support_live}
					{tag_remove native_support_source}
					{tag_remove ally_sup_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
			)

			(define "native_support_complete_handoff"
				("native_support_mark_runtime_clone")
				{"set_i" {var "native_support_stage$"} {op "="} {value 12}}
				{"actor_state"
					{selector {ignore_captured_by_user 0} {tag native_support_new}}
					{drop orders}
					{control user}
					{ai_move {mode disable}}
				}
				{"delay" {time 1}}
				("native_support_set_mate_owner")
				{"actor_state"
					{selector {ignore_captured_by_user 0} {tag native_support_new}}
					{control AI}
					{ai_move {mode enable}}
					{remove select}
				}
				{"set_i" {var "native_support_stage$"} {op "="} {value 2}}
				{"delay" {time 3}}
				{"entity_state"
					{selector {source advanced} {group {select {tag {tag flag}}} {exclude {state {state inactive}}}} {sort {type shuffle}} {amount 1}}
					{tag_add native_support_target}
				}
				{"action"
					{selector {ignore_captured_by_user 0} {tag native_support_new}}
					{drop orders}
					{action move}
					{target {selector {ignore_captured_by_user 0} {tag native_support_target}}}
				}
				{"entity_state" {selector {ignore_captured_by_user 0} {tag native_support_target}} {tag_remove native_support_target}}
				{"entity_state" {selector {ignore_captured_by_user 0} {tag native_support_new}} {tag_remove native_support_new}}
				{"set_i" {var "native_support_stage$"} {op "="} {value 3}}
				{"set_i" {var "native_support_wave_num$"} {op "+"} {value 1}}
				{"set_i" {var "native_support_waves_left$"} {op "-"} {value 1}}
				{"set_i" {var "native_support_busy$"} {op "="} {value 0}}
			)

			(define "native_support_issue_line"
				{"set_i" {var "native_support_busy$"} {op "="} {value 1}}
				{"set_i" {var "native_support_stage$"} {op "="} {value 10}}
				("native_support_claim_current_line")
				{"delay" {time 0.1}}
				("native_support_prepare_source")
				{"delay" {time 0.1}}
				{"set_i" {var "native_support_stage$"} {op "="} {value 11}}
				("native_support_clone_to_entry")
				{"delay" {time 0.25}}
				("native_support_restore_source")
			)

			{"native_support/init"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6 & 7 & 8"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_native_birth$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support_human$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "id_attack_support_mate$"} {op ">"} {value 0}}
						{"6.cmp_i" {var "native_support_armed$"} {op "=="} {value 0}}
						{"7.cmp_i" {var "support_templates_culled$"} {op "=="} {value 1}}
						{"8.cmp_i" {var "support_templates_ally$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "native_support_armed$"} {op "="} {value 1}}
					{"set_i" {var "native_support_stage$"} {op "="} {value 0}}
					{"set_i" {var "native_support_wave_num$"} {op "="} {value 0}}
					{"set_i" {var "native_support_waves_left$"} {op "="} {value 1}}
					{"set_i" {var "native_support_busy$"} {op "="} {value 0}}
					{"delay" {time 5}}
					{"trigger" {name "native_support/wave"}}
				}
			}

			{"native_support/wave"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "native_support_armed$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "native_support_busy$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "native_support_waves_left$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "support_templates_ally$"} {op ">"} {value 0}}
					}
				}
				{actions
					("native_support_issue_line")
				}
			}

			{"native_support/claim_clone"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "native_support_stage$"} {op "=="} {value 11}}
						{"2.entities"
							{selector
								{source advanced}
								{group
									{select {zone {zone "gamezone"}}}
									{include {prop {prop human}}}
									{exclude {state {state linked}}}
								}
							}
						}
					}
				}
				{actions
					("native_support_complete_handoff")
				}
			}
