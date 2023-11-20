#include "stdafx.h"
#include "DirectionVector.h"

DirectionVector::DirectionVector()
{
	m_num_vectors = 15;
	m_direction_vectors.resize(m_num_vectors);
	Initialize();
}

DirectionVector::DirectionVector(int num_vectors)
{
	if(num_vectors<=0)
	{
		m_num_vectors = 15;
	}
	else
	{
		m_num_vectors = num_vectors;
	}

	m_direction_vectors.resize(m_num_vectors);
	Initialize();
}

void DirectionVector::UpdatePrevDirections(cv::Point2f DirectionVector)
// PRE: The current direction 
// POS: Our vector of pevious directions is updated
{
	// do this for all vector slots except for m_prevDirections[0]
	for(int i=0;i<(m_num_vectors-1);i++)
	{
		m_direction_vectors[m_num_vectors - i - 1] = m_direction_vectors[m_num_vectors - i - 2];
	}

	m_direction_vectors[0].x = DirectionVector.x;
	m_direction_vectors[0].y = DirectionVector.y;

	m_n_frames++;
}

void DirectionVector::GetAvgDirectionVector(float &avg_dir_x, float &avg_dir_y)
{
	float avgDirectionX = 0;
	float avgDirectionY = 0;

	for(int i=0;i<m_num_vectors;i++)
	{
		avgDirectionX += m_direction_vectors[i].x;
		avgDirectionY += m_direction_vectors[i].y;
	}

	// average by the number of vectors we already have
	if(m_n_frames < m_num_vectors)
	{
		if(m_n_frames > 0)
		{
			avgDirectionX = avgDirectionX / float(m_n_frames);
			avgDirectionY = avgDirectionY / float(m_n_frames);	
		}
	}
	else
	{
		avgDirectionX = avgDirectionX / float(m_num_vectors);
		avgDirectionY = avgDirectionY / float(m_num_vectors);
	}

	avg_dir_x = avgDirectionX;
	avg_dir_y = avgDirectionY;
}


void DirectionVector::GetUnitDirectionVector(float &unit_dir_x, float &unit_dir_y)
{
	float avg_dir_x = 0.;
	float avg_dir_y = 0.;
	GetAvgDirectionVector(avg_dir_x, avg_dir_y);

	float magnitude = sqrt((float)(avg_dir_x*avg_dir_x+avg_dir_y*avg_dir_y));

	if(magnitude > 0)
	{
		unit_dir_x = avg_dir_x / magnitude;
		unit_dir_y = avg_dir_y / magnitude;

	}
	else
	{
		unit_dir_x = 0.;
		unit_dir_y = 0.;
	}
}

void DirectionVector::SetNumDirVectors(int num_vectors)
{
	if(num_vectors<=0)
	{
		m_num_vectors = 15;
	}
	else
	{
		m_num_vectors = num_vectors;
	}
	m_direction_vectors.clear();
	m_direction_vectors.resize(num_vectors);
}

void DirectionVector::Initialize()
{
	for(int i=0;i<(int)m_direction_vectors.size();i++)
	{
		m_direction_vectors[i] = cv::Point2f(0., 0.);
	}

	m_n_frames = 0;
}

void DirectionVector::Reset()
{
	Initialize();
}

bool DirectionVector::IsSufficientNumDirectionVectors()
{
	if(m_n_frames > m_num_vectors)
	{
		return true;
	}
	else
	{
		return false;
	}
}