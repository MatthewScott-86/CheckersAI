
#include "Agent.h"

int main()
{
	auto board = Board();
	board.Print();
	auto agentR = Agent(board, Color::RED);
	auto agentB = Agent(board, Color::BLACK);

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
		agentR.Move();
		cout << endl;
		board.Print();
		i++;
	}

	cout << "";
}