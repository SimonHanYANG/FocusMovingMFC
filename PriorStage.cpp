#include "stdafx.h"
#include "PriorStage.h"

PriorStage::PriorStage()
{
}

bool PriorStage::InitPriorStage(long Port)
{
	AfxOleInit();  //Disable in VC 2008, Windows 7
	COleException e;
	CLSID clsid;
	LPUNKNOWN lpUnk;
	LPDISPATCH lpDispatch;
	//long Port = 2; //COM 2

	if (CLSIDFromProgID(OLESTR("prior.scan"), &clsid) != NOERROR)
	{
		return false;
	}

	if (GetActiveObject(clsid, NULL, &lpUnk) == NOERROR)
	{
		HRESULT hr = lpUnk->QueryInterface(IID_IDispatch, (LPVOID*)&lpDispatch);
		lpUnk->Release();
		if (hr == NOERROR)
			m_scan.AttachDispatch(lpDispatch, TRUE);
	}
	// if not dispatch ptr attached yet, need to create one
	if (m_scan.m_lpDispatch == NULL && !m_scan.CreateDispatch(clsid, &e))
	{
		return false;
	}

	if (CLSIDFromProgID(OLESTR("prior.stage"), &clsid) != NOERROR)
	{
		return false;
	}

	if (GetActiveObject(clsid, NULL, &lpUnk) == NOERROR)
	{
		HRESULT hr = lpUnk->QueryInterface(IID_IDispatch, (LPVOID*)&lpDispatch);
		lpUnk->Release();
		if (hr == NOERROR)
			m_stage.AttachDispatch(lpDispatch, TRUE);
	}
	// if not dispatch ptr attached yet, need to create one
	if (m_stage.m_lpDispatch == NULL && !m_stage.CreateDispatch(clsid, &e))
	{
		return false;
	}

	if (CLSIDFromProgID(OLESTR("prior.z"), &clsid) != NOERROR)
	{
		return false;
	}

	if (GetActiveObject(clsid, NULL, &lpUnk) == NOERROR)
	{
		HRESULT hr = lpUnk->QueryInterface(IID_IDispatch, (LPVOID*)&lpDispatch);
		lpUnk->Release();
		if (hr == NOERROR)
			m_zfocus.AttachDispatch(lpDispatch, TRUE);
	}
	// if not dispatch ptr attached yet, need to create one
	if (m_zfocus.m_lpDispatch == NULL && !m_zfocus.CreateDispatch(clsid, &e))
	{
		return false;
	}

	m_scan.Connect (&Port);
	if (Port < 0)
	{
		return false;
	}

	m_stage.SetMaxSpeed(m_default_stage_speed);
	m_stage.SetMaxAcceleration(9999);

	// Init prior stage default position
	//m_stage.SetPosition(0,0);

	// Init focusing values
	m_zfocus.SetMaxAcceleration(5000);	//6723
	m_zfocus.SetCurve(1000000);			//15034270
	m_zfocus.SetMaxSpeed(250);			//250
	m_zfocus.SetHostBackLashDistance(0);
	// come into if any of the "if" statements above do not fail
	return true;
}

void PriorStage::WaitForStageStop()
{
	long status;
	m_scan.IsMoving(&status);

	// extracts the Z axis movement only, wait for z focus to stop
	while ( (status&1) > 0 || (status&2) > 0 )
	{
		m_scan.IsMoving(&status);
	}
}

void PriorStage::WaitForFocusStop()
{
	long status;
	m_scan.IsMoving(&status);

	// extracts the Z axis movement only, wait for z focus to stop
	while ( (status&4) > 0)
	{
		m_scan.IsMoving(&status);
	}
}