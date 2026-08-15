(define "codex_drop_plane_spawn"
	{on spawn
		{if not user_control
			{tags add "codex_drop_plane"}
			{tags add "ai_para_assigned"}
			{tags add "ai_para_assigned2"}
			{tags remove "wait_para_player_set"}
			{tags remove "plane_test_to_exit"}
			{tags remove "para_altitude_keeper"}
			{delay 1.2
				{tags remove "wait_para_player_set"}
				{tags remove "plane_test_to_exit"}
				{tags remove "para_altitude_keeper"}
			}
		}
	}
)

{"il-76td_para"
	("codex_drop_plane_spawn")
	{on "codex_force_drop"
		{if not user_control
			{tags remove "place1_busy"}
			{call "drop_paratrooper"}
			{delay 0.5
				{call "drop_desant1"}
			}
		}
	}
}

{"il-76td_para_ai"
	("codex_drop_plane_spawn")
	{on "codex_force_drop"
		{if not user_control
			{tags remove "place1_busy"}
			{call "drop_paratrooper"}
			{delay 0.5
				{call "drop_desant1"}
			}
		}
	}
}

{"c130_para"
	("codex_drop_plane_spawn")
	{on "codex_force_drop"
		{if not user_control
			{tags remove "place1_busy"}
			{call "drop_paratrooper"}
			{delay 0.5
				{call "drop_desant1"}
			}
		}
	}
}

{"c130_para_ai"
	("codex_drop_plane_spawn")
	{on "codex_force_drop"
		{if not user_control
			{tags remove "place1_busy"}
			{call "drop_paratrooper"}
			{delay 0.5
				{call "drop_desant1"}
			}
		}
	}
}

{"il-76td_bmd2"
	("codex_drop_plane_spawn")
	{on "codex_force_drop"
		{if not user_control
			{tags add "place1_busy"}
			{tags remove "dont_drop"}
			{tags remove "cargo_open"}
			{call "open_cargo"}
			{delay 1.5
				{ani_play "vehicle1_out" callback}
				{call "drop_desant1"}
			}
			{delay 3
				{call "drop_desant2"}
			}
		}
	}
}

{"c130_lav25"
	("codex_drop_plane_spawn")
	{on "codex_force_drop"
		{if not user_control
			{tags add "place1_busy"}
			{tags remove "dont_drop"}
			{tags remove "cargo_open"}
			{call "open_cargo"}
			{delay 1.5
				{ani_play "vehicle1_out" callback}
				{call "drop_desant1"}
			}
			{delay 3
				{call "drop_desant2"}
			}
		}
	}
}

{"codex_arty_105_16"
	{on spawn
		{if not tagged "fired"
			{tags add "fired"}
			{start_sound "weapon/shot/ambient_cannon"}
			(define "rnd_art_bomb"
				{delay %delay
					{start_sound "weapon/shot/mortar/whistle"}
					{delay 1
						{spawn "artillery_single_105" random_position 800}
					}
				}
			)
			("rnd_art_bomb" delay(1.5))
			("rnd_art_bomb" delay(3))
			("rnd_art_bomb" delay(4.5))
			("rnd_art_bomb" delay(6))
			("rnd_art_bomb" delay(7.5))
			("rnd_art_bomb" delay(9))
			("rnd_art_bomb" delay(10.5))
			("rnd_art_bomb" delay(12))
			("rnd_art_bomb" delay(13.5))
			("rnd_art_bomb" delay(15))
			("rnd_art_bomb" delay(16.5))
			("rnd_art_bomb" delay(18))
			("rnd_art_bomb" delay(19.5))
			("rnd_art_bomb" delay(21))
			("rnd_art_bomb" delay(22.5))
			("rnd_art_bomb" delay(24))
			{hide 30}
		}
	}
}

{"codex_arty_150_8"
	{on spawn
		{if not tagged "fired"
			{tags add "fired"}
			{start_sound "weapon/shot/ambient_cannon"}
			(define "rnd_art_bomb"
				{delay %delay
					{start_sound "weapon/shot/mortar/whistle"}
					{delay 1
						{spawn "artillery_single_150" random_position 800}
					}
				}
			)
			("rnd_art_bomb" delay(1.5))
			("rnd_art_bomb" delay(3))
			("rnd_art_bomb" delay(4.5))
			("rnd_art_bomb" delay(6))
			("rnd_art_bomb" delay(7.5))
			("rnd_art_bomb" delay(9))
			("rnd_art_bomb" delay(10.5))
			("rnd_art_bomb" delay(12))
			{hide 18}
		}
	}
}

