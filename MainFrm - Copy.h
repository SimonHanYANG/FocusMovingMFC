
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
//////////////////////////////////////////////////////////////////////
// Project head files

#include "RoboticBiopsyDoc.h"
#include "RoboticBiopsyView.h"

//////////////////////////////////////////////////////////////////////
// Class header files

#include "ParameterDefinition.h"
#include "OpenCV.h"
#include "Methods.h"
//#include "SutterManip.h"
#include "SiskiyouAmnl.h"
#include "Camera.h"
#include "PriorStage.h"
#include "EppendorfPump.h"
#include "SpermImmoTracker.h"
#include "NikonMicroscope.h"
#include "SingleSpermMorphology.h"
#include "SingleSpermData.h"
#include "MultiSpermTracker.h"
#include "MultiSpermMorphology.h"
#include "JoyStick.h"

//////////////////////////////////////////////////////////////////////
// Messeage for Camera 
// Message fired by the GrabImageThread when image has been acquired from the camera
const UINT WM_IMAGE_ACQUIRED = WM_USER + 100;



class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
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
	cv::Mat m_displayColorImg;	//display image;  8-bit, three channels
	cv::Mat m_copyImg;				//copied image for image processing
	cv::Mat m_recordImg;			//for RecordVideo; copied from m_display_color_mat
	CCriticalSection m_cs_copy_mat;				//Critical section for protecting m_copy_mat for image processing
	CCriticalSection m_cs_display_color_mat;
	CCriticalSection m_cs_record;

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
	
	std::ofstream m_fileSave;
	CString m_save_file_prepath;
	bool m_output_data_to_file;

	clock_t m_time_start, m_time_stop;
	std::deque<cv::Mat> m_record_deque;	//image deque for recording video

	//////////////////////////////////////////////////////////////////////////
	// Draw Image processing results
	void DrawImageResults();
	int m_drawDelayFrames;

	static DWORD WINAPI ClosedLoopControlThread(void* pParameter);
	void ClosedLoopControl();

	//////////////////////////////////////////////////////////////////////////
	// Manipulator function and variables
