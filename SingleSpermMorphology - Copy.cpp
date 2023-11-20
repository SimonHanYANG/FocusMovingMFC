
#pragma once
#include "stdafx.h"
#include "SingleSpermMorphology.h"
#include "Methods.h"

SingleSpermMorphology::SingleSpermMorphology(void)
{
}


SingleSpermMorphology::~SingleSpermMorphology(void)
{
}

void SingleSpermMorphology::fftShift(cv::Mat magI)
{
	// crop if it has an odd number of rows or columns
	magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

	int cx = magI.cols/2;
	int cy = magI.rows/2;

	cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));  // Top-Right
	cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));  // Bottom-Left
	cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy)); // Bottom-Right

	cv::Mat tmp;                            // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);                     // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);
}



void SingleSpermMorphology::computeIDFT(cv::Mat& complex, cv::Mat& dest)
{
	cv::idft(complex, dest,cv::DFT_REAL_OUTPUT+cv::DFT_SCALE);

	//following is same process of idft:DFT_REAL_OUTPUT+DFT_SCALE
	//dft(complex, work, DFT_INVERSE + DFT_SCALE);
	//Mat planes[] = {Mat::zeros(complex.size(), CV_32F), Mat::zeros(complex.size(), CV_32F)};
	//split(work, planes);                // planes[0] = Re(DFT(I)), planes[1] = Im(DFT(I))
	//magnitude(planes[0], planes[1], work);	  // === sqrt(Re(DFT(I))^2 + Im(DFT(I))^2)
}

void SingleSpermMorphology::computeDFT(cv::Mat& image, cv::Mat& dest)//not used 
{
	cv::Mat padded;                            //expand input image to optimal size
	int m = cv::getOptimalDFTSize( image.rows );
	int n = cv::getOptimalDFTSize( image.cols ); // on the border add zero values

	cv::copyMakeBorder(image, padded, 0, m - image.rows, 0, n - image.cols, cv::BORDER_REPLICATE);

	cv::Mat imgf;
	padded.convertTo(imgf,CV_32F);	
	cv::dft(imgf, dest, cv::DFT_COMPLEX_OUTPUT);  // fourier transform

	//other implimentation
	//Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};
	//merge(planes, 2, dest);         // Add to the expanded another plane with zeros
	//dft(dest, dest, DFT_COMPLEX_OUTPUT);  // furier transform
}



cv::Mat SingleSpermMorphology::createGaussFilterMask(cv::Size imsize, cv::Mat kernel)
{
	// call openCV gaussian kernel generator
	//double sigma = radius2sigma(radius);
	//Mat kernelX = getGaussianKernel(2*radius+1, sigma, CV_32F);
	//Mat kernelY = getGaussianKernel(2*radius+1, sigma, CV_32F);
	//// create 2d gaus
	//Mat kernel = kernelX * kernelY.t();

	int w = imsize.width-kernel.cols;
	int h = imsize.height-kernel.rows;

	int r = w/2;
	int l = imsize.width-kernel.cols -r;

	int b = h/2;
	int t = imsize.height-kernel.rows -b;

	cv::Mat ret;
	cv::copyMakeBorder(kernel,ret,t,b,l,r,cv::BORDER_CONSTANT,cv::Scalar::all(0));

	return ret;
}



void SingleSpermMorphology:: deconvoluteWiener(cv::Mat& img, cv::Mat& kernel,float snr)
{
	int width = img.cols;
	int height=img.rows;

	cv::Mat_<cv::Vec2f> src = kernel;
	cv::Mat_<cv::Vec2f> dst = img;

	float eps =  + 0.0001f;
	float power, factor, tmp,denom;
	float inv_snr = 1.f / (snr + 0.00001f);
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			power = src(y,x)[0] * src(y,x)[0] + src(y,x)[1] * src(y,x)[1]+eps;
			denom = 1.f / (power+inv_snr);

			tmp = dst(y,x)[0];

			dst(y,x)[0] = (src(y,x)[0] * tmp + src(y,x)[1] * dst(y,x)[1]) * denom;

			dst(y,x)[1] = (src(y,x)[0] * dst(y,x)[1] - src(y,x)[1] * tmp) * denom;	

		}
	}
}


cv::Mat SingleSpermMorphology::getGaussianDeriv(float sigma, float direction)
{
	int kernelsize=5;//5
	cv::Mat G(kernelsize,kernelsize,CV_32F);

	float x,y;
	float sum=0;
	direction=-direction*3.1415/180;//3.1415-(direction+180)*3.1415/180
	for (int i=0;i<kernelsize;i++)
	{
		for (int j=0;j<kernelsize;j++)
		{
			x=j+1-((float)kernelsize/2+0.5);
			y=i+1-((float)kernelsize/2+0.5);
			float c=-(std::pow(x,2)+std::pow(y,2))/std::pow(sigma,2);
			float d=-(std::pow(x,2)+std::pow(y,2))/std::pow(sigma,2);
			G.at<float>(i,j)=-x*std::exp(c)*cos(direction);
			G.at<float>(i,j) = G.at<float>(i,j) - y*exp(d)*sin(direction);
			sum=sum+std::abs(G.at<float>(i,j));
		}
	}
	G=G/sum;
	return G;
}

