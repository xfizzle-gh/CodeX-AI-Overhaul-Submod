{"human"
	{on spawn
		{add_view	"marker_personage"			"marker_personage"			"head" not_manual not_scenario}
		{add_view	"marker_personage_acute"	"marker_personage_acute"	"head" not_manual not_scenario}
	}

	{on "start_personage"
		{view pause "marker_repair"}
		{view pause "marker_personage_acute"}
		{view start ally neutral "marker_personage"}
		{tags add "start_personage"}
		{tags add "bleeding"}
		{delay 2.5
			{if rand 0.5
				{ani_play "lie_wounded" loop}
				{spawn "bloodstain" "basis" x}		;// spawns a pool of blood that spreads
			else rand 0.5
				{ani_play "lie_wounded_1" loop}
				{spawn "bloodstain" "basis" x}		;// spawns a pool of blood that spreads
			else rand 0.5
				{ani_play "lie_wounded_2" loop}
				{spawn "bloodstain" "basis" x}		;// spawns a pool of blood that spreads
			}
		}
	}

	{on "stop_personage"
		{set "die_sound" 0}
		{view pause "marker_personage"}
		{view pause "marker_personage_acute"}
		{tags remove "bleeding"}
		{ani_stop "lie_wounded"}
		{ani_stop "lie_wounded_1"}
		{ani_stop "lie_wounded_2"}
		{if tagged "start_personage"
			{delay 1
				{if dead
					{spawn "bloodstain" "basis" x}		;// spawns a pool of blood that spreads
					{tags add "_bleedout_"}
				}
			}
		}
		{tags remove "start_personage"}
	}
}
