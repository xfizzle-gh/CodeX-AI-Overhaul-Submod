; Enemy defender garrison, patrols and edge reinforcements.
; The mirror image of attack_support_waves.inc, aimed the other way: on a HUMAN
; ATTACK mission (campaign_capture_the_flag, user_is_defender$ == 0) it gives the
; AI defender MI-driven infantry the purchase economy never buys it - a garrison
; sitting on the live objectives from the first minute, patrols that keep the map
; alive between assaults, and reinforcements walking in off its own map edge.
;
; Included by all fourteen CWA campaign_capture_the_flag.mi maps. Provably inert on
; a human-DEFENCE mission: every single trigger carries user_is_defender$ == 0, and
; enemy_defense/init additionally waits for id_1st_enemy$ and enemy_spawnside$,
; which conquest.lua only publishes from its mission-authority branch.
;
; DELIVERY (identical to the attack-support engine, do not deviate): the off-map
; real-breed prototypes in enemy_defense_templates.inc are MOVED with {"placement"}
; and promoted in place. Never cloned - three promote designs each matched zero
; freshly created entities, because a new entity's provenance is invisible to every
; selector this format can express. Ownership is a literal 1-16 {player} switch on
; id_1st_enemy$ (the live defender bot on an attack mission); the engine will not
; accept a var in a {player} node.
;
; FACTION SELECTION is a var switch, not a cascade. conquest.lua publishes
; bot_army$ from its nationMap (1 rusa, 2 ukr, 3 nato, 4 csa, 5 sov, 6 prc, 7 frg,
; 8 pol); ed_resolve_army folds that onto the four pools this mod ships
; (enemy_defense_army$ 1 rusa / 2 ukr / 3 nato / 4 prc), routing sov+pol to the
; rusa pool and csa+frg to the nato pool. An unpublished bot_army$ lands in the
; default and draws rusa, so the system never stalls waiting for a var.
;
; WHAT ARRIVES, AND WHEN
;   garrison   ~12s in, one 4-man line fireteam MOVED onto each ACTIVE flag point
;              (a mission activates only ~2 of a map's 2-5 flag_point entities, so
;              the claim excludes {state inactive}). They take cover where they land.
;   patrols    four self-re-arming per-group re-order triggers, each on its own
;              60-120s ladder. ~50% of re-orders cycle the group between the active
;              flags, ~50% send it roaming - the spare (inactive) flag points and
;              the two map-edge entry waypoints are the only anchors that exist on
;              all fourteen maps, so they are the roam set. Groups 1-3 are the
;              garrison, 4 is the surge stream; trickle arrivals join group 3.
;   trickle    self-re-arming 45/60/75/90s ladder, one 3-man light team per cycle.
;   surge      self-re-arming 180/220/260/300s ladder, one 4-man fireteam per cycle,
;              weapons-flavoured (AT + marksman) at level 2 and above.
;   Both spawners deliver at the DEFENDER's own map edge: attack_support_entry_a
;   when enemy_spawnside$ is 1, _b when it is 2. Note this is the opposite reading
;   of enemy_spawnside$ from the attack-support engine, which deliberately enters
;   from the side the enemy is NOT on.
;
; BUDGETS, kept deliberately at or below the friendly attack-support system so the
; two halves stay in proportion:
;   waves      defense_level$ (CE campaign progression, 1-3): L1 4, L2 6, L3 8,
;              SHARED by the trickle and surge spawners, so the two ladders stay
;              asynchronous without doubling the total. Level 0 falls back to L1.
;   garrison   one fireteam per active flag, on top of the wave budget and not
;              drawn from it - it is the standing defence, not a reinforcement.
;   live cap   more than 16 live enemy_def_src infantry defers the cycle WITHOUT
;              consuming a wave, exactly as the attack-support cap does.
;
; Fireteams are 3-4 strong, weapon_prepare on, fire_mode open and no_retreat OFF:
; these are patrolling infantry that may give ground, not a suicide push.

			(define "ed_own_to_enemy"
				{"switch"
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 1}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "1"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 2}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "2"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 3}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "3"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 4}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "4"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 5}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "5"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 6}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "6"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 7}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "7"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 8}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "8"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 9}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "9"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 10}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "10"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 11}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "11"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 12}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "12"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 13}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "13"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 14}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "14"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 15}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "15"}}}
					{"case" {condition {type cmp_i} {var "id_1st_enemy$"} {op "=="} {value 16}} {"player" {selector {ignore_captured_by_user 0} {tag enemy_def_deploy}} {operation set} {player "16"}}}
					{"default"
						{"player"
							{selector {ignore_captured_by_user 0} {tag enemy_def_deploy}}
							{operation set}
							{player "2"}
						}
					}
				}
			)

			; Delivery point. The garrison is MOVED onto the flag point it is meant to
			; hold - {"placement"} takes a plain {target} entity selector, the same form
			; dcg_functions.inc uses to drop conquest spawn helpers onto a1..b4 - while
			; every reinforcement enters at the defender's own map edge.
			(define "ed_place"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_defense_place$"} {op "=="} {value 1}}
						{"placement"
							{selector
								{source advanced}
								{group {select {tag {tag enemy_def_deploy}}}}
							}
							{target {ignore_captured_by_user 0} {tag enemy_def_af1}}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_defense_place$"} {op "=="} {value 2}}
						{"placement"
							{selector
								{source advanced}
								{group {select {tag {tag enemy_def_deploy}}}}
							}
							{target {ignore_captured_by_user 0} {tag enemy_def_af2}}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_defense_place$"} {op "=="} {value 3}}
						{"placement"
							{selector
								{source advanced}
								{group {select {tag {tag enemy_def_deploy}}}}
							}
							{target {ignore_captured_by_user 0} {tag enemy_def_af3}}
						}
					}
					{"default"
						{"switch"
							{"case"
								{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 1}}
								{"placement"
									{selector
										{source advanced}
										{group {select {tag {tag enemy_def_deploy}}}}
									}
									{target_waypoint "attack_support_entry_a"}
								}
							}
							{"case"
								{condition {type cmp_i} {var "enemy_spawnside$"} {op "=="} {value 2}}
								{"placement"
									{selector
										{source advanced}
										{group {select {tag {tag enemy_def_deploy}}}}
									}
									{target_waypoint "attack_support_entry_b"}
								}
							}
							{"default"
								{"placement"
									{selector
										{source advanced}
										{group {select {tag {tag enemy_def_deploy}}}}
									}
									{target_waypoint "attack_support_entry_a"}
								}
							}
						}
					}
				}
			)

			; Patrol group membership. Tag-swapped on purpose: a deployed body carries
			; enemy_def_src and enemy_def_pN but never a pool tag again, so no spawner can
			; re-pick a patroller. The patrol triggers are armed from map load and fire the
			; moment their group has a live member, so nothing has to poke them here.
			(define "ed_assign_group"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_defense_group$"} {op "=="} {value 1}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag enemy_def_deploy}}}}
							}
							{tag_add enemy_def_p1}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_defense_group$"} {op "=="} {value 2}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag enemy_def_deploy}}}}
							}
							{tag_add enemy_def_p2}
						}
					}
					{"case"
						{condition {type cmp_i} {var "enemy_defense_group$"} {op "=="} {value 3}}
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag enemy_def_deploy}}}}
							}
							{tag_add enemy_def_p3}
						}
					}
					{"default"
						{"entity_state"
							{selector
								{source advanced}
								{group {select {tag {tag enemy_def_deploy}}}}
							}
							{tag_add enemy_def_p4}
						}
					}
				}
			)

			; Promote + own + hand to the bot AI. enemy_def_src is the live roster marker
			; and is never removed: the live-unit cap counts it. A garrison team digs in
			; where it landed; a reinforcement pushes for the first active flag and is then
			; taken over by its patrol trigger.
			(define "ed_finish"
				{"delay" {time 0.5}}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag enemy_def_deploy}}}}
					}
					{tag_add enemy_def_src}
					{tag_remove enemy_def_tpl}
					{tag_remove hidden}
					{inactive off}
					{impregnability disabled}
					{discovered on}
				}
				{"delay" {time 0.2}}
				("ed_own_to_enemy")
				{"delay" {time 0.4}}
				{"actor_state"
					{selector {ignore_captured_by_user 0} {tag enemy_def_deploy}}
					{control AI}
					{ai_move {mode enable}}
					{weapon_prepare on}
					{fire_mode open}
					{move_mode free}
					{movement {speed normal} {kind normal} {type normal}}
					{ai {no_retreat off} {advance_ratio 1} {retreat_ratio 0}}
				}
				{"ables"
					{selector {ignore_captured_by_user 0} {tag enemy_def_deploy}}
					{remove select}
				}
				{"delay" {time 1}}
				("ed_assign_group")
				{"delay" {time 0.2}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_defense_place$"} {op ">"} {value 0}}
						{"actor_to_cover"
							{selector {ignore_captured_by_user 0} {tag enemy_def_deploy}}
						}
					}
					{"default"
						{"action"
							{selector {ignore_captured_by_user 0} {tag enemy_def_deploy}}
							{drop orders}
							{action advance}
							{target {ignore_captured_by_user 0} {tag enemy_def_af1}}
						}
					}
				}
				{"delay" {time 0.2}}
				{"entity_state"
					{selector
						{source advanced}
						{group {select {tag {tag enemy_def_deploy}}}}
					}
					{tag_remove enemy_def_deploy}
				}
				{"set_i" {var "enemy_defense_transferred$"} {op "="} {value 1}}
			)

			; bot_army$ -> enemy_defense_army$. conquest.lua publishes bot_army$ from its
			; nationMap (1 rusa, 2 ukr, 3 nato, 4 csa, 5 sov, 6 prc, 7 frg, 8 pol) in the
			; same mission-authority call that writes user_is_defender$, so it is already
			; there by the time init runs. sov and pol draw the rusa pool, csa and frg the
			; nato pool, and an unpublished 0 lands in the default rather than stalling.
			(define "ed_resolve_army"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 2}}
						{"set_i" {var "enemy_defense_army$"} {op "="} {value 2}}
						{"timer" {time 8} {title "ENEMY DEFENSE ARMY - UKR"}}
					}
					{"case"
						{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 3}}
						{"set_i" {var "enemy_defense_army$"} {op "="} {value 3}}
						{"timer" {time 8} {title "ENEMY DEFENSE ARMY - NATO"}}
					}
					{"case"
						{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 4}}
						{"set_i" {var "enemy_defense_army$"} {op "="} {value 3}}
						{"timer" {time 8} {title "ENEMY DEFENSE ARMY - NATO (csa)"}}
					}
					{"case"
						{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 6}}
						{"set_i" {var "enemy_defense_army$"} {op "="} {value 4}}
						{"timer" {time 8} {title "ENEMY DEFENSE ARMY - PRC"}}
					}
					{"case"
						{condition {type cmp_i} {var "bot_army$"} {op "=="} {value 7}}
						{"set_i" {var "enemy_defense_army$"} {op "="} {value 3}}
						{"timer" {time 8} {title "ENEMY DEFENSE ARMY - NATO (frg)"}}
					}
					{"default"
						{"set_i" {var "enemy_defense_army$"} {op "="} {value 1}}
						{"timer" {time 8} {title "ENEMY DEFENSE ARMY - RUSA"}}
					}
				}
			)

			; Anchors, claimed once: flag activation is fixed for the mission. af1..af3 are
			; the ACTIVE flag points, shuffled - a mission activates only ~2 of a map's 2-5
			; flag_point entities and a squad sent to a dead objective just stands there.
			; r1/r2 are the leftovers, which is exactly the set of spare flag points, and
			; they are the roam anchors. Capture points are addressed as {tag flag}: the
			; fpc1..fpc5 tags are missing from one of the fourteen maps entirely.
			(define "ed_claim_anchors"
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add enemy_def_af1}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
								{tag {tag enemy_def_af1}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add enemy_def_af2}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{state {state inactive}}
								{tag {tag enemy_def_af1}}
								{tag {tag enemy_def_af2}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add enemy_def_af3}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{tag {tag enemy_def_af1}}
								{tag {tag enemy_def_af2}}
								{tag {tag enemy_def_af3}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add enemy_def_r1}
				}
				{"delay" {time 0.05}}
				{"entity_state"
					{selector
						{source advanced}
						{group
							{select {tag {tag flag}}}
							{exclude
								{tag {tag enemy_def_af1}}
								{tag {tag enemy_def_af2}}
								{tag {tag enemy_def_af3}}
								{tag {tag enemy_def_r1}}
							}
						}
						{sort {type shuffle}}
						{amount 1}
					}
					{tag_add enemy_def_r2}
				}
				{"delay" {time 0.05}}
			)

			; Command 1 - light team, 3 bodies from the line pool. Only the trigger whose
			; enemy_defense_army$ matches can clear it, so poking all four is safe.
			(define "ed_poke_light"
				{"trigger" {name "enemy_defense/rusa_light"}}
				{"trigger" {name "enemy_defense/ukr_light"}}
				{"trigger" {name "enemy_defense/prc_light"}}
				{"trigger" {name "enemy_defense/nato_light"}}
			)

			; Command 2 - line fireteam, 4 bodies from the line pool. Only the trigger whose
			; enemy_defense_army$ matches can clear it, so poking all four is safe.
			(define "ed_poke_line"
				{"trigger" {name "enemy_defense/rusa_line"}}
				{"trigger" {name "enemy_defense/ukr_line"}}
				{"trigger" {name "enemy_defense/prc_line"}}
				{"trigger" {name "enemy_defense/nato_line"}}
			)

			; Command 3 - weapons fireteam, 4 bodies from the wpn pool. Only the trigger whose
			; enemy_defense_army$ matches can clear it, so poking all four is safe.
			(define "ed_poke_wpn"
				{"trigger" {name "enemy_defense/rusa_wpn"}}
				{"trigger" {name "enemy_defense/ukr_wpn"}}
				{"trigger" {name "enemy_defense/prc_wpn"}}
				{"trigger" {name "enemy_defense/nato_wpn"}}
			)

			; The garrison draw: one 4-strong line fireteam for the flag named by
			; enemy_defense_place$. A composition clears the command as its first action,
			; so a command still standing four seconds later means the pool came up short.
			(define "ed_pick_garrison"
				{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 2}}
				("ed_poke_line")
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_defense_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
						{"timer" {time 6} {title "ENEMY DEFENSE POOL EXHAUSTED"}}
					}
					{"default"}
				}
			)

			; The trickle draw: one 3-strong light team off the line pool, every level.
			(define "ed_pick_light"
				{"timer" {time 8} {title "ENEMY DEFENSE TRICKLE - LIGHT TEAM"}}
				{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 1}}
				("ed_poke_light")
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_defense_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
						{"timer" {time 6} {title "ENEMY DEFENSE POOL EXHAUSTED"}}
					}
					{"default"}
				}
			)

			; The surge draw, widening with the campaign level: L1 fields plain line
			; fireteams, L2 mixes in the AT/marksman weapons team a third of the time, L3
			; leads with it. Level 0 - CE has not published a level yet - lands in the
			; default and behaves as L1.
			(define "ed_pick_squad"
				{"switch"
					{"case"
						{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
						{"switch"
							{"case"
								{condition {type rand} {value 0.6}}
								{"timer" {time 8} {title "ENEMY DEFENSE SURGE - WEAPONS FIRETEAM"}}
								{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 3}}
								("ed_poke_wpn")
							}
							{"default"
								{"timer" {time 8} {title "ENEMY DEFENSE SURGE - LINE FIRETEAM"}}
								{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 2}}
								("ed_poke_line")
							}
						}
					}
					{"case"
						{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
						{"switch"
							{"case"
								{condition {type rand} {value 0.34}}
								{"timer" {time 8} {title "ENEMY DEFENSE SURGE - WEAPONS FIRETEAM"}}
								{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 3}}
								("ed_poke_wpn")
							}
							{"default"
								{"timer" {time 8} {title "ENEMY DEFENSE SURGE - LINE FIRETEAM"}}
								{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 2}}
								("ed_poke_line")
							}
						}
					}
					{"default"
						{"timer" {time 8} {title "ENEMY DEFENSE SURGE - LINE FIRETEAM"}}
						{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 2}}
						("ed_poke_line")
					}
				}
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_defense_wave_cmd$"} {op ">"} {value 2}}
						{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 2}}
						("ed_poke_line")
						{"timer" {time 6} {title "ENEMY DEFENSE POOL SHORT - LINE TEAM INSTEAD"}}
					}
					{"default"}
				}
				{"delay" {time 4}}
				{"switch"
					{"case"
						{condition {type cmp_i} {var "enemy_defense_wave_cmd$"} {op ">"} {value 0}}
						{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
						{"timer" {time 6} {title "ENEMY DEFENSE POOL EXHAUSTED"}}
					}
					{"default"}
				}
			)

			; Arms once per attack mission. enemy_spawnside$ > 0 is the readiness signal:
			; conquest.lua writes it immediately after user_is_defender$ inside the same
			; mission-authority call, so a positive spawn side proves the defence-side
			; perspective vars have actually been published rather than merely defaulting
			; to 0 - which is what makes this engine provably inert on a defence mission.
			{"enemy_defense/init"
				{condition
					{expression "1 & 2 & 3 & 4"}
					{terms
						{"1.cmp_i" {var "enemy_defense_armed$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_spawnside$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_armed$"} {op "="} {value 1}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 1}}
					{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_wave_num$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_place$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_group$"} {op "="} {value 4}}
					{"set_i" {var "enemy_defense_trickle_ok$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_trickle_busy$"} {op "="} {value 1}}
					{"set_i" {var "enemy_defense_surge_ok$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_surge_busy$"} {op "="} {value 1}}
					{"delay" {time 2}}
					("ed_resolve_army")
					; Wave budget by campaign progression, shared by both spawners.
					{"switch"
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 3}}
							{"set_i" {var "enemy_defense_waves_left$"} {op "="} {value 8}}
							{"timer" {time 10} {title "ENEMY DEFENSE ARMED - L3, 8 WAVES"}}
						}
						{"case"
							{condition {type cmp_i} {var "defense_level$"} {op "=="} {value 2}}
							{"set_i" {var "enemy_defense_waves_left$"} {op "="} {value 6}}
							{"timer" {time 10} {title "ENEMY DEFENSE ARMED - L2, 6 WAVES"}}
						}
						{"default"
							{"set_i" {var "enemy_defense_waves_left$"} {op "="} {value 4}}
							{"timer" {time 10} {title "ENEMY DEFENSE ARMED - L1, 4 WAVES"}}
						}
					}
					("ed_claim_anchors")
					; Let the mission place its own opening forces before the garrison lands.
					{"delay" {time 12}}
					{"if"
						{condition {type entities} {selector {tag enemy_def_af1}}}
						{"timer" {time 8} {title "ENEMY DEFENSE GARRISON AT FLAG 1"}}
						{"set_i" {var "enemy_defense_place$"} {op "="} {value 1}}
						{"set_i" {var "enemy_defense_group$"} {op "="} {value 1}}
						("ed_pick_garrison")
					}
					{"delay" {time 8}}
					{"if"
						{condition {type entities} {selector {tag enemy_def_af2}}}
						{"timer" {time 8} {title "ENEMY DEFENSE GARRISON AT FLAG 2"}}
						{"set_i" {var "enemy_defense_place$"} {op "="} {value 2}}
						{"set_i" {var "enemy_defense_group$"} {op "="} {value 2}}
						("ed_pick_garrison")
					}
					{"delay" {time 8}}
					{"if"
						{condition {type entities} {selector {tag enemy_def_af3}}}
						{"timer" {time 8} {title "ENEMY DEFENSE GARRISON AT FLAG 3"}}
						{"set_i" {var "enemy_defense_place$"} {op "="} {value 3}}
						{"set_i" {var "enemy_defense_group$"} {op "="} {value 3}}
						("ed_pick_garrison")
					}
					{"delay" {time 8}}
					{"set_i" {var "enemy_defense_place$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 2}}
					; Hand the cadence over only once the garrison has finished landing. The
					; two ladders are independent from here, so arrivals never synchronise.
					{"delay" {time 10}}
					{"set_i" {var "enemy_defense_trickle_busy$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_surge_busy$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_trickle_ok$"} {op "="} {value 1}}
					{"set_i" {var "enemy_defense_surge_ok$"} {op "="} {value 1}}
					{"trigger" {name "enemy_defense/trickle"}}
					{"trigger" {name "enemy_defense/surge"}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 3}}
				}
			}

			; TRICKLE SPAWNER - the light stream. Its own 45-90s ladder and its own
			; busy latch, so it is completely independent of the surge spawner below;
			; the only thing the two share is the wave budget. Arrivals join patrol
			; group 3, which the third garrison team also uses when a map has a third
			; active flag.
			{"enemy_defense/trickle"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "enemy_defense_trickle_ok$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "enemy_defense_trickle_busy$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "enemy_defense_waves_left$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"5.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_trickle_busy$"} {op "="} {value 1}}
					{"switch"
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 45}}
						}
						{"case"
							{condition {type rand} {value 0.34}}
							{"delay" {time 60}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 75}}
						}
						{"default"
							{"delay" {time 90}}
						}
					}
					{"delay" {time 0.1}}
					{"switch"
						; LIVE-UNIT CAP. Skip the cycle without consuming a wave while the
						; defender still has a platoon of MI infantry on the field. Counted on
						; the simple selector form the mission scripts use for live units - the
						; advanced selector's prop/state decorations zero the match on these.
						{"case"
							{condition
								{type entities}
								{selector
									{ignore_captured_by_user 0}
									{tag enemy_def_src}
									{type human}
									{state "not dead"}
								}
								{count {op ">"} {value 16}}
							}
							{"timer" {time 8} {title "ENEMY DEFENSE NEAR CAP DEFER"}}
						}
						{"default"
							{"set_i" {var "enemy_defense_wave_num$"} {op "+"} {value 1}}
							{"set_i" {var "enemy_defense_waves_left$"} {op "-"} {value 1}}
							{"set_i" {var "enemy_defense_place$"} {op "="} {value 0}}
							{"set_i" {var "enemy_defense_group$"} {op "="} {value 3}}
							("ed_pick_light")
						}
					}
					{"delay" {time 0.1}}
					{"set_i" {var "enemy_defense_trickle_busy$"} {op "="} {value 0}}
					{"trigger" {name "enemy_defense/trickle"}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "enemy_defense_waves_left$"} {op "=="} {value 0}}
							{"timer" {time 10} {title "ENEMY DEFENSE WAVES EXHAUSTED"}}
						}
						{"default"}
					}
				}
			}

			; SURGE SPAWNER - the squad stream, on a much slower 180-300s ladder with an
			; independent latch. Arrivals join patrol group 4.
			{"enemy_defense/surge"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "enemy_defense_surge_ok$"} {op "=="} {value 1}}
						{"2.cmp_i" {var "enemy_defense_surge_busy$"} {op "=="} {value 0}}
						{"3.cmp_i" {var "enemy_defense_waves_left$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"5.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_surge_busy$"} {op "="} {value 1}}
					{"switch"
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 180}}
						}
						{"case"
							{condition {type rand} {value 0.34}}
							{"delay" {time 220}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 260}}
						}
						{"default"
							{"delay" {time 300}}
						}
					}
					{"delay" {time 0.1}}
					{"switch"
						; LIVE-UNIT CAP. Skip the cycle without consuming a wave while the
						; defender still has a platoon of MI infantry on the field. Counted on
						; the simple selector form the mission scripts use for live units - the
						; advanced selector's prop/state decorations zero the match on these.
						{"case"
							{condition
								{type entities}
								{selector
									{ignore_captured_by_user 0}
									{tag enemy_def_src}
									{type human}
									{state "not dead"}
								}
								{count {op ">"} {value 16}}
							}
							{"timer" {time 8} {title "ENEMY DEFENSE NEAR CAP DEFER"}}
						}
						{"default"
							{"set_i" {var "enemy_defense_wave_num$"} {op "+"} {value 1}}
							{"set_i" {var "enemy_defense_waves_left$"} {op "-"} {value 1}}
							{"set_i" {var "enemy_defense_place$"} {op "="} {value 0}}
							{"set_i" {var "enemy_defense_group$"} {op "="} {value 4}}
							("ed_pick_squad")
						}
					}
					{"delay" {time 0.1}}
					{"set_i" {var "enemy_defense_surge_busy$"} {op "="} {value 0}}
					{"trigger" {name "enemy_defense/surge"}}
					{"switch"
						{"case"
							{condition {type cmp_i} {var "enemy_defense_waves_left$"} {op "=="} {value 0}}
							{"timer" {time 10} {title "ENEMY DEFENSE WAVES EXHAUSTED"}}
						}
						{"default"}
					}
				}
			}

			; PATROL GROUP 1. Self-re-arming re-order on its own 60-120s ladder, so the
			; four groups never move in step. Roughly half of all re-orders cycle the group
			; between the active flag points and half send it roaming: the spare flag
			; points (r1/r2) plus the two map-edge entry waypoints are the only anchors
			; that exist on all fourteen maps. Maps with only two flag points have no
			; spare, so each roam-flag branch falls back to waypoint "0". The condition
			; simply stops matching once the group is wiped, and matches again if a later
			; wave joins the group.
			{"enemy_defense/patrol_1"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag enemy_def_p1}
								{type human}
								{state "not dead"}
							}
							{count {op ">"} {value 0}}
						}
					}
				}
				{actions
					{"switch"
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 60}}
						}
						{"case"
							{condition {type rand} {value 0.34}}
							{"delay" {time 80}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 100}}
						}
						{"default"
							{"delay" {time 120}}
						}
					}
					{"delay" {time 0.1}}
					{"switch"
						{"case"
							{condition {type rand} {value 0.25}}
							{"action"
								{selector {ignore_captured_by_user 0} {tag enemy_def_p1}}
								{drop orders}
								{action advance}
								{target {ignore_captured_by_user 0} {tag enemy_def_af1}}
							}
						}
						{"case"
							{condition {type rand} {value 0.34}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag enemy_def_af2}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p1}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_af2}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p1}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_af1}}
									}
								}
							}
						}
						{"case"
							{condition {type rand} {value 0.4}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag enemy_def_r1}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p1}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_r1}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p1}}
										{drop orders}
										{action move}
										{waypoint "0"}
									}
								}
							}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag enemy_def_r2}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p1}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_r2}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p1}}
										{drop orders}
										{action move}
										{waypoint "0"}
									}
								}
							}
						}
						{"case"
							{condition {type rand} {value 0.6}}
							{"action"
								{selector {ignore_captured_by_user 0} {tag enemy_def_p1}}
								{drop orders}
								{action move}
								{waypoint "attack_support_entry_a"}
							}
						}
						{"default"
							{"action"
								{selector {ignore_captured_by_user 0} {tag enemy_def_p1}}
								{drop orders}
								{action move}
								{waypoint "attack_support_entry_b"}
							}
						}
					}
					{"delay" {time 0.1}}
					{"trigger" {name "enemy_defense/patrol_1"}}
				}
			}

			; PATROL GROUP 2. Self-re-arming re-order on its own 60-120s ladder, so the
			; four groups never move in step. Roughly half of all re-orders cycle the group
			; between the active flag points and half send it roaming: the spare flag
			; points (r1/r2) plus the two map-edge entry waypoints are the only anchors
			; that exist on all fourteen maps. Maps with only two flag points have no
			; spare, so each roam-flag branch falls back to waypoint "0". The condition
			; simply stops matching once the group is wiped, and matches again if a later
			; wave joins the group.
			{"enemy_defense/patrol_2"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag enemy_def_p2}
								{type human}
								{state "not dead"}
							}
							{count {op ">"} {value 0}}
						}
					}
				}
				{actions
					{"switch"
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 60}}
						}
						{"case"
							{condition {type rand} {value 0.34}}
							{"delay" {time 80}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 100}}
						}
						{"default"
							{"delay" {time 120}}
						}
					}
					{"delay" {time 0.1}}
					{"switch"
						{"case"
							{condition {type rand} {value 0.25}}
							{"action"
								{selector {ignore_captured_by_user 0} {tag enemy_def_p2}}
								{drop orders}
								{action advance}
								{target {ignore_captured_by_user 0} {tag enemy_def_af1}}
							}
						}
						{"case"
							{condition {type rand} {value 0.34}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag enemy_def_af2}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p2}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_af2}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p2}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_af1}}
									}
								}
							}
						}
						{"case"
							{condition {type rand} {value 0.4}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag enemy_def_r1}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p2}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_r1}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p2}}
										{drop orders}
										{action move}
										{waypoint "0"}
									}
								}
							}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag enemy_def_r2}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p2}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_r2}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p2}}
										{drop orders}
										{action move}
										{waypoint "0"}
									}
								}
							}
						}
						{"case"
							{condition {type rand} {value 0.6}}
							{"action"
								{selector {ignore_captured_by_user 0} {tag enemy_def_p2}}
								{drop orders}
								{action move}
								{waypoint "attack_support_entry_a"}
							}
						}
						{"default"
							{"action"
								{selector {ignore_captured_by_user 0} {tag enemy_def_p2}}
								{drop orders}
								{action move}
								{waypoint "attack_support_entry_b"}
							}
						}
					}
					{"delay" {time 0.1}}
					{"trigger" {name "enemy_defense/patrol_2"}}
				}
			}

			; PATROL GROUP 3. Self-re-arming re-order on its own 60-120s ladder, so the
			; four groups never move in step. Roughly half of all re-orders cycle the group
			; between the active flag points and half send it roaming: the spare flag
			; points (r1/r2) plus the two map-edge entry waypoints are the only anchors
			; that exist on all fourteen maps. Maps with only two flag points have no
			; spare, so each roam-flag branch falls back to waypoint "0". The condition
			; simply stops matching once the group is wiped, and matches again if a later
			; wave joins the group.
			{"enemy_defense/patrol_3"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag enemy_def_p3}
								{type human}
								{state "not dead"}
							}
							{count {op ">"} {value 0}}
						}
					}
				}
				{actions
					{"switch"
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 60}}
						}
						{"case"
							{condition {type rand} {value 0.34}}
							{"delay" {time 80}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 100}}
						}
						{"default"
							{"delay" {time 120}}
						}
					}
					{"delay" {time 0.1}}
					{"switch"
						{"case"
							{condition {type rand} {value 0.25}}
							{"action"
								{selector {ignore_captured_by_user 0} {tag enemy_def_p3}}
								{drop orders}
								{action advance}
								{target {ignore_captured_by_user 0} {tag enemy_def_af1}}
							}
						}
						{"case"
							{condition {type rand} {value 0.34}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag enemy_def_af2}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p3}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_af2}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p3}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_af1}}
									}
								}
							}
						}
						{"case"
							{condition {type rand} {value 0.4}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag enemy_def_r1}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p3}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_r1}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p3}}
										{drop orders}
										{action move}
										{waypoint "0"}
									}
								}
							}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag enemy_def_r2}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p3}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_r2}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p3}}
										{drop orders}
										{action move}
										{waypoint "0"}
									}
								}
							}
						}
						{"case"
							{condition {type rand} {value 0.6}}
							{"action"
								{selector {ignore_captured_by_user 0} {tag enemy_def_p3}}
								{drop orders}
								{action move}
								{waypoint "attack_support_entry_a"}
							}
						}
						{"default"
							{"action"
								{selector {ignore_captured_by_user 0} {tag enemy_def_p3}}
								{drop orders}
								{action move}
								{waypoint "attack_support_entry_b"}
							}
						}
					}
					{"delay" {time 0.1}}
					{"trigger" {name "enemy_defense/patrol_3"}}
				}
			}

			; PATROL GROUP 4. Self-re-arming re-order on its own 60-120s ladder, so the
			; four groups never move in step. Roughly half of all re-orders cycle the group
			; between the active flag points and half send it roaming: the spare flag
			; points (r1/r2) plus the two map-edge entry waypoints are the only anchors
			; that exist on all fourteen maps. Maps with only two flag points have no
			; spare, so each roam-flag branch falls back to waypoint "0". The condition
			; simply stops matching once the group is wiped, and matches again if a later
			; wave joins the group.
			{"enemy_defense/patrol_4"
				{condition
					{expression "1 & 2"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.entities"
							{selector
								{ignore_captured_by_user 0}
								{tag enemy_def_p4}
								{type human}
								{state "not dead"}
							}
							{count {op ">"} {value 0}}
						}
					}
				}
				{actions
					{"switch"
						{"case"
							{condition {type rand} {value 0.25}}
							{"delay" {time 60}}
						}
						{"case"
							{condition {type rand} {value 0.34}}
							{"delay" {time 80}}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"delay" {time 100}}
						}
						{"default"
							{"delay" {time 120}}
						}
					}
					{"delay" {time 0.1}}
					{"switch"
						{"case"
							{condition {type rand} {value 0.25}}
							{"action"
								{selector {ignore_captured_by_user 0} {tag enemy_def_p4}}
								{drop orders}
								{action advance}
								{target {ignore_captured_by_user 0} {tag enemy_def_af1}}
							}
						}
						{"case"
							{condition {type rand} {value 0.34}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag enemy_def_af2}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p4}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_af2}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p4}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_af1}}
									}
								}
							}
						}
						{"case"
							{condition {type rand} {value 0.4}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag enemy_def_r1}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p4}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_r1}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p4}}
										{drop orders}
										{action move}
										{waypoint "0"}
									}
								}
							}
						}
						{"case"
							{condition {type rand} {value 0.5}}
							{"switch"
								{"case"
									{condition {type entities} {selector {tag enemy_def_r2}}}
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p4}}
										{drop orders}
										{action advance}
										{target {ignore_captured_by_user 0} {tag enemy_def_r2}}
									}
								}
								{"default"
									{"action"
										{selector {ignore_captured_by_user 0} {tag enemy_def_p4}}
										{drop orders}
										{action move}
										{waypoint "0"}
									}
								}
							}
						}
						{"case"
							{condition {type rand} {value 0.6}}
							{"action"
								{selector {ignore_captured_by_user 0} {tag enemy_def_p4}}
								{drop orders}
								{action move}
								{waypoint "attack_support_entry_a"}
							}
						}
						{"default"
							{"action"
								{selector {ignore_captured_by_user 0} {tag enemy_def_p4}}
								{drop orders}
								{action move}
								{waypoint "attack_support_entry_b"}
							}
						}
					}
					{"delay" {time 0.1}}
					{"trigger" {name "enemy_defense/patrol_4"}}
				}
			}

			; COMPOSITION TRIGGERS - four faction pools x three draws. Command gated the
			; way the attack-support compositions are: the command IS the draw id and the
			; trigger clears it as its first action, so a command still standing means no
			; pool could field it. Each also gates on enemy_defense_army$, so exactly one
			; faction can ever answer a poke, and on its own pool being stocked - a claim
			; strips the pool tag, so the tag count is precisely "still parked".
			{"enemy_defense/rusa_light"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "enemy_defense_wave_cmd$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_defense_army$"} {op "=="} {value 1}}
						{"5.entities"
							{selector {tag enemy_def_rusa_line}}
							{count {op ">="} {value 3}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 11}}
					{"timer" {time 8} {title "ENEMY DEFENSE RUSA LIGHT TEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_rusa_line}}}}
							{amount 3}
						}
						{tag_add enemy_def_deploy}
						{tag_remove enemy_def_rusa_line}
					}
					{"delay" {time 0.1}}
					("ed_place")
					("ed_finish")
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 12}}
				}
			}

			{"enemy_defense/rusa_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "enemy_defense_wave_cmd$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_defense_army$"} {op "=="} {value 1}}
						{"5.entities"
							{selector {tag enemy_def_rusa_line}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 21}}
					{"timer" {time 8} {title "ENEMY DEFENSE RUSA LINE FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_rusa_line}}}}
							{amount 4}
						}
						{tag_add enemy_def_deploy}
						{tag_remove enemy_def_rusa_line}
					}
					{"delay" {time 0.1}}
					("ed_place")
					("ed_finish")
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 22}}
				}
			}

			{"enemy_defense/rusa_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "enemy_defense_wave_cmd$"} {op "=="} {value 3}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_defense_army$"} {op "=="} {value 1}}
						{"5.entities"
							{selector {tag enemy_def_rusa_wpn}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 31}}
					{"timer" {time 8} {title "ENEMY DEFENSE RUSA WEAPONS FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_rusa_wpn}}}}
							{amount 4}
						}
						{tag_add enemy_def_deploy}
						{tag_remove enemy_def_rusa_wpn}
					}
					{"delay" {time 0.1}}
					("ed_place")
					("ed_finish")
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 32}}
				}
			}

			{"enemy_defense/ukr_light"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "enemy_defense_wave_cmd$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_defense_army$"} {op "=="} {value 2}}
						{"5.entities"
							{selector {tag enemy_def_ukr_line}}
							{count {op ">="} {value 3}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 11}}
					{"timer" {time 8} {title "ENEMY DEFENSE UKR LIGHT TEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_ukr_line}}}}
							{amount 3}
						}
						{tag_add enemy_def_deploy}
						{tag_remove enemy_def_ukr_line}
					}
					{"delay" {time 0.1}}
					("ed_place")
					("ed_finish")
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 12}}
				}
			}

			{"enemy_defense/ukr_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "enemy_defense_wave_cmd$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_defense_army$"} {op "=="} {value 2}}
						{"5.entities"
							{selector {tag enemy_def_ukr_line}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 21}}
					{"timer" {time 8} {title "ENEMY DEFENSE UKR LINE FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_ukr_line}}}}
							{amount 4}
						}
						{tag_add enemy_def_deploy}
						{tag_remove enemy_def_ukr_line}
					}
					{"delay" {time 0.1}}
					("ed_place")
					("ed_finish")
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 22}}
				}
			}

			{"enemy_defense/ukr_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "enemy_defense_wave_cmd$"} {op "=="} {value 3}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_defense_army$"} {op "=="} {value 2}}
						{"5.entities"
							{selector {tag enemy_def_ukr_wpn}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 31}}
					{"timer" {time 8} {title "ENEMY DEFENSE UKR WEAPONS FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_ukr_wpn}}}}
							{amount 4}
						}
						{tag_add enemy_def_deploy}
						{tag_remove enemy_def_ukr_wpn}
					}
					{"delay" {time 0.1}}
					("ed_place")
					("ed_finish")
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 32}}
				}
			}

			{"enemy_defense/prc_light"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "enemy_defense_wave_cmd$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_defense_army$"} {op "=="} {value 4}}
						{"5.entities"
							{selector {tag enemy_def_prc_line}}
							{count {op ">="} {value 3}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 11}}
					{"timer" {time 8} {title "ENEMY DEFENSE PRC LIGHT TEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_prc_line}}}}
							{amount 3}
						}
						{tag_add enemy_def_deploy}
						{tag_remove enemy_def_prc_line}
					}
					{"delay" {time 0.1}}
					("ed_place")
					("ed_finish")
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 12}}
				}
			}

			{"enemy_defense/prc_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "enemy_defense_wave_cmd$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_defense_army$"} {op "=="} {value 4}}
						{"5.entities"
							{selector {tag enemy_def_prc_line}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 21}}
					{"timer" {time 8} {title "ENEMY DEFENSE PRC LINE FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_prc_line}}}}
							{amount 4}
						}
						{tag_add enemy_def_deploy}
						{tag_remove enemy_def_prc_line}
					}
					{"delay" {time 0.1}}
					("ed_place")
					("ed_finish")
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 22}}
				}
			}

			{"enemy_defense/prc_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "enemy_defense_wave_cmd$"} {op "=="} {value 3}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_defense_army$"} {op "=="} {value 4}}
						{"5.entities"
							{selector {tag enemy_def_prc_wpn}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 31}}
					{"timer" {time 8} {title "ENEMY DEFENSE PRC WEAPONS FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_prc_wpn}}}}
							{amount 4}
						}
						{tag_add enemy_def_deploy}
						{tag_remove enemy_def_prc_wpn}
					}
					{"delay" {time 0.1}}
					("ed_place")
					("ed_finish")
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 32}}
				}
			}

			{"enemy_defense/nato_light"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "enemy_defense_wave_cmd$"} {op "=="} {value 1}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_defense_army$"} {op "=="} {value 3}}
						{"5.entities"
							{selector {tag enemy_def_nato_line}}
							{count {op ">="} {value 3}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 11}}
					{"timer" {time 8} {title "ENEMY DEFENSE NATO LIGHT TEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_nato_line}}}}
							{amount 3}
						}
						{tag_add enemy_def_deploy}
						{tag_remove enemy_def_nato_line}
					}
					{"delay" {time 0.1}}
					("ed_place")
					("ed_finish")
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 12}}
				}
			}

			{"enemy_defense/nato_line"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "enemy_defense_wave_cmd$"} {op "=="} {value 2}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_defense_army$"} {op "=="} {value 3}}
						{"5.entities"
							{selector {tag enemy_def_nato_line}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 21}}
					{"timer" {time 8} {title "ENEMY DEFENSE NATO LINE FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_nato_line}}}}
							{amount 4}
						}
						{tag_add enemy_def_deploy}
						{tag_remove enemy_def_nato_line}
					}
					{"delay" {time 0.1}}
					("ed_place")
					("ed_finish")
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 22}}
				}
			}

			{"enemy_defense/nato_wpn"
				{condition
					{expression "1 & 2 & 3 & 4 & 5"}
					{terms
						{"1.cmp_i" {var "user_is_defender$"} {op "=="} {value 0}}
						{"2.cmp_i" {var "enemy_defense_wave_cmd$"} {op "=="} {value 3}}
						{"3.cmp_i" {var "id_1st_enemy$"} {op ">"} {value 0}}
						{"4.cmp_i" {var "enemy_defense_army$"} {op "=="} {value 3}}
						{"5.entities"
							{selector {tag enemy_def_nato_wpn}}
							{count {op ">="} {value 4}}
						}
					}
				}
				{actions
					{"set_i" {var "enemy_defense_wave_cmd$"} {op "="} {value 0}}
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 31}}
					{"timer" {time 8} {title "ENEMY DEFENSE NATO WEAPONS FIRETEAM"}}
					{"entity_state"
						{selector
							{source advanced}
							{group {select {tag {tag enemy_def_nato_wpn}}}}
							{amount 4}
						}
						{tag_add enemy_def_deploy}
						{tag_remove enemy_def_nato_wpn}
					}
					{"delay" {time 0.1}}
					("ed_place")
					("ed_finish")
					{"set_i" {var "enemy_defense_stage$"} {op "="} {value 32}}
				}
			}
