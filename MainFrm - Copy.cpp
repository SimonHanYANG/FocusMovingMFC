
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "RoboticBiopsy.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_MESSAGE_VOID(WM_CLOSE, Close)
	ON_MESSAGE(WM_IMAGE_ACQUIRED, OnImageAcquired)
	ON_REGISTERED_MESSAGE( WM_3DMOUSE, On3DMouse )
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	//ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_current_state = M_IDLE;

	// Images
	m_image_is_loaded = false;
	m_raw_img = cvCreateImage(cvSize(nImgWidth, nImgHeight), IPL_DEPTH_8U, 1);

	m_save_image_prepath = _T("F:\\Roobotic ICSI Projects\\Laser\\Temporary Saved Images\\");
	m_save_video_prepath = _T("F:\\Roobotic ICSI Projects\\Temporary Saved Videos\\DNA DataBase\\");//("F:\\Roobotic ICSI Projects\\Temporary Saved Videos\\single tracking\\");
	m_save_file_prepath = _T("F:\\Roobotic ICSI Projects\\Laser\\Temporary Saved Data\\Trajectory Feedforward\\");
	m_is_recording_video = false;
	m_is_video_writer_created = false;


	m_output_data_to_file = 0;
	m_save_laser_count = 0;


	m_ImmoIsSaveToDATABASE = true;
	m_save_dataBase_prepath = _T("F:\\Roobotic ICSI Projects\\Temporary Saved Videos\\DNA DataBase\\");

	if (m_ImmoIsSaveToDATABASE)
	{
		CString file_ext(".csv");
		CTime tm_ctime = CTime::GetCurrentTime();
		CString tm_cstring = tm_ctime.Format("%Y-%m-%d-%H.%M.%S");
		exceltm_cstring=tm_cstring;
		CString file_full_cstring_name = m_save_dataBase_prepath + _T("DataBase_") +tm_cstring + file_ext;
		m_ImmoRecordCSVfile.open(file_full_cstring_name);
		m_ImmoRecordCSVfile<<"id,VCL,VSL,VAP,ALH,LIN,WOB,STR,BCF,MAD,head length,head width,head ellipse,\n";
	}

// 	if (m_output_data_to_file)
// 	{
// 		CString file_ext(".csv");
// 		CTime tm_ctime = CTime::GetCurrentTime();
// 		CString tm_cstring = tm_ctime.Format("%Y-%m-%d-%H.%M.%S");
// 		exceltm_cstring=tm_cstring;
// 		CString file_full_cstring_name = m_save_file_prepath + tm_cstring + file_ext;
// 		m_fileSave.open(file_full_cstring_name);
// 		//m_fileSave<<"x,y,degree,prior_x,prior_y,v_visual_x,v_visual_y,v_com_x,v_com<y,v_final_x,v_final_y,radian/s,steps"<<std::endl;
// 		//m_fileSave<<"x,y\n";
// 		//m_fileSave<<"VCL,VSL,VAP,ALH,LIN,WOB,STR,BCF,MAD,head length,head width,head ellipse\n";
// 		//m_fileSave<<"speed,orientation,successful"<<std::endl;
// 		//m_fileSave<<"NO, VCL,VSL,VAP,ALH,LIN,WOB,STR,BCF,MAD,head length,head width,head ellipse, headLength,headWidth,headArea,headEllipticity,acrosomeArea,nucleusArea,acrosomeRatio,tailLength,tailWidth,isTailCurved,midpieceLength,midpieceWidth,neckAngle,isNeckBent,isVacuole"<<std::endl;
// 		//m_fileSave<<"NO,VCL,VSL,VAP,ALH,LIN,WOB,STR,BCF,MAD,length,width,ellipticity,VCL100X,VSL100X,VAP100X,ALH100X,LIN100X,WOB100X,STR100X,BCF100X,MAD100X,head length,head width,head ellipse, tailLength,midpieceLength,midpieceWidth,neckAngle,vacuoleNumber,vacuoleArea,vacuoleRatio\n";
// 		//m_fileSave<<"NO, length100X,width100X,ellipticity100X\n";
// 		//m_fileSave<<"number,isSuccessful"<<std::endl;
	// 	}

	m_ImmoPipetteHeight = 35.0;

	m_drawDelayFrames = 0;

	m_multi_tracker = NULL;

	// Manipulator Initialization
	if(ENABLE_MANIP_LEFT)
	{
// 		CString manip_com = "COM4";
// 		m_manip_left.Initialize("COM4");
// 		m_manip_left.SetVelocity(500);   /// default 2000

		m_manip_left.Initialize(3);	//4

	}
	
	if (ENABLE_MANIP_RIGHT)
	{
// 		CString manip_com = "COM3";
// 		m_manip_right.Initialize("COM3");
// 		m_manip_right.SetVelocity(2000);  /// default 2000
		m_manip_right.Initialize(4);
	}


	// Prior stage initialization
	if (ENABLE_PRIOR_STAGE)
	{
		if(m_prior.InitPriorStage(1))	//COM 1
			std::cout<<"Successfully connected to Prior stage."<<std::endl;
		else
		{
			std::cout<<"Fail to connect to Prior stage."<<std::endl;
			AfxMessageBox(_T("Fail to connect to Prior stage."));
		}
	}
	// 3D MOUSE
	int res;  /* Is 3D mouse detected? */

	/* intitialize 3D mouse */
	res = m_joy_stick.SbInit(&m_hWnd);

	WM_3DMOUSE = RegisterWindowMessage (_T("SpaceWareMessage00"));

	// Eppendorf Pump Initialization
	m_is_pump_motor_start = false;
	if(ENABLE_PUMP_MOTOR && ENABLE_ROTATIONAL_STAGE)
	{
		std::cout<<"Error: Both Eppendorf pump motor and rotational stage selected."<<std::endl;
		AfxMessageBox(_T("Error: Both Eppendorf pump motor and rotational stage selected."));
	}
	else if (ENABLE_PUMP_MOTOR)
	{
		if (m_pump_motor.Initialize())
		{
			m_is_pump_motor_start = true;
		}
		std::cout<<"Is pump motor start: "<<m_is_pump_motor_start<<std::endl;
	}
	else if (ENABLE_ROTATIONAL_STAGE)
	{
		if (m_rotate_stage.Initialize())
		{
			m_is_pump_motor_start = true;
		}
		std::cout<<"Is rotation motor start: "<<m_is_pump_motor_start<<std::endl;
	}

	//Nikon Microscope Initialization
	m_nikon.InitializeMicroscope();

	isNoisepieceDescend=false;
	m_TargetIsSelected=false;
	isShowCooridinate=false;
	//morphology measure
	m_drawMorphologyIsSuccess=false	;
	SpermNumber=0;

	//sperm under 100X
	m_isTargetSperm100XSelected=false;
	m_isTargetSpermToCenter100X = false;
	m_isTargetSpermCentered100X = false;

	//Create Thread for grab image
	m_aquiring_images = true;
	CreateThread(NULL, 0, GrabImageThread, this, 0, NULL);


	//Sperm Immo variables
	m_ContactPipPresetPos = cv::Point(150,450);
	m_ImmoViewCenter = cv::Point(nImgWidth/2, nImgHeight/2);
	m_ImmoIsSpermSelected = false;
	m_ImmoIsSpermCentered = false;
	m_ImmoToServoSpermCenter = false;
	m_ImmoIsRotationNeeded = false;


	m_ImmoLaserCenter = cv::Point(667, 597);	//601, 663 laser position

	//Save and Move to Dish and Gel variables
	m_isDishPositionSet = false;
	m_targetGelNo = 1;

	//Contact Detection variables
	m_ContactIsPipSelected = false;
	m_ContactIsSuccessful = false;


	m_rotateTargetSelected = false;

	//Help Strings
	m_HelpCase1 =	"Contact detection...\r\n"
					"Please bring the pipette near and focus on the dish bottom\r\n"
					"Then double click on the pipette tip.";

	m_HelpCase2 =	"Pipette tip clicked.\r\n"
					"Detecting contact...";

	m_HelpCase3 =	"Immobilizing sperm...\r\n"
					"Please double click on sperm head.";

	m_HelpCase4 =	"Unable to immobilizing sperm.\r\n"
					"Please do contact detection first.";

	m_HelpCase6 =	"Contact successfully found.";

	m_HelpCase7 =	"Contact detection failed.";

	m_HelpCase8 =	"Progress Stopped.";

	m_HelpCase9 =	"Servoing sperm head to center...";

	m_HelpCase10 =	"Sperm centered.\r\n"
		"Performing sperm immobilization...";

	m_HelpCase11 =	"Sperm immobilization completed.";

	m_HelpCase12 = "Select sperm for 100X tracking";

	m_HelpCase13 = "Servoing sperm 100X";
}

CMainFrame::~CMainFrame()
{
	cvReleaseImage(&m_raw_img);
	
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	//***********************Set Initial Size of the GUI Window (Start)*******************
	cs.cx = WINDOW_WIDTH;
	cs.cy = WINDOW_HEIGHT;
	cs.x = 0;
	cs.y = 0;
	cs.style ^= WS_SIZEBOX;
	cs.style ^= WS_MAXIMIZEBOX;
	cs.style &= ~FWS_ADDTOTITLE;
	//***********************Set Initial Size of the GUI Window (end)*******************

	return TRUE;
}

// OnIdle
BOOL CMainFrame::OnIdle(LONG lCount)
{
	CRoboticBiopsyView* pView=(CRoboticBiopsyView*)GetActiveView();

	pView->m_dlg_tools.UpdateDialogControls(&pView->m_dlg_tools, FALSE);
	
	return FALSE;
}

// Close button
void CMainFrame::Close()
// POS: called when closing the program
{
	if (pView->m_view_is_recording_video)
	{
		AfxMessageBox(_T("Please stop video first."));
		return;
	}
// 
// 	// prevents disabling close when camera is not connected
	if(m_camera.IsConnected())
	{
		if(!m_camera.IsStarted())
		{
			AfxMessageBox(_T("Waiting for camera to start..."));
			return;
		}
		else
		{
			m_camera.Stop();
		}
	}

	if (m_fileSave.is_open())
		m_fileSave.close();

	FreeConsole();

	this->OnClose();
}


BOOL CMainFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	pView = (CRoboticBiopsyView*)GetActiveView();

	CString step_string;
	pView->m_dlg_tools.m_combo_focus_step_size.GetWindowText(step_string);
	m_focus_step_size = atof(step_string);
	m_mouse_wheel_zDelta = zDelta;

	CreateThread(NULL, 0, MouseWheelFocusThread, this, 0, NULL);

	return CFrameWnd::OnMouseWheel(nFlags, zDelta, pt);
}

DWORD WINAPI CMainFrame::MouseWheelFocusThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;

	if (ENABLE_PRIOR_STAGE)
	{
		if (This->m_mouse_wheel_zDelta > 0)
		{
			This->m_prior.Focus().MoveUp(This->m_focus_step_size);
			This->m_prior.WaitForFocusStop();
		} 
		else
		{
			This->m_prior.Focus().MoveDown(This->m_focus_step_size);
			This->m_prior.WaitForFocusStop();
		}
	}

	return 0;
}

LRESULT CMainFrame::On3DMouse( WPARAM wParam, LPARAM lParam )
{

}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{

	m_joy_stick.SbPreTranslate(pMsg, &m_manip_left);//for single one 

	if (pMsg->message == WM_KEYDOWN && ENABLE_PRIOR_STAGE)
	{		
		if (m_current_state == M_IDLE)
		{
			// check if stage is already moving
			long status;
			m_prior.Scan().IsMoving(&status);

			// if stage is, i.e., status <=0 not moving, allow stage to move
			if(status <= 0)
			{
				switch(pMsg->wParam) 
				{
				case   VK_LEFT:
					m_prior.Stage().MoveAtVelocity(200,0);
					break; 
				case   VK_RIGHT: 
					m_prior.Stage().MoveAtVelocity(-200,0);
					break;
				case   VK_UP: 
					m_prior.Stage().MoveAtVelocity(0,200);
					break;
				case   VK_DOWN: 
					m_prior.Stage().MoveAtVelocity(0,-200);
					break;
				} 
			}
		}
	}
	//// mouse wheel event handling
	else if(pMsg->message==WM_MOUSEWHEEL)
	{
		POINT ptCursorPos;
		GetCursorPos(&ptCursorPos);
		if (m_current_state==M_IDLE)
		{
			CRoboticBiopsyView* pView=(CRoboticBiopsyView*)GetActiveView();
			if (pView->CameraDisplayArea.PtInRect(ptCursorPos)==1)
			{
				pView->SetFocus();
			}
		}
	}
	else if(pMsg->message == WM_KEYUP && ENABLE_PRIOR_STAGE)
	{
		m_prior.Stage().MoveAtVelocity(0, 0);
	}
	else
		return CFrameWnd::PreTranslateMessage(pMsg);

	return 0;
}

// Mouse left button double click to select initial position of sperm or pipette
void CMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

// 	if (m_output_data_to_file)
// 	{
// 		m_fileSave<<point.x<<","<<point.y<<std::endl;
// 	}

	switch (m_current_state)
	{

	case M_CONTACT_DETECTION:
		{
			m_PipInImagePos = cv::Point(point.x, point.y);
			m_ContactIsPipSelected = true;
			std::cout<<"Pipette Selected.\n";

		}break;
		
		case M_SPERM_IMMO:
		{
//			m_RotateMotorIniPosition = m_rotate_stage.GetCurrentPosition();
 			double stageX = 0.0, stageY = 0.0;
 			m_prior.Stage().GetPosition(&stageX, &stageY);
			//std::cout<<"initial: "<<stageX-RotCenterX<<" , "<<stageY-RotCenterY<<std::endl;
			//m_fileSave<<m_RotateMotorIniPosition<<","<<stageX-RotCenterX<<","<<stageY-RotCenterY<<","<<point.x<<","<<point.y<<std::endl;
			m_spermImmoTracker.setStagePosition(-stageX, -stageY);
			m_spermImmoTracker.setInitialPosition(point.x, point.y);
			m_ImmoIsSpermSelected = true;
			std::cout<<"Sperm Selected.\n";

		}break;

		case M_ROTATING_STAGE:
		{
			m_rotateTargetSelected = true;
			m_rotateTargetPosPx = cv::Point(point.x, point.y);
			m_spermImmoTracker.setInitialPosition(point.x, point.y);

		}break;

		case M_SWITCH_TARGET:
			{
				m_TargetInImagePos = cv::Point(point.x, point.y);
				m_TargetIsSelected = true;
				if (isShowCooridinate==true)
				{
					std::cout<<m_TargetInImagePos.x <<","<<m_TargetInImagePos.y <<std::endl;

				}


			}break;

		case M_TRACK100X:
			{
				double stageX = 0.0, stageY = 0.0;
				m_prior.Stage().GetPosition(&stageX, &stageY);
				m_spermImmoTracker.setStagePosition(-stageX, -stageY);

				m_isTargetSperm100XSelected = true;
				//m_TargetSperm100XPos = cv::Point(point.x, point.y);//not used
				m_spermImmoTracker.setInitialPosition(point.x, point.y);

			}break;

		//case M_TRACK100X:
		//	{
		//		double stageX = 0.0, stageY = 0.0;
		//		m_prior.Stage().GetPosition(&stageX, &stageY);
		//		m_spermImmoTracker.setStagePosition(-stageX, -stageY);

		//		m_isTargetSperm100XSelected = true;
		//		//m_TargetSperm100XPos = cv::Point(point.x, point.y);//not used
		//		m_spermImmoTracker.setInitialPosition(point.x, point.y);

		//	}break;

		default:
			break;
	}


	CFrameWnd::OnLButtonDblClk(nFlags, point);
}

/**************************** Functions *****************************/

//////////////////////////////////////////////////////////////////////
// moving the manipulator

// Function called in Dialog
void CMainFrame::OnMoveManipLeft(int axis, float displacement)
{
	m_manip_axis_left = axis;
	m_manip_disp_left = displacement;
	CreateThread(NULL, 0, MoveManipLeftThread, this, 0, NULL);
}

