// Unit Testing Framework
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// DFR Custom Dependencies
#include "../../../Shifter_System/Program/Src/Application/ShiftController/shift_controller.hpp"

namespace application {

class ShiftControllerWrapper : public ShiftController {
public:

    int16_t rpm_;
	std::array<float, 2> wheel_speeds_;

    ShiftControllerWrapper()
      : ShiftController(rpm_,
                        wheel_speeds_) { }

    bool IsLowGearState() { return dynamic_cast<LowGear*>(current_state_) != nullptr; }
    bool IsNeutralState() { return dynamic_cast<Neutral*>(current_state_) != nullptr; }
    bool IsMidGearState() { return dynamic_cast<MidGear*>(current_state_) != nullptr; }
    bool IsHighGearState() { return dynamic_cast<HighGear*>(current_state_) != nullptr; }
};


class ShiftControllerFixture : public testing::Test {
protected:
    ShiftControllerWrapper shift_controller_;
};

TEST_F(ShiftControllerFixture, DefaultToNeutralState) {
    EXPECT_TRUE(shift_controller_.IsNeutralState());
}

} // namespace application
