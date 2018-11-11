#pragma once
#include "Agent.h"
#include "RandomAgent.h"
#include <numeric>
#include <fstream>

#define POOL_SIZE 100
#define CROSS_OVER_RATE 0.9
#define MUTATION_RATE 0.05
#define WIN_BONUS 100
#define FITNESS_TOLERANCE 1e-4f
#define DEPTH_MUTATION_RATE 0.2
#define WINNER_POOL_SIZE 3
#define NO_MOVES_PENALTY 50

class GenAlgo
{
public:
	GenAlgo(Weights testWeights) : 
		m_weightCount(testWeights.m_weights.size()), 
		m_testWeights(testWeights), 
		m_testRandomAgent(Color::RED)
	{
		for (int i = 0; i < POOL_SIZE; i++)
			m_candidateWeights.emplace_back(m_weightCount);
		m_selectedWeights.reserve(POOL_SIZE);
		for (int i = 0; i < m_weightCount; i++)
		{
			m_lowerBounds.push_back(0);
			m_upperBounds.push_back(i % 2 == 0 ? 10 : 20);
		}
		m_depthLowerBound = 4;
		m_depthUpperBound = 5;
		fill(m_bestFitnesses, m_bestFitnesses + WINNER_POOL_SIZE, -1);

		reportStream.open(R"(C:\Users\Matt\Desktop\CS664\CS663_FinalProject_MatthewScott\Tournament6.txt)", std::ostream::out);
		reportStream << "Candidate,Weight1,Weight2,Weight3,Weight4,MaxDepth,MoveCount,WL,Fitness" << endl;
	};

	void UpdateFitness(Weights weights, double fitness)
	{
		vector<int> order(WINNER_POOL_SIZE);
		iota(order.begin(), order.end(), 0);
		auto& fitnesses = m_bestFitnesses;
		sort(order.begin(), order.end(), [&fitnesses] (int i, int j) {
			return fitnesses[i] < fitnesses[j];
		});
		for (int i = 0; i < WINNER_POOL_SIZE; i++)
		{
			if (fitness > m_bestFitnesses[order[i]])
			{
				m_bestFitnesses[order[i]] = fitness;
				m_bestWeights[order[i]] = weights;
				break;
			}
		}
	}

	void Selection()
	{
		double totalFitness = 0;
		double fitnessArr[POOL_SIZE];
		double fitnessRef[POOL_SIZE];
		for (int i = 0; i < POOL_SIZE; i++)
		{
			cout << "Candidate " << i << endl;
			double fitness = 0;
			auto board = Board();
			auto candidateAgent = Agent(board, Color::BLACK, m_candidateWeights[i]);
			auto testAgent = Agent(board, Color::RED, m_testWeights);
			reportStream << i;
			for (int j = 0; j < m_candidateWeights[i].m_weights.size(); j++)
				reportStream << "," << m_candidateWeights[i].m_weights[j];
			reportStream << "," << m_candidateWeights[i].m_depth << ",";
			while (true)
			{
				if (board.m_moveCount > WIN_BONUS)
					board.getPieces(Color::BLACK).clear();
				
				if (!board.NoWinner())
					break;
				candidateAgent.Move();
				//cout << endl;
				//board.Print();

				//cout << endl;

				if (!board.NoWinner())
					break;
				testAgent.Move();
				//board.Print();
			}

			reportStream << board.m_moveCount;
			// if agent wins, gets win bonus, plus bonus for less moves
			//  otherwise bonus is based on number of moves only at a max of the win bonus
			if (board.getWinner() == candidateAgent.m_color)
			{
				reportStream << ",W,";
				fitness += (WIN_BONUS + max(WIN_BONUS + int(board.getPieces(candidateAgent.m_color).size()) * 10, 0));
			}
			else
			{
				reportStream << ",L,";
				fitness += min(120 - int(board.getPieces(Common::OtherColor(candidateAgent.m_color)).size()) * 10, WIN_BONUS);
			}

			if (board.m_noMoves && board.getWinner() == candidateAgent.m_color)
				fitness = max(double(0), fitness - NO_MOVES_PENALTY);
			
			reportStream << fitness << endl;

			fitnessRef[i] = fitness;
			totalFitness += fitness;

			UpdateFitness(m_candidateWeights[i], fitness);
			fitnessArr[i] = totalFitness;
		}

		for (int i = 0; i < POOL_SIZE; i++)
		{
			fitnessArr[i] /= totalFitness;
		}

		m_selectedWeights.clear();
		for (int i = 0; i < POOL_SIZE; i++)
		{
			double randNum = rand();
			randNum /= RAND_MAX;
			double lowerBound = 0; 
			for (int j = 0; j < POOL_SIZE; j++)
			{
				if (randNum >= lowerBound && randNum <= fitnessArr[j] + FITNESS_TOLERANCE)
				{
					m_selectedWeights.emplace_back(m_candidateWeights[j]);
					break;
				}
				lowerBound = fitnessArr[j];
			}

		}
	}