// Thread created upon button click
DWORD CMainFrame::MoveManipLeftThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*) pParameter;  // instance pointer
	This->MoveManipLeft(This->m_manip_axis_left, This->m_manip_disp_left);
	return 0;
}

// Worker function in thread
void CMainFrame::MoveManipLeft(int axis, float displacement)
{
// 	switch (axis)
// 	{		
// 		case 1://move x axis
// 		{
// 			//get current position
// 			m_manip_position_left = m_manip_left.GetPosition();
// 			m_manip_left.Move(m_manip_position_left.x + displacement, m_manip_position_left.y, m_manip_position_left.z);
// 			break;
// 		}
// 		case 2: //move y axis
// 		{	
// 			m_manip_position_left = m_manip_left.GetPosition();//get current position
// 			m_manip_left.Move(m_manip_position_left.x, m_manip_position_left.y + displacement, m_manip_position_left.z);
// 			break;
// 		}
// 		case 3: //move z axis
// 		{
// 			m_manip_position_left = m_manip_left.GetPosition();//get current position
// 			m_manip_left.Move(m_manip_position_left.x, m_manip_position_left.y, m_manip_position_left.z + displacement);
// 			break;
// 		}
// 		default:
// 			break;
// 	}	
	m_manip_left.MoveRelative(axis, displacement);

}

// Wait for Stop function
void CMainFrame::WaitForManipLeftStop()
{
	//while(m_manip_left.is_moving){;}
	m_manip_left.WaitForManipStop(1);
	m_manip_left.WaitForManipStop(2);
	m_manip_left.WaitForManipStop(3);
}

// Function called in Dialog
void CMainFrame::OnMoveManipRight(int axis, float displacement)
{
	m_manip_axis_right = axis;
	m_manip_disp_right = displacement;
	CreateThread(NULL, 0, MoveManipRightThread, this, 0, NULL);
}

// Thread created upon button click
DWORD CMainFrame::MoveManipRightThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*) pParameter;  // instance pointer
	This->MoveManipRight(This->m_manip_axis_right, This->m_manip_disp_right);
	return 0;
}

// Worker function in thread
void CMainFrame::MoveManipRight(int axis, float displacement)
{
// 	switch (axis)
// 	{		
// 	case 1://move x axis
// 		{
// 			//get current position
// 			m_manip_position_right = m_manip_right.GetPosition();
// 			m_manip_right.Move(m_manip_position_right.x + displacement, m_manip_position_right.y, m_manip_position_right.z);
// 			break;
// 		}
// 	case 2: //move y axis
// 		{	
// 			m_manip_position_right = m_manip_right.GetPosition();//get current position
// 			m_manip_right.Move(m_manip_position_right.x, m_manip_position_right.y + displacement, m_manip_position_right.z);
// 			break;
// 		}
// 	case 3: //move z axis
// 		{
// 			m_manip_position_right = m_manip_right.GetPosition();//get current position
// 			m_manip_right.Move(m_manip_position_right.x, m_manip_position_right.y, m_manip_position_right.z + displacement);
// 			break;
// 		}
// 	default:
// 		break;
// 	}	

	// 	}	
	m_manip_right.MoveRelative(axis, displacement);
}



// Wait for Stop function
void CMainFrame::WaitForManipRightStop()
{
	//while(m_manip_right.is_moving){;}

	m_manip_right.WaitForManipStop(1);
	m_manip_right.WaitForManipStop(2);
	m_manip_right.WaitForManipStop(3);
}


//////////////////////////////////////////////////////////////////////////
// Eppendorf Pump Control via UI input
void CMainFrame::OnPumpMotorControl()
{
	CreateThread(NULL, 0, PumpMotorControlThread, this, 0, NULL);
}

DWORD CMainFrame::PumpMotorControlThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;
	
	if (ENABLE_PUMP_MOTOR)
	{
		This->PumpMotorControl();
	}
	else if (ENABLE_ROTATIONAL_STAGE)
	{
		This->StageMotorControl();
	}

	return 0;
}

void CMainFrame::PumpMotorControl()
{
	if (pView->m_dlg_tools.m_pump_position_mode.GetCheck())	// Position Mode
	{
		m_pump_motor.SetVelocity(500.0);
		if (pView->m_dlg_tools.m_pump_is_aspirate)
			m_pump_motor.MoveRelative(-pView->m_dlg_tools.m_pump_steps);
		else
			m_pump_motor.MoveRelative(pView->m_dlg_tools.m_pump_steps);
	} 
	else	// velocity mode
	{
		if (pView->m_dlg_tools.m_pump_is_aspirate)
			m_pump_motor.MoveAtVelocity(-pView->m_dlg_tools.m_pump_steps);
		else
			m_pump_motor.MoveAtVelocity(pView->m_dlg_tools.m_pump_steps);
	}
}

void CMainFrame::StageMotorControl()
{
	if (pView->m_dlg_tools.m_pump_position_mode.GetCheck())	// Position Mode
	{
		m_rotate_stage.SetVelocity(500.0);
		if (pView->m_dlg_tools.m_pump_is_aspirate)
			m_rotate_stage.MoveRelative(-pView->m_dlg_tools.m_pump_steps);
		else
			m_rotate_stage.MoveRelative(pView->m_dlg_tools.m_pump_steps);
	} 
	else	// velocity mode
	{
		if (pView->m_dlg_tools.m_pump_is_aspirate)
			m_rotate_stage.MoveAtVelocity(-pView->m_dlg_tools.m_pump_steps);
		else
			m_rotate_stage.MoveAtVelocity(pView->m_dlg_tools.m_pump_steps);
	}
}



//////////////////////////////////////////////////////////////////////
// Basler and Image Acquisition 

void CMainFrame::StartCamera()
{
	if(!m_camera.IsStarted())
	{
		m_camera.Start();

		// Initialize the pointer to the View
		pView=(CRoboticBiopsyView*)GetActiveView();
	}
}

void CMainFrame::AcquireImage()
{
	while(m_camera.IsStarted())
	{
		// enables the viewer to start drawing images to screen
		pView->m_is_displaying_image = true;
		m_image_is_loaded = true;

		if(m_camera.AcquireImage())
		{
			m_raw_img->imageData = m_camera.ImageData();
			// Message sent to trigger OnIdle to run each time an image has been retrieved
			PostMessage(WM_IMAGE_ACQUIRED, 0, 0);
		}
	}
}

DWORD CMainFrame::GrabImageThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*) pParameter;  // instance pointer
	This->StartCamera();
	This->AcquireImage();

	return 0;
}

// Record video
void CMainFrame::OnRecordVideo()
{

	if (!m_is_recording_video)
	{
		CString file_ext(".avi");
		CTime tm_ctime = CTime::GetCurrentTime();
		CString tm_cstring = tm_ctime.Format("%Y-%m-%d-%H.%M.%S");
		CString file_full_cstring_name = m_save_video_prepath + tm_cstring + file_ext;
		m_video_writer.open(LPCTSTR(file_full_cstring_name), CV_FOURCC('X','V','I','D'), 33.0, cv::Size(nImgWidth, nImgHeight), 1);
		if (m_video_writer.isOpened())
		{
			m_is_recording_video = true;
			pView->m_view_is_recording_video = true;
			CreateThread(NULL, 0, RecordVideoThread, this, 0, NULL);
		}
		else
		{
			std::cout<<"Failed to create video writer!"<<std::endl;
			AfxMessageBox(_T("Failed to create video writer!"));
		}
	}
	else
	{
		m_is_recording_video = false;
	}
}

DWORD CMainFrame::RecordVideoThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;

	This->RecordVideo();

	return 0;
}

void CMainFrame::RecordVideo()
{
	while(m_is_recording_video)
	{
		m_cs_record.Lock();
		if (!m_record_deque.empty())
		{
			m_video_writer.write(m_record_deque.front());
			m_record_deque.pop_front();
		}
		m_cs_record.Unlock();
		Sleep(1);
	}
	
	m_cs_record.Lock();
	while (!m_record_deque.empty())
	{
		m_video_writer.write(m_record_deque.front());
		m_record_deque.pop_front();
	}
	m_cs_record.Unlock();

	pView->m_view_is_recording_video = false;

	m_video_writer.release();
	std::cout<<"Record video done!"<<std::endl;
	AfxMessageBox(_T("Record video done!"));
}

//	Save current displayed image
void CMainFrame::SaveImage()
{
	CString file_ext(".png");
	CTime tm_ctime = CTime::GetCurrentTime();
	CString tm_cstring = tm_ctime.Format("%Y-%m-%d-%H.%M.%S");
	CString file_full_cstring_name = m_save_image_prepath + tm_cstring + file_ext;
	//CStringA stringa_name(file_full_cstring_name);

	std::vector<int> compression_para;
	compression_para.push_back(cv::IMWRITE_PNG_COMPRESSION);
	compression_para.push_back(0);

	m_cs_display_color_mat.Lock();
	cv::imwrite(LPCTSTR(file_full_cstring_name), m_displayColorImg, compression_para);
	m_cs_display_color_mat.Unlock();
	
}


afx_msg LRESULT CMainFrame::OnImageAcquired(WPARAM wParam, LPARAM lParam)
// PRE: WM_IMAGE_ACQUIRED message sent
// POS: event handler for WM_IMAGE_ACQUIRED 
{

	//draw image
	pView=(CRoboticBiopsyView*)GetActiveView();

	if(pView->m_is_displaying_image)
	{
		m_cs_copy_mat.Lock();
		m_copyImg = cv::cvarrToMat(m_raw_img, true);
		cv::cvtColor(m_copyImg, m_displayColorImg, cv::COLOR_GRAY2BGR);
		m_cs_copy_mat.Unlock();

		CreateThread(NULL, 0, ClosedLoopControlThread, this, 0, NULL);

		DrawImageResults();

		if (m_is_recording_video)
		{
			m_cs_display_color_mat.Lock();
			m_displayColorImg.copyTo(m_recordImg);
			m_cs_display_color_mat.Unlock();

			m_cs_record.Lock();
			m_record_deque.push_back(m_recordImg);
			m_cs_record.Unlock();
		}
	}


	//Main image window
	RECT rect;
	rect.left = pView->CameraDisplayArea.left+2;
	rect.right = pView->CameraDisplayArea.right-2;
	rect.bottom = TopDlgabarHeight + pView->CameraDisplayArea.bottom-2;
	rect.top = TopDlgabarHeight + pView->CameraDisplayArea.top+2;
	InvalidateRect(&rect);

	return 0;
}


void CMainFrame::OnEmergencyStop()
{
	m_current_state = M_IDLE;


	m_ImmoIsSpermSelected = false;
	m_ImmoIsSpermCentered = false;
	m_ImmoToServoSpermCenter = false;


	//100X
	m_isTargetSperm100XSelected=false;
	m_isTargetSpermToCenter100X=false;
	m_isTargetSpermCentered100X=false;

//	m_ContactIsSuccessful = false;
	m_ContactIsPipSelected = false;

	m_HelpString = m_HelpCase8;
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);


	m_rotateTargetSelected = false;
// 	clock_t timeStart = clock();
// 	double xPos = 0., yPos = 0.;
// 	m_prior.Stage().GetPosition(&xPos, &yPos);
// 	clock_t timeStop = clock();
// 	std::cout<<"time cost: "<<timeStop-timeStart<<" ms\n";
// 	std::cout<<"xPos: "<<xPos<<" , yPos: "<<yPos<<std::endl;

	if (ENABLE_MANIP_LEFT)
		m_manip_left.StopAll();


	m_prior.Stage().MoveAtVelocity(0.,0.);
	m_prior.Focus().MoveAtVelocity(0);

	m_rotate_stage.Stop();

	m_spermImmoTracker.cancelTracking();

	if (m_fileSave.is_open())
		m_fileSave.close();


	if (m_ImmoIsSaveToDATABASE && m_ImmoIsSpermGood)
	{
		//file name
		CTime tm_ctime = CTime::GetCurrentTime();
		CString tm_cstring = tm_ctime.Format("%Y-%m-%d-%H.%M.%S");

		//csv DATA file
		m_ImmoRecordCSVfile<<tm_cstring<<","<<m_spermImmoTracker.m_data.VCL
		<<","<<m_spermImmoTracker.m_data.VSL
		<<","<<m_spermImmoTracker.m_data.VAP
		<<","<<m_spermImmoTracker.m_data.ALH
		<<","<<m_spermImmoTracker.m_data.LIN
		<<","<<m_spermImmoTracker.m_data.WOB
		<<","<<m_spermImmoTracker.m_data.STR
		<<","<<m_spermImmoTracker.m_data.BCF
		<<","<<m_spermImmoTracker.m_data.MAD
		<<","<<m_spermImmoTracker.m_data.headLength
		<<","<<m_spermImmoTracker.m_data.headWidth
		<<","<<m_spermImmoTracker.m_data.ellipserate<<std::endl;

		//recorded video clip
		CString file_ext(".avi");
		CString file_full_cstring_name = m_save_dataBase_prepath + tm_cstring + file_ext;
		cv::VideoWriter video_writer;
		video_writer.open(LPCTSTR(file_full_cstring_name), CV_FOURCC('X','V','I','D'), 33.0, cv::Size(nImgWidth, nImgHeight), 0);

		for (int ii = 0; ii < m_ImmoRecordVideoVec.size(); ++ii)
		{
			video_writer.write(m_ImmoRecordVideoVec[ii]);
		}
		video_writer.release();
		std::vector<cv::Mat> swap1;
		m_ImmoRecordVideoVec.swap(swap1);

	}
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

