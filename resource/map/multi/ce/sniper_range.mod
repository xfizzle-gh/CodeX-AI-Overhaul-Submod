{Modifiers
	{modifier
		{name aio_sniper_range}
		{filter
			{include
				{tag
					{tag aio_sniper}
				}
			}
			{exclude
				{state
					{state dead}
				}
				{state
					{state inactive}
				}
			}
		}
		{parameters
			{aim_range
				{place "*"}
				{scale 1.25}
			}
		}
	}
}
