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

    void SetLowGearState() { current_state_ = &low_gear_state_; }
    void SetMidGearState() { current_state_ = &mid_gear_state_; }
    void SetHighGearState() { current_state_ = &high_gear_state_; }

    bool CheckUpshiftConditions() { return UpshiftConditions(); }
    bool CheckDownshiftConditions() { return DownshiftConditions(); }
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

    void PerformAndCheckUpshiftRequest(bool is_requested) {
        // Upshift interrupt. When idle, assume toggle detected in case of noise
        EXPECT_CALL(*upshift_paddle_, ToggleDetected()).Times(1).WillOnce(Return(true));
        EXPECT_CALL(*upshift_paddle_, Read()).Times(1).WillOnce(Return(is_requested));
    }

    void PerformAndCheckDownshiftRequest(bool is_requested) {
        // Downshift interrupt. When idle, assume toggle detected in case of noise
        EXPECT_CALL(*downshift_paddle_, ToggleDetected()).Times(1).WillOnce(Return(true));
        EXPECT_CALL(*downshift_paddle_, Read()).Times(1).WillOnce(Return(is_requested));
    }

    void EngageNeutralMonitor(bool engage) {
        EXPECT_CALL(*neutral_monitor_, Read()).Times(1).WillOnce(Return(engage));
    }
};


//**************************************************************************
//				            Neutral State Cases
//**************************************************************************
TEST_F(ShiftControllerFixture, Neutral_DefaultState) {
    EXPECT_TRUE(shift_controller_.IsNeutralState());
}

TEST_F(ShiftControllerFixture, Neutral_ToLowGear) {
    // TODO: Update test case when UpshiftConditions() is fully implemented
    EXPECT_TRUE(shift_controller_.IsNeutralState());

    PerformAndCheckUpshiftRequest(true);
    EngageNeutralMonitor(true);
    
    shift_controller_.Run();
    EXPECT_TRUE(shift_controller_.IsLowGearState());
}

TEST_F(ShiftControllerFixture, Neutral_NoUpshiftRequest) {
    EXPECT_TRUE(shift_controller_.IsNeutralState());

    PerformAndCheckUpshiftRequest(false);

    // No request occurred, so neutral engagement is not checked
    EXPECT_CALL(*neutral_monitor_, Read()).Times(0);
    
    shift_controller_.Run();
    EXPECT_TRUE(shift_controller_.IsNeutralState());
}

TEST_F(ShiftControllerFixture, Neutral_IgnoreDownshiftRequest) {
    EXPECT_TRUE(shift_controller_.IsNeutralState());

    // User may possibly request a down shift, but the downshift_paddle
    // is ignored during Neutral state
    EXPECT_CALL(*downshift_paddle_, ToggleDetected()).Times(0);
    EXPECT_CALL(*downshift_paddle_, Read()).Times(0);
    
    shift_controller_.Run();
    EXPECT_TRUE(shift_controller_.IsNeutralState());
}



//**************************************************************************
//				            LowGear State Cases
//**************************************************************************
TEST_F(ShiftControllerFixture, LowGear_ToNeutral) {
    shift_controller_.SetLowGearState();
    EXPECT_TRUE(shift_controller_.IsLowGearState());

    PerformAndCheckDownshiftRequest(true);

    shift_controller_.Run();
    EXPECT_TRUE(shift_controller_.IsNeutralState());
}

TEST_F(ShiftControllerFixture, LowGear_ToMidGear) {
    shift_controller_.SetLowGearState();
    EXPECT_TRUE(shift_controller_.IsLowGearState());

    rpm_ = 5001;
    PerformAndCheckUpshiftRequest(true);

    shift_controller_.Run();
    EXPECT_TRUE(shift_controller_.IsMidGearState());
}


//**************************************************************************
//				            MidGear State Cases
//**************************************************************************
TEST_F(ShiftControllerFixture, MidGear_ToLowGear) 
{
    shift_controller_.SetMidGearState();
    EXPECT_TRUE(shift_controller_.IsMidGearState());

    rpm_ = 4999;
    PerformAndCheckDownshiftRequest(true);

    shift_controller_.Run();
    EXPECT_TRUE(shift_controller_.IsLowGearState());
}

