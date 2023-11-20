#pragma once

//////////////////////////////////////////////////////////////////////////
//	Kalman Filter based on OpenCV
//	One degree of freedom
//	Key function: UpdateFilter
//		input current measured data and return filtered data


#include "OpenCV.h"


class KalmanFilterZ
{
public:
	KalmanFilterZ(void);

	void Reset(void);

	void SetMeasureCov(double measure);
	void SetProcessCov(double process);

	CvPoint2D32f UpdateFilter(CvPoint2D32f data);
	cv::Point2f UpdateFilter(cv::Point2f data);


	~KalmanFilterZ(void);

private:
	CvKalman* filter;
	float A[4][4];
	
	double measure_cov;
	double process_cov;
	CvMat* measurement;
};
