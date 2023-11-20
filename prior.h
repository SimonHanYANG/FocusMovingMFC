// Machine generated IDispatch wrapper class(es) created with ClassWizard
/////////////////////////////////////////////////////////////////////////////
// IStage wrapper class

class IStage : public COleDispatchDriver
{
public:
	IStage() {}		// Calls COleDispatchDriver default constructor
	IStage(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IStage(const IStage& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long GetJoystickEnable();
	void SetJoystickEnable(long nNewValue);
	double GetCurve();
	void SetCurve(double newValue);
	double GetHostBackLashDistance();
	void SetHostBackLashDistance(double newValue);
	long GetHostBackLashEnable();
	void SetHostBackLashEnable(long nNewValue);
	double GetJoystickBackLashDistance();
	void SetJoystickBackLashDistance(double newValue);
	long GetJoystickBacklashEnable();
	void SetJoystickBacklashEnable(long nNewValue);
	double GetMaxAcceleration();
	void SetMaxAcceleration(double newValue);
	double GetMaximumJoystickSpeed();
	void SetMaximumJoystickSpeed(double newValue);
	double GetMaxSpeed();
	void SetMaxSpeed(double newValue);
	double GetRunningCurrent();
	void SetRunningCurrent(double newValue);
	double GetStandbyCurrent();
	void SetStandbyCurrent(double newValue);
	short GetXHostDirection();
	void SetXHostDirection(short nNewValue);
	short GetYHostDirection();
	void SetYHostDirection(short nNewValue);
	long GetXJoystickDirection();
	void SetXJoystickDirection(long nNewValue);
	long GetYJoystickDirection();
	void SetYJoystickDirection(long nNewValue);
	double GetXPosition();
	void SetXPosition(double newValue);
	double GetYPosition();
	void SetYPosition(double newValue);
	long GetParams(long* type, BSTR* Description, long* X_Size, long* Y_Size, long* uStepsPerMicron, long* OpenLimits);
	long SetParams(LPCTSTR Description, long X_Size, long Y_Size, long uStepsPerMicron, long OpenLimits);
	long GetPosition(double* XPos, double* YPos);
	long MoveAtVelocity(double XSpeed, double YSpeed);
	long MoveBack(double Distance);
	long MoveForward(double Distance);
	long MoveLeft(double Distance);
	long MoveRelative(double XPos, double YPos);
	long MoveRight(double Distance);
	long MoveToAbsolute(double XPos, double YPos);
	long MoveXToAbsolute(double Position);
	long MoveYToAbsolute(double Position);
	long SetPosition(double XPos, double YPos);
	long SetCorrection(double a, double b, double c, double d);
	long GetCorrectionEnable();
	void SetCorrectionEnable(long nNewValue);
	long MoveXAtVelocityUntilEncoderReference(double Speed);
	long MoveYAtVelocityUntilEncoderReference(double Speed);
	long SetReference();
	long RestoreReference();
	double GetSkewAngle();
	void SetSkewAngle(double newValue);
	long GetSkewEnabled();
	void SetSkewEnabled(long nNewValue);
	long GetMappingEnabled();
	void SetMappingEnabled(long nNewValue);
	long SkewAbout();
	long SkewSecond();
};
/////////////////////////////////////////////////////////////////////////////
// IScan wrapper class

class IScan : public COleDispatchDriver
{
public:
	IScan() {}		// Calls COleDispatchDriver default constructor
	IScan(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IScan(const IScan& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	long Connect(long* port);
	long IsMoving(long* Status);
	long MoveXYZtoAbsolute(double XPos, double YPos, double ZPos);
	long MoveXYZRelative(double XPos, double YPos, double ZPos);
	long KillAllMotion();
	long InterruptAllMotion();
	long GetPowerSupplyVoltage(long* Voltage);
	long GetBoardTemperature(float* Temperature);
	long OpenShutter(long Shutter, long OpenNotClose);
	float GetVersion();
	short GetRevision();
	short GetIsAtLimits();
	long GetIsShutterOpen(long Shutter);
	void SetIsShutterOpen(long Shutter, long nNewValue);
	long GetSerialNumber();
	long GetFourthParams(long* type, BSTR* Description);
	long GetFocusParams(long* type, BSTR* Description);
	long QuerySystem(BSTR* Controller, BSTR* Stage, BSTR* Focus, BSTR* Fourth, BSTR* Filter1, BSTR* Filter2);
	long RestoreDefaults();
	long GetShuttersPresent(long* Shutter_1, long* Shutter_2, long* Shutter_3);
	long DisConnect();
	long QueryEeprom(BSTR* content);
	long SyncAndWait(long delay);
	long GetLumenLightLevel();
	void SetLumenLightLevel(long nNewValue);
	long LumenLightReset();
	long GetLumenPower();
	void SetLumenPower(long nNewValue);
	long GetControllerFirmwareVersion();
};
/////////////////////////////////////////////////////////////////////////////
// IZ wrapper class

class IZ : public COleDispatchDriver
{
public:
	IZ() {}		// Calls COleDispatchDriver default constructor
	IZ(LPDISPATCH pDispatch) : COleDispatchDriver(pDispatch) {}
	IZ(const IZ& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

// Attributes
public:

// Operations
public:
	double GetCurve();
	void SetCurve(double newValue);
	double GetHostBackLashDistance();
	void SetHostBackLashDistance(double newValue);
	long GetHostBackLashEnable();
	void SetHostBackLashEnable(long nNewValue);
	short GetHostDirection();
	void SetHostDirection(short nNewValue);
	double GetJoystickBackLashDistance();
	void SetJoystickBackLashDistance(double newValue);
	long GetJoystickBacklashEnable();
	void SetJoystickBacklashEnable(long nNewValue);
	long GetJoystickDirection();
	void SetJoystickDirection(long nNewValue);
	double GetMaxAcceleration();
	void SetMaxAcceleration(double newValue);
	double GetMaximumJoystickSpeed();
	void SetMaximumJoystickSpeed(double newValue);
	double GetMaxSpeed();
	void SetMaxSpeed(double newValue);
	double GetMicronsPerMotorRevolution();
	void SetMicronsPerMotorRevolution(double newValue);
	double GetPosition();
	void SetPosition(double newValue);
	double GetRunningCurrent();
	void SetRunningCurrent(double newValue);
	double GetStandbyCurrent();
	void SetStandbyCurrent(double newValue);
	long MoveAtVelocity(double ZSpeed);
	long MoveDown(double Distance);
	long MoveUp(double Distance);
	long MoveToAbsolute(double ZPos);
	long MoveAtVelocityUntilEncoderReference(double ZSpeed);
	long SetReference();
	long RestoreReference();
	long StartAutoFocus(long range);
	long SetActiveAutoFocusArea(long area);
	long SetUserActiveArea(long left, long top, long right, long bottom);
	long StartUserDefinedAutoFocus(long delay, long number, long stepSize, long divisor);
	long GetGreyInactiveArea();
	void SetGreyInactiveArea(long nNewValue);
	long GetAutoFocusScore();
	void SetAutoFocusScore(long nNewValue);
	long GetGreyIntensity();
	void SetGreyIntensity(long nNewValue);
	long GetDefaultRangeForFireButton();
	void SetDefaultRangeForFireButton(long nNewValue);
	long GetCurrentFocusScore();
	void SetCurrentFocusScore(long nNewValue);
};