TEST_F(ShiftControllerFixture, MidGear_ToHighGear)
{
    shift_controller_.SetMidGearState();
    EXPECT_TRUE(shift_controller_.IsMidGearState());

    rpm_ = 5001;
    PerformAndCheckUpshiftRequest(true);

    shift_controller_.Run();
    EXPECT_TRUE(shift_controller_.IsHighGearState());
}


//**************************************************************************
//				            HighGear State Cases
//**************************************************************************
TEST_F(ShiftControllerFixture, HighGear_IgnoreUpshiftRequest) {
    shift_controller_.SetHighGearState();
    EXPECT_TRUE(shift_controller_.IsHighGearState());

    // Ignore upshift paddle
    EXPECT_CALL(*upshift_paddle_, ToggleDetected()).Times(0);
    EXPECT_CALL(*upshift_paddle_, Read()).Times(0);

    shift_controller_.Run();
    EXPECT_TRUE(shift_controller_.IsHighGearState());
}

TEST_F(ShiftControllerFixture, HighGear_ToMidGear) {
    shift_controller_.SetHighGearState();
    EXPECT_TRUE(shift_controller_.IsHighGearState());

    rpm_ = 8999;
    PerformAndCheckDownshiftRequest(true);

    shift_controller_.Run();
    EXPECT_TRUE(shift_controller_.IsMidGearState());
}


//**************************************************************************
//				            Upshift Conditions
//**************************************************************************
TEST_F(ShiftControllerFixture, UpshiftConditions_Satisfied) {
    rpm_ = 5001;
    EXPECT_TRUE(shift_controller_.CheckUpshiftConditions());
}

TEST_F(ShiftControllerFixture, UpshiftConditions_GearAboveThreshold) {
    shift_controller_.SetHighGearState();
    EXPECT_FALSE(shift_controller_.CheckUpshiftConditions());
}

TEST_F(ShiftControllerFixture, UpshiftConditions_InNeutral) {
    shift_controller_.SetNeutralState();
    EXPECT_FALSE(shift_controller_.CheckUpshiftConditions());
}

TEST_F(ShiftControllerFixture, UpshiftConditions_RpmTooLow) {
    rpm_ = 5000;
    EXPECT_FALSE(shift_controller_.CheckUpshiftConditions());
}

TEST_F(ShiftControllerFixture, UpshiftConditions_WheelSpeedNotCloseEnough) {
    wheel_speeds_[0] = 10.0f;
    wheel_speeds_[1] = 30.0f;
    rpm_ = 5001;
    EXPECT_FALSE(shift_controller_.CheckUpshiftConditions());
}

//**************************************************************************
//				            Downshift Conditions
//**************************************************************************
TEST_F(ShiftControllerFixture, DownshiftConditions_Satisfied) {
    rpm_ = 8999;
    tps_ = 31.0f;
    EXPECT_TRUE(shift_controller_.CheckDownshiftConditions());
}

TEST_F(ShiftControllerFixture, DownshiftConditions_RpmTooHigh) {
    rpm_ = 9000;
    EXPECT_FALSE(shift_controller_.CheckDownshiftConditions());
}

TEST_F(ShiftControllerFixture, DownshiftConditions_WheelSpeedNotCloseEnough) {
    wheel_speeds_[0] = 10.0f;
    wheel_speeds_[1] = 30.0f;
    rpm_ = 8999;
    EXPECT_FALSE(shift_controller_.CheckDownshiftConditions());
}

TEST_F(ShiftControllerFixture, DownshiftConditions_TpsTooLow) {
    rpm_ = 8999;
    tps_ = 30.0f;
    EXPECT_FALSE(shift_controller_.CheckDownshiftConditions());
}

TEST_F(ShiftControllerFixture, DownshiftConditions_GearDisengaged) {
    shift_controller_.SetNeutralState();
    rpm_ = 8999;
    tps_ = 30.0f;
    EXPECT_FALSE(shift_controller_.CheckDownshiftConditions());
}


} // namespace application