void SingleSpermMorphology::DicRestore(cv::Mat _src, cv::Mat _output, float direction)
{
	_src.convertTo(_src,CV_32F);
	cv::Mat dftmat;
	//FFT input signal
	computeDFT(_src, dftmat);
	//generating kernel
	//int r=3;
	cv::Mat H=getGaussianDeriv(0.5,direction);
	//std::cout<<H<<std::endl;
	cv::Mat mask = createGaussFilterMask(dftmat.size(),H);
	fftShift(mask);

	cv::Mat kernel;
	computeDFT(mask,kernel);//generate Gaussian Kernel


	//deconvolution
	int snr=10;
	deconvoluteWiener(dftmat,kernel,snr);//for Wiener filter snr*snr/100.0


	//imshowFFTSpectrum("spectrum filtered",dftmat);// show spectrum

	cv::Mat filteredFFT;
	computeIDFT(dftmat,filteredFFT);		// do inverse transform


	//convert scale
	double minVal;
	double maxVal;
	cv::minMaxLoc(filteredFFT, &minVal, &maxVal);

	double scale1 = 255/(maxVal - minVal);
	double shift = -minVal * scale1;
	cv::Mat filteredFFT_full;
	filteredFFT.convertTo(filteredFFT_full,CV_32F,scale1,shift);

	//for visualization
	cv::Mat filterdFIR_8u;
	cv::Mat filteredFFT_8u;
	_src.convertTo(filterdFIR_8u,CV_8U);
	filteredFFT_full.convertTo(_output,CV_8U);
}

void SingleSpermMorphology::Segmentation(cv::Mat _src, cv::Mat _spermImage)
{
	//remove noise
	cv::GaussianBlur(_src,_src,cv::Size(5,5),0,0);

	cv::Mat edgeImage(_src.rows,_src.cols,CV_8U);

	cv::Canny(_src,edgeImage,15,30,3,1);//15,30

	//find contour
	std::vector<std::vector<cv::Point> > smallcontours,spermcontours, headContours,tailContours;

	cv::findContours(edgeImage, smallcontours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	cv::Rect recBound;
	int minSize=50;//40

	//remove small contours 
	for (int hh=0; hh<smallcontours.size(); ++hh)
	{
		recBound = cv::boundingRect(smallcontours[hh]);
		if ( smallcontours[hh].size()<minSize )
		{
			drawContours(edgeImage,smallcontours,hh,cv::Scalar(0,0,0),-1,8);
		}

	}

	//morphological 
	int erosion_size = 5;  
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1)
		);
	cv::morphologyEx( edgeImage, edgeImage, cv::MORPH_CLOSE, element,cv::Point(-1,-1),2);//close the edge, or 3 times


	cv::findContours(edgeImage, spermcontours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	int area,maxarea=0,maxIndex=0;

	//find sperm contour
	for (int ii=0; ii<spermcontours.size(); ++ii)
	{

		if(spermcontours[ii].size()>maxarea)
		{
			maxarea=spermcontours[ii].size();
			maxIndex=ii;
		}
	}

	if (spermcontours.size()>0)
	{
		cv::Mat SegmentSperm(_src.rows,_src.cols,CV_8U);
		SegmentSperm.setTo(0);
		spermContour=spermcontours[maxIndex];
		drawContours(SegmentSperm,std::vector<std::vector<cv::Point> >(1,spermContour),-1,cv::Scalar(255,255,255),-1,8);

		SegmentSperm.copyTo(_spermImage);//SegmentSpermImage
	}

	
	
}