// Draw image processing results
void CMainFrame::DrawImageResults()
{

	m_cs_display_color_mat.Lock();
	cv::circle(m_displayColorImg, m_ImmoLaserCenter, 1, cv::Scalar(255,0,255), 2);
	m_cs_display_color_mat.Unlock();

//  	m_cs_display_color_mat.Lock();
//  	cv::line(m_displayColorImg, cv::Point(200,50), cv::Point(1000,850), cv::Scalar(255,0,255));
// 	cv::line(m_displayColorImg, cv::Point(200,850), cv::Point(1000,50), cv::Scalar(255,0,255));
	//  	m_cs_display_color_mat.Unlock();

	if ( (m_current_state == M_SPERM_IMMO) && m_ImmoIsSpermSelected )
	{
		m_cs_display_color_mat.Lock();
		cv::circle(m_displayColorImg, m_ImmoHeadPt, 1, cv::Scalar(0,0,255), 2);
		cv::circle(m_displayColorImg, m_ImmoTailPt, 1, cv::Scalar(255,0,0), 2);
		//cv::circle(m_displayColorImg, m_ImmoViewCenter, 1, cv::Scalar(0,255,255), 2);	//m_ImmoLaserCenter

		if (m_spermImmoTracker.m_data.SingleSpermEllipse.size() > 0)
			cv::ellipse(m_displayColorImg, m_spermImmoTracker.m_data.SingleSpermEllipse.back(), cv::Scalar(0,255,0));

		//cv::circle(m_displayColorImg, predictedHeadPosition, 3, cv::Scalar(0,255,0), -1);
		//drawContours(m_displayColorImg,std::vector<std::vector<cv::Point> >(1,m_spermImmoTracker.headContour),1,cv::Scalar(0,0,255),1,8);
		m_cs_display_color_mat.Unlock();
	}

	if (m_is_multi_track_done)
	{
		m_cs_display_color_mat.Lock();
		for (int ii=0; ii<m_multi_allData.size(); ++ii)
		{
			if (m_multi_allData[ii].VCL > 0)
				cv::ellipse(m_displayColorImg, m_multi_allData[ii].SingleSpermEllipse.back(), cv::Scalar(0,255,0));
		}
		m_cs_display_color_mat.Unlock();
	}



	if ( (m_current_state == M_CONTACT_DETECTION) && m_ContactIsPipSelected)
	{
		int iROIWidth = 50, iROIHeight = 100;
		cv::Rect tipROI(m_ContactPipPresetPos.x, m_ContactPipPresetPos.y-iROIHeight/2, iROIWidth, iROIHeight);
		m_cs_display_color_mat.Lock();
		cv::rectangle(m_displayColorImg, tipROI, cv::Scalar(255,0,0));
		cv::circle(m_displayColorImg, m_PipInImagePos, 1, cv::Scalar(0,0,255), 2);

		m_cs_display_color_mat.Unlock();
	}

	if ( (m_current_state == M_SWITCH_TARGET) && m_TargetIsSelected )
	{
		m_cs_display_color_mat.Lock();
		cv::circle(m_displayColorImg, m_TargetInImagePos, 1, cv::Scalar(0,0,255), 2);
		m_cs_display_color_mat.Unlock();
	}

	if ( (m_current_state == M_ROTATING_STAGE) && m_rotateTargetSelected)
	{
		m_cs_display_color_mat.Lock();
		cv::circle(m_displayColorImg, m_ImmoHeadPt, 1, cv::Scalar(0,0,255), 2);
		cv::circle(m_displayColorImg, m_ImmoTailPt, 1, cv::Scalar(255,0,0), 2);
		m_cs_display_color_mat.Unlock();
	}

	if ( (m_current_state == M_TRACK100X) && m_isTargetSperm100XSelected )
	{
		m_cs_display_color_mat.Lock();
		if (m_isShowTrack100X)
		{
			cv::circle(m_displayColorImg, m_TargetSperm100XPos, 1, cv::Scalar(0,0,255), 2);
		}
		
		m_cs_display_color_mat.Unlock();
	}

	



	if (m_drawMorphologyIsSuccess)
	{
		if (m_drawDelayFrames <= 20)// to show results only in 20 frames 
		{
			m_cs_display_color_mat.Lock();
		//////////////////////////////////////////////////////////////////////////
		//m_displayColorImg=cv::imread("F:\\Roobotic ICSI Projects\\Temporary Saved Images\\2017-08-21-12.27.45.png",1);
		//drawContours(m_displayColorImg,std::vector<std::vector<cv::Point> >(1,m_SpermMorphology.spermContour),-1,cv::Scalar(0,0,255),1,8);
			//if (m_SpermMorphology.isHeadContourDetected==true)
			//{
			/*drawContours(m_displayColorImg,std::vector<std::vector<cv::Point> >(1,m_SpermMorphology.headContour),-1,cv::Scalar(0,255,0),2,8);
			cv::ellipse( m_displayColorImg, m_SpermMorphology.headEllipse,cv::Scalar(0,0,255), 2, 8 );*/
				
				//roi
				//drawContours(m_displayColorImg,std::vector<std::vector<cv::Point> >(1,m_SpermMorphology.headContour),-1,cv::Scalar(0,255,0),2,8,0,0,cv::Point(m_SpermMorphology.boundRect.x,m_SpermMorphology.boundRect.y));
				cv::ellipse( m_displayColorImg, m_SpermMorphology.headEllipse,cv::Scalar(0,0,255), 2, 8 );

				//cv::circle(m_displayColorImg, m_SpermMorphology.headEllipse.center, 1, cv::Scalar(0,0,255), 2);
			//}

		//draw tail contour
		drawContours(m_displayColorImg,std::vector<std::vector<cv::Point> >(1,m_SpermMorphology.tailContour),-1,cv::Scalar(0,255,0),2,8);

		//draw midpiece bounding box
			cv::Point2f midpieceVertices[4];
		m_SpermMorphology.midpieceRotateBound.points(midpieceVertices);
		for (int j = 0; j < 4; j++)
		{
			cv::line(m_displayColorImg, midpieceVertices[j], midpieceVertices[(j+1)%4], cv::Scalar(255,0,0),3);
		}

			drawContours(m_displayColorImg,m_SpermMorphology.vacuoleContours,-1,cv::Scalar(0,255,255),-1,8);

		//////////////////////////////////////////////////////////////////////////
			m_cs_display_color_mat.Unlock();
			m_drawDelayFrames++;
		}
		else
		{
			m_drawDelayFrames = 0;
			m_drawMorphologyIsSuccess = false;
		}

	}

}



// Control algorithms in real time;
DWORD CMainFrame::ClosedLoopControlThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;
	This->ClosedLoopControl();
	return 0;
}

void CMainFrame::ClosedLoopControl()
{
	if ( (m_current_state == M_SPERM_IMMO) && m_ImmoToServoSpermCenter )
	{
		ServoSpermHeadCenter();
	}

	if ( (m_current_state == M_ROTATING_STAGE) && m_rotateTargetSelected )
	{
		ServoCompensateRotateStage();

		//ServoTheta();
	}
	if ( (m_current_state == M_TRACK100X) && 	m_isTargetSpermToCenter100X )
	{
		ServoSpermCenter100X();
	}

	if (m_current_state == M_MULTISPERM_TRACKING)
	{
		cv::Mat _copyImg;
		m_cs_copy_mat.Lock();
		m_copyImg.copyTo(_copyImg);
		m_cs_copy_mat.Unlock();

		m_multi_tracker->track(_copyImg);
	}

}

//////////////////////////////////////////////////////////////////////////
// Sperm Immobilization functions

void CMainFrame::OnSpermImmo()
{
	//if ( m_ContactIsSuccessful )
	{
		m_HelpString = m_HelpCase3;
		pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);
		std::cout<<"Immobilizing sperm...\r\n";

		CreateThread(NULL, 0, SpermImmoThread, this, 0, NULL);
	}
// 	else
// 	{
// 		m_HelpString = m_HelpCase4;
// 		pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);
// 
// 		std::cout<<"Please do contact detection first.\n";
// 	}
}

DWORD CMainFrame::SpermImmoThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;

	This->SpermImmo();

	return 0;
}

void CMainFrame::SpermImmo()
{
	m_spermImmoTracker.is40X = true;

	m_ImmoSpermCenterCount = 0;
	std::deque<cv::Point2f> vSwap1;	m_ImmoPIDErrorSum.swap(vSwap1);
	std::deque<double> vSwap2; m_RotateAnglePIDErrorSum.swap(vSwap2);
	std::vector<cv::Point2f> vSwap3; m_LaserSpermHeadVec.swap(vSwap3);
// 	std::vector<cv::Point2f> vSwap3; m_ImmoSaveSpeedVector.swap(vSwap3);
// 	std::vector<double> vSwap4; m_ImmoSaveAngleVector.swap(vSwap4);

	m_thetaCount = 0;
	m_RotateMotorIniPosition = m_rotate_stage.GetCurrentPosition();
	m_prior.Stage().SetMaxSpeed(6000.0);

	m_current_state = M_SPERM_IMMO;

	m_ImmoIsSpermSelected = false;
	m_ImmoIsSpermCentered = false;
	m_ImmoIsRotateDone = false;
	m_ImmoToServoSpermCenter = false;

	//1. wait until the sperm is selected
	while( !m_ImmoIsSpermSelected)
	{
		Sleep(1);
		if ( m_current_state != M_SPERM_IMMO)
			return;
	}

	m_HelpString = "Evaluating sperm parameters.\r\n";
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);
	std::cout<<"Evaluating sperm parameters.\r\n";

// 	if (m_output_data_to_file)
// 	{
// 		CString file_ext(".csv");
// 		CTime tm_ctime = CTime::GetCurrentTime();
// 		CString tm_cstring = tm_ctime.Format("%Y-%m-%d-%H.%M.%S");
// 		exceltm_cstring=tm_cstring;
// 		CString file_full_cstring_name = m_save_file_prepath + tm_cstring + file_ext;
// 		m_fileSave.open(file_full_cstring_name);
// 		//m_fileSave<<"x,y,degree,prior_x,prior_y,v_visual_x,v_visual_y,v_com_x,v_com<y,v_final_x,v_final_y,radian/s,steps"<<std::endl;
// 		m_fileSave<<"x,y,v_visual_x,v_visual_y,v_com_x,v_com_y,v_out_x,v_out_y,";
// 	}

	//2. evaluate sperm speed
	m_ImmoToServoSpermCenter = true;
	m_ImmoIsEvaluating = true;	// does not evaluate; for first immo trial
	m_ImmoIsSpermGood = false;


	while (m_ImmoIsEvaluating)
	{
		Sleep(1);
		if ( m_current_state != M_SPERM_IMMO)
			return;
	}

	if (m_ImmoIsSpermGood)
	{
		m_HelpString = "Evaluation done. The Sperm is GOOD.\r\n";
		pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);
	}
	else
	{
		m_HelpString = "The Sperm is BAD. Please select another sperm.\r\n";
		pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);

		m_current_state = M_IDLE;
		Sleep(500);
		m_spermImmoTracker.cancelTracking();
		std::vector<cv::Mat> swap1;
		m_ImmoRecordVideoVec.swap(swap1);
		return;
	}

	//3. wait until the sperm is centered
	while( !m_ImmoIsSpermCentered )
	{
		Sleep(1);
		if ( m_current_state != M_SPERM_IMMO)
			return;
	}
	std::cout<<"Sperm centered.\n";
	std::cout<<"Performing sperm immobilization...\n";
	m_HelpString = m_HelpCase10;
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);

	if (m_output_data_to_file && m_fileSave.is_open())
		m_fileSave.close();



	if ( m_current_state != M_SPERM_IMMO )
		return;

	//4. Perform Sperm hit
	//SpermHit();
	//Sleep(1000);
	m_ImmoToServoSpermCenter = false;
	std::cout<<"Immobilization completed.\n";

	m_current_state = M_IDLE;
	m_HelpString = m_HelpCase11;
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);


//	Sleep(400);

	//m_manip_left.SetVelocity(500);

// 	m_rotate_stage.MoveAtVelocity(0.0);
// 	m_rotate_stage.PowerOff();
// 	m_prior.Stage().MoveAtVelocity(0.0, 0.0);

// 	m_ImmoIsSpermSelected = false;
// 	m_ImmoIsSpermCentered = false;
// 	m_ImmoToServoSpermCenter = false;
// 
// 	if (m_output_data_to_file)
// 		m_fileSave.close();

}





void CMainFrame::SaveImmoSuccessRate(bool isSuccessful)
{
	if (m_output_data_to_file)
	{
		//m_fileSave<<m_ImmoSaveAvgSpeed<<","<<m_ImmoSaveAvgAngle<<","<<isSuccessful<<std::endl;
	}
	else
	{
		std::cout<<"Output to file DISABLED.\n";
		AfxMessageBox(_T("Output to file DISABLED."));
	}
}


