; Attack support waves â€” the human attacker's AI teammate reinforcement engine.
; Included by all fourteen CWA campaign_capture_the_flag.mi maps. Runs ONLY when the
; human is attacking; attack_support.lua publishes id_attack_support$ /
; attack_support_ready$ / attack_support_use_mi$ and nothing here moves until it has.
;
; DELIVERY (proven, do not deviate): off-map real-breed prototypes in
; attack_support_templates.inc are MOVED to the attack-side entry waypoint with
; {"placement"} and promoted in place. Never cloned: three promote designs each
; matched zero freshly created entities, because a new entity's provenance is
; invisible to every selector this format can express. Ownership is a literal 1-16
; {player} switch on id_attack_support$ â€” the engine will not accept a var there.
;
; SCHEDULE
;   opening wave  30/38/45s after the identity is published, one small rifle team
;   recurring     self-re-arming clock, randomized 120/150/180/210/240s cadence (~20% each)
;                 (~20% tighter than the old 150-300s ladder so wave 2 is ~3-5 min, not 5-6)
;   budget        defense_level$ (CE campaign progression, 1-3): L1 4 waves,
;                 L2 6, L3 8 - including the opening wave. Level 0 falls back to L1.
;
; COMPOSITION POOL, one drawn at random per wave, widening with the level:
;   1 usmc     5 USMC infantry                        L1 L2 L3   (also the pool-short fallback)
;   2 1ad      5 1AD infantry                         L1 L2 L3
;   5 arf      5 ARF/NATO infantry                    L1 L2 L3
;   3 acav     4 1AD infantry + 1 crewed humvee          L2 L3
;   4 pzgren   6 German pzgren + 2 crewed humvees           L3
; Waves stay deliberately small: this is a fireteam of support, not a second army.
; Place is staggered one-body-at-a-time then immediate advance off the pad (no corpse piles).
;
; SAFETY GATES on every wave: the readiness gates above, the wave budget, and a
; live-unit cap (defer without consuming a wave while more than 14 attack-support
; infantry are alive). A composition whose pool cannot field it clears no command,
; so the pick falls back to the USMC rifle team and then gives up for that cycle.
;
; Vehicles deploy one at a time with a gap, because two humvees placed together
; clip and flip. Movement is {action advance} on a claimed flag after a slow
; {"placement"}, never a beeline from a raw coordinate.
;
; DIAGNOSTICS / support_debug$ - the ship toggle. Every on-screen {"timer" ...} line in
; this file is a developer diagnostic, never player-facing text, and every one of them is
; wrapped in the same gate:
;   {"switch"
;     {"case" {condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}} <timer> }
;     {"default"}
;   }
; support_debug$ is declared in dcg_vars.inc and nothing ever writes it, so it reads 0 and
; the shipped default is OFF: a player sees zero support timers on any of the fourteen maps.
; Flipping that one var to 1 - a single {"set_i" {var "support_debug$"} {op "="} {value 1}}
; anywhere that runs, or sc:SetVar("support_debug", 1) from Lua - restores every diagnostic
; in all four engines at once. Never add an ungated timer: tools/deploy_attack_support_probe.ps1
; and the test suite both fail the build the moment one appears.
; PLAYER ANNOUNCEMENTS / support_announce$ - separate gate (default ON via init).
; Player-facing radio uses support_announce$ + {"talk"} (soldier portrait), never support_debug$. Declared in dcg_vars.inc.
;
; ENTRY POINTS / triple pads. Each side's map-edge arrival is THREE waypoints, not one:
; attack_support_entry_a1/a2/a3 and b1/b2/b3, an equilateral triangle of side ~27m whose
; first point sits exactly on that side's spawn centroid. Point 1 is the same coordinate
; the legacy attack_support_entry_a / _b still carry, so those names survive as aliases
; and the patrol / roam move orders that use them are unchanged. All six are (re)built
; from the repo maps by tools/deploy_attack_support_probe.ps1, idempotently.
; FLANK PADS / attack_support_flank_<side>1/2 at ~50% depth. as_choose_entry rolls 25%
; per batch; enemy-near-pad guard falls back to main edge pads. Q1 only.
; AIRMOBILE (Phase 5 E1): narrative helo insert at attack_support_air_<side>1/2 (~65% depth).
; Production: L2+ ~15% roll, cap attack_support_air_left$=2, cmd 18.
; DAY-2 TEST: attack_support_air_test$=1 (default) forces first insert ~30s after arm.
; Set attack_support_air_test$=0 in init to restore production-only schedule.
;
; am_entry_next bumps attack_support_entry_rr$ (1 -> 2 -> 3 -> 1) once per placement batch, at the top
; of am_place_at_entry, and the placement site is a three-case cascade on it. So consecutive
; batches never land on the same pad - including G1 and G2 of one wave, which is what was
; dropping bodies on top of each other and killing them on arrival.

			; Rotating entry point, bumped once per placement batch so no two
			; consecutive batches land on the same pad - including G1 and G2 of a
			; single wave, which is what was stacking bodies on top of each other.
			(define "am_entry_next"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "attack_support_entry_rr$"} {op ">="} {value 3}}
						{"set_i" {var "attack_support_entry_rr$"} {op "="} {value 1}}
					}
					{"default"
						{"set_i" {var "attack_support_entry_rr$"} {op "+"} {value 1}}
					}
				}
			)

			; Player-facing radio talk (support_announce$ default ON via init).
			; Uses stock {"talk"} so a live player soldier portrait delivers the line.
			; Distinct from support_debug$ diagnostics. Never mix the two gates.
			(define "as_announce_wave"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"talk"
							{selector
								{ignore_captured_by_user 0}
								{amount 1}
								{tag player}
								{zone "gamezone"}
								{type human}
								{state "not dead"}
							}
							{text "mission/multi/support/wave_inbound"}
						}
					}
					{"default"}
				}
			)
			(define "as_announce_vehicle"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"talk"
							{selector
								{ignore_captured_by_user 0}
								{amount 1}
								{tag player}
								{zone "gamezone"}
								{type human}
								{state "not dead"}
							}
							{text "mission/multi/support/vehicle_inbound"}
						}
					}
					{"default"}
				}
			)
			(define "as_announce_flank"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"talk"
							{selector
								{ignore_captured_by_user 0}
								{amount 1}
								{tag player}
								{zone "gamezone"}
								{type human}
								{state "not dead"}
							}
							{text "mission/multi/support/flank_inbound"}
						}
					}
					{"default"}
				}
			)
			
			(define "as_announce_motor"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"talk"
							{selector
								{ignore_captured_by_user 0}
								{amount 1}
								{tag player}
								{zone "gamezone"}
								{type human}
								{state "not dead"}
							}
							{text "mission/multi/support/motorized_inbound"}
						}
					}
					{"default"}
				}
			)

			(define "as_announce_airborne"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 1}}
								{"talk"
									{selector
										{ignore_captured_by_user 0}
										{amount 1}
										{tag player}
										{zone "gamezone"}
										{type human}
										{state "not dead"}
									}
									{text "mission/multi/support/airborne_inbound_rusa"}
								}
							}
							{"case"
								{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 2}}
								{"talk"
									{selector
										{ignore_captured_by_user 0}
										{amount 1}
										{tag player}
										{zone "gamezone"}
										{type human}
										{state "not dead"}
									}
									{text "mission/multi/support/airborne_inbound_ukr"}
								}
							}
							{"case"
								{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 4}}
								{"talk"
									{selector
										{ignore_captured_by_user 0}
										{amount 1}
										{tag player}
										{zone "gamezone"}
										{type human}
										{state "not dead"}
									}
									{text "mission/multi/support/airborne_inbound_prc"}
								}
							}
							{"default"
								{"talk"
									{selector
										{ignore_captured_by_user 0}
										{amount 1}
										{tag player}
										{zone "gamezone"}
										{type human}
										{state "not dead"}
									}
									{text "mission/multi/support/airborne_inbound_nato"}
								}
							}
						}
					}
					{"default"}
				}
			)

			(define "as_announce_exhausted"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"talk"
							{selector
								{ignore_captured_by_user 0}
								{amount 1}
								{tag player}
								{zone "gamezone"}
								{type human}
								{state "not dead"}
							}
							{text "mission/multi/support/waves_exhausted"}
						}
					}
					{"default"}
				}
			)

			; Place a single currently-tagged body, then clear the one-shot tag.
			(define "am_place_one"
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag attack_support_deploy}}}
							{exclude {tag {tag attack_support_placed}}}
						}
						{amount 1}
					}
					{tag_add attack_support_place_one}
				}
				{"delay" {time 0.05}}
				{"switch"
					{"case"
						{condition {type entities} {selector {tag attack_support_place_one}}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "attack_support_use_air$"} {op "=="} {value 1}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_flank_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_air_b2"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_air_b1"}
												}
											}
										}
									}
									{"case"
										{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_flank_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_air_a2"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_air_a1"}
												}
											}
										}
									}
									{"default"
										{"placement"
											{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
											{target_waypoint "attack_support_air_b1"}
										}
									}
								}
							}
							{"case"
								{condition {type cmp_i} {var "attack_support_use_flank$"} {op "=="} {value 1}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_flank_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_flank_b2"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_flank_b1"}
												}
											}
										}
									}
									{"case"
										{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_flank_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_flank_a2"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_flank_a1"}
												}
											}
										}
									}
									{"default"
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_flank_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_flank_b2"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_flank_b1"}
												}
											}
										}
									}
								}
							}
							{"default"
								{"switch"
									{"case"
										{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_entry_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_b2"}
												}
											}
											{"case"
												{condition {type cmp_i} {var "attack_support_entry_rr$"} {op "=="} {value 3}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_b3"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_b1"}
												}
											}
										}
									}
									{"case"
										{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_entry_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_a2"}
												}
											}
											{"case"
												{condition {type cmp_i} {var "attack_support_entry_rr$"} {op "=="} {value 3}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_a3"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_a1"}
												}
											}
										}
									}
									{"default"
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_entry_rr$"} {op "=="} {value 2}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_b2"}
												}
											}
											{"case"
												{condition {type cmp_i} {var "attack_support_entry_rr$"} {op "=="} {value 3}}
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_b3"}
												}
											}
											{"default"
												{"placement"
													{selector {ignore_captured_by_user 0} {tag attack_support_place_one}}
													{target_waypoint "attack_support_entry_b1"}
												}
											}
										}
									}
								}
							}
						}
						{"entity_state"
							{selector {tag attack_support_place_one}}
							{tag_add attack_support_placed}
							{tag_remove attack_support_place_one}
						}
						{"delay" {time 0.2}}
					}
					{"default"}
				}
			)


			; Stagger up to 8 bodies onto the entry (same waypoint, spaced in time so
			; physics does not crush a full fireteam into one pile).
			
			; Pick main triple pads vs flank pads for this placement batch (attack only).
			; 25% flank roll; round-robin flank_rr 1/2; fall back to main if enemies near pad.
			(define "as_choose_entry"
				{"set_i" {var "attack_support_use_flank$"} {op "="} {value 0}}
				; Airmobile batch already chose LZ — do not also roll flank.
				{"switch"
					{"case"
						{condition {type cmp_i} {var "attack_support_use_air$"} {op "=="} {value 1}}
					}
					{"default"
						{"switch"
							{"case"
								{condition {type rand} {value 0.25}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_flank_rr$"} {op ">="} {value 2}}
										{"set_i" {var "attack_support_flank_rr$"} {op "="} {value 1}}
									}
									{"default"
										{"set_i" {var "attack_support_flank_rr$"} {op "+"} {value 1}}
									}
								}
								; Safety: if any live bot human is already within 120 of the chosen
								; flank pad, stay on the main edge pads this wave.
								{"switch"
									{"case"
										{condition
											{type entities}
											{selector
												{ignore_captured_by_user 0}
												{type human}
												{state "not dead"}
												{tag _bot}
												{distance 120}
											}
											{count {op ">"} {value 0}}
										}
										{"set_i" {var "attack_support_use_flank$"} {op "="} {value 0}}
									}
									{"default"
										{"set_i" {var "attack_support_use_flank$"} {op "="} {value 1}}
										("as_announce_flank")
									}
								}
							}
							{"default"
								{"set_i" {var "attack_support_use_flank$"} {op "="} {value 0}}
							}
						}
					}
				}
			)

			(define "am_place_at_entry"
				("as_choose_entry")
				("am_entry_next")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				("am_place_one")
				{"entity_state"
					{selector {tag attack_support_placed}}
					{tag_remove attack_support_placed}
				}
			)

			(define "am_own_to_support"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag attack_support_deploy}} {operation set} {player "16"}}}
					{"default"
						{"player"
							{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
							{operation set}
							{player "3"}
						}
					}
				}
			)

			; Promote + own + slow MOVE (not a sprint advance). Split into staggered
			; fireteams so a wave does not walk one line. attack_support_src is the live
			; attack-support roster marker and is never removed: the live-unit cap counts it.
			(define "am_finish_deploy"
				{"delay" {time 0.5}}
				; Fresh scatter picks each deploy (clear previous wave flag tags).
				{"entity_state"
					{selector {tag attack_support_flag1}}
					{tag_remove attack_support_flag1}
				}
				{"entity_state"
					{selector {tag attack_support_flag2}}
					{tag_remove attack_support_flag2}
				}
				{"entity_state"
					{selector {tag attack_support_flag3}}
					{tag_remove attack_support_flag3}
				}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag attack_support_deploy}}}}
					}
					{tag_add attack_support_src}
					{tag_remove attack_support_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
				{"delay" {time 0.2}}
				("am_own_to_support")
				{"delay" {time 0.4}}
				{"actor_state"
					{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
					{control AI}
					{ai_move {mode enable}}
					{weapon_prepare on}
					{fire_mode open}
					{move_mode free}
					{movement {speed normal} {kind normal} {type normal}}
					{ai {no_retreat on} {advance_ratio 1} {retreat_ratio 0}}
				}
				{"ables"
					{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
					{remove select}
				}
				; Brief settle only â€” long pad dwell was stacking corpses at the entry.
				{"delay" {time 0.25}}
				; Up to 3 active flags, shuffled â€” different elements peel different ways.
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
					{tag_add attack_support_flag1}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
								{tag {tag attack_support_flag1}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add attack_support_flag2}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
								{tag {tag attack_support_flag1}}
								{tag {tag attack_support_flag2}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add attack_support_flag3}
				}
				{"delay" {time 0.1}}
				; Split deploy into staggered fireteams so they do not laser as one blob.
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag attack_support_deploy}}}}
						{amount 2}
					}
					{tag_add attack_support_g1}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag attack_support_deploy}}}
							{exclude {tag {tag attack_support_g1}}}
						}
						{amount 2}
					}
					{tag_add attack_support_g2}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag attack_support_deploy}}}
							{exclude
								{tag {tag attack_support_g1}}
								{tag {tag attack_support_g2}}
							}
						}
						{amount 2}
					}
					{tag_add attack_support_g3}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag attack_support_deploy}}}
							{exclude
								{tag {tag attack_support_g1}}
								{tag {tag attack_support_g2}}
								{tag {tag attack_support_g3}}
							}
						}
					}
					{tag_add attack_support_g4}
				}
				; Get everyone OFF the pad immediately (staggered peels, no multi-second dwell).
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_g1}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag1}}
				}
				{"delay" {time 0.35}}
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_g2}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag2}}
				}
				{"delay" {time 0.35}}
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_g3}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag1}}
				}
				{"delay" {time 0.35}}
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_g4}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag3}}
				}
				{"delay" {time 0.2}}
								; Final player handoff for terrain FoW. Keep AI control and selection disabled above.
				{"autoassign"
					{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
				}
{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag attack_support_deploy}}}}
					}
					{tag_remove attack_support_deploy}
					{tag_remove attack_support_g1}
					{tag_remove attack_support_g2}
					{tag_remove attack_support_g3}
					{tag_remove attack_support_g4}
				}
				{"entity_state"
					{selector {tag attack_support_flag3}}
					{tag_remove attack_support_flag3}
				}
				{"set_i" {var "attack_support_transferred$"} {op "="} {value 1}}
			)

			; Vehicle instances are consumed in declaration order, tracked by
			; attack_support_hmmwv_left$. Each humvee's crew is {Link}ed to that specific
			; hull, so an instance has to move as a whole - never select a count of bodies
			; out of a shared vehicle pool, or a hull arrives without its driver.
			(define "am_deploy_next_hmmwv"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "attack_support_hmmwv_left$"} {op "=="} {value 4}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_hmmwv1}}}}
							}
							{tag_add attack_support_deploy}
						}
						{"set_i" {var "attack_support_hmmwv_left$"} {op "-"} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "attack_support_hmmwv_left$"} {op "=="} {value 3}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_hmmwv2}}}}
							}
							{tag_add attack_support_deploy}
						}
						{"set_i" {var "attack_support_hmmwv_left$"} {op "-"} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "attack_support_hmmwv_left$"} {op "=="} {value 2}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_hmmwv3}}}}
							}
							{tag_add attack_support_deploy}
						}
						{"set_i" {var "attack_support_hmmwv_left$"} {op "-"} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "attack_support_hmmwv_left$"} {op "=="} {value 1}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag attack_support_hmmwv4}}}}
							}
							{tag_add attack_support_deploy}
						}
						{"set_i" {var "attack_support_hmmwv_left$"} {op "-"} {value 1}}
					}
					{"default"}
				}
				{"delay" {time 0.1}}
				("am_place_at_entry")
				("am_finish_deploy")
			)

			; Player nation -> faction_support_army$ (1 rusa / 2 ukr / 3 nato / 4 prc).
			; Same fold as enemy bot_army: sov+pol->rusa, csa+frg->nato.
			(define "as_resolve_army"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 2}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 2}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 6}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 4}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 1}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 5}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 8}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 1}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 3}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 3}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 4}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 3}}
					}
					{"case"
						{condition {type cmp_i} {var "user_nation$"} {op "=="} {value 7}}
						{"set_i" {var "faction_support_army$"} {op "="} {value 3}}
					}
					{"default"
						; Fall back to hint, then NATO.
						{"switch"
							{"case"
								{condition {type cmp_i} {var "user_nation_hint$"} {op "=="} {value 1}}
								{"set_i" {var "faction_support_army$"} {op "="} {value 1}}
							}
							{"default"
								{"set_i" {var "faction_support_army$"} {op "="} {value 3}}
							}
						}
					}
				}
			)

			; cmd 10 line 11 wpn 12 recon 13 assault 14 eng 15 manpad 16 light veh
			; 17 IFV 18 airmobile 19 motorized truck (RUSA/UKR/NATO/PRC sx2190 passenger)
			; NATO specialty cmds 1-5 still used when faction_support_army$ == 3.
			(define "as_poke_faction_line"
				{"trigger" {name "attack_support/ally_rusa_line"}}
				{"trigger" {name "attack_support/ally_ukr_line"}}
				{"trigger" {name "attack_support/ally_prc_line"}}
				{"trigger" {name "attack_support/ally_nato_line"}}
			)
			(define "as_poke_faction_wpn"
				{"trigger" {name "attack_support/ally_rusa_wpn"}}
				{"trigger" {name "attack_support/ally_ukr_wpn"}}
				{"trigger" {name "attack_support/ally_prc_wpn"}}
				{"trigger" {name "attack_support/ally_nato_wpn"}}
			)
			(define "as_poke_faction_recon"
				{"trigger" {name "attack_support/ally_rusa_recon"}}
				{"trigger" {name "attack_support/ally_ukr_recon"}}
				{"trigger" {name "attack_support/ally_prc_recon"}}
				{"trigger" {name "attack_support/ally_nato_recon"}}
			)
			(define "as_poke_faction_assault"
				{"trigger" {name "attack_support/ally_rusa_assault"}}
				{"trigger" {name "attack_support/ally_ukr_assault"}}
				{"trigger" {name "attack_support/ally_prc_assault"}}
				{"trigger" {name "attack_support/ally_nato_assault"}}
			)
			(define "as_poke_faction_eng"
				{"trigger" {name "attack_support/ally_rusa_eng"}}
				{"trigger" {name "attack_support/ally_ukr_eng"}}
				{"trigger" {name "attack_support/ally_prc_eng"}}
				{"trigger" {name "attack_support/ally_nato_eng"}}
			)
			(define "as_poke_faction_manpad"
				{"trigger" {name "attack_support/ally_rusa_manpad"}}
				{"trigger" {name "attack_support/ally_ukr_manpad"}}
				{"trigger" {name "attack_support/ally_prc_manpad"}}
				{"trigger" {name "attack_support/ally_nato_manpad"}}
			)
			(define "as_poke_faction_air"
				{"trigger" {name "attack_support/ally_rusa_air"}}
				{"trigger" {name "attack_support/ally_ukr_air"}}
				{"trigger" {name "attack_support/ally_prc_air"}}
				{"trigger" {name "attack_support/ally_nato_air"}}
			)
			(define "as_poke_faction_ifv"
				{"trigger" {name "attack_support/ally_rusa_ifv"}}
				{"trigger" {name "attack_support/ally_ukr_ifv"}}
				{"trigger" {name "attack_support/ally_prc_ifv"}}
				{"trigger" {name "attack_support/ally_nato_ifv"}}
			)
			; PRC uses shaanxi_sx2190_passenger (group passengers). PLA IFVs still use group crew.
			(define "as_poke_faction_motor"
				{"trigger" {name "attack_support/ally_rusa_motor"}}
				{"trigger" {name "attack_support/ally_ukr_motor"}}
				{"trigger" {name "attack_support/ally_prc_motor"}}
				{"trigger" {name "attack_support/ally_nato_motor"}}
			)
			(define "as_poke_faction_veh"
				{"trigger" {name "attack_support/ally_ukr_veh"}}
				{"trigger" {name "attack_support/ally_nato_veh"}}
			)

			; Non-NATO hybrid pick by level. L1 line/recon; L2+ widens; L3 rare AA/veh.
			(define "as_pick_hybrid_non_nato"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
						{"switch"
							{"case"
								{condition {type rand} {value 0.20}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_motor_left$"} {op ">"} {value 0}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 19}}
										("as_poke_faction_motor")
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
										("as_poke_faction_line")
									}
								}
							}
							{"case"
								{condition {type rand} {value 0.15}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_air_left$"} {op ">"} {value 0}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 18}}
										("as_poke_faction_air")
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
										("as_poke_faction_line")
									}
								}
							}
							{"case"
								{condition {type rand} {value 0.15}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_ifv_left$"} {op ">"} {value 0}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 17}}
										("as_poke_faction_ifv")
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
										("as_poke_faction_line")
									}
								}
							}
							{"case"
								{condition {type rand} {value 0.1}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 16}}
								("as_poke_faction_veh")
							}
							{"case"
								{condition {type rand} {value 0.11}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 15}}
								("as_poke_faction_manpad")
							}
							{"case"
								{condition {type rand} {value 0.13}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 14}}
								("as_poke_faction_eng")
							}
							{"case"
								{condition {type rand} {value 0.21}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 13}}
								("as_poke_faction_assault")
							}
							{"case"
								{condition {type rand} {value 0.25}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 12}}
								("as_poke_faction_recon")
							}
							{"case"
								{condition {type rand} {value 0.4}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 11}}
								("as_poke_faction_wpn")
							}
							{"default"
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
								("as_poke_faction_line")
							}
						}
					}
					{"case"
						{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
						{"switch"
							{"case"
								{condition {type rand} {value 0.20}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_motor_left$"} {op ">"} {value 0}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 19}}
										("as_poke_faction_motor")
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
										("as_poke_faction_line")
									}
								}
							}
							{"case"
								{condition {type rand} {value 0.15}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_air_left$"} {op ">"} {value 0}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 18}}
										("as_poke_faction_air")
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
										("as_poke_faction_line")
									}
								}
							}
							{"case"
								{condition {type rand} {value 0.10}}
								{"switch"
									{"case"
										{condition {type cmp_i} {var "attack_support_ifv_left$"} {op ">"} {value 0}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 17}}
										("as_poke_faction_ifv")
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
										("as_poke_faction_line")
									}
								}
							}
							{"case"
								{condition {type rand} {value 0.12}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 16}}
								("as_poke_faction_veh")
							}
							{"case"
								{condition {type rand} {value 0.12}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 14}}
								("as_poke_faction_eng")
							}
							{"case"
								{condition {type rand} {value 0.18}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 13}}
								("as_poke_faction_assault")
							}
							{"case"
								{condition {type rand} {value 0.28}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 12}}
								("as_poke_faction_recon")
							}
							{"case"
								{condition {type rand} {value 0.4}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 11}}
								("as_poke_faction_wpn")
							}
							{"default"
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
								("as_poke_faction_line")
							}
						}
					}
					{"default"
						{"switch"
							{"case"
								{condition {type rand} {value 0.35}}
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 12}}
								("as_poke_faction_recon")
							}
							{"default"
								{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
								("as_poke_faction_line")
							}
						}
					}
				}
			)

			(define "am_pick_composition"
				("as_resolve_army")
				{"switch"
					{"case"
						{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 1}}
						("as_pick_hybrid_non_nato")
					}
					{"case"
						{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 2}}
						("as_pick_hybrid_non_nato")
					}
					{"case"
						{condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 4}}
						("as_pick_hybrid_non_nato")
					}
					; NATO: specialty mix + hybrid recon/eng/manpad/veh injected at L2/L3.
					{"default"
						{"switch"
							{"case"
								{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
								{"switch"
									{"case"
										{condition {type rand} {value 0.18}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_motor_left$"} {op ">"} {value 0}}
												{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 19}}
												("as_poke_faction_motor")
											}
											{"default"
												{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
												{"trigger" {name "attack_support/comp_usmc"}}
											}
										}
									}
									{"case"
										{condition {type rand} {value 0.1}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 16}}
										("as_poke_faction_veh")
									}
									{"case"
										{condition {type rand} {value 0.1}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 15}}
										("as_poke_faction_manpad")
									}
									{"case"
										{condition {type rand} {value 0.1}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 14}}
										("as_poke_faction_eng")
									}
									{"case"
										{condition {type rand} {value 0.14}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 12}}
										("as_poke_faction_recon")
									}
									{"case"
										{condition {type rand} {value 0.12}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 13}}
										("as_poke_faction_assault")
									}
									{"case"
										{condition {type rand} {value 0.18}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 4}}
										{"trigger" {name "attack_support/comp_pzgren"}}
									}
									{"case"
										{condition {type rand} {value 0.22}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 3}}
										{"trigger" {name "attack_support/comp_acav"}}
									}
									{"case"
										{condition {type rand} {value 0.3}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 5}}
										{"trigger" {name "attack_support/comp_arf"}}
									}
									{"case"
										{condition {type rand} {value 0.5}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 2}}
										{"trigger" {name "attack_support/comp_1ad"}}
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
										{"trigger" {name "attack_support/comp_usmc"}}
									}
								}
							}
							{"case"
								{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
								{"switch"
									{"case"
										{condition {type rand} {value 0.15}}
										{"switch"
											{"case"
												{condition {type cmp_i} {var "attack_support_motor_left$"} {op ">"} {value 0}}
												{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 19}}
												("as_poke_faction_motor")
											}
											{"default"
												{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
												{"trigger" {name "attack_support/comp_usmc"}}
											}
										}
									}
									{"case"
										{condition {type rand} {value 0.1}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 16}}
										("as_poke_faction_veh")
									}
									{"case"
										{condition {type rand} {value 0.1}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 14}}
										("as_poke_faction_eng")
									}
									{"case"
										{condition {type rand} {value 0.15}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 12}}
										("as_poke_faction_recon")
									}
									{"case"
										{condition {type rand} {value 0.12}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 13}}
										("as_poke_faction_assault")
									}
									{"case"
										{condition {type rand} {value 0.22}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 3}}
										{"trigger" {name "attack_support/comp_acav"}}
									}
									{"case"
										{condition {type rand} {value 0.3}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 5}}
										{"trigger" {name "attack_support/comp_arf"}}
									}
									{"case"
										{condition {type rand} {value 0.5}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 2}}
										{"trigger" {name "attack_support/comp_1ad"}}
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
										{"trigger" {name "attack_support/comp_usmc"}}
									}
								}
							}
							{"default"
								{"switch"
									{"case"
										{condition {type rand} {value 0.2}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 12}}
										("as_poke_faction_recon")
									}
									{"case"
										{condition {type rand} {value 0.3}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 5}}
										{"trigger" {name "attack_support/comp_arf"}}
									}
									{"case"
										{condition {type rand} {value 0.5}}
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 2}}
										{"trigger" {name "attack_support/comp_1ad"}}
									}
									{"default"
										{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 1}}
										{"trigger" {name "attack_support/comp_usmc"}}
									}
								}
							}
						}
					}
				}
				; Pool-short fallback: faction line, then give up.
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "attack_support_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 10}}
						("as_poke_faction_line")
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "ATTACK SUPPORT POOL SHORT - FACTION LINE"}}
							}
							{"default"}
						}
					}
					{"default"}
				}
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "attack_support_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
						("as_announce_wave")
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "ATTACK SUPPORT POOL EXHAUSTED"}}
							}
							{"default"}
						}
					}
					{"default"}
				}
			)

			; Once per mission: 100% chance support waves arm on either side (friendly + enemy) for test build.
			; (Default production value: 50% roll / 0.5).
			; Lives in attack_support_waves.inc because maps include this file first.
			{"support_mission/roll"
				{condition
					{expression "1"}
					{terms
						{"1.cmp_i" {var "support_mission_roll_done$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"set_i" {var "support_mission_roll_done$"} {op "="} {value 1}}
					{"switch"
						{"case"
							{condition {type rand} {value 1.0}}
							{"set_i" {var "support_mission_enabled$"} {op "="} {value 1}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 8} {title "SUPPORT MISSION ON (100% roll)"}}
								}
								{"default"}
							}
						}
						{"default"
							{"set_i" {var "support_mission_enabled$"} {op "="} {value 0}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 8} {title "SUPPORT MISSION OFF (100% roll)"}}
								}
								{"default"}
							}
						}
					}
				}
			}

			; Arms once per attack mission. attack_support_use_mi$ is the enable switch:
			; attack_support.lua sets it alongside id_attack_support$, because MI delivery
			; is the only path that works on the attack support slot (Lua Spawn reports no
			; units available there). The clock stays parked behind attack_support_busy$ /
			; attack_support_next_ok$ until the opening wave has been issued, otherwise its
			; condition is already true here and it fires alongside init.
			; support_mission_enabled$ is the per-mission 50% gate (see support_mission/roll).
			{"attack_support/init"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "attack_support_armed$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"5.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"6.cmp_i" {var "support_mission_enabled$"} {op "=="} {value 1}}
					}
				}
				{actions
					{"set_i" {var "attack_support_armed$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_flank_rr$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_use_flank$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_ifv_left$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_motor_left$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_motor_test$"} {op "="} {value 0}}	; TEST: 1=forced staggered trucks, 0=production rolls
					{"set_i" {var "attack_support_motor_test_done$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_air_left$"} {op "="} {value 2}}
					; DAY-2 TEST default ON. Set value 0 to restore production airmobile schedule only.
					{"set_i" {var "attack_support_air_test$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
					{"set_i" {var "support_announce$"} {op "="} {value 1}}
					{"set_i" {var "support_e2_test$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_stage$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_fail$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_lz$"} {op "="} {value 0}}
					{"set_i" {var "support_e2_flag$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_busy$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_next_ok$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"set_i" {var "attack_support_wave_num$"} {op "="} {value 0}}
					{"set_i" {var "attack_support_hmmwv_left$"} {op "="} {value 4}}
					{"set_i" {var "attack_support_ukr_veh_left$"} {op "="} {value 3}}
					{"set_i" {var "attack_support_nato_veh_left$"} {op "="} {value 2}}
					{"set_i" {var "attack_support_stage$"} {op "="} {value 1}}
					; Wave budget by campaign progression. defense_level$ is computed by the
					; CE mission setup (1-3, roughly campaign games 0 / 6 / 12).
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
							{"set_i" {var "attack_support_waves_left$"} {op "="} {value 8}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 10} {title "ATTACK SUPPORT ARMED - L3, 8 WAVES"}}
								}
								{"default"}
							}
						}
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
							{"set_i" {var "attack_support_waves_left$"} {op "="} {value 6}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 10} {title "ATTACK SUPPORT ARMED - L2, 6 WAVES"}}
								}
								{"default"}
							}
						}
						{"default"
							{"set_i" {var "attack_support_waves_left$"} {op "="} {value 4}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 10} {title "ATTACK SUPPORT ARMED - L1, 4 WAVES"}}
								}
								{"default"}
							}
						}
					}
					; Opening wave ~30s in. DAY-2 TEST (air_test$=1): force airmobile.
					; Set attack_support_air_test$=0 in init to restore production pick only.
					{"delay" {time 30}}
					{"set_i" {var "attack_support_wave_num$"} {op "+"} {value 1}}
					{"set_i" {var "attack_support_waves_left$"} {op "-"} {value 1}}
					("as_resolve_army")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "attack_support_air_test$"} {op "=="} {value 1}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "attack_support_air_left$"} {op ">"} {value 0}}
									{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 18}}
									("as_poke_faction_air")
								}
								{"default"
									("am_pick_composition")
								}
							}
						}
						{"default"
							("am_pick_composition")
						}
					}
					; Hand the cadence over only once the opening wave has finished landing.
					{"delay" {time 20}}
					{"set_i" {var "attack_support_next_ok$"} {op "="} {value 1}}
					{"set_i" {var "attack_support_busy$"} {op "="} {value 0}}
					{"trigger" {name "attack_support/clock"}}
				}
			}

			; Self-re-arming cadence. One cycle = randomized wait, then either a
			; live-unit-cap defer (no wave consumed) or one randomly picked wave.
			; When the budget runs out the condition stops matching and the loop ends.
			{"attack_support/clock"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "attack_support_next_ok$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "attack_support_busy$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "attack_support_waves_left$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_busy$"} {op "="} {value 1}}
					; Randomized 120-240s cadence (~20% tighter): ~20% per bucket.
					{"switch"
						{"case"
							{condition {type rand} {value 0.2}}
							{"delay" {time 120}}
						}
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 150}}
						}
						{"case"
							{condition {type rand} {value 0.33}}
							{"delay" {time 180}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 210}}
						}
						{"default"
							{"delay" {time 240}}
						}
					}
					{"delay" {time 0.1}}
					{"switch"
						; LIVE-UNIT CAP. Skip this cycle without consuming a wave while the
						; teammate still has a platoon on the field. Counted on the simple
						; selector form the mission scripts use for live units - the advanced
						; selector's prop/state decorations zero the match on these entities.
						{"case"
							{condition
								{type entities}
								{selector
									{ignore_captured_by_user 0}
									{tag attack_support_src}
									{type human}
									{state "not dead"}
								}
								{count {op ">"} {value 14}}
							}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 8} {title "ATTACK SUPPORT NEAR CAP DEFER"}}
								}
								{"default"}
							}
						}
						{"default"
							{"set_i" {var "attack_support_wave_num$"} {op "+"} {value 1}}
							{"set_i" {var "attack_support_waves_left$"} {op "-"} {value 1}}
							("am_pick_composition")
						}
					}
					{"delay" {time 0.1}}
					{"set_i" {var "attack_support_busy$"} {op "="} {value 0}}
					{"trigger" {name "attack_support/clock"}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "attack_support_waves_left$"} {op "=="} {value 0}}
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
									{"timer" {time 10} {title "ATTACK SUPPORT WAVES EXHAUSTED"}}
								}
								{"default"}
							}
						("as_announce_exhausted")
						}
						{"default"}
					}
				}
			}

			; COMPOSITION 1 â€” 5 USMC infantry. Also the pool-short fallback, so this pool
			; is the deepest. Taking the bodies strips the pool tag, so the gate below can
			; simply count the tag to know what is still parked and available.
			{"attack_support/comp_usmc"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_usmc}}
							{count {op ">="} {value 5}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 11}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - USMC RIFLE TEAM"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_usmc}}}}
							{amount 5}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_usmc}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 12}}
				}
			}

			; COMPOSITION 5 â€” 5 ARF / generic NATO infantry (European ARF kit).
			{"attack_support/comp_arf"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 5}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_arf}}
							{count {op ">="} {value 5}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 51}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - ARF RIFLE TEAM"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_arf}}}}
							{amount 5}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_arf}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 52}}
				}
			}

			; COMPOSITION 2 â€” 5 1AD infantry.
			{"attack_support/comp_1ad"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_1ad}}
							{count {op ">="} {value 5}}
						}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 21}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - 1AD RIFLE TEAM"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_1ad}}}}
							{amount 5}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_1ad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 22}}
				}
			}

			; COMPOSITION 3 (level 2+) â€” 4 1AD infantry, then one crewed humvee behind
			; them. Infantry first: a vehicle placed into the same spot clips and flips.
			{"attack_support/comp_acav"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 3}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_1ad}}
							{count {op ">="} {value 4}}
						}
						{"5.cmp_i" {var "attack_support_hmmwv_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 31}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - 1AD TEAM + HUMVEE"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_1ad}}}}
							{amount 4}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_1ad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 6} {title "ATTACK SUPPORT HUMVEE MOVING UP"}}
						}
						{"default"}
					}
					("am_deploy_next_hmmwv")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 32}}
				}
			}

			; COMPOSITION 4 (level 3) â€” 6 German pzgren, then two crewed humvees one at a
			; time with a gap between them.
			{"attack_support/comp_pzgren"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 4}}
						{"3.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"4.entities"
							{selector {tag attack_support_inf_pzgd}}
							{count {op ">="} {value 6}}
						}
						{"5.cmp_i" {var "attack_support_hmmwv_left$"} {op ">"} {value 1}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 41}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PZGREN + 2 HUMVEES"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag attack_support_inf_pzgd}}}}
							{amount 6}
						}
						{tag_add attack_support_deploy}
						{tag_remove attack_support_inf_pzgd}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 6} {title "ATTACK SUPPORT HUMVEE 1 MOVING UP"}}
						}
						{"default"}
					}
					("am_deploy_next_hmmwv")
					{"delay" {time 6}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 6} {title "ATTACK SUPPORT HUMVEE 2 MOVING UP"}}
						}
						{"default"}
					}
					("am_deploy_next_hmmwv")
					{"set_i" {var "attack_support_stage$"} {op "="} {value 42}}
				}
			}

			; ===== FACTION LINE / WPN (player nation pools) =====
			; cmd 10 = line (4), cmd 11 = wpn (4). Only the matching army trigger clears.

			{"attack_support/ally_rusa_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 10}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_rusa_line}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA LINE"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_line}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_line}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_ukr_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 10}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_ukr_line}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR LINE"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_line}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_line}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_prc_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 10}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_prc_line}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC LINE"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_line}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_line}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 10}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_nato_line}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO LINE"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_line}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_line}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_rusa_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 11}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_rusa_wpn}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA WEAPONS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_wpn}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_wpn}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_ukr_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 11}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_ukr_wpn}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR WEAPONS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_wpn}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_wpn}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_prc_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 11}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_prc_wpn}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC WEAPONS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_wpn}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_wpn}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 11}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_nato_wpn}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO WEAPONS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_wpn}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_wpn}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}

			; ===== HYBRID: recon(3) assault(4) eng(3) manpad(2) light veh =====
			{"attack_support/ally_rusa_recon"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 12}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_rusa_recon}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA RECON"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_recon}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_recon}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_ukr_recon"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 12}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_ukr_recon}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR RECON"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_recon}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_recon}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_prc_recon"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 12}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_prc_recon}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC RECON"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_recon}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_recon}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_recon"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 12}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_nato_recon}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO RECON"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_recon}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_recon}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_rusa_assault"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 13}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_rusa_assault}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA ASSAULT"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_assault}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_assault}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_ukr_assault"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 13}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_ukr_assault}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR ASSAULT"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_assault}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_assault}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_prc_assault"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 13}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_prc_assault}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC ASSAULT"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_assault}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_assault}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_assault"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 13}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_nato_assault}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO ASSAULT"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_assault}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_assault}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_rusa_eng"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 14}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_rusa_eng}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA ENGINEERS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_eng}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_eng}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_ukr_eng"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 14}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_ukr_eng}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR ENGINEERS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_eng}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_eng}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_prc_eng"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 14}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_prc_eng}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC ENGINEERS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_eng}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_eng}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_eng"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 14}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_nato_eng}} {count {op ">="} {value 3}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO ENGINEERS"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_eng}}}} {amount 3}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_eng}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_rusa_manpad"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 15}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_rusa_manpad}} {count {op ">="} {value 2}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA MANPAD"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_manpad}}}} {amount 2}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_manpad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_ukr_manpad"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 15}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_ukr_manpad}} {count {op ">="} {value 2}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR MANPAD"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_manpad}}}} {amount 2}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_manpad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_prc_manpad"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 15}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_prc_manpad}} {count {op ">="} {value 2}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC MANPAD"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_manpad}}}} {amount 2}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_manpad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_manpad"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 15}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.entities" {selector {tag ally_sup_nato_manpad}} {count {op ">="} {value 2}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_wave")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO MANPAD"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_manpad}}}} {amount 2}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_manpad}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			; Light wheels: one linked hull+crew instance at a time (UKR humvee / NATO fennek).
			{"attack_support/ally_ukr_veh"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 16}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_ukr_veh_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR HUMVEE"}}
						}
						{"default"}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "attack_support_ukr_veh_left$"} {op "=="} {value 3}}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_ukr_veh1}}}}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_ukr_veh}
								{tag_remove ally_sup_ukr_veh1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "attack_support_ukr_veh_left$"} {op "=="} {value 2}}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_ukr_veh2}}}}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_ukr_veh}
								{tag_remove ally_sup_ukr_veh2}
							}
						}
						{"case"
							{condition {type cmp_i} {var "attack_support_ukr_veh_left$"} {op "=="} {value 1}}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_ukr_veh3}}}}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_ukr_veh}
								{tag_remove ally_sup_ukr_veh3}
							}
						}
						{"default"}
					}
					{"set_i" {var "attack_support_ukr_veh_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			{"attack_support/ally_nato_veh"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 16}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_nato_veh_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO FENNEK"}}
						}
						{"default"}
					}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "attack_support_nato_veh_left$"} {op "=="} {value 2}}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_nato_veh1}}}}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_nato_veh}
								{tag_remove ally_sup_nato_veh1}
							}
						}
						{"case"
							{condition {type cmp_i} {var "attack_support_nato_veh_left$"} {op "=="} {value 1}}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_nato_veh2}}}}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_nato_veh}
								{tag_remove ally_sup_nato_veh2}
							}
						}
						{"default"}
					}
					{"set_i" {var "attack_support_nato_veh_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			; ===== RARE IFV (cmd 17) =====

			{"attack_support/ally_rusa_ifv"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 17}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_ifv_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_rusa_ifv_inf}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - RUSA IFV"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_ifv_inf}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_ifv_inf}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_ifv1}}}}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_ifv}
						{tag_remove ally_sup_rusa_ifv1}
					}
					{"set_i" {var "attack_support_ifv_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}

			{"attack_support/ally_ukr_ifv"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 17}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_ifv_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_ukr_ifv_inf}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - UKR IFV"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_ifv_inf}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_ifv_inf}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_ifv1}}}}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_ifv}
						{tag_remove ally_sup_ukr_ifv1}
					}
					{"set_i" {var "attack_support_ifv_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}

			{"attack_support/ally_prc_ifv"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 17}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_ifv_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_prc_ifv_inf}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - PRC IFV"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_ifv_inf}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_ifv_inf}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_ifv1}}}}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_ifv}
						{tag_remove ally_sup_prc_ifv1}
					}
					{"set_i" {var "attack_support_ifv_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}

			{"attack_support/ally_nato_ifv"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 17}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_ifv_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_nato_ifv_inf}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_vehicle")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT WAVE - NATO IFV"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_ifv_inf}}}} {amount 4}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_ifv_inf}
					}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
					{"delay" {time 5}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_ifv1}}}}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_ifv}
						{tag_remove ally_sup_nato_ifv1}
					}
					{"set_i" {var "attack_support_ifv_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("am_place_at_entry")
					("am_finish_deploy")
				}
			}
			; ===== AIRMOBILE INSERT (cmd 18, Phase 5 E1) =====

			{"attack_support/ally_rusa_air"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 18}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_air_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_airborne")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT AIRMOBILE INSERT"}}
						}
						{"default"}
					}
					{"delay" {time 4.5}}
					; Prefer recon fireteam; fall back to line if recon pool short.
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_rusa_recon}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_rusa_recon}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_rusa_recon}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_rusa_line}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_rusa_line}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_rusa_line}
							}
						}
						{"default"
							{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "ATTACK SUPPORT AIRMOBILE POOL SHORT"}}
							}
							{"default"}
						}
						}
					}
					{"switch"
						{"case"
							{condition {type entities} {selector {tag attack_support_deploy}}}
							; Enemy near LZ guard — if bots already within 120, fall back to edge pads.
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{type human}
											{state "not dead"}
											{tag _bot}
											{distance 120}
										}
										{count {op ">"} {value 0}}
									}
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
								}
								{"default"
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 1}}
									{"switch"
										{"case"
											{condition {type cmp_i} {var "attack_support_flank_rr$"} {op ">="} {value 2}}
											{"set_i" {var "attack_support_flank_rr$"} {op "="} {value 1}}
										}
										{"default"
											{"set_i" {var "attack_support_flank_rr$"} {op "+"} {value 1}}
										}
									}
								}
							}
							{"set_i" {var "attack_support_air_left$"} {op "-"} {value 1}}
							{"delay" {time 0.1}}
							("am_place_at_entry")
							("am_finish_deploy")
							{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
						}
						{"default"}
					}
				}
			}

			{"attack_support/ally_ukr_air"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 18}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_air_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_airborne")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT AIRMOBILE INSERT"}}
						}
						{"default"}
					}
					{"delay" {time 4.5}}
					; Prefer recon fireteam; fall back to line if recon pool short.
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_ukr_recon}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_ukr_recon}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_ukr_recon}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_ukr_line}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_ukr_line}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_ukr_line}
							}
						}
						{"default"
							{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "ATTACK SUPPORT AIRMOBILE POOL SHORT"}}
							}
							{"default"}
						}
						}
					}
					{"switch"
						{"case"
							{condition {type entities} {selector {tag attack_support_deploy}}}
							; Enemy near LZ guard — if bots already within 120, fall back to edge pads.
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{type human}
											{state "not dead"}
											{tag _bot}
											{distance 120}
										}
										{count {op ">"} {value 0}}
									}
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
								}
								{"default"
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 1}}
									{"switch"
										{"case"
											{condition {type cmp_i} {var "attack_support_flank_rr$"} {op ">="} {value 2}}
											{"set_i" {var "attack_support_flank_rr$"} {op "="} {value 1}}
										}
										{"default"
											{"set_i" {var "attack_support_flank_rr$"} {op "+"} {value 1}}
										}
									}
								}
							}
							{"set_i" {var "attack_support_air_left$"} {op "-"} {value 1}}
							{"delay" {time 0.1}}
							("am_place_at_entry")
							("am_finish_deploy")
							{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
						}
						{"default"}
					}
				}
			}

			{"attack_support/ally_prc_air"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 18}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_air_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_airborne")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT AIRMOBILE INSERT"}}
						}
						{"default"}
					}
					{"delay" {time 4.5}}
					; Prefer recon fireteam; fall back to line if recon pool short.
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_prc_recon}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_prc_recon}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_prc_recon}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_prc_line}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_prc_line}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_prc_line}
							}
						}
						{"default"
							{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "ATTACK SUPPORT AIRMOBILE POOL SHORT"}}
							}
							{"default"}
						}
						}
					}
					{"switch"
						{"case"
							{condition {type entities} {selector {tag attack_support_deploy}}}
							; Enemy near LZ guard — if bots already within 120, fall back to edge pads.
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{type human}
											{state "not dead"}
											{tag _bot}
											{distance 120}
										}
										{count {op ">"} {value 0}}
									}
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
								}
								{"default"
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 1}}
									{"switch"
										{"case"
											{condition {type cmp_i} {var "attack_support_flank_rr$"} {op ">="} {value 2}}
											{"set_i" {var "attack_support_flank_rr$"} {op "="} {value 1}}
										}
										{"default"
											{"set_i" {var "attack_support_flank_rr$"} {op "+"} {value 1}}
										}
									}
								}
							}
							{"set_i" {var "attack_support_air_left$"} {op "-"} {value 1}}
							{"delay" {time 0.1}}
							("am_place_at_entry")
							("am_finish_deploy")
							{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
						}
						{"default"}
					}
				}
			}

			{"attack_support/ally_nato_air"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 18}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_air_left$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_airborne")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT AIRMOBILE INSERT"}}
						}
						{"default"}
					}
					{"delay" {time 4.5}}
					; Prefer recon fireteam; fall back to line if recon pool short.
					{"switch"
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_nato_recon}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_nato_recon}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_nato_recon}
							}
						}
						{"case"
							{condition
								{type entities}
								{selector {tag ally_sup_nato_line}}
								{count {op ">="} {value 4}}
							}
							{"entity_state"
								{selector {source advanced} {group {select {tag {tag ally_sup_nato_line}}}} {amount 4}}
								{tag_add attack_support_deploy}
								{tag_remove ally_sup_nato_line}
							}
						}
						{"default"
							{"switch"
							{"case"
								{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
								{"timer" {time 6} {title "ATTACK SUPPORT AIRMOBILE POOL SHORT"}}
							}
							{"default"}
						}
						}
					}
					{"switch"
						{"case"
							{condition {type entities} {selector {tag attack_support_deploy}}}
							; Enemy near LZ guard — if bots already within 120, fall back to edge pads.
							{"switch"
								{"case"
									{condition
										{type entities}
										{selector
											{ignore_captured_by_user 0}
											{type human}
											{state "not dead"}
											{tag _bot}
											{distance 120}
										}
										{count {op ">"} {value 0}}
									}
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
								}
								{"default"
									{"set_i" {var "attack_support_use_air$"} {op "="} {value 1}}
									{"switch"
										{"case"
											{condition {type cmp_i} {var "attack_support_flank_rr$"} {op ">="} {value 2}}
											{"set_i" {var "attack_support_flank_rr$"} {op "="} {value 1}}
										}
										{"default"
											{"set_i" {var "attack_support_flank_rr$"} {op "+"} {value 1}}
										}
									}
								}
							}
							{"set_i" {var "attack_support_air_left$"} {op "-"} {value 1}}
							{"delay" {time 0.1}}
							("am_place_at_entry")
							("am_finish_deploy")
							{"set_i" {var "attack_support_use_air$"} {op "="} {value 0}}
						}
						{"default"}
					}
				}
			}

			; ===== E2 REAL AIR INSERT PROBES =====
			; Default-off live flight proof. Gate 1 flies a helo; gate 2 is reserved for
			; Task 4's real paradrop. Forced probes never touch production wave budgets.

			(define "e2_reset_target"
				{"entity_state" {selector {tag support_e2_flag_target}} {tag_remove support_e2_flag_target}}
				{"set_i" {var "support_e2_flag$"} {op "="} {value 0}}
				{"set_i" {var "support_e2_lz$"} {op "="} {value 0}}
			)

			(define "e2_announce_helo"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"talk"
							{selector {ignore_captured_by_user 0} {amount 1} {tag player} {zone "gamezone"} {type human} {state "not dead"}}
							{text "mission/multi/support/e2_helo_inbound"}
						}
					}
					{"default"}
				}
			)

			(define "e2_announce_failed"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "support_announce$"} {op "=="} {value 1}}
						{"talk"
							{selector {ignore_captured_by_user 0} {amount 1} {tag player} {zone "gamezone"} {type human} {state "not dead"}}
							{text "mission/multi/support/e2_insert_failed"}
						}
					}
					{"default"}
				}
			)

			; Both tags are required: support_e2_aircraft also exists on unclaimed parked
			; templates, which must never be deleted by another package's cleanup.
			(define "e2_delete_aircraft"
				{"delete" {selector {ignore_captured_by_user 0} {tag support_e2_aircraft} {tag support_e2_claim}}}
			)

			(define "e2_fail_and_cleanup"
				("e2_delete_aircraft")
				{"entity_state" {selector {tag support_e2_claim}} {tag_remove support_e2_claim}}
				{"entity_state" {selector {tag support_e2_team}} {tag_remove support_e2_team}}
				{"entity_state" {selector {tag support_e2_helo}} {tag_remove support_e2_helo}}
				{"entity_state" {selector {tag support_e2_lz_marker}} {tag_remove support_e2_lz_marker}}
				{"entity_state" {selector {tag support_e2_place_one}} {tag_remove support_e2_place_one}}
				{"entity_state" {selector {tag support_e2_placed}} {tag_remove support_e2_placed}}
				{"entity_state" {selector {tag support_e2_flag_target}} {tag_remove support_e2_flag_target}}
				{"set_i" {var "support_e2_stage$"} {op "="} {value 70}}
				("e2_announce_failed")
			)

			(define "e2_complete_cleanup"
				("e2_delete_aircraft")
				{"entity_state" {selector {tag support_e2_claim}} {tag_remove support_e2_claim}}
				{"entity_state" {selector {tag support_e2_helo}} {tag_remove support_e2_helo}}
				{"entity_state" {selector {tag support_e2_lz_marker}} {tag_remove support_e2_lz_marker}}
				{"entity_state" {selector {tag support_e2_place_one}} {tag_remove support_e2_place_one}}
				{"entity_state" {selector {tag support_e2_placed}} {tag_remove support_e2_placed}}
				{"entity_state" {selector {tag support_e2_flag_target}} {tag_remove support_e2_flag_target}}
				{"set_i" {var "support_e2_stage$"} {op "="} {value 70}}
			)

			(define "e2_choose_flag"
				("e2_reset_target")
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag flag}}} {exclude {state {state inactive}}}}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add support_e2_flag_target}
				}
				{"delay" {time 0.1}}
				{"switch"
					{"case"
						{condition {type entities} {selector {tag support_e2_flag_target}}}
						; Some managed maps do not expose numbered capture tags. A value of 1
						; therefore records the honest portable fact: one active target was selected.
						{"set_i" {var "support_e2_flag$"} {op "="} {value 1}}
					}
					{"default"
						; The caller owns the single abort/cleanup path.
						{"set_i" {var "support_e2_fail$"} {op "="} {value 3}}
					}
				}
			)

			(define "e2_own_current"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_attack_support$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag support_e2_claim}} {operation set} {player "16"}}}
					{"default" {"set_i" {var "support_e2_fail$"} {op "="} {value 8}}}
				}
			)

			(define "e2_place_one"
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag support_e2_team}}} {exclude {tag {tag support_e2_placed}}}}
						{amount 1}
					}
					{tag_add support_e2_place_one}
					{tag_remove support_e2_team}
				}
				{"delay" {time 0.05}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"switch"
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_air_a2"}}}
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 1}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_air_a1"}}}
							{"default" {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target {ignore_captured_by_user 0} {amount 1} {tag spawn_a}}}}
						}
					}
					{"default"
						{"switch"
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_air_b2"}}}
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 1}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_air_b1"}}}
							{"default" {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target {ignore_captured_by_user 0} {amount 1} {tag spawn_b}}}}
						}
					}
				}
				{"entity_state"
					{selector {tag support_e2_place_one}}
					{tag_add support_e2_team}
					{tag_add support_e2_placed}
					{tag_add attack_support_src}
					{tag_remove ally_sup_tpl}
					{tag_remove support_e2_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
					{tag_remove support_e2_place_one}
				}
				{"delay" {time 0.5}}
			)

			(define "e2_place_one_entry"
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag support_e2_team}}} {exclude {tag {tag support_e2_placed}}}}
						{amount 1}
					}
					{tag_add support_e2_place_one}
					{tag_remove support_e2_team}
				}
				{"delay" {time 0.05}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_entry_b1"}}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_entry_a1"}}
					}
					{"default"
						{"placement" {selector {ignore_captured_by_user 0} {tag support_e2_place_one}} {target_waypoint "attack_support_entry_b1"}}
					}
				}
				{"entity_state"
					{selector {tag support_e2_place_one}}
					{tag_add support_e2_team}
					{tag_add support_e2_placed}
					{tag_add attack_support_src}
					{tag_remove ally_sup_tpl}
					{tag_remove support_e2_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
					{tag_remove support_e2_place_one}
				}
				{"delay" {time 0.5}}
			)

			(define "e2_order_team"
				{"actor_state"
					{selector {ignore_captured_by_user 0} {tag support_e2_team}}
					{control AI}
					{ai_move {mode enable}}
					{weapon_prepare on}
					{fire_mode open}
					{move_mode free}
					{movement {speed normal} {kind normal} {type normal}}
					{ai {no_retreat on} {advance_ratio 1} {retreat_ratio 0}}
				}
				{"action"
					{selector {ignore_captured_by_user 0} {tag support_e2_team}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag support_e2_flag_target}}
				}
			)

			; Probe each numbered air pad independently with the attested near condition.
			; If both are unsafe, lz=0 means infantry-only standoff at the corresponding
			; numbered entry waypoint and fail code 4.
			(define "e2_claim_lz_marker"
				{"entity_state"
					{selector {ignore_captured_by_user 0} {tag support_e2_team} {amount 1}}
					{tag_add support_e2_lz_marker}
				}
			)

			(define "e2_place_lz_marker"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"switch"
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {target_waypoint "attack_support_air_b2"}}}
							{"default" {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {target_waypoint "attack_support_air_b1"}}}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"switch"
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {target_waypoint "attack_support_air_a2"}}}
							{"default" {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {target_waypoint "attack_support_air_a1"}}}
						}
					}
					{"default"
						{"switch"
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}} {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {target_waypoint "attack_support_air_b2"}}}
							{"default" {"placement" {selector {ignore_captured_by_user 0} {tag support_e2_lz_marker}} {target_waypoint "attack_support_air_b1"}}}
						}
					}
				}
			)

			; Anchor each safety query to the candidate pad with one still-hidden claimed
			; team human. The inactive marker cannot match the operatable-units query itself.
			(define "e2_choose_lz"
				("e2_claim_lz_marker")
				{"switch"
					{"case" {condition {type rand} {value 0.5}} {"set_i" {var "support_e2_lz$"} {op "="} {value 2}}}
					{"default" {"set_i" {var "support_e2_lz$"} {op "="} {value 1}}}
				}
				("e2_place_lz_marker")
				{"delay" {time 0.05}}
				{"switch"
					{"case"
						{condition
							{type near}
							{units {ignore_captured_by_user 0} {tag _bot} {type human} {state operatable}}
							{near_to {ignore_captured_by_user 0} {tag support_e2_lz_marker}}
							{distance 120}
						}
						{"switch"
							{"case" {condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 1}} {"set_i" {var "support_e2_lz$"} {op "="} {value 2}}}
							{"default" {"set_i" {var "support_e2_lz$"} {op "="} {value 1}}}
						}
						("e2_place_lz_marker")
						{"delay" {time 0.05}}
						{"switch"
							{"case"
								{condition
									{type near}
									{units {ignore_captured_by_user 0} {tag _bot} {type human} {state operatable}}
									{near_to {ignore_captured_by_user 0} {tag support_e2_lz_marker}}
									{distance 120}
								}
								{"set_i" {var "support_e2_fail$"} {op "="} {value 4}}
								{"set_i" {var "support_e2_lz$"} {op "="} {value 0}}
							}
							{"default"}
						}
					}
					{"default"}
				}
			)

			(define "e2_promote_helo"
				{"entity_state"
					{selector {tag support_e2_helo}}
					{tag_remove ally_sup_tpl}
					{tag_remove support_e2_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
			)

			(define "e2_place_aircraft_entry"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"placement" {selector {ignore_captured_by_user 0} {tag support_e2_aircraft} {tag support_e2_claim}} {target_waypoint "attack_support_entry_b1"}}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement" {selector {ignore_captured_by_user 0} {tag support_e2_aircraft} {tag support_e2_claim}} {target_waypoint "attack_support_entry_a1"}}
					}
					{"default"
						{"placement" {selector {ignore_captured_by_user 0} {tag support_e2_aircraft} {tag support_e2_claim}} {target_waypoint "attack_support_entry_b1"}}
					}
				}
			)

			(define "e2_order_aircraft_lz"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}}
								{"action" {selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}} {drop orders} {action move} {waypoint "attack_support_air_b2"}}
							}
							{"default" {"action" {selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}} {drop orders} {action move} {waypoint "attack_support_air_b1"}}}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"switch"
							{"case"
								{condition {type cmp_i} {var "support_e2_lz$"} {op "=="} {value 2}}
								{"action" {selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}} {drop orders} {action move} {waypoint "attack_support_air_a2"}}
							}
							{"default" {"action" {selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}} {drop orders} {action move} {waypoint "attack_support_air_a1"}}}
						}
					}
					{"default" {"action" {selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}} {drop orders} {action move} {waypoint "attack_support_air_b1"}}}
				}
			)

			(define "e2_order_aircraft_exit"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"action" {selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}} {drop orders} {action move} {waypoint "attack_support_entry_b1"}}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"action" {selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}} {drop orders} {action move} {waypoint "attack_support_entry_a1"}}
					}
					{"default" {"action" {selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}} {drop orders} {action move} {waypoint "attack_support_entry_b1"}}}
				}
			)

			{"attack_support/e2_helo_rusa"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6 & 7 & 8 & 9 & 10"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "support_e2_test$"} {op "=="} {value 1}}
						{"6.cmp_i" {var "support_e2_stage$"} {op "=="} {value 10}}
						{"7.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"8.entities" {selector {tag support_e2_rusa_helo}} {count {op ">="} {value 1}}}
						{"9.entities" {selector {tag support_e2_rusa_helo_crew}} {count {op ">="} {value 2}}}
						{"10.entities" {selector {tag support_e2_rusa_helo_team}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					; Consume one parked hull, both linked pilots, and four independent troops.
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag support_e2_rusa_helo}}}} {amount 1}}
						{tag_add support_e2_claim}
						{tag_add support_e2_helo}
						{tag_remove support_e2_rusa_helo}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag support_e2_rusa_helo_crew}}}} {amount 2}}
						{tag_add support_e2_claim}
						{tag_add support_e2_helo}
						{tag_remove support_e2_rusa_helo_crew}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag support_e2_rusa_helo_team}}}} {amount 4}}
						{tag_add support_e2_claim}
						{tag_add support_e2_team}
						{tag_remove support_e2_rusa_helo_team}
					}
					("e2_choose_flag")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
							{"set_i" {var "support_e2_stage$"} {op "="} {value 20}}
							("e2_own_current")
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
									("e2_choose_lz")
									{"switch"
										{"case"
											{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 4}}
											; Both deep pads unsafe: delete the unused helo and deliver the
											; four claimed troops at the entry standoff. Failure remains honest.
											("e2_announce_failed")
											{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}
											("e2_place_one_entry")
											("e2_place_one_entry")
											("e2_place_one_entry")
											("e2_place_one_entry")
											("e2_order_team")
											{"set_i" {var "support_e2_stage$"} {op "="} {value 50}}
											("e2_complete_cleanup")
										}
										{"default"
											("e2_promote_helo")
											("e2_place_aircraft_entry")
											; Attested order: establish altitude, give AI control, then move.
											{"air_state"
												{selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}}
												{altitude 30}
											}
											{"actor_state"
												{selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}}
												{drop sensor}
												{control AI}
												{movement {speed fast}}
											}
											("e2_order_aircraft_lz")
											{"set_i" {var "support_e2_stage$"} {op "="} {value 30}}
											; Time-based primary arrival window for the flight proof.
											{"delay" {time 40}}
											{"switch"
												{"case"
													{condition
														{type near}
														{units {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft} {state operatable}}
														{near_to {ignore_captured_by_user 0} {tag support_e2_lz_marker}}
														{distance 120}
													}
													("e2_announce_helo")
													{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}
													("e2_place_one")
													("e2_place_one")
													("e2_place_one")
													("e2_place_one")
													("e2_order_team")
													{"set_i" {var "support_e2_stage$"} {op "="} {value 50}}
													("e2_order_aircraft_exit")
													{"set_i" {var "support_e2_stage$"} {op "="} {value 60}}
													{"delay" {time 60}}
													("e2_complete_cleanup")
												}
												{"default"
													{"set_i" {var "support_e2_fail$"} {op "="} {value 5}}
													("e2_order_aircraft_exit")
													{"delay" {time 10}}
													("e2_fail_and_cleanup")
												}
											}
										}
									}
								}
								{"default" ("e2_fail_and_cleanup")}
							}
						}
						{"default" ("e2_fail_and_cleanup")}
					}
				}
			}


			{"attack_support/e2_helo_ukr"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6 & 7 & 8 & 9 & 10"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "support_e2_test$"} {op "=="} {value 1}}
						{"6.cmp_i" {var "support_e2_stage$"} {op "=="} {value 10}}
						{"7.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"8.entities" {selector {tag support_e2_ukr_helo}} {count {op ">="} {value 1}}}
						{"9.entities" {selector {tag support_e2_ukr_helo_crew}} {count {op ">="} {value 2}}}
						{"10.entities" {selector {tag support_e2_ukr_helo_team}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					; Consume one parked hull, both linked pilots, and four independent troops.
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag support_e2_ukr_helo}}}} {amount 1}}
						{tag_add support_e2_claim}
						{tag_add support_e2_helo}
						{tag_remove support_e2_ukr_helo}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag support_e2_ukr_helo_crew}}}} {amount 2}}
						{tag_add support_e2_claim}
						{tag_add support_e2_helo}
						{tag_remove support_e2_ukr_helo_crew}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag support_e2_ukr_helo_team}}}} {amount 4}}
						{tag_add support_e2_claim}
						{tag_add support_e2_team}
						{tag_remove support_e2_ukr_helo_team}
					}
					("e2_choose_flag")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
							{"set_i" {var "support_e2_stage$"} {op "="} {value 20}}
							("e2_own_current")
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
									("e2_choose_lz")
									{"switch"
										{"case"
											{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 4}}
											; Both deep pads unsafe: delete the unused helo and deliver the
											; four claimed troops at the entry standoff. Failure remains honest.
											("e2_announce_failed")
											{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}
											("e2_place_one_entry")
											("e2_place_one_entry")
											("e2_place_one_entry")
											("e2_place_one_entry")
											("e2_order_team")
											{"set_i" {var "support_e2_stage$"} {op "="} {value 50}}
											("e2_complete_cleanup")
										}
										{"default"
											("e2_promote_helo")
											("e2_place_aircraft_entry")
											; Attested order: establish altitude, give AI control, then move.
											{"air_state"
												{selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}}
												{altitude 30}
											}
											{"actor_state"
												{selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}}
												{drop sensor}
												{control AI}
												{movement {speed fast}}
											}
											("e2_order_aircraft_lz")
											{"set_i" {var "support_e2_stage$"} {op "="} {value 30}}
											; Time-based primary arrival window for the flight proof.
											{"delay" {time 40}}
											{"switch"
												{"case"
													{condition
														{type near}
														{units {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft} {state operatable}}
														{near_to {ignore_captured_by_user 0} {tag support_e2_lz_marker}}
														{distance 120}
													}
													("e2_announce_helo")
													{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}
													("e2_place_one")
													("e2_place_one")
													("e2_place_one")
													("e2_place_one")
													("e2_order_team")
													{"set_i" {var "support_e2_stage$"} {op "="} {value 50}}
													("e2_order_aircraft_exit")
													{"set_i" {var "support_e2_stage$"} {op "="} {value 60}}
													{"delay" {time 60}}
													("e2_complete_cleanup")
												}
												{"default"
													{"set_i" {var "support_e2_fail$"} {op "="} {value 5}}
													("e2_order_aircraft_exit")
													{"delay" {time 10}}
													("e2_fail_and_cleanup")
												}
											}
										}
									}
								}
								{"default" ("e2_fail_and_cleanup")}
							}
						}
						{"default" ("e2_fail_and_cleanup")}
					}
				}
			}


			{"attack_support/e2_helo_nato"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6 & 7 & 8 & 9 & 10"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "support_e2_test$"} {op "=="} {value 1}}
						{"6.cmp_i" {var "support_e2_stage$"} {op "=="} {value 10}}
						{"7.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"8.entities" {selector {tag support_e2_nato_helo}} {count {op ">="} {value 1}}}
						{"9.entities" {selector {tag support_e2_nato_helo_crew}} {count {op ">="} {value 2}}}
						{"10.entities" {selector {tag support_e2_nato_helo_team}} {count {op ">="} {value 4}}}
					}
				}
				{actions
					; Consume one parked hull, both linked pilots, and four independent troops.
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag support_e2_nato_helo}}}} {amount 1}}
						{tag_add support_e2_claim}
						{tag_add support_e2_helo}
						{tag_remove support_e2_nato_helo}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag support_e2_nato_helo_crew}}}} {amount 2}}
						{tag_add support_e2_claim}
						{tag_add support_e2_helo}
						{tag_remove support_e2_nato_helo_crew}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag support_e2_nato_helo_team}}}} {amount 4}}
						{tag_add support_e2_claim}
						{tag_add support_e2_team}
						{tag_remove support_e2_nato_helo_team}
					}
					("e2_choose_flag")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
							{"set_i" {var "support_e2_stage$"} {op "="} {value 20}}
							("e2_own_current")
							{"switch"
								{"case"
									{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 0}}
									("e2_choose_lz")
									{"switch"
										{"case"
											{condition {type cmp_i} {var "support_e2_fail$"} {op "=="} {value 4}}
											; Both deep pads unsafe: delete the unused helo and deliver the
											; four claimed troops at the entry standoff. Failure remains honest.
											("e2_announce_failed")
											{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}
											("e2_place_one_entry")
											("e2_place_one_entry")
											("e2_place_one_entry")
											("e2_place_one_entry")
											("e2_order_team")
											{"set_i" {var "support_e2_stage$"} {op "="} {value 50}}
											("e2_complete_cleanup")
										}
										{"default"
											("e2_promote_helo")
											("e2_place_aircraft_entry")
											; Attested order: establish altitude, give AI control, then move.
											{"air_state"
												{selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}}
												{altitude 30}
											}
											{"actor_state"
												{selector {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft}}
												{drop sensor}
												{control AI}
												{movement {speed fast}}
											}
											("e2_order_aircraft_lz")
											{"set_i" {var "support_e2_stage$"} {op "="} {value 30}}
											; Time-based primary arrival window for the flight proof.
											{"delay" {time 40}}
											{"switch"
												{"case"
													{condition
														{type near}
														{units {ignore_captured_by_user 0} {tag support_e2_helo} {tag support_e2_aircraft} {state operatable}}
														{near_to {ignore_captured_by_user 0} {tag support_e2_lz_marker}}
														{distance 120}
													}
													("e2_announce_helo")
													{"set_i" {var "support_e2_stage$"} {op "="} {value 40}}
													("e2_place_one")
													("e2_place_one")
													("e2_place_one")
													("e2_place_one")
													("e2_order_team")
													{"set_i" {var "support_e2_stage$"} {op "="} {value 50}}
													("e2_order_aircraft_exit")
													{"set_i" {var "support_e2_stage$"} {op "="} {value 60}}
													{"delay" {time 60}}
													("e2_complete_cleanup")
												}
												{"default"
													{"set_i" {var "support_e2_fail$"} {op "="} {value 5}}
													("e2_order_aircraft_exit")
													{"delay" {time 10}}
													("e2_fail_and_cleanup")
												}
											}
										}
									}
								}
								{"default" ("e2_fail_and_cleanup")}
							}
						}
						{"default" ("e2_fail_and_cleanup")}
					}
				}
			}


			{"attack_support/e2_dispatch"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_ready$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "attack_support_use_mi$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "support_e2_stage$"} {op "=="} {value 0}}
						{"6.cmp_i" {var "support_e2_test$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "support_e2_fail$"} {op "="} {value 0}}
					; Reserve atomically before explicit child-trigger dispatch.
					{"set_i" {var "support_e2_stage$"} {op "="} {value 10}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_test$"} {op "=="} {value 1}}
							{"switch"
								{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 1}} {"trigger" {name "attack_support/e2_helo_rusa"}}}
								{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 2}} {"trigger" {name "attack_support/e2_helo_ukr"}}}
								{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 3}} {"trigger" {name "attack_support/e2_helo_nato"}}}
								{"default"
									{"set_i" {var "support_e2_fail$"} {op "="} {value 1}}
									("e2_fail_and_cleanup")
								}
							}
						}
						{"case"
							{condition {type cmp_i} {var "support_e2_test$"} {op "=="} {value 2}}
							; Task 4 supplies these triggers. Keeping the dispatch cases here makes
							; gate 2 explicit while the default-zero ship state remains inert.
							{"switch"
								{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 1}} {"trigger" {name "attack_support/e2_para_rusa"}}}
								{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 2}} {"trigger" {name "attack_support/e2_para_ukr"}}}
								{"case" {condition {type cmp_i} {var "faction_support_army$"} {op "=="} {value 3}} {"trigger" {name "attack_support/e2_para_nato"}}}
								{"default"
									{"set_i" {var "support_e2_fail$"} {op "="} {value 1}}
									("e2_fail_and_cleanup")
								}
							}
						}
						{"default"
							{"set_i" {var "support_e2_fail$"} {op "="} {value 1}}
							("e2_fail_and_cleanup")
						}
					}
					; A supported trigger claims immediately. Remaining at zero means its
					; exact hull/crew/team pool condition failed.
					{"delay" {time 1}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_e2_stage$"} {op "=="} {value 10}}
							{"set_i" {var "support_e2_fail$"} {op "="} {value 2}}
							("e2_fail_and_cleanup")
						}
						{"default"}
					}
				}
			}

			; ===== E2 PARADROP (Task 4 boundary; no release lifecycle in Task 3) =====



			; TEST OVERLAY: place the complete linked truck package in one operation.
			; Use the original spawn-centroid entry waypoint. The generated rear_a1/b1
			; pads can lie beyond the usable map edge and strand a valid truck off-map.
			(define "as_place_motor_visible"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"placement"
							{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
							{target_waypoint "attack_support_entry_b"}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement"
							{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
							{target_waypoint "attack_support_entry_a"}
						}
					}
					{"default"
						{"placement"
							{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
							{target_waypoint "attack_support_entry_b"}
						}
					}
				}
			)

			; ===== MOTORIZED INSERT (cmd 19) =====
			; Truck + linked crew/pax place at entry, drive toward flag, emit passengers,
			; pax advance, truck holds. Cap attack_support_motor_left$=1. All four factions.
			(define "as_finish_motor"
				{"delay" {time 0.5}}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag attack_support_deploy}}}}
					}
					{tag_add attack_support_src}
					{tag_remove attack_support_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
				{"delay" {time 0.2}}
				("am_own_to_support")
				{"delay" {time 0.4}}
				{"actor_state"
					{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
					{control AI}
					{ai_move {mode enable}}
					{weapon_prepare on}
					{fire_mode open}
					{move_mode free}
					{movement {speed normal} {kind normal} {type normal}}
					{ai {no_retreat on} {advance_ratio 1} {retreat_ratio 0}}
				}
				{"ables"
					{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
					{remove select}
				}
				{"delay" {time 0.25}}
				; Pick one active flag as the approach objective.
				{"entity_state"
					{selector {tag attack_support_flag1}}
					{tag_remove attack_support_flag1}
				}
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
					{tag_add attack_support_flag1}
				}
				{"delay" {time 0.1}}
				; Truck drives toward the flag (standoff handled by emit after travel delay).
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_motor_hull}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag1}}
				}
				{"delay" {time 60}}
				; Dismount passengers only (crew stays).
				{"emit"
					{selector
						{ignore_captured_by_user 0}
						{tag attack_support_motor_hull}
						{type vehicle}
						{state inhabited}
					}
					{drop orders}
					{emit
						{mode passengers}
					}
				}
				{"delay" {time 3}}
				; Pax advance on foot; truck holds.
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_motor_pax}}
					{drop orders}
					{action advance}
					{target {ignore_captured_by_user 0} {tag attack_support_flag1}}
				}
				; Driver leaves the field instead of idling: drive for waypoint "0" and the
				; cleanup trigger below removes the hull 45s later, wreck included.
				{"entity_state"
					{selector {ignore_captured_by_user 0} {tag attack_support_motor_hull}}
					{tag_add am_motor_leaving}
				}
				{"action"
					{selector {ignore_captured_by_user 0} {tag attack_support_motor_hull}}
					{drop orders}
					{action move}
					{waypoint "0"}
				}
				{"entity_state"
					{selector {tag attack_support_deploy}}
					{tag_remove attack_support_deploy}
				}
				{"entity_state"
					{selector {tag attack_support_motor_hull}}
					{tag_remove attack_support_motor_hull}
				}
				{"entity_state"
					{selector {tag attack_support_motor_pax}}
					{tag_remove attack_support_motor_pax}
				}
				{"entity_state"
					{selector {tag attack_support_g1}}
					{tag_remove attack_support_g1}
				}
				{"entity_state"
					{selector {tag attack_support_g2}}
					{tag_remove attack_support_g2}
				}
				{"entity_state"
					{selector {tag attack_support_g3}}
					{tag_remove attack_support_g3}
				}
				{"entity_state"
					{selector {tag attack_support_g4}}
					{tag_remove attack_support_g4}
				}
				{"set_i" {var "attack_support_transferred$"} {op "="} {value 1}}
			)


			{"attack_support/ally_rusa_motor"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 19}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_motor_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_rusa_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_motor")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT MOTORIZED INSERT"}}
						}
						{"default"}
					}
					; Claim whole package (hull + linked crew + pax).
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_motor}}}}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_rusa_motor}
						{tag_remove ally_sup_rusa_motor_crew}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_motor_hull}}}}}
						{tag_add attack_support_motor_hull}
						{tag_remove ally_sup_rusa_motor_hull}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_motor_pax}}}}}
						{tag_add attack_support_motor_pax}
						{tag_remove ally_sup_rusa_motor_pax}
					}
					{"set_i" {var "attack_support_motor_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("as_place_motor_visible")
					("as_finish_motor")
				}
			}

			{"attack_support/ally_ukr_motor"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 19}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_motor_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_ukr_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_motor")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT MOTORIZED INSERT"}}
						}
						{"default"}
					}
					; Claim whole package (hull + linked crew + pax).
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_motor}}}}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_ukr_motor}
						{tag_remove ally_sup_ukr_motor_crew}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_motor_hull}}}}}
						{tag_add attack_support_motor_hull}
						{tag_remove ally_sup_ukr_motor_hull}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_motor_pax}}}}}
						{tag_add attack_support_motor_pax}
						{tag_remove ally_sup_ukr_motor_pax}
					}
					{"set_i" {var "attack_support_motor_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("as_place_motor_visible")
					("as_finish_motor")
				}
			}


			{"attack_support/ally_prc_motor"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 19}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_motor_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_prc_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_motor")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT MOTORIZED INSERT"}}
						}
						{"default"}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_motor}}}}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_prc_motor}
						{tag_remove ally_sup_prc_motor_crew}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_motor_hull}}}}}
						{tag_add attack_support_motor_hull}
						{tag_remove ally_sup_prc_motor_hull}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_motor_pax}}}}}
						{tag_add attack_support_motor_pax}
						{tag_remove ally_sup_prc_motor_pax}
					}
					{"set_i" {var "attack_support_motor_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("as_place_motor_visible")
					("as_finish_motor")
				}
			}

			{"attack_support/ally_nato_motor"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_wave_cmd$"} {op "=="} {value 19}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "attack_support_motor_left$"} {op ">"} {value 0}}
						{"6.entities" {selector {tag ally_sup_nato_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "attack_support_wave_cmd$"} {op "="} {value 0}}
					("as_announce_motor")
					{"switch"
						{"case"
							{condition {type cmp_i} {var "support_debug$"} {op "=="} {value 1}}
							{"timer" {time 8} {title "ATTACK SUPPORT MOTORIZED INSERT"}}
						}
						{"default"}
					}
					; Claim whole package (hull + linked crew + pax).
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_motor}}}}}
						{tag_add attack_support_deploy}
						{tag_remove ally_sup_nato_motor}
						{tag_remove ally_sup_nato_motor_crew}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_motor_hull}}}}}
						{tag_add attack_support_motor_hull}
						{tag_remove ally_sup_nato_motor_hull}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_motor_pax}}}}}
						{tag_add attack_support_motor_pax}
						{tag_remove ally_sup_nato_motor_pax}
					}
					{"set_i" {var "attack_support_motor_left$"} {op "-"} {value 1}}
					{"delay" {time 0.1}}
					("as_place_motor_visible")
					("as_finish_motor")
				}
			}

			; TEST ONLY - forced motorized inserts, staggered. Gate attack_support_motor_test$:
			;   1 = force two trucks (armed+15s, +60s), bypassing the L2+ gate and the roll
			;   0 = production only (L2+ random roll, cap 1) - flip in attack_support/init
			; The 45s spacing exceeds one truck's full drive+emit sequence, so two trucks
			; never share a pad or an unload zone.
			; One-shot per truck: give the departing hull 45s of driving, then remove it
			; (dead or alive) so trucks never accumulate. Fires only when a hull is leaving.
