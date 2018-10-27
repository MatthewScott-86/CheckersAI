
#include "GenAlgo.h"

void runSoloMatch()
{
	srand(1234567890);
	auto board = Board();
	board.Print();
	auto agentR = Agent(board, Color::RED);
	auto agentB = RandomAgent(board, Color::BLACK);

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

			// if agent wins, gets win bonus, plus bonus for less moves
			//  otherwise bonus is based on number of moves only at a max of the win bonus
			if (board.getWinner() == black.m_color)
			{
				fitnesses[i] = (WIN_BONUS + max(WIN_BONUS - board.m_moveCount, 0));
				fitnesses[j] = min(board.m_moveCount, WIN_BONUS);
			}
			else
			{
				fitnesses[j] = (WIN_BONUS + max(WIN_BONUS - board.m_moveCount, 0));
				fitnesses[i] = min(board.m_moveCount, WIN_BONUS);
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
	Weights testWeights({ 0.3595,	2.2327,	3.0226,	11.8821	}, 4);
	GenAlgo genAlgo(testWeights);
	
	for (int i = 0; i < 100; i++)
	{
		cout << "Tournament " << i << endl;
		genAlgo.Selection();
		genAlgo.CrossOver();
		genAlgo.Mutation();
	}
	Weights tournamentWeights[WINNER_POOL_SIZE + 1];
	tournamentWeights[0] = testWeights;
	for (int i = 0; i < WINNER_POOL_SIZE; i++)
	{
		tournamentWeights[i + 1] = genAlgo.m_bestWeights[i];
	}
	
	auto fitnesses = Tournament(tournamentWeights, WINNER_POOL_SIZE + 1);

	genAlgo.PrintBestAgent();
	for (auto fitness : fitnesses)
		cout << fitness << endl;

}