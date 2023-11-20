// MainFrm.cpp : implementation of the CMainFrame class
//
//#include "vld.h"
#include "stdafx.h"
#include "RoboticBiopsy.h"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "MainFrm.h"
#include <opencv2/videoio/videoio_c.h>
#include "inference.h"
#include "Methods.h"
//#include "phidget22.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
std::string projectBasePath = "D:/ZYB/ball_first_weights/"; // Set your ultralytics base path

bool runOnGPU = true;

//
// Pass in either:
//
// "yolov8s.onnx" or "yolov5s.onnx"
// 
// To run Inference with yolov8/yolov5 (ONNX)
//

// Note that in this example the classes are hard-coded and 'classes.txt' is a place holder.
Inference inf(projectBasePath + "best.onnx", cv::Size(640, 640), "D:/ZYB/ball_first_weights/classes.txt", runOnGPU);
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE_VOID(WM_CLOSE, Close)
	ON_MESSAGE(WM_IMAGE_ACQUIRED, OnImageAcquired)
	//ON_MESSAGE(WM_CLFEEDBACK_ACQUIRED, OnCLFeedbackAcquired)
	ON_WM_MOUSEWHEEL()
	//ON_WM_LBUTTONDBLCLK()
	//ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	std::string projectBasePath = "D:/ZYB/ball_first_weights/"; // Set your ultralytics base path

	bool runOnGPU = true;
	//Inference inf(projectBasePath + "best.onnx", cv::Size(640, 640), "D:/ZYB/ball_first_weights/classes.txt", runOnGPU);
	// TODO: add member initialization code here
	m_current_state = M_IDLE;

	// Images
	m_image_is_loaded = false;
	m_temp_img = cvCreateImage(cvSize(nImgWidth*1.5, nImgHeight*1.5), IPL_DEPTH_8U, 1);
	m_raw_img = cvCreateImage(cvSize(nImgWidth, nImgHeight), IPL_DEPTH_8U, 1);
	m_save_image_prepath = _T("D:\\Simon_workspace\\Focus_Moving_System\\tmpImage\\");
	m_save_video_prepath = _T("D:\\Simon_workspace\\Focus_Moving_System\\tmpVideo\\");
	m_is_recording_video = false;
	m_is_video_writer_created = false;

	m_drawDelayFrames = 0;


	// Manipulator Initialization
	if (ENABLE_MANIP_LEFT)
	{
		CString manip_com = _T("COM8");
		//m_manip_left.Initialize("COM3");
		m_manip_left.Initialize(manip_com);
		m_manip_left.SetVelocity(500);   // default 2000

	}

	if (ENABLE_MANIP_RIGHT)
	{
		// 		CString manip_com = "COM3";
		// 		m_manip_right.Initialize("COM3");
		// 		m_manip_right.SetVelocity(2000);  /// default 2000
				//m_manip_right.Initialize(4);	//3
	}


	// Prior stage initialization
	if (ENABLE_PRIOR_STAGE)
	{
		if(m_prior.InitPriorStage(4))	//COM 4
			std::cout<<"Successfully connected to Prior stage."<<std::endl;
		else
		{
			std::cout<<"Fail to connect to Prior stage."<<std::endl;
			AfxMessageBox(_T("Fail to connect to Prior stage."));
		}
	}

	// Eppendorf Pump Initialization
	m_is_pump_motor_start = false;

	if (m_pump_motor.Initialize())
	{
		m_is_pump_motor_start = true;
	}
	std::cout << "Pump motor state: " << m_is_pump_motor_start << std::endl;

	

	//Create Thread for grab image
	m_aquiring_images = true;
	CreateThread(NULL, 0, GrabImageThread, this, 0, NULL);
	//Methods::tools::PrintTime();  1

	//Help Strings
	m_HelpCase1 = "Contact detection...\r\n"
		"Please bring the pipette near and focus on the dish bottom\r\n"
		"Then double click on the pipette tip.";

	m_HelpCase2 = "Pipette tip clicked.\r\n"
		"Detecting contact...";

	m_HelpCase3 = "Immobilizing sperm...\r\n"
		"Please double click on sperm head.";

	m_HelpCase4 = "Unable to immobilizing sperm.\r\n"
		"Please do contact detection first.";

	m_HelpCase6 = "Contact successfully found.";

	m_HelpCase7 = "Contact detection failed.";

	m_HelpCase8 = "Progress Stopped.";

	m_HelpCase9 = "Servoing sperm head to center...";

	m_HelpCase10 = "Sperm centered.\r\n"
		"Performing sperm immobilization...";

	m_HelpCase11 = "Sperm immobilization completed.";

	m_HelpCase12 = "Select sperm for 100X tracking";

	m_HelpCase13 = "Servoing sperm 100X";
}

