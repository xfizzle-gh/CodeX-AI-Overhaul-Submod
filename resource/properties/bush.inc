game_entity
	(include "seasons.inc")
	{volume "crown"
		{able clear {visible 1} {blast 1} {overlap 1}}
	}	
	{volume "crown1"
		{able clear {visible 1} {blast 1} {overlap 1}}
	}

	{collider "bush"}
	{obstacleID "nothing"}
	{props "tree" "bush" "wind_has_foliage" "remove_on_land_patch"}
	{volume defaultmaterial "wood"}
	{lodclass "decor_bush"}
	{NoGroupMesh 1}