void CMainFrame::TrackDarkObject(cv::Mat _srcImg, cv::Point2f &Pt, double &angle_degree)
{
	cv::Mat blurImg, binImg;
	cv::GaussianBlur(_srcImg, blurImg, cv::Size(5,5), 0);
	Methods::maxEntropieThreshold(blurImg, binImg);

	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(binImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	cv::Point2f tempPt;
	double tempMaxArea = 0.;
	int cIndex = 0;
	for (int ii = 0; ii < contours.size(); ++ii)
	{
		double cArea =  cv::contourArea( contours[ii]);
		if ( cArea > tempMaxArea )
		{
			tempMaxArea = cArea;
			cv::Moments mu = cv::moments(contours[ii], true);
			tempPt.x = mu.m10/mu.m00;
			tempPt.y = mu.m01/mu.m00;
			cIndex = ii;
		}
	}

	Pt = tempPt;

	// calculate object angle by Least Square fitting 
	cv::Mat thinImg = cv::Mat::zeros(_srcImg.size(), CV_8UC1);
	cv::drawContours(thinImg, contours, cIndex, cv::Scalar(255,255,255), -1);

	cv::Rect cRect = cv::boundingRect(contours[cIndex]);
	cv::Mat roiThinImg = cv::Mat::zeros(cRect.size(), CV_8UC1);
	thinImg(cRect).copyTo(roiThinImg);

	cv::erode(roiThinImg, roiThinImg, cv::Mat(), cv::Point(-1,-1), 3);
	cv::dilate(roiThinImg, roiThinImg, cv::Mat(), cv::Point(-1,-1), 3);

	Methods::thinningGuoHall(roiThinImg);

	std::vector<cv::Point> thinnedPts;
	for (int ii = 0; ii < roiThinImg.rows; ++ii)
	{
		for (int jj = 0; jj < roiThinImg.cols; ++jj)
		{
			if (roiThinImg.at<uchar>(ii,jj))
				thinnedPts.push_back(cv::Point(jj, ii));
		}
	}

	double lineK = 0., lineB = 0.;
	Methods::FitLeastSquareLine(thinnedPts, lineK, lineB);

	if ( _isnan(lineK) == false )
	{
		angle_degree = abs(atan(lineK)*180.0/CV_PI);
	}
	else
	{
		angle_degree = 90.0;
	}
}


void CMainFrame::ServoSpermHeadCenter()
{
	cv::Mat _copyImg;
	m_cs_copy_mat.Lock();
	m_copyImg.copyTo(_copyImg);
	m_cs_copy_mat.Unlock();

	double stageX = 0., stageY = 0.;
	m_prior.Stage().GetPosition(&stageX, &stageY);
	m_spermImmoTracker.setStagePosition(-stageX, -stageY);
	if (m_ImmoIsEvaluating)
		m_spermImmoTracker.track(_copyImg, true);	//true
	else
		m_spermImmoTracker.track(_copyImg, false);

	m_ImmoHeadPt = m_spermImmoTracker.getHeadPositionPx();
	m_ImmoTailPt = m_spermImmoTracker.calcTailPositionPx(10.0);
	m_ImmoHeadAngleDegree = m_spermImmoTracker.getHeadAngleRadian()*180.0/CV_PI;

	if (m_ImmoIsEvaluating)
	{
		if (m_spermImmoTracker.is40X)
		{
			SpermEvaluate40X(66);	// evaluate for 3 seconds
			m_ImmoRecordVideoVec.push_back(_copyImg);
		}
		else
		{
			SpermEvaluate40X(33);
			//SpermEvaluate(66);	//number of frames to be evaluated; at least 33
			m_ImmoIsSpermGood = true;
		}
	}
	else if (m_ImmoIsSpermGood)
	{
		if (m_spermImmoTracker.is40X)
			PIDSpermHeadCenter40X();
		else
			PIDSpermHeadCenter();
	}
}


void CMainFrame::SpermEvaluate(int totalFrames)
{
	m_spermImmoTracker.updateSpermData();
	if (m_spermImmoTracker.m_data.count >= totalFrames)
	{

		std::cout<<"VCL: "<<m_spermImmoTracker.m_data.VCL<<std::endl;
		std::cout<<"VSL: "<<m_spermImmoTracker.m_data.VSL<<std::endl;
		std::cout<<"VAP: "<<m_spermImmoTracker.m_data.VAP<<std::endl;
		std::cout<<"ALH: "<<m_spermImmoTracker.m_data.ALH<<std::endl;
		std::cout<<"LIN: "<<m_spermImmoTracker.m_data.LIN<<std::endl;
		std::cout<<"WOB: "<<m_spermImmoTracker.m_data.WOB<<std::endl;
		std::cout<<"STR: "<<m_spermImmoTracker.m_data.STR<<std::endl;
		std::cout<<"BCF: "<<m_spermImmoTracker.m_data.BCF<<std::endl;
		std::cout<<"MAD: "<<m_spermImmoTracker.m_data.MAD<<std::endl;
		std::cout<<"len: "<<m_spermImmoTracker.m_data.headLength<<std::endl;
		std::cout<<"wid: "<<m_spermImmoTracker.m_data.headWidth<<std::endl;
		std::cout<<"ell: "<<m_spermImmoTracker.m_data.ellipserate<<std::endl;
		std::cout<<std::endl;

		VCL=m_spermImmoTracker.m_data.VCL;
		VSL=m_spermImmoTracker.m_data.VSL;
		VAP=m_spermImmoTracker.m_data.VAP;
		ALH=m_spermImmoTracker.m_data.ALH;
		LIN=m_spermImmoTracker.m_data.LIN;
		WOB=m_spermImmoTracker.m_data.WOB;
		STR=m_spermImmoTracker.m_data.STR; 
		BCF=m_spermImmoTracker.m_data.BCF;
		MAD=m_spermImmoTracker.m_data.MAD;
		ellipserate=m_spermImmoTracker.m_data.ellipserate;
		headLength=m_spermImmoTracker.m_data.headLength;
		headWidth=m_spermImmoTracker.m_data.headWidth;

		m_ImmoIsEvaluating = false;
	}
}

void CMainFrame::SpermEvaluate40X(int totalFrames)
{
	m_spermImmoTracker.updateSpermData();
	if (m_spermImmoTracker.m_data.count >= totalFrames)
	{
		std::cout<<"VCL: "<<m_spermImmoTracker.m_data.VCL<<std::endl
			<<"LIN: "<<m_spermImmoTracker.m_data.LIN<<std::endl
			<<"ELL: "<<m_spermImmoTracker.m_data.ellipserate<<std::endl;
		if (m_spermImmoTracker.m_data.VCL >= VCL_LOW_LIMIT	//20
			&& m_spermImmoTracker.m_data.LIN >= LIN_LOW_LIMIT	//0.95
			&& m_spermImmoTracker.m_data.ellipserate >= ELL_LOW_LIMIT	//1.5
			&& m_spermImmoTracker.m_data.ellipserate <= ELL_HIGH_LIMIT	//1.67
			)
		{
			m_ImmoIsSpermGood = true;
		}

		m_ImmoIsEvaluating = false;
	}

// 	m_LaserSpermHeadVec.push_back(m_ImmoHeadPt);
// 	if (m_LaserSpermHeadVec.size() >= totalFrames)
// 	{
// // 		std::cout<<"StartPt: "<<m_LaserSpermHeadVec.front().x<<" , "<<m_LaserSpermHeadVec.front().y<<std::endl;
// // 		std::cout<<"Stop_Pt: "<<m_LaserSpermHeadVec.back().x<<" , "<<m_LaserSpermHeadVec.back().y<<std::endl;
// // 
// 		float compensate_x = (m_LaserSpermHeadVec.back().x - m_LaserSpermHeadVec.front().x)*MicronPerPixel20X*33./(float)totalFrames;
// 		float compensate_y = (m_LaserSpermHeadVec.back().y - m_LaserSpermHeadVec.front().y)*MicronPerPixel20X*33./(float)totalFrames;
// 
// 		if (m_output_data_to_file)
// 		{
// 			m_fileSave<<compensate_x<<","<<compensate_y<<std::endl;
// 		}
// // 
// // 		std::cout<<"compensate_x: "<<compensate_x<<std::endl;
// // 		std::cout<<"compensate_y: "<<compensate_y<<std::endl;
// 
// 		m_ImmoIsEvaluating = false;
// 	}

}

void CMainFrame::SaveSpermData()
{
	if(m_output_data_to_file)
	{
		m_fileSave<<m_spermImmoTracker.m_data.VCL<<","
			<<m_spermImmoTracker.m_data.VSL<<","
			<<m_spermImmoTracker.m_data.VAP<<","
			<<m_spermImmoTracker.m_data.ALH<<","
			<<m_spermImmoTracker.m_data.LIN<<","
			<<m_spermImmoTracker.m_data.WOB<<","
			<<m_spermImmoTracker.m_data.STR<<","
			<<m_spermImmoTracker.m_data.BCF<<","
			<<m_spermImmoTracker.m_data.MAD<<","
			<<m_spermImmoTracker.m_data.headLength<<","
			<<m_spermImmoTracker.m_data.headWidth<<","
			<<m_spermImmoTracker.m_data.ellipserate<<std::endl;

		std::cout<<m_spermImmoTracker.m_data.VCL<<","
			<<m_spermImmoTracker.m_data.VSL<<","
			<<m_spermImmoTracker.m_data.VAP<<","
			<<m_spermImmoTracker.m_data.ALH<<","
			<<m_spermImmoTracker.m_data.LIN<<","
			<<m_spermImmoTracker.m_data.WOB<<","
			<<m_spermImmoTracker.m_data.STR<<","
			<<m_spermImmoTracker.m_data.BCF<<","
			<<m_spermImmoTracker.m_data.MAD<<","
			<<m_spermImmoTracker.m_data.headLength<<","
			<<m_spermImmoTracker.m_data.headWidth<<","
			<<m_spermImmoTracker.m_data.ellipserate<<std::endl;

		std::cout<<"DATA SAVED.\n";
	}
}


void CMainFrame::PIDSpermHeadCenter()
{
	double dThetaOutput = 0.0;
	double omega = 0.0;
	double V_x_compensate = 0.0;
	double V_y_compensate = 0.0;

	V_x_compensate = -(m_LaserSpermHeadVec.back().x - m_LaserSpermHeadVec.front().x)*MicronPerPixel20X*33./(float)33.0;
	V_y_compensate = -(m_LaserSpermHeadVec.back().y - m_LaserSpermHeadVec.front().y)*MicronPerPixel20X*33./(float)33.0;


// 	m_prior.Stage().GetPosition(&m_PriorX, &m_PriorY);
// 	if ( m_ImmoIsRotationNeeded && (m_ImmoIsRotateDone == false))
// 	{
// 		dThetaOutput = ServoTheta(m_ImmoHeadAngleDegree);
// 
// 		double STEP_TO_DEGREE_RADIAN = 2.0*CV_PI/8890.;
// 		//__int64 steps = pView->m_dlg_tools.m_pump_steps;
// 		double motorStepSpeed = m_rotate_stage.GetCurrentVelocity();
// 		//std::cout<<"motorSpeed: "<<motorStepSpeed<<std::endl;
// 		//omega = STEP_TO_DEGREE_RADIAN*dThetaOutput;	//speed in rad/s
// 		omega = STEP_TO_DEGREE_RADIAN*motorStepSpeed;	//speed in rad/s
// 
// 		double dx = m_PriorX-RotCenterX;
// 		double dy = m_PriorY-RotCenterY;
// 		m_RotateTheta0 = atan2(dy, dx);
// 		m_RotateRadius = sqrt(dx*dx+dy*dy);
// 		V_x_compensate = -omega*m_RotateRadius*sin(m_RotateTheta0);
// 		V_y_compensate = omega*m_RotateRadius*cos(m_RotateTheta0);
// 	}
	double dErrorX = m_ImmoViewCenter.x - m_ImmoTailPt.x;	// HeadPt changed to TailPt
	double dErrorY = m_ImmoViewCenter.y - m_ImmoTailPt.y;	// HeadPt changed to TailPt

	if (m_ImmoPIDErrorSum.size() >= 5 )
		m_ImmoPIDErrorSum.pop_front();
	m_ImmoPIDErrorSum.push_back(cv::Point2f(dErrorX, dErrorY));

	double dErrorSumX = 0., dErrorSumY = 0.;
	for (int ii = 0; ii < m_ImmoPIDErrorSum.size(); ++ii)
	{
		dErrorSumX += m_ImmoPIDErrorSum[ii].x;
		dErrorSumY += m_ImmoPIDErrorSum[ii].y;
	}

	double V_x_visual, V_y_visual;

	double dKp = 0.8;	//1.0	//1.5
	double dKi = 0.03;	//0.05	//0.1
	V_x_visual = dKp*dErrorX + dKi*dErrorSumX;
	V_y_visual = dKp*dErrorY + dKi*dErrorSumY;

	double V_x = V_x_visual + V_x_compensate;
	double V_y = V_y_visual + V_y_compensate;

// 	std::cout<<"V_x_visual, V_y_visual: "<<V_x_visual<<" , "<< V_y_visual<<std::endl;
// 	std::cout<<"V_x_compen, V_y_compen: "<<V_x_compensate<<" , "<<V_y_compensate<<std::endl;


	if ( (m_current_state != M_SPERM_IMMO) || (m_ImmoToServoSpermCenter == false) )
	{
		m_prior.Stage().MoveAtVelocity(0.0, 0.0);
		m_rotate_stage.MoveAtVelocity(0.0);
	}
	else
	{
		m_prior.Stage().MoveAtVelocity(V_x, V_y);
		m_rotate_stage.MoveAtVelocity(dThetaOutput);
	}

	if (m_output_data_to_file)
	{
		m_fileSave<<m_ImmoTailPt.x<<","<<m_ImmoTailPt.y<<","<<V_x_visual<<","<<V_y_visual<<","<<V_x_compensate<<","<<V_y_compensate<<","<<V_x<<","<<V_y<<std::endl;
	}

	double stopThreshold = 5.0;//m_spermImmoTracker.m_data.VCL*1.2;	//50
	if( (abs(dErrorX) < stopThreshold) && (abs(dErrorY) < stopThreshold) )	//30.0
		++m_ImmoSpermCenterCount;
	else
		m_ImmoSpermCenterCount = 0;

	if (m_ImmoSpermCenterCount >= 15 )
		m_ImmoIsSpermCentered = true;
}

void CMainFrame::PIDSpermHeadCenter40X()
{
	double dThetaOutput = 0.0;
	double omega = 0.0;
	double V_x_compensate = 0.0;
	double V_y_compensate = 0.0;

// 	V_x_compensate = -(m_LaserSpermHeadVec.back().x - m_LaserSpermHeadVec.front().x)*MicronPerPixel40X*33./(float)33.0;
// 	V_y_compensate = -(m_LaserSpermHeadVec.back().y - m_LaserSpermHeadVec.front().y)*MicronPerPixel40X*33./(float)33.0;

	// 	m_prior.Stage().GetPosition(&m_PriorX, &m_PriorY);
	// 	if ( m_ImmoIsRotationNeeded && (m_ImmoIsRotateDone == false))
	// 	{
	// 		dThetaOutput = ServoTheta(m_ImmoHeadAngleDegree);
	// 
	// 		double STEP_TO_DEGREE_RADIAN = 2.0*CV_PI/8890.;
	// 		//__int64 steps = pView->m_dlg_tools.m_pump_steps;
	// 		double motorStepSpeed = m_rotate_stage.GetCurrentVelocity();
	// 		//std::cout<<"motorSpeed: "<<motorStepSpeed<<std::endl;
	// 		//omega = STEP_TO_DEGREE_RADIAN*dThetaOutput;	//speed in rad/s
	// 		omega = STEP_TO_DEGREE_RADIAN*motorStepSpeed;	//speed in rad/s
	// 
	// 		double dx = m_PriorX-RotCenterX;
	// 		double dy = m_PriorY-RotCenterY;
	// 		m_RotateTheta0 = atan2(dy, dx);
	// 		m_RotateRadius = sqrt(dx*dx+dy*dy);
	// 		V_x_compensate = -omega*m_RotateRadius*sin(m_RotateTheta0);
	// 		V_y_compensate = omega*m_RotateRadius*cos(m_RotateTheta0);
	// 	}
	double dErrorX = m_ImmoLaserCenter.x - m_ImmoTailPt.x;
	double dErrorY = m_ImmoLaserCenter.y - m_ImmoTailPt.y;

	if (m_ImmoPIDErrorSum.size() >= 5 )
		m_ImmoPIDErrorSum.pop_front();
	m_ImmoPIDErrorSum.push_back(cv::Point2f(dErrorX, dErrorY));

	double dErrorSumX = 0., dErrorSumY = 0.;
	for (int ii = 0; ii < m_ImmoPIDErrorSum.size(); ++ii)
	{
		dErrorSumX += m_ImmoPIDErrorSum[ii].x;
		dErrorSumY += m_ImmoPIDErrorSum[ii].y;
	}

	double V_x_visual, V_y_visual;

	double dKp = 0.4;	//1.0	//1.5 //0.05
	double dKi = 0.05;	//0.05	//0.1  //0.005
	V_x_visual = dKp*dErrorX + dKi*dErrorSumX;
	V_y_visual = dKp*dErrorY + dKi*dErrorSumY;

	double V_x = V_x_visual + V_x_compensate;
	double V_y = V_y_visual + V_y_compensate;

	if ( (m_current_state != M_SPERM_IMMO) || (m_ImmoToServoSpermCenter == false) )
	{
		m_prior.Stage().MoveAtVelocity(0.0, 0.0);
		m_rotate_stage.MoveAtVelocity(0.0);
	}
	else
	{
		m_prior.Stage().MoveAtVelocity(V_x, V_y);
		m_rotate_stage.MoveAtVelocity(dThetaOutput);
	}

//	std::cout<<"V_x, V_y: "<<V_x<<" , "<<V_y<<std::endl;

	// 	if (m_output_data_to_file)
	// 	{
	// 		m_fileSave<<m_ImmoHeadPt.x<<","<<m_ImmoHeadPt.y<<","<<m_ImmoHeadAngleDegree<<","<<m_PriorX<<","<<m_PriorY
	// 			<<","<<V_x_visual<<","<<V_y_visual<<","<<V_x_compensate<<","<<V_y_compensate<<","<<V_x<<","<<V_y<<","<<omega<<","<<(m_rotate_stage.GetCurrentPosition()-m_RotateMotorIniPosition)<<std::endl;
	// 	}

	double stopThreshold = 50.0;//m_spermImmoTracker.m_data.VCL*1.2;
	if( (abs(dErrorX) < stopThreshold) && (abs(dErrorY) < stopThreshold) )	//30.0
		++m_ImmoSpermCenterCount;
	else
		m_ImmoSpermCenterCount = 0;

	if (m_ImmoSpermCenterCount >= 15 )
		m_isTargetSpermCentered100X= true;

}

void CMainFrame::SaveLaserSuccessful(bool isSuccess)
{
	if (m_output_data_to_file)
	{
		m_save_laser_count++;

		std::cout<<"total save count: "<<m_save_laser_count<<" , "<<"Save: "<<isSuccess<<std::endl;

		m_fileSave<<m_save_laser_count<<","<<isSuccess<<std::endl;
	}
}


void CMainFrame::SpermHit()
{
	//0. Position the pipette to the left of target tail point
	m_manip_left.SetVelocity(200);
	float fDistX = (float)(m_ImmoTailPt.x - m_ContactPipPresetPos.x)*MicronPerPixel20X - 20.0;
	float fDistY = (float)(m_ImmoTailPt.y - m_ContactPipPresetPos.y)*MicronPerPixel20X;
	m_manip_left.Move(m_ContactedManipPos.x + fDistX, m_ContactedManipPos.y + fDistY, m_ContactedManipPos.z - m_ImmoPipetteHeight);
	WaitForManipLeftStop();

// 	m_manip_left.SetVelocity(100);
	sManipPosition manipInitialPos = m_manip_left.GetPosition();
	
	// 1. move 60 um to the right of the sperm tail point
	fDistX = 50.;		//80.0
	m_manip_left.Move(manipInitialPos.x+fDistX, manipInitialPos.y, manipInitialPos.z);
	WaitForManipLeftStop();

	// 2. the pipette down
	m_manip_left.SetVelocity(6000);
	m_manip_left.Move(manipInitialPos.x+fDistX, manipInitialPos.y, manipInitialPos.z + m_ImmoPipetteHeight+20.0);	//30.0
	WaitForManipLeftStop();

	// 3. after pressing the sperm against the bottom, move the pipette up
	m_manip_left.SetVelocity(200);
	m_manip_left.Move(manipInitialPos.x+fDistX, manipInitialPos.y, manipInitialPos.z);		//+0
	WaitForManipLeftStop();

	// 4. move the pipette back to its initial position after contact detection
	m_manip_left.Move(m_ContactedManipPos.x, m_ContactedManipPos.y, m_ContactedManipPos.z - m_ImmoPipetteHeight);
	WaitForManipLeftStop();

}

//////////////////////////////////////////////////////////////////////////
// Track Multiple Sperms in real time
void CMainFrame::OnTrackMultipleSperm()
{
	m_multi_tracker = new MultiSpermTracker;
	m_multi_tracker->initTracking(true, true);	// only track motility
	m_is_multi_track_done = false;
	std::vector<SingleSpermData> swap1;	m_multi_allData.swap(swap1);
	CreateThread(NULL, 0, TrackMultipleSpermThread, this, 0, NULL);
}


DWORD CMainFrame::TrackMultipleSpermThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;

	This->TrackMultipleSperm();

	return 0;
}

