//////////////////////////////////////////////////////////////////////

// PriorStage Class

// Wrapper class for controlling the prior stage (X-Y) and the focus (Z)
// Please see Prior Scientific SDK documentation for more info 

#pragma once

#include <iostream>
#include "Prior.h"

static const int m_default_stage_speed = 2000;

class PriorStage
{
public:
	PriorStage();
	bool InitPriorStage(long Port);								// serial communication set up for the prior stage
	void WaitForStageStop();							// Holds the thread until the x-y stage stops moving
	void WaitForFocusStop();							// Holds the thread until the z focus stops moving
	
	// accessor methods
	IScan &Scan(){return m_scan;};
	IStage &Stage(){return m_stage;};
	IZ &Focus(){return m_zfocus;};

protected:
	IScan m_scan;										// For getting the status of X-Y-Z stage and focus
	IStage m_stage;										// X-Y stage movement
	IZ m_zfocus;										// Z focus movement

};