#include "stdafx.h"
#include "SpermImmobilizer.h"
#include "Methods.h"

#define SPERM_IMMO_HEAD_ROI_WIDTH 100
#define SPERM_IMMO_NECK_ROI_WIDTH 120
#define KALMAN_PROCESS_NOISE_COEF 5e-3
#define KALMAN_MEASURE_NOISE_COEF 1e-1
#define SPERM_MOVE_DIST_THRESHOLD 150

SpermImmobilizer::SpermImmobilizer(void)
{
	bIsTracking = false;
	pfHeadPos = cv::Point2f(-1, -1);
	pfHeadPosLast = cv::Point2f(-1, -1);
	pfTailPos = cv::Point2f(-1, -1);
	kalFilter.Reset();
	kalFilter.SetMeasureCov(KALMAN_MEASURE_NOISE_COEF);
	kalFilter.SetProcessCov(KALMAN_PROCESS_NOISE_COEF);
	dHeadAxisAngleLast = 720.;
	iKalFilterCount = 0;
}


SpermImmobilizer::~SpermImmobilizer(void)
{


}



void SpermImmobilizer::setInitialPos(bool toTrack, int x, int y)
{
	bIsTracking = toTrack;
	kalFilter.Reset();
	kalFilter.SetMeasureCov(KALMAN_MEASURE_NOISE_COEF);
	kalFilter.SetProcessCov(KALMAN_PROCESS_NOISE_COEF);
	if (toTrack)
	{
		pfHeadPosLast = cv::Point2f((float)x, (float)y);
		pfHeadPos = pfHeadPosLast;
		iKalFilterCount = 0;
		dHeadAxisAngleLast = 720.;
	}
	else
	{
		pfHeadPos = cv::Point2f(-1, -1);
		pfHeadPosLast = cv::Point2f(-1, -1);
		pfTailPos = cv::Point2f(-1, -1);
	}
}

cv::Point2f SpermImmobilizer::trackTailTapPt(cv::Mat _srcGrayImg, double _headToTailMicrons)
{
	if (bIsTracking)
	{
		if (_srcGrayImg.channels() == 3)
			cv::cvtColor(_srcGrayImg, srcImg, CV_BGR2GRAY);
		else
			_srcGrayImg.copyTo(srcImg);
		cv::GaussianBlur(srcImg, srcImg, cv::Size(5,5), 1.0);

		pfHeadPos = calcHeadPos();

		pfTailPos = calcTailPos(_headToTailMicrons);

		return pfTailPos;
	}
	else
		return cv::Point2f(0, 0);
}


