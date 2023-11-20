#include "stdafx.h"
#include "RS232Communication.h"

RS232Communication::RS232Communication()
{
	m_is_connected = false;
	m_port_is_busy = false;
	m_is_read_success = false;
	
	// in milliseconds
	m_time_out = 200;

	m_last_error = ERROR_SUCCESS;
}

// HardwareCommInterface
bool RS232Communication::Connect(char* com_port, 
								 CSerial::EBaudrate eBaudrate, 
								 CSerial::EDataBits eDataBits,
								 CSerial::EParity   eParity,
								 CSerial::EStopBits eStopBits)
{
	 // Attempt to open the serial port (COM1)
    m_last_error = m_serial.Open(_T(com_port),0,0,true);

	if (m_last_error != ERROR_SUCCESS)
	{
		m_is_connected = false;
		std::cout << "Unable to open COM-port" << std::endl;
		//return ::ShowError(serial.GetLastError(), _T("Unable to open COM-port"));
	}
	else
	{

		// Setup the serial port (9600,8N1, which is the default setting)
		m_last_error = m_serial.Setup(eBaudrate,eDataBits,eParity,eStopBits);
		if (m_last_error != ERROR_SUCCESS)
		{
			m_is_connected = false;
			std::cout << "Unable to set COM-port setting" << std::endl;
		}
		else
		{
			m_is_connected = true;
		}
	}

	// Setup handshaking (default is no handshaking)
    /*m_last_error = serial.SetupHandshaking(CSerial::EHandshakeHardware);
	if (m_last_error != ERROR_SUCCESS)
		return ::ShowError(serial.GetLastError(), _T("Unable to set COM-port handshaking"));*/
	
	return m_is_connected;
}

