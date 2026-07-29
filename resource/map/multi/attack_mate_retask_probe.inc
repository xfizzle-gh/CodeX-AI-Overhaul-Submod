; TEST-ONLY attack-mate proof (human ATTACK only).
; prep_inform is NOT required (attack often never fires PrepTimeOver).
; 1) Immediate arm timer so we know the include loaded.
; 2) Source = the probe's OWN off-map pool, attack_mate_probe_templates.inc:
;    4 REAL-BREED (mp/nato/2022s/1ad_rifleman) humans, MID 9011-9014, parked at
;    -3000..-2700/-35100, statically tagged attack_mate_tpl + hidden. Every
;    selector anomaly below showed up only on breed-less {Human ""} actors, and
;    Indomitus/DOTD only ever prove this machinery on real breeds - hence the
;    switch. The breed-less allied_support_templates.inc pool is left untouched.
;    (was: 8 breed-less prototypes, MID 9001-9008, parked at -4100/-35100,
;    tagged allied_support_template + hidden + cmp_def.)
;    Live map-garrison defenders are NOT usable: cmp_def/_def/
;    _ai_defender is map-state-dependent (armed on woodland, never on border) and
;    purchased attack-mission squads never carry those tags.
; 3) Move 4 to attack_mate_entry, own to id_attacker_mate (fallback P3),
;    advance to the nearest capture point, then retask to the next one.
; 4) The prototypes now carry the breed's own loadout (m4a1_v3b, ammo, grenades),
;    so nothing has to be filled in on player assignment. What this probe proves
;    is ownership transfer plus movement/orders.
;
; FLAG ADDRESSING. fpc1..fpc5 is an Indomitus-40k naming convention and is NOT
; a reliable handle on CWA maps. Census of the 14 dcg_[cwa71]_* maps:
;   - fpc* tags:  present on 13, but outback has ZERO - which is exactly the
;     map the live run used, so {tag fpc1} had no target and the units stood
;     still forever. There is no runtime tag_add for fpc1..fpc5 anywhere.
;   - "flag" in the .mi {Tags} lines: only 3 maps (europe, factory,
;     train_station) - so authored tags are not the mechanism either.
;   - {Entity "flag_point_campaign_N"} with {Extender "map_point" {name fN}}:
;     present on ALL 14 maps, 2-5 per map. These are the real capture points.
; The engine tags those entities `flag`, which is why dcg_script.inc addresses
; capture points as {tag flag} throughout (e.g. lines 1208, 1231, 2770) rather
; than by any authored name. This probe does the same. If a run ever reports
; stage 51, that assumption is wrong on that map and {tag flag} found nothing.
;
; ENTRY ASYMMETRY. allied_support_entry is a DEFENSE-side anchor: on 13 of the 14
; CWA maps it sits at exactly the spawn_a centroid, and on fulda within 1.5k of it,
; i.e. the defender's rear. The defense wave engine wants that. This probe only
; ever runs with user_is_defender==0, and the attacking human is on the B side -
; live on outback the units teleported to allied_support_entry, landed in enemy
; territory and died. So the probe uses its own per-map waypoint attack_mate_entry,
; placed at that map's spawn_b centroid pulled 12% toward map centre so the units
; land just forward of the spawn line instead of stacking on the spawn markers.
;
; SELECTOR RULE (the hard-won lesson, learned on the breed-less pool). On those
; template entities, ANY decoration on an advanced selector - {include {prop
; {prop human}}}, {state ...} excludes, zone tests - silently reduces the match to
; zero. Live proof from one run: the simple form {selector {tag attack_mate_src}
; {type human}} gated the move and PASSED; the bare advanced form {group {select
; {tag {tag attack_mate_src}}}} moved all 4; the SAME select plus {include {prop
; {prop human}}} and state excludes matched NOTHING in the very next action.
; So: select by tag, bare, and nothing else. Everything downstream keys on
; attack_mate_src - the one tag proven queryable on those units - and that tag is
; deliberately never removed. attack_mate_probe/attack_mate_owned are still applied
; by promote, but purely as best-effort markers: nothing gates on them.
;
; NO-CLONE DECISION. This probe does not clone. Three separate promote designs
; each failed to match a single freshly created entity, all reaching stage 9:
;   1. select by the runtime tag the source carried - tag_add does not survive
;   2. zone separation via gamezone - the entry is a waypoint and is not inside
;      gamezone, so the test cut the wrong way in both directions
;   3. tagless identity: player-0 human with everything tagged excluded
; On this engine version a new entity's provenance is not visible to any selector
; we can express, so there is no way to hand it to the AI mate. Instead we MOVE
; the 4 pool originals to the entry waypoint and promote them in place: they keep
; attack_mate_src by construction, so the promote selector cannot miss.
; Cost: the 4-strong pool is consumed outright, so the probe runs once per match -
; the stage 8 default stops instead of retrying.
; The probe owns its pool outright now: attack_mate_tpl is read by nothing else in
; the mod, and allied_support_templates.inc is never selected, tagged or consumed.
; The new prototypes carry only {Able "-select"} (DOTD's parked form) instead of
; the old ram-Volume/{disabled} combo, so there is no baked visibility or physics
; state left to restore - which also removes the last suspect for the zero-match.

			{"attack_mate/probe_boot"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "attack_mate_probe_started$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"delay" {time 15}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "attack_mate_probe_started$"} {op "=="} {value 0}}
							{"timer" {time 15} {title "ATTACK MATE PROBE BOOT ATTACK SIDE"}}
							{"trigger" {name "attack_mate/probe_init"}}
						}
						{"default"}
					}
				}
			}

			{"attack_mate/probe_init"
				{condition
					{expression "1 & 2 & 3"}
					{terms
						{"1.cmp_i" {var "attack_mate_probe_started$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						; Pool presence. Simple selector, the form proven to work on these
						; parked templates. attack_mate_tpl is unique to our own 4.
						{"3.entities"
							{selector {tag attack_mate_tpl} {type human}}
							{count {op ">"} {value 0}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_mate_probe_started$"} {op "="} {value 1}}
					{"timer" {time 12} {title "ATTACK MATE PROBE ARMED CLONE TEST"}}
					{"delay" {time 5}}

					; Claim up to 4 parked prototypes. Bare select, no include/exclude:
					; attack_mate_tpl alone uniquely identifies the pool, and any
					; decoration here risks the same zero-match that killed promote.
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_mate_tpl}}}}
							{amount 4}
						}
						{tag_add attack_mate_pool}
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
							{"set_i" {var "attack_mate_probe_stage$"} {op "="} {value 1}}
							; MOVE, not clone: the same placement verb with the clone sub-node dropped,
							; which teleports the existing entities to the waypoint. Mirrors the
							; ce_functions.inc:2452-2463 form (advanced selector, amount, target_waypoint,
							; no clone). PROVEN: all 4 originals teleported with this exact selector.
							{"placement"
								{selector
									{source advanced}
									{group {select {tag {tag attack_mate_src}}}}
									{amount 4}
								}
								{target_waypoint "attack_mate_entry"}
							}
							{"set_i" {var "attack_mate_probe_stage$"} {op "="} {value 2}}
							{"delay" {time 0.5}}
							{"set_i" {var "attack_mate_probe_stage$"} {op "="} {value 21}}

							; Promote in place. Selector is byte-for-byte the placement's proven
							; form: bare select on attack_mate_src, no include, no exclude. The
							; previous version differed only by {include {prop {prop human}}} plus
							; state excludes and it matched zero.
							; attack_mate_src is NOT removed - the whole chain below keys on it.
							{"entity_state"
								{selector
									{source advanced}
									{group {select {tag {tag attack_mate_src}}}}
								}
								{tag_add attack_mate_probe}
								{tag_add attack_mate_owned}
								{tag_remove attack_mate_pool}
								{tag_remove attack_mate_tpl}
								{tag_remove hidden}
								{inactive off}
								{impregnability disabled}
								{discovered on}
							}
							{"delay" {time 0.1}}

							{"switch"
								{"case"
									{condition
										{type entities}
										{selector {tag attack_mate_src} {type human}}
										{count {op ">"} {value 0}}
									}
									{"set_i" {var "attack_mate_probe_stage$"} {op "="} {value 3}}
									{"timer" {time 8} {title "ATTACK MATE PROBE 1 CLONES READY"}}
									{"switch"
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "1"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "2"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "3"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "4"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "5"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "6"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "7"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "8"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "9"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "10"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "11"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "12"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "13"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "14"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "15"}}}
										{"case" {condition {type cmp_i} {var "id_attacker_mate$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}} {operation set} {player "16"}}}
										{"default"
											{"timer" {time 6} {title "ATTACK MATE PROBE OWNER FALLBACK P3"}}
											{"player"
												{selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}}
												{operation set}
												{player "3"}
											}
										}
									}
									{"delay" {time 0.3}}
									; State decorations dropped here too ({state "not dead"},
									; {state operatable}): same zero-match risk on breed-less units.
									{"actor_state"
										{selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}}
										{control AI}
										{ai_move {mode enable}}
										{weapon_prepare on}
										{fire_mode open}
										{move_mode free}
										{movement {speed fast} {kind fast} {type normal}}
										{ai {no_retreat on} {advance_ratio 2} {retreat_ratio 0}}
									}
									{"ables"
										{selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}}
										{remove select}
									}
									{"set_i" {var "attack_mate_probe_transferred$"} {op "="} {value 1}}
									{"set_i" {var "attack_mate_probe_stage$"} {op "="} {value 4}}
									{"timer" {time 8} {title "ATTACK MATE PROBE 2 TRANSFERRED"}}

									; Claim the capture point nearest our squad. Proximity idiom is
									; ai_enhance_dcg.inc:78-101: sort the candidate set by distance to a
									; reference entity, then {amount 1}. Tagging the pick lets the retask
									; step exclude it and pick a genuinely different one.
									{"entity_state"
										{selector
											{source advanced}
											{group {select {tag {tag flag}}}}
											{sort
												{type entity}
												{entity
													{source advanced}
													{group {select {tag {tag attack_mate_src}}}}
													{amount 1}
												}
											}
											{amount 1}
										}
										{tag_add attack_mate_flag1}
									}
									{"delay" {time 0.1}}
									{"switch"
										{"case"
											{condition
												{type entities}
												{selector {tag attack_mate_flag1}}
												{count {op ">"} {value 0}}
											}
											; Real target: advance to the claimed capture point. The old
											; fpc_inf_to_flag1 call is gone - it advanced to {tag fpc1} and its
											; only other useful bits (move_mode free, fast movement, no_retreat)
											; are already set by the actor_state above.
											{"action"
												{selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}}
												{drop orders}
												{action advance}
												{target {ignore_captured_by_user 0} {tag attack_mate_flag1}}
											}
											{"timer" {time 10} {title "ATTACK MATE PROBE 3 LEG1 ORDERED"}}
											{"trigger" {name "attack_mate/probe_retask"}}
											{"trigger" {name "attack_mate/probe_timeout"}}
										}
										{"default"
											{"set_i" {var "attack_mate_probe_stage$"} {op "="} {value 51}}
											{"timer" {time 15} {title "ATTACK MATE PROBE FAIL NO FLAG POINTS"}}
										}
									}
								}
								{"default"
									{"set_i" {var "attack_mate_probe_stage$"} {op "="} {value 9}}
									{"timer" {time 15} {title "ATTACK MATE PROBE FAIL NO CLONES"}}
								}
							}
						}
						{"default"
							{"set_i" {var "attack_mate_probe_stage$"} {op "="} {value 8}}
							{"timer" {time 15} {title "ATTACK MATE PROBE FAIL NO POOL"}}
							; STOP - no retry. The move consumes the originals, so a retry loop would
							; spin forever once the 4-strong pool is empty. started$ stays 1, which is
							; what both probe_init and probe_boot gate on.
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
						; Arrival without zones: the {type near} + {near_to} + {distance} idiom
						; from dcg_script.inc:2757-2775, which is how that file detects units at a
						; capture point. fpc zones do not exist on these maps.
						{"3.near"
							{units
								{source advanced}
								{group {select {tag {tag attack_mate_src}}}}
							}
							{near_to
								{source advanced}
								{ignore_captured_by_user 0}
								{group {select {tag {tag attack_mate_flag1}}}}
							}
							{distance 60}
						}
					}
				}
				{actions
					{"set_i" {var "attack_mate_probe_retasked$"} {op "="} {value 1}}
					{"timer" {time 8} {title "ATTACK MATE PROBE FLAG 1 REACHED"}}

					; Second target: nearest capture point that is NOT the one just reached.
					; Same proximity idiom, with the first pick excluded - that exclusion is
					; what makes this a retask rather than a re-issue of the same order.
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select {tag {tag flag}}}
								{exclude {tag {tag attack_mate_flag1}}}
							}
							{sort
								{type entity}
								{entity
									{source advanced}
									{group {select {tag {tag attack_mate_src}}}}
									{amount 1}
								}
							}
							{amount 1}
						}
						{tag_add attack_mate_flag2}
					}
					{"delay" {time 0.1}}
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag attack_mate_flag2}}
								{count {op ">"} {value 0}}
							}
							{"action"
								{selector {ignore_captured_by_user 0} {tag attack_mate_src} {type human}}
								{drop orders}
								{action advance}
								{target {ignore_captured_by_user 0} {tag attack_mate_flag2}}
							}
							{"set_i" {var "attack_mate_probe_stage$"} {op "="} {value 5}}
							{"timer" {time 15} {title "ATTACK MATE PROBE 4 RETASKED TO FLAG 2"}}
						}
						{"default"
							{"set_i" {var "attack_mate_probe_stage$"} {op "="} {value 52}}
							{"timer" {time 15} {title "ATTACK MATE PROBE ONLY ONE FLAG POINT"}}
						}
					}
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
							{"timer" {time 20} {title "ATTACK MATE PROBE TIMEOUT BEFORE FLAG 1"}}
						}
						{"default"}
					}
				}
			}
