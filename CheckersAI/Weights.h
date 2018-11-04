#pragma once
#include "Common.h"

class Weights
{
public:
	Weights()
	{

	}
	Weights(vector<double> weights, int depth) : m_weights(weights), m_count(weights.size()), m_depth(depth) {}
	Weights(int count, int depth) : m_count(count), m_depth(depth)
	{
		for (int i = 0; i < count; i++)
		{
			// because this weighting worked in first tests
			int weighting = i % 2 == 0 ? 10 : 20;
			m_weights.push_back(double(rand()) / double(RAND_MAX) * weighting);
		}
	}
	Weights(int count) : m_count(count)
	{
		for (int i = 0; i < count; i++)
		{
			// because this weighting worked in first tests
			int weighting = i % 2 == 0 ? 10 : 20;
			m_weights.push_back(double(rand()) / double(RAND_MAX) * weighting);
		}
		int randInt = rand() % 2 + 4;
		m_depth = randInt;
	}
	vector<double> m_weights;
	int m_depth;
	int m_count;
}; 
