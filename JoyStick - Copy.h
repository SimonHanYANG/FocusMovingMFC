//////////////////////////////////////////////////////////////////////

// JoyStick Class

// Defines the functionality fo the 3d mouse wheel,
// Enables the 3d mouse wheel to control the micromanipulator

#ifndef _JOYSTICK_HPP
#define _JOYSTICK_HPP

#include <iostream>
#include "si.h"
#include "SiskiyouAmnl.h"

const float max_value = 350.0;				// maximum rotation value for each axis
const int wait_counts = 10;					// wait counts to prevent multiple threads from being created
const float weight = 0.1;					// smoothing weight

class JoyStick
{
public:
	JoyStick();
	int SbInit(HWND *Wnd);
	void SbMotionEvent(SiSpwEvent *pEvent, SiskiyouAmnl* manip);		// handles mouse motion events (e.g., left, right, rotate...)
	void SbZeroEvent();												// handles idle events										
	void SbButtonPressEvent(int buttonnumber);						// handes button press events
	void SbButtonReleaseEvent(int buttonnumber);					// handles button release events
	void SbPreTranslate(MSG *msg, SiskiyouAmnl* manip);					// goes into the preTranslate method of MainFrame

protected:
	SiHdl m_DevHdl;       /* Handle to 3D mouse Device */
	bool m_was_moving;
	float m_dx, m_dy, m_dz;
	float m_dx_raw, m_dy_raw, m_dz_raw;
	int m_move_counter;				// keeps track of the consecutive number of frames move events are triggered above a movement threshold value

	SiskiyouAmnl* m_local_manip;		// pointer to the manipulator object
	void Move();
	static DWORD WINAPI MoveThread(void* pParameter);

	bool m_thread_created;

	int m_min_speed_1;
	int m_min_speed_2;
	int m_max_speed_1;
	int m_max_speed_2;

};
#endif