void CMainFrame::TrackMultipleSperm()
{
	m_current_state = M_MULTISPERM_TRACKING;

	SleepEx(2000, FALSE);

	m_current_state = M_IDLE;

	SleepEx(500, FALSE);

	std::cout<<"Tracked 2 seconds. Done."<<std::endl;
	m_multi_allData = m_multi_tracker->getData();

	SleepEx(500, FALSE);

	m_is_multi_track_done = true;

	//	std::vector<SingleSpermData> allData = m_multi_tracker->getData();

// 	std::cout<<"\ntotal: "<<allData.size()<<" sperms detected.\n";
// 
// 	for (int ii = 0; ii < allData.size(); ++ii)
// 	{
// 		std::cout<<ii+1<<" -th sperm:\n"<<"VCL: "<<allData[ii].VCL<<" , LIN: "<<allData[ii].LIN<<" ,  ratio: "<<allData[ii].ellipserate<<std::endl;
// 	}

	SleepEx(1000, FALSE);

	m_is_multi_track_done = false;

	delete m_multi_tracker;
	std::cout<<"Thread end.\n";
}




//////////////////////////////////////////////////////////////////////////
// Contact Detection
void CMainFrame::OnContactDetection()
{
	CreateThread(NULL, 0, ContactDetectionThread, this, 0, NULL);
}


DWORD CMainFrame::ContactDetectionThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;

	This->ContactDetection();

	return 0;
}


void CMainFrame::ContactDetection()
{
	m_current_state = M_CONTACT_DETECTION;

	m_HelpString = m_HelpCase1;
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);


	//1. wait until the pipette is selected
	while ( !m_ContactIsPipSelected )
	{
		Sleep(10);
		if (m_current_state != M_CONTACT_DETECTION)
			return;
	}

	m_HelpString = m_HelpCase2;
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);

	//2. position the pipette to the preset position
	PositionPipetteToPreSetPos();
	
	//3. detect contact in image
	m_ContactIsSuccessful = DetectContactImage();

	if (m_ContactIsSuccessful)
	{
		m_HelpString = m_HelpCase6;
		m_ContactedManipPos = m_manip_left.GetPosition();
	}
	else
		m_HelpString = m_HelpCase7;

	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);

	m_current_state = M_IDLE;
}

// The image processing thread
bool CMainFrame::DetectContactImage()
{
	bool bSuccessful = false;
	int iROIWidth = 50, iROIHeight = 100;
	int iDebugCount = 0, iMaxDebugCount = 50;
	const int iDiffThreshold = 11000;		//15000
	const float dTipSlideThreshold = 0.0;	// 5.6 pixels = 2 um	//3.0
	float fManipStep = 5.;
	cv::Rect tipROI(m_PipInImagePos.x, m_PipInImagePos.y-iROIHeight/2, iROIWidth, iROIHeight);
	int iPipROIWidth = m_PipInImagePos.x+100;
	int iPipROIHeight = 200;
	cv::Rect tipDetectionROI(0, m_PipInImagePos.y-iPipROIHeight/2, iPipROIWidth, iPipROIHeight);

	cv::Mat firstImg, thisImg, diffImg, wholeImg;
	m_cs_copy_mat.Lock();
	m_copyImg(tipROI).copyTo(firstImg);
	m_cs_copy_mat.Unlock();
	cv::GaussianBlur(firstImg, firstImg, cv::Size(5,5), 1.0);

	cv::Point tipPtLast(-1, -1);
	cv::Point tipPt;

	double dTipSlidingDist = 0.;

	while(m_current_state == M_CONTACT_DETECTION)
	{
		m_cs_copy_mat.Lock();
		m_copyImg(tipROI).copyTo(thisImg);
		m_copyImg.copyTo(wholeImg);
		m_cs_copy_mat.Unlock();

		// Image Diff method
		cv::GaussianBlur(thisImg, thisImg, cv::Size(5,5), 1.0);
		cv::absdiff(firstImg, thisImg, diffImg);
		int iPixelSum = cv::sum(diffImg).val[0];
		std::cout<<"iPixelSum: "<<iPixelSum<<std::endl;

		// Tip position method
		tipPt = Methods::detectPipetteTipPoint(wholeImg, 0.75, tipDetectionROI);
		m_PipInImagePos = tipPt;
		//std::cout<<"tipPt: "<<tipPt.x<<" , "<<tipPt.y<<std::endl;
		if (tipPtLast.x == -1 && tipPtLast.y == -1)
			tipPtLast = tipPt;
		//dTipSlidingDist = Methods::CalcDistance(tipPt, tipPtLast);
		dTipSlidingDist = tipPt.x - tipPtLast.x;
		dTipSlidingDist = dTipSlidingDist > 30 ? 0 : dTipSlidingDist;
		std::cout<<"Sliding distance: "<<dTipSlidingDist<<std::endl;


		if( iPixelSum > iDiffThreshold && dTipSlidingDist >= dTipSlideThreshold)
		{
			bSuccessful = true;

			sManipPosition tempPos = m_manip_left.GetPosition();
			m_manip_left.Move(tempPos.x, tempPos.y, tempPos.z - 1.*fManipStep);
			WaitForManipLeftStop();

			break;
		}
		else
		{
			sManipPosition tempPos = m_manip_left.GetPosition();
			m_manip_left.Move(tempPos.x, tempPos.y, tempPos.z + fManipStep);
			WaitForManipLeftStop();
		}


		if ( ++iDebugCount >= iMaxDebugCount)
		{
			std::cout<<"Contact detection failed after "<<iMaxDebugCount<<" times.\n";
			break;
		}

		tipPtLast = tipPt;
	}

	return bSuccessful;
}

void CMainFrame::PositionPipetteToPreSetPos()
{
	float x_disp = (float)(m_ContactPipPresetPos.x-m_PipInImagePos.x)*MicronPerPixel20X;
	float y_disp = (float)(m_ContactPipPresetPos.y-m_PipInImagePos.y)*MicronPerPixel20X;	
	sManipPosition manipPos = m_manip_left.GetPosition();
	m_manip_left.Move(manipPos.x + x_disp, manipPos.y + y_disp, manipPos.z);
	WaitForManipLeftStop();
	m_PipInImagePos = m_ContactPipPresetPos;
}


//////////////////////////////////////////////////////////////////////////
// Set and Move to Dish and Gel Position
void CMainFrame::OnSetDishInitialPosition()
{
	if (m_isDishPositionSet == false)
	{
		m_prior.Stage().GetPosition(&m_DishPriorPosition_x, &m_DishPriorPosition_y);
		m_DishManipZPosition = m_manip_left.GetPosition(3);
		m_isDishPositionSet = true;
	}
	else
	{
		//AfxMessageBox("1",MB_YESNO|MB_ICONQUESTION);
		int a = MessageBox( "Do you want to overwrite the dish bottom position" , "Warning!!",MB_YESNO|MB_ICONWARNING);
		if (a == IDYES)
		{
			m_prior.Stage().GetPosition(&m_DishPriorPosition_x, &m_DishPriorPosition_y);
			m_DishManipZPosition = m_manip_left.GetPosition(3);	
			AfxMessageBox("Position overwritten");
		}
		else
		{
		}
			//AfxMessageBox("No"); 
		//std::cout<<"Dish initial position already set. This will overwrite the position."<<std::endl;
	}
}


void CMainFrame::OnMoveStageToDish()
{
	if (m_isDishPositionSet)
		CreateThread(NULL, 0, MoveStageToDishThread, this, 0, NULL);
	else
		std::cout<<"Please set Dish Position first!\n";
}


DWORD CMainFrame::MoveStageToDishThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;

	This->MoveStageToDish();

	return 0;
}


void CMainFrame::MoveStageToDish()
{
	m_HelpString = "Moving...";
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);



	m_manip_left.SetVelocity(2000);

	//m_manip_left.MoveRelative(3, 2000);

	m_manip_left.MoveRelative(3, 20000);
	WaitForManipLeftStop();

	m_prior.Stage().SetMaxSpeed(5000);
	m_prior.Stage().MoveToAbsolute(m_DishPriorPosition_x, m_DishPriorPosition_y);
	m_prior.WaitForStageStop();

	m_TopManipZPosition = m_manip_left.GetPosition(3);

	m_manip_left.MoveRelative(3, -(m_TopManipZPosition-m_DishManipZPosition));

	WaitForManipLeftStop();

	m_HelpString = "Reached target position.";
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);


	
}

void CMainFrame::OnMoveStageToGel()
{
	if (m_isDishPositionSet)
		CreateThread(NULL, 0, MoveStageToGelThread, this, 0, NULL);
	else
		std::cout<<"Please set Dish Position first!\n";
}


DWORD CMainFrame::MoveStageToGelThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;

	This->MoveStageToGel();

	return 0;
}


void CMainFrame::MoveStageToGel()
{
	if (m_targetCellNo == 1)
	{
		m_GelPosition_x = -67000;
		m_GelPosition_y = -5000;
	}
	if (m_targetCellNo == 2)
	{
		m_GelPosition_x = -68000;
		m_GelPosition_y = -5000;
	}
	if (m_targetCellNo == 3)
	{
		m_GelPosition_x = -67000;
		m_GelPosition_y = -22300;
	}
	if (m_targetCellNo == 4)
	{
		m_GelPosition_x = -68000;
		m_GelPosition_y = -22300;
	}



	m_HelpString = "Moving...";
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);

	m_manip_left.SetVelocity(2000);
	//MoveManipLeft(3, -15000);
	m_manip_left.MoveRelative(3, 20000);
	WaitForManipLeftStop();

	m_prior.Stage().SetMaxSpeed(5000);
	m_prior.Stage().MoveToAbsolute(m_GelPosition_x, m_GelPosition_y);
	m_prior.WaitForStageStop();

	m_TopManipZPosition = m_manip_left.GetPosition(3);

	m_manip_left.MoveRelative(3, -(m_TopManipZPosition-m_DishManipZPosition+200));
	WaitForManipLeftStop();

	m_HelpString = "Reached target position.";
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);
}



//////////////////////////////////////////////////////////////////////////
//Nikon control
//	SwitchObjective button
void CMainFrame::OnSwitchObjective(long nPosition)
{
	m_nikon.SetNosepiece(nPosition);
	//CreateThread(NULL, 0, SwitchObjectiveThread, this, 0, NULL);
}

//DWORD CMainFrame::SwitchObjectiveThread(void *pParameter)
//{
//	CMainFrame* This = (CMainFrame*) pParameter;
//	This->m_current_state = M_SWITCH_OBJECTIVE;
//	This->SwitchObjective();
//	This->m_current_state = M_IDLE;
//	return 0;
//}
//
//void CMainFrame::SwitchObjective()
//{
//	long nPosition=3L;
//	m_nikon.SetNosepiece(nPosition);
//}

void CMainFrame::OnSwitchCondenser(long nPosition)
{
	m_nikon.SetCondenser(nPosition);
}

void CMainFrame::OnSwitchTarget()
{
	if (m_current_state==M_SWITCH_TARGET)
	{
		m_current_state=M_IDLE;
	} 
	else
	{
		m_current_state=M_SWITCH_TARGET;// mouse double-click case
	}
	

	

	//wait until target is selected, should be in the thread
// 	while ( !m_TargetIsSelected )
// 	{
// 		Sleep(10);
// 		if (m_current_state != M_SWITCH_TARGET)
// 			return;
// 	}
}

void CMainFrame::OnSwitchTargetAfter()
{
	if (isShowCooridinate==false)
	{
		isShowCooridinate=true;
	} 
	else
	{
		isShowCooridinate=false;
	}

}

void CMainFrame::OnSwitchto100()
{
//	CreateThread(NULL, 0, Switchto100Thread, this, 0, NULL);

	m_current_state=M_SWITCH_OBJECTIVE;
	m_prior.Focus().SetMaxSpeed(500);

	double UpDistance=300, DownDistance=335;	//78+40
	m_prior.Focus().MoveUp(UpDistance);	//actually move the objective downward
	m_prior.WaitForFocusStop();
	// 
	// 	//Switch objective5

	long nPosition=6L;
	OnSwitchObjective(nPosition);
	// 
	// 	//Move Prior stage
	m_prior.Focus().MoveDown(DownDistance);
	//m_prior.WaitForFocusStop();

	if (!m_TargetIsSelected)
	{
		m_TargetInImagePos.x=600;
		m_TargetInImagePos.y=450;

	}
	double XPosition=(600-(m_TargetInImagePos.x*5.0-2051)+197)*MicronPerPixel100X;
	double YPosition=(450-(m_TargetInImagePos.y*5.0-2729)+159)*MicronPerPixel100X;
	m_prior.Stage().MoveRelative(XPosition, YPosition);

	//for 20X AIR
	//double XPosition=(600-(m_TargetInImagePos.x*5.0-2519)-225)*MicronPerPixel100X;
	//double YPosition=(450-(m_TargetInImagePos.y*5.0-2722)-88)*MicronPerPixel100X;
	//m_prior.WaitForStageStop();

	//Increase Light
	m_nikon.SetDiaLampVoltage(22);


	m_current_state = M_IDLE;

}

DWORD CMainFrame::Switchto100Thread(void *pParameter)
{
	CMainFrame* This = (CMainFrame*) pParameter;
	This->m_current_state = M_SWITCH_OBJECTIVE;
	This->Switchto100();
	This->m_current_state = M_IDLE;
	return 0;
}


void CMainFrame::Switchto100()
{
	//Lower objective
	/*m_prior.Focus().SetMaxSpeed(250);

	double UpDistance=2000, DownDistance=1955;
	m_prior.Focus().MoveUp(UpDistance);

	m_prior.WaitForFocusStop();
	// 
	// 
	// 	//Move Prior stage
	m_prior.Focus().MoveDown(DownDistance);
	double XPosition=(600-(600*4.9-3074.5))*MicronPerPixel100X;
	double YPosition=(450-(450*5-3064.5))*MicronPerPixel100X;
	m_prior.Stage().MoveRelative(XPosition, YPosition);*/


	//////////////////////////////////////////////////////////////////////////
	// added by Zhuoran
/*	m_prior.Focus().SetMaxSpeed(500);

	double UpDistance=2000, DownDistance=2030;	//78+40
	m_prior.Focus().MoveUp(UpDistance);	//actually move the objective downward

	m_prior.WaitForFocusStop();
	// 
	// 	//Switch objective5

	long nPosition=6L;
	OnSwitchObjective(nPosition);
	// 
	// 	//Move Prior stage
	m_prior.Focus().MoveDown(DownDistance);
	m_prior.WaitForFocusStop();

	if (!m_TargetIsSelected)
	{
		m_TargetInImagePos.x=600;
		m_TargetInImagePos.y=450;

	}
	double XPosition=(600-(m_TargetInImagePos.x*5.0-2519))*MicronPerPixel100X;
	double YPosition=(450-(m_TargetInImagePos.y*5.0-2722))*MicronPerPixel100X;
	m_prior.Stage().MoveRelative(XPosition, YPosition);
	m_prior.WaitForStageStop();

	//Increase Light
	m_nikon.SetDiaLampVoltage(17);

	//Autofocus(30.0, 5.0);
	// 	Autofocus(30.0, 5.0);
	*/
}

//void CMainFrame::Switchto100Objective()
//{
//	// 	//Switch objective5
//
//	long nPosition=6L;
//	OnSwitchObjective(nPosition);
//
//	//Increase Light
//	m_nikon.SetDiaLampVoltage(30);
//
//	isNoisepieceDescend=false;
//}


