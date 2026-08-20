; EDITOR ONLY Old Boy captive-sequence diagnostic. Do not include from
; ce_triggers.inc / dcg_script.inc. Civilian-mirror path stays parked.
; No CWA-map / production surrender changes.
;
; Corrected Old Boy POW path (not player 0 + control AI):
;   1. {effect start_white_flag}
;   2. {"player"} {operation set} {player "0"}
;   3. {tag_remove enemy}
;   4. {"inventory"} {action drop} of the weapon in hands
;   5. {"animation"} {collage stand_giveup_1}
;
; P1->P0 alone already PASS on d7fa808. This file is the next bounded test.
; No {control AI}, able neutral, morale/POW/camp tags, fight/select, or clone.
;
; Setup: include this file in the trigger block. Place one Player-1
; mp/nato/2022s/nato_rifleman, tag aio_pow_ob_src. After 2s the sequence runs.
; Deliberately shoot the actor. Stop if AV. No Conquest.
			{"editor_pow_oldboy/captive"
				{condition
					{terms
						{"1.entities"
							{selector
								{source advanced}
								{group
									{select
										{tag
											{tag aio_pow_ob_src}
										}
									}
									{include
										{prop
											{prop human}
										}
									}
									{exclude
										{state
											{state dead}
										}
										{state
											{state inactive}
										}
										{tag
											{tag aio_pow_ob_done}
										}
										{tag
											{tag hidden}
										}
									}
								}
							}
						}
					}
				}
				{actions
					{"delay"
						{time 2}
					}
					{"effect"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_ob_src}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{tag
										{tag aio_pow_ob_done}
									}
								}
							}
						}
						{effect start_white_flag}
					}
					{"delay"
						{time 0.05}
					}
					{"player"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_ob_src}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{tag
										{tag aio_pow_ob_done}
									}
								}
							}
						}
						{operation set}
						{player "0"}
					}
					{"delay"
						{time 0.05}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_ob_src}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{tag
										{tag aio_pow_ob_done}
									}
								}
							}
						}
						{tag_remove enemy}
					}
					{"delay"
						{time 0.05}
					}
					{"inventory"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_ob_src}
									}
								}
								{include
									{prop
										{prop human}
									}
									{with_item
										{type using}
										{item "weapon"}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{tag
										{tag aio_pow_ob_done}
									}
								}
							}
						}
						{action drop}
					}
					{"delay"
						{time 0.05}
					}
					{"animation"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_ob_src}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
									{state
										{state inactive}
									}
									{tag
										{tag aio_pow_ob_done}
									}
								}
							}
						}
						{collage stand_giveup_1}
					}
					{"entity_state"
						{selector
							{source advanced}
							{group
								{select
									{tag
										{tag aio_pow_ob_src}
									}
								}
								{include
									{prop
										{prop human}
									}
								}
								{exclude
									{state
										{state dead}
									}
								}
							}
						}
						{tag_add aio_pow_ob_done}
					}
				}
			}