CMainFrame::~CMainFrame()
{
	cvReleaseImage(&m_raw_img);
	//m_copyImg.release();
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
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
	CRoboticBiopsyView* pView = (CRoboticBiopsyView*)GetActiveView();

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
	if (m_camera.IsConnected())
	{
		if (!m_camera.IsStarted())
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

/************************************************************************/
/* OnMouseWheel 是 MFC 中的一个预定义的函数，用于处理鼠标滚轮的事件。
   函数的参数包括：nFlags（标志位，描述了在发生鼠标滚轮事件时哪些键（如 Shift、Ctrl）被按下），
   zDelta（滚轮的滚动量，向前滚则为正，向后滚则为负），pt（一个 CPoint 对象，描述了鼠标事件发生时的鼠标位置）。                                                           */
/************************************************************************/
BOOL CMainFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	pView = (CRoboticBiopsyView*)GetActiveView();

	CString step_string;
	pView->m_dlg_tools.m_combo_focus_step_size.GetWindowText(step_string);
	m_focus_step_size = atof(step_string);
	cout << "Focus Step Size: " << m_focus_step_size << endl;
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



// Mouse left button double click to select initial position of sperm or pipette
/*void CMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	switch (m_current_state)
	{

	case M_CONTACT_DETECTION:
		{
		m_PipInImagePos = cv::Point(point.x, point.y);
		std::cout << "Double Clicked Pos: " << m_PipInImagePos << std::endl;
		m_ContactIsPipSelected = true;
		std::cout << "Pipette Selected.\n";

		}break;
		

		default:
			break;
	}


	CFrameWnd::OnLButtonDblClk(nFlags, point);
}*/

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
	CMainFrame* This = (CMainFrame*)pParameter;  // instance pointer
	This->MoveManipLeft(This->m_manip_axis_left, This->m_manip_disp_left);
	return 0;
}

// Worker function in thread
void CMainFrame::MoveManipLeft(int axis, float displacement)
{
	switch (axis)
 	{		
 	case 1://move x axis
 	{
 		m_manip_position_left = m_manip_left.GetPosition();//get current position
		std::cout << "x: " << m_manip_position_left.x << ", y: " << m_manip_position_left.y << ", z: " << m_manip_position_left.z << std::endl;
 		m_manip_left.Move(m_manip_position_left.x + displacement, m_manip_position_left.y, m_manip_position_left.z);
 		break;
 	}
 	case 2: //move y axis
 	{	
 		m_manip_position_left = m_manip_left.GetPosition();//get current position
		std::cout << "x: " << m_manip_position_left.x << ", y: " << m_manip_position_left.y << ", z: " << m_manip_position_left.z << std::endl;
 		m_manip_left.Move(m_manip_position_left.x, m_manip_position_left.y + displacement, m_manip_position_left.z);
 		break;
 	}
 	case 3: //move z axis
 	{
 		m_manip_position_left = m_manip_left.GetPosition();//get current position
		std::cout << "x: " << m_manip_position_left.x << ", y: " << m_manip_position_left.y << ", z: " << m_manip_position_left.z << std::endl;
 		m_manip_left.Move(m_manip_position_left.x, m_manip_position_left.y, m_manip_position_left.z + displacement);
 		break;
 	}
 	default:
 		break;
 	}	
}

void CMainFrame::WaitForManipLeftStop()
{
	while(m_manip_left.is_moving){;}
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
	This->PumpMotorControl();
	return 0;
}
//pid
void CMainFrame::PumpMotorControl()
{
	m_current_state = M_ASPIRATE;
	int m_target_destination_x = 640;
	int m_position_error = 0;
	int m_position_error_sum = 0;
	int m_in_position_cnt = 0;
	float m_PID_P = 0.00005;
	float m_PID_I = 0.0001;
	bool m_is_embryo_in = FALSE;
	while (m_current_state == M_ASPIRATE)
	{
		//printf("m_current_position.x=%d, m_current_position.y=%d\n", m_current_position.x, m_current_position.y);
		m_pump_motor.SetVelocity(1000.0);
		m_pump_motor.MovetoAbsolute(640);
		double v = m_pump_motor.GetCurrentVelocity();
		if (m_current_position.x > 0)
		{
			m_position_error = m_target_destination_x - m_current_position.x;
			m_position_error_sum = m_position_error_sum + m_position_error;
			if (m_target_destination_x > m_current_position.x)
			{
				m_pump_motor.PowerOn();
				m_pump_motor.MoveRelative(m_PID_P*m_position_error*m_position_error);
				//Sleep(100);
				std::cout << "is injecting" << std::endl;
			}
			else
			{
				m_pump_motor.PowerOn();
				m_pump_motor.MoveRelative(-m_PID_P * m_position_error*m_position_error);
				//Sleep(100);
				std::cout << "is sucking" << std::endl;
			}
			m_pump_motor.PowerOff();
			//std::cout << "position error: " << m_position_error << std::endl;
			if (abs(m_position_error) < 10)
				m_in_position_cnt++;
			else
				m_in_position_cnt = 0;//reset the stable check
			if (m_in_position_cnt > 3)//stability check
				break;
		}
		
	}
}

//////////////////////////////////////////////////////////////////////
// Basler and Image Acquisition 

void CMainFrame::StartCamera()
{
	if (!m_camera.IsStarted())
	{
		m_camera.Start();

		// Initialize the pointer to the View
		pView = (CRoboticBiopsyView*)GetActiveView();
	}
}

void CMainFrame::AcquireImage()
{
	while (m_camera.IsStarted())
	{
		// enables the viewer to start drawing images to screen
		pView->m_is_displaying_image = true;
		m_image_is_loaded = true;

		if (m_camera.AcquireImage())
		{
			//Methods::tools::PrintTime();
			m_temp_img->imageData = m_camera.ImageData();
			cvResize(m_temp_img, m_raw_img);
			string filename = "D:\\ZJC\\raw.png"; // 保存文件的路径和文件名 
			imwrite(filename, cvarrToMat(m_raw_img)); // 将二值化图像保存到文件中
			//m_camera.ReleaseMemory();
			//cvSetData(m_raw_img,m_camera.ImageData(),1920);
			// Message sent to trigger OnIdle to run each time an image has been retrieved
			PostMessage(WM_IMAGE_ACQUIRED, 0, 0);
		}
	}
}

DWORD CMainFrame::GrabImageThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;  // instance pointer
	This->StartCamera();
	//Methods::tools::PrintTime(); 1
	This->AcquireImage();
	//Methods::tools::PrintTime(); none
	return 0;
}

