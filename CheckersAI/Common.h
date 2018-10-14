#pragma once

using namespace std;
enum PieceType
{
	NONE = 0,
	NORMAL,
	KING
};

enum Color
{
	NO_COLOR = 0,
	RED,
	BLACK
};

using Position = pair<int, int>;

static const char* PieceTypeStrings[]
{
	"N",
	"P",
	"K"
};

static const char* ColorStrings[]
{
	"N",
	"R",
	"B"
};
class Common
{
public:
	static int WeightStart(Color col) { return col == Color::RED ? 0 : 2; }
	static Color OtherColor(Color col) { return col == Color::RED ? Color::BLACK : col == Color::BLACK ? Color::RED : Color::NO_COLOR; }
	static Position JumpedPosition(Position start, Position end) { 
		return Position 
		{start.first - end.first > 0 ? start.first - 1 : start.first + 1
		, start.second - end.second > 0 ? start.second - 1 : start.second + 1 }; 
	}
	static Position AttackFromMove(Position pos) { return Position{ pos.first * 2, pos.second * 2 }; }
	static Position PositionPlusMove(Position pos, Position move) { return Position{ pos.first + move.first, pos.second + move.second }; }
};
