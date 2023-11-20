#include "stdafx.h"
#include "KalmanFilterZ.h"

KalmanFilterZ::KalmanFilterZ(void)
{

	A[0][0] = 1;	A[0][1] = 0;	A[0][2] = 1;	A[0][3] = 0;
	A[1][0] = 0;	A[1][1] = 1;	A[1][2] = 0;	A[1][3] = 1;
	A[2][0] = 0;	A[2][1] = 0;	A[2][2] = 1;	A[2][3] = 0;
	A[3][0] = 0;	A[3][1] = 0;	A[3][2] = 0;	A[3][3] = 1;
	measure_cov = 1e-1;
	process_cov = 1e-5;
	measurement = cvCreateMat(2, 1, CV_32FC1);

	filter = cvCreateKalman(4,2);
	memcpy(filter->transition_matrix->data.fl, A, sizeof(A));
	cvSetIdentity(filter->measurement_matrix, cvRealScalar(1));
	cvSetIdentity(filter->process_noise_cov, cvRealScalar(process_cov));
	cvSetIdentity(filter->measurement_noise_cov, cvRealScalar(measure_cov));
	cvSetIdentity(filter->error_cov_post, cvRealScalar(1));

}

KalmanFilterZ::~KalmanFilterZ(void)
{
	cvReleaseKalman(&filter);
}

void KalmanFilterZ::Reset(void)
{
	cvReleaseKalman(&filter);
	filter = cvCreateKalman(4,2);
	memcpy(filter->transition_matrix->data.fl, A, sizeof(A));
	cvSetIdentity(filter->measurement_matrix, cvRealScalar(1));
	cvSetIdentity(filter->process_noise_cov, cvRealScalar(process_cov));
	cvSetIdentity(filter->measurement_noise_cov, cvRealScalar(measure_cov));
	cvSetIdentity(filter->error_cov_post, cvRealScalar(1));
}


void KalmanFilterZ::SetMeasureCov(double measure)
{
	measure_cov = measure;
}

void KalmanFilterZ::SetProcessCov(double process)
{
	process_cov = process;
}


CvPoint2D32f KalmanFilterZ::UpdateFilter(CvPoint2D32f data)
{
	const CvMat* prediction = cvKalmanPredict(filter, 0);
	measurement->data.fl[0] = data.x;
	measurement->data.fl[1] = data.y;
	const CvMat* correction = cvKalmanCorrect(filter, measurement);
	return cvPoint2D32f(correction->data.fl[0], correction->data.fl[1]);
}

cv::Point2f KalmanFilterZ::UpdateFilter(cv::Point2f data)
{
	const CvMat* prediction = cvKalmanPredict(filter, 0);
	measurement->data.fl[0] = data.x;
	measurement->data.fl[1] = data.y;
	const CvMat* correction = cvKalmanCorrect(filter, measurement);
	return cv::Point2f(correction->data.fl[0], correction->data.fl[1]);
}