// 	SutterManip m_manip_left;
// 	SutterManip m_manip_right;

	SiskiyouAmnl m_manip_left;
	SiskiyouAmnl m_manip_right;

	int m_manip_axis_left, m_manip_axis_right;
	float m_manip_disp_left, m_manip_disp_right;
	sManipPosition m_manip_position_left, m_manip_position_right;

	void OnMoveManipLeft(int axis, float displacement);
	static DWORD WINAPI MoveManipLeftThread(void* pParameter);
	void MoveManipLeft(int axis, float displacement);
	void WaitForManipLeftStop();

	void OnMoveManipRight(int axis, float displacement);
	static DWORD WINAPI MoveManipRightThread(void* pParameter);
	void MoveManipRight(int axis, float displacement);
	void WaitForManipRightStop();

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
	void StageMotorControl();
	//////////////////////////////////////////////////////////////////////////
	// Nikon microscope control
	NikonMicroscope m_nikon;
	void OnSwitchObjective(long nPosition);
	static DWORD WINAPI SwitchObjectiveThread(void* pParameter);
	void SwitchObjective();
	void OnSwitchCondenser(long nPosition);

	// Objective switch control
	void OnSwitchTarget();
	void OnSwitchTargetAfter();

	void OnSwitchto100();
	static DWORD WINAPI Switchto100Thread(void *pParameter);
	void Switchto100();
	void Switchto100Objective();

	void OnSwitchto20();
	static DWORD WINAPI Switchto20Thread(void *pParameter);
	void Switchto20();
	void ReadLight();


	CString exceltm_cstring;
	cv::Point m_TargetInImagePos;
	bool m_TargetIsSelected;
	bool isNoisepieceDescend;
	bool isShowCooridinate;

	cv::Point predictedHeadPosition;

	//Morphological measurement

	SingleSpermMorphology m_SpermMorphology;
	void OnMorphologyMeasure();
	static DWORD WINAPI MorphologyMeasureThread(void *pParameter);
	void MorphologyMeasure();

	bool m_drawMorphologyIsSuccess;

	int SpermNumber;
	void SaveMorphologySuccess();

	//SPERM tracking and servoing under 100X
	//100X SPERM 
	void OnSpermTracking100X();
	static DWORD WINAPI SpermTracking100XThread(void* pParameter);
	void SpermTracking100X();
	void OnEvaluate100X();
	void Evaluate100X(int totalFrames);

	void OnSpermTracking20X();
	static DWORD WINAPI SpermTracking20XThread(void* pParameter);
	void SpermTracking20X();

	void OnPredictPosition();
	static DWORD WINAPI PredictPositionThread(void* pParameter);
	void PredictPosition();

	void OnPredictSwitch();
	static DWORD WINAPI PredictSwitchThread(void* pParameter);
	void PredictSwitch();

	bool m_isTargetSperm100XSelected;
	bool m_isTargetSpermToCenter100X;
	bool m_isTargetSpermCentered100X;
	bool m_isEvaluate100X;
	bool m_isShowTrack100X;

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

	cv::Point m_TargetSperm100XPos;
	void ServoSpermCenter100X();
	void PIDSpermHeadCenter100X();

	void OnRecordData();
	static DWORD WINAPI RecordTrajectoryThread(void* pParameter);
	void RecordTrajectory();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Funcitons and algorithms
	
	void OnEmergencyStop();

	// Sperm Immobilization
	void OnSpermImmo();
	static DWORD WINAPI SpermImmoThread(void* pParameter);
	void SpermImmo();
	void ServoSpermHeadCenter();
	void PIDSpermHeadCenter();
	void PIDSpermHeadCenter40X();
	void SpermHit();
	SpermImmoTracker m_spermImmoTracker;
	bool m_ImmoIsSpermSelected;
	bool m_ImmoToServoSpermCenter;
	cv::Point m_ImmoViewCenter;
	cv::Point2f m_ImmoHeadPt, m_ImmoTailPt;
	std::deque<cv::Point2f> m_ImmoPIDErrorSum;
	int m_ImmoSpermCenterCount;
	bool m_ImmoIsSpermCentered;
	double m_ImmoPipetteHeight;

	// variables for laser immobilization
	cv::Point m_ImmoLaserCenter;
	std::vector<cv::Point2f> m_LaserSpermHeadVec;
	void SpermEvaluate40X(int totalFrames);
	void SaveLaserSuccessful(bool isSuccess);
	unsigned int m_save_laser_count;

	// variables and functions for saving data to DATABASE
	bool m_ImmoIsSpermGood;
	bool m_ImmoIsSaveToDATABASE;
	CString m_save_dataBase_prepath;
	std::vector<cv::Mat> m_ImmoRecordVideoVec;
	std::ofstream m_ImmoRecordCSVfile;
	// Selecting a target sperm satisfying the criteria out of multiple sperms
	MultiSpermTracker* m_multi_tracker;
	void OnTrackMultipleSperm();
	static DWORD WINAPI TrackMultipleSpermThread(void* pParameter);
	void TrackMultipleSperm();
	bool m_is_multi_track_done;
	std::vector<SingleSpermData> m_multi_allData;



	// rotation during Immobilization
	EppendorfPump m_rotate_stage;
	std::deque<double> m_RotateAnglePIDErrorSum;
	double ServoTheta(double current_angle_degree);
	double m_PriorX;
	double m_PriorY;
	bool m_ImmoIsRotateDone;
	double m_ImmoHeadAngleDegree;
	bool m_ImmoIsRotationNeeded;
	double m_ImmoRotationTargetDegree;
	int m_thetaCount;

	// save success rate data
	void SpermEvaluate(int totalFrames);
	void SaveImmoSuccessRate(bool isSuccessful);
	bool m_ImmoIsEvaluating;
	void SaveSpermData();



	void TrackDarkObject(cv::Mat _srcImg, cv::Point2f &Pt, double &angle_degree); 

	// Contact Detection of pipette tip
	void OnContactDetection();
	static DWORD WINAPI ContactDetectionThread(void* pParameter);
	void ContactDetection();
	void PositionPipetteToPreSetPos();
	bool DetectContactImage();	// combines the first and second method
	cv::Point m_ContactPipPresetPos;
	cv::Point m_PipInImagePos;
	bool m_ContactIsPipSelected;
	bool m_ContactIsSuccessful;
	sManipPosition m_ContactedManipPos;

	//////////////////////////////////////////////////////////////////////////
	//	Autofocus with Fibonacci algorithm on whole image
	static DWORD WINAPI AutofocusThread(void* pParameter);
	void Autofocus(double search_range, double stepSearchDirection = 20.0, cv::Rect search_ROI = cv::Rect(0,0,0,0));
	void OnAutofocus();

	/////////////////////////////////////

	static DWORD WINAPI RotateStageThread(void* pParameter);
	void RotateStage();
	void OnRotateStage();
	bool m_rotateTargetSelected;
	cv::Point m_rotateTargetPosPx;
	void RotateStagePurePositionCom();


	void ServoCompensateRotateStage();
	__int64 m_RotateMotorIniPosition;

	double m_RotateTheta0;
	double m_RotateRadius;
	std::deque<double> m_RotateAngleFilter;

	JoyStick m_joy_stick;

	//////////////////////////////////////////////////////////////////////////
	//	Deposit sperm into gel
	//	Movement functions
	void OnSetDishInitialPosition();

	void OnMoveStageToDish();
	static DWORD WINAPI MoveStageToDishThread(void* pParameter);
	void MoveStageToDish();

	void OnMoveStageToGel();
	static DWORD WINAPI MoveStageToGelThread(void* pParameter);
	void MoveStageToGel();

	double m_DishPriorPosition_x, m_DishPriorPosition_y;
	sManipPosition m_DishManipPosition;
	double m_DishManipZPosition;
	sManipPosition m_TopManipPosition;
	double m_TopManipZPosition;

	double m_GelPosition_x, m_GelPosition_y;
	bool m_isDishPositionSet;
	int m_targetGelNo;
	int m_targetCellNo;

	int WM_3DMOUSE;

	afx_msg LRESULT On3DMouse( WPARAM wParam, LPARAM lParam );


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

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


