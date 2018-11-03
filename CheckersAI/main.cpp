
#include "GenAlgo.h"

void runSoloMatch()
{
	srand(1234567890);
	auto board = Board();
	board.Print();
	auto agentB = Agent(board, Color::BLACK, Weights({ 0.3595,	2.2327,	3.0226,	11.8821 }, 4));
	auto agentR = Agent(board, Color::RED, Weights({ 3.14066,18.8214,2.86081,6.72628 },4));

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



vector<double> Tournament(Weights agentWeights[], int size)
{
	vector<double> fitnesses(size, 0);

	for (int i = 0; i < size; i++)
	{
		for (int j = i + 1; j < size; j++)
		{
			auto board = Board();

			auto black = Agent(board, Color::BLACK, agentWeights[i]);
			auto red = Agent(board, Color::RED, agentWeights[j]);
			int turn = 0;
			while (true)
			{
				if (board.m_moveCount > 200)
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
			if (board.getWinner() == black.m_color)
			{
				fitnesses[i] += (WIN_BONUS + max(WIN_BONUS - board.m_moveCount, 0));
				fitnesses[j] += min(board.m_moveCount, WIN_BONUS);
			}
			else
			{
				fitnesses[j] += (WIN_BONUS + max(WIN_BONUS - board.m_moveCount, 0));
				fitnesses[i] += min(board.m_moveCount, WIN_BONUS);
			}
		}
	}
	return fitnesses;
}

static Weights agents[] = {
	Weights({ 0.3595,	2.2327,		3.0226,		11.8821 }, 4),	// First Agent
	Weights({ 1.97851,	16.8432,	1.23325,	2.19855 }, 4),	// winners 1
	Weights({ 7.69066,	8.21436,	2.01971,	12.5614 }, 4),	// winners 1
	Weights({ 2.52724,	2.88949,	0.0161748,	1.22013 }, 4)	// winners 1
};

int main()
{
	//runSoloMatch();
	Weights testWeights({ 0.3595,	2.2327,	3.0226,	11.8821	}, 4);
	/*GenAlgo genAlgo(testWeights);
	
	for (int i = 0; i < 100; i++)
	{
		cout << "Tournament " << i << endl;
		genAlgo.Selection();
		genAlgo.CrossOver();
		genAlgo.Mutation();
	}
	*/Weights tournamentWeights[WINNER_POOL_SIZE + 1];
	tournamentWeights[0] = testWeights;
	/*for (int i = 0; i < WINNER_POOL_SIZE; i++)
	{
		tournamentWeights[i + 1] = genAlgo.m_bestWeights[i];
	}*/
	tournamentWeights[1] = Weights({ 1.978514969,
		16.84316538,
		1.233252968,
		2.198553423 }, 4);

	tournamentWeights[2] = Weights({ 7.690664388,
		8.214362011,
		2.019714957,
		12.5614185 }, 4);


	tournamentWeights[3] = Weights({ 2.52723777,
		2.889492477,
		0.01617481,
		1.220130009 }, 4);

	auto fitnesses = Tournament(tournamentWeights, WINNER_POOL_SIZE + 1);

	//genAlgo.PrintBestAgent();
	for (auto fitness : fitnesses)
		cout << fitness << endl;

}