#include "SingleSpermTracker.h"

#define PDAF_Process_Noise_Cov				20.0		//20	//10
#define PDAF_Measure_Noise_Cov				10.0		//10	//200
#define PDAF_Mahalanobis_Gate_Probability	0.99		//the probability of a measurement is gated as valid measurement

using namespace Methods;


SingleSpermTracker::SingleSpermTracker(void)
{
	bIsTracking = false;
	headPosition = cv::Point2f(-1.0, -1.0);
}


SingleSpermTracker::~SingleSpermTracker(void)
{

}


void SingleSpermTracker::setInitialPosition(int x, int y)
{
	initialPos = cv::Point(x, y);
	bIsTracking = true;

	initPDAFilter();

}


void SingleSpermTracker::cancelTracking()
{
	bIsTracking = false;
	headPosition = cv::Point2f(-1.0, -1.0);
}


void SingleSpermTracker::initPDAFilter()
{
	m_PDAFilter.init(cv::Point2f(initialPos.x, initialPos.y));
	m_PDAFilter.setProcessNoiseCov(PDAF_Process_Noise_Cov);
	m_PDAFilter.setMeasureNoiseCov(PDAF_Measure_Noise_Cov);
	m_PDAFilter.setGatingProbability(PDAF_Mahalanobis_Gate_Probability);
}

void SingleSpermTracker::track(cv::Mat _srcImg)
{
	if ( !bIsTracking )
		return;

	if ( _srcImg.channels() == 3)
		cv::cvtColor(_srcImg, copyImg, CV_BGR2GRAY);
	else
		_srcImg.copyTo(copyImg);
	cv::cvtColor(copyImg, showImg, CV_GRAY2BGR);

	std::vector<cv::Point2f> vHead = detectHead();

	headPosition = headAssociationFilter(vHead);

}


std::vector<cv::Point2f> SingleSpermTracker::detectHead()
{
	std::vector<cv::Point2f> resVec;

	cv::Mat blurImg, binImg;
	cv::GaussianBlur(copyImg, blurImg, cv::Size(5,5), 0);
//	cv::threshold(blurImg, binImg, 0, 255, CV_THRESH_BINARY_INV|CV_THRESH_OTSU);

	int entrophythreshol= maxEntropieThreshold(blurImg, binImg);

	cv::imshow("Binary", binImg);

//	cv::erode(binImg, binImg, cv::Mat());
// 	cv::dilate(binImg, binImg, cv::Mat());

	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(binImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);



	cv::Point2f tempPt;

	for (int ii = 0; ii < contours.size(); ++ii)
	{
		cv::Rect recBound = cv::boundingRect(contours[ii]);
		if ( recBound.width>=5 && recBound.height>=5 )
		{
			cv::Moments mu = cv::moments(contours[ii], true);
			tempPt.x = mu.m10/mu.m00;
			tempPt.y = mu.m01/mu.m00;
			resVec.push_back(tempPt);
		}
	}

	return resVec;
}

cv::Point2f SingleSpermTracker::headAssociationFilter(std::vector<cv::Point2f> headVec)
{
	cv::Point2f resPt;

	cv::RotatedRect errorEllipse = m_PDAFilter.predict();
	
	cv::Mat filteredMatrix = m_PDAFilter.correct(headVec);
	
	resPt.x = filteredMatrix.at<float>(0,0);
	resPt.y = filteredMatrix.at<float>(1,0);

 	//std::cout<<resPt.x<<" , "<<resPt.y<<"\n\n";

	for (int ii = 0; ii < headVec.size(); ++ii)
		cv::circle(showImg, headVec[ii], 1, cv::Scalar(0,255,0), 0);

	cv::circle(showImg, resPt, 1, cv::Scalar(0,0,255), 2);

	cv::ellipse(showImg, errorEllipse, cv::Scalar(255,0,0));


	cv::imshow("showImg", showImg);
	cv::waitKey(1);

	return resPt;
}


