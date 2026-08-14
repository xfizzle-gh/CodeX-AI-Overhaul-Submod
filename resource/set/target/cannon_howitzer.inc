{selector
	{type "cannon_howitzer"}

	{group
		{ignore {class airborne}}
		{ignore {class proj_rus_antirad}}
		{ignore {class proj_usa_antirad}}
		{select {class howitzer} {in_range 1500}}
		{sort {class howitzer} {in_range 1500}}
		{sort {class cannon_big} {in_range 1500}}
		{sort {class cannon_small} {in_range 1500}}
		{sort {class cannon_zenite} {in_range 1500}}
		{sort distance}
	}
	{group
		{ignore {class airborne}}
		{select {class tank_heavy} {in_range 1500}}
		{select {class tank_medium} {in_range 1500}}
		{select {class tank_light} {in_range 1500}}
		{sort {threating}}
		{sort distance}
	}
	{group
		{ignore {class airborne}}
		{select {class armoredcar} {in_range 1500}}
		{select {class armoredcar_l} {in_range 1500}}
		{select {class car} {in_range 1500}}
		{sort {attacking}}
		{sort distance}
	}
	{group
		{ignore {class airborne}}
		{select {class human} {in_range 1500}}
		{sort {class human} {in_range 1500}}
		{sort distance}
	}
	{group
		{sort {attacking}}
		{sort distance}
	}
}
