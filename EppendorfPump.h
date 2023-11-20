#pragma once

#include "phidget22.h"
#pragma comment (lib, "phidget22.lib")


class EppendorfPump
{
public:
	EppendorfPump(void);
	~EppendorfPump(void);

public:

	bool Initialize(void);

	int Stop(void);

	void PowerOn(void);

	void PowerOff(void);

	int GetPowerState(void);

	int MoveRelative(__int64 steps);

	int MovetoAbsolute(__int64 position);

	int MoveAtVelocity(double velocity);

	__int64 GetCurrentPosition(void);

	void SetVelocity(double limit);

	double GetCurrentVelocity(void);

	bool IsMoving(void);

	bool bIsPumpAttached;

	PhidgetReturnCode res;

private:

	//the motor control handle
	PhidgetStepperHandle stepper;

	//__int64 current_position;
	double current_position;
};



//callback functions for attaching the stepper
void EppendorfPumpCallEventAttachHandler(PhidgetHandle stepper, void *userptr);

