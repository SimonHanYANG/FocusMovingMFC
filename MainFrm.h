
// MainFrm.h : interface of the CMainFrame class
//

#pragma once

//////////////////////////////////////////////////////////////////////
// general C++ header files
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>
#include <deque>
#include <iomanip>
#include <ctime>

using namespace std;
//////////////////////////////////////////////////////////////////////
// Project head files
#include "opencv2/highgui.hpp"
#include "RoboticBiopsyDoc.h"
#include "RoboticBiopsyView.h"
#include "opencv2/core/types_c.h"
#include "opencv2/videoio.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp>
//////////////////////////////////////////////////////////////////////
// Class header files

#include "ParameterDefinition.h"
#include "OpenCV.h"
#include "Methods.h"
#include "PriorStage.h"
#include "SutterManip.h"
#include "Camera.h"
#include "EppendorfPump.h"
#include "opencv2/core/types_c.h"
#include "inference.h"

//extern Inference inf;

//////////////////////////////////////////////////////////////////////
// Messeage for Camera 
// Message fired by the GrabImageThread when image has been acquired from the camera
const UINT WM_IMAGE_ACQUIRED = WM_USER + 100;



class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct) ;

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL OnIdle(LONG lCount);

// Implementation
	
	StateMap m_current_state;
	
	CRoboticBiopsyView* pView;	// pointer to viewer

	//////////////////////////////////////////////////////////////////////////
	// Basler Camera function and variables
	Camera m_camera;
	bool m_aquiring_images;							// true when acquiring images
	bool m_image_is_loaded;

	void StartCamera();
	void AcquireImage();
	static DWORD WINAPI GrabImageThread(void* pParameter);

	afx_msg LRESULT OnImageAcquired(WPARAM wParam, LPARAM lParam);

	IplImage* m_raw_img;			//raw image data; 8-bit, single channel
	IplImage* m_temp_img;
	cv::Mat m_displayColorImg;	    //display image;  8-bit, three channels 
	cv::Mat m_copyImg;				//copied image for image processing
	cv::Mat m_recordImg;			//for RecordVideo; copied from m_display_color_mat
	CCriticalSection m_cs_copy_mat;				//Critical section for protecting m_copy_mat for image processing
	CCriticalSection m_cs_display_color_mat;
	CCriticalSection m_cs_record;
	cv::Point m_current_position;

	// For SaveImage
	void SaveImage();
	CString m_save_image_prepath;

	// For RecordVideo
	CString m_save_video_prepath;
	bool m_is_recording_video;
	cv::VideoWriter m_video_writer;
	bool m_is_video_writer_created;
	void OnRecordVideo();
	static DWORD WINAPI RecordVideoThread(void* pParameter);
	void RecordVideo();
	unsigned int m_record_video_frame_no;


	std::ofstream m_fileSave;
	CString m_save_file_prepath;
	bool m_output_data_to_file;

	clock_t m_time_start, m_time_stop;
	std::deque<cv::Mat> m_record_deque;	//image deque for recording video

	//////////////////////////////////////////////////////////////////////////
	// Draw Image processing results
	void DrawImageResults();
	int m_drawDelayFrames;

	static DWORD WINAPI ClosedLoopControlFeedbackThread(void* pParameter);
	void ClosedLoopControlFeedback();
	//static DWORD WINAPI ClosedLoopControlThread(void* pParameter);
	//void ClosedLoopControl();

	//////////////////////////////////////////////////////////////////////////
	// Manipulator function and variables
 	SutterManip m_manip_left;
// 	SutterManip m_manip_right;

	float m_manip_disp_left;
	int m_manip_axis_left;
	sManipPosition m_manip_position_left;

	void OnMoveManipLeft(int axis, float displacement);
	static DWORD WINAPI MoveManipLeftThread(void* pParameter);
	void MoveManipLeft(int axis, float displacement);
	void WaitForManipLeftStop();
	


	//////////////////////////////////////////////////////////////////////////



	// Prior stage variables and methods
	PriorStage m_prior;


	//////////////////////////////////////////////////////////////////////////
	// Override Close
	afx_msg void Close();
	
	//////////////////////////////////////////////////////////////////////////
	
	// Eppendorf pump control
	EppendorfPump m_pump_motor;
	bool m_is_pump_motor_start;
	void OnPumpMotorControl();
	static DWORD WINAPI PumpMotorControlThread(void* pParameter);
	void PumpMotorControl();
	

	cv::Mat savedImage;

	float VCL;
	float VSL;
	float VAP;
	float ALH;
	float LIN;
	float WOB;
	float STR; 
	float BCF;
	float MAD;
	float ellipserate;
	float headLength;
	float headWidth;
	CString exceltm_cstring;


	//////////////////////////////////////////////////////////////////////////
	// Funcitons and algorithms
	
	// Contact Detection of pipette tip
	void OnContactDetection();
	static DWORD WINAPI ContactDetectionThread(void* pParameter);
	void ContactDetection();
	void PositionPipetteToPreSetPos();
	bool DetectContactImage();	// combines the first and second method
	void ServoSpermHeadCenter();
	cv::Point m_ContactPipPresetPos;
	cv::Point m_PipInImagePos;
	bool m_ContactIsPipSelected = false;
	bool m_ContactIsSuccessful;
	sManipPosition m_ContactedManipPos;

	//////////////////////////////////////////////////////////////////////////
	//	Focus Moving for Depth Imaging Saving
	void OnFocusMoving();
	static DWORD WINAPI FocusMovingThread(void* pParameter);
	void CMainFrame::FocusMoving(int time);
	//void CMainFrame::MoveZFocus(int mouse_wheel_zDelta, void* pParameter);
	void CMainFrame::SavingFocusMovingDepthImg(int time);



	//////////////////////////////////////////////////////////////////////////
	//	Autofocus with Fibonacci algorithm on whole image
	

	

	//////////////////////////////////////////////////////////////////////////
	//	Deposit sperm into gel
	//	Movement functions
	


	//////////////////////////////////////////////////////////////////////////
	// Help String
	CString m_HelpString;
	CString m_HelpCase1, m_HelpCase2, m_HelpCase3, m_HelpCase4, m_HelpCase5;
	CString m_HelpCase6, m_HelpCase7, m_HelpCase8, m_HelpCase9, m_HelpCase10;
	CString m_HelpCase11, m_HelpCase12, m_HelpCase13, m_HelpCase14, m_HelpCase15;



public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	
	// Mouse Wheel for focus adjustment
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	static DWORD WINAPI MouseWheelFocusThread(void* pParameter);
	double m_focus_step_size;
	short m_mouse_wheel_zDelta;

	//afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


