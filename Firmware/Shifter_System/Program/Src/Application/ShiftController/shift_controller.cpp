
#include "shift_controller.hpp"

namespace application {


ShiftController::ShiftController(int16_t &rpm_observer,
				std::array<float, 2> &wheel_speeds_observer,
				std::shared_ptr<platform::IGpio> neutral_switch_observer)
  : rpm_(rpm_observer),
	wheel_speeds_(wheel_speeds_observer),
	neutral_switch_(neutral_switch_observer) {}

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
	if(context.neutral_switch_->Read()) {
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
