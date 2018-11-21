
#include "GenAlgo.h"

void runSoloMatch()
{
	srand(1234567890);
	auto board = Board();
	board.Print();
	auto agentB = Agent(board, Color::BLACK, &PieceWeights({ 0.3595,	2.2327,	3.0226,	11.8821 }, 4));
	auto agentR = Agent(board, Color::RED, &PieceWeights({ 3.14066,18.8214,2.86081,6.72628 },4));

	int i = 0;
	while (true)
	{
		if (!board.NoWinner())
			break;

		cout << "Black " + to_string(i) << " : ";
		agentB.Move();
		cout << endl;
		board.Print();

		if (!board.NoWinner())
			break;

		cout << "Red " + to_string(i) << " : ";
		cout << endl;
		agentR.Move();
		cout << endl;
		board.Print();
		
		i++;
	}

	cout << "";
}



vector<double> Tournament(PieceWeights agentWeights[], int size, int moveCountMax = 200)
{
	vector<double> fitnesses(size, 0);

	for (int i = 0; i < size; i++)
	{
		for (int j = i + 1; j < size; j++)
		{
			auto board = Board();

			auto black = Agent(board, Color::BLACK, &agentWeights[i]);
			auto red = Agent(board, Color::RED, &agentWeights[j]);
			int turn = 0;
			while (true)
			{
				if ((turn + 1) % 100 == 0)
				{
					black.m_maxDepth++;
					red.m_maxDepth++;
				}
				if (board.m_moveCount > moveCountMax)
				{
					board.m_noMoves = true;
					break;
				}
				if (!board.NoWinner())
					break;

				cout << "Black " + to_string(turn) << " : ";
				black.Move();
				cout << endl;
				board.Print();

				if (!board.NoWinner())
					break;
				cout << "Red " + to_string(turn) << " : ";
				cout << endl;
				red.Move();
				cout << endl;
				board.Print();

				turn++;
			}
			if (board.m_noMoves)
			{
				continue;
			}
			// if agent wins, gets win bonus, plus bonus for less moves
			//  otherwise bonus is based on number of moves only at a max of the win bonus
			/*if (board.getWinner() == black.m_color)
			{
				fitnesses[i] += (WIN_BONUS + max(WIN_BONUS - board.m_moveCount, 0));
				fitnesses[j] += min(board.m_moveCount, WIN_BONUS);
			}
			else
			{
				fitnesses[j] += (WIN_BONUS + max(WIN_BONUS - board.m_moveCount, 0));
				fitnesses[i] += min(board.m_moveCount, WIN_BONUS);
			}*/

			if (board.getWinner() == black.m_color)
			{
				fitnesses[i] += (WIN_BONUS + max(WIN_BONUS + int(board.getPieces(black.m_color).size()) * 10, 0));
				fitnesses[j] += min(120 - int(board.getPieces(Common::OtherColor(black.m_color)).size()) * 10, WIN_BONUS);
			}
			else
			{
				fitnesses[i] += min(120 - int(board.getPieces(Common::OtherColor(black.m_color)).size()) * 10, WIN_BONUS);
				fitnesses[j] += (WIN_BONUS + max(WIN_BONUS + int(board.getPieces(black.m_color).size()) * 10, 0));
			}


		}
	}
	return fitnesses;
}

