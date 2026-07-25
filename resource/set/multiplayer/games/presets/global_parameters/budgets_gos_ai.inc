	{chargeFactors
		{value "*=0"}
		{customize "0;0.5;1.0;1.5;2.0;2.5;5.0;10.0"}
	}
	{budgets
		{vehicle
			{mode "standard"}
			{resource
				{start		1}
				{finish		1}
				{growTime	1} ;// in seconds
				{growTable	"1"} ;// each entry divides divides grow time: # of entries = growtime / #		
				{payback
					{cp			"0		10		20		30		40		51"}
					{restore	"0		0		0		0		0		0"}
				}
			}
			{buttons "inf1 inf2 empl vehicles tanks player division"}
		}
	}