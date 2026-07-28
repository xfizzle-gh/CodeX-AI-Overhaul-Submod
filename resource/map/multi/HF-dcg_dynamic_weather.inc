			{"environment_dynamic/summer"
				{condition
				}
				{actions
					{"delay"
						{time 3}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.3}
							}
							{"set_i"
								{var "dynamic_weather_case$"}
								{op "="}
								{value 1}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.3}
							}
							{"set_i"
								{var "dynamic_weather_case$"}
								{op "="}
								{value 2}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.3}
							}
							{"set_i"
								{var "dynamic_weather_case$"}
								{op "="}
								{value 3}
							}
						}
						{"default"
							{"set_i"
								{var "dynamic_weather_case$"}
								{op "="}
								{value 4}
							}
						}
					}
					{"delay"
						{time 10}
					}
					{"event"
						{event "dynamic_weather_failsafe"}
						{mode set}
						{delay 0}
					}
				}
			}

			{"environment_dynamic/summer/case 1"
				{condition
					{terms
						{"1.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 1}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/01_dawn"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/02_morning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/03_crispmorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/04_earlyhazymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/05_latehazymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/06_dustymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/07_foggymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/08_palemorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/morning_rain_1"}
								{time 1}
								{apply}
							}
						}
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/10_coldmorning"}
								{time 1}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/11_chillyday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/13_mistyday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/14_paleday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/15_sunnyday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/16_vibrantday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/17_coldday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/18_realistic_natural"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/19_realistic_filmic"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/19b_realistic_filmic_saturated"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/20_cloudy"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/21_overcast"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/22_lightrain"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/23_palerain"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/26_slush"}
								{time 60}
								{apply}
							}
						}
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/12_clearday"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/sunset_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/sunset_rain_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/33_evening"}
								{time 60}
								{apply}
							}
						}
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/34_dusk"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/35_moonlight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/37_foggynight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/single/02-ger2/1944_08_falaise/dawn"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/night_2"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_2"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_3"}
								{time 60}
								{apply}
							}
						}
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/38_midnight"}
								{time 60}
								{apply}
							}
						}
					}
				}
			}
			{"environment_dynamic/summer/case 2"
				{condition
					{terms
						{"1.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 2}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/11_chillyday"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/13_mistyday"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/14_paleday"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/15_sunnyday"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/16_vibrantday"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/17_coldday"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/18_realistic_natural"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/19_realistic_filmic"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/19b_realistic_filmic_saturated"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/20_cloudy"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/21_overcast"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/22_lightrain"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/23_palerain"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/26_slush"}
								{time 1}
								{apply}
							}
						}
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/12_clearday"}
								{time 1}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/sunset_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/sunset_rain_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/33_evening"}
								{time 60}
								{apply}
							}
						}
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/34_dusk"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/35_moonlight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/37_foggynight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/single/02-ger2/1944_08_falaise/dawn"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/night_2"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_2"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_3"}
								{time 60}
								{apply}
							}
						}
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/38_midnight"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 1200}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/01_dawn"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/02_morning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/03_crispmorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/04_earlyhazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/05_latehazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/06_dustymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/07_foggymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/08_palemorning"}
								{time 60}
								{apply}
							}
						}
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/10_coldmorning"}
								{time 60}
								{apply}
							}
						}
					}
				}
			}
			{"environment_dynamic/summer/case 3"
				{condition
					{terms
						{"1.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 3}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"switch" ;//evening
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/sunset_1"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/sunset_rain_1"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/33_evening"}
								{time 1}
								{apply}
							}
						}				
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/34_dusk"}
								{time 1}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//night
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/35_moonlight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/37_foggynight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/single/02-ger2/1944_08_falaise/dawn"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/night_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_2"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_3"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/38_midnight"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 1200}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//morning
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/01_dawn"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/02_morning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/03_crispmorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/04_earlyhazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/05_latehazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/06_dustymorning"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/07_foggymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/08_palemorning"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/10_coldmorning"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//day
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/11_chillyday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/13_mistyday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/14_paleday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/15_sunnyday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/16_vibrantday"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/17_coldday"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/18_realistic_natural"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/19_realistic_filmic"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/19b_realistic_filmic_saturated"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/20_cloudy"}
								{time 60}
								{apply}
							}
						}			
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/21_overcast"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/22_lightrain"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/23_palerain"}
								{time 60}
								{apply}
							}
						}							
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/26_slush"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/12_clearday"}
								{time 60}
								{apply}
							}
						}
					}
				}
			}
			{"environment_dynamic/summer/case 4"
				{condition
					{terms
						{"1.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 4}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"switch" ;//night
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/35_moonlight"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/37_foggynight"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/single/02-ger2/1944_08_falaise/dawn"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/night_1"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/night_2"}
								{time 1}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_1"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_2"}
								{time 1}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_3"}
								{time 1}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/38_midnight"}
								{time 1}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 1200}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//morning
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/01_dawn"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/02_morning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/03_crispmorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/04_earlyhazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/05_latehazymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/06_dustymorning"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/07_foggymorning"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/08_palemorning"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/10_coldmorning"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//day
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/11_chillyday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/13_mistyday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/14_paleday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/15_sunnyday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/16_vibrantday"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/17_coldday"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/18_realistic_natural"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/19_realistic_filmic"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/19b_realistic_filmic_saturated"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/20_cloudy"}
								{time 60}
								{apply}
							}
						}			
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/21_overcast"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/22_lightrain"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/23_palerain"}
								{time 60}
								{apply}
							}
						}							
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/26_slush"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/12_clearday"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//evening
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/sunset_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/sunset_rain_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/33_evening"}
								{time 60}
								{apply}
							}
						}				
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/34_dusk"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch" ;//night
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/35_moonlight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/37_foggynight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/single/02-ger2/1944_08_falaise/dawn"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/night_2"}
								{time 60}
								{apply}
							}
						}		
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_2"}
								{time 60}
								{apply}
							}
						}	
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_3"}
								{time 60}
								{apply}
							}
						}							
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/38_midnight"}
								{time 60}
								{apply}
							}
						}
					}
				}
			}

			{"environment_dynamic/summer/case failsafe"
				{condition
					{expression "!1 & !2 & !3 & !4 & 5"}
					{terms
						{"1.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 1}
						}
						{"2.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 2}
						}
						{"3.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 3}
						}
						{"4.cmp_i"
							{var "dynamic_weather_case$"}
							{op "=="}
							{value 4}
						}
						{"5.event"
							{id "dynamic_weather_failsafe"}
						}
					}
				}
				{actions
					{"delay"
						{time 1}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/01_dawn"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/02_morning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/03_crispmorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/04_earlyhazymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/05_latehazymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/06_dustymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/07_foggymorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/08_palemorning"}
								{time 1}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/morning_rain_1"}
								{time 1}
								{apply}
							}
						}
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/10_coldmorning"}
								{time 1}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/11_chillyday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/13_mistyday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/14_paleday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/15_sunnyday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/16_vibrantday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/17_coldday"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/18_realistic_natural"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/19_realistic_filmic"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/19b_realistic_filmic_saturated"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/20_cloudy"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/21_overcast"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/22_lightrain"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/23_palerain"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/26_slush"}
								{time 60}
								{apply}
							}
						}
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/12_clearday"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/sunset_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/sunset_rain_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/33_evening"}
								{time 60}
								{apply}
							}
						}
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/34_dusk"}
								{time 60}
								{apply}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 900}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"delay"
								{time 450}
							}
						}
						{"default"
							{"delay"
								{time 750}
							}
						}
					}
					{"switch"
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/35_moonlight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/custom/37_foggynight"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/single/02-ger2/1944_08_falaise/dawn"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/summer/night_2"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_1"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_2"}
								{time 60}
								{apply}
							}
						}
						{"case"
							{condition
								{type rand}
								{value 0.5}
							}
							{"environment"
								{name "goh/west/wip/autumn/night_3"}
								{time 60}
								{apply}
							}
						}
						{"default"
							{"delay"
								{time 1}
							}
							{"environment"
								{name "goh/custom/38_midnight"}
								{time 60}
								{apply}
							}
						}
					}
				}
			}