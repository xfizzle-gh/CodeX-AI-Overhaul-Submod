; PR B native-probe observability only.
; Repeats the final autodemo result on-screen so a tester cannot miss a transient talk.
; This file must not mutate morale/classification/gameplay state.
			{"conquest_enhanced_mechanics/morale/report_source_fail"
				{condition
					{terms
						{"1.cmp_i"
							{var "enable_ce_morale_autodemo$"}
							{op ">"}
							{value 0}
						}
						{"2.cmp_i"
							{var "ce_morale_autodemo_done$"}
							{op ">"}
							{value 0}
						}
						{"3.cmp_i"
							{var "ce_morale_source_tag_seen$"}
							{op "="}
							{value 0}
						}
					}
				}
				{actions
					{"talk"
						{selector
							{ignore_captured_by_user 0}
							{amount 1}
							{tag player}
							{zone "gamezone"}
							{type human}
							{state "not dead"}
						}
						{text "mission/multi/ce_morale_probe_source_fail"}
					}
					{"delay" {time 5}}
					{"trigger" {name "conquest_enhanced_mechanics/morale/report_source_fail"}}
				}
			}
			{"conquest_enhanced_mechanics/morale/report_regular_only"
				{condition
					{terms
						{"1.cmp_i" {var "enable_ce_morale_autodemo$"} {op ">"} {value 0}}
						{"2.cmp_i" {var "ce_morale_autodemo_done$"} {op ">"} {value 0}}
						{"3.cmp_i" {var "ce_morale_source_tag_seen$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "ce_morale_source_quality_seen$"} {op "="} {value 0}}
					}
				}
				{actions
					{"talk"
						{selector
							{ignore_captured_by_user 0}
							{amount 1}
							{tag player}
							{zone "gamezone"}
							{type human}
							{state "not dead"}
						}
						{text "mission/multi/ce_morale_probe_source_regular_only"}
					}
					{"delay" {time 5}}
					{"trigger" {name "conquest_enhanced_mechanics/morale/report_regular_only"}}
				}
			}
			{"conquest_enhanced_mechanics/morale/report_late_fail"
				{condition
					{terms
						{"1.cmp_i" {var "enable_ce_morale_autodemo$"} {op ">"} {value 0}}
						{"2.cmp_i" {var "ce_morale_autodemo_done$"} {op ">"} {value 0}}
						{"3.cmp_i" {var "ce_morale_source_quality_seen$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "ce_morale_late_seen$"} {op "="} {value 0}}
					}
				}
				{actions
					{"talk"
						{selector
							{ignore_captured_by_user 0}
							{amount 1}
							{tag player}
							{zone "gamezone"}
							{type human}
							{state "not dead"}
						}
						{text "mission/multi/ce_morale_probe_late_fail"}
					}
					{"delay" {time 5}}
					{"trigger" {name "conquest_enhanced_mechanics/morale/report_late_fail"}}
				}
			}
			{"conquest_enhanced_mechanics/morale/report_pass"
				{condition
					{terms
						{"1.cmp_i" {var "enable_ce_morale_autodemo$"} {op ">"} {value 0}}
						{"2.cmp_i" {var "ce_morale_autodemo_done$"} {op ">"} {value 0}}
						{"3.cmp_i" {var "ce_morale_source_quality_seen$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "ce_morale_late_seen$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"talk"
						{selector {ignore_captured_by_user 0} {amount 1} {tag player} {zone "gamezone"} {type human} {state "not dead"}}
						{text "mission/multi/ce_morale_probe_quality_ok"}
					}
					{"delay" {time 2}}
					{"talk"
						{selector {ignore_captured_by_user 0} {amount 1} {tag player} {zone "gamezone"} {type human} {state "not dead"}}
						{text "mission/multi/ce_morale_force_shaken"}
					}
					{"delay" {time 2}}
					{"talk"
						{selector {ignore_captured_by_user 0} {amount 1} {tag player} {zone "gamezone"} {type human} {state "not dead"}}
						{text "mission/multi/ce_morale_probe_late_ok"}
					}
					{"delay" {time 2}}
					{"talk"
						{selector {ignore_captured_by_user 0} {amount 1} {tag player} {zone "gamezone"} {type human} {state "not dead"}}
						{text "mission/multi/ce_morale_force_panic"}
					}
					{"delay" {time 5}}
					{"trigger" {name "conquest_enhanced_mechanics/morale/report_pass"}}
				}
			}