static PieceWeights agents[] = {
	PieceWeights({ 0.3595,	2.2327,		3.0226,		11.8821 }, 4),	// First Agent
	PieceWeights({ 1.97851,	16.8432,	1.23325,	2.19855 }, 4),	// winners 1
	PieceWeights({ 7.69066,	8.21436,	2.01971,	12.5614 }, 4),	// winners 1
	PieceWeights({ 2.52724,	2.88949,	0.0161748,	1.22013 }, 4),	// winners 1
	PieceWeights({5.99048,	6.71896,	7.35008,	16.1748	}, 4),  // winners 4 (first no bugs tournament)
	PieceWeights({7.33085,	16.6924,	8.39442,	13.9952	}, 4),	// winners 4 (first no bugs tournament)
	PieceWeights({7.33085,	6.71896,	8.02606,	10.3977	}, 5),	// winners 4 (first no bugs tournament)
	PieceWeights({4.79873,	7.00583,	8.95962,	16.4568 }, 4),  // winners 5 (piece based fitness function)
	PieceWeights({6.07685,	2.90353,	9.8178,		12.3997 }, 5),  // winners 5 (piece based fitness function)
	PieceWeights({7.32017,	16.8731,	9.94629,	11.4518 }, 5),   // winners 5 (piece based fitness function)
	PieceWeights({ 1.31626,	9.83795,	6.85629,	6.72628	}, 4), //winners 6 (against last winner)
	PieceWeights({ 3.83007,	0.0854518,	4.1792,		1.64495	}, 5), //winners 6 (against last winner)
	PieceWeights({ 6.57308,	14.8656,	9.5172,		15.7073	}, 4)  //winners 6 (against last winner)

};

void RunTournamentFour()
{
	PieceWeights testWeights({ 0.3595,	2.2327,	3.0226,	11.8821 }, 4);
	PieceWeights tournamentWeights[WINNER_POOL_SIZE + 1];
	tournamentWeights[0] = testWeights;
	for (int i = 0; i < WINNER_POOL_SIZE; i++)
	{
		tournamentWeights[i + 1] = agents[i + 4];
	}
	auto fitnesses = Tournament(tournamentWeights, WINNER_POOL_SIZE + 1, 1000);

	for (auto fitness : fitnesses)
		cout << fitness << endl;
}

void RunTournamentFive()
{
	PieceWeights testWeights({ 0.3595,	2.2327,	3.0226,	11.8821 }, 4);
	PieceWeights tournamentWeights[WINNER_POOL_SIZE + 1];
	tournamentWeights[0] = testWeights;
	for (int i = 0; i < WINNER_POOL_SIZE; i++)
	{
		tournamentWeights[i + 1] = agents[i + 7];
	}
	auto fitnesses = Tournament(tournamentWeights, WINNER_POOL_SIZE + 1, 1000);

	for (auto fitness : fitnesses)
		cout << fitness << endl;

}

void RunTournamentSix()
{
	PieceWeights testWeights({ 6.07685,	2.90353,	9.8178,		12.3997 }, 5);
	PieceWeights tournamentWeights[WINNER_POOL_SIZE + 1];
	tournamentWeights[0] = testWeights;
	for (int i = 0; i < WINNER_POOL_SIZE; i++)
	{
		tournamentWeights[i + 1] = agents[i + 10];
	}
	
	auto fitnesses = Tournament(tournamentWeights, WINNER_POOL_SIZE + 1, 1000);

	for (auto fitness : fitnesses)
		cout << fitness << endl;

}

int main()
{
	//runSoloMatch();
	//RunTournamentSix();

	PieceWeights testWeights({ 6.07685,	2.90353,	9.8178,		12.3997 }, 5);
	GenAlgo genAlgo(&testWeights);
	
	for (int i = 0; i < 100; i++)
	{
		cout << "Tournament " << i << endl;
		genAlgo.Selection();
		genAlgo.CrossOver();
		genAlgo.Mutation(i, 100);
	}
	IWeight* tournamentWeights[WINNER_POOL_SIZE + 1];
	tournamentWeights[0] = &testWeights;
	for (int i = 0; i < WINNER_POOL_SIZE; i++)
	{
		tournamentWeights[i + 1] = genAlgo.m_bestWeights[i];
	}
	

	/*auto fitnesses = Tournament(tournamentWeights, WINNER_POOL_SIZE + 1);

	genAlgo.PrintBestAgent();
	for (auto fitness : fitnesses)
		cout << fitness << endl;*/

}