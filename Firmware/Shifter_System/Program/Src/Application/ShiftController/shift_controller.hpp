
#ifndef SHIFT_CONTROLLER_H
#define SHIFT_CONTROLLER_H

// Standard Libraries
#include <cstdio>

namespace application {

class ShiftController {
public:

	ShiftController();

	~ShiftController();

	void Run();


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

private:
	void SetState(State* new_state);


	LowGear low_gear_state_;
	Neutral neutral_state_;
	MidGear mid_gear_state_;
	HighGear high_gear_state_;
	State* current_state_{&neutral_state_};
};

} // namespace application

#endif // SHIFT_CONTROLLER_H
