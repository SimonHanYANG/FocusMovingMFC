#pragma once
#ifndef CVVIMAGE_CLASS_DEF
#define CVVIMAGE_CLASS_DEF
#include "opencv2/core/types_c.h"

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <opencv2/opencv.hpp>
/* CvvImage class definition */
class  CvvImage
{
public:
	CvvImage();
	virtual ~CvvImage();
	/* Create image (BGR or grayscale) */
	virtual bool  Create(int width, int height, int bits_per_pixel, int image_origin = 0);
	/* Load image from specified file */
	virtual bool  Load(const char* filename, int desired_color = cv::IMREAD_COLOR);
	/* Load rectangle from the file */
	virtual bool  LoadRect(const char* filename,
		int desired_color, cv::Rect r);
#if defined WIN32 || defined _WIN32
	virtual bool  LoadRect(const char* filename,
		int desired_color, RECT r)
	{
		return LoadRect(filename, desired_color,
			cv::Rect(r.left, r.top, r.right - r.left, r.bottom - r.top));
	}
#endif
	/* Save entire image to specified file. */
	//virtual bool  Save(const char* filename);
	/* Get copy of input image ROI */
	virtual void  CopyOf(CvvImage& image, int desired_color = -1);
	virtual void  CopyOf2(cv::Mat img, int desired_color = -1);
	cv::Mat& GetImage() { return m_img; }
	virtual void  Destroy(void);
	/* width and height of ROI */
	int Width() { return !m_img.empty() ? (m_img.channels() == 1 ? m_img.cols : m_img.size().width) : 0; }
	int Height() { return !m_img.empty() ? (m_img.channels() == 1 ? m_img.rows : m_img.size().height) : 0; }
	int Bpp() { return !m_img.empty() ? (m_img.depth() & 255) * m_img.channels() : 0; }
	virtual void  Fill(int color);
	/* draw to highgui window */
	virtual void  Show(const char* window);

#if defined WIN32 || defined _WIN32
	/* draw part of image to the specified DC */
	virtual void  Show(HDC dc, int x, int y, int width, int height,
		int from_x = 0, int from_y = 0);
	/* draw the current image ROI to the specified rectangle of the destination DC */
	virtual void  DrawToHDC(HDC hDCDst, RECT* pDstRect);
#endif
protected:
	cv::Mat  m_img;
};
typedef CvvImage CImage;
#endif