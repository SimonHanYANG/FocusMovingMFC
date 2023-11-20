#include "stdafx.h"
#include "JoyStick.h"

#include "spwmacro.h"  /* Common macros used by SpaceWare functions. */
#include "si.h"        /* Required for any SpaceWare support within an app.*/
extern "C"
{
#include "siapp.h"     /* Required for siapp.lib symbols */
}

JoyStick::JoyStick()
{
	m_was_moving = false;

	m_dx_raw = 0;
	m_dy_raw = 0;
	m_dz_raw = 0;

	m_dx = 0;
	m_dy = 0;
	m_dz = 0;

	m_move_counter = 0;

	m_max_speed_1 = 4000;
	m_max_speed_2 = 2000;
	m_min_speed_1 = m_max_speed_1*0.1;
	m_min_speed_2 = m_max_speed_2*0.1;

	m_thread_created = false;
}

int JoyStick::SbInit(HWND *Wnd)
// This function initializes the 3D mouse and opens ball for use.
// PRE: None
// POS: int  res         result of SiOpen, =0 if Fail =1 if it Works
{
  int res;                                 //result of SiOpen, to be returned  
  SiOpenData oData;                        //OS Independent data to open ball  
  
  if (SiInitialize() == SPW_DLL_LOAD_ERROR)   //init the SpaceWare input library
     {
		 /*MessageBox(_T("Error: Could not load SiAppDll dll files"),
		         NULL, MB_ICONEXCLAMATION);*/
		 std::cout << "Could not load SiAppDLL dll files" << std::endl;
	 }
  SiOpenWinInit (&oData, *Wnd);          //init Win. platform specific data  
  SiSetUiMode(&m_DevHdl, SI_UI_ALL_CONTROLS); //Config SoftButton Win Display 

  //open data, which will check for device type and return the device handle
  // to be used by this function  
  if ( (m_DevHdl = SiOpen ("TestSP", SI_ANY_DEVICE, SI_NO_MASK,  
                         SI_EVENT, &oData)) == NULL )
     {
     SiTerminate();  //called to shut down the SpaceWare input library 
     res = 0;        //could not open device
     return res; 
     }
  else
     {
     res = 1;        //opened device succesfully
     return res;  
     }  
}

void JoyStick::SbMotionEvent(SiSpwEvent *pEvent, SiskiyouAmnl *manip)
// This function receives motion information and prints out the info on screen
// PRE: SiSpwEvent *pEvent   Containts Data from a 3D Mouse Event
{
	//int curr_x = pEvent->u.spwData.mData[SI_RZ];
	//int curr_y = pEvent->u.spwData.mData[SI_RX];
	//int curr_z = -pEvent->u.spwData.mData[SI_RY];

	int curr_x = pEvent->u.spwData.mData[SI_RZ];
	int curr_y = pEvent->u.spwData.mData[SI_RX];
	int curr_z = -pEvent->u.spwData.mData[SI_RY];

	std::cout<<pEvent->u.spwData.mData[SI_TX]<<std::endl;

	m_dx_raw = (float)(curr_x/max_value)*m_max_speed_1;
	m_dy_raw = (float)(curr_y/max_value)*m_max_speed_1;
	m_dz_raw = (float)(curr_z/max_value)*m_max_speed_2;

	// smoothing
	m_dx = weight*m_dx_raw + (1-weight)*m_dx;
	m_dy = weight*m_dy_raw + (1-weight)*m_dy;
	m_dz = weight*m_dz_raw + (1-weight)*m_dz;
	
	// create thread, and save pointer to the manip object
	m_local_manip = manip;

	if(m_move_counter > 0 && m_move_counter%wait_counts==0)
	{
		if(!m_thread_created)
		{
			// initialize previous corrected values...default is just the raw
			m_dx = m_dx_raw;
			m_dy = m_dy_raw;
			m_dz = m_dz_raw;

			// create thread for moving micromanipulator
			m_thread_created = true;
			CreateThread(NULL, 0, MoveThread, this, 0, NULL);
		}
	}

	m_move_counter++;
}

DWORD JoyStick::MoveThread(void* pParameter)
{
	JoyStick* This = (JoyStick*) pParameter;  // instance pointer
	This->Move();
	return 0;
}

void JoyStick::Move()
// POS: Keeps on looping until non-motion idle event happens
{
	while(m_thread_created)
	{

		// Move Z if greater than threshold
		if(abs(m_dz) >= m_min_speed_2)
		{
			m_was_moving = true;
			m_local_manip->VelocityMode(3, m_dz);
			std::cout << "dz value: " << m_dz << std::endl;
		}

		// Move X and Y
		else if( abs(m_dx) >= m_min_speed_1 || abs(m_dy) >= m_min_speed_1)
		{
			// check if our x is significantly bigger than y
			if(abs(m_dx)/abs(m_dy+1) >= 4)
			{
				m_dy = 0;
			}

			// check if our y is significantly bigger than x
			else if(abs(m_dy)/abs(m_dx+1) >= 4)
			{	
				m_dx = 0;
			}
			
			// move pipette every 15 frames
			m_was_moving = true;
			m_local_manip->VelocityMode(m_dx, m_dy);
			std::cout << "dx and dy values: " << m_dx << " " << m_dy << std::endl;
		}

		// value of movement is too low
		else
		{
			m_local_manip->VelocityMode(0,0,0);
			m_was_moving = false;
			std::cout << "Threshold too low, stop moving" << std::endl;
		}
		//Sleep(100);
	} // while

	m_local_manip->VelocityMode(0,0,0);
	std::cout << "STOP MOVING!" << std::endl;
}

