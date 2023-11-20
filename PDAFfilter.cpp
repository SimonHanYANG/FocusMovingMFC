#include "stdafx.h"
#include "PDAFfilter.h"
#include <cmath>


PDAFfilter::PDAFfilter(void)
{
}


PDAFfilter::~PDAFfilter(void)
{
}


void PDAFfilter::init(int DP, int MP, int CP, int type)
{
	mType = type;
	float dt = 0.03;
	transitionMatrix = (cv::Mat_<float>(4, 4) << 1,0,dt,0,   0,1,0,dt,  0,0,1,0,  0,0,0,1);
	measurementMatrix = (cv::Mat_<float>(2, 4) << 1,0,0,0,  0,1,0,0);
	statePred = cv::Mat::zeros(DP, 1, type);
	statePost = cv::Mat::zeros(DP, 1, type);
	stateCovPred = cv::Mat::zeros(DP, DP, type);
	stateCovPost = cv::Mat::eye(DP, DP, type);
	measurePred = cv::Mat::zeros(MP, 1, type);
	processNoiseCovPred = cv::Mat::eye(DP, DP, type);
	processNoiseCovPost = cv::Mat::eye(DP,DP, type);
	measureNoiseCov = cv::Mat::eye(MP, MP, type);
	errorCov = cv::Mat::zeros(MP, MP, type);
	gain = cv::Mat::zeros(DP, MP, type);

	Pd = 1;
	Pg = 0.99;	//0.9999
	GateThreshold = 9.21;

	cv::transpose(transitionMatrix, transitionMatrixT);
	cv::transpose(measurementMatrix, measurementMatrixT);
}

void PDAFfilter::init(cv::Point2f pt, int DP, int MP, int CP, int type)
{
	init(DP, MP, CP, type);

	setInitState(pt);
}



cv::RotatedRect PDAFfilter::predict()
{
	// predict state: x(cap) = F*x
	statePred = transitionMatrix*statePost;

	// predict measurement: z(cap) = H*x
	measurePred = measurementMatrix*statePost;

	// adaptive Process Noise Covariance
	cv::Mat v = statePred-statePost;
	cv::Mat vT;	cv::transpose(v, vT);
	processNoiseCovPred = 0.7*processNoiseCovPost + 0.15*v*vT + 0.15*Q0;
	processNoiseCovPost = processNoiseCovPred;

	// predict state covariance: P(cap) = FPF'+Q
	stateCovPred = transitionMatrix*stateCovPost*transitionMatrixT + processNoiseCovPred;

	// calculate the innovation covariance matrix: S = HPH'+R
	errorCov = measurementMatrix*stateCovPred*measurementMatrixT + measureNoiseCov;

	// calculate filter gain: W = PH'S^(-1)
	invS = errorCov.inv();
	gain = stateCovPred*measurementMatrixT*invS;
	
	// calculate gating ellipse
	cv::Point2f predPt;
	predPt.x = measurePred.at<float>(0,0);
	predPt.y = measurePred.at<float>(1,0);
	cv::RotatedRect resRect = getErrorEllipse(GateThreshold, predPt, errorCov);

	return resRect;
}