bool RS232Communication::ReadReply(int num_char)
{
	// by default true
	m_is_read_success = true;

	if(m_is_connected==false)
	{
		std::cout << "Not connected to device!" << std::endl;
		m_is_read_success = false;
		return m_is_read_success;
	}

	// reset buffer that holds the reply message from the device
	m_total_reply_size = 0;
	int arr_size = sizeof(m_reply)/sizeof(m_reply[0]);
	for(int i=0;i<arr_size;i++)
	{
		m_reply[i] = ';';
	}

	// Register only for the receive event
    m_last_error = m_serial.SetMask(CSerial::EEventBreak |
								CSerial::EEventCTS   |
								CSerial::EEventDSR   |
								CSerial::EEventError |
								CSerial::EEventRing  |
								CSerial::EEventRLSD  |
								CSerial::EEventRecv);
	if (m_last_error != ERROR_SUCCESS)
	{
		//return ::ShowError(m_serial.GetLastError(), _T("Unable to set COM-port event mask"));
		std::cout << "Unable to set COM-port event mask" << std::endl;
		m_is_read_success = false;
		return m_is_read_success;
	}

	// Use 'non-blocking' reads, because we don't know how many bytes
	// will be received. This is normally the most convenient mode
	// (and also the default mode for reading data).
    m_last_error = m_serial.SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	if (m_last_error != ERROR_SUCCESS)
	{
		//return ::ShowError(m_serial.GetLastError(), _T("Unable to set COM-port read timeout."));
		std::cout << "Unable to set COM-port read timeout." << std::endl;\
		m_is_read_success = false;
		return m_is_read_success;
	}

	// Create a handle for the overlapped operations
	HANDLE hevtOverlapped = ::CreateEvent(0,TRUE,FALSE,0);;
	if (hevtOverlapped == 0)
	{
		//return ::ShowError(m_serial.GetLastError(), _T("Unable to create manual-reset event for overlapped I/O."));
		std::cout << "Unable to create manual-reset event for overlapped I/O." << std::endl;
		m_is_read_success = false;
		return m_is_read_success;
	}

	// Setup the overlapped structure
	OVERLAPPED ov = {0};
	ov.hEvent = hevtOverlapped;

	// Open the "STOP" handle
	HANDLE hevtStop = ::CreateEvent(0,TRUE,FALSE,_T("Overlapped_Stop_Event"));
	if (hevtStop == 0)
	{
		//return ::ShowError(m_serial.GetLastError(), _T("Unable to create manual-reset event for stop event."));
		std::cout << "Unable to create manual-reset event for stop event." << std::endl;
		m_is_read_success = false;
		return m_is_read_success;
	}

    // Keep reading data, until an EOF (CTRL-Z) has been received
	bool fContinue = true;
	do
	{
		// Wait for an event
		m_last_error = m_serial.WaitEvent(&ov);
		if (m_last_error != ERROR_SUCCESS)
		{
			//return ::ShowError(m_serial.GetLastError(), _T("Unable to wait for a COM-port event."));
			std::cout << m_serial.GetLastError() << ", Unable to wait for a COM-port event." << std::endl;
			//m_serial.Purge();
			m_is_read_success = false;
			return m_is_read_success;
		}

		// Setup array of handles in which we are interested
		HANDLE ahWait[2];
		ahWait[0] = hevtOverlapped;
		ahWait[1] = hevtStop;

		// Wait until something happens
		switch (::WaitForMultipleObjects(sizeof(ahWait)/sizeof(*ahWait),ahWait,FALSE,/*INFINITE*/m_time_out))
		{
		case WAIT_OBJECT_0:
			{
				// Save event
				const CSerial::EEvent eEvent = m_serial.GetEventType();

				// Handle break event
// 				if (eEvent & CSerial::EEventBreak)
// 				{
// 					printf("\n### BREAK received ###\n");
// 				}
// 
// 				// Handle CTS event
// 				if (eEvent & CSerial::EEventCTS)
// 				{
// 					printf("\n### Clear to send %s ###\n", m_serial.GetCTS()?"on":"off");
// 				}
// 
// 				// Handle DSR event
// 				if (eEvent & CSerial::EEventDSR)
// 				{
// 					printf("\n### Data set ready %s ###\n", m_serial.GetDSR()?"on":"off");
// 				}

				// Handle error event
				if (eEvent & CSerial::EEventError)
				{
					printf("\n### ERROR: ");
					switch (m_serial.GetError())
					{
					case CSerial::EErrorBreak:		printf("Break condition");			break;
					case CSerial::EErrorFrame:		printf("Framing error");			break;
					case CSerial::EErrorIOE:		printf("IO device error");			break;
					case CSerial::EErrorMode:		printf("Unsupported mode");			break;
					case CSerial::EErrorOverrun:	printf("Buffer overrun");			break;
					case CSerial::EErrorRxOver:		printf("Input buffer overflow");	break;
					case CSerial::EErrorParity:		printf("Input parity error");		break;
					case CSerial::EErrorTxFull:		printf("Output buffer full");		break;
					default:						printf("Unknown");					break;
					}
					printf(" ###\n");
				}

				// Handle ring event
				if (eEvent & CSerial::EEventRing)
				{
					//printf("\n### RING ###\n");
				}

				// Handle RLSD/CD event
				if (eEvent & CSerial::EEventRLSD)
				{
					//printf("\n### RLSD/CD %s ###\n", m_serial.GetRLSD()?"on":"off");
				}

				// Handle data receive event
				if (eEvent & CSerial::EEventRecv)
				{
					// Read data, until there is nothing left
					DWORD dwBytesRead = 0;
					do
					{
						char szBuffer[101];

						// Read data from the COM-port
						m_last_error = m_serial.Read(szBuffer,sizeof(szBuffer)-1,&dwBytesRead);
						if (m_last_error != ERROR_SUCCESS)
						{
							//return ::ShowError(m_serial.GetLastError(), _T("Unable to read from COM-port."));
							std::cout << m_serial.GetLastError() << ", Unable to read from COM-port." << std::endl;
							m_is_read_success = false;
							return m_is_read_success;
						}

						if (dwBytesRead > 0)
						{
							// Finalize the data, so it is a valid string
							//szBuffer[dwBytesRead] = '\0';

							for(int i=0;i<dwBytesRead;i++)
							{
								m_reply[m_total_reply_size] = szBuffer[i];
								m_total_reply_size++;
							}

							// For debugging: display the data
							/*std::cout << "reply size: " << m_total_reply_size << std::endl;
							for(int i=0;i<m_total_reply_size;i++)
							{
								std::cout << m_reply[i] << std::endl;
							}*/
							//std::cout << "size: " << m_total_reply_size << std::endl;

							// exit loop if we have read exactly the number of bytes back from the device
							if(m_total_reply_size==num_char)
							{
								fContinue = false;
								m_is_read_success = true;

								//std::cout << "All bytes read: " << m_total_reply_size << " = " << num_char << std::endl;
							}
							else if(m_total_reply_size>num_char)
							{
								fContinue = false;
								m_is_read_success = false;
								std::cout << "Read too many bytes!!!: " << m_total_reply_size << " > " << num_char << std::endl;
								return m_is_read_success;
							}
						}
					}
					while (dwBytesRead > 0);
				}
			}
			break;

		case WAIT_OBJECT_0+1:
			{
				// Set the continue bit to false, so we'll exit
				m_serial.Purge();
				fContinue = false;
			}
			break;

		case WAIT_TIMEOUT:
			{
				fContinue = false;
				m_is_read_success = false;
				//std::cout << "time out!" << std::endl;
				return m_is_read_success;
			}
			break;

		default:
			{
				// Something went wrong
				//return ::ShowError(m_serial.GetLastError(), _T("Error while calling WaitForMultipleObjects."));
				std::cout << "Error while calling WaitForMultipleObjects." << std::endl;
				m_is_read_success = false;
				return m_is_read_success;
			}
			break;
		}
	}
	while (fContinue);

	// read was successful, as while loop broke, return true
	// need to watch out for time out's here!!!!
	return m_is_read_success;
}

void RS232Communication::Close()
{
	m_serial.Close();
	m_is_connected = false;
}

void RS232Communication::Purge()
{
	m_serial.Purge();
}

std::string RS232Communication::IntToStr(int number)
{
   std::stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}