{"codex_smoke_6"
	{on spawn
		{if not tagged "fired"
			{tags add "fired"}
			{start_sound "weapon/shot/ambient_cannon"}
			(define "rnd_smk"
				{delay %delay
					{start_sound "weapon/shot/mortar/whistle"}
					{delay 1
						{spawn "artillery_single_smoke" random_position 1600}
					}
				}
			)
			("rnd_smk" delay(1))
			("rnd_smk" delay(2))
			("rnd_smk" delay(3.5))
			("rnd_smk" delay(5))
			("rnd_smk" delay(6.5))
			("rnd_smk" delay(8))
			("rnd_smk" delay(9.5))
			("rnd_smk" delay(11))
			{hide 16}
		}
	}
}

{"codex_155_quiet"
	{on spawn
		{start_sound "weapon/shot/mortar/whistle"}
		{delay 6
			{start_sound "weapon/shot/artillery"}
			{spawn "artillery_single_150" offset -250 0 1000}
			{hide 1}
		}
	}
}

{"codex_arty_ambient"
	{on spawn
		{if not tagged "fired"
			{tags add "fired"}
			{delay 0.5
				{spawn "mortar_single_120_dummy" random_position 6500}
			}
			{delay 1.5
				{spawn "mortar_single_120_dummy" random_position 6500}
			}
			{delay 2.5
				{spawn "mortar_single_120_dummy" random_position 6500}
			}
			{delay 3.5
				{if rand 0.5
					{spawn "mortar_single_120_dummy" random_position 6500}
				}
			}
			{hide 12}
		}
	}
}

{"codex_arty_155_3"
	{on spawn
		{if not tagged "fired"
			{tags add "fired"}
			{delay 1
				{spawn "artillery_single_150" random_position 900}
			}
			{delay 15
				{spawn "artillery_single_150" random_position 900}
			}
			{delay 29
				{spawn "artillery_single_150" random_position 900}
			}
			{hide 36}
		}
	}
}

{"mortar_barrage_120"
	{on spawn
		{if not tagged "fired"
			{tags add "fired"}
			(define "rnd_art_bomb"
				{delay %delay 0.5
					{spawn "mortar_single_120_dummy" random_position 900}
				}
			)
			("rnd_art_bomb" delay(0.25))
			("rnd_art_bomb" delay(0.5))
			("rnd_art_bomb" delay(0.75))
			("rnd_art_bomb" delay(1))
			("rnd_art_bomb" delay(4.25))
			("rnd_art_bomb" delay(4.5))
			("rnd_art_bomb" delay(4.75))
			("rnd_art_bomb" delay(5))
			("rnd_art_bomb" delay(9.25))
			("rnd_art_bomb" delay(9.5))
			("rnd_art_bomb" delay(9.75))
			("rnd_art_bomb" delay(10))
			("rnd_art_bomb" delay(14.25))
			("rnd_art_bomb" delay(14.5))
			("rnd_art_bomb" delay(14.75))
			("rnd_art_bomb" delay(15))
			{hide 15}
		}
	}
}

{"mortar_barrage_160"
	{on spawn
		{if not tagged "fired"
			{tags add "fired"}
			(define "rnd_art_bomb"
				{delay %delay 0.5
					{spawn "mortar_single_160_dummy" random_position 900}
				}
			)
			("rnd_art_bomb" delay(0.25))
			("rnd_art_bomb" delay(0.5))
			("rnd_art_bomb" delay(0.75))
			("rnd_art_bomb" delay(1))
			("rnd_art_bomb" delay(4.25))
			("rnd_art_bomb" delay(4.5))
			("rnd_art_bomb" delay(4.75))
			("rnd_art_bomb" delay(5))
			("rnd_art_bomb" delay(9.25))
			("rnd_art_bomb" delay(9.5))
			("rnd_art_bomb" delay(9.75))
			("rnd_art_bomb" delay(10))
			("rnd_art_bomb" delay(14.25))
			("rnd_art_bomb" delay(14.5))
			("rnd_art_bomb" delay(14.75))
			("rnd_art_bomb" delay(15))
			{hide 15}
		}
	}
}
