
// RoboticBiopsy.h : main header file for the RoboticBiopsy application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols

#include <io.h>

// CRoboticBiopsyApp:
// See RoboticBiopsy.cpp for the implementation of this class
//

class CRoboticBiopsyApp : public CWinApp
{
public:
	CRoboticBiopsyApp();


// Overrides
public:
	virtual BOOL InitInstance();

	//***************Create OnIdle(Start)***********
    virtual BOOL OnIdle(LONG lCount);
    //***************Create  OnIdle (End)***********
// Implementation
	afx_msg void OnAppAbout();


	DECLARE_MESSAGE_MAP()
//	virtual int ExitInstance();
};

extern CRoboticBiopsyApp theApp;