void JoyStick::SbButtonPressEvent(int buttonnumber)
// This function recieves 3D Mouse button information and prints out the info on screen.
// PRE: int     buttonnumber   //Containts number of button pressed 
{
   /* print button pressed(does not include rezero button) */

   switch (buttonnumber) 
   {
      case SI_APP_FIT_BUTTON:           /* #31 defined in si.h*/
		  std::cout << "Fit Button Pressed" << std::endl;
         break;
      case 1:
		  m_max_speed_1 = 4000;
		  m_max_speed_2 = 2000;
		  m_min_speed_1 = m_max_speed_1*0.1;
		  m_min_speed_2 = m_max_speed_2*0.1;
		  std::cout << "Button 1 Pressed" << std::endl;
         break;
      case 2:
		  m_max_speed_1 = 10000;
		  m_max_speed_2 = 5000;
		  m_min_speed_1 = m_max_speed_1*0.1;
		  m_min_speed_2 = m_max_speed_2*0.1;
		  std::cout << "Button 2 Pressed" << std::endl;
         break;
      case 3:
		  std::cout << "Button 3 Pressed" << std::endl;
         break;
      case 4:
		  std::cout << "Button 4 Pressed" << std::endl;
         break;
      case 5:
		  std::cout << "Button 5 Pressed" << std::endl;
         break;
      case 6:
		  std::cout << "Button 6 Pressed" << std::endl;
         break;
      case 7:
		  std::cout << "Button 7 Pressed" << std::endl;
         break;
      case 8:
		  std::cout << "Button 8 Pressed" << std::endl;
         break; 
      default: 
		  std::cout << "Button ? Pressed" << std::endl;
         break;
   } 
}

void JoyStick::SbButtonReleaseEvent(int buttonnumber)
// This function recieves 3D Mouse button information and prints out the info on screen.
// PRE: int     buttonnumber   //Containts number of button pressed 
{
   /* print button pressed(does not include rezero button) */
   switch (buttonnumber) 
   {
      case SI_APP_FIT_BUTTON:           /* #31 defined in si.h*/
         std::cout << "Fit Button Released" << std::endl;
         break;
      case 1:
         std::cout << "Button 1 Released" << std::endl;
         break;
      case 2:
         std::cout << "Button 2 Released" << std::endl;
         break;
      case 3:
         std::cout << "Button 3 Released" << std::endl;
         break;
      case 4:
         std::cout << "Button 4 Released" << std::endl;
         break;
      case 5:
          std::cout << "Button 5 Released" << std::endl;
         break;
      case 6:
          std::cout << "Button 6 Released" << std::endl;
         break;
      case 7:
         std::cout << "Button 7 Released" << std::endl;
         break;
      case 8:
          std::cout << "Button 8 Released" << std::endl;
         break; 
      default: 
          std::cout << "Button ? Released" << std::endl;
         break;
   }
}

void JoyStick::SbZeroEvent()
//  This function clears the previous data, no motion data was recieved
{
	if(m_was_moving)
	{
		m_was_moving = false;
	}
	m_move_counter = 0;

	m_thread_created = false;

	m_dx = 0;
	m_dy = 0;
	m_dz = 0;

	m_dx_raw = 0;
	m_dy_raw = 0;
	m_dz_raw = 0;
}

void JoyStick::SbPreTranslate(MSG* pMsg, SiskiyouAmnl* manip)
// goes into the PreTranslate method of MainFrame
{
	int            num;      /* number of button returned */
	BOOL           handled;  /* is message handled yet */ 
    SiSpwEvent     Event;    /* SpaceWare Event */ 
    SiGetEventData EData;    /* SpaceWare Event Data */
   
    handled = SPW_FALSE;         /* init handled */

    /* init Window platform specific data for a call to SiGetEvent */
    SiGetEventWinInit(&EData, pMsg->message, pMsg->wParam, pMsg->lParam);
  
    /* check whether msg was a 3D mouse event and process it */
    if (SiGetEvent (m_DevHdl, 0, &EData, &Event) == SI_IS_EVENT)
	{
       /* figure out what kind of 3D mouse event it is */
       if (Event.type == SI_MOTION_EVENT)
       {
		   SbMotionEvent(&Event, manip);  /* process 3D mouse motion event */     
	   }
       if (Event.type == SI_ZERO_EVENT)
       {
          SbZeroEvent();          /* process 3D mouse zero event */     
       }
       if (Event.type == SI_BUTTON_EVENT)
       {
          if ((num = SiButtonPressed (&Event)) != SI_NO_BUTTON)	
          {
             SbButtonPressEvent(num);     /* process 3D mouse button event */
          }

		  if ((num = SiButtonReleased (&Event)) != SI_NO_BUTTON)	
          {
             SbButtonReleaseEvent(num);     /* process 3D mouse button event */
          }
       }
       handled = SPW_TRUE;            /* 3D mouse event handled */ 
    } // 3d mouse event handler
}