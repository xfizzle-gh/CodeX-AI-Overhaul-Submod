{easy
	{unitset
		{value %period}
	}
	{balancingFactorBonus
		{cp 0.80}
		{mpStart 1.10}
		{mpFinish 0.8}
		{payback 0.8}
		{chargeDelay 1.0}	;// adjusts the initial delay only
		{charge 1.0}		;// adjusts the interval only
	}
	{unitScore 1.2}
}
{normal
	{unitset
		{value %period}
	}
	{balancingFactorBonus
		{cp 0.90}
		{mpStart 1.10}
		{mpFinish 0.90}
		{payback 0.90}
		{chargeDelay 1.0}	;// adjusts the initial delay only
		{charge 1.0}		;// adjusts the interval only
	}
	{unitScore 1.0}
}
{hard
	{unitset
		{value %period}
	}
	{balancingFactorBonus
		{cp 1.00}
		{mpStart 1.10}
		{mpFinish 1.25}
		{payback 3.50}
		{chargeDelay 1.0}	;// adjusts the initial delay only
		{charge 1.0}		;// adjusts the interval only
	}
	{unitScore 0.8}
}
{heroic
	{unitset
		{value %period}
	}
	{balancingFactorBonus
		{cp 1.10}
		{mpStart 1.10}
		{mpFinish 1.75}
		{payback 4.00}
		{chargeDelay 1.0}	;// adjusts the initial delay only
		{charge 1.0}		;// adjusts the interval only
	}
	{unitScore 0.5}
}
{extreme
	{unitset
		{value %period}
	}
	{balancingFactorBonus
		{cp 1.25}
		{mpStart 1.30}
		{mpFinish 2.0}
		{payback 5.00}
		{chargeDelay 1.0}	;// adjusts the initial delay only
		{charge 0.75}		;// adjusts the interval only
	}
	{unitScore 0.33}
}
{cruel
	{unitset
		{value %period}
	}
	{balancingFactorBonus
		{cp 1.5}
		{mpStart 1.50}
		{mpFinish 2.5}
		{payback 7.50}
		{chargeDelay 0.8}	;// adjusts the initial delay only
		{charge 0.5}		;// adjusts the interval only
	}
	{unitScore 0.25}
}