cv::Mat PDAFfilter::correct(std::vector<cv::Point2f> _allMeasurements)
{
	// 1.measurement validation based on Mahalanobis Distance
	//   valid measurement are stored in validMeasureVec
	std::vector<cv::Point2f> validMeasureVec;
	std::vector<float> validDistanceVec;
	float tempMahDistance;

	for (int ii = 0; ii < _allMeasurements.size(); ++ii)
	{
		// distance = sqrt([z-z(cap)]'S^(-1)[z-z(cap)])
		tempMahDistance = calcMahalDistance(_allMeasurements[ii]);

		if (tempMahDistance <= GateThreshold)
		{
			validMeasureVec.push_back(_allMeasurements[ii]);
			validDistanceVec.push_back(tempMahDistance);
		}
	}

	int validDataNum = validMeasureVec.size();

	//std::cout<<"validDataNum: "<<validDataNum<<std::endl;

	// use predicted state as measurement if no valid measurement
	if (validDataNum == 0)
	{
		statePost = statePred;
		stateCovPost = stateCovPred;
		return statePost;
	} 

	// 2.Evaluate association probability for each valid measurement
	std::vector<double> betta;
	std::vector<double> logLik;
	double sumLik = 0.;
	double detS = cv::determinant(errorCov);
	for (int ii = 0; ii < validDataNum; ++ii)
	{
		//double likelyHood = sqrt(CV_PI*0.5)*exp(-0.5*validDistanceVec[ii]*validDistanceVec[ii])*Pd*validDataNum*sqrt(detS);
		double likelyHood = sqrt(CV_PI*0.5)*exp(-0.5*validDistanceVec[ii]*validDistanceVec[ii])*GateThreshold*GateThreshold/validDataNum;
		logLik.push_back(likelyHood);
		sumLik += likelyHood;
	}
	
	//float betta0 = (1.-Pd*Pg)*2.*validDataNum*sqrt(detS)/(Pd*GateThreshold);
	//float betta0 = validDataNum*2*CV_PI*sqrt(detS)*(1-Pd*Pg)/Pd;
	double den = 1.0-Pd*Pg+sumLik;
	for (int ii = 0; ii < validDataNum; ++ii)
	{
		double tempBettaI = logLik[ii]/den;
		betta.push_back(tempBettaI);
	}
	
// 	for (int ii = 0; ii< betta.size(); ++ii)
// 		std::cout<<"betta "<<ii+1<<": "<<betta[ii]<<std::endl;

	double betta0 = (1.0-Pd*Pg)/den;
	//std::cout<<"betta0: "<<betta0<<std::endl;


	// 3. state estimation
	cv::Mat errorCombined = cv::Mat::zeros(2, 1, CV_32F);
	for (int ii = 0; ii < validDataNum; ++ii)
	{
		cv::Mat errorI = cv::Mat::zeros(2,1,CV_32F);
		errorI.at<float>(0,0) = validMeasureVec[ii].x - measurePred.at<float>(0,0);
		errorI.at<float>(1,0) = validMeasureVec[ii].y - measurePred.at<float>(1,0);
		errorCombined += betta[ii]*errorI;
	}

	statePost = statePred + gain*errorCombined;

	// 4. update state covariance
 	cv::Mat Pc, PP;
	// Pc = (I-WH)*P_predic;
	Pc = (cv::Mat::eye(4,4,CV_32F) - gain*measurementMatrix)*stateCovPred;

	cv::Mat temp1 = cv::Mat::zeros(2, 2, CV_32F);
	for (int ii = 0; ii < validDataNum; ++ii)
	{
		cv::Mat errorI = cv::Mat::zeros(2,1,CV_32F);
		errorI.at<float>(0,0) = validMeasureVec[ii].x - measurePred.at<float>(0,0);
		errorI.at<float>(1,0) = validMeasureVec[ii].y - measurePred.at<float>(1,0);

		cv::Mat eiT; cv::transpose(errorI, eiT);
		temp1 += betta[ii]*errorI*eiT;
	}

	// PP = W(temp1-vv')W'
	cv::Mat eT;		cv::transpose(errorCombined, eT);
	cv::Mat gainT;	cv::transpose(gain, gainT);
	PP = gain*(temp1- errorCombined*eT)*gainT;

	// P = belta0*P_pred + [1-belta0]*Pc + PP
	stateCovPost = betta0*stateCovPred + (1.0-betta0)*Pc + PP;

	return statePost;
}


float PDAFfilter::calcMahalDistance(cv::Point2f _oneMeasure)
{
	// 	cv::Mat currentMeasure = (cv::Mat_<float>(2, 1) << _oneMeasure.x, _oneMeasure.y);
	// 	cv::Mat measurePredDiff = currentMeasure - measurePred;
	// 	cv::Mat measurePredDiffT; cv::transpose(measurePredDiff, measurePredDiffT);
	// 	cv::Mat distanceMatrix = measurePredDiffT*invS*measurePredDiff;
	// 	return sqrt(distanceMatrix.at<float>(0,0));

	// direct calculation is much faster than matrix operation
	float dx = _oneMeasure.x - measurePred.at<float>(0);
	float dy = _oneMeasure.y - measurePred.at<float>(1);
	float a00 = invS.at<float>(0,0);	float a01 = invS.at<float>(0,1);
	float a10 = invS.at<float>(1,0);	float a11 = invS.at<float>(1,1);
	float resVal = dx*(dx*a00+dy*a10)+dy*(dx*a01+dy*a11);
	return sqrt(resVal);
}


