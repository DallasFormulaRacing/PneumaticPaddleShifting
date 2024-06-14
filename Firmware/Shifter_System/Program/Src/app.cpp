/*
* Pneumatic Paddle Shifting
* (c) 2024 Dallas Formula Racing
* Formula SAE International Collegiate Chapter
* GPL-3.0 License
*/

#include "app.hpp"

// Standard Libraries
#include <cstdio>
#include <cinttypes>

// ST HAL Dependencies
#include "gpio.h"

// 3rd Party Libraryes and Frameworks


// DFR Custom Dependencies



void cppMain() {



	for(;;){
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		HAL_Delay(1000);
	}
}

