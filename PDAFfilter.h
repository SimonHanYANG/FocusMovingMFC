//////////////////////////////////////////////////////////////////////////
//	Probabilistic data association filter		//////////////////////////
//	Developed by Zhuoran Zhang in April 2017	//////////////////////////
//	Matrix operation is based on OpneCV library	//////////////////////////
//	Reference: The Probabilistic Data Association Filter.	//////////////
//	DOI:10.1109/MCS.2009.934469					//////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//	only deals with A = [1 0 1 0; 0 1 0 1; 0 0 1 0; 0 0 0 1]


#pragma once

#include "OpenCV.h"
#include <vector>

class PDAFfilter
{
public:
	PDAFfilter(void);
	~PDAFfilter(void);

	//initialize the filter
	//DP: dynamics Parameter (dimension)
	//MP: measure Parameter (dimension)
	//CP: control Parameter (dimension)
	void init(int DP=4, int MP=2, int CP=0, int type=CV_32F);

	void init(cv::Point2f pt, int DP=4, int MP=2, int CP=0, int type=CV_32F);
	
	void setInitState(cv::Point2f initPos){statePost = (cv::Mat_<float>(4, 1)<<initPos.x, initPos.y, 0.0, 0.0);};

	//set necessary parameters	
	void setProcessNoiseCov(float pNoise){processNoiseCovPost = (cv::Mat_<float>(4, 4) << pNoise,0,0,0,   0,pNoise,0,0,  0,0,pNoise,0,  0,0,0,pNoise);	processNoiseCovPost.copyTo(Q0);};
	
	void setMeasureNoiseCov(float mNoise){measureNoiseCov = (cv::Mat_<float>(2, 2) << mNoise,0,		  0,mNoise);};
	
	void setGatingProbability(float value){Pg = value; GateThreshold = -2.0*log(1-Pg);};


	/*************core functions*************/
	//1. predict state
	//returns the Mahalanobis gating ellipse
	cv::RotatedRect predict();

	//2. correct based on selected valid measurements from all measurements
	//the error and state corvariance are weighted based on a standard kalman filter
	cv::Mat correct(std::vector<cv::Point2f> _allMeasurements);

protected:
								//notation follows ref[MSC2009]
	cv::Mat statePred;			//predicted state x(cap)
	cv::Mat statePost;			//corrected state x
	cv::Mat stateCovPred;		//predicted state covariance matrix P(k|k-1)
	cv::Mat stateCovPost;		//corrected state covariance matrix P(k|k)
	cv::Mat measurePred;		//predicted measurement z(cap)(k|k-1)
	cv::Mat transitionMatrix;	//system matrix (F)
	cv::Mat measurementMatrix;	//measurement (H)
//	cv::Mat controlMatrix;		//control matrix (B) not used in most cases
	cv::Mat processNoiseCovPred;//process noise covariance matrix (Q)
	cv::Mat processNoiseCovPost;//updated Q adaptive
	cv::Mat Q0;					//initialized Q
	cv::Mat measureNoiseCov;	//measurement noise covariance matrix (R)
	cv::Mat errorCov;			//innovation covariznce matrix (S)
	cv::Mat gain;				//Kalman gain matrix (W)

	float Pd;					//target detection probability
	float Pg;					//gate probability
	float GateThreshold;		//gate threshold for the Mahalanobis distance
	
	// temporary matrices
	cv::Mat invS;
	cv::Mat transitionMatrixT;	//transpose of F (transitionMatrix)
	cv::Mat measurementMatrixT;	//transpose of H (measurementMatrix)

	int mType;

protected:

	float calcMahalDistance(cv::Point2f _oneMeasure);
	
	void printMatrix(cv::Mat Matrix);

	cv::RotatedRect getErrorEllipse(double chisquare_val, cv::Point2f mean, cv::Mat covmat);

	// Other correction methods for performance comparison
	// Nearest Neighbor (using Mahalanobis distance)
	cv::Point2f correctNN(std::vector<cv::Point2f> _allMeasurements);
	
	//	standard kalman filter
	cv::Point2f correctKalman(std::vector<cv::Point2f> _allMeasurements);
};

