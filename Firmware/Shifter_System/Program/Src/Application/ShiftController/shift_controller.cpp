
#include "shift_controller.hpp"

namespace application {


ShiftController::ShiftController(int16_t &rpm_observer,
								 float &tps_observer,
								 std::array<float, 2> &wheel_speeds_observer,
								 std::shared_ptr<platform::IGpio> neutral_monitor,
								 std::shared_ptr<platform::IGpio> upshift_paddle,
								 std::shared_ptr<platform::IGpio> downshift_paddle,
								 std::shared_ptr<platform::IGpio> neutral_button)
  : rpm_(rpm_observer),
	tps_(tps_observer),
	wheel_speeds_(wheel_speeds_observer),
	neutral_monitor_(neutral_monitor),
	upshift_paddle_(upshift_paddle),
	downshift_paddle_(downshift_paddle),
	neutral_button_(neutral_button) {}

ShiftController::~ShiftController() { }

void ShiftController::SetState(State* new_state) {
	if (new_state != nullptr) {
		current_state_->Exit(*this);
		current_state_ = new_state;
		current_state_->Enter(*this);
	}
}

void ShiftController::Run() {
	current_state_->Compute(*this);
}

bool ShiftController::NeutralEngaged() {
	return neutral_monitor_->Read();
}

bool ShiftController::UpshiftRequested() {
	return upshift_paddle_->ToggleDetected() &&
		   upshift_paddle_->Read();
}

bool ShiftController::UpshiftConditions() {
	// TODO: Gear below 6th
	uint8_t gear_num = 0;

	// TODO: wheel speed close to engine speed
	bool wheel_speed_close_enough = true;


	return (gear_num < 6) &&			// Gear below 6th
			!NeutralEngaged() &&		// Car not in neutral
			(rpm_ > 5000)  &&			// Engine speed above 5000
			wheel_speed_close_enough;	// Wheel speed close to engine speed for current gear
}


bool ShiftController::DownshiftRequested() {
	return upshift_paddle_->ToggleDetected() &&
		   upshift_paddle_->Read();
}

bool ShiftController::DownshiftConditions() {
	// TODO: wheel speed close to engine speed
	bool wheel_speed_close_enough = true;

	// TODO: Check for gear engagement
	bool gear_enaged = true;

	return (rpm_ < 9000) &&						// Engine speed below 9000
			wheel_speed_close_enough &&			// Wheel speed corelates with engine speed
		   (tps_ > 30.0f) &&	// Throttle posision sensor above 30%
			gear_enaged;						// Check for gear engagement 
}


//*************************************************
//					LowGear State
//*************************************************
void ShiftController::LowGear::Enter(ShiftController& context) {
	printf("[ShiftController] Entering LowGear state.\n");
}

void ShiftController::LowGear::Compute(ShiftController& context) {

}

void ShiftController::LowGear::Exit(ShiftController& context) {
	printf("[ShiftController] Leaving LowGear state.\n");
}


//*************************************************
//					Neutral State
//*************************************************
void ShiftController::Neutral::Enter(ShiftController& context) {
	printf("[ShiftController] Entering Neutral state.\n");
}

void ShiftController::Neutral::Compute(ShiftController& context) {
	if (context.UpshiftRequested() && context.UpshiftConditions()) {
		context.SetState(&context.low_gear_state_);
	}
}

void ShiftController::Neutral::Exit(ShiftController& context) {
	printf("[ShiftController] Leaving Neutral state.\n");
}


//*************************************************
//					MidGear State
//*************************************************
void ShiftController::MidGear::Enter(ShiftController& context) {
	printf("[ShiftController] Entering MidGear state.\n");
}

void ShiftController::MidGear::Compute(ShiftController& context) {

}

void ShiftController::MidGear::Exit(ShiftController& context) {
	printf("[ShiftController] Leaving MidGear state.\n");
}


//*************************************************
//					HighGear State
//*************************************************
void ShiftController::HighGear::Enter(ShiftController& context) {
	printf("[ShiftController] Entering HighGear state.\n");
}

void ShiftController::HighGear::Compute(ShiftController& context) {

}

void ShiftController::HighGear::Exit(ShiftController& context) {
	printf("[ShiftController] Leaving HighGear state.\n");
}

} // namespace application
