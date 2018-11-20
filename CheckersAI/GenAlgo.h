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
	GenAlgo(IWeight* testWeights) :
		m_testWeights(testWeights), 
		m_testRandomAgent(Color::RED)
	{
		m_candidateWeights.resize(POOL_SIZE);
		for (int i = 0; i < POOL_SIZE; i++)
			m_candidateWeights[i] = new PieceSquareWeights();
		m_selectedWeights.reserve(POOL_SIZE);
		
		fill(m_bestFitnesses, m_bestFitnesses + WINNER_POOL_SIZE, -1);

		reportStream.open(R"(C:\Users\Matt\Desktop\CS664\CS664_FinalProject_MatthewScott\TournamentTest.txt)", std::ostream::out);
		reportStream << "Candidate";
		for (auto i = 0; i < m_candidateWeights[0]->getWeightCount(); i++)
			reportStream << ",Weight" + to_string(i+1);
		reportStream << ",MoveCount,WL,Fitness" << endl;
	};

	void UpdateFitness(IWeight* weights, double fitness)
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
			for (int j = 0; j < m_candidateWeights[i]->getWeightCount(); j++)
				reportStream << "," << m_candidateWeights[i]->getWeight(j);
			reportStream.flush();

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

			reportStream << "," << board.m_moveCount;
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
				int randPoint = rand() % (m_candidateWeights[i * 2]->getWeightCount()) + 1;

				m_candidateWeights[i * 2]		->CrossOverLeft(m_selectedWeights[i * 2], randPoint);
				m_candidateWeights[i * 2 + 1]	->CrossOverLeft(m_selectedWeights[i * 2 + 1], randPoint);

				m_candidateWeights[i * 2 + 1]	->CrossOverRight(m_selectedWeights[i * 2], randPoint);
				m_candidateWeights[i * 2]		->CrossOverRight(m_selectedWeights[i * 2 + 1], randPoint);
			}
		}
	}

	void Mutation()
	{
		for (int i = 0; i < POOL_SIZE; i++)
		{
			m_selectedWeights[i]->Mutate(MUTATION_RATE, DEPTH_MUTATION_RATE);
		}
	}
	void PrintBestAgent()
	{
		cout << "Best Agent" << endl;
		for (int i = 0; i < WINNER_POOL_SIZE; i++)
		{
			for (int j = 0; j < m_bestWeights[i]->getWeightCount(); j++)
			{
				cout << m_bestWeights[i]->getWeight(j) << endl;
			}
		}
	}


	IWeight* m_testWeights;
	IWeight* m_bestWeights[WINNER_POOL_SIZE];
	double m_bestFitnesses[WINNER_POOL_SIZE];
	RandomAgent m_testRandomAgent;
	vector<IWeight*> m_candidateWeights;
	vector<IWeight*> m_selectedWeights;
	ofstream reportStream;
};