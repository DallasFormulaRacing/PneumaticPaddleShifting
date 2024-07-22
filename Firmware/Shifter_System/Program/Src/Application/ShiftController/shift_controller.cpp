
#include "shift_controller.hpp"

namespace application {

int ShiftController::getCurrentGear(){
    float gearR[6] = {2.833, 2.062, 1.647, 1.421, 1.272, 1.173};
    float wheelRPM = (DrivenWheelSpeed() * 60)/ 1.7083;
    float gearRatio = Rpm/wheelRPM;
    int foundGear = -1;
    switch(gearRatio){
    case gearR[0]: foundGear = 1;
    break;
    case gearR[1]: foundGear = 2;
    break;
    case gearR[2]: foundGear = 3;
    break;
    case gearR[3]: foundGear = 4;
    break;
    case gearR[4]: foundGear = 5;
    break; 
    case gearR[5]: foundGear = 6;
    break;
    }
    if(foundGear != -1)
        return foundGear;
        
        float prevDiff = 10000;
        //int prevGear = 0;
        for(int i = 0; i < 6; i++){
            if(prevDiff > (abs(gearR[i] - gearRatio)){
                foundGearGear = gearR[i] + 1;
                prevDiff = abs(gearR[i] - gearRatio)
            }
            
        }
    
    return foundGear;
    
    
}


ShiftController::ShiftController() { }

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
