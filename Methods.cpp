#include "stdafx.h"
#include "Methods.h"
#include "ParameterDefinition.h"
#include <ctime>
#include <deque>
#include <vector>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <chrono>
#include <iostream>
#include "inference.h"



namespace Methods{
	namespace Vision
	{
		cv::Point FindContours(cv::Mat img)//img BGR 
		{
			cv::Rect roiRect(240,350,800,100);//(0, 0, 1280, 693);//(540,350,200,100); 
			//cv::Mat gray;
			//cvtColor(img, gray, COLOR_BGR2GRAY);
			cv::Mat roi = img(roiRect);

			cv::Mat binary, edges;
			//threshold(roi, binary, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
			adaptiveThreshold(roi, binary, 255, ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 11, 6);  // 奇-小少大多,小多大少
			//roi.release();
			// 进行高斯滤波，降低噪声
			//GaussianBlur(binary, binary, Size(9, 9), 2, 2);

			// 进行边缘检测
			Canny(binary, binary, 250, 350);
			// 增加对比度
			/*
			double alpha = 1.5; // 可以适当调整增加的对比度程度
			cv::Mat contrast;
			cv::convertScaleAbs(edges, contrast, alpha, 0);
			*/
			string filename = "D:\\ZJC\\binary.png"; // 保存文件的路径和文件名 
			imwrite(filename, binary); // 将二值化图像保存到文件中

			
			cv::Mat binary_clone;

			std::vector<std::vector<cv::Point>> contours;// (10000, std::vector<cv::Point>(10000));
			//vector<cv::Vec4i> hierarchy;

			if (binary.data && !binary.empty())
			{
				if (binary.isContinuous()) {
					binary_clone = binary.clone();
				}
				findContours(binary_clone, contours, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
			}
			//binary.release();
			//binary_clone.release();
			cout << "ROI number: " << contours.size() << endl;
			vector< cv::Point > contour;
			unsigned long long max_contour=0;
			double max_area = 1000;// cv::contourArea(cv::InputArray(contours[0]), false);
			if (contours.size() > 0) {	
				printf("max_area=%d\n",max_area);
				for (unsigned long long i = 0; i != contours.size(); i++) {
					/*if (hierarchy.empty()) {
						continue;
					}*/
					if (contours[i].empty()) {
						continue;
					}
					if (i >= contours.size()) {
						continue;
					}
					double temp_area = cv::contourArea(cv::InputArray(contours[i]), false);
					//printf("the %d contour's area = %d", i , temp_area);
					if (max_area < temp_area && temp_area < 30000) {
						max_area = temp_area;
						max_contour = i;
					}
				}
				contour = contours[max_contour];
			}

			//cout << "max_contour: " << max_contour << endl;
			
			contours.clear();

			//cv::Moments m = moments(contour, true);
			//cv::Point center(m.m10 / m.m00, m.m01 / m.m00);
			//过滤掉非圆形的轮廓
			std::vector<cv::Point> approx;
			cv::approxPolyDP(contour, approx, cv::arcLength(cv::Mat(contour), true) * 0.02, true);
			if (approx.size() < 6) {
				return cv::Point(roiRect.x + roiRect.width / 2, roiRect.y + roiRect.height / 2);
			}
			cv::Rect rect = cv::boundingRect(contour);
			double radius = rect.width / 2.0;
			if (radius <= 0) {
				return cv::Point(roiRect.x + roiRect.width / 2, roiRect.y + roiRect.height / 2);
			}
			cv::Point center(rect.x + rect.width / 2, rect.y + rect.height / 2);

			center.x += roiRect.x;
			center.y += roiRect.y;
			return center;
			//return cv::Point(-1, -1);
		}
		void init_model() {
			std::string projectBasePath = "D:/ZYB/ball_first_weights/"; // Set your ultralytics base path

			//bool runOnGPU = true;

			//
			// Pass in either:
			//
			// "yolov8s.onnx" or "yolov5s.onnx"
			//
			// To run Inference with yolov8/yolov5 (ONNX)
			//

			// Note that in this example the classes are hard-coded and 'classes.txt' is a place holder.
			// inf =Inference(projectBasePath + "best.onnx", cv::Size(640, 640), "D:/ZYB/ball_first_weights/classes.txt", runOnGPU);
		}
		cv::Point FindContours_v8(cv::Mat img)//img BGR 
		{
			cv::Rect roiRect(240,350,800,100);//(0, 0, 1280, 693);//(540,350,200,100); 
			//cv::Mat gray;
			//cvtColor(img, gray, COLOR_BGR2GRAY);
			//cv::Mat roi = img(roiRect);
			std::vector<cv::Point> centers;

			// 计算图像中心点坐标
			int centerX = 640;
			int centerY = 400;
			//int centerX = img.cols / 2;
			//int centerY = img.rows / 2;
			//Inference inf;
            /*
			std::string projectBasePath = "D:/ZYB/ball_first_weights/"; // Set your ultralytics base path
			
			bool runOnGPU = true;

			//
			// Pass in either:
			//
			// "yolov8s.onnx" or "yolov5s.onnx"
			//
			// To run Inference with yolov8/yolov5 (ONNX)
			//

			// Note that in this example the classes are hard-coded and 'classes.txt' is a place holder.
			Inference inf(projectBasePath + "best.onnx", cv::Size(640, 640), "D:/ZYB/ball_first_weights/classes.txt", runOnGPU);
			*/
			
			std::vector<std::string> imageNames;

				cv::Mat frame ;
				// Inference starts here...
				cv::cvtColor(img, frame, cv::COLOR_GRAY2RGB);
				std::vector<Detection> output =   inf.runInference(frame);
				int detections = output.size();
				//std::cout << "Number of detections:" << detections << std::endl;

				for (int i = 0; i < detections; ++i)
				{
					Detection detection = output[i];

					cv::Rect box = detection.box;
					int boxCenterX = box.x + box.width / 2;
					int boxCenterY = box.y + box.height / 2;
					double distance = std::sqrt((centerX - boxCenterX) * (centerX - boxCenterX) + (centerY - boxCenterY) * (centerY - boxCenterY));
					// 将检测框中心点保存到列表中
					centers.push_back(cv::Point(boxCenterX, boxCenterY));
					
					cv::Scalar color = detection.color;
					// Detection box
					cv::rectangle(frame, box, color, 2);
					// Detection box text
					std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
					cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
					cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

					cv::rectangle(frame, textBox, color, cv::FILLED);
					cv::putText(frame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
					
				}
				// Inference ends here...
				// This is only for preview purposes
				
				float scale = 0.8;
				cv::resize(frame, frame, cv::Size(frame.cols*scale, frame.rows*scale));
				//cv::imshow("Inference", frame);

				cv::waitKey(-1);
				
				cv::Point center;
				std::vector<cv::Point> centersInROI;

				// 遍历centers向量中的每个点
				for (const auto& center : centers) {
					// 检查该点是否在ROI区域内
					if (center.x >= roiRect.x && center.x <= roiRect.x + roiRect.width
						&& center.y >= roiRect.y && center.y <= roiRect.y + roiRect.height) {
						// 如果该点在ROI区域内，将其添加到新的向量中
						centersInROI.push_back(center);
					}
				}
				if (!centersInROI.empty()) {
					 center = centersInROI[0];
				}
				else {
					cv::Point center(0,0);
				}
					// ...
				
                
			return center;
			//return cv::Point(-1, -1);
		}
		
	}
	namespace tools
	{
		void PrintTime()
		{
			// 获取当前时间点
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

			// 将时间点转换为 std::time_t 类型
			std::time_t now_c = std::chrono::system_clock::to_time_t(now);

			// 将 std::time_t 类型转换为本地时间
			std::tm* now_tm = std::localtime(&now_c);

			// 格式化时间为字符串
			char time_buf[20];
			std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", now_tm);

			// 输出时间字符串
			std::cout << "Current time: " << time_buf<<std::endl;

		
		}
	}
	
double CalcDistance(cv::Point p, cv::Point q)
{
	return sqrt((double)((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y)));
}

double CalcDistance(cv::Point2f p, cv::Point2f q)
{
	return sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y));
}

double CalcDistance(cv::Point p, cv::Point2f q)
{
	return sqrt( ((double)(p.x)-q.x)*((double)(p.x)-q.x)+((double)(p.y)-q.y)*((double)(p.y)-q.y) );
}

void FitLeastSquareLine(std::vector<cv::Point> pt, double &k, double &b)
{
// 	double k = 0., b = 0.;
	double sum_x_square=0.;
	double sum_x=0.;
	double sum_y=0.;
	double sum_x_y=0.;
	double x_sum_square=0.;
	int N = pt.size();

	for (int i=0; i<N;i++)
	{
		sum_x_square += pt[i].x*pt[i].x;
		sum_y += pt[i].y;
		sum_x += pt[i].x;
		sum_x_y += pt[i].x*pt[i].y;
	}
	x_sum_square = sum_x*sum_x;

	b=((sum_x_square*sum_y)-(sum_x*sum_x_y))/((double)(N)*sum_x_square-x_sum_square);
	k=(((double)(N)*sum_x_y)-(sum_x*sum_y))/((double)(N)*sum_x_square-x_sum_square);


}


void thinningGuoHallIteration(cv::Mat& im, int iter)
{
    cv::Mat marker = cv::Mat::zeros(im.size(), CV_8UC1); 

    for (int i = 1; i < im.rows; i++)
    {
        for (int j = 1; j < im.cols; j++)
        {
            uchar p2 = im.at<uchar>(i-1, j);
            uchar p3 = im.at<uchar>(i-1, j+1);
            uchar p4 = im.at<uchar>(i, j+1);
            uchar p5 = im.at<uchar>(i+1, j+1);
            uchar p6 = im.at<uchar>(i+1, j);
            uchar p7 = im.at<uchar>(i+1, j-1);
            uchar p8 = im.at<uchar>(i, j-1); 
            uchar p9 = im.at<uchar>(i-1, j-1);

            int C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
                     (!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
            int N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
            int N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
            int N  = N1 < N2 ? N1 : N2;
            int m  = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);

            if (C == 1 && (N >= 2 && N <= 3) & m == 0)
                marker.at<uchar>(i,j) = 1;
        }
    }

    im &= ~marker;
}

/**
 * Function for thinning the given binary image
 *
 * @param  im  Binary image with range = 0-255
 */
void thinningGuoHall(cv::Mat& im)
{
    im /= 255;

    cv::Mat prev = cv::Mat::zeros(im.size(), CV_8UC1);
    cv::Mat diff;

    do {
        thinningGuoHallIteration(im, 0);
        thinningGuoHallIteration(im, 1);
        cv::absdiff(im, prev, diff);
        im.copyTo(prev);
    } 
    while (cv::countNonZero(diff) > 0);

    im *= 255;
}


cv::Rect setRoiWithinImage(cv::Point center, int height, int width, cv::Size ImgSize)
{
	cv::Rect roi(center.x-0.5*width, center.y-0.5*height, width, height);

	roi.x = roi.tl().x < 0 ? 0 : roi.tl().x;
	roi.y = roi.tl().y < 0 ? 0 : roi.tl().y;

	roi.x = roi.br().x > ImgSize.width ? ImgSize.width-width : roi.tl().x;
	roi.y = roi.br().y > ImgSize.height? ImgSize.height-height: roi.tl().y;

	return roi;
}

/************************************************************************/
/* 将灰度图像转换为标准偏差图像，用于检测图像中的边缘。首先将输入的灰度图像转换为 CV_32F 格式，
   然后使用 blur 函数对图像进行模糊处理，得到图像的平均值。接着，使用 blur 函数对输入图像的平方进行模糊处理，得到平方值的平均值。
   最后，根据标准偏差公式，计算标准偏差图像并将其转换为 CV_8UC1 格式并返回                                                                 
/************************************************************************/
cv::Mat ConvertToSDimg(cv::Mat src_gray8, int KernalSize)
{
	cv::Mat src32_img, mu_img, mu2_img, temp_img, std_gray_img;
	src_gray8.convertTo(src32_img, CV_32F);

	cv::blur(src32_img, mu_img, cv::Size(KernalSize,KernalSize));
	cv::blur(src32_img.mul(src32_img), mu2_img, cv::Size(KernalSize,KernalSize));
	cv::sqrt(mu2_img-mu_img.mul(mu_img), temp_img);
	//temp_img = mu2_img-mu_img.mul(mu_img);
	temp_img.convertTo(std_gray_img, CV_8UC1);

	return std_gray_img;
}


uchar maxEntropieThreshold(const cv::Mat1b& src8, cv::Mat& dst)
{
	// Histogram
	cv::Mat hist;//(1, 256, 0.0);
/*	for (int r=0; r<src8.rows; ++r)
		for (int c=0; c<src8.cols; ++c)
			hist(src8(r,c))++;
*/
	const int histSize[] = {256};  
	float range[] = {0, 255};  
	const float* ranges[] = {range};  
	const int channels[] = {0};  

	cv::calcHist(&src8,1,channels,cv::Mat(),hist,1,histSize,ranges,true,false);  

	// Normalize
	hist /= double(src8.rows * src8.cols);

	// Cumulative histogram
	cv::Mat1d cumhist(1, 256, 0.0);
	float sum = 0;
	for (int i = 0; i < 256; ++i)
	{
		sum += hist.at<float>(i);
		cumhist(i) = sum;
	}

	cv::Mat1d hl(1, 256, 0.0);
	cv::Mat1d hh(1, 256, 0.0);

	for (int t = 0; t < 256; ++t)
	{
		// low range entropy
		double cl = cumhist(t);
		if (cl > 0)
		{
			for (int i = 0; i <= t; ++i)
			{
				if (hist.at<float>(i) > 0)
				{
					hl(t) = hl(t) - (hist.at<float>(i) / cl) * log(hist.at<float>(i) / cl);
				}
			}

		}

		// high range entropy
		double ch = 1.0 - cl;  // constraint cl + ch = 1
		if (ch > 0)
		{
			for (int i = t+1; i < 256; ++i)
			{
				if (hist.at<float>(i) > 0)
				{
					hh(t) = hh(t) - (hist.at<float>(i) / ch) * log(hist.at<float>(i) / ch);
				}
			}
		}
	}

	// choose best threshold

	cv::Mat1d entropie(1, 256, 0.0);
	double h_max = hl(0) + hh(0);
	uchar threshold = 0;
	entropie(0) = h_max;

	for (int t = 1; t < 256; ++t)
	{
		entropie(t) = hl(t) + hh(t);
		if (entropie(t) > h_max)
		{
			h_max = entropie(t);
			threshold = uchar(t);
		}
	}

	// Create output image
	dst = src8 < threshold;

	// Ensure white object on black background
	cv::Scalar meanVal = cv::mean(dst);
	if (meanVal[0] > 120)
	{
		cv::bitwise_not(dst, dst);
	}

	return threshold;
}


void DetectEmbryoSD(cv::Mat src_gray, std::vector<cv::Point2f> &embryo_PtVector, std::vector<float> &embryo_radius_Vector, cv::Rect pipette_ROI)
{
	//	swap to clear and release memory of the vector
	std::vector<cv::Point2f> swapPtVect;
	embryo_PtVector.swap(swapPtVect);
	std::vector<float> swapfloatVect;
	embryo_radius_Vector.swap(swapfloatVect);

	cv::Mat std_img, std_binary_img;
	src_gray.copyTo(std_img);

	//Gaussian Smooth before calculate SD image
	cv::GaussianBlur(std_img, std_img, cv::Size(3,3), 0.);

	std_img = ConvertToSDimg(std_img);

	//set pipette_ROI to zero
	std_img(pipette_ROI) = cv::Scalar(0);

	//////////////////////////////////////////////////////////////////////////
	//	Only for debugging Multi Focus
	cv::Rect letterMask(0,0,200,70);
	std_img(letterMask) = cv::Scalar(0);

	double max_std;
	cv::minMaxLoc(std_img, 0, &max_std);
	//std::cout<<"max_std: "<<max_std<<std::endl;
	if (max_std >= 10)
	{
		cv::threshold(std_img, std_binary_img, 0.2*max_std, 255, CV_THRESH_BINARY);
		cv::erode(std_binary_img, std_binary_img, cv::Mat()); 
		cv::dilate(std_binary_img, std_binary_img, cv::Mat());
		//		cv::imshow("std_binary_img", std_binary_img);
		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(std_binary_img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		for (int i=0; i!=contours.size(); i++)
		{
			cv::Point2f center;
			float radius;
			cv::minEnclosingCircle(contours[i], center, radius);
			cv::RotatedRect box =  cv::minAreaRect(contours[i]);

			double box_ratio = box.size.height > box.size.width ? box.size.width/box.size.height : box.size.height/box.size.width;
			if ( 20. <= radius && radius <= 40. && box_ratio > 0.7)	//20	40	0.7
			{
				//std::cout<<"x: "<<center.x<<" , y: "<<center.y<<", radius: "<<radius<<" , box_ratio: "<<box_ratio<<std::endl;
				embryo_PtVector.push_back(center);
				embryo_radius_Vector.push_back(radius);
			}
		}
		//  		cv::imshow("std_img", std_img);
		//  		cv::waitKey(1);
	}
}


double getFocusScore_Sobel(cv::Mat _scrImg)
{
	double focus_measure = 0.0;
	cv::Scalar scalarMean; 

	cv::Mat sobelMat_x, sobelMat_y, sobelMat_pow;   // CV_32FC1  CV_16SC1
	cv::Sobel(_scrImg, sobelMat_x, CV_32FC1 , 1, 0, 3);   //[-1 0 1; -2 0 2; -1 0 1]
	cv::Sobel(_scrImg, sobelMat_y, CV_32FC1 , 0, 1, 3);  //cv::BORDER_DEFAULT is the different from the old getFocusScore
	cv::pow( sobelMat_x, 2.0, sobelMat_x );
	cv::pow( sobelMat_y, 2.0, sobelMat_y );
	sobelMat_pow = sobelMat_x + sobelMat_y; 
	//cv::add( sobelMat_x, sobelMat_y, sobelMat_pow);

	/// method 01 //// //! computes mean value of selected array elements
	scalarMean = cv::mean( sobelMat_pow );
	focus_measure = scalarMean.val[0];

	return focus_measure;
}



std::vector<cv::Point> detectPipetteContour(cv::Mat _src_gray, float _scaleFactor, cv::Rect _Roi)
{
	cv::Mat blurImg, binImg, sdImg, src_gray;
	cv::Mat showImg;
	if (_Roi.area() > 0)
	{
		_src_gray(_Roi).copyTo(src_gray);
	}
	else
		_src_gray.copyTo(src_gray);

	cv::GaussianBlur(src_gray, blurImg, cv::Size(5,5), 1.0, 0.0);
	cv::resize(blurImg, blurImg, cv::Size(src_gray.cols*_scaleFactor, src_gray.rows*_scaleFactor));
	showImg = cv::Mat::zeros( blurImg.size(), CV_8UC3);
	cv::cvtColor(blurImg, showImg, CV_GRAY2BGR);

// 	cv::Scalar scalarMean = cv::mean(blurImg);
// 	double dThreshold = scalarMean.val[0]*0.75;
// 	cv::threshold(blurImg, binImg, dThreshold, 255, cv::THRESH_BINARY_INV);
	//cv::Canny(blur_src, bin_mat, dThreshold*0.5, dThreshold);	//canny with this threshold also works well

	// 将图像转换为标准偏差图像
	sdImg = ConvertToSDimg(blurImg);
	cv::threshold(sdImg, binImg, 2, 255, cv::THRESH_BINARY);

	// add a line for contour detection
	for (int iRow = 0; iRow < binImg.rows; ++iRow)	
	{
		//binImg.at<uchar>(iRow, 0) = 255;
		binImg.at<uchar>(iRow, 1) = 255;	//starts from the second row for contour detection
	}

	cv::erode(binImg, binImg, cv::Mat());
	cv::dilate(binImg, binImg, cv::Mat());

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Point> pipContour;
	cv::findContours(binImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	for (int ii = 0; ii < contours.size(); ++ii)
	{
		//std::cout<<"contour "<<ii<<" size: "<<contours[ii].size()<<std::endl;
		int iZeroCount = 0;
		std::vector<cv::Point>::iterator it;
		for(it = contours[ii].begin(); it < contours[ii].end(); )
		{
			if ((*it).x == 1)
			{
				iZeroCount++;
				//std::cout<<iZeroCount<<" : "<<(*it).x<<",  "<<(*it).y<<std::endl;
				contours[ii].erase(it);
			}
			else
				++it;
		}
		if(iZeroCount > 20)
		{
			cv::drawContours(showImg, contours, ii, cv::Scalar(0,0,255));
			pipContour = contours[ii];
		}
	}

	cv::imshow("showImg", showImg);
	cv::waitKey(0);

	if ( _scaleFactor != 1 || _Roi.area() > 0)
	{
		for (int ii = 0; ii < pipContour.size(); ++ii)
		{
			pipContour[ii].x = cvRound((float)(pipContour[ii].x)/_scaleFactor + _Roi.tl().x);
			pipContour[ii].y = cvRound((float)(pipContour[ii].y)/_scaleFactor + _Roi.tl().y);
		}
	}

	return pipContour;
	// Save contour points for algorithm development
// 	std::ofstream outputFile;
// 	outputFile.open("../SavedData/pipette_2.csv");
// 
// 	for (int i = 0; i<pipContour.size(); i++)
// 	{
// 		//std::cout<<i<<": "<<pipContour[i].x<<",   "<<pipContour[i].y<<std::endl;
// 		outputFile<<pipContour[i].x<<","<<pipContour[i].y<<std::endl;
// 	}
// 	outputFile.close();

}

cv::Point detectPipetteTipPoint(cv::Mat src_gray, float _scaleFactor, cv::Rect _Roi)
{
	std::vector<cv::Point> pipContour = detectPipetteContour(src_gray, _scaleFactor, _Roi);
	cv::Point resPt;
	int iTempMaxRight = 0;
	for (int ii = 0; ii < pipContour.size(); ++ii)
	{
		if (pipContour[ii].x > iTempMaxRight)
		{
			iTempMaxRight = pipContour[ii].x;
			resPt = pipContour[ii];
		}
	}

	std::cout << "pip tip x: " << resPt.x << "; y: " << resPt.y << ". \n" << std::endl;

	return resPt;
}



cv::Rect findPipette( cv::Mat _srcImg, int _iMethod )
{
	cv::Rect pipetteRect; 
	cv::Mat grayImg, binaryImg, showImg; 
	showImg = cv::Mat::zeros( _srcImg.size(), CV_8UC3);
	cv::GaussianBlur( _srcImg, _srcImg, cv::Size(5,5), 1.0, 1.0 );
	double dThresholding(0.0); 
	//	int iMethod = 4; 
	cv::Mat sobelMat_X, sobelMat_Y, sobelMat_mag, sobelMat_binary;
	switch( _iMethod )
	{
	case 1:
		{
			/// method 01  for thresholding ///////////////////////////
			cv::Scalar scalarMean = cv::mean( _srcImg ); 
			dThresholding = scalarMean.val[0]*0.7;  
			cv::threshold( _srcImg, binaryImg, dThresholding, 255, CV_THRESH_BINARY_INV ); 
			cv::dilate( binaryImg, binaryImg, cv::Mat(), cv::Point(-1,-1), 1);   /// dilate first due to the lack of micropipette tip
			cv::erode( binaryImg, binaryImg, cv::Mat(), cv::Point(-1,-1), 1); 
			//		cv::imshow( "threshold", binaryImg); 
			break;
		}
	case 2:
		{
			/// method 02 for thresholding -- fixed value ////////////////////////////
			/// problem: the pipette will be divided into two sperate parts after guassian filtering; 
			dThresholding = 30.0; 
			cv::threshold( _srcImg, binaryImg, dThresholding, 255, CV_THRESH_BINARY_INV ); 
			cv::dilate( binaryImg, binaryImg, cv::Mat(), cv::Point(-1,-1), 1); 
			cv::erode( binaryImg, binaryImg, cv::Mat(), cv::Point(-1,-1), 1); 
			break;
			/// //////////////////////////////////////////////////////////////////////
		}
	case 3:
		{
			/// method 03 for thresholding -- OTsu adaptive thresholding //////////////
			dThresholding = cv::threshold( _srcImg, binaryImg, 0.0 , 255, CV_THRESH_BINARY_INV|CV_THRESH_OTSU ); 
			//binaryImg.zeros( binaryImg.size(), CV_8UC1 ); 
			cv::threshold( _srcImg, binaryImg, dThresholding*0.7, 255, CV_THRESH_BINARY_INV ); 
			cv::dilate( binaryImg, binaryImg, cv::Mat(), cv::Point(-1,-1), 1); 
			cv::erode( binaryImg, binaryImg, cv::Mat(), cv::Point(-1,-1), 1); 
			break;
			/// ////////////////////////////////////////////////////////////////////////  
		}
	case 4:  /// combination of 1 and sobel 
		{
			cv::Scalar scalarMean = cv::mean( _srcImg ); 
			dThresholding = scalarMean.val[0]*0.75;  
			cv::threshold( _srcImg, binaryImg, dThresholding, 255, CV_THRESH_BINARY_INV ); 

			/// calculate the gradient along Y direction with sobelX
			cv::Sobel( _srcImg, sobelMat_X, CV_32FC1, 1, 0, 1, cv::BORDER_DEFAULT ); 
			sobelMat_mag = cv::abs( sobelMat_X ); 
			sobelMat_mag.convertTo( sobelMat_mag, CV_8UC1); 
			dThresholding = cv::threshold( sobelMat_mag, sobelMat_binary, 0.0, 255, CV_THRESH_BINARY|CV_THRESH_OTSU); 
			cv::erode( sobelMat_binary, sobelMat_binary, cv::Mat(), cv::Point(-1,-1), 2); 

			binaryImg = binaryImg + sobelMat_binary; 
			cv::erode( binaryImg, binaryImg, cv::Mat(), cv::Point(-1,-1), 1);   /// erode first due to the much information from micropipette tip
			cv::dilate( binaryImg, binaryImg, cv::Mat(), cv::Point(-1,-1), 1); 
			//cv::imshow("binaryImg", binaryImg);
			//cv::imshow("sobelMat_binary", sobelMat_binary); 
			break; 
			
		}
	default:
		{
			std::cout<< "Please select one method for pipette detection." << std::endl; 
			break;
		}

	}
	/// ////////////////////////////////////////////////////////////////////////  

	std::vector < std::vector < cv::Point > > contours;
	std::vector < cv::Point > contourPnts; 
	double dmaxArea = 0;
	int iMaxArea_index = 0;

	cv::findContours( binaryImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE ); 
	for ( int ii=0; ii< (int)contours.size(); ii++ )
	{
		double dArea = cv::contourArea( contours[ii] ); 
		if ( dArea > dmaxArea )
		{
			dmaxArea = dArea; 
			pipetteRect = cv::boundingRect( cv::Mat (contours[ii]) );
			iMaxArea_index = ii; 
		}
		cv::drawContours( showImg, contours, ii, CV_RGB(255,0,0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
	}

/*	cv::Vec4f lineVec; 
	/// (vx, vy, x0, y0) in 2d, unit vector (vx, vy); (x0, y0) one point on the line   
	/// http://docs.opencv.org/trunk/doc/py_tutorials/py_imgproc/py_contours/py_contour_features/py_contour_features.html 
	cv::fitLine( cv::Mat(contours[iMaxArea_index]), lineVec, CV_DIST_L2, 0,0.01,0.01); 
	float vx = lineVec[0];		float vy = lineVec[1];
	float x = lineVec[2];		float y = lineVec[3]; 
	int	lefty = int((-x*vy/vx) + y);
	int	righty = int(((_srcImg.cols-x)*vy/vx)+y);
	//	int	righty = int(((pipetteROI.br().y-x)*vy/vx)+y);
	cv::line( showImg, cv::Point(_srcImg.cols-1,righty),cv::Point(0,lefty), CV_RGB(255,255,0), 2,8,0 );  */

	/// for showing images 
	cv::rectangle( showImg, pipetteRect, CV_RGB(0,255,0), 2, 8, 0);
	cv::imshow( "ShowImg", showImg); 
	return pipetteRect; 

}

cv::Rect findPipettebySobel(cv::Mat _srcImg, int _iMethod )
{
	cv::Rect pipetteRect; 
	cv::Mat grayImg, binaryImg, showImg, tempImg8u; 
	cv::Mat sobelMat_X, sobelMat_Y, sobelMat_mag, sobleMat_orientation;
	cv::Mat cannyImg; 
	showImg = cv::Mat::zeros( _srcImg.size(), CV_8UC3);
	cv::GaussianBlur( _srcImg, _srcImg, cv::Size(5,5), 1.0, 1.0 );
	//cv::Canny( _srcImg, cannyImg, 30, 60); 
	//cv::imshow( "canny", cannyImg ); 
	double dThresholding(0.0); 
	cv::Sobel( _srcImg, sobelMat_X, CV_32FC1, 1, 0, 1, cv::BORDER_DEFAULT ); 
	cv::Sobel( _srcImg, sobelMat_Y, CV_32FC1, 0, 1, 1, cv::BORDER_DEFAULT ); 
	switch( _iMethod ) 
	{
	case 1:  /// XY 
		{
			sobelMat_mag = cv::abs( sobelMat_X) + cv::abs( sobelMat_Y ); 
			break;
		}
	case 2:  /// Y -- gradient along X  
		{
			/// detect vertical feature only  
			sobelMat_mag = cv::abs( sobelMat_Y ); 
			break; 
		}
	case 3:   /// mag = x^2 + y^2 
		{
			cv::cartToPolar(sobelMat_X,sobelMat_Y,sobelMat_mag,sobleMat_orientation);
			break;
		}
	default:
		break; 
	}

	sobelMat_mag.convertTo( tempImg8u, CV_8UC1); 
	cv::normalize( tempImg8u, tempImg8u, 0.0, 255, cv::NORM_MINMAX);
	dThresholding = cv::threshold( tempImg8u, binaryImg, 0.0, 255, CV_THRESH_BINARY|CV_THRESH_OTSU); 
	//	_cprintf_s("dThresholding=%f\n", dThresholding);  
	//	cv::threshold( tempImg8u, binaryImg, dThresholding, 255, CV_THRESH_BINARY);
	//cv::dilate( binaryImg, binaryImg, cv::Mat(), cv::Point(-1,-1),1); 
	//cv::erode( binaryImg, binaryImg, cv::Mat(), cv::Point(-1,-1),1); 
	//cv::dilate( binaryImg, binaryImg, cv::Mat(), cv::Point(-1,-1),2); 

	/// find the contour information 
	cv::imshow( "binaryImg", binaryImg); 
	std::vector < std::vector < cv::Point > > contours;
	std::vector < cv::Point > contourPnts; 
	double dmaxArea = 0;
	int iMaxArea_index =1; 

	cv::findContours( binaryImg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE );   /// changed binaryImg
	for ( int ii=0; ii< (int)contours.size(); ii++ )
	{
		double dArea = cv::contourArea( contours[ii] ); 
		if ( dArea > dmaxArea )
		{
			dmaxArea = dArea; 
			pipetteRect = cv::boundingRect( cv::Mat (contours[ii]) );
			iMaxArea_index =ii; 
		}
		cv::drawContours( showImg, contours, ii, CV_RGB(255,0,0), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
	}
	//cv::Canny( binaryImg, cannyImg, 30, 90); 
	//cv::imshow( "canny2", cannyImg ); 

	/// Hough transform for line detection
/*	std::vector<cv::Vec2f> lines;
	cv::HoughLines(binaryImg,lines,1,PI/180,60);
	cv::Mat result(binaryImg.rows,binaryImg.cols,CV_8U,cv::Scalar(255));
	binaryImg.copyTo(result);

	std::vector<cv::Vec2f>::const_iterator it= lines.begin();
	while (it!=lines.end()) 
	{

		float rho= (*it)[0];   // first element is distance rho
		float theta= (*it)[1]; // second element is angle theta

		if (theta < PI/4. || theta > 3.*PI/4.) { // ~vertical line

			// point of intersection of the line with first row
			cv::Point pt1(rho/cos(theta),0);        
			// point of intersection of the line with last row
			cv::Point pt2((rho-result.rows*sin(theta))/cos(theta),result.rows);
			// draw a white line
			cv::line( result, pt1, pt2, cv::Scalar(255), 1); 

		} else { // ~horizontal line

			// point of intersection of the line with first column
			cv::Point pt1(0,rho/sin(theta));        
			// point of intersection of the line with last column
			cv::Point pt2(result.cols,(rho-result.cols*cos(theta))/sin(theta));
			// draw a white line
			cv::line( result, pt1, pt2, cv::Scalar(255), 1); 
		}

	//	std::cout << "line: (" << rho << "," << theta << ")\n"; 

		++it;
	}
	cv::imshow("result", result);  */

	cv::rectangle( showImg, pipetteRect, CV_RGB(0,255,0), 2, 8, 0);
	cv::imshow( "ShowImg", showImg); 

	return pipetteRect; 

}



std::vector<double> SavitzkyGolaySmooth(std::vector<double> &v, const int order, const int window)
{
	std::vector<double> res(v.size(), 0.0);
	int width = (window-1)/2;
	if ((width < 1) || (order < 1) || (v.size() < (2 * width + 2))) 
		return res;

	const int endidx = v.size() - 1;

	// handle border cases first because we need different coefficients
	int i,j;  
	for (i = 0; i < width; ++i) 
	{
		std::vector<double> b1(window, 0.0);
		b1[i] = 1.0;

		const std::vector<double> c1(sgCoeff(b1, order));
		for (j = 0; j < window; ++j)
		{
			res[i]          += c1[j] * v[j];
			res[endidx - i] += c1[j] * v[endidx - j];
		} 
	}

	// now loop over rest of data. reusing the "symmetric" coefficients.
	std::vector<double> b2(window, 0.0);
	b2[width] = 1.0;
	const std::vector<double> c2(sgCoeff(b2, order));

	for (i = 0; i <= (v.size() - window); ++i) 
	{
		for (j = 0; j < window; ++j) {
			res[i + width] += c2[j] * v[i + j];
		} 
	}
	return res;
}


std::vector<double> sgCoeff(const std::vector<double> &b, const unsigned int order)
{
	int rows(b.size());
	int cols(order + 1);
	cv::Mat A(rows, cols, CV_32FC1);
	std::vector<double> res(rows);

	// generate input matrix for least squares fit
	int i,j;
	for (i = 0; i < rows; ++i) {
		for (j = 0; j < cols; ++j) {
			A.at<float>(i,j) = pow(double(i), double(j));
		}
	}

	//c = invert(transpose(A) * A) * (transpose(A) * transpose(b))
	cv::Mat c, AT, aINV;

	cv::transpose(A, AT);
	cv::invert(AT*A, aINV);
	cv::Mat bT(rows, 1, CV_32FC1);
	for (i = 0; i < b.size(); ++i)
		bT.at<float>(i,0) = b[i];

	c = aINV * (AT*bT);

	for (i = 0; i < b.size(); ++i) 
	{
		res[i] = c.at<float>(0,0);
		for (j = 1; j <= order; ++j) 
		{
			res[i] += c.at<float>(j,0) * pow(double(i), double(j));
		}
	}
	return res;
}





double GetFocusScore(cv::Mat _scrImg, cv::Rect ROI)
{
	double focus_measure = 0.0;
	cv::Scalar scalarMean; 

	cv::Mat copyImg;

	if (ROI.x==0 && ROI.y==0)
	{
		_scrImg.copyTo(copyImg);
	}
	else
	{
		_scrImg(ROI).copyTo(copyImg);
	}
	cv::Mat sobelMat_x, sobelMat_y, sobelMat_pow;   // CV_32FC1  CV_16SC1
	cv::Sobel(_scrImg, sobelMat_x, CV_32FC1 , 1, 0, 3);   //[-1 0 1; -2 0 2; -1 0 1]
	cv::Sobel(_scrImg, sobelMat_y, CV_32FC1 , 0, 1, 3);  //cv::BORDER_DEFAULT is the different from the old getFocusScore
	cv::pow( sobelMat_x, 2.0, sobelMat_x );
	cv::pow( sobelMat_y, 2.0, sobelMat_y );
	sobelMat_pow = sobelMat_x + sobelMat_y; 
		//cv::add( sobelMat_x, sobelMat_y, sobelMat_pow);

		/// method 01 //// //! computes mean value of selected array elements
	scalarMean = cv::mean( sobelMat_pow );
	focus_measure = scalarMean.val[0];

	return focus_measure;
	
// 	double focus_measure = 0.0; 
// 	cv::Scalar scalarMean, scalarStdev; 
// 	cv::meanStdDev( _scrImg, scalarMean, scalarStdev); 
// 	//	focus_measure = std::pow( scalarStdev.val[0], 2) / scalarMean.val[0]; 
// 	focus_measure = scalarStdev.val[0];
// 	return focus_measure; 

}


double getFocusScore_NormalizedVaraiance( cv::Mat _srcImg ) 
{
	double focus_measure = 0.0; 
	cv::Scalar scalarMean, scalarStdev; 
	cv::meanStdDev( _srcImg, scalarMean, scalarStdev); 
	//	focus_measure = std::pow( scalarStdev.val[0], 2) / scalarMean.val[0]; 
	focus_measure = scalarStdev.val[0];
	return focus_measure; 
	/// previous method
	//CvScalar mean,stdev;
	//cvAvgSdv(copy_roi_img,&mean,&stdev);
	//double variance = stdev.val[0]; 
}

}//namespace Methods