; BEGIN FOUR-QUADRANT NORMAL TRANSPORT PATROL — AS

			; Ordinary combat transport: no timed emit, turnaround, withdrawal,

			; deletion, or cleanup. Route targets flag-perimeter waypoints only.

			(define "transport_as_place"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
						{"placement"
							{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
							{target_waypoint "attack_support_entry_b1"}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
						{"placement"
							{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
							{target_waypoint "attack_support_entry_a1"}
						}
					}
					{"default"
						{"placement"
							{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
							{target_waypoint "attack_support_entry_b1"}
						}
					}
				}
			)

			(define "transport_as_issue_patrol"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "transport_as_step$"} {op "=="} {value 1}}
						{"action"
							{selector {ignore_captured_by_user 0} {tag transport_as_hull}}
							{drop orders}
							{action move}
							{waypoint "transport_patrol_flag_1"}
						}
						{"set_i" {var "transport_as_step$"} {op "="} {value 2}}
					}
					{"case"
						{condition {type cmp_i} {var "transport_as_step$"} {op "=="} {value 2}}
						{"action"
							{selector {ignore_captured_by_user 0} {tag transport_as_hull}}
							{drop orders}
							{action move}
							{waypoint "transport_patrol_flag_2"}
						}
						{"set_i" {var "transport_as_step$"} {op "="} {value 3}}
					}
					{"case"
						{condition {type cmp_i} {var "transport_as_step$"} {op "=="} {value 3}}
						{"action"
							{selector {ignore_captured_by_user 0} {tag transport_as_hull}}
							{drop orders}
							{action move}
							{waypoint "transport_patrol_flag_3"}
						}
						{"set_i" {var "transport_as_step$"} {op "="} {value 4}}
					}
					{"case"
						{condition {type cmp_i} {var "transport_as_step$"} {op "=="} {value 4}}
						{"action"
							{selector {ignore_captured_by_user 0} {tag transport_as_hull}}
							{drop orders}
							{action move}
							{waypoint "transport_patrol_flag_4"}
						}
						{"set_i" {var "transport_as_step$"} {op "="} {value 5}}
					}
					{"case"
						{condition {type cmp_i} {var "transport_as_step$"} {op "=="} {value 5}}
						{"action"
							{selector {ignore_captured_by_user 0} {tag transport_as_hull}}
							{drop orders}
							{action move}
							{waypoint "transport_patrol_flag_5"}
						}
						{"set_i" {var "transport_as_step$"} {op "="} {value 1}}
					}
					{"default"
						{"set_i" {var "transport_as_step$"} {op "="} {value 1}}
					}
				}
			)

			{"attack_support/normal_transport_rusa"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_armed$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 1}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "transport_as_done$"} {op "=="} {value 0}}
						{"6.entities" {selector {tag ally_sup_rusa_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "transport_as_done$"} {op "="} {value 1}}
					{"delay" {time 45}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_motor}}}}}
						{tag_add attack_support_deploy}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_motor_hull}}}}}
						{tag_add transport_as_hull}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_rusa_motor_pax}}}}}
						{tag_add transport_as_pax}
					}
					("transport_as_place")
					{"entity_state"
						{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
						{tag_remove ally_sup_tpl}
						{tag_remove ally_sup_rusa_motor}
						{tag_remove hidden}
						{inactive off}
						{impregnability disabled}
						{discovered on}
					}
					{"delay" {time 0.2}}
					("am_own_to_support")
					{"delay" {time 0.4}}
					{"actor_state"
						{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
						{control AI}
						{ai_move {mode enable}}
						{weapon_prepare on}
						{fire_mode open}
						{move_mode free}
						{movement {speed normal} {kind normal} {type normal}}
						{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}
					}
					{"set_i" {var "transport_as_step$"} {op "="} {value 1}}
					("transport_as_issue_patrol")
					{"entity_state"
						{selector {tag attack_support_deploy}}
						{tag_remove attack_support_deploy}
					}
					{"delay" {time 45}}
					{"trigger" {name "attack_support/normal_transport_patrol"}}
				}
			}

			{"attack_support/normal_transport_ukr"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_armed$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 2}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "transport_as_done$"} {op "=="} {value 0}}
						{"6.entities" {selector {tag ally_sup_ukr_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "transport_as_done$"} {op "="} {value 1}}
					{"delay" {time 45}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_motor}}}}}
						{tag_add attack_support_deploy}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_motor_hull}}}}}
						{tag_add transport_as_hull}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_ukr_motor_pax}}}}}
						{tag_add transport_as_pax}
					}
					("transport_as_place")
					{"entity_state"
						{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
						{tag_remove ally_sup_tpl}
						{tag_remove ally_sup_ukr_motor}
						{tag_remove hidden}
						{inactive off}
						{impregnability disabled}
						{discovered on}
					}
					{"delay" {time 0.2}}
					("am_own_to_support")
					{"delay" {time 0.4}}
					{"actor_state"
						{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
						{control AI}
						{ai_move {mode enable}}
						{weapon_prepare on}
						{fire_mode open}
						{move_mode free}
						{movement {speed normal} {kind normal} {type normal}}
						{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}
					}
					{"set_i" {var "transport_as_step$"} {op "="} {value 1}}
					("transport_as_issue_patrol")
					{"entity_state"
						{selector {tag attack_support_deploy}}
						{tag_remove attack_support_deploy}
					}
					{"delay" {time 45}}
					{"trigger" {name "attack_support/normal_transport_patrol"}}
				}
			}

			{"attack_support/normal_transport_nato"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_armed$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 3}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "transport_as_done$"} {op "=="} {value 0}}
						{"6.entities" {selector {tag ally_sup_nato_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "transport_as_done$"} {op "="} {value 1}}
					{"delay" {time 45}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_motor}}}}}
						{tag_add attack_support_deploy}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_motor_hull}}}}}
						{tag_add transport_as_hull}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_nato_motor_pax}}}}}
						{tag_add transport_as_pax}
					}
					("transport_as_place")
					{"entity_state"
						{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
						{tag_remove ally_sup_tpl}
						{tag_remove ally_sup_nato_motor}
						{tag_remove hidden}
						{inactive off}
						{impregnability disabled}
						{discovered on}
					}
					{"delay" {time 0.2}}
					("am_own_to_support")
					{"delay" {time 0.4}}
					{"actor_state"
						{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
						{control AI}
						{ai_move {mode enable}}
						{weapon_prepare on}
						{fire_mode open}
						{move_mode free}
						{movement {speed normal} {kind normal} {type normal}}
						{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}
					}
					{"set_i" {var "transport_as_step$"} {op "="} {value 1}}
					("transport_as_issue_patrol")
					{"entity_state"
						{selector {tag attack_support_deploy}}
						{tag_remove attack_support_deploy}
					}
					{"delay" {time 45}}
					{"trigger" {name "attack_support/normal_transport_patrol"}}
				}
			}

			{"attack_support/normal_transport_prc"
				{condition
					{expression "1 & 2 & 3 & 4 & 5 & 6"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "attack_support_armed$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "faction_support_army$"} {op "=="} {value 4}}
						{"4.cmp_i" {var "id_attack_support$"} {op ">"} {value 0}}
						{"5.cmp_i" {var "transport_as_done$"} {op "=="} {value 0}}
						{"6.entities" {selector {tag ally_sup_prc_motor_hull}} {count {op ">="} {value 1}}}
					}
				}
				{actions
					{"set_i" {var "transport_as_done$"} {op "="} {value 1}}
					{"delay" {time 45}}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_motor}}}}}
						{tag_add attack_support_deploy}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_motor_hull}}}}}
						{tag_add transport_as_hull}
					}
					{"entity_state"
						{selector {source advanced} {group {select {tag {tag ally_sup_prc_motor_pax}}}}}
						{tag_add transport_as_pax}
					}
					("transport_as_place")
					{"entity_state"
						{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
						{tag_remove ally_sup_tpl}
						{tag_remove ally_sup_prc_motor}
						{tag_remove hidden}
						{inactive off}
						{impregnability disabled}
						{discovered on}
					}
					{"delay" {time 0.2}}
					("am_own_to_support")
					{"delay" {time 0.4}}
					{"actor_state"
						{selector {ignore_captured_by_user 0} {tag attack_support_deploy}}
						{control AI}
						{ai_move {mode enable}}
						{weapon_prepare on}
						{fire_mode open}
						{move_mode free}
						{movement {speed normal} {kind normal} {type normal}}
						{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}
					}
					{"set_i" {var "transport_as_step$"} {op "="} {value 1}}
					("transport_as_issue_patrol")
					{"entity_state"
						{selector {tag attack_support_deploy}}
						{tag_remove attack_support_deploy}
					}
					{"delay" {time 45}}
					{"trigger" {name "attack_support/normal_transport_patrol"}}
				}
			}

			; Route continues only while at least one tagged passenger remains within
			; eight metres of the hull. Normal AI dismount under contact breaks that
			; proximity and therefore stops all further scripted route orders.
			{"attack_support/normal_transport_patrol"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "transport_as_done$"} {op "=="} {value 1}}
						{"3.entities" {selector {tag transport_as_hull} {state operatable}} {count {op ">="} {value 1}}}
						{"4.near"
							{units {ignore_captured_by_user 0} {tag transport_as_pax} {state operatable}}
							{near_to {ignore_captured_by_user 0} {tag transport_as_hull} {state operatable}}
							{distance 80}
						}
					}
				}
				{actions
					("transport_as_issue_patrol")
					{"delay" {time 45}}
					{"trigger" {name "attack_support/normal_transport_patrol"}}
				}
			}

; END FOUR-QUADRANT NORMAL TRANSPORT PATROL — AS

			{"attack_support/motor_cleanup"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.entities" {selector {tag am_motor_leaving}} {count {op ">"} {value 0}}}
					}
				}
				{actions
					{"delay" {time 90}}
					{"delete" {selector {ignore_captured_by_user 0} {tag am_motor_leaving}}}
				}
			}
