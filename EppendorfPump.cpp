#include "stdAfx.h"
#include "EppendorfPump.h"
#include <iostream>


EppendorfPump::EppendorfPump(void)
{
	stepper = 0;
	current_position = 0;
}

EppendorfPump::~EppendorfPump(void)
{
	bIsPumpAttached = false;
	PhidgetStepper_setEngaged(stepper, PFALSE);
	Phidget_close((PhidgetHandle)stepper);
	//Phidget_delete((PhidgetHandle*)stepper);
	PhidgetHandle handle = (PhidgetHandle)stepper;
	Phidget_delete(&handle);
}

bool EppendorfPump::Initialize()
{
	//create the stepper object
	PhidgetStepper_create(&stepper);

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	Phidget_setOnAttachHandler((PhidgetHandle)stepper, EppendorfPumpCallEventAttachHandler, this);

	//open the device for connections
	Phidget_open((PhidgetHandle)stepper);
	Sleep(100);
	PhidgetStepper_setCurrentLimit(stepper, 0.65);	//Set max current 0.5A
	Sleep(100);
	PhidgetStepper_setVelocityLimit(stepper, 10000.0);	//Velocity Limit is motor's target velocity. Set default moving velocity to be 50000 steps/second. 
	Sleep(100);
	PhidgetStepper_setAcceleration(stepper, 100000.0);	//Set acceleration to be 1000000 step*s^-2.
	Sleep(100);
	//PhidgetStepper_setCurrentPosition(stepper, 0);	//Set current motor reference position to be 0.
	current_position = 0;
	//PhidgetStepper_setEngaged(stepper, 1);
	Sleep(100);
	MoveAtVelocity(0.0);
	return bIsPumpAttached;
}

void EppendorfPump::SetVelocity(double limit)
{
	res=PhidgetStepper_setVelocityLimit(stepper, limit);
	/*if (res == EPHIDGET_OK) {
		printf("successfully set velocity\n");
	}*/
	
	Sleep(100);
}


int EppendorfPump::MoveRelative(__int64 steps)
{
	//CPhidgetStepper_getCurrentPosition(stepper, 0, &current_position);
	//CPhidgetStepper_setVelocityLimit(stepper, 0, 50000.0);

	PhidgetStepper_setTargetPosition(stepper, current_position + steps);
	Sleep(100);
	return 1;
}


bool EppendorfPump::IsMoving()
{
	// 	int moving = -1;
	// 	CPhidgetStepper_getStopped(stepper, 0, &moving);
	// 	moving = 1-moving;
	// 	std::cout<<"Pump is Moving? "<<moving<<std::endl;

	double velocity = 0.0;
	PhidgetStepper_getVelocity(stepper,  &velocity);
	//	std::cout<<"velocity: "<<velocity<<std::endl;
	bool moving;
	if (velocity == 0)
		moving = false;
	else
		moving = true;
	return moving;
}

int EppendorfPump::MovetoAbsolute(__int64 position)
{
	//CPhidgetStepper_setVelocityLimit(stepper, 0, 50000.0);
	//PhidgetStepper_setTargetPosition(stepper, position);
	res = PhidgetStepper_setTargetPosition(stepper, position);
	if (res == EPHIDGET_OK) {
		//printf("ok\n");
	}
	if (res != EPHIDGET_OK) {
		const char *errMsg7;
		fprintf(stderr, "Failed to set target position: %s\n", Phidget_getErrorDescription(res, &errMsg7));
		return 1;
	}

	return 1;
}

int EppendorfPump::MoveAtVelocity(double velocity)
{
	__int64 ultimate_position = (velocity > 0.) ? 1000000000000000 : -1000000000000000;

	double abs_velocity = (velocity > 0.) ? velocity : (-velocity);

	PhidgetStepper_setVelocityLimit(stepper,  abs_velocity);

	PhidgetStepper_setTargetPosition(stepper,  ultimate_position);

	return 1;
}


__int64 EppendorfPump::GetCurrentPosition()
{
	PhidgetStepper_getPosition(stepper, &current_position);

	return current_position;
}

double EppendorfPump::GetCurrentVelocity(void)
{
	double velocity;
	PhidgetStepper_getVelocity(stepper, &velocity);
	return velocity;
}


int EppendorfPump::Stop()
{
	//MoveRelative(0);
	MoveAtVelocity(0.);

	return 1;
}


void EppendorfPump::PowerOn()
{
	PhidgetStepper_setEngaged(stepper,PTRUE);
}

void EppendorfPump::PowerOff()
{
	PhidgetStepper_setEngaged(stepper,  PFALSE);
}

int EppendorfPump::GetPowerState()
{
	int engaged = 0;
	PhidgetStepper_getEngaged(stepper,&engaged);
	return engaged;
}


//The motor attach event call back funciton
void EppendorfPumpCallEventAttachHandler(PhidgetHandle phid, void *ctx)
{
	//AfxMessageBox(_T("Motor controller attached!"));
	std::cout << "Motor controller 1067 is attached!" << std::endl;

	EppendorfPump* This = (EppendorfPump*)ctx;

	This->bIsPumpAttached = true;

	//return 0;
}