void CMainFrame::OnSwitchto20()
{
	//CreateThread(NULL, 0, Switchto100Thread, this, 0, NULL);

	m_current_state = M_SWITCH_OBJECTIVE;
	//Lower objective
	m_prior.Focus().SetMaxSpeed(500);
	double UpDistance=300, DownDistance=275;
	m_prior.Focus().MoveUp(UpDistance);

	m_prior.WaitForFocusStop();
	//Switch objective5
	long nPosition=1L;
	OnSwitchObjective(nPosition);

	m_prior.Focus().MoveDown(DownDistance);
	//Move Prior stage

	if (!m_TargetIsSelected)
	{
		m_TargetInImagePos.x=600;
		m_TargetInImagePos.y=450;

	}

	double XPosition=(600-(m_TargetInImagePos.x*0.2+421))*MicronPerPixel20X;//600*0.2032+450*0.0011+628.405
	double YPosition=(450-(m_TargetInImagePos.y*0.2+550))*MicronPerPixel20X;//600*0.0006+450*0.1991+612.169
	m_prior.Stage().MoveRelative(XPosition, YPosition);

	//TO 20X AIR
	//double XPosition=(600-(m_TargetInImagePos.x*0.2+519))*MicronPerPixel20X;//600*0.2032+450*0.0011+628.405
	//double YPosition=(450-(m_TargetInImagePos.y*0.2+554))*MicronPerPixel20X;//600*0.0006+450*0.1991+612.169

	//Increase Light
	m_nikon.SetDiaLampVoltage(9.5);

	m_current_state = M_IDLE;
}

DWORD CMainFrame::Switchto20Thread(void *pParameter)
{
	CMainFrame* This = (CMainFrame*) pParameter;
	This->m_current_state = M_SWITCH_OBJECTIVE;
	This->Switchto100();
	This->m_current_state = M_IDLE;
	return 0;
}


void CMainFrame::Switchto20()
{
	//Lower objective
	/*double DownDistance, UpDistance;
	m_prior.Focus().MoveDown(DownDistance);

	//Switch objective
	long nPosition=5L;
	OnSwitchObjective(nPosition);

	//Move Prior stage
	double XPosition, YPosition;
	m_prior.Stage().MoveRelative(XPosition, YPosition);*/

}

void CMainFrame::ReadLight()
{
	std::cout<<m_nikon.ReadDiaLampVoltage()<<std::endl;
}


void CMainFrame::OnSpermTracking100X()
{
	CreateThread(NULL, 0, SpermTracking100XThread, this, 0, NULL);
}

DWORD CMainFrame::SpermTracking100XThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;

	This->SpermTracking100X();

	return 0;
}

void CMainFrame::SpermTracking100X()
{
	m_spermImmoTracker.is40X=true;
	m_isShowTrack100X=true;

	m_current_state = M_TRACK100X;
	std::deque<cv::Point2f> vSwap1;	m_ImmoPIDErrorSum.swap(vSwap1);

	m_isTargetSperm100XSelected = false;
	m_isTargetSpermToCenter100X = false;
	m_isTargetSpermCentered100X = false;


	m_prior.Stage().SetMaxSpeed(500);

	m_HelpString = m_HelpCase12;
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);
	//std::cout<<"Servoing sperm head to center...\n";
	//wait until the sperm is selected
	while( !m_isTargetSperm100XSelected)
	{
		Sleep(1);
		if ( m_current_state != M_TRACK100X)
			return;
	}
	//evaluate sperm speed
	//m_ImmoIsEvaluating = true;
	m_isTargetSpermToCenter100X = true;//IS servo to center
	m_isEvaluate100X=true;

	m_HelpString = m_HelpCase13;
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);

/*	while (m_ImmoIsEvaluating)
	{
		Sleep(1);
		if ( m_current_state != M_SPERM_IMMO)
			return;
	}*/

	//wait until the sperm is centered
	while( !m_isTargetSpermCentered100X )//IS centered already
	{
		Sleep(1);
		if ( m_current_state != M_TRACK100X)
			return;
	}
	m_isShowTrack100X=false;
	//spermHit100X();
	//m_current_state=M_IDLE;

	//return;
}


void CMainFrame::Evaluate100X(int totalFrames)
{
	m_spermImmoTracker.updateSpermData100X();
	if (m_spermImmoTracker.m_data.count >= totalFrames)
	{
		std::cout<<"100X measurement "<<std::endl;
		std::cout<<"VCL: "<<m_spermImmoTracker.m_data.VCL100X<<std::endl;
		std::cout<<"VSL: "<<m_spermImmoTracker.m_data.VSL100X<<std::endl;
		std::cout<<"VAP: "<<m_spermImmoTracker.m_data.VAP100X<<std::endl;
		std::cout<<"ALH: "<<m_spermImmoTracker.m_data.ALH100X<<std::endl;
		std::cout<<"LIN: "<<m_spermImmoTracker.m_data.LIN100X<<std::endl;
		std::cout<<"WOB: "<<m_spermImmoTracker.m_data.WOB100X<<std::endl;
		std::cout<<"STR: "<<m_spermImmoTracker.m_data.STR100X<<std::endl;
		std::cout<<"BCF: "<<m_spermImmoTracker.m_data.BCF100X<<std::endl;
		std::cout<<"MAD: "<<m_spermImmoTracker.m_data.MAD100X<<std::endl;
		std::cout<<std::endl;

		m_isEvaluate100X = false;
	}
}

void CMainFrame::ServoSpermCenter100X()//inside closeloop control
{


	cv::Mat _copyImg;
	m_cs_copy_mat.Lock();
	m_copyImg.copyTo(_copyImg);
	m_cs_copy_mat.Unlock();

	double stageX = 0., stageY = 0.;
	m_prior.Stage().GetPosition(&stageX, &stageY);
	m_spermImmoTracker.setStagePosition(-stageX, -stageY);
	//if (m_ImmoIsEvaluating)
		//m_spermImmoTracker.track(_copyImg, true);
	//else
		m_spermImmoTracker.track(_copyImg, false);

	m_TargetSperm100XPos = m_spermImmoTracker.getHeadPositionPx();// tracking the sperm
	//m_ImmoTailPt = m_spermImmoTracker.calcTailPositionPx(17.5);
	//m_ImmoHeadAngleDegree = m_spermImmoTracker.getHeadAngleRadian()*180.0/CV_PI;

	if (m_isEvaluate100X)//
	{
		Evaluate100X(50);	//number of frames to be evaluated; at least 33
	}
	else
	{
	if (m_TargetSperm100XPos.x>0 && m_TargetSperm100XPos.x<1200 && m_TargetSperm100XPos.y>0 && m_TargetSperm100XPos.y<900 )
	{
		m_isShowTrack100X=false;
		PIDSpermHeadCenter100X();
	}
		
	}
}

void CMainFrame::PIDSpermHeadCenter100X()
{
	
	

	double dThetaOutput = 0.0;
	double omega = 0.0;
	double V_x_compensate = 0.0;
	double V_y_compensate = 0.0;
	// 	m_prior.Stage().GetPosition(&m_PriorX, &m_PriorY);
	// 	if ( m_ImmoIsRotationNeeded && (m_ImmoIsRotateDone == false))
	// 	{
	// 		dThetaOutput = ServoTheta(m_ImmoHeadAngleDegree);
	// 
	// 		double STEP_TO_DEGREE_RADIAN = 2.0*CV_PI/8890.;
	// 		//__int64 steps = pView->m_dlg_tools.m_pump_steps;
	// 		double motorStepSpeed = m_rotate_stage.GetCurrentVelocity();
	// 		//std::cout<<"motorSpeed: "<<motorStepSpeed<<std::endl;
	// 		//omega = STEP_TO_DEGREE_RADIAN*dThetaOutput;	//speed in rad/s
	// 		omega = STEP_TO_DEGREE_RADIAN*motorStepSpeed;	//speed in rad/s
	// 
	// 		double dx = m_PriorX-RotCenterX;
	// 		double dy = m_PriorY-RotCenterY;
	// 		m_RotateTheta0 = atan2(dy, dx);
	// 		m_RotateRadius = sqrt(dx*dx+dy*dy);
	// 		V_x_compensate = -omega*m_RotateRadius*sin(m_RotateTheta0);
	// 		V_y_compensate = omega*m_RotateRadius*cos(m_RotateTheta0);
	// 	}
	double dErrorX = m_ImmoViewCenter.x - m_TargetSperm100XPos.x;
	double dErrorY = m_ImmoViewCenter.y - m_TargetSperm100XPos.y;

	if (m_ImmoPIDErrorSum.size() >= 5 )
		m_ImmoPIDErrorSum.pop_front();
	m_ImmoPIDErrorSum.push_back(cv::Point2f(dErrorX, dErrorY));

	double dErrorSumX = 0., dErrorSumY = 0.;
	for (int ii = 0; ii < m_ImmoPIDErrorSum.size(); ++ii)
	{
		dErrorSumX += m_ImmoPIDErrorSum[ii].x;
		dErrorSumY += m_ImmoPIDErrorSum[ii].y;
	}

	double V_x_visual, V_y_visual;

	double dKp = 0.05;	//1.0	//1.5 //0.05
	double dKi = 0.005;	//0.05	//0.1  //0.005
	V_x_visual = dKp*dErrorX + dKi*dErrorSumX;
	V_y_visual = dKp*dErrorY + dKi*dErrorSumY;

	double V_x = V_x_visual + V_x_compensate;
	double V_y = V_y_visual + V_y_compensate;

	if ( (m_current_state != M_TRACK100X) || (m_isTargetSpermToCenter100X == false) )
	{
		m_prior.Stage().MoveAtVelocity(0.0, 0.0);
		m_rotate_stage.MoveAtVelocity(0.0);
	}
	else
	{
		m_prior.Stage().MoveAtVelocity(V_x, V_y);
		m_rotate_stage.MoveAtVelocity(dThetaOutput);
	}

	// 	if (m_output_data_to_file)
	// 	{
	// 		m_fileSave<<m_ImmoHeadPt.x<<","<<m_ImmoHeadPt.y<<","<<m_ImmoHeadAngleDegree<<","<<m_PriorX<<","<<m_PriorY
	// 			<<","<<V_x_visual<<","<<V_y_visual<<","<<V_x_compensate<<","<<V_y_compensate<<","<<V_x<<","<<V_y<<","<<omega<<","<<(m_rotate_stage.GetCurrentPosition()-m_RotateMotorIniPosition)<<std::endl;
	// 	}

	double stopThreshold = 50.0;//m_spermImmoTracker.m_data.VCL*1.2;
	if( (abs(dErrorX) < stopThreshold) && (abs(dErrorY) < stopThreshold) )	//30.0
		++m_ImmoSpermCenterCount;
	else
		m_ImmoSpermCenterCount = 0;

	if (m_ImmoSpermCenterCount >= 15 )
		m_isTargetSpermCentered100X= true;
}
void CMainFrame::OnSpermTracking20X()
{
	CreateThread(NULL, 0, SpermTracking20XThread, this, 0, NULL);
}

DWORD CMainFrame::SpermTracking20XThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;

	This->SpermTracking20X();

	return 0;
}

void CMainFrame::SpermTracking20X()//for prediction
{
	
	m_spermImmoTracker.is40X=false;
	m_ImmoSpermCenterCount = 0;
	std::deque<cv::Point2f> vSwap1;	m_ImmoPIDErrorSum.swap(vSwap1);
	std::deque<double> vSwap2; m_RotateAnglePIDErrorSum.swap(vSwap2);

	predictedHeadPosition=cv::Point(0,0);
	// 	std::vector<cv::Point2f> vSwap3; m_ImmoSaveSpeedVector.swap(vSwap3);
	// 	std::vector<double> vSwap4; m_ImmoSaveAngleVector.swap(vSwap4);

	m_thetaCount = 0;
	m_RotateMotorIniPosition = m_rotate_stage.GetCurrentPosition();
	m_prior.Stage().SetMaxSpeed(3000.0);//6000.0

	m_current_state = M_SPERM_IMMO;

	m_ImmoIsSpermSelected = false;
	m_ImmoIsSpermCentered = false;
	m_ImmoIsRotateDone = false;
	m_ImmoToServoSpermCenter = false;



	//2. wait until the sperm is selected
	while( !m_ImmoIsSpermSelected)
	{
		Sleep(1);
		if ( m_current_state != M_SPERM_IMMO)
			return;
	}

	m_HelpString = m_HelpCase9;
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);
	std::cout<<"Servoing sperm head to center...\n";

	//3. evaluate sperm speed
	m_ImmoToServoSpermCenter = true;
	m_ImmoIsEvaluating = true;

	while (m_ImmoIsEvaluating)
	{
		Sleep(1);
		if ( m_current_state != M_SPERM_IMMO)
			return;
	}

	//3. wait until the sperm is centered
	while( !m_ImmoIsSpermCentered )
	{
		Sleep(1);
		if ( m_current_state != M_SPERM_IMMO)
			return;
	}
	std::cout<<"Sperm centered.\n";
	m_HelpString = m_HelpCase10;
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);

	if ( m_current_state != M_SPERM_IMMO )
		return;
	
	

}

void CMainFrame::OnPredictPosition()
{
	CreateThread(NULL, 0, PredictPositionThread, this, 0, NULL);
	
}

DWORD CMainFrame::PredictPositionThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;

	This->PredictPosition();

	return 0;
}

void CMainFrame::PredictPosition()
{
	m_ImmoToServoSpermCenter = false;
	m_prior.Stage().MoveAtVelocity(0.,0.);
	//m_ImmoIsEvaluating = true;
	predictedHeadPosition=m_spermImmoTracker.predictHeadPosition(2);
}

void CMainFrame::OnPredictSwitch()
{
	//CreateThread(NULL, 0, PredictSwitchThread, this, 0, NULL);

	//PredictPosition();

	m_ImmoToServoSpermCenter = false;
	m_prior.Stage().MoveAtVelocity(0.,0.);
	m_prior.Stage().MoveAtVelocity(0.,0.);
	//m_ImmoIsEvaluating = true;
	predictedHeadPosition=m_spermImmoTracker.predictHeadPosition(3);

	m_current_state=M_SWITCH_OBJECTIVE;
	m_prior.Focus().SetMaxSpeed(500);

	double UpDistance=200, DownDistance=225;	//78+40
	m_prior.Focus().MoveUp(UpDistance);	//actually move the objective downward
	m_prior.WaitForFocusStop();
	// 
	// 	//Switch objective5

	long nPosition=6L;
	OnSwitchObjective(nPosition);
	// 
	// 	//Move Prior stage
	m_prior.Focus().MoveDown(DownDistance);
	//m_prior.WaitForFocusStop();

	double XPosition=(600-(predictedHeadPosition.x*5.0-2051)+197)*MicronPerPixel100X;
	double YPosition=(450-(predictedHeadPosition.y*5.0-2729)+159)*MicronPerPixel100X;

	//air to 100 oil
	//double XPosition=(600-(predictedHeadPosition.x*5.0-2519)-225)*MicronPerPixel100X;
	//double YPosition=(450-(predictedHeadPosition.y*5.0-2722)-88)*MicronPerPixel100X;

	m_prior.Stage().MoveRelative(XPosition, YPosition);
	//m_prior.WaitForStageStop();

	//Increase Light
	m_nikon.SetDiaLampVoltage(21);


	OnSpermTracking100X();

	//start track
	//m_current_state = M_TRACK100X;

	//std::deque<cv::Point2f> vSwap1;	m_ImmoPIDErrorSum.swap(vSwap1);


	//m_prior.Stage().SetMaxSpeed(500);
	//
	//m_isTargetSpermCentered100X = false;

	//double stageX = 0.0, stageY = 0.0;
	//m_prior.Stage().GetPosition(&stageX, &stageY);
	//m_spermImmoTracker.setStagePosition(-stageX, -stageY);

	//m_isTargetSperm100XSelected = true;
	//m_spermImmoTracker.setInitialPosition(600, 450);

	//m_isTargetSpermToCenter100X = true;//enable servoing


}
DWORD CMainFrame::PredictSwitchThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;

	This->PredictSwitch();

	return 0;
}
void CMainFrame::PredictSwitch()
{
	//PredictPosition();

	m_ImmoToServoSpermCenter = false;
	m_prior.Stage().MoveAtVelocity(0.,0.);
	//m_ImmoIsEvaluating = true;
	predictedHeadPosition=m_spermImmoTracker.predictHeadPosition(10);

	m_current_state=M_SWITCH_OBJECTIVE;
	m_prior.Focus().SetMaxSpeed(500);

	double UpDistance=2000, DownDistance=2040;	//78+40
	m_prior.Focus().MoveUp(UpDistance);	//actually move the objective downward
	m_prior.WaitForFocusStop();
	// 
	// 	//Switch objective5

	long nPosition=6L;
	OnSwitchObjective(nPosition);
	// 
	// 	//Move Prior stage
	m_prior.Focus().MoveDown(DownDistance);
	//m_prior.WaitForFocusStop();

	
	double XPosition=(600-(predictedHeadPosition.x*5.0-2519)-225)*MicronPerPixel100X;
	double YPosition=(450-(predictedHeadPosition.y*5.0-2722)-88)*MicronPerPixel100X;
	m_prior.Stage().MoveRelative(XPosition, YPosition);
	//m_prior.WaitForStageStop();

	//Increase Light
	m_nikon.SetDiaLampVoltage(20);


	m_current_state = M_IDLE;

}



