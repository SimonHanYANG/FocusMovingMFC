#pragma once

#include <vector>
#include "OpenCV.h"

class DirectionVector
{
public:
	DirectionVector();
	DirectionVector(int num_vectors);
	void UpdatePrevDirections(cv::Point2f DirectionVector);
	void GetAvgDirectionVector(float &avg_dir_x, float &avg_dir_y);
	void GetUnitDirectionVector(float &unit_dir_x, float &unit_dir_y);
	int& NumFrames(){return m_n_frames;};		// internal frame counter to count number of available direction vectors
	int GetNumDirVectors(){return m_num_vectors;};
	void SetNumDirVectors(int num_vectors);
	void Initialize();
	void Reset();	// sets all direction vectors to (0,0)
	bool IsSufficientNumDirectionVectors();

protected:
	int m_num_vectors;
	int m_n_frames;
	std::vector<cv::Point2f> m_direction_vectors;	// stores direction vectors, can be used to find the average direction the sperm is moving in

};
