// Machine generated IDispatch wrapper class(es) created with ClassWizard

#include "stdafx.h"
#include "prior.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// IStage properties

/////////////////////////////////////////////////////////////////////////////
// IStage operations

long IStage::GetJoystickEnable()
{
	long result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IStage::SetJoystickEnable(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

double IStage::GetCurve()
{
	double result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IStage::SetCurve(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double IStage::GetHostBackLashDistance()
{
	double result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IStage::SetHostBackLashDistance(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

long IStage::GetHostBackLashEnable()
{
	long result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IStage::SetHostBackLashEnable(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

double IStage::GetJoystickBackLashDistance()
{
	double result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IStage::SetJoystickBackLashDistance(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

long IStage::GetJoystickBacklashEnable()
{
	long result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IStage::SetJoystickBacklashEnable(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

double IStage::GetMaxAcceleration()
{
	double result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IStage::SetMaxAcceleration(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x7, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double IStage::GetMaximumJoystickSpeed()
{
	double result;
	InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IStage::SetMaximumJoystickSpeed(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x8, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double IStage::GetMaxSpeed()
{
	double result;
	InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IStage::SetMaxSpeed(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double IStage::GetRunningCurrent()
{
	double result;
	InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IStage::SetRunningCurrent(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0xa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double IStage::GetStandbyCurrent()
{
	double result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IStage::SetStandbyCurrent(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0xb, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

short IStage::GetXHostDirection()
{
	short result;
	InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
	return result;
}

void IStage::SetXHostDirection(short nNewValue)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0xc, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

short IStage::GetYHostDirection()
{
	short result;
	InvokeHelper(0xd, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
	return result;
}

void IStage::SetYHostDirection(short nNewValue)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0xd, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IStage::GetXJoystickDirection()
{
	long result;
	InvokeHelper(0xe, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IStage::SetXJoystickDirection(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xe, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IStage::GetYJoystickDirection()
{
	long result;
	InvokeHelper(0xf, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IStage::SetYJoystickDirection(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xf, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

double IStage::GetXPosition()
{
	double result;
	InvokeHelper(0x10, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IStage::SetXPosition(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x10, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double IStage::GetYPosition()
{
	double result;
	InvokeHelper(0x11, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IStage::SetYPosition(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x11, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

long IStage::GetParams(long* type, BSTR* Description, long* X_Size, long* Y_Size, long* uStepsPerMicron, long* OpenLimits)
{
	long result;
	static BYTE parms[] =
		VTS_PI4 VTS_PBSTR VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		type, Description, X_Size, Y_Size, uStepsPerMicron, OpenLimits);
	return result;
}

long IStage::SetParams(LPCTSTR Description, long X_Size, long Y_Size, long uStepsPerMicron, long OpenLimits)
{
	long result;
	static BYTE parms[] =
		VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_I4;
	InvokeHelper(0x13, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Description, X_Size, Y_Size, uStepsPerMicron, OpenLimits);
	return result;
}

long IStage::GetPosition(double* XPos, double* YPos)
{
	long result;
	static BYTE parms[] =
		VTS_PR8 VTS_PR8;
	InvokeHelper(0x14, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		XPos, YPos);
	return result;
}

long IStage::MoveAtVelocity(double XSpeed, double YSpeed)
{
	long result;
	static BYTE parms[] =
		VTS_R8 VTS_R8;
	InvokeHelper(0x15, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		XSpeed, YSpeed);
	return result;
}

long IStage::MoveBack(double Distance)
{
	long result;
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x16, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Distance);
	return result;
}

long IStage::MoveForward(double Distance)
{
	long result;
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x17, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Distance);
	return result;
}

long IStage::MoveLeft(double Distance)
{
	long result;
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x18, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Distance);
	return result;
}

long IStage::MoveRelative(double XPos, double YPos)
{
	long result;
	static BYTE parms[] =
		VTS_R8 VTS_R8; 
	InvokeHelper(0x19, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		XPos, YPos); 
	return result;
}

long IStage::MoveRight(double Distance)
{
	long result;
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x1a, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Distance);
	return result;
}

long IStage::MoveToAbsolute(double XPos, double YPos)
{
	long result;
	static BYTE parms[] =
		VTS_R8 VTS_R8;
	InvokeHelper(0x1b, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		XPos, YPos);
	return result;
}

long IStage::MoveXToAbsolute(double Position)
{
	long result;
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x1c, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Position);
	return result;
}

long IStage::MoveYToAbsolute(double Position)
{
	long result;
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x1d, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Position);
	return result;
}

long IStage::SetPosition(double XPos, double YPos)
{
	long result;
	static BYTE parms[] =
		VTS_R8 VTS_R8;
	InvokeHelper(0x1e, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		XPos, YPos);
	return result;
}

long IStage::SetCorrection(double a, double b, double c, double d)
{
	long result;
	static BYTE parms[] =
		VTS_R8 VTS_R8 VTS_R8 VTS_R8;
	InvokeHelper(0x1f, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		a, b, c, d);
	return result;
}

long IStage::GetCorrectionEnable()
{
	long result;
	InvokeHelper(0x20, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IStage::SetCorrectionEnable(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x20, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IStage::MoveXAtVelocityUntilEncoderReference(double Speed)
{
	long result;
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x21, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Speed);
	return result;
}

long IStage::MoveYAtVelocityUntilEncoderReference(double Speed)
{
	long result;
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x22, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Speed);
	return result;
}

long IStage::SetReference()
{
	long result;
	InvokeHelper(0x23, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

long IStage::RestoreReference()
{
	long result;
	InvokeHelper(0x24, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

double IStage::GetSkewAngle()
{
	double result;
	InvokeHelper(0x25, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IStage::SetSkewAngle(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x25, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

long IStage::GetSkewEnabled()
{
	long result;
	InvokeHelper(0x26, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IStage::SetSkewEnabled(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x26, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IStage::GetMappingEnabled()
{
	long result;
	InvokeHelper(0x27, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IStage::SetMappingEnabled(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x27, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IStage::SkewAbout()
{
	long result;
	InvokeHelper(0x28, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

long IStage::SkewSecond()
{
	long result;
	InvokeHelper(0x29, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IScan properties

/////////////////////////////////////////////////////////////////////////////
// IScan operations

long IScan::Connect(long* port)
{
	long result;
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		port);
	return result;
}

long IScan::IsMoving(long* Status)
{
	long result;
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Status);
	return result;
}

long IScan::MoveXYZtoAbsolute(double XPos, double YPos, double ZPos)
{
	long result;
	static BYTE parms[] =
		VTS_R8 VTS_R8 VTS_R8;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		XPos, YPos, ZPos);
	return result;
}

long IScan::MoveXYZRelative(double XPos, double YPos, double ZPos)
{
	long result;
	static BYTE parms[] =
		VTS_R8 VTS_R8 VTS_R8;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		XPos, YPos, ZPos);
	return result;
}

long IScan::KillAllMotion()
{
	long result;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

long IScan::InterruptAllMotion()
{
	long result;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

long IScan::GetPowerSupplyVoltage(long* Voltage)
{
	long result;
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Voltage);
	return result;
}

long IScan::GetBoardTemperature(float* Temperature)
{
	long result;
	static BYTE parms[] =
		VTS_PR4;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Temperature);
	return result;
}

long IScan::OpenShutter(long Shutter, long OpenNotClose)
{
	long result;
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Shutter, OpenNotClose);
	return result;
}

float IScan::GetVersion()
{
	float result;
	InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_R4, (void*)&result, NULL);
	return result;
}

short IScan::GetRevision()
{
	short result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
	return result;
}

short IScan::GetIsAtLimits()
{
	short result;
	InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
	return result;
}

long IScan::GetIsShutterOpen(long Shutter)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0xd, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, parms,
		Shutter);
	return result;
}

void IScan::SetIsShutterOpen(long Shutter, long nNewValue)
{
	static BYTE parms[] =
		VTS_I4 VTS_I4;
	InvokeHelper(0xd, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 Shutter, nNewValue);
}

long IScan::GetSerialNumber()
{
	long result;
	InvokeHelper(0xe, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

long IScan::GetFourthParams(long* type, BSTR* Description)
{
	long result;
	static BYTE parms[] =
		VTS_PI4 VTS_PBSTR;
	InvokeHelper(0xf, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		type, Description);
	return result;
}

long IScan::GetFocusParams(long* type, BSTR* Description)
{
	long result;
	static BYTE parms[] =
		VTS_PI4 VTS_PBSTR;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		type, Description);
	return result;
}

long IScan::QuerySystem(BSTR* Controller, BSTR* Stage, BSTR* Focus, BSTR* Fourth, BSTR* Filter1, BSTR* Filter2)
{
	long result;
	static BYTE parms[] =
		VTS_PBSTR VTS_PBSTR VTS_PBSTR VTS_PBSTR VTS_PBSTR VTS_PBSTR;
	InvokeHelper(0x11, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Controller, Stage, Focus, Fourth, Filter1, Filter2);
	return result;
}

long IScan::RestoreDefaults()
{
	long result;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

long IScan::GetShuttersPresent(long* Shutter_1, long* Shutter_2, long* Shutter_3)
{
	long result;
	static BYTE parms[] =
		VTS_PI4 VTS_PI4 VTS_PI4;
	InvokeHelper(0x13, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Shutter_1, Shutter_2, Shutter_3);
	return result;
}

long IScan::DisConnect()
{
	long result;
	InvokeHelper(0x15, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

long IScan::QueryEeprom(BSTR* content)
{
	long result;
	static BYTE parms[] =
		VTS_PBSTR;
	InvokeHelper(0x16, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		content);
	return result;
}

long IScan::SyncAndWait(long delay)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x17, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		delay);
	return result;
}

long IScan::GetLumenLightLevel()
{
	long result;
	InvokeHelper(0x18, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IScan::SetLumenLightLevel(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x18, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IScan::LumenLightReset()
{
	long result;
	InvokeHelper(0x19, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

long IScan::GetLumenPower()
{
	long result;
	InvokeHelper(0x1a, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IScan::SetLumenPower(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IScan::GetControllerFirmwareVersion()
{
	long result;
	InvokeHelper(0x1b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// IZ properties

/////////////////////////////////////////////////////////////////////////////
// IZ operations

double IZ::GetCurve()
{
	double result;
	InvokeHelper(0x1, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IZ::SetCurve(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x1, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double IZ::GetHostBackLashDistance()
{
	double result;
	InvokeHelper(0x2, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IZ::SetHostBackLashDistance(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x2, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

long IZ::GetHostBackLashEnable()
{
	long result;
	InvokeHelper(0x3, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IZ::SetHostBackLashEnable(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x3, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

short IZ::GetHostDirection()
{
	short result;
	InvokeHelper(0x4, DISPATCH_PROPERTYGET, VT_I2, (void*)&result, NULL);
	return result;
}

void IZ::SetHostDirection(short nNewValue)
{
	static BYTE parms[] =
		VTS_I2;
	InvokeHelper(0x4, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

double IZ::GetJoystickBackLashDistance()
{
	double result;
	InvokeHelper(0x5, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IZ::SetJoystickBackLashDistance(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x5, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

long IZ::GetJoystickBacklashEnable()
{
	long result;
	InvokeHelper(0x6, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IZ::SetJoystickBacklashEnable(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x6, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IZ::GetJoystickDirection()
{
	long result;
	InvokeHelper(0x7, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IZ::SetJoystickDirection(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x7, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

double IZ::GetMaxAcceleration()
{
	double result;
	InvokeHelper(0x8, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IZ::SetMaxAcceleration(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x8, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double IZ::GetMaximumJoystickSpeed()
{
	double result;
	InvokeHelper(0x9, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IZ::SetMaximumJoystickSpeed(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x9, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double IZ::GetMaxSpeed()
{
	double result;
	InvokeHelper(0xa, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IZ::SetMaxSpeed(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0xa, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double IZ::GetMicronsPerMotorRevolution()
{
	double result;
	InvokeHelper(0xb, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IZ::SetMicronsPerMotorRevolution(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0xb, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double IZ::GetPosition()
{
	double result;
	InvokeHelper(0xc, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IZ::SetPosition(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0xc, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double IZ::GetRunningCurrent()
{
	double result;
	InvokeHelper(0xd, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IZ::SetRunningCurrent(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0xd, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

double IZ::GetStandbyCurrent()
{
	double result;
	InvokeHelper(0xe, DISPATCH_PROPERTYGET, VT_R8, (void*)&result, NULL);
	return result;
}

void IZ::SetStandbyCurrent(double newValue)
{
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0xe, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 newValue);
}

long IZ::MoveAtVelocity(double ZSpeed)
{
	long result;
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0xf, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		ZSpeed);
	return result;
}

long IZ::MoveDown(double Distance)
{
	long result;
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Distance);
	return result;
}

long IZ::MoveUp(double Distance)
{
	long result;
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x11, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		Distance);
	return result;
}

long IZ::MoveToAbsolute(double ZPos)
{
	long result;
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		ZPos);
	return result;
}

long IZ::MoveAtVelocityUntilEncoderReference(double ZSpeed)
{
	long result;
	static BYTE parms[] =
		VTS_R8;
	InvokeHelper(0x13, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		ZSpeed);
	return result;
}

long IZ::SetReference()
{
	long result;
	InvokeHelper(0x14, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

long IZ::RestoreReference()
{
	long result;
	InvokeHelper(0x15, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return result;
}

long IZ::StartAutoFocus(long range)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x16, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		range);
	return result;
}

long IZ::SetActiveAutoFocusArea(long area)
{
	long result;
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x17, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		area);
	return result;
}

long IZ::SetUserActiveArea(long left, long top, long right, long bottom)
{
	long result;
	static BYTE parms[] =
		VTS_I4 VTS_I4 VTS_I4 VTS_I4;
	InvokeHelper(0x18, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		left, top, right, bottom);
	return result;
}

long IZ::StartUserDefinedAutoFocus(long delay, long number, long stepSize, long divisor)
{
	long result;
	static BYTE parms[] =
		VTS_I4 VTS_I4 VTS_I4 VTS_I4;
	InvokeHelper(0x19, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		delay, number, stepSize, divisor);
	return result;
}

long IZ::GetGreyInactiveArea()
{
	long result;
	InvokeHelper(0x1a, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IZ::SetGreyInactiveArea(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1a, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IZ::GetAutoFocusScore()
{
	long result;
	InvokeHelper(0x1b, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IZ::SetAutoFocusScore(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1b, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IZ::GetGreyIntensity()
{
	long result;
	InvokeHelper(0x1c, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IZ::SetGreyIntensity(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1c, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IZ::GetDefaultRangeForFireButton()
{
	long result;
	InvokeHelper(0x1d, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IZ::SetDefaultRangeForFireButton(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1d, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}

long IZ::GetCurrentFocusScore()
{
	long result;
	InvokeHelper(0x1e, DISPATCH_PROPERTYGET, VT_I4, (void*)&result, NULL);
	return result;
}

void IZ::SetCurrentFocusScore(long nNewValue)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x1e, DISPATCH_PROPERTYPUT, VT_EMPTY, NULL, parms,
		 nNewValue);
}
