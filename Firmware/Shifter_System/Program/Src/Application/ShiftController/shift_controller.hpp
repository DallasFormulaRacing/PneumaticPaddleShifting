
#ifndef SHIFT_CONTROLLER_H
#define SHIFT_CONTROLLER_H

// Standard Libraries
#include <cstdio>
#include <cinttypes>

#include <array>
#include <memory>


// Custom DFR Libraries
#include "../../Platform/Interfaces/igpio.hpp"


namespace application {

class ShiftController {
protected:
	class State {
	public:
		virtual ~State() = default;

		virtual void Enter(ShiftController& context) = 0;
		virtual void Compute(ShiftController& context) = 0;
		virtual void Exit(ShiftController& context) = 0;

	protected:
		State() = default;
	};

	class LowGear : public State {
	public:
		LowGear() = default;
		virtual void Enter(ShiftController& context) override;
		virtual void Compute(ShiftController& context) override;
		virtual void Exit(ShiftController& context) override;
	};

	class Neutral : public State {
	public:
		Neutral() = default;

		virtual void Enter(ShiftController& context) override;
		virtual void Compute(ShiftController& context) override;
		virtual void Exit(ShiftController& context) override;
	};

	class MidGear : public State {
	public:
		MidGear() = default;

		virtual void Enter(ShiftController& context) override;
		virtual void Compute(ShiftController& context) override;
		virtual void Exit(ShiftController& context) override;
	};

	class HighGear : public State {
	public:
		HighGear() = default;

		virtual void Enter(ShiftController& context) override;
		virtual void Compute(ShiftController& context) override;
		virtual void Exit(ShiftController& context) override;
	};

	bool UpshiftConditions();
	bool DownshiftConditions();

	State* current_state_{&neutral_state_};
	LowGear low_gear_state_;
	Neutral neutral_state_;
	MidGear mid_gear_state_;
	HighGear high_gear_state_;

	

public:
	ShiftController(int16_t &rpm_observer,
					float &tps_observer,
					std::array<float, 2> &wheel_speeds_observer,
					std::shared_ptr<platform::IGpio> neutral_monitor,
					std::shared_ptr<platform::IGpio> upshift_paddle,
					std::shared_ptr<platform::IGpio> downshift_paddle,
					std::shared_ptr<platform::IGpio> neutral_button);

	~ShiftController();

	void Run();

	State* GetState() { return current_state_; }
	
	
private:
	void SetState(State* new_state);
	bool NeutralEngaged();
	bool UpshiftRequested();
	bool DownshiftRequested();
	

	
	int16_t& rpm_;
	float& tps_;
	std::array<float, 2>& wheel_speeds_;
	std::shared_ptr<platform::IGpio> neutral_monitor_;
	
	// Steering wheel
	std::shared_ptr<platform::IGpio> upshift_paddle_;
	std::shared_ptr<platform::IGpio> downshift_paddle_;
	std::shared_ptr<platform::IGpio> neutral_button_;
};

} // namespace application

#endif // SHIFT_CONTROLLER_H
