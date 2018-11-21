#pragma once
#include "Common.h"

class IWeight
{
public:
	virtual void CrossOverRight(IWeight* other, int crossOverPoint) = 0;
	virtual void CrossOverLeft(IWeight* other, int crossOverPoint) = 0;
	virtual void Mutate(double mutationRate, int searchIteration, int maxIteration) = 0;
	virtual double getWeight(int i) = 0;
	virtual void setWeight(double weight, int i) = 0;
	virtual int getWeightCount() = 0;
};

class IWeightBase : public IWeight
{
public:
	virtual void CrossOverRight(IWeight* other, int crossOverPoint) override { throw new exception("Cant be in WeightBase"); };
	virtual void CrossOverLeft(IWeight* other, int crossOverPoint) override { throw new exception("Cant be in WeightBase"); };
	virtual void Mutate(double mutationRate, int searchIteration, int maxIteration) override { throw new exception("Cant be in WeightBase"); };
	virtual double getWeight(int i) override { throw new exception("Cant be in WeightBase"); };
	virtual void setWeight(double weight, int i) { throw new exception("Cant be in WeightBase"); };
	virtual int getWeightCount() override { throw new exception("Cant be in WeightBase"); };
};

class PieceWeights : public IWeightBase
{
public:
	PieceWeights()
	{
		m_count = 4;
		for (int i = 0; i < m_count; i++)
		{
			// because this weighting worked in first tests
			int weighting = i % 2 == 0 ? 10 : 20;
			m_weights.push_back(double(rand()) / double(RAND_MAX) * weighting);
		}
	}
	PieceWeights(PieceWeights* other)
	{
		for (auto i = 0; i < getWeightCount(); i++)
			setWeight(other->getWeight(i), i);
	}
	PieceWeights(vector<double> weights, int depth) : m_weights(weights), m_count(weights.size()) {}
	virtual void CrossOverRight(IWeight* other, int crossOverPoint) override
	{
		for (auto i = crossOverPoint; i < m_weights.size(); i++)
			m_weights[i] = other->getWeight(i);
	}
	virtual void CrossOverLeft(IWeight* other, int crossOverPoint) override
	{
		for (auto i = 0; i < crossOverPoint; i++)
			m_weights[i] = other->getWeight(i);
	}
	virtual void Mutate(double mutationRate, int searchIteration, int maxIteration) override
	{
		for (auto i = 0; i < m_weights.size(); i++)
		{
			double randNum = rand();
			randNum /= RAND_MAX;
			if (randNum < mutationRate)
			{
				//TODO try simulated annealing here instead
				double randMut = rand();
				randMut /= RAND_MAX;
				randMut *= m_upperBounds[i] * 2;
				randMut -= m_upperBounds[i];
				m_weights[i] += randMut;

				if (m_weights[i] > m_upperBounds[i])
					m_weights[i] = m_upperBounds[i];
				if (m_weights[i] < m_lowerBounds[i])
					m_weights[i] = m_lowerBounds[i];
			}
		}
	}
	virtual double getWeight(int i) override
	{
		if (i < 0 || i > 3)
			throw exception("out of bounds");
		return m_weights[i];
	}
	virtual void setWeight(double weight, int i) 
	{ 
		m_weights[i] = weight;
	};
	virtual int getWeightCount() override { return m_weights.size(); }

	vector<double> m_weights;
	vector<double> m_lowerBounds = { 0,0,0,0 };
	vector<double> m_upperBounds = { 10,20,10,20 };
	int m_count;
}; 


class PieceSquareWeights : public IWeightBase
{
public:
	PieceSquareWeights()
	{
		m_count = 36;
		for (int i = 0; i < m_count; i++)
		{
			// because this weighting worked in first tests
			double weighting = m_upperBounds[i];
			m_weights.push_back(double(rand()) / double(RAND_MAX) * weighting);
		}
	}
	PieceSquareWeights(PieceSquareWeights* other)
	{
		for (auto i = 0; i < getWeightCount(); i++)
			setWeight(other->getWeight(i), i);
	}
	PieceSquareWeights(vector<double> weights) : m_weights(weights), m_count(weights.size()) {}

	virtual void CrossOverRight(IWeight* other, int crossOverPoint) override
	{
		for (auto i = crossOverPoint; i < m_weights.size(); i++)
			m_weights[i] = other->getWeight(i);
	}
	virtual void CrossOverLeft(IWeight* other, int crossOverPoint) override
	{
		for (auto i = 0; i < crossOverPoint; i++)
			m_weights[i] = other->getWeight(i);
	}
	virtual void Mutate(double mutationRate, int searchIteration, int maxIteration) override
	{
		//	Simulated Annealing like Mutation
		//		more often in later iterations, up to every iteration and weight
		double lateIterationIncrease = double(searchIteration) / double(maxIteration);
		//		but less extreme movements later on
		//			assuming 100 iterations,
		//			multiplier is 1 on first iteration (zeroth iteration)
		//			0.99 on second iteration (first iteration)
		//			0 on last iteration (never reached)
		//			0.01 on second to last iteration
		double multiplier = double(maxIteration - searchIteration) / double(maxIteration);

		for (auto i = 0; i < m_weights.size(); i++)
		{
			double randNum = rand();
			randNum /= RAND_MAX;
			if (randNum < mutationRate + lateIterationIncrease)
			{
				double maxDistance = (m_upperBounds[i] - m_lowerBounds[i]) / 2; // never want to move more than half

				double mutation = NormalDistribution::getRandomNormalValue(maxDistance * multiplier);
				m_weights[i] += mutation;

				if (m_weights[i] > m_upperBounds[i])
					m_weights[i] = m_upperBounds[i];
				if (m_weights[i] < m_lowerBounds[i])
					m_weights[i] = m_lowerBounds[i];
			}
		}
	}
	virtual double getWeight(int i) override
	{
		if (i < 0 || i > m_count - 1)
			throw exception("out of bounds");
		return m_weights[i];
	}
	virtual void setWeight(double weight, int i)
	{
		m_weights[i] = weight;
	};
	virtual int getWeightCount() override { return m_weights.size(); }

	vector<double> m_weights;
	vector<double> m_lowerBounds = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	vector<double> m_upperBounds = { 10,20,10,20,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5 };
	int m_count;
};