	void CrossOver()
	{
		for (int i = 0; i < POOL_SIZE / 2; i++)
		{
			double randProb = rand();
			randProb /= RAND_MAX;
			if (randProb > CROSS_OVER_RATE)
			{
				int randPoint = rand() % (m_weightCount) + 1;
				for (int j = 0; j < randPoint; j++)
				{
					m_candidateWeights[i * 2].m_weights[j]		= m_selectedWeights[i * 2].m_weights[j];
					m_candidateWeights[i * 2 + 1].m_weights[j]	= m_selectedWeights[i * 2 + 1].m_weights[j];
				}
				for (int j = randPoint; j < m_weightCount; j++)
				{
					m_candidateWeights[i * 2 + 1].m_weights[j]	= m_selectedWeights[i * 2].m_weights[j];
					m_candidateWeights[i * 2].m_weights[j]		= m_selectedWeights[i * 2 + 1].m_weights[j];
				}
				//depth
				if (randPoint < m_weightCount)
				{
					m_candidateWeights[i * 2].m_depth		= m_selectedWeights[i * 2].m_depth;
					m_candidateWeights[i * 2 + 1].m_depth	= m_selectedWeights[i * 2 + 1].m_depth;
				}
				else
				{
					m_candidateWeights[i * 2 + 1].m_depth	= m_selectedWeights[i * 2].m_depth;
					m_candidateWeights[i * 2].m_depth		= m_selectedWeights[i * 2 + 1].m_depth;
				}
			}
		}
	}

	void Mutation()
	{
		for (int i = 0; i < POOL_SIZE; i++)
		{
			for (int j = 0; j < m_weightCount; j++)
			{
				double randNum = rand();
				randNum /= RAND_MAX;
				if (randNum > MUTATION_RATE)
				{
					//TODO try simulated annealing here instead
					double randMut = rand();
					randMut /= RAND_MAX;
					randMut *= m_upperBounds[j] * 2;
					randMut -= m_upperBounds[j];
					m_selectedWeights[i].m_weights[j] += randMut;

					if (m_selectedWeights[i].m_weights[j] > m_upperBounds[j])
						m_selectedWeights[i].m_weights[j] = m_upperBounds[j];
					if (m_selectedWeights[i].m_weights[j] < m_lowerBounds[j])
						m_selectedWeights[i].m_weights[j] = m_lowerBounds[j];
				}
			}
			double randNum = rand();
			randNum /= RAND_MAX;
			if (randNum > MUTATION_RATE)
			{
				double randMut = rand();
				randMut /= RAND_MAX;
				if (randMut > 1 - DEPTH_MUTATION_RATE)
					m_selectedWeights[i].m_depth = min(m_depthUpperBound, m_selectedWeights[i].m_depth + 1);
				if (randMut < DEPTH_MUTATION_RATE)
					m_selectedWeights[i].m_depth = max(m_depthLowerBound, m_selectedWeights[i].m_depth - 1);
			}
		}
	}
	void PrintBestAgent()
	{
		cout << "Best Agent" << endl;
		for (int i = 0; i < WINNER_POOL_SIZE; i++)
		{
			for (int j = 0; j < m_weightCount; j++)
			{
				cout << m_bestWeights[i].m_weights[j] << endl;
			}
			cout << m_bestWeights[i].m_depth << endl;
		}
	}


	Weights m_testWeights;
	Weights m_bestWeights[WINNER_POOL_SIZE];
	double m_bestFitnesses[WINNER_POOL_SIZE];
	RandomAgent m_testRandomAgent;
	vector<Weights> m_candidateWeights;
	vector<Weights> m_selectedWeights;
	vector<double> m_lowerBounds;
	vector<double> m_upperBounds;
	int m_depthUpperBound;
	int m_depthLowerBound;
	int m_weightCount;
	ofstream reportStream;
};