cv::Point2f SpermImmobilizer::calcHeadPos()
{
	cv::Point2f resPt;

	cv::Rect rtHeadROI((int)(pfHeadPosLast.x - SPERM_IMMO_HEAD_ROI_WIDTH/2), (int)(pfHeadPosLast.y - SPERM_IMMO_HEAD_ROI_WIDTH/2), SPERM_IMMO_HEAD_ROI_WIDTH, SPERM_IMMO_HEAD_ROI_WIDTH);
	rtHeadROI.x = rtHeadROI.x < 0 ? 0 : rtHeadROI.x;
	rtHeadROI.y = rtHeadROI.y < 0 ? 0 : rtHeadROI.y;
	rtHeadROI.x = rtHeadROI.br().x > nImgWidth ? nImgWidth-SPERM_IMMO_HEAD_ROI_WIDTH : rtHeadROI.x;
	rtHeadROI.y = rtHeadROI.br().y > nImgHeight ? nImgHeight-SPERM_IMMO_HEAD_ROI_WIDTH : rtHeadROI.y;

	cv::Mat headImg(SPERM_IMMO_HEAD_ROI_WIDTH, SPERM_IMMO_HEAD_ROI_WIDTH, CV_8UC1);
	cv::Mat headSdImg, headEquImg, headBinImg;
	cv::Mat showImg(headImg.size(), CV_8UC3);
	srcImg(rtHeadROI).copyTo(headImg);

	headSdImg = Methods::ConvertToSDimg(headImg);
	//cv::equalizeHist(headImg, headEquImg);

	//cv::threshold(headEquImg, headBinImg, 40, 255, CV_THRESH_BINARY_INV);
	cv::threshold(headSdImg, headBinImg, 1, 255, CV_THRESH_BINARY);

	cv::erode(headBinImg, headBinImg, cv::Mat());
	cv::dilate(headBinImg, headBinImg, cv::Mat());


	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(headBinImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	double dTempMaxArea = -1.;
	for (int ii = 0; ii < contours.size(); ++ii)
	{
		double dTempArea = cv::contourArea(contours[ii]);
		if (dTempArea > dTempMaxArea)
		{
			dTempMaxArea = dTempArea;
			cv::Moments conMoment = cv::moments(contours[ii], true);
			resPt.x =  (double)rtHeadROI.tl().x + conMoment.m10/conMoment.m00;
			resPt.y =  (double)rtHeadROI.tl().y + conMoment.m01/conMoment.m00;
// 			showImg.setTo(0);
// 			cv::drawContours(showImg, contours, ii, cv::Scalar(255,255,255), -1);
// 			cv::circle(showImg, cv::Point(conMoment.m10/conMoment.m00, conMoment.m01/conMoment.m00), 1, cv::Scalar(0,0,255), 2);
		}
	}

	if (Methods::CalcDistance(resPt, pfHeadPosLast) > SPERM_MOVE_DIST_THRESHOLD)
		resPt = pfHeadPosLast;

	cv::Point2f pfKalHeadpt = kalFilter.UpdateFilter(resPt);
	if ( ++iKalFilterCount > 10)
		resPt = pfKalHeadpt;

	pfHeadPosLast = resPt;

	return resPt;
}


cv::Point2f SpermImmobilizer::calcTailPos(double _headTailMicrons)
{
	cv::Point2f resPt;

	cv::Rect rtHeadROI((int)(pfHeadPosLast.x - SPERM_IMMO_NECK_ROI_WIDTH/2), (int)(pfHeadPosLast.y - SPERM_IMMO_NECK_ROI_WIDTH/2), SPERM_IMMO_NECK_ROI_WIDTH, SPERM_IMMO_NECK_ROI_WIDTH);
	rtHeadROI.x = rtHeadROI.x < 0 ? 0 : rtHeadROI.x;
	rtHeadROI.y = rtHeadROI.y < 0 ? 0 : rtHeadROI.y;
	rtHeadROI.x = rtHeadROI.br().x > nImgWidth ? nImgWidth-SPERM_IMMO_NECK_ROI_WIDTH : rtHeadROI.x;
	rtHeadROI.y = rtHeadROI.br().y > nImgHeight ? nImgHeight-SPERM_IMMO_NECK_ROI_WIDTH : rtHeadROI.y;
	
	cv::Mat headImg(SPERM_IMMO_NECK_ROI_WIDTH, SPERM_IMMO_NECK_ROI_WIDTH, CV_8UC1);
	cv::Mat headEquImg, headBinImg, headThinImg(headImg.size(), CV_8UC1);
	cv::Mat showImg(headImg.size(), CV_8UC3);
	srcImg(rtHeadROI).copyTo(headImg);
	
	cv::equalizeHist(headImg, headEquImg);
	cv::threshold(headEquImg, headBinImg, 40, 255, CV_THRESH_BINARY_INV);
	
	cv::erode(headBinImg, headBinImg, cv::Mat());
	cv::dilate(headBinImg, headBinImg, cv::Mat());

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Point> headContour;
	cv::findContours(headBinImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	double dTempMinDist = 100000.;
	for (int ii = 0; ii < contours.size(); ++ii)
	{
		cv::Moments conMoment = cv::moments(contours[ii], true);
		cv::Point2f pfTemp;
		pfTemp.x = (double)rtHeadROI.tl().x + conMoment.m10/conMoment.m00;
		pfTemp.y = (double)rtHeadROI.tl().y + conMoment.m01/conMoment.m00;
		double dDist = Methods::CalcDistance(pfTemp, pfHeadPos);

		if (dDist < dTempMinDist && cv::contourArea(contours[ii]) > 100)
		{
			dTempMinDist = dDist;
			headContour = contours[ii];
			showImg.setTo(0);
			headThinImg.setTo(0);
			cv::drawContours(showImg, contours, ii, cv::Scalar(255,255,255), -1);
			cv::drawContours(headThinImg, contours, ii, cv::Scalar(255,255,255), -1);
		}
	}

	Methods::thinningGuoHall(headThinImg);

	std::vector<cv::Point> headThinContour;
	double dTempMaxDist = -1.;
	cv::Point ptFar;
	for (int ii = 0; ii < headThinImg.rows; ++ii)
	{
		for (int ij = 0; ij < headThinImg.cols; ++ij)
		{
			if( headThinImg.at<uchar>(ij, ii) )
			{
				headThinContour.push_back(cv::Point(ii, ij));
				double dDist = Methods::CalcDistance(cv::Point(ii, ij), cv::Point(pfHeadPos.x-rtHeadROI.tl().x, pfHeadPos.y-rtHeadROI.tl().y));
				if (dDist > dTempMaxDist)
				{
					dTempMaxDist = dDist;
					ptFar = cv::Point(ii, ij);
				}
			}
		}
	}


	std::vector<double> vecHeadAxisCoef = Methods::FitLeastSquareLine(headThinContour);
	//std::cout<<"k: "<<vecHeadAxisCoef[0]<<std::endl<<"b: "<<vecHeadAxisCoef[1]<<std::endl;
	cv::Point2f pfHeadFit, pfFarFit;
	if (_isnan(vecHeadAxisCoef[0]))
	{
		std::cout<<"Head Axis Slope is NAN.\n";
		return cv::Point2f(0.,0.);
	}

	pfHeadFit.x = pfHeadPos.x-rtHeadROI.tl().x;
	pfHeadFit.y = pfHeadFit.x*vecHeadAxisCoef[0]+vecHeadAxisCoef[1];
	pfFarFit.x = ptFar.x;
	pfFarFit.y = pfFarFit.x*vecHeadAxisCoef[0]+vecHeadAxisCoef[1];

	dHeadAxisAngle = atan2(pfFarFit.y-pfHeadFit.y, pfFarFit.x-pfHeadFit.x);

	if (dHeadAxisAngleLast == 720.)
		dHeadAxisAngleLast = dHeadAxisAngle;
	
	if (abs(dHeadAxisAngle-dHeadAxisAngleLast) > 90.*CV_PI/180.)
		dHeadAxisAngle = dHeadAxisAngleLast;

	dHeadAxisAngleLast = dHeadAxisAngle;

	//std::cout<<"head axis angle: "<<dHeadAxisAngle<<std::endl;

	float nor_dirx = 0., nor_diry = 0.;
	nor_dirx = cos(dHeadAxisAngle);
	nor_diry = sin(dHeadAxisAngle);

	resPt.x = pfHeadPos.x + nor_dirx * _headTailMicrons/MicronPerPixel20X;
	resPt.y = pfHeadPos.y + nor_diry * _headTailMicrons/MicronPerPixel20X;

// 	cv::cvtColor(headImg, showImg, CV_GRAY2BGR);
// 	cv::Point pStart(1, vecHeadAxisCoef[0]+vecHeadAxisCoef[1]);
// 	cv::Point pEnd(SPERM_IMMO_NECK_ROI_WIDTH, SPERM_IMMO_NECK_ROI_WIDTH*vecHeadAxisCoef[0]+vecHeadAxisCoef[1]);
// 	for (int ii = 0; ii < headThinContour.size(); ++ii)
// 		cv::circle(showImg, headThinContour[ii], 0, cv::Scalar(0,255,255));
// 	cv::line(showImg, pStart, pEnd, cv::Scalar(0,0,255));
// 	cv::circle(showImg, ptFar, 1, cv::Scalar(255,0,0), 2);
// 	cv::circle(showImg, cv::Point(pfHeadPos.x-rtHeadROI.tl().x, pfHeadPos.y-rtHeadROI.tl().y), 1, cv::Scalar(0,0,255), 2);
// 	cv::imshow("showImg2", showImg);

	return resPt;
}


std::vector<cv::Point> SpermImmobilizer::calcTapWayPoints(cv::Point2f t, cv::Point2f h, int _halfWidth)
{
	std::vector<cv::Point> vecRes;

	cv::Point ptStart, ptEnd;

	_halfWidth = _halfWidth/MicronPerPixel20X;

	if (abs(t.x - h.x) < 2.0)
	{
		ptStart.x = t.x - _halfWidth;
		ptStart.y = t.y;
		ptEnd.x = t.x + _halfWidth;
		ptEnd.y = t.y;
	}
	else if (abs(t.y - h.y) < 2.0)
	{
		ptStart.x = t.x;
		ptStart.y = t.y - _halfWidth;
		ptEnd.x = t.x;
		ptEnd.y = t.y + _halfWidth;
	}
	else
	{
		double k = (t.y - h.y)/(t.x - h.x);
		double dx = 0.;
		double kk = -1.0/k;
		dx = _halfWidth/sqrt(1.+1./(k*k));
		ptStart.x = (int)(t.x - dx);
		ptStart.y = (int)(kk*(ptStart.x - t.x) + t.y);
		ptEnd.x = (int)(t.x + dx);
		ptEnd.y = (int)(kk*(ptEnd.x - t.x) + t.y);
	}

	vecRes.push_back(ptStart);
	vecRes.push_back(ptEnd);

	return vecRes;
}