cv::Point2f PDAFfilter::correctNN(std::vector<cv::Point2f> _allMeasurements)
{
	cv::Point2f resPt;
	// 1.measurement validation based on Mahalanobis Distance
	//   valid measurement are stored in validMeasureVec
	float tempMahDistance;	cv::Mat distanceMatrix;
	cv::Mat currentMeasure;
	cv::Mat measurePredDiff, measurePredDiffT;

	float tempMin = 1000000000.;

	for (int ii = 0; ii < _allMeasurements.size(); ++ii)
	{
		currentMeasure = (cv::Mat_<float>(2, 1) << _allMeasurements[ii].x, _allMeasurements[ii].y);
		measurePredDiff = currentMeasure - measurePred;
		cv::transpose(measurePredDiff, measurePredDiffT);

		// distance = [z-z(cap)]'S^(-1)[z-z(cap)]
		distanceMatrix = measurePredDiffT*invS*measurePredDiff;
		tempMahDistance = distanceMatrix.at<float>(0,0);

		if (tempMahDistance <= tempMin)
		{
			tempMin = tempMahDistance;
			resPt = _allMeasurements[ii];
		}
	}

	statePost = (cv::Mat_<float>(4, 1) << resPt.x, resPt.y, 0, 0);

	return resPt;
}


cv::Point2f PDAFfilter::correctKalman(std::vector<cv::Point2f> _allMeasurements)
{
	cv::Point2f resPt, tempPt(0,0);

	float tempMahDistance;	cv::Mat distanceMatrix;
	cv::Mat currentMeasure;
	cv::Mat measurePredDiff, measurePredDiffT;

	float fTempMinDist = 10000000.;
	
	
	for (int ii = 0; ii < _allMeasurements.size(); ++ii)
	{
		currentMeasure = (cv::Mat_<float>(2, 1) << _allMeasurements[ii].x, _allMeasurements[ii].y);
		measurePredDiff = currentMeasure - measurePred;
		cv::transpose(measurePredDiff, measurePredDiffT);

		// distance = [z-z(cap)]'S^(-1)[z-z(cap)]
		distanceMatrix = measurePredDiffT*invS*measurePredDiff;
		tempMahDistance = distanceMatrix.at<float>(0,0);

		std::cout<<"distance: "<<tempMahDistance<<std::endl;

		if (tempMahDistance <= GateThreshold)
		{
			fTempMinDist = tempMahDistance;
			tempPt = _allMeasurements[ii];
		}
	}

	currentMeasure = (cv::Mat_<float>(2, 1) << tempPt.x, tempPt.y);
	// temp5 = z(k) - H*x'(k)
	cv::Mat temp5 = currentMeasure - measurementMatrix*statePred;

	// x(k) = x'(k) + K(k)*temp5
	statePost = statePred + gain*temp5;

	stateCovPost = stateCovPred - gain*(measurementMatrix * stateCovPred);


	resPt.x = statePost.at<float>(0,0);
	resPt.y = statePost.at<float>(1,0);

	return tempPt;
}


void PDAFfilter::printMatrix(cv::Mat Matrix)
{
	for (int i = 0; i < Matrix.rows; ++i)
	{
		for (int j = 0; j < Matrix.cols; ++j)
		{
			std::cout<<Matrix.at<float>(i,j)<<"  ";
		}
		std::cout<<std::endl;
	}
	std::cout<<std::endl;
}




cv::RotatedRect PDAFfilter::getErrorEllipse(double chisquare_val, cv::Point2f mean, cv::Mat covmat){

	//Get the eigenvalues and eigenvectors
	cv::Mat eigenvalues(2,1,CV_32F), eigenvectors(2,2,CV_32F);
	cv::eigen(covmat, eigenvalues, eigenvectors);

	//Calculate the angle between the largest eigenvector and the x-axis
	double angle = atan2(eigenvectors.at<float>(1,0), eigenvectors.at<float>(0,0));

	//Conver to degrees instead of radians
	angle = 180.0*angle/CV_PI;

	//Calculate the size of the minor and major axes
	double MajorAxis=2.0*chisquare_val*sqrt(eigenvalues.at<float>(0));
	double MinorAxis=2.0*chisquare_val*sqrt(eigenvalues.at<float>(1));

	//Return the oriented ellipse
	//The -angle is used because OpenCV defines the angle clockwise instead of anti-clockwise
	return cv::RotatedRect(mean, cv::Size2f(MajorAxis, MinorAxis), -angle);
}

