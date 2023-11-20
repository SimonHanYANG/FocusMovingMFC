//////////////////////////////////////////////////////////////////////

// Camera Class

// The Camera class is a wrapper class that uses CGrabBuffer
// to control Basler Cameras (e.g., scA-1300 and 601F)

#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include <queue>

#include <pylon/PylonIncludes.h>
#include <opencv2/opencv.hpp>

#include "ParameterDefinition.h"

using namespace Pylon;

using namespace std;

using namespace cv;

typedef CInstantCamera Camera_t;

// 相机参数
static const uint32_t WIDTH = nImgWidth;
static const uint32_t HEIGHT = nImgHeight;
static const uint32_t OFFSET_X = 0;
static const uint32_t OFFSET_Y = 0;
static const uint32_t EXPOSURE_TIME = 5000; //c
static const String_t PIXEL_FORMAT = "Mono8"; //"BGR8";// 
static const uint32_t BUFFER_SIZE = 1;


// 相机类
class Camera
{
public:
	Camera();
	~Camera();

	bool Start();
	bool Stop();
	bool AcquireImage();

	bool IsStarted() { return m_camera_started; };			// Checks if camera is initialized
	bool IsConnected() { return m_camera_connected; };		// Checks if camera is connected

	//unsigned char* ImageData() const { return m_img_data; }
	char* ImageData() const { return m_img_data; }
	void ReleaseMemory();

private:
	Camera_t m_camera;
	//CPylonImage m_image;
	//unsigned char* m_img_data = nullptr;
	char* m_img_data;
	//char* m_img_data = new char[WIDTH * HEIGHT];
	//std::queue<unsigned char*> m_buffer_queue;
	bool m_camera_started;
	bool m_camera_connected;						// true when camera is physically connected to machine (default true)
};



#endif