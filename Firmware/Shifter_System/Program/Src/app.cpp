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

#include <array>

// ST HAL Dependencies
#include "gpio.h"

#include "usart.h"
extern UART_HandleTypeDef huart2;

#include "can.h"
extern CAN_HandleTypeDef hcan1;


// DFR Custom Dependencies
#include "app.hpp"
#include "../../Core/Inc/retarget.h"
#include "Application/ShiftController/shift_controller.hpp"
#include "./Platform/STM/F4/CAN/bxcan_stmf4.hpp"
#include "./Platform/STM/F4/GPIO/gpio_stmf4.hpp"
#include "./Platform/Interfaces/ican.hpp"
#include "./Platform/Interfaces/igpio.hpp"
#include "./Sensor/ECU/PE3/iecu.hpp"
#include "./Sensor/ECU/PE3/pe3.hpp"


/**************************************************************
 * 				CAN Bus Interrupt Callback
 **************************************************************/
std::shared_ptr<platform::BxCanStmF4> bx_can_callback_ptr(nullptr);

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
	bx_can_callback_ptr->ReceiveCallback();
}



using ReceiveInterruptMode = platform::BxCanStmF4::ReceiveInterruptMode;



void cppMain() {
	// Enable `printf()` using USART
	RetargetInit(&huart2);


	auto bx_can_peripheral = std::make_shared<platform::BxCanStmF4>(hcan1);
	std::shared_ptr<platform::ICan> can_bus = bx_can_peripheral;

	sensor::Pe3 pe3_ecu(can_bus);
	const std::vector<uint32_t>& can_id_list = pe3_ecu.CanIdList();

	// Subscribe to messages with PE3's CAN IDs
	for (const uint32_t& can_id : can_id_list) {
		bx_can_peripheral->ConfigureFilter((can_id >> 13), (can_id & 0x1FFF));
	}

	bx_can_peripheral->Start();

	// Configure and enable CAN message arrival interrupts
	bx_can_callback_ptr = bx_can_peripheral;
	ReceiveInterruptMode rx_interrupt_mode = ReceiveInterruptMode::kFifo0MessagePending;
	bx_can_peripheral->ConfigureReceiveCallback(rx_interrupt_mode);
	bx_can_peripheral->EnableInterruptMode();

	int16_t rpm = 0;
	float tps = 0.0f;
	std::array<float, 2> wheel_speeds {0.0f, 0.0f};

	// TODO: initialize GPIO with proper pins and setup interrupt callback
	auto neutral_monitor = std::make_shared<platform::GpioStmF4>(GPIOF, GPIO_PIN_15);
	auto upshift_paddle = std::make_shared<platform::GpioStmF4>(GPIOF, GPIO_PIN_15);
	auto downshift_paddle = std::make_shared<platform::GpioStmF4>(GPIOF, GPIO_PIN_15);
	auto neutral_button = std::make_shared<platform::GpioStmF4>(GPIOF, GPIO_PIN_15);

	application::ShiftController shift_controller(
		rpm,
		tps,
		wheel_speeds,
		neutral_monitor,
		upshift_paddle,
		downshift_paddle,
		neutral_button
	);

	for(;;){

		// Check for updated CAN messages
		if (pe3_ecu.NewMessageArrived()) {
			can_bus->DisableInterruptMode();

			pe3_ecu.Update();
			uint32_t can_id = pe3_ecu.LatestCanId();


			switch(can_id) {
			case FramePe1Id:
				printf("[ECU] PE1 arrived\n");
				rpm = pe3_ecu.Rpm();
				tps = pe3_ecu.Tps();
				break;

			case FramePe13Id:
				printf("[ECU] PE13 arrived\n");
				wheel_speeds[0] = pe3_ecu.DrivenWheelSpeed(0);
				wheel_speeds[1] = pe3_ecu.DrivenWheelSpeed(1);
				break;
			}


			can_bus->EnableInterruptMode();
		}

		shift_controller.Run();

	}
}

