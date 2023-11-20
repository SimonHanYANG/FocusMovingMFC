#pragma once

#include "ParameterDefinition.h"
#include "OpenCV.h"
#include "KalmanFilterZ.h"

class SpermImmobilizer
{
public:
	SpermImmobilizer(void);
	~SpermImmobilizer(void);

	void setInitialPos(bool toTrack, int x = -1, int y = -1);


	// returns sperm tail point for immobilization
	cv::Point2f trackTailTapPt(cv::Mat _srcGrayImg, double _headToTailMicrons);

	std::vector<cv::Point> calcTapWayPoints(cv::Point2f t, cv::Point2f h, int _halfWidthMicrons);

	cv::Point2f getHeadPosition(){return pfHeadPos;};
	cv::Point2f getTailPosition(){return pfTailPos;};
	double getHeadAngle(){return dHeadAxisAngle;};

private:
	
	cv::Point2f calcHeadPos();
	cv::Point2f calcTailPos(double _headTailMicrons);


	cv::Mat srcImg;
	cv::Point2f pfHeadPosLast, pfHeadPos, pfTailPos;
	bool bIsTracking;
	double dHeadAxisAngle, dHeadAxisAngleLast;


	KalmanFilterZ kalFilter;
	int iKalFilterCount;
};

