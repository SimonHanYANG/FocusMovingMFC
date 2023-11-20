#pragma once

#include "OpenCV.h"
#include "ParameterDefinition.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/core/types_c.h>
#include "inference.h"
using namespace cv;  
using namespace std;
extern Inference inf;
namespace Methods{
	namespace Vision
	{
		
		cv::Point FindContours(cv::Mat img);
		cv::Point FindContours_v8(cv::Mat img);
		//void init_model();

	}
	namespace tools
	{
		void PrintTime();
	}
/**
 * Function for thinning the given binary image
 *
 * @param  im  Binary image with range = 0-255
 */
void thinningGuoHallIteration(cv::Mat& im, int iter);
void thinningGuoHall(cv::Mat& im);

//	Ensure the set ROI within image boundary
//	returns the ROI Rect
cv::Rect setRoiWithinImage(cv::Point center, int height, int width, cv::Size ImgSize = cv::Size(nImgWidth, nImgHeight));


//	Convert img to standard deviation image.
//	input: KernalSize: calculate sd of neighbor pixels, default 5*5 pixels.
cv::Mat ConvertToSDimg(cv::Mat src_gray8, int KernalSize = 5);

//	Threshold images based on maximum entropy
//	returns the threshold value
uchar maxEntropieThreshold(const cv::Mat1b& src8, cv::Mat& dst);


//	Detect Embryo position based on variance image
void DetectEmbryoSD(cv::Mat _src_gray, std::vector<cv::Point2f> &embryo_PtVector, std::vector<float> &embryo_radius_Vector, cv::Rect pipette_ROI = cv::Rect(0,0,0,0));

//	Description:get focus measure by using sobel operator 
double getFocusScore_Sobel(cv::Mat _scrImg); 


//	find pipette edge;
//	_scaleFactor: < 1.0 to downresolution to speed up calculation
std::vector<cv::Point> detectPipetteContour(cv::Mat src_gray, float _scaleFactor = 1.0, cv::Rect _Roi = cv::Rect(0,0,0,0));	//TODO: add an input parameter of pipette located to the left or right

//	find the pipette tip point (right most point of the contour)
cv::Point detectPipetteTipPoint(cv::Mat src_gray, float _scaleFactor = 0.5, cv::Rect _Roi = cv::Rect(0,0,0,0));


/// Description: find the micropipette using contour detection, and return the ROI 
/// method 1:mean*0.75; method 3:Otsu; method 4:soble+mean*0.75  
cv::Rect findPipette( cv::Mat _srcImg, int _iMethod = 4 );

/// Description: find the micropipette using gradient subtraction, and return the ROI 
cv::Rect findPipettebySobel(cv::Mat _srcImg, int _iMethod = 2);


//	Calculate distance
double CalcDistance(cv::Point p, cv::Point q);
double CalcDistance(cv::Point2f p, cv::Point2f q);
double CalcDistance(cv::Point p, cv::Point2f q);
void FitLeastSquareLine(std::vector<cv::Point> pt, double &k, double &b);
template <typename T> int sgn(T val) {
	return (T(0) < val) - (val < T(0));
}

template <typename T> float getMedian(std::vector<T> srcVec){
	if(srcVec.size() % 2)
	{
		const auto medianIt = srcVec.begin() + srcVec.size()/2;
		std::nth_element(srcVec.begin(), medianIt, srcVec.end());
		return *medianIt;
	}
	else
	{
		const auto medianIt1 = srcVec.begin() + srcVec.size() / 2 - 1;
		const auto medianIt2 = srcVec.begin() + srcVec.size() / 2;
		std::nth_element(srcVec.begin(), medianIt1, srcVec.end());
		std::nth_element(srcVec.begin(), medianIt2, srcVec.end());
		return (*medianIt1 + *medianIt2)/2.0;
	}
}

//Savitzky-Golay Filter
std::vector<double> SavitzkyGolaySmooth(std::vector<double> &v, const int order, const int window);	//window size should be an odd number
std::vector<double> sgCoeff(const std::vector<double> &b, const unsigned int order);


//Focus score for auto-focus
double GetFocusScore(cv::Mat _scrImg, cv::Rect ROI = cv::Rect(0,0,0,0));//(440,310,90,80));

double getFocusScore_NormalizedVaraiance( cv::Mat _srcImg ) ;


}

