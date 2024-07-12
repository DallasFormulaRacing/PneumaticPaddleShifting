// Unit Testing Framework
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// DFR Custom Dependencies
#include "../../../Shifter_System/Program/Src/Application/ShiftController/shift_controller.hpp"

class MockGpio : public platform::IGpio {
public:
    MOCK_METHOD(bool, Read, (), (override));
    MOCK_METHOD(void, Write, (bool signal), (override));
    MOCK_METHOD(bool, ToggleDetected, (), (override));
};


namespace application {

class ShiftControllerWrapper : public ShiftController {
public:
    ShiftControllerWrapper(int16_t &rpm,
                          std::array<float, 2> &wheel_speeds,
                          std::shared_ptr<platform::IGpio> neutral_switch)
      : ShiftController(rpm,
                        wheel_speeds,
                        neutral_switch) { }

    bool IsLowGearState() { return dynamic_cast<LowGear*>(current_state_) != nullptr; }
    bool IsNeutralState() { return dynamic_cast<Neutral*>(current_state_) != nullptr; }
    bool IsMidGearState() { return dynamic_cast<MidGear*>(current_state_) != nullptr; }
    bool IsHighGearState() { return dynamic_cast<HighGear*>(current_state_) != nullptr; }
};


class ShiftControllerFixture : public testing::Test {
protected:
    int16_t rpm;
    std::array<float, 2> wheel_speeds_;
    std::shared_ptr<MockGpio> neutral_switch_ = std::make_shared<MockGpio>();
    ShiftControllerWrapper shift_controller_{rpm, wheel_speeds_, neutral_switch_};
};

TEST_F(ShiftControllerFixture, DefaultToNeutralState) {
    EXPECT_TRUE(shift_controller_.IsNeutralState());
}

TEST_F(ShiftControllerFixture, TransitionToLowGearState) {
    EXPECT_TRUE(shift_controller_.IsNeutralState());

    EXPECT_CALL(*neutral_switch_, Read())
        .Times(1)
        .WillOnce(::testing::Return(true));
    
    shift_controller_.Run();
    EXPECT_TRUE(shift_controller_.IsLowGearState());
}

TEST_F(ShiftControllerFixture, RemainNeutralState) {
    EXPECT_TRUE(shift_controller_.IsNeutralState());

    EXPECT_CALL(*neutral_switch_, Read())
        .Times(1)
        .WillOnce(::testing::Return(false));
    
    shift_controller_.Run();
    EXPECT_TRUE(shift_controller_.IsNeutralState());
}

} // namespace application
