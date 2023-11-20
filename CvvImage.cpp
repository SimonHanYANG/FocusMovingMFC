#include "StdAfx.h"
#include "CvvImage.h"
#include "opencv2/opencv.hpp"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CV_INLINE RECT NormalizeRect( RECT r );
CV_INLINE RECT NormalizeRect( RECT r )
{
   int t;
   if( r.left > r.right )
   {
      t = r.left;
      r.left = r.right;
      r.right = t;
   }
   if( r.top > r.bottom )
   {
      t = r.top;
      r.top = r.bottom;
      r.bottom = t;
   }

   return r;
}
CV_INLINE cv::Rect RectToCvRect( RECT sr );
CV_INLINE cv::Rect RectToCvRect( RECT sr )
{
   sr = NormalizeRect( sr );
   return cv::Rect( sr.left, sr.top, sr.right - sr.left, sr.bottom - sr.top );
}
CV_INLINE RECT CvRectToRect( cv::Rect sr );
CV_INLINE RECT CvRectToRect( cv::Rect sr )
{
   RECT dr;
   dr.left = sr.x;
   dr.top = sr.y;
   dr.right = sr.x + sr.width;
   dr.bottom = sr.y + sr.height;

   return dr;
}
CV_INLINE cv::Rect RectToROI(RECT r);
CV_INLINE cv::Rect RectToROI(RECT r)
{
	r = NormalizeRect(r);
	return cv::Rect(r.left, r.top, r.right - r.left, r.bottom - r.top);
}
void  FillBitmapInfo( BITMAPINFO* bmi, int width, int height, int bpp, int origin )
{
	std::cout << "bmi: " << bmi << std::endl;
	std::cout << "width: " << width << std::endl;
	std::cout << "height: " << height << std::endl;
	std::cout << "bpp: " << bpp << std::endl;

   assert( bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));

   BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);

   memset( bmih, 0, sizeof(*bmih));
   bmih->biSize = sizeof(BITMAPINFOHEADER);
   bmih->biWidth = width;
   bmih->biHeight = origin ? abs(height) : -abs(height);
   bmih->biPlanes = 1;
   bmih->biBitCount = (unsigned short)bpp;
   bmih->biCompression = BI_RGB;
   if( bpp == 8 )
   {
      RGBQUAD* palette = bmi->bmiColors;
      int i;
      for( i = 0; i < 256; i++ )
      {
         palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
         palette[i].rgbReserved = 0;
      }
   }
}
CvvImage::CvvImage()
{
	// 将 m_img 初始化为空 cv::Mat
	m_img = cv::Mat();
}

void CvvImage::Destroy()
{
	// 释放 m_img 中的指针和数据
	m_img.release();
	// 将 m_img 赋值为空 cv::Mat，释放 m_img 本身的内存
	m_img = cv::Mat();
}
CvvImage::~CvvImage()
{
   Destroy();
}
bool CvvImage::Create(int w, int h, int bpp, int origin)
{
	const unsigned max_img_size = 10000;
	std::cout << "bpp = " << bpp << std::endl;
std::cout << "w = " << w << std::endl;
std::cout << "h = " << h << std::endl;
std::cout << "max_img_size = " << max_img_size << std::endl;
std::cout << "origin = " << origin << std::endl;
if ((bpp != 8 && bpp != 24 && bpp != 32) ||
    (unsigned)w >= max_img_size || (unsigned)h >= max_img_size ||
    (origin != 0 && cv::Point(origin % w, origin / w) != cv::Point(0, 0)))
{
	std::cout << "xzt" << std::endl;
	std::cout << "jq" << std::endl;
    assert(0); // most probably, it is a programming error
    return false;
}

	if (m_img.empty() || m_img.depth() != CV_8U || m_img.channels() != bpp / 8 || m_img.cols != w || m_img.rows != h)
	{
		Destroy();
		/* prepare Mat header */
		 
		m_img.create(h, w, CV_MAKE_TYPE(CV_8U, bpp / 8));
		
	}
	std::cout << "!m_img.empty():" << !m_img.empty()<< std::endl;
	if (!m_img.empty())
		if (origin != 0 && cv::Point(origin % m_img.cols, origin / m_img.cols) != cv::Point(0, 0))
		{
			// 计算偏移量
			int offset = origin / m_img.cols * m_img.step + origin % m_img.cols * m_img.elemSize();

			// 创建新的图像对象，并将原图像数据复制到新对象中
			cv::Mat img;
			m_img.copyTo(img);

			// 更新原点
			m_img = cv::Mat(m_img.rows, m_img.cols, CV_MAKETYPE(CV_8U, m_img.channels()), m_img.data + offset);

			// 如果需要，将新对象转换为目标数据类型
			if (CV_MAT_DEPTH(m_img.type()) != CV_8U || m_img.channels() != img.channels())
			{
				img.convertTo(m_img, CV_MAKETYPE(CV_8U, m_img.channels()));
			}
		}
	return !m_img.empty();
}
void  CvvImage::CopyOf(CvvImage& image, int desired_color)
{
	cv::Mat img = image.m_img.clone();
	
	if (!img.empty())
	{
		CopyOf2(img, desired_color);
	}
}
#define HG_IS_IMAGE(img) \
   (!img.empty() && img.type() == CV_8UC1 || img.type() == CV_8UC3)