/*void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	switch (m_current_state)
	{

	

	case M_TRACK100X:
		{
			m_isTargetSperm100XSelected = true;
			//m_TargetSperm100XPos = cv::Point(point.x, point.y);//not used
			m_spermImmoTracker.setInitialPosition(point.x, point.y);

		}break;


	default:
		break;
	}

	CFrameWnd::OnLButtonDown(nFlags, point);
}*/

//Morphology measurement
void CMainFrame::OnMorphologyMeasure()
{ 
	//if (m_drawMorphologyIsSuccess==false)
	{
		CreateThread(NULL, 0, MorphologyMeasureThread, this, 0, NULL);
	}
	//else
	{
		//m_drawMorphologyIsSuccess=false;
	}
	
} 

DWORD CMainFrame:: MorphologyMeasureThread(void *pParameter)
{
	CMainFrame* This = (CMainFrame*) pParameter;
	//This->m_current_state = M_MORPHOLOGY_MEASURE;
	This->MorphologyMeasure();
	//This->m_current_state = M_IDLE;
	return 0;
}

void CMainFrame::MorphologyMeasure()
{
	if ((m_current_state == M_TRACK100X) && m_isTargetSperm100XSelected)
	{
	
	cv::Mat morphologyImg;

	m_cs_copy_mat.Lock();
	m_copyImg.copyTo(morphologyImg);
	m_cs_copy_mat.Unlock();

	//morphologyImg=cv::imread("F:\\Roobotic ICSI Projects\\Temporary Saved Images\\Position Rotation Data\\2017-11-12-15.31.46.png",0);
	//measure process
	//cv::Mat colormorphologyImg;

	//cv::cvtColor(morphologyImg, colormorphologyImg, cv::COLOR_GRAY2BGR);
	cv::Mat TailImage(morphologyImg.rows,morphologyImg.cols,CV_8U);
	////ROI
	m_SpermMorphology.processROI(morphologyImg,m_TargetSperm100XPos);//measurehead
	m_SpermMorphology.process(morphologyImg,TailImage);

	//cv::Mat SpermImage(morphologyImg.rows,morphologyImg.cols,CV_8U);
	//cv::Mat HeadImage(morphologyImg.rows,morphologyImg.cols,CV_8U);
	


	//m_SpermMorphology.Segmentation(morphologyImg,SpermImage);
	//m_SpermMorphology.DetectHead(colormorphologyImg,SpermImage,HeadImage);
	//m_SpermMorphology.DetectTail(HeadImage,SpermImage,TailImage);
	m_SpermMorphology.DetectMidpiece(TailImage);
	m_SpermMorphology.DetectVacuole(morphologyImg);

	morphologyImg.copyTo(savedImage);

	m_drawMorphologyIsSuccess=true;

	std::cout<<"length: "<<m_SpermMorphology.headLength<<std::endl;
	std::cout<<"width: "<<m_SpermMorphology.headWidth<<std::endl;
	std::cout<<"ellipticity: "<<m_SpermMorphology.headEllipticity<<std::endl;
	std::cout<<"Vacuole Number: "<<m_SpermMorphology.vacuoleNumber<<std::endl;
	std::cout<<"Vacuole Area: "<<m_SpermMorphology.vacuoleArea<<std::endl;
	std::cout<<"Midpiece Length: "<<m_SpermMorphology.midpieceLength<<std::endl;
	std::cout<<"Midpiece Width: "<<m_SpermMorphology.midpieceWidth<<std::endl;
	std::cout<<"Midpiece Angle: "<<m_SpermMorphology.midpieceAngle<<std::endl;
	std::cout<<"Tail Length: "<<m_SpermMorphology.tailLength<<std::endl;
	std::cout<<std::endl;

	////OUTPUT
	//std::cout<<"-----Sperm Parameter Measurement-----"<<std::endl;
	//std::cout<<std::left<<std::setw(20)<<"VCL: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.VCL<<std::endl;
	//std::cout<<std::setw(20)<<"VSL: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.VSL<<std::endl;
	//std::cout<<std::setw(20)<<"VAP: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.VAP<<std::endl;
	//std::cout<<std::setw(20)<<"ALH: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.ALH<<std::endl;
	//std::cout<<std::setw(20)<<"LIN: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.LIN<<std::endl;
	//std::cout<<std::setw(20)<<"WOB: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.WOB<<std::endl;
	//std::cout<<std::setw(20)<<"STR: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.STR<<std::endl;
	//std::cout<<std::setw(20)<<"BCF: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.BCF<<std::endl;
	//std::cout<<std::setw(20)<<"MAD: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.MAD<<std::endl;
	//std::cout<<std::setw(20)<<"Head Length: "<<std::setprecision(3)<<m_SpermMorphology.headLength*MicronPerPixel100X<<std::endl;
	//std::cout<<std::setw(20)<<"Head Width: "<<std::setprecision(3)<<m_SpermMorphology.headWidth*MicronPerPixel100X<<std::endl;
	
	}
}


void CMainFrame::SaveMorphologySuccess()
{
	if (m_output_data_to_file)
	{
		SpermNumber++;
		//m_fileSave<<m_ImmoSaveAvgSpeed<<","<<m_ImmoSaveAvgAngle<<","<<isSuccessful<<std::endl;
		m_fileSave<<SpermNumber<<","
			<< m_SpermMorphology.headLength<<","
			<<m_SpermMorphology.headWidth<<","
			<<m_SpermMorphology.headArea<<","
			<<m_SpermMorphology.headEllipticity<<","
			<<m_SpermMorphology.acrosomeArea<<","
			<<m_SpermMorphology.nucleusArea<<","
			<<m_SpermMorphology.acrosomeAreaRatio<<","
			<<m_SpermMorphology.tailLength<<","
			<<m_SpermMorphology.tailWidth<<","
			<<m_SpermMorphology.isTailCurved<<","
			<<m_SpermMorphology.midpieceLength<<","
			<<m_SpermMorphology.midpieceWidth<<","
			<<m_SpermMorphology.midpieceAngle<<","
			<<m_SpermMorphology.isBentNeck<<","
			<<m_SpermMorphology.isVacuole<<std::endl;
		//saveImage
		m_drawMorphologyIsSuccess=false;

		CString file_ext(".png");
		CTime tm_ctime = CTime::GetCurrentTime();
		CString tm_cstring = tm_ctime.Format("%Y-%m-%d-%H.%M.%S");
		CString tm_cstring2;
		tm_cstring2.Format("%d",SpermNumber);
		CString file_full_cstring_name = m_save_image_prepath + tm_cstring + tm_cstring2+ file_ext;
		//CStringA stringa_name(file_full_cstring_name);

		std::vector<int> compression_para;
		compression_para.push_back(cv::IMWRITE_PNG_COMPRESSION);
		compression_para.push_back(0);

		m_cs_display_color_mat.Lock();
		cv::imwrite(LPCTSTR(file_full_cstring_name), m_displayColorImg, compression_para);
		m_cs_display_color_mat.Unlock();
	}
	else
	{
		std::cout<<"Output to file DISABLED.\n";
		AfxMessageBox(_T("Output to file DISABLED."));
	}
}
void CMainFrame::OnEvaluate100X()//just record 100X motility OR morphology
{
	if (m_output_data_to_file)
	{
		SpermNumber++;

		//////////////////////////////////////////////////////////////////////////
		///*	m_fileSave<<SpermNumber<<","
		//		<<VCL<<","
		//		<<VSL<<","
		//		<<VAP<<","
		//		<<ALH<<","
		//		<<LIN<<","
		//		<<WOB<<","
		//		<<STR<<","
		//		<<BCF<<","
		//		<<MAD<<","
		//		<<m_spermImmoTracker.m_data.VCL100X<<","
		//		<<m_spermImmoTracker.m_data.VSL100X<<","
		//		<<m_spermImmoTracker.m_data.VAP100X<<","
		//		<<m_spermImmoTracker.m_data.ALH100X<<","
		//		<<m_spermImmoTracker.m_data.LIN100X<<","
		//		<<m_spermImmoTracker.m_data.WOB100X<<","
		//		<<m_spermImmoTracker.m_data.STR100X<<","
		//		<<m_spermImmoTracker.m_data.BCF100X<<","
		//		<<m_spermImmoTracker.m_data.MAD100X<<std::endl;*/

		m_fileSave<<SpermNumber<<","
			<<m_spermImmoTracker.m_data.headLength<<","
			<<m_spermImmoTracker.m_data.headWidth<<","
			<<m_spermImmoTracker.m_data.ellipserate<<","
			<<m_SpermMorphology.headWidth<<","
			<<m_SpermMorphology.headWidth<<","
			<<m_SpermMorphology.headEllipticity<<std::endl;

	}

}

void CMainFrame::OnRecordData()// my save data
{

	if (m_output_data_to_file)
	{
		SpermNumber++;
	
		/*std::cout<<"-----Sperm Parameter Measurement-----"<<std::endl;
		std::cout<<std::left<<std::setw(20)<<"VCL: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.VCL<<std::endl;
		std::cout<<std::setw(20)<<"VSL: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.VSL<<std::endl;
		std::cout<<std::setw(20)<<"VAP: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.VAP<<std::endl;
		std::cout<<std::setw(20)<<"ALH: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.ALH<<std::endl;
		std::cout<<std::setw(20)<<"LIN: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.LIN<<std::endl;
		std::cout<<std::setw(20)<<"WOB: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.WOB<<std::endl;
		std::cout<<std::setw(20)<<"STR: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.STR<<std::endl;
		std::cout<<std::setw(20)<<"BCF: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.BCF<<std::endl;
		std::cout<<std::setw(20)<<"MAD: "<<std::setprecision(3)<<m_spermImmoTracker.m_data.MAD<<std::endl;
		std::cout<<std::setw(20)<<"Head Length: "<<std::setprecision(3)<<m_SpermMorphology.headLength*MicronPerPixel100X<<std::endl;
		std::cout<<std::setw(20)<<"Head Width: "<<std::setprecision(3)<<m_SpermMorphology.headWidth*MicronPerPixel100X<<std::endl;
		std::cout<<std::setw(20)<<"Head Ellipticity: "<<std::setprecision(3)<<m_SpermMorphology.headEllipticity<<std::endl;
		std::cout<<std::setw(20)<<"Acrosome Area: "<<std::setprecision(3)<<m_SpermMorphology.acrosomeArea*MicronPerPixel100X*MicronPerPixel100X<<std::endl;
		std::cout<<std::setw(20)<<"Nucleus Area: "<<std::setprecision(3)<<m_SpermMorphology.nucleusArea*MicronPerPixel100X*MicronPerPixel100X<<std::endl;
		std::cout<<std::setw(20)<<"Vacuole Number: "<<std::setprecision(3)<<m_SpermMorphology.vacuoleNumber<<std::endl;
		std::cout<<std::setw(20)<<"Vacuole Area: "<<std::setprecision(3)<<m_SpermMorphology.vacuoleArea*MicronPerPixel100X*MicronPerPixel100X<<std::endl;
		std::cout<<std::setw(20)<<"Midpiece Length: "<<std::setprecision(3)<<m_SpermMorphology.midpieceLength*MicronPerPixel100X<<std::endl;
		std::cout<<std::setw(20)<<"Midpiece Width: "<<std::setprecision(3)<<m_SpermMorphology.midpieceWidth*MicronPerPixel100X<<std::endl;
		std::cout<<std::setw(20)<<"Midpiece Angle: "<<std::setprecision(3)<<m_SpermMorphology.midpieceAngle<<std::endl;
		std::cout<<std::setw(20)<<"Tail Length: "<<std::setprecision(3)<<m_SpermMorphology.tailLength*MicronPerPixel100X<<std::endl;
		std::cout<<std::endl;

		std::cout<<"DATA SAVED.\n";*/

		//////////////////////////////////////////////////////////////////////////
		m_fileSave<<SpermNumber<<","
			<<VCL<<","
			<<VSL<<","
			<<VAP<<","
			<<ALH<<","
			<<LIN<<","
			<<WOB<<","
			<<STR<<","
			<<BCF<<","
			<<MAD<<","
			<<headLength<<","
			<<headWidth<<","
			<<ellipserate<<","//motility
			<<m_spermImmoTracker.m_data.VCL100X<<","
			<<m_spermImmoTracker.m_data.VSL100X<<","
			<<m_spermImmoTracker.m_data.VAP100X<<","
			<<m_spermImmoTracker.m_data.ALH100X<<","
			<<m_spermImmoTracker.m_data.LIN100X<<","
			<<m_spermImmoTracker.m_data.WOB100X<<","
			<<m_spermImmoTracker.m_data.STR100X<<","
			<<m_spermImmoTracker.m_data.BCF100X<<","
			<<m_spermImmoTracker.m_data.MAD100X<<","
			<< m_SpermMorphology.headLength<<","
			<<m_SpermMorphology.headWidth<<","
			<<m_SpermMorphology.headEllipticity<<","
			<<m_SpermMorphology.tailLength<<","
			<<m_SpermMorphology.midpieceLength<<","
			<<m_SpermMorphology.midpieceWidth<<","
			<<m_SpermMorphology.midpieceAngle<<","
			<<m_SpermMorphology.vacuoleNumber<<","
			<<m_SpermMorphology.vacuoleArea<<","
			<<m_SpermMorphology.vacuoleRatio<<std::endl;

		//////////////////////////////////////////////////////////////////////////saveImage

		m_drawMorphologyIsSuccess=false;

		CString file_ext(".png");
		CTime tm_ctime = CTime::GetCurrentTime();
		CString tm_cstring2;
		tm_cstring2.Format("%d",SpermNumber);
		CString file_full_cstring_name = m_save_image_prepath + exceltm_cstring+"_"+ tm_cstring2+ file_ext;
		//CStringA stringa_name(file_full_cstring_name);

		std::vector<int> compression_para;
		compression_para.push_back(cv::IMWRITE_PNG_COMPRESSION);
		compression_para.push_back(0);

		//m_cs_display_color_mat.Lock();
		cv::imwrite(LPCTSTR(file_full_cstring_name), savedImage, compression_para);
		//m_cs_display_color_mat.Unlock();
	}
	else
	{
		std::cout<<"Output to file DISABLED.\n";
		AfxMessageBox(_T("Output to file DISABLED."));
	}

}


//////////////////////////////////////////////////////////////////////////
//	Autofocus button
void CMainFrame::OnAutofocus()
{
	CreateThread(NULL, 0, AutofocusThread, this, 0, NULL);
}

DWORD CMainFrame::AutofocusThread(void *pParameter)
{
	CMainFrame* This = (CMainFrame*) pParameter;
	This->m_current_state = M_MOVING_FOCUS;
	This->Autofocus(30.0, 5.0);
	//This->Autofocus(30.0, 5.0);
	This->m_current_state = M_IDLE;
	return 0;
}

