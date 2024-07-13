// Unit Testing Framework
#include <gtest/gtest.h>
#include <gmock/gmock.h>

// DFR Custom Dependencies
#include "../../../Shifter_System/Program/Src/Application/ShiftController/shift_controller.hpp"

using ::testing::Return;

class MockGpio : public platform::IGpio {
public:
    MOCK_METHOD(bool, Read, (), (override));
    MOCK_METHOD(void, Write, (bool signal), (override));
    MOCK_METHOD(bool, ToggleDetected, (), (override));
};


namespace application {

class ShiftControllerWrapper : public ShiftController {
public:
    ShiftControllerWrapper(int16_t &rpm_observer,
                           float &tps_observer,
                           std::array<float, 2> &wheel_speeds,
                           std::shared_ptr<platform::IGpio> neutral_monitor,
                           std::shared_ptr<platform::IGpio> upshift_paddle,
                           std::shared_ptr<platform::IGpio> downshift_paddle,
                           std::shared_ptr<platform::IGpio> neutral_button)
      : ShiftController(rpm_observer,
                        tps_observer,
                        wheel_speeds,
                        neutral_monitor,
                        upshift_paddle,
                        downshift_paddle,
                        neutral_button) { }

    bool IsLowGearState() { return dynamic_cast<LowGear*>(current_state_) != nullptr; }
    bool IsNeutralState() { return dynamic_cast<Neutral*>(current_state_) != nullptr; }
    bool IsMidGearState() { return dynamic_cast<MidGear*>(current_state_) != nullptr; }
    bool IsHighGearState() { return dynamic_cast<HighGear*>(current_state_) != nullptr; }
};


class ShiftControllerFixture : public testing::Test {
protected:
    int16_t rpm_ = 0;
    float tps_ = 0.0f;
    std::array<float, 2> wheel_speeds_ {0.0f, 0.0f};
    std::shared_ptr<MockGpio> neutral_monitor_ = std::make_shared<MockGpio>();

    // Steering wheel
    std::shared_ptr<MockGpio> upshift_paddle_ = std::make_shared<MockGpio>();
    std::shared_ptr<MockGpio> downshift_paddle_ = std::make_shared<MockGpio>();
    std::shared_ptr<MockGpio> neutral_button_ = std::make_shared<MockGpio>();

    ShiftControllerWrapper shift_controller_ {
        rpm_,
        tps_,
        wheel_speeds_,
        neutral_monitor_,
        upshift_paddle_,
        downshift_paddle_,
        neutral_button_
    };

    void PerformUpshiftRequest(bool is_requested) {
        // Upshift interrupt. When idle, assume toggle detected in case of noise
        EXPECT_CALL(*upshift_paddle_, ToggleDetected()).Times(1).WillOnce(Return(true));
        EXPECT_CALL(*upshift_paddle_, Read()).Times(1).WillOnce(Return(is_requested));
    }

    void EngageNeutralMonitor(bool engage) {
        EXPECT_CALL(*neutral_monitor_, Read())
            .Times(1)
            .WillOnce(Return(engage));
    }
};


//**************************************************************************
//				            Neutral State Cases
//**************************************************************************
TEST_F(ShiftControllerFixture, Neutral_DefaultState) {
    EXPECT_TRUE(shift_controller_.IsNeutralState());
}

TEST_F(ShiftControllerFixture, Neutral_ToLowGear) {
    EXPECT_TRUE(shift_controller_.IsNeutralState());

    rpm_ = 5001;
    PerformUpshiftRequest(true);
    
    // Car not in neutral
    EXPECT_CALL(*neutral_monitor_, Read())
        .Times(1)
        .WillOnce(Return(false));
    
    shift_controller_.Run();
    EXPECT_TRUE(shift_controller_.IsLowGearState());
}

TEST_F(ShiftControllerFixture, Neutral_NoUpshiftRequest) {
    EXPECT_TRUE(shift_controller_.IsNeutralState());

    PerformUpshiftRequest(false);

    // No request occurred, so upshift conditions are not checked
    EXPECT_CALL(*neutral_monitor_, Read()).Times(0);
    
    shift_controller_.Run();
    EXPECT_TRUE(shift_controller_.IsNeutralState());
}

} // namespace application