void CvvImage::CopyOf2(cv::Mat img, int desired_color)
{
	std::cout << "img1.depth(): " << img.depth() << std::endl;
	if (!img.empty())
	{
		int color = desired_color;
		cv::Size size = img.size();
		if (color < 0)
			color = img.channels() > 1;

		if (Create(size.width, size.height,
			 8,
			0))



		{
			cv::imshow("Image", img);
			cv::imwrite("D:/ZYB/6.jpg", img);
			std::cout << "img.depth(): " << img.depth() << std::endl;
			std::cout << "m_img.depth(): " << m_img.depth() << std::endl;
			std::cout << "img.channels(): " << img.channels() << std::endl;
			std::cout << "m_img.channels(): " << m_img.channels() << std::endl;
			img.convertTo(m_img, m_img.depth());
			std::cout << "img.depth2(): " << img.depth() << std::endl;
			cv::imshow("Gray Image", m_img);
		}
	}
}
bool CvvImage::Load(const char* filename, int desired_color)
{
	cv::Mat img = cv::imread(filename, desired_color);
	if (img.empty())
		return false;

	CopyOf2(img, desired_color);

	return true;
}
bool  CvvImage::LoadRect( const char* filename,
                   int desired_color, cv::Rect r )
{
   if( r.width < 0 || r.height < 0 ) return false;

   cv::Mat img = cv::imread(filename, desired_color);
   if (img.empty())
	   return false;

    
   if (r.width == 0 || r.height == 0)
   {
	   r.width = img.cols;
	   r.height = img.rows;
   }
   if (r.x > img.cols || r.y > img.rows ||
	   r.x + r.width < 0 || r.y + r.height < 0)
   {
	   return false;
   }
   /* truncate r to source image */
   if (r.x < 0)
   {
	   r.width += r.x;
	   r.x = 0;
   }
   if (r.y < 0)
   {
	   r.height += r.y;
	   r.y = 0;
   }
   if (r.x + r.width > img.cols)
	   r.width = img.cols - r.x;

   if (r.y + r.height > img.rows)
	   r.height = img.rows - r.y;

   cv::Mat img_roi = img(r).clone();
   CopyOf2(img_roi, desired_color);

   return true;;
}
void  CvvImage::Show( const char* window )
{
	if (!m_img.empty())
		cv::imshow(window, m_img);
}
void  CvvImage::Show( HDC dc, int x, int y, int w, int h, int from_x, int from_y )
{
	if (!m_img.empty() && m_img.depth() == CV_8U)
	{
		uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
		BITMAPINFO* bmi = (BITMAPINFO*)buffer;
		int bmp_w = m_img.cols, bmp_h = m_img.rows;
		std::cout << "m_img.channels(): " << m_img.channels() << std::endl;
		std::cout << "m_img.depth(): " << m_img.depth() << std::endl;
		FillBitmapInfo(bmi, bmp_w, bmp_h, Bpp(), m_img.step * (m_img.rows - 1));

		from_x = MIN(MAX(from_x, 0), bmp_w - 1);
		from_y = MIN(MAX(from_y, 0), bmp_h - 1);
		int sw = MAX(MIN(bmp_w - from_x, w), 0);
		int sh = MAX(MIN(bmp_h - from_y, h), 0);

		SetDIBitsToDevice(dc, x, y, sw, sh, from_x, from_y, from_y, sh,
			m_img.data + from_y * m_img.step,
			bmi, DIB_RGB_COLORS);
	}
}
void  CvvImage::DrawToHDC(HDC hDCDst, RECT* pDstRect)
{
	if (pDstRect && !m_img.empty() && m_img.depth() == CV_8U && m_img.data)
	{
		uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
		BITMAPINFO* bmi = (BITMAPINFO*)buffer;
		int bmp_w = m_img.cols, bmp_h = m_img.rows;
		cv::Rect roi = cv::Rect(0, 0, bmp_w, bmp_h);
		if (!m_img.isContinuous())
		{
			roi = roi & cv::Rect(0, 0, m_img.cols, m_img.rows);
		}

		cv::Rect dst = RectToCvRect(*pDstRect);
		if (roi.width == dst.width && roi.height == dst.height)
		{
			Show(hDCDst, dst.x, dst.y, dst.width, dst.height, roi.x, roi.y);
			return;
		}

		if (roi.width > dst.width)
		{
			SetStretchBltMode(
				hDCDst,           // handle to device context
				HALFTONE);
		}
		else
		{
			SetStretchBltMode(
				hDCDst,           // handle to device context
				COLORONCOLOR);
		}

		FillBitmapInfo(bmi, bmp_w, bmp_h, Bpp(), m_img.isContinuous() ? 0 : 1);
		::StretchDIBits(
			hDCDst,
			dst.x, dst.y, dst.width, dst.height,
			roi.x, roi.y, roi.width, roi.height,
			m_img.data, bmi, DIB_RGB_COLORS, SRCCOPY);
	}
}
void CvvImage::Fill(int color)
{
	m_img.setTo(cv::Scalar(color & 255, (color >> 8) & 255, (color >> 16) & 255, (color >> 24) & 255));
}