// Record video
void CMainFrame::OnRecordVideo()
{
	m_record_video_frame_no = 0;
	if (!m_is_recording_video)
	{
		CString file_ext(".avi");
		CTime tm_ctime = CTime::GetCurrentTime();
		CString tm_cstring = tm_ctime.Format("%Y-%m-%d-%H.%M.%S");
		CString file_full_cstring_name = m_save_video_prepath + tm_cstring + file_ext;
		m_video_writer.open(LPCTSTR(file_full_cstring_name), CV_FOURCC('X', 'V', 'I', 'D'), 33.0, cv::Size(nImgWidth, nImgHeight), 1);
		if (m_video_writer.isOpened())
		{
			m_is_recording_video = true;
			pView->m_view_is_recording_video = true;
			CreateThread(NULL, 0, RecordVideoThread, this, 0, NULL);
		}
		else
		{
			std::cout << "Failed to create video writer!" << std::endl;
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
	while (m_is_recording_video)
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
	std::cout << "Record video done!" << std::endl;
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
	
	pView = (CRoboticBiopsyView*)GetActiveView();

	if (pView->m_is_displaying_image)
	{
		//m_copyImg.release();
		m_cs_copy_mat.Lock();
		m_copyImg = cv::cvarrToMat(m_raw_img, true);
		//int width = m_raw_img->width;
		//int height = m_raw_img->height;
		//printf("width=%d,height=%d\n",width,height); 1280 800
		//int width = m_copyImg.cols;
		//int height = m_copyImg.rows;
		//printf("width=%d,height=%d\n",width,height); 1200 800
		// Gray to Color
		cv::cvtColor(m_copyImg, m_displayColorImg, cv::COLOR_GRAY2BGR);
		//m_displayColorImg = m_copyImg;

		m_cs_copy_mat.Unlock();

		HANDLE m_threadHandle;
		m_threadHandle=CreateThread(NULL, 0, ClosedLoopControlFeedbackThread, this, 0, NULL);
		WaitForSingleObject(m_threadHandle, INFINITE);
		//CloseHandle(m_threadHandle);
		m_threadHandle = nullptr;

		DrawImageResults();

		if (m_is_recording_video)
		{
			m_cs_display_color_mat.Lock();
			m_displayColorImg.copyTo(m_recordImg);
			m_cs_display_color_mat.Unlock();

			m_cs_record.Lock();
			m_record_deque.push_back(m_recordImg);
			m_cs_record.Unlock();

			if (++m_record_video_frame_no >= 165)//record for 165 frames = 5 seconds
				;
			//m_is_recording_video = false;
		}
	}


	//Main image window
	RECT rect;
	rect.left = pView->CameraDisplayArea.left + 2;
	rect.right = pView->CameraDisplayArea.right - 2;
	rect.bottom = TopDlgabarHeight + pView->CameraDisplayArea.bottom - 2;
	rect.top = TopDlgabarHeight + pView->CameraDisplayArea.top + 2;
	InvalidateRect(&rect);

	return 0;
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

// CMainFrame message handlers

// Draw image processing results
void CMainFrame::DrawImageResults()
{
	//cv::Rect m_ROI(240, 350, 800, 100);
	//cv::Point p1(640, 350); // 标记线起点坐标
	//cv::Point p2(640, 450); // 标记线终点坐标
	//m_cs_display_color_mat.Lock();
	//cv::circle(m_displayColorImg, cv::Point(640, 400), 1, BLACK, 2);
	//cv::rectangle(m_displayColorImg, m_ROI, WHITE);
	//cv::line(m_displayColorImg, p1, p2, MAGENTA, 2);
	//cv::putText(m_displayColorImg, "target_pos", p1, cv::FONT_HERSHEY_COMPLEX_SMALL, 0.7, MAGENTA, 1);
	//m_cs_display_color_mat.Unlock();
	//std::cout << "m_current_state:" << m_current_state << std::endl;
	if ((m_current_state == M_CONTACT_DETECTION) && m_ContactIsPipSelected)
	{
		int iROIWidth = 50, iROIHeight = 100;   // 50 100
		//cv::Rect tipROI(m_ContactPipPresetPos.x, m_ContactPipPresetPos.y - iROIHeight / 2, iROIWidth, iROIHeight);
		cv::Rect tipROI(m_PipInImagePos.x, m_PipInImagePos.y - iROIHeight / 2, iROIWidth, iROIHeight);
		m_cs_display_color_mat.Lock();
		cv::rectangle(m_displayColorImg, tipROI, cv::Scalar(255, 0, 0));
		cv::circle(m_displayColorImg, m_PipInImagePos, 1, cv::Scalar(0, 0, 255), 2);

		m_cs_display_color_mat.Unlock();
	}
	if (m_current_state == M_ASPIRATE)
	{
		int iROIWidth = 50, iROIHeight = 50;   // 50 100
		//cv::Rect tipROI(m_ContactPipPresetPos.x, m_ContactPipPresetPos.y - iROIHeight / 2, iROIWidth, iROIHeight);
		cv::Rect SpermROI(m_current_position.x - iROIHeight / 2, m_current_position.y - iROIHeight / 2, iROIWidth, iROIHeight);
		m_cs_display_color_mat.Lock();
		cv::rectangle(m_displayColorImg, SpermROI, cv::Scalar(255, 0, 0));
		//cv::circle(m_displayColorImg, m_current_position, 1, cv::Scalar(0, 0, 255), 2);

		m_cs_display_color_mat.Unlock();
	}

}

// Control algorithms in real time;
DWORD CMainFrame::ClosedLoopControlFeedbackThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;
	This->ClosedLoopControlFeedback();
	return 0;
}

void CMainFrame::ClosedLoopControlFeedback()
{

	if (m_current_state == M_ASPIRATE) {
		//std::cout << "torch"<<std::endl;

		ServoSpermHeadCenter();

	}
	else
	{
		//Methods::Vision::init_model();
		//std::cout << "np_torch"<<std::endl;
	}
	
	//m_multi_tracker->track(_copyImg);
	//printf("m_current_position.x=%d, m_current_position.y=%d\n", m_current_position.x, m_current_position.y);

}

//////////////////////////////////////////////////////////////////////////
// Sperm Immobilization functions
void CMainFrame::ServoSpermHeadCenter()
{
	cv::Mat _copyImg;
	m_cs_copy_mat.Lock();
	//_copyImg.release();
	m_copyImg.copyTo(_copyImg);
	//_copyImg = m_copyImg;
	m_cs_copy_mat.Unlock();

	m_current_position = Methods::Vision::FindContours_v8(_copyImg);
	
	//printf("m_current_position.x=%d, m_current_position.y=%d\n", m_current_position.x, m_current_position.y);
}



//////////////////////////////////////////////////////////////////////////
// Track Multiple Sperms in real time



//////////////////////////////////////////////////////////////////////////
// Focus Moving
//BOOL CMainFrame::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
//{
//	// TODO: Add your message handler code here and/or call default
//	pView = (CRoboticBiopsyView*)GetActiveView();
//
//	CString step_string;
//	pView->m_dlg_tools.m_combo_focus_step_size.GetWindowText(step_string);
//	m_focus_step_size = atof(step_string);
//	cout << "Focus Step Size: " << m_focus_step_size << endl;
//	m_mouse_wheel_zDelta = zDelta;
//
//	CreateThread(NULL, 0, MouseWheelFocusThread, this, 0, NULL);
//
//	return CFrameWnd::OnMouseWheel(nFlags, zDelta, pt);
//}
//
//DWORD WINAPI CMainFrame::MouseWheelFocusThread(void* pParameter)
//{
//	CMainFrame* This = (CMainFrame*)pParameter;
//
//	if (ENABLE_PRIOR_STAGE)
//	{
//		if (This->m_mouse_wheel_zDelta > 0)
//		{
//			This->m_prior.Focus().MoveUp(This->m_focus_step_size);
//			This->m_prior.WaitForFocusStop();
//		}
//		else
//		{
//			This->m_prior.Focus().MoveDown(This->m_focus_step_size);
//			This->m_prior.WaitForFocusStop();
//		}
//	}
//
//	return 0;
//}

void CMainFrame::OnFocusMoving()
{
	cout << "Focus Moving Btn HIT!" << endl;

	pView = (CRoboticBiopsyView*)GetActiveView();

	CString step_string;
	pView->m_dlg_tools.m_combo_focus_step_size.GetWindowText(step_string);
	m_focus_step_size = atof(step_string);
	cout << "Focus Step Size: " << m_focus_step_size << endl;

	CreateThread(NULL, 0, FocusMovingThread, this, 0, NULL);
}

DWORD CMainFrame::FocusMovingThread(void* pParameter)
{
	CMainFrame* This = (CMainFrame*)pParameter;

	// 1 for MoveUp, -1 for MoveDown
	int mouse_wheel_zDelta = 1;

	for (int i = 0; i < 3; i++)
	{
		This->SavingFocusMovingDepthImg(i);

		if (ENABLE_PRIOR_STAGE)
		{
			if (mouse_wheel_zDelta > 0)
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

		cout << "Z-Focus Moving End. Ready for next move...\n" << endl;

	}


	return 0;
}

void CMainFrame::FocusMoving(int time)
{

	SavingFocusMovingDepthImg(time);
}

//void CMainFrame::MoveZFocus(int mouse_wheel_zDelta, void* pParameter)
//{
//	CMainFrame* This = (CMainFrame*)pParameter;
//
//	if (ENABLE_PRIOR_STAGE)
//	{
//		if (mouse_wheel_zDelta > 0)
//		{
//			This->m_prior.Focus().MoveUp(This->m_focus_step_size);
//			This->m_prior.WaitForFocusStop();
//		}
//		else
//		{
//			This->m_prior.Focus().MoveDown(This->m_focus_step_size);
//			This->m_prior.WaitForFocusStop();
//		}
//	}
//}

void CMainFrame::SavingFocusMovingDepthImg(int time)
{
	// change int time into CString
	CString time_str;
	time_str.Format(_T("%d"), time);

	// change double focus_step_size into CString
	CString focus_step_size_str;
	focus_step_size_str.Format(_T("%f"), m_focus_step_size);

	// Saving Image
	//////////////////////////////////////////////////////////////////////
	CString file_ext(".png");
	CTime tm_ctime = CTime::GetCurrentTime();
	CString tm_cstring = tm_ctime.Format("%Y-%m-%d-%H.%M.%S");

	CString times_step = "_timeswith";

	CString focus_step = "_focusstep";

	CString file_full_cstring_name = m_save_image_prepath + tm_cstring + times_step + time_str + focus_step + focus_step_size_str + file_ext;
	//CStringA stringa_name(file_full_cstring_name);
	cout << "File saved as: " << file_full_cstring_name << "!\n" << endl;

	std::vector<int> compression_para;
	compression_para.push_back(cv::IMWRITE_PNG_COMPRESSION);
	compression_para.push_back(0);

	m_cs_display_color_mat.Lock();
	cv::imwrite(LPCTSTR(file_full_cstring_name), m_displayColorImg, compression_para);
	m_cs_display_color_mat.Unlock();
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
	while (!m_ContactIsPipSelected)
	{
		Sleep(10);
		if (m_current_state != M_CONTACT_DETECTION)
			return;
	}

	m_HelpString = m_HelpCase2;
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);

	//2. position the pipette to the preset position
	//PositionPipetteToPreSetPos();

	//3. detect contact in image
	m_ContactIsSuccessful = DetectContactImage();

	if (m_ContactIsSuccessful)
	{
		m_HelpString = m_HelpCase6;
		m_ContactedManipPos = m_manip_left.GetPosition();
	}
	else
	{
		m_HelpString = m_HelpCase7;
	}
	
	pView->m_dlg_tools.m_help_string.SetWindowTextA(m_HelpString);

	m_current_state = M_IDLE;

}

void CMainFrame::PositionPipetteToPreSetPos()
{
	// m_ContactPipPresetPos: Double Clicked
	float x_disp = (float)(m_ContactPipPresetPos.x - m_PipInImagePos.x)*MicronPerPixel20X;
	float y_disp = (float)(m_ContactPipPresetPos.y - m_PipInImagePos.y)*MicronPerPixel20X;

	// 0 0
	std::cout << "m_ContactPipPresetPos X: " << m_ContactPipPresetPos.x << ", Y: " << m_ContactPipPresetPos.y << std::endl;
	std::cout << "X_disp: " << x_disp << ", Y_disp: " << y_disp << std::endl;
	std::cout << "Manip X: " << m_manip_left.GetPosition().x << ", Y: " << m_manip_left.GetPosition().y << ", Z: " << m_manip_left.GetPosition().z << std::endl;

	//m_manip_left.MoveRelative(x_disp, y_disp); ??
	m_manip_left.Move(x_disp, m_manip_left.GetPosition().y, m_manip_left.GetPosition().z);
	
	float tmpx = m_PipInImagePos.x;
	float tmpy = m_PipInImagePos.y;

	WaitForManipLeftStop();
	m_ContactPipPresetPos = m_PipInImagePos;
}

// The image processing thread
bool CMainFrame::DetectContactImage()
{
	bool bSuccessful = false;
	int iROIWidth = 50, iROIHeight = 100;  // 50 100
	int iDebugCount = 0, iMaxDebugCount = 50;
	const int iDiffThreshold = 11000;		//15000
	const float dTipSlideThreshold = 0.0;	// 5.6 pixels = 2 um	//3.0
	float fManipStep = 5.;
	cv::Rect tipROI(m_PipInImagePos.x, m_PipInImagePos.y - iROIHeight / 2, iROIWidth, iROIHeight);
	int iPipROIWidth = m_PipInImagePos.x + 100;
	int iPipROIHeight = 200;
	cv::Rect tipDetectionROI(0, m_PipInImagePos.y - iPipROIHeight / 2, iPipROIWidth, iPipROIHeight);

	cv::Mat firstImg, thisImg, diffImg, wholeImg;
	m_cs_copy_mat.Lock();
	m_copyImg(tipROI).copyTo(firstImg);
	m_cs_copy_mat.Unlock();
	cv::GaussianBlur(firstImg, firstImg, cv::Size(5, 5), 1.0);

	// show firstImg
	cv::imshow("FirstImg", firstImg);
	cv::waitKey(0);

	cv::Point tipPtLast(-1, -1);
	cv::Point tipPt;

	double dTipSlidingDist = 0.;

	while (m_current_state == M_CONTACT_DETECTION)
	{
		m_cs_copy_mat.Lock();
		m_copyImg(tipROI).copyTo(thisImg);
		m_copyImg.copyTo(wholeImg);
		m_cs_copy_mat.Unlock();

		// Image Diff method
		cv::GaussianBlur(thisImg, thisImg, cv::Size(5, 5), 1.0);
		cv::absdiff(firstImg, thisImg, diffImg);
		int iPixelSum = cv::sum(diffImg).val[0];
		std::cout << "iPixelSum: " << iPixelSum << std::endl;

		// Tip position method
		tipPt = Methods::detectPipetteTipPoint(wholeImg, 0.75, tipDetectionROI);
		m_PipInImagePos = tipPt;
		//std::cout<<"tipPt: "<<tipPt.x<<" , "<<tipPt.y<<std::endl;
		if (tipPtLast.x == -1 && tipPtLast.y == -1)
		{
			tipPtLast = tipPt;
		}
		//dTipSlidingDist = Methods::CalcDistance(tipPt, tipPtLast);
		dTipSlidingDist = tipPt.x - tipPtLast.x;
		dTipSlidingDist = dTipSlidingDist > 50 ? 0 : dTipSlidingDist;    // threshold: 30(older)
		std::cout << "Sliding distance: " << dTipSlidingDist << std::endl;

		if (iPixelSum > iDiffThreshold && dTipSlidingDist >= dTipSlideThreshold)
		{
			bSuccessful = true;

			sManipPosition tempPos;
			tempPos = m_manip_left.GetPosition();

			//m_manip_left.MoveRelative(3, -1.*fManipStep);
			MoveManipLeft(3, -1.*fManipStep);
			WaitForManipLeftStop();

			break;
		}
		else
		{
			sManipPosition tempPos;
			tempPos = m_manip_left.GetPosition();

			//m_manip_left.MoveRelative(3, fManipStep);
			MoveManipLeft(3, fManipStep);
			WaitForManipLeftStop();
		}

		if (++iDebugCount >= iMaxDebugCount)
		{
			std::cout << "Contact detection failed after " << iMaxDebugCount << " times.\n";
			break;
		}

		tipPtLast = tipPt;

	}

	return bSuccessful;
}


//////////////////////////////////////////////////////////////////////////
// Set and Move to Dish and Gel Position



//////////////////////////////////////////////////////////////////////////
// Record and Move to Collection Droplet




//////////////////////////////////////////////////////////////////////////
//Nikon control
//	SwitchObjective button




//////////////////////////////////////////////////////////////////////////
//	Autofocus button
