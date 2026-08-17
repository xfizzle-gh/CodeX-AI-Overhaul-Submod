; Native allied-support birth bridge, issue #97.
;
; #110 proved:
;   genuine human-native Conquest actor -> automatic human->Mate handoff
;   -> Mate AI control -> bright shared terrain FoW.
;
; #111 v1 failed for a narrower reason. Its MI state reached stage 3 but game.log
; contained no human->Mate transfer and the user saw no unit. The v1 code assumed a
; runtime-added tag on the parked source survived {placement ... {clone}}. The existing
; production support source explicitly warns that cloned provenance is not expressible
; through its selectors, so this revision removes that assumption.
;
; Revision:
;   parked current-faction line source -> temporary human owner/user control
;   -> placement clone at the attack entry
;   -> identify the clone by RUNTIME STATE, not inherited tag:
;        inside gamezone + human + operatable + user_control + not inactive/linked
;   -> tag exactly five as native_support_new
;   -> restore parked source
;   -> proven #110 human->Mate handoff -> 3 sec settle -> move to flag.
;
; Diagnostic rule: do not deploy a player stage before this one-wave probe finishes.
; At this early point the temporary source is outside gamezone, so a human/user-controlled
; infantry actor inside gamezone is the clone candidate rather than the parked source.

			(define "native_support_set_human_owner"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 1}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 2}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 3}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_human$"} {op "=="} {value 4}} {"player" {selector {source advanced} {group {select {tag {tag native_support_source}}} {exclude {zone {zone "gamezone"}}}}} {operation set} {player "4"}}}
					{"default"}
				}
			)

			(define "native_support_set_mate_owner"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support_mate$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag native_support_new}} {operation set} {player "4"}}}
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
					{inactive off}
					{impregnability enabled}
				}
				{"actor_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag native_support_source}}}
							{include {prop {prop human}}}
							{exclude {zone {zone "gamezone"}}}
						}
					}
					{drop orders}
					{control user}
					{ai_move {mode disable}}
				}
			)

			(define "native_support_clone_to_entry"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement"
							{selector
								{source advanced}
								{group
									{select {tag {tag native_support_source}}}
									{include {prop {prop human}}}
									{exclude {zone {zone "gamezone"}}}
								}
							}
							{target_waypoint "attack_support_entry_a1"}
							{clone}
						}
					}
					{"default"
						{"placement"
							{selector
								{source advanced}
								{group
									{select {tag {tag native_support_source}}}
									{include {prop {prop human}}}
									{exclude {zone {zone "gamezone"}}}
								}
							}
							{target_waypoint "attack_support_entry_b1"}
							{clone}
						}
					}
				}
			)

			(define "native_support_mark_runtime_clone"
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select
								{zone {zone "gamezone"}}
								{state {state operatable}}
								{state {state user_control}}
							}
							{include {prop {prop human}}}
							{exclude
								{state {state inactive}}
								{state {state linked}}
							}
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

			(define "native_support_restore_source"
				{"player"
					{selector
						{source advanced}
						{group
							{select {tag {tag native_support_source}}}
							{exclude {zone {zone "gamezone"}}}
						}
					}
					{operation set}
					{player "0"}
				}
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
					{tag_remove native_support_source}
				}
			)

			(define "native_support_finish_clone"
				{"delay" {time 0.25}}

				; V2 FIX: do not search for an inherited source tag on the clone.
				("native_support_mark_runtime_clone")
				{"set_i" {var "native_support_stage$"} {op "="} {value 12}}
				("native_support_restore_source")

				; Match the accepted #110 handoff boundary.
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
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude {state {state inactive}}}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add native_support_target}
				}
				{"action"
					{selector {ignore_captured_by_user 0} {tag native_support_new}}
					{drop orders}
					{action move}
					{target {selector {ignore_captured_by_user 0} {tag native_support_target}}}
				}
				{"entity_state"
					{selector {ignore_captured_by_user 0} {tag native_support_target}}
					{tag_remove native_support_target}
				}
				{"entity_state"
					{selector {ignore_captured_by_user 0} {tag native_support_new}}
					{tag_remove native_support_new}
				}
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
				{"set_i" {var "native_support_stage$"} {op "="} {value 11}}
				{"delay" {time 0.1}}
				("native_support_clone_to_entry")
				("native_support_finish_clone")
			)

			; Guaranteed one-wave provenance test. No support roll.
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
					{"switch"
						{"case" {condition {type cmp_i} {var "support_templates_ally$"} {op "=="} {value 1}} {"set_i" {var "faction_support_army$"} {op "="} {value 1}}}
						{"case" {condition {type cmp_i} {var "support_templates_ally$"} {op "=="} {value 2}} {"set_i" {var "faction_support_army$"} {op "="} {value 2}}}
						{"case" {condition {type cmp_i} {var "support_templates_ally$"} {op "=="} {value 4}} {"set_i" {var "faction_support_army$"} {op "="} {value 4}}}
						{"default" {"set_i" {var "faction_support_army$"} {op "="} {value 3}}}
					}
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