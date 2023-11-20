#pragma once

//GUI windows information
#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1040

// Configuration
#define USE_GIGE 1

//Math
#define PI 3.14159265359

#define ENABLE_MANIP_LEFT 0
#define ENABLE_MANIP_RIGHT 0
#define ENABLE_PRIOR_STAGE 1
#define ENABLE_PUMP_MOTOR 0			//pump motor: the Eppendorf syringe pump
#define ENABLE_PUMP_HOLDING 0		//pump holding: the pneumatic pump
#define	ENABLE_ROTATIONAL_STAGE 0	//rotation stage: the same driver board as the Eppendorf syringe pump

//Display windows
#define ClientLeft 0
#define ClientTop 0
#define nImgWidth 1280//1200
#define nImgHeight 800//900
//#define nImgWidth 640
//#define nImgHeight 480
//Display buttons
#define imagexcenter ClientLeft + nImgWidth/2
#define imageycenter ClientTop + nImgHeight/2
#define LeftDlgbarWidth 136
//#define TopDlgabarHeight 25
#define TopDlgabarHeight 0
#define ButtonHeight 50
#define ButtonWidth 50
//Color zjc:useless
#define BackGroundColor RGB(230,200,200)
#define DialogColor RGB(239,235,222)
#define GreenColor RGB(0,255,0)
#define BlackColor RGB(0,0,0)
#define BlueColor RGB(0,0,255)

//BGR Color
// 基本颜色
#define BLACK cv::Scalar(0, 0, 0)
#define WHITE cv::Scalar(255, 255, 255)
#define RED cv::Scalar(0, 0, 255)
#define GREEN cv::Scalar(0, 255, 0)
#define BLUE cv::Scalar(255, 0, 0)
#define YELLOW cv::Scalar(0, 255, 255)
#define CYAN cv::Scalar(255, 255, 0)
#define MAGENTA cv::Scalar(255, 0, 255)

// 深色系
#define DARKGRAY cv::Scalar(50, 50, 50)
#define DARKRED cv::Scalar(0, 0, 128)
#define DARKGREEN cv::Scalar(0, 128, 0)
#define DARKBLUE cv::Scalar(128, 0, 0)
#define DARKYELLOW cv::Scalar(0, 128, 128)
#define DARKCYAN cv::Scalar(128, 128, 0)
#define DARKMAGENTA cv::Scalar(128, 0, 128)

// 浅色系
#define LIGHTGRAY cv::Scalar(200, 200, 200)
#define LIGHTRED cv::Scalar(0, 0, 255)
#define LIGHTGREEN cv::Scalar(0, 255, 0)
#define LIGHTBLUE cv::Scalar(255, 0, 0)
#define LIGHTYELLOW cv::Scalar(0, 255, 255)
#define LIGHTCYAN cv::Scalar(255, 255, 0)
#define LIGHTMAGENTA cv::Scalar(255, 0, 255)

#define MicronPerPixel20X 0.36	//0.36
#define MicronPerPixel40X 0.18	//0.18
#define MicronPerPixel100X  0.072 //0.072

#define FocusPerUm20X 0.6972

#define RotCenterX	-7377.5	//-7442.5		//13080.96	//13054.08
#define RotCenterY  50.19	//23.19		//14317.44	//14314.24


#define VSL_LOW_LIMIT 16		//20	//16
#define LIN_LOW_LIMIT 0.9	//0.9	//0.95
#define ELL_LOW_LIMIT 1.5	//1.5	//1.5
#define ELL_HIGH_LIMIT 1.75	//1.75	//1.67



enum StateMap {M_IDLE, M_TEST, M_CONTACT_DETECTION, M_MULTISPERM_TRACKING, M_SPERM_IMMO, M_MOVING_DISH_GEL,
	M_MOVING_FOCUS, M_ROTATING_STAGE,M_SWITCH_OBJECTIVE,M_SWITCH_TARGET, M_MORPHOLOGY_MEASURE, M_TRACK100X, M_ASPIRATE};