void SingleSpermMorphology::DetectHead(cv::Mat _srcColor, cv::Mat _spermImage, cv::Mat _headImage)
{
	isHeadContourDetected=false;

	std::vector<std::vector<cv::Point> > headContours;
		cv::Mat distanceImage;

	cv::distanceTransform(_spermImage,distanceImage,CV_DIST_C,3);
	cv::normalize(distanceImage, distanceImage, 0, 1., cv::NORM_MINMAX);
	cv::threshold(distanceImage, distanceImage, .4, 1., cv::THRESH_BINARY);
	cv::Mat dist_8u;

	//morphology open
	int erosion_size2 = 9;  
	cv::Mat element2 = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(2 * erosion_size2 + 1, 2 * erosion_size2 + 1)
		);
	cv::morphologyEx( distanceImage, distanceImage, cv::MORPH_OPEN, element2,cv::Point(-1,-1),1 );//open the threshold of distance image

	distanceImage.convertTo(dist_8u, CV_8U);
	//Find total markers
	cv::vector<cv::vector<cv::Point> > distcontours;
	findContours(dist_8u, distcontours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	// Create the marker image for the watershed algorithm
	cv::Mat markers = cv::Mat::zeros(distanceImage.size(), CV_32SC1);
	// Draw the foreground markers
	for (size_t i = 0; i < distcontours.size(); i++)
		drawContours(markers, distcontours, static_cast<int>(i), cv::Scalar::all(static_cast<int>(i)+1), -1);
	// Draw the background marker
	cv::circle(markers, cv::Point(5,5), 3, CV_RGB(255,255,255), -1);

	cv::watershed(_srcColor, markers);

	// Create the result image
	cv::Mat dst = cv::Mat::zeros(markers.size(), CV_8U);
	cv::Mat headImage;
	// Fill labeled objects with random colors
	for (int i = 0; i < markers.rows; i++)
	{
		for (int j = 0; j < markers.cols; j++)
		{
			int index = markers.at<int>(i,j);

			if (index ==1  )
				dst.at<uchar>(i,j) = 255;
			if (index ==0  )
				dst.at<uchar>(i,j) = 0;
		}
	}

	cv::morphologyEx( dst, dst, cv::MORPH_OPEN, element2,cv::Point(-1,-1),1 );//open the watershed image
	dst.copyTo(headImage);

	cv::findContours(dst, headContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	int headarea,headmaxarea=0,headmaxIndex=0;


	//find head contour
	for (int jj=0; jj<headContours.size(); ++jj)
	{

		if(headContours[jj].size()>headmaxarea)//
		{
			headmaxarea=headContours[jj].size();
			headmaxIndex=jj;
		}
	}
	
	if (headContours.size()>0)
	{
		if ( headContours[headmaxIndex].size()>10 )
		{
			headEllipse=cv::fitEllipse(headContours[headmaxIndex]);
			isHeadContourDetected=true;
			headContour=headContours[headmaxIndex];

			headLength=headEllipse.size.height*MicronPerPixel100X;
			headWidth=headEllipse.size.width*MicronPerPixel100X;
			headEllipticity=float(headLength)/float(headWidth);
			headArea=3.14*(headLength/2)*(headWidth/2);
			acrosomeArea=headArea*0.4;
			nucleusArea=headArea*0.6;

		}
		
	}
	
	
	headImage.copyTo(_headImage);
	
}

void SingleSpermMorphology::DetectTail(cv::Mat _headImage,cv::Mat _spermImage, cv::Mat _tailImage)//src is headimage
{
	std::vector<std::vector<cv::Point> > largeheadContours,tailContours;
	//enlarge head contour to remove tail from head
	int erosion_size3=2;
	cv::Mat element3 = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(2 * erosion_size3 + 1, 2 * erosion_size3 + 1)
		);
	cv::morphologyEx( _headImage, _headImage, cv::MORPH_DILATE, element3,cv::Point(-1,-1),5 );
	cv::findContours(_headImage, largeheadContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	drawContours(_spermImage,largeheadContours,-1, cv::Scalar(0,0,0),-1,8);

	cv::findContours(_spermImage, tailContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	int tailmaxarea=0,tailmaxIndex=0;
	//find largest tail contour
	for (int ll=0; ll<tailContours.size(); ++ll)
	{

		if(tailContours[ll].size()>tailmaxarea)//
		{
			tailmaxarea=tailContours[ll].size();
			tailmaxIndex=ll;
		}
	}
	if (tailContours.size()>0)
	{
		tailContour=tailContours[tailmaxIndex];
		//tailLength=tailContour.size()*3;
		tailLength=tailContour.size()*1.5;	//by Zhuoran Nov 14,2017
	}
	
	_spermImage.copyTo(_tailImage);
}

void SingleSpermMorphology::DetectAcrosome(cv::Mat _src)
{
	cv::Mat headBinaryImage,acrosomeImage;
	_src(headBound).copyTo(headBinaryImage);

	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
	cv::morphologyEx( headBinaryImage, acrosomeImage, cv::MORPH_OPEN, element,cv::Point(-1,-1),6);

	nucleusArea=cv::countNonZero(acrosomeImage);
	acrosomeArea=headArea-nucleusArea;
	acrosomeAreaRatio=float(acrosomeArea)/float(headArea);

	//find contour
	std::vector<cv::Vec4i> hierachy;
	std::vector<std::vector<cv::Point> > acrosomecontours; 
	cv::findContours(acrosomeImage, acrosomecontours, hierachy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	int maxAcrosomeArea=0,maxAcrosomeIndex=0;
	int jj=0;
	if (acrosomecontours.size()>0)
	{
		for (;jj>=0;jj=hierachy[jj][0])
		{
			if(acrosomecontours[jj].size()>maxAcrosomeArea )
			{
				maxAcrosomeArea=acrosomecontours[jj].size();
				maxAcrosomeIndex=jj;
			}
		}
		acrosomeContour=acrosomecontours[maxAcrosomeIndex];
	}
	

}

void SingleSpermMorphology::DetectMidpiece(cv::Mat _src)
{
	cv::Mat tailImage,midpieceImage;
	_src.copyTo(tailImage);



	cv::Point2f headCenter,tailCenter;
	if (headContour.size()==0)
		return;

	if (tailContour.size()==0)
		return;
	cv::Rect headBound = cv::boundingRect(headContour);
	cv::Rect tailBound = cv::boundingRect(tailContour);
	cv::Rect midpieceBound;



	headCenter.x = headBound.x+0.5*headBound.width;
	headCenter.y = headBound.y+0.5*headBound.height;
	tailCenter.x = tailBound.x+0.5*tailBound.width;
	tailCenter.y = tailBound.y+0.5*tailBound.height;

	midpieceBound.width=headBound.width;//the rect may override the boundary
	int midpieceHeight=70;
	midpieceBound.height=midpieceHeight;

	//to prevent ROI overiding the boundary
	if (tailCenter.y<headCenter.y)
	{
		midpieceBound.y=(tailBound.y+tailBound.height-midpieceBound.height)>0?
			(tailBound.y+tailBound.height-midpieceBound.height):0;
	}
	else
	{
		midpieceBound.y=tailBound.y;
	}

	midpieceBound.height=(midpieceBound.y+midpieceBound.height)<tailImage.rows?
midpieceHeight:(tailImage.rows-midpieceBound.y);


	if (tailCenter.x<headCenter.x)
	{
		midpieceBound.x=(tailBound.x+tailBound.width-midpieceBound.width)>0?
			(tailBound.x+tailBound.width-midpieceBound.width):0;
	} 
	else
	{
		midpieceBound.x=tailBound.x;
	}
	midpieceBound.width=(midpieceBound.x+midpieceBound.width)<tailImage.cols?
		headBound.width:(tailImage.cols-midpieceBound.x);


	//cv::rectangle(tailImage,headBoundinTailImage,cv::Scalar(255,255,255),1,8,0);
	//cv::rectangle(tailImage,midpieceBoundinTailImage,cv::Scalar(255,255,255),1,8,0);
	tailImage(midpieceBound).copyTo(midpieceImage);

	std::vector<std::vector<cv::Point> >midpiececontour; 
	cv::findContours(midpieceImage, midpiececontour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	int midpieceArea,midpieceMaxArea=0,midpieceMaxIndex=0;

	for (int mm=0; mm<midpiececontour.size(); ++mm)
	{
		midpieceArea=midpiececontour[mm].size();
		if(midpieceArea>midpieceMaxArea )//size is curve length

		{
			midpieceMaxArea=midpieceArea;
			midpieceMaxIndex=mm;
		}
	}

	//midpieceContour=midpiececontour[midpieceMaxIndex];

	//midpiece rotatebounding box
	midpieceRotateBound = cv::minAreaRect(midpiececontour[midpieceMaxIndex]);
	midpieceRotateBound.center.x=midpieceRotateBound.center.x+midpieceBound.x+spermBound.x;
	midpieceRotateBound.center.y=midpieceRotateBound.center.y+midpieceBound.y+spermBound.y;
	if (midpieceRotateBound.size.height>midpieceRotateBound.size.width)
	{
		midpieceLength=midpieceRotateBound.size.height;
		midpieceWidth=midpieceRotateBound.size.width;
	}
	else
	{
		midpieceLength=midpieceRotateBound.size.width;
		midpieceWidth=midpieceRotateBound.size.height;
	}


	//midpiece angle
	float midpieceAbsAngle=midpieceRotateBound.angle+90;
	float headAngle=headEllipse.angle;
	midpieceAngle=abs(midpieceAbsAngle-headAngle);



	isBentNeck=false;
	if (midpieceAngle>30)
	{
		isBentNeck=true;
	}

}
void SingleSpermMorphology::DetectVacuole(cv::Mat _src)
{
	vacuoleContour.clear();
	vacuoleNumber=0;
	vacuoleArea=0;

	cv:: Mat headImage(_src.rows,_src.cols,CV_8U);
	cv:: Mat headMaskImage(_src.rows,_src.cols,CV_8U);
	headImage.setTo(0);
	headMaskImage.setTo(0);

	std::vector<std::vector<cv::Point> > vacuolecontours;

	drawContours(headMaskImage,std::vector<std::vector<cv::Point> >(1,headContour),-1,cv::Scalar(255,255,255),-1,8);
	cv::Mat edgeImage(_src.rows,_src.cols,CV_8U);

	cv::Canny(_src,edgeImage,40,60,3,1);

	int erosion_size4 = 5;  
	cv::Mat element4 = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(2 * erosion_size4 + 1, 2 * erosion_size4 + 1)
		);

	cv::morphologyEx( headMaskImage, headMaskImage, cv::MORPH_ERODE, element4,cv::Point(-1,-1),2);
	//cv::morphologyEx( headImage, headImage, cv::MORPH_CLOSE, element4,cv::Point(-1,-1),3);

	for(int i=0;i<headMaskImage.rows;i++)
	{
		for(int j=0;j<headMaskImage.cols;j++)
		{
			if (headMaskImage.at<uchar>(i,j)==255)
			{
				headImage.at<uchar>(i,j)=edgeImage.at<uchar>(i,j);
			}
		}
	}
	cv::imshow("vacuole",headImage);
	cv::findContours(headImage, vacuolecontours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	int tailmaxarea=0,tailmaxIndex=0;
	cv::RotatedRect vacuoleRect;

	for (int ll=0; ll<vacuolecontours.size(); ++ll)
	{
		if(vacuolecontours[ll].size()>5) 
		{
			vacuoleContours.push_back(vacuolecontours[ll]);
			vacuoleRect=cv::minAreaRect(vacuolecontours[ll]);
			isVacuole=true;
			vacuoleNumber++;
			vacuoleArea+=3.14*pow((vacuoleRect.size.width/2),2);
		}
	}

}


void SingleSpermMorphology::processROI(cv::Mat _img, cv::Point _pt)
{
	//construct ROI
	cv::Mat copyImg, copyColorImg;
	int boundWidth=200;
	boundRect.x=_pt.x-0.5*boundWidth;
	boundRect.y=_pt.y-0.5*boundWidth;
	boundRect.width=boundWidth,boundRect.height=boundWidth;

	//_img.copyTo(copyColorImg);
	_img(boundRect).copyTo(copyColorImg);

	if ( _img.channels() == 3)
		cv::cvtColor(_img, _img, CV_BGR2GRAY);

	_img(boundRect).copyTo(copyImg);//copy gray image

	//remove noise
	cv::GaussianBlur(copyImg,copyImg,cv::Size(3,3),0,0);

	//cv::Mat smallCopyImg(copyImg.rows*0.25,copyImg.cols*0.25,CV_8U);
	//cv::Mat edgeImage(copyImg.rows*0.25,copyImg.cols*0.25,CV_8U);
	//cv::Mat spermImage(copyImg.rows*0.25,copyImg.cols*0.25,CV_8U);

	cv::Mat smallCopyImg(copyImg.rows,copyImg.cols,CV_8U);
	cv::Mat edgeImage(copyImg.rows,copyImg.cols,CV_8U);
	cv::Mat spermImage(copyImg.rows,copyImg.cols,CV_8U);


	cv::Mat  edgeImageCopy,edgeImageCopy2, edgeImageCopy3, binImg;

	//clock_t start,ends;
	//start= clock();
	//cv::resize(copyImg,smallCopyImg,smallCopyImg.size(),0,0);
	cv::Canny(copyImg,edgeImage,25,50,3,1);	//15,30 or 50,100 or 25,50// 20,40
	edgeImage.copyTo(edgeImageCopy);
	//cv::equalizeHist(copyImg, binImg);
	//cv::imshow("image0",edgeImageCopy);
	//binImg = Methods::ConvertToSDimg(blurImg);
	//cv::threshold(binImg, binImg, 5, 255, CV_THRESH_BINARY_INV);

	//ends=clock();
	//std::cout<<ends-start<<std::endl;

	//find contour
	std::vector<std::vector<cv::Point> > smallcontours,spermcontours, tailContours;

	edgeImage.copyTo(edgeImageCopy2);

	cv::findContours(edgeImageCopy2, smallcontours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	cv::Rect recBound;
	int minSize=40;//40

	//remove small contours, especially some halo around the sperm 
	for (int hh=0; hh<smallcontours.size(); ++hh)
	{
		recBound = cv::boundingRect(smallcontours[hh]);
		if ( smallcontours[hh].size()<minSize )
		{
			drawContours(edgeImage,smallcontours,hh,cv::Scalar(0,0,0),-1,8);
		}
	}



	//cv::imshow("image01",edgeImage);
	//morphological 
	int erosion_size = 5;  
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1)
		);




	cv::morphologyEx( edgeImage, edgeImage, cv::MORPH_CLOSE, element,cv::Point(-1,-1),5);//close the edge, or 3 times

	//cv::morphologyEx( edgeImage, edgeImage, cv::MORPH_OPEN, element,cv::Point(-1,-1),5);//to get the sperm head

	//edgeImage.copyTo(edgeImageCopy3);

	//cv::imshow("image02",edgeImage);


	cv::findContours(edgeImage, spermcontours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	cv::Point2f tempPt;

	spermImage.setTo(0);
	//find sperm contour
	for (int ii=0; ii<spermcontours.size(); ++ii)
	{

		//cv::Rect recBound = cv::boundingRect(spermcontours[ii]);
		//if ( recBound.width>=5 && recBound.height>=5 )
		if(spermcontours[ii].size()>50)
		{
			spermContour=spermcontours[ii];
			drawContours(spermImage,std::vector<std::vector<cv::Point> >(1,spermContour),-1,cv::Scalar(255,255,255),-1,8);
		}
	}

	//cv::imshow("image1",spermImage);
	//////////////////////////////////////////////////////////////////////////


	std::vector<std::vector<cv::Point> > headContours;
	cv::Mat distanceImage;

	cv::distanceTransform(spermImage,distanceImage,CV_DIST_C,3);
	cv::normalize(distanceImage, distanceImage, 0, 1., cv::NORM_MINMAX);
	cv::threshold(distanceImage, distanceImage, .4, 1., cv::THRESH_BINARY);
	cv::Mat dist_8u;

	//morphology open
	int erosion_size2 = 7;  
	cv::Mat element2 = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(2 * erosion_size2 + 1, 2 * erosion_size2 + 1)
		);
	cv::morphologyEx( distanceImage, distanceImage, cv::MORPH_OPEN, element,cv::Point(-1,-1),1 );//open the threshold of distance image
	cv::morphologyEx( distanceImage, distanceImage, cv::MORPH_DILATE, element,cv::Point(-1,-1),1 );//to include the halo of the sperm head

	cv::Mat distanceImage2,dist_8u2;
	distanceImage.copyTo(distanceImage2);
	cv::morphologyEx( distanceImage2, distanceImage2, cv::MORPH_DILATE, element,cv::Point(-1,-1),2 );

	distanceImage.convertTo(dist_8u, CV_8U);
	distanceImage2.convertTo(dist_8u2, CV_8U);
	//cv::imshow("image2",dist_8u*10000);

	//Find total markers
	cv::vector<cv::vector<cv::Point> > distcontours;
	findContours(dist_8u, distcontours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	// Create the marker image for the watershed algorithm
	cv::Mat markers = cv::Mat::zeros(distanceImage.size(), CV_32SC1);
	// Draw the foreground markers
	int maxsize=0,maxindex=0;
	for (size_t i = 0; i < distcontours.size(); i++)
	{
		if (distcontours[i].size()>maxsize)
		{
			maxsize=distcontours[i].size();
			maxindex=i;
		}
	}

	cv::vector<cv::vector<cv::Point> > distcontours2;
	findContours(dist_8u2, distcontours2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	int maxsize2=0,maxindex2=0;
	for (size_t i = 0; i < distcontours2.size(); i++)
	{
		if (distcontours2[i].size()>maxsize2)
		{
			maxsize2=distcontours2[i].size();
			maxindex2=i;
		}
	}
	//	drawContours(markers, distcontours, static_cast<int>(maxindex), cv::Scalar::all(1), -1);
	//// Draw the background marker
	//cv::circle(markers, cv::Point(5,5), 3, CV_RGB(255,255,255), -1);

	//cv::imshow("image3",markers*10000);

	//cv::watershed(copyColorImg, markers);

	//// Create the result image
	//cv::Mat dst = cv::Mat::zeros(markers.size(), CV_8U);
	//cv::Mat headImage;
	//// Fill labeled objects with random colors
	//for (int i = 0; i < markers.rows; i++)
	//{
	//	for (int j = 0; j < markers.cols; j++)
	//	{
	//		int index = markers.at<int>(i,j);

	//		if (index ==1  )
	//			dst.at<uchar>(i,j) = 255;
	//		if (index ==0  )
	//			dst.at<uchar>(i,j) = 0;
	//	}
	//}

	//cv::imshow("image4",dst);

	//cv::morphologyEx( dst, dst, cv::MORPH_OPEN, element,cv::Point(-1,-1),1 );//open the watershed image
	//dst.copyTo(headImage);

	//cv::findContours(dst, headContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//int headarea,headmaxarea=0,headmaxIndex=0;


	////find head contour
	//for (int jj=0; jj<headContours.size(); ++jj)
	//{

	//	if(headContours[jj].size()>headmaxarea)//
	//	{
	//		headmaxarea=headContours[jj].size();
	//		headmaxIndex=jj;
	//	}
	//}

	if (distcontours.size()>0)
	{
		if ( distcontours[maxindex].size()>10 )
		{

			headContour=distcontours[maxindex];
			largeheadContour=distcontours2[maxindex2];
			headEllipse=cv::fitEllipse(headContour);
			headEllipse.center.x+=boundRect.x;
			headEllipse.center.y+=boundRect.y;
			headLength=headEllipse.size.height;
			headWidth=headEllipse.size.width;
			headEllipticity=float(headLength)/float(headWidth);
			//headArea=3.14*(headLength/2)*(headWidth/2);
			//acrosomeArea=headArea*0.4;
			//nucleusArea=headArea*0.6;
		}
	}

	//if (headContours.size()>0)
	//{
	//	if ( headContours[headmaxIndex].size()>10 )
	//	{

	//		
	//		headContour=headContours[headmaxIndex];

	//		//ellipse=cv::fitEllipse(headContour);
	//		//ellipse.center.x+=boundRect.x;
	//		//ellipse.center.y+=boundRect.y;
	//		//headLength=ellipse.size.height;
	//		//headWidth=ellipse.size.width;
	//		//headEllipticity=float(headLength)/float(headWidth);
	//		//headArea=3.14*(headLength/2)*(headWidth/2);
	//		//acrosomeArea=headArea*0.4;
	//		//nucleusArea=headArea*0.6;

	//	}


	//}


	//detect vacuoles


}


void SingleSpermMorphology::DetectHeadOld(cv::Mat _src)//old ones
{
	cv::Mat spermRoiImage,spermRoiOpenImage, spermContourImage,headRoiImage;
	std::vector<cv::Vec4i> hierachy;
	//segment head
	_src(spermBound).copyTo(spermRoiImage);
	spermRoiImage.copyTo(spermRoiOpenImage);
	//
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));

	//seperate head
		cv::morphologyEx( spermRoiOpenImage, spermRoiOpenImage, cv::MORPH_OPEN, element,cv::Point(-1,-1),7 );// remove the branch on the sperm head 
		cv::morphologyEx( spermRoiOpenImage, spermRoiOpenImage, cv::MORPH_CLOSE, element,cv::Point(-1,-1),10 );//5 for seperating midpiece and tail 
		cv::morphologyEx( spermRoiOpenImage, spermRoiOpenImage, cv::MORPH_OPEN, element,cv::Point(-1,-1),10 );//5
		cv::morphologyEx( spermRoiOpenImage, spermRoiOpenImage, cv::MORPH_DILATE, element,cv::Point(-1,-1),5 );// enlarge headBound
	spermRoiOpenImage.copyTo(spermContourImage);

	std::vector<std::vector<cv::Point> > headOpencontours; 
	cv::findContours(spermContourImage, headOpencontours, hierachy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	int maxOpenheadArea=0,maxOpenheadIndex=0;
	int jj=0;
	if (headOpencontours.size()>0)
	{
		for (;jj>=0;jj=hierachy[jj][0])
		{
			if(headOpencontours[jj].size()>maxOpenheadArea )
			{
				maxOpenheadArea=headOpencontours[jj].size();
				maxOpenheadIndex=jj;
			}
		}
		headOpenContour=headOpencontours[maxOpenheadIndex];
	}




	//extract headContour
	headBound = cv::boundingRect(headOpencontours[maxOpenheadIndex]);
	originheadBound = cv::boundingRect(headOpencontours[maxOpenheadIndex]);
	spermRoiImage(headBound).copyTo(headRoiImage);

	cv::morphologyEx( headRoiImage, headRoiImage, cv::MORPH_OPEN, element,cv::Point(-1,-1), 7);//5  TO remove the branches on the head
	//cv::morphologyEx( headRoiImage, headRoiImage, cv::MORPH_OPEN, element,cv::Point(-1,-1),10 );
	cv::morphologyEx( headRoiImage, headRoiImage, cv::MORPH_DILATE, element,cv::Point(-1,-1),5 );//5

	headArea=cv::countNonZero(headRoiImage);

	std::vector<std::vector<cv::Point> > headcontours; 
	cv::findContours(headRoiImage, headcontours, hierachy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	 int maxheadArea=0,maxheadIndex=0;
	jj=0;
	if (headcontours.size()>0)
	{
		for (;jj>=0;jj=hierachy[jj][0])
		{

			if(headcontours[jj].size()>maxheadArea )

			{

				maxheadArea=headcontours[jj].size();

				maxheadIndex=jj;

			}

		}
		headContour=headcontours[maxheadIndex];
	}
	

	//enlarge headbound,headBound is later used for vacuole detection
	int headBorder=30;
	headBound.x=headBound.x+spermBound.x;
	headBound.y=headBound.y+spermBound.y;
	//headBound.width+=2*headBorder;
	//headBound.height+=2*headBorder;

	//originheadBound.x=originheadBound.x+spermBound.x;
	//originheadBound.y=originheadBound.y+spermBound.y;

	//offset headBound for headContour; spermBound for headOpenContour
	//head rotatebounding box
	//headRotateBound = cv::minAreaRect(headOpenContour);
	//headRotateBound.center.x+=spermBound.x;
	//headRotateBound.center.y+=spermBound.y;

	//ellipse fitting to get the orientation of the sperm head
	headEllipse=cv::fitEllipse(headContour);
	headEllipse.center.x += headBound.x;
	headEllipse.center.y += headBound.y;

	headLength=headEllipse.size.height;
	headWidth=headEllipse.size.width;
	headEllipticity=float(headLength)/float(headWidth);



}

void SingleSpermMorphology::DetectTailOld(cv::Mat _src,cv::Mat _output)
{
	cv::Mat tailImage,tailcontourImage;

	_src(spermBound).copyTo(tailImage);

	drawContours(tailImage,std::vector<std::vector<cv::Point> >(1,headOpenContour),-1,cv::Scalar(0,0,0),-1,8);

	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));     //CLOSE THE GAP ON THE TAIL, Tail has been cut during head and tail seperation
	//cv::morphologyEx( tailImage, tailImage, cv::MORPH_CLOSE, element,cv::Point(-1,-1),5 );

	tailImage.copyTo(tailcontourImage);

	std::vector<std::vector<cv::Point> >tailcontours;
	cv::findContours(tailcontourImage, tailcontours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	int tailContourLength,tailMaxLength=0,tailMaxIndex=0;
	if (tailcontours.size()>0)
	{
	}
	for (int ll=0; ll<tailcontours.size(); ++ll)
	{
		tailContourLength=tailcontours[ll].size();
		if(tailContourLength>tailMaxLength )//size is curve length

		{
			tailMaxLength=tailContourLength;
			tailMaxIndex=ll;
		}
	}


	tailImage.setTo(0);
	drawContours(tailImage,tailcontours,tailMaxIndex,cv::Scalar(255,255,255),-1,8);//filled contour
	tailContour=tailcontours[tailMaxIndex];

	int tailArea=cv::countNonZero(tailImage);
	tailWidth=float(tailArea)/float(tailMaxLength);

	tailLength=((float)tailMaxLength-2*tailWidth)*MicronPerPixel100X;//should devide 2, but tailMaxLength not equal to the real contour length. Contour points are not all connected.
	//std::cout<<tailWidth<<std::endl;
	//std::cout<<tailMaxLength<<std::endl;
	//std::cout<<tailLength<<std::endl;

	//judge whether curved,based on 
	isTailCurved=false;
	if (tailLength<200)
	{
		isTailCurved=true;
	}

	tailImage.copyTo(_output);

}

void SingleSpermMorphology::SegmentationOld(cv::Mat _src, cv::Mat _output)
{
	cv::Mat blurImage, binaryImage,binaryImageOrigin, imgHist,contourImage;
	//Contour
	std::vector<std::vector<cv::Point> > contours;
	cv::Rect recBound;

	cv::medianBlur(_src,blurImage,3);

	cv::equalizeHist(blurImage,imgHist);

	//Threshold
	binaryImage=imgHist>230;//230

	binaryImage.copyTo(binaryImageOrigin);// binaryimage will undergo morphological operation, binaryimageorigin is only binarizaton.


	//morphological to remove small particle
	int erosion_size = 1;  
	cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE,
		cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1)
		);
	cv::morphologyEx( binaryImage, binaryImage, cv::MORPH_OPEN, element,cv::Point(-1,-1),2 );

	binaryImage.copyTo(contourImage);

	cv::findContours(contourImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	int minSize=100,area,maxarea=0,maxIndex=0;

	//remove large particles
	for (int ii=0; ii<contours.size(); ++ii)
	{
		recBound = cv::boundingRect(contours[ii]);
		area=contours[ii].size();
		if ( area<minSize )
		{
			drawContours(binaryImage,contours,ii,cv::Scalar(0,0,0),-1,8);//not necessary
		}
		if(area>maxarea && recBound.x>10 && recBound.x<(binaryImage.rows-10))//
		{
			maxarea=area;
			maxIndex=ii;
		}
	}

	//output
	binaryImageOrigin.copyTo(_output);
	//binaryImage.copyTo(_output);
	spermContour=contours[maxIndex];
	spermBound = cv::boundingRect(contours[maxIndex]);
	//sperm rotatebounding box
	cv::RotatedRect spermRotateBound = cv::minAreaRect(spermContour);

	isSpermDetected=true;
	if (spermRotateBound.size.width<30 || spermRotateBound.size.height<30)
	{
		isSpermDetected=false;
	}
	//std::cout<<spermRotateBound.size.width<<std::endl;
	//std::cout<<spermRotateBound.size.height<<std::endl;

	int spermBorder=30;
	spermBound.x-=spermBorder;
	spermBound.y-=spermBorder;
	spermBound.width+=2*spermBorder;
	spermBound.height+=2*spermBorder;
}

void SingleSpermMorphology::DetectMidpieceOld(cv::Mat _src)
{
	cv::Mat tailImage,midpieceImage;
	_src.copyTo(tailImage);

	cv::Point2f headCenterInTailImage,tailCenterInTailImage;
	cv::Rect headBoundinTailImage = cv::boundingRect(headOpenContour);
	cv::Rect tailBoundinTailImage = cv::boundingRect(tailContour);
	cv::Rect midpieceBoundinTailImage;



	headCenterInTailImage.x = headBoundinTailImage.x+0.5*headBoundinTailImage.width;
	headCenterInTailImage.y = headBoundinTailImage.y+0.5*headBoundinTailImage.height;
	tailCenterInTailImage.x = tailBoundinTailImage.x+0.5*tailBoundinTailImage.width;
	tailCenterInTailImage.y = tailBoundinTailImage.y+0.5*tailBoundinTailImage.height;

	midpieceBoundinTailImage.width=headBoundinTailImage.width;//the rect may override the boundary
	int midpieceHeight=15;
	midpieceBoundinTailImage.height=midpieceHeight;

	//to prevent ROI overiding the boundary
	if (tailCenterInTailImage.y<headCenterInTailImage.y)
	{
		midpieceBoundinTailImage.y=(tailBoundinTailImage.y+tailBoundinTailImage.height-midpieceBoundinTailImage.height)>0?
			(tailBoundinTailImage.y+tailBoundinTailImage.height-midpieceBoundinTailImage.height):0;
	}
	else
	{
		midpieceBoundinTailImage.y=tailBoundinTailImage.y;
	}

	midpieceBoundinTailImage.height=(midpieceBoundinTailImage.y+midpieceBoundinTailImage.height)<tailImage.rows?
		midpieceHeight:(tailImage.rows-midpieceBoundinTailImage.y);


	if (tailCenterInTailImage.x<headCenterInTailImage.x)
	{
		midpieceBoundinTailImage.x=(tailBoundinTailImage.x+tailBoundinTailImage.width-midpieceBoundinTailImage.width)>0?
			(tailBoundinTailImage.x+tailBoundinTailImage.width-midpieceBoundinTailImage.width):0;
	} 
	else
	{
		midpieceBoundinTailImage.x=tailBoundinTailImage.x;
	}
	midpieceBoundinTailImage.width=(midpieceBoundinTailImage.x+midpieceBoundinTailImage.width)<tailImage.cols?
		headBoundinTailImage.width:(tailImage.cols-midpieceBoundinTailImage.x);


	//cv::rectangle(tailImage,headBoundinTailImage,cv::Scalar(255,255,255),1,8,0);
	//cv::rectangle(tailImage,midpieceBoundinTailImage,cv::Scalar(255,255,255),1,8,0);
	tailImage(midpieceBoundinTailImage).copyTo(midpieceImage);

	std::vector<std::vector<cv::Point> >midpiececontour; 
	cv::findContours(midpieceImage, midpiececontour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	int midpieceThisLength,midpieceMaxLength=0,midpieceMaxIndex=0;

	for (int mm=0; mm<midpiececontour.size(); ++mm)
	{
		midpieceThisLength=midpiececontour[mm].size();
		if(midpieceThisLength>midpieceMaxLength )//size is curve length

		{
			midpieceMaxLength=midpieceThisLength;
			midpieceMaxIndex=mm;
		}
	}

	//midpieceContour=midpiececontour[midpieceMaxIndex];

	//midpiece rotatebounding box
	midpieceRotateBound = cv::minAreaRect(midpiececontour[midpieceMaxIndex]);
	midpieceRotateBound.center.x=midpieceRotateBound.center.x+midpieceBoundinTailImage.x+spermBound.x;
	midpieceRotateBound.center.y=midpieceRotateBound.center.y+midpieceBoundinTailImage.y+spermBound.y;

	midpieceLength=midpieceRotateBound.size.height;
	midpieceWidth=midpieceRotateBound.size.width;
	
	//midpiece angle
	float midpieceAbsAngle=midpieceRotateBound.angle+180;
	float headAngle=headEllipse.angle;
	midpieceAngle=abs(midpieceAbsAngle-headAngle);

	//std::cout<<midpieceAbsAngle<<std::endl;
	//std::cout<<headAngle<<std::endl;

	isBentNeck=false;
	if (midpieceAngle>45)
	{
		isBentNeck=true;
	}

}

void SingleSpermMorphology::DetectVacuoleOld(cv::Mat _src, cv::Mat _output)
{
	cv::Mat headRestoreImage,vacuoleImage,vacuoleContourImage,headImgHist;

	_src(headBound).copyTo(headRestoreImage);
	cv::equalizeHist(headRestoreImage,headImgHist);
	vacuoleImage=headImgHist<30;
	vacuoleImage.copyTo(vacuoleContourImage);

	std::vector<std::vector<cv::Point> > vacuolecontours; 
	std::vector<cv::Vec4i> hierachy,vacuoleHierachy;
	cv::findContours(vacuoleContourImage, vacuolecontours, vacuoleHierachy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	
	int vacuoleIndex=0, smallRadius=headEllipse.size.height>headEllipse.size.width?headEllipse.size.width:headEllipse.size.height;
	double distance;
	int kk=0,leastDistance=100;
	isVacuole=false;
	cv::Point2f centerPt;
	for (;kk>=0;kk=vacuoleHierachy[kk][0])
	{
		cv::Moments mu = cv::moments(vacuolecontours[kk], true);
		centerPt.x = mu.m10/mu.m00+headBound.x;
		centerPt.y = mu.m01/mu.m00+headBound.y;
		distance=Methods::CalcDistance(headEllipse.center,centerPt);
			//cv::Rect vacuoleBound =cv::boundingRect(vacuolecontours[kk]);
			//distance=Methods::CalcDistance(s.center,cvPoint(vacuoleBound.x,vacuoleBound.y));
			//if (distance<leastDistance)//distance<double(smallRadius-40)
			//{
			//	leastDistance=distance;
			//	vacuoleIndex=kk;
			//}
			if (distance<10)
			{
				vacuoleIndex=kk;
				isVacuole=true;
				std::cout<<distance<<std::endl;
			}
	}
	
	vacuoleContour=vacuolecontours[vacuoleIndex];
	cv::Moments mu = cv::moments(vacuoleContour, true);
	centerPtVacuole.x=mu.m10/mu.m00+headBound.x;
	centerPtVacuole.y=mu.m01/mu.m00+headBound.y;

	vacuoleImage.copyTo(_output);
	
}
