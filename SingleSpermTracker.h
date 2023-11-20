#pragma once
#include "OpenCV.h"
#include "PDAFfilter.h"
#include "Methods.h"
#include <vector>


class SingleSpermTracker
{
public:
	SingleSpermTracker(void);
	~SingleSpermTracker(void);


	// set the initial position by mouse clicking
	void setInitialPosition(int x, int y);
	
	// cancel tracking
	void cancelTracking();

	// track function to be called
	void track(cv::Mat _srcImg);

	// get HeadPosition result
	cv::Point2f getTrackResult(){return headPosition;};

protected:
	
	std::vector<cv::Point2f> detectHead();

	cv::Point2f headAssociationFilter(std::vector<cv::Point2f> headVec);

	void initPDAFilter();

protected:
	cv::Mat copyImg;	//gray scale
	cv::Mat showImg;	//three-channel image for displaying results for debug
	bool bIsTracking;
	cv::Point initialPos;
	cv::Point2f headPosition;

	PDAFfilter m_PDAFilter;
};