void CMainFrame::Autofocus(double search_range, double stepSearchDirection, cv::Rect search_ROI)
{
	clock_t m_time_start = clock();

	//	First determine the search direction
	double dIniZ = m_prior.Focus().GetPosition();

	const double dStepSearchDirection = stepSearchDirection;	//20

	m_cs_copy_mat.Lock();
	double dIniFocus = Methods::GetFocusScore(m_copyImg, search_ROI);
	m_cs_copy_mat.Unlock();
	std::cout<<"z: "<<m_prior.Focus().GetPosition()<<",   dIniFocus:  "<<dIniFocus<<std::endl;

	m_prior.Focus().MoveUp(dStepSearchDirection);
	m_prior.WaitForFocusStop();
	m_cs_copy_mat.Lock();
	double dUpFocus = Methods::GetFocusScore(m_copyImg, search_ROI);
	m_cs_copy_mat.Unlock();
	std::cout<<"z: "<<m_prior.Focus().GetPosition()<<",   dUpFocus:  "<<dUpFocus<<std::endl;

	m_prior.Focus().MoveDown(2.0*dStepSearchDirection);
	m_prior.WaitForFocusStop();
	m_cs_copy_mat.Lock();
	double dDownFocus = Methods::GetFocusScore(m_copyImg, search_ROI);
	m_cs_copy_mat.Unlock();
	std::cout<<"z: "<<m_prior.Focus().GetPosition()<<",   dDownFocus:  "<<dDownFocus<<std::endl;

	m_prior.Focus().MoveToAbsolute(dIniZ);
	m_prior.WaitForFocusStop();

	if (dIniFocus > dUpFocus && dIniFocus > dDownFocus)
		return;

	bool bIsMovingUp = dUpFocus > dDownFocus ? true : false;
	std::cout<<"is Moving Up? "<<bIsMovingUp<<std::endl;

	//	Calculate next Fibonacci search position (search space is const 900 um)
	int N = 0;
	const double Fib[23]  = {0.,1.,1.,2.,3.,5.,8.,13.,21.,34.,55.,89.,144.,233.,377.,610.,987.,1597.,2584.,4181.,6765.,10946.,17711.};
	double a = 0.;
	double b = search_range > 0 ? search_range : -search_range;
	double x1, x2, y1, y2, dMoveStep;

	//	Determine the minimum N such that Fib(N) >= search_range;
	for (int ii = 0; ii < 23; ii++)
	{
		if (Fib[ii] > search_range)
		{
			N = ii;
			std::cout<<"Fibonacci N: "<<N<<std::endl;
			break;
		}
	}


	x1 = a + Fib[N-2]*(b-a)/Fib[N];
	x2 = b - Fib[N-2]*(b-a)/Fib[N];

	dMoveStep = bIsMovingUp ? x1 : -x1;	// moving up, motor z position gets smaller
	m_prior.Focus().MoveToAbsolute(dIniZ + dMoveStep);
	m_prior.WaitForFocusStop();
	m_cs_copy_mat.Lock();
	y1 = Methods::GetFocusScore(m_copyImg, search_ROI);
	m_cs_copy_mat.Unlock();

	std::cout<<"step: "<<dMoveStep<<" position: "<< m_prior.Focus().GetPosition()<< ",  focus: "<<y1<<std::endl;

	dMoveStep = bIsMovingUp ? x2 : -x2;
	m_prior.Focus().MoveToAbsolute(dIniZ + dMoveStep);
	m_prior.WaitForFocusStop();
	m_cs_copy_mat.Lock();
	y2 = Methods::GetFocusScore(m_copyImg, search_ROI);
	m_cs_copy_mat.Unlock();

	std::cout<<"step: "<<dMoveStep<<" position: "<< m_prior.Focus().GetPosition()<< ",  focus: "<<y2<<std::endl;

	//	iterate until the best focus is found
	for (int ii = N - 1; ii > 2; ii--)
	{
		if  (y1 < y2)
		{
			a = x1;
			x1 = x2;
			y1 = y2;
			x2 = b - Fib[ii-2]*(b-a)/Fib[ii];

			dMoveStep = bIsMovingUp ? x2 : -x2;
			m_prior.Focus().MoveToAbsolute(dIniZ + dMoveStep);
			m_prior.WaitForFocusStop();
			m_cs_copy_mat.Lock();
			y2 = Methods::GetFocusScore(m_copyImg, search_ROI);
			m_cs_copy_mat.Unlock();


			std::cout<<"step: "<<dMoveStep<<" position: "<< m_prior.Focus().GetPosition()<< ",  focus: "<<y2<<std::endl;
		}
		else
		{
			b = x2;
			x2 = x1;
			y2 = y1;
			x1 = a + Fib[ii-2]*(b-a)/Fib[ii];

			dMoveStep = bIsMovingUp ? x1 : -x1;
			m_prior.Focus().MoveToAbsolute(dIniZ + dMoveStep);
			m_prior.WaitForFocusStop();
			m_cs_copy_mat.Lock();
			y1 = Methods::GetFocusScore(m_copyImg, search_ROI);
			m_cs_copy_mat.Unlock();

			std::cout<<"step: "<<dMoveStep<<" position: "<< m_prior.Focus().GetPosition()<< ",  focus: "<<y1<<std::endl;
		}
	}

	clock_t m_time_stop = clock();
	std::cout<<"Total time consumed: "<<(double)(m_time_stop-m_time_start)/(double)CLOCKS_PER_SEC<<" s"<<std::endl;
	std::cout<<"Autofocusing completed!"<<std::endl;
}


// Rotation stage debug functions
void CMainFrame::OnRotateStage()
{
	CreateThread(NULL, 0, RotateStageThread, this, 0, NULL);
}


DWORD CMainFrame::RotateStageThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;

	//This->RotateStage();
	This->RotateStagePurePositionCom();

	return 0;
}

void CMainFrame::RotateStagePurePositionCom()
{
	clock_t timeStart = clock();

	m_current_state = M_ROTATING_STAGE;

	const double STEP_TO_DEGREE_RADIAN = 2.0*CV_PI/8890.;

// 	double theta_radian = pView->m_dlg_tools.m_pump_steps;
// 	__int64 steps = pView->m_dlg_tools.m_pump_steps;

	__int64 steps = 2222.5;	//2222.5

	// wait for the target to be selected
// 	m_rotateTargetSelected = false;
// 	while ( !m_rotateTargetSelected)
// 	{
// 		Sleep(10);
// 		if ( m_current_state != M_ROTATING_STAGE)
// 			return;
// 	}

	

	// calculate target coordinate in the prior stage coordinate after rotation
	double theta_radian = STEP_TO_DEGREE_RADIAN*(double)steps;	// theta positive when counterclockwise

	//std::cout<<"theta_radian: "<<theta_radian<<std::endl;

	double dXzero = 0.0, dYzero = 0.0;
	m_prior.Stage().GetPosition(&dXzero, &dYzero);
//	double dXzero = -14669.4, dYzero = 1153.1;
	
	// account for the image position
// 	dXzero += m_rotateTargetPosPx.x*MicronPerPixel20X;
// 	dYzero += m_rotateTargetPosPx.y*MicronPerPixel20X;
	double dNewX = cos(theta_radian)*(dXzero - RotCenterX) - sin(theta_radian)*(dYzero- RotCenterY) + RotCenterX;// - m_rotateTargetPosPx.x*MicronPerPixel20X;
	double dNewY = sin(theta_radian)*(dXzero - RotCenterX) + cos(theta_radian)*(dYzero- RotCenterY) + RotCenterY;// - m_rotateTargetPosPx.y*MicronPerPixel20X;

	std::cout<<"distance moved x: "<<dNewX-dXzero<<std::endl;
	std::cout<<"distance moved y: "<<dNewY-dYzero<<std::endl;

	m_prior.Stage().SetMaxSpeed(5000);
	m_prior.Stage().MoveToAbsolute(dNewX, dNewY);
	m_prior.WaitForStageStop();



	m_rotate_stage.PowerOn();
	m_rotate_stage.SetVelocity(10000);
	m_rotate_stage.MoveRelative(steps);



	//Sleep(100);
	//SaveImage();

	clock_t timeEnd = clock();
	std::cout<<"Time cost: "<<timeEnd-timeStart<<" ms.\n";

	m_prior.Stage().SetMaxSpeed(2000);
	m_current_state = M_IDLE;
	std::cout<<"Rotation completed.\n";
}


void CMainFrame::RotateStage()
{
	m_current_state = M_ROTATING_STAGE;

	std::deque<cv::Point2f> vSwap1;
	m_ImmoPIDErrorSum.swap(vSwap1);
	std::deque<double> vSwap2;
	m_RotateAnglePIDErrorSum.swap(vSwap2);

	//__int64 steps = pView->m_dlg_tools.m_pump_steps;

	m_RotateMotorIniPosition = m_rotate_stage.GetCurrentPosition();
	m_ImmoIsRotateDone = false;
	//double x_0 = 0.0, y_0 = 0.0;
	m_prior.Stage().GetPosition(&m_PriorX, &m_PriorY);

	if (m_output_data_to_file)
	{
		CString file_ext(".csv");
		CTime tm_ctime = CTime::GetCurrentTime();
		CString tm_cstring = tm_ctime.Format("%Y-%m-%d-%H.%M.%S");
		CString file_full_cstring_name = m_save_file_prepath + tm_cstring + file_ext;
		m_fileSave.open(file_full_cstring_name);
		m_fileSave<<"x,y,degree,prior_x,prior_y,v_visual_x,v_visual_y,v_com_x,v_com<y,v_final_x,v_final_y,radian/s"<<std::endl;
	}



	m_rotateTargetSelected = false;
	while ( !m_rotateTargetSelected)
	{
		Sleep(10);
		if ( m_current_state != M_ROTATING_STAGE)
			return;
	}

	double dx = m_PriorX-RotCenterX;
	double dy = m_PriorY-RotCenterY;
	m_RotateTheta0 = atan2(dy, dx);
	m_RotateRadius = sqrt(dx*dx+dy*dy);

	m_rotate_stage.MoveAtVelocity(0.);
	m_rotate_stage.PowerOn();
}


void CMainFrame::ServoCompensateRotateStage()
{
	//1. Image-based Visual Servo Control
	cv::Mat _copyImg;
	m_cs_copy_mat.Lock();
	m_copyImg.copyTo(_copyImg);
	m_cs_copy_mat.Unlock();

	
	//TrackDarkObject(_copyImg, m_ImmoHeadPt, angle);
	m_spermImmoTracker.track(_copyImg);
// 	if ( m_RotateAngleFilter.size() >= 10)
// 		m_RotateAngleFilter.pop_front();
// 	m_RotateAngleFilter.push_back(angle);

	m_ImmoHeadPt = m_spermImmoTracker.getHeadPositionPx();
	m_ImmoTailPt = m_spermImmoTracker.calcTailPositionPx(15.0);
	double angle_degree = m_spermImmoTracker.getHeadAngleRadian()*180.0/CV_PI;


	double dErrorX = m_ImmoViewCenter.x - m_ImmoHeadPt.x;
	double dErrorY = m_ImmoViewCenter.y - m_ImmoHeadPt.y;

	if (m_ImmoPIDErrorSum.size() >= 5 )
		m_ImmoPIDErrorSum.pop_front();
	m_ImmoPIDErrorSum.push_back(cv::Point2f(dErrorX, dErrorY));

	double dErrorSumX = 0., dErrorSumY = 0.;
	for (int ii = 0; ii < m_ImmoPIDErrorSum.size(); ++ii)
	{
		dErrorSumX += m_ImmoPIDErrorSum[ii].x;
		dErrorSumY += m_ImmoPIDErrorSum[ii].y;
	}

	double V_x_visual, V_y_visual;

	double dKp = 1.0;	//1.0
	double dKi = 0.01;	//0.05
	V_x_visual = dKp*dErrorX + dKi*dErrorSumX;
	V_y_visual = dKp*dErrorY + dKi*dErrorSumY;

//	std::cout<<"Control Output: "<<dOutputX<<" , "<<dOutputY<<std::endl;
// 	dOutputX = dOutputX > 800. ? 800. : dOutputX;	//300
// 	dOutputY = dOutputX > 800. ? 800. : dOutputY;	//300

	//2. Compensate the Translation Induced by Rotation Stage
	double dThetaOutput = 0.0;
	double omega = 0.0;
	double V_x_compensate = 0.0;
	double V_y_compensate = 0.0;
	m_prior.Stage().GetPosition(&m_PriorX, &m_PriorY);
	if (m_rotateTargetSelected == false )
	{
		dThetaOutput= ServoTheta(angle_degree);
	
		double STEP_TO_DEGREE_RADIAN = 2.0*CV_PI/8890.;
		//__int64 steps = pView->m_dlg_tools.m_pump_steps;
		omega = STEP_TO_DEGREE_RADIAN*dThetaOutput;	//speed in rad/s

// 	__int64 currentMotorStep = m_rotate_stage.GetCurrentPosition();
// 	double theta_radian = m_RotateTheta0 + STEP_TO_DEGREE_RADIAN*(double)(currentMotorStep-m_RotateMotorIniPosition);
// 	double V_x_compensate = -omega*m_RotateRadius*sin(theta_radian);
// 	double V_y_compensate = omega*m_RotateRadius*cos(theta_radian);

		//m_prior.Stage().GetPosition(&m_RotatePriorX, &m_RotatePriorY);
		double dx = m_PriorX-RotCenterX;
		double dy = m_PriorY-RotCenterY;
		m_RotateTheta0 = atan2(dy, dx);
		m_RotateRadius = sqrt(dx*dx+dy*dy);
		V_x_compensate = -omega*m_RotateRadius*sin(m_RotateTheta0);
		V_y_compensate = omega*m_RotateRadius*cos(m_RotateTheta0);
	}

	double V_x = V_x_visual + V_x_compensate;
	double V_y = V_y_visual + V_y_compensate;

	m_prior.Stage().MoveAtVelocity(V_x, V_y);

	m_rotate_stage.MoveAtVelocity(dThetaOutput);

// 	if (dThetaOutput != 0.0)
// 	{
// 		std::cout<<"V_x_visual: "<<V_x_visual<<" , V_y_visual: "<<V_y_visual<<std::endl;
// 		std::cout<<"V_x_compen: "<<V_x_compensate<<" , V_y_compen: "<<V_y_compensate<<std::endl;
// 		std::cout<<"V_x_final : "<<V_x<<"  ,  V_y_final : "<<V_y<<std::endl;
// 		std::cout<<"RotateR   : "<<m_RotateRadius<<" um\n";
// 		//std::cout<<"angle_deg : "<<angle<<" degree\n\n";
// 		std::cout<<std::endl;
// 	}
	if (m_output_data_to_file)
	{
		m_fileSave<<m_ImmoHeadPt.x<<","<<m_ImmoHeadPt.y<<","<<angle_degree<<","<<m_PriorX<<","<<m_PriorY
			<<","<<V_x_visual<<","<<V_y_visual<<","<<V_x_compensate<<","<<V_y_compensate<<","<<V_x<<","<<V_y<<","<<omega<<std::endl;
	}
}


double CMainFrame::ServoTheta(double current_angle_degree)
{
	//1. average filtering the calculated angle
	double dError = m_ImmoRotationTargetDegree - m_ImmoHeadAngleDegree;

	if (m_RotateAnglePIDErrorSum.size() > 5)
		m_RotateAnglePIDErrorSum.pop_front();
	m_RotateAnglePIDErrorSum.push_back(current_angle_degree);
	double dErrorSum = 0.0;
	for (int ii = 0; ii < m_RotateAnglePIDErrorSum.size(); ++ii)
	{
		dErrorSum += m_RotateAnglePIDErrorSum[ii];
	}

	double dKp = 15.0;	// 5
	double dKi = 0.05;	// 0.05

	double dThetaOut = dKp*dError + dKi*dErrorSum;

	//std::cout<<++m_thetaCount<<", angle: "<<current_angle_degree<<", output: "<<dThetaOut<<std::endl;

	if (abs(dError) < 5.0)
	{
		m_ImmoIsRotateDone = true;
		m_rotate_stage.MoveAtVelocity(0.0);
		std::cout<<"Theta servo done! "<<std::endl;
	}

	return dThetaOut;

}

