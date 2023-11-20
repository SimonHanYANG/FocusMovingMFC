//////////////////////////////////////////////////////////////////////

// Sutter Manipulator Class

// Controls the Sutter micromanipulator via
// serial communication. For a full list of serial

#ifndef _SUTTER_HPP
#define _SUTTER_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
#include "RS232Communication.h"

typedef struct sManipPosition
{
	sManipPosition()
	{
		x = 0;
		y = 0;
		z = 0;
	}
	float x, y, z;
}sManipPosition;

class SutterManip
{
public:
	SutterManip();
	bool is_moving;
	float m_x, m_y, m_z;
	HANDLE hCom;
public:
	void Initialize(CString com_index);
	sManipPosition GetPosition();
	void SetVelocity(unsigned short v);
	bool CheckStatus();
	void Move(float xValue, float yValue, float zValue);

protected:

};

#endif;