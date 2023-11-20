#include "stdafx.h"
#include "Camera.h"

Camera::Camera() :
	m_camera_started(false),
	m_camera_connected(true),
	m_img_data(nullptr)
{
	// 分配图像缓存
	/*for (uint32_t i = 0; i < BUFFER_SIZE; ++i)
	{
		unsigned char* buffer = new unsigned char[WIDTH * HEIGHT];
		m_buffer_queue.push(buffer);
	}*/
}

Camera::~Camera()
{
	//Stop();
	
}


bool Camera::Start()
{
	// Before using any pylon methods, the pylon runtime must be initialized. 
	PylonInitialize();

	try {
		CTlFactory& tlFactory = CTlFactory::GetInstance();

		// 获取可用的相机数量
		// Get all attached cameras and exit application if no camera is found
		DeviceInfoList_t devices;

		// 如果没有可用的相机，直接返回
		if (tlFactory.EnumerateDevices(devices) == 0)
		{
			std::cerr << "No camera found." << std::endl;
			m_camera_connected = false;
			return -1;
		}

		// 创建一个 IPylonDevice 对象
		IPylonDevice* pDevice = tlFactory.CreateFirstDevice();

		m_camera.Attach(pDevice, Cleanup_Delete);

		// 打开相机
		m_camera.Open();
		

		// 设定相机参数
		CIntegerParameter width(m_camera.GetNodeMap(), "Width");
		CIntegerParameter height(m_camera.GetNodeMap(), "Height");
		CIntegerParameter offsetX(m_camera.GetNodeMap(), "OffsetX");
		CIntegerParameter offsetY(m_camera.GetNodeMap(), "OffsetY");
		CIntegerParameter exposureTime(m_camera.GetNodeMap(), "ExposureTimeRaw");

		//width.TrySetValue(WIDTH);
		//height.TrySetValue(HEIGHT);
		//width.TrySetValue(1200);
		//height.TrySetValue(900);
		offsetX.TrySetValue(OFFSET_X);
		offsetY.TrySetValue(OFFSET_Y);
		exposureTime.TrySetValue(EXPOSURE_TIME);
		
		// 获取BinningHorizontal和BinningVertical参数对象，并设置参数值 支持mono
		//CIntegerParameter binningHorizontal(m_camera.GetNodeMap(), "BinningHorizontal");
		//CIntegerParameter binningVertical(m_camera.GetNodeMap(), "BinningVertical");
		// 设置相机的缩放模式为等比例缩放
		//binningHorizontal.SetValue(1);
		//binningVertical.SetValue(1);

		// 获取DecimationHorizontal和DecimationVertical参数对象，并设置参数值 不支持
		//CFloatParameter decimationHorizontal(m_camera.GetNodeMap(), "DecimationHorizontal");
		//CFloatParameter decimationVertical(m_camera.GetNodeMap(), "DecimationVertical");
		// 设置相机的缩放模式为等比例缩放
		//decimationHorizontal.SetValue(2);
		//decimationVertical.SetValue(2);

		// 获取ScalingHorizontal和ScalingVertical参数对象，并设置参数值 不支持
		//CFloatParameter scalingHorizontal(m_camera.GetNodeMap(), "ScalingHorizontal");
		//CFloatParameter scalingVertical(m_camera.GetNodeMap(), "ScalingVertical");
		// 设置相机的缩放模式为等比例缩放
		//scalingHorizontal.SetValue(1);
		//scalingVertical.SetValue(1);

		// 设置相机的分辨率为指定值
		width.SetValue(1920);
		height.SetValue(1200);


		CEnumParameter(m_camera.GetNodeMap(), "PixelFormat").SetValue(PIXEL_FORMAT);

		// 获取像素格式并设置相应的 EPixelType
		/*EPixelType pixelType = PixelType_Mono8;
		size_t imageSize = WIDTH * HEIGHT;

		m_img_data = new unsigned char[WIDTH * HEIGHT];
		*/
		// 分配图像缓冲区
		//m_image.AttachUserBuffer(reinterpret_cast<unsigned char*>(m_img_data), imageSize, pixelType, WIDTH, HEIGHT, 0);
		//m_camera.MaxNumBuffer.SetValue(BUFFER_SIZE);

		m_camera.StartGrabbing();

		m_camera_started = true;
	}
	catch (const GenericException& e) {
		std::cerr << "Error starting camera: " << e.GetDescription() << std::endl;
		return false;
	}

	return true;
}

