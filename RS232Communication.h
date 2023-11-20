//////////////////////////////////////////////////////////////////////

// RS232Communciation Class

// Provide connection, read, and write methods for serial communication
// Uses the Seriallibrary, for more info on the Serial library,
// please see: http://www.codeproject.com/Articles/992/Serial-library-for-C

#ifndef _RS232_COMMUNICATION_HPP
#define _RS232_COMMUNICATION_HPP

//#define STRICT
#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "Serial.h"
#include <iostream>
#include <sstream>

class RS232Communication
{
public:
	RS232Communication();
	bool Connect(char* com_port,
				 CSerial::EBaudrate eBaudrate = CSerial::EBaud9600,
				 CSerial::EDataBits eDataBits = CSerial::EData8,
				 CSerial::EParity   eParity   = CSerial::EParNone,
				 CSerial::EStopBits eStopBits = CSerial::EStop1);

	bool ReadReply(int num_char);									// reads reply from harder, checks the number of bytes is equal to num_char

	int& TimeOut(){return m_time_out;};

	void Close();

	void Purge();

	std::string IntToStr(int number);

protected:
	bool m_is_connected;
	bool m_port_is_busy;
	bool m_is_read_success;
	LONG m_last_error;
	CSerial m_serial;

	char m_reply[40];
	int m_total_reply_size;

	int m_time_out;
};
#endif