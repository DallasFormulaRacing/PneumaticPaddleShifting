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

#include "usart.h"
extern UART_HandleTypeDef huart2;


// DFR Custom Dependencies
#include "../../Core/Inc/retarget.h"


void cppMain() {
	// Enable `printf()` using USART
	RetargetInit(&huart2);


	for(;;){
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		printf("hi\n");
		HAL_Delay(1000);
	}
}