bool Camera::Stop()
{
	try {
		if (!m_camera_started) {
			return false;
		}

		// stop the camera acquisition
		m_camera.StopGrabbing();

		// close the camera
		m_camera.Close();

		m_camera_started = false;

		return true;
	}
	catch (const GenericException& e) {
		cerr << "An exception occurred:" << endl
			<< e.GetDescription() << endl;
		return false;
	}
}

//bool Camera::Stop()
//{
//	if (m_camera_started) {
//		try {
//			// 停止相机采集
//			m_camera.StopGrabbing();
//
//			// 释放图像缓冲区
//			while (!m_buffer_queue.empty()) {
//				delete[] m_buffer_queue.front();
//				m_buffer_queue.pop();
//			}
//
//			// 关闭相机
//			m_camera.Close();
//		}
//		catch (const GenericException& e) {
//			std::cerr << "Error stopping camera: " << e.GetDescription() << std::endl;
//			return false;
//		}
//
//		m_camera_started = false;
//
//		PylonTerminate(); // 终止 Pylon 运行时系统
//	}
//
//	return true;
//}

bool Camera::AcquireImage()
{
	try {
		if (!m_camera_started) {
			return false;
		}

		// retrieve the next image from the camera
		CGrabResultPtr ptrGrabResult;
		m_camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);

		// check if the image acquisition was successful
		if (ptrGrabResult->GrabSucceeded()) {
			// copy the image data to the m_img_data variable
			const uint8_t* buffer = (uint8_t*)ptrGrabResult->GetBuffer();
			// 获取图像的宽度和高度
			//int width = ptrGrabResult->GetWidth();
			//int height = ptrGrabResult->GetHeight();
			//printf("width=%d,height=%d\n",width,height);//1920 1200
			//m_img_data = new char(ptrGrabResult->GetPayloadSize());

			m_img_data = (char*)buffer;

			return true;
		}
		else {
			cerr << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
			return false;
		}
	}
	catch (const GenericException& e) {
		cerr << "An exception occurred:" << endl
			<< e.GetDescription() << endl;
		return false;
	}
}

void Camera::ReleaseMemory()
{
	//delete[] m_img_data;      // 释放m_img_data所占用的内存空间
	//m_img_data = nullptr;
}
//bool Camera::AcquireImage()
//{
//	if (!m_camera_started) {
//		std::cerr << "Camera not started" << std::endl;
//		return false;
//	}
//
//	// 等待图像帧进入 buffer
//	if (m_buffer_queue.empty()) {
//		if (!m_camera.WaitForFrameTriggerReady(10000, TimeoutHandling_ThrowException)) {
//			std::cerr << "Timeout waiting for frame" << std::endl;
//			return false;
//		}
//		return true;
//	}
//
//	// 从 buffer 中获取图像帧数据
//	/*m_img_data = m_buffer_queue.front();
//	m_buffer_queue.pop();*/
//
//	// 获取最新的图像帧
//	CGrabResultPtr grab_result;
//	try {
//		if (!m_camera.RetrieveResult(1000, grab_result, TimeoutHandling_Return)) {
//			std::cerr << "Error retrieving frame" << std::endl;
//			return false;
//		}
//	}
//	catch (const GenericException& e) {
//		std::cerr << "Error retrieving frame: " << e.GetDescription() << std::endl;
//		return false;
//	}
//
//	// 拷贝图像数据到缓冲区
//	memcpy(m_img_data, grab_result->GetBuffer(), grab_result->GetImageSize());
//
//	// 将图像帧数据放回 buffer
//	//if (m_buffer_queue.size() < BUFFER_SIZE) {
//	//	m_buffer_queue.push(m_img_data);
//	//}
//	//else {
//	//	delete[] m_img_data;
//	//	// 为了避免内存泄漏，可以在释放 m_img_data 指向的内存之后，将 m_img_data 设置为 nullptr 或清空，以确保该指针不再指向已释放的内存空间。
//	//	m_img_data = nullptr;
//	//}
//
//	return true;
//}
