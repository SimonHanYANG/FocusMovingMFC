#pragma once
#include "OpenCV.h"


class SingleSpermMorphology
{
public:
	SingleSpermMorphology(void);
	~SingleSpermMorphology(void);

	//MEMBER
	std::vector<cv::Point>  spermContour;//relative to binaryImage
	cv::Rect spermBound;//relative to binaryImage
	std::vector<cv::Point>  headOpenContour;//relative to spermRoiImage
	std::vector<cv::Point>  headContour;//relative to headRoiImage
	std::vector<cv::Point>  acrosomeContour;//relative to headRoiImage
	cv::Rect headBound;//relative to binaryImage, for headOpenContour
	cv::Rect originheadBound;//relative to binaryImage
	cv::RotatedRect headRotateBound;//relative to binaryImage
	cv::RotatedRect s;//relative to binaryImage
	std::vector<cv::Point>  tailContour;//relative to spermRoiImage
	cv::RotatedRect midpieceRotateBound;//relative to binaryImage
	std::vector<cv::Point>  vacuoleContour;//relative to headRoiImage
	cv::Point2f centerPtVacuole;
	//Measurement
	bool isSpermDetected;
	bool isHeadContourDetected;

	int headLength;
	int headWidth;
	int headArea;
	float headEllipticity;

	int acrosomeArea;
	int nucleusArea;
	float acrosomeAreaRatio;

	float tailLength;
	float tailWidth;
	bool isTailCurved;

	int midpieceLength;
	int midpieceWidth;
	float midpieceAngle;
	bool isBentNeck;

	bool isVacuole;
	int vacuoleNumber;
	int vacuoleAreaRatio;

	


	//IMPELMENTATION
	void DicRestore(cv::Mat _src, cv::Mat _output, float direction);

	void Segmentation(cv::Mat _src, cv::Mat _output);
	void DetectHead(cv::Mat _srcColor, cv::Mat _spermImage, cv::Mat _headImage);
	void DetectTail(cv::Mat _headImage,cv::Mat _spermImage,cv::Mat _tailImage );
	void DetectMidpiece(cv::Mat _src);
	void DetectVacuole(cv::Mat _src);
	void DetectAcrosome(cv::Mat _src);

	void SegmentationOld(cv::Mat _src, cv::Mat _output);
	void DetectHeadOld(cv::Mat _src);
	void DetectTailOld(cv::Mat _src,cv::Mat _output);
	void DetectMidpieceOld(cv::Mat _src);
	void DetectVacuoleOld(cv::Mat _src, cv::Mat _output);
private:
	void fftShift(cv::Mat magI);
	cv::Mat getGaussianDeriv(float sigma, float direction);
	cv::Mat createGaussFilterMask(cv::Size imsize, cv::Mat kernel);
	void computeDFT(cv::Mat& image, cv::Mat& dest);
	void computeIDFT(cv::Mat& complex, cv::Mat& dest);
	void deconvoluteWiener(cv::Mat& img, cv::Mat& kernel,float snr);
};

