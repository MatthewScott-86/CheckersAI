#pragma once

#include <vector>
#include <string>

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


static const double zTable[]
{
	0,
	0.008862501,
	0.017726395,
	0.026593075,
	0.035463939,
	0.044340388,
	0.05322383,
	0.062115681,
	0.071017365,
	0.079930319,
	0.08885599,
	0.097795843,
	0.106751356,
	0.115724025,
	0.124715368,
	0.133726922,
	0.142760248,
	0.151816935,
	0.160898596,
	0.170006877,
	0.179143455,
	0.18831004,
	0.197508383,
	0.206740272,
	0.216007537,
	0.225312055,
	0.234655752,
	0.244040604,
	0.253468643,
	0.262941962,
	0.272462715,
	0.282033122,
	0.291655476,
	0.301332146,
	0.311065583,
	0.320858322,
	0.330712992,
	0.340632322,
	0.350619143,
	0.3606764,
	0.370807159,
	0.381014611,
	0.391302088,
	0.401673067,
	0.412131182,
	0.422680239,
	0.433324222,
	0.444067311,
	0.454913899,
	0.4658686,
	0.476936276,
	0.488122051,
	0.499431332,
	0.510869837,
	0.522443617,
	0.534159088,
	0.546023058,
	0.558042769,
	0.570225932,
	0.582580773,
	0.595116081,
	0.607841267,
	0.620766423,
	0.633902395,
	0.647260861,
	0.660854425,
	0.674696721,
	0.688802528,
	0.703187913,
	0.717870384,
	0.732869078,
	0.748204971,
	0.763901132,
	0.779983014,
	0.796478806,
	0.813419848,
	0.830841128,
	0.848781888,
	0.867286351,
	0.886404622,
	0.906193802,
	0.926719377,
	0.948056976,
	0.970294619,
	0.993535628,
	1.017902465,
	1.043541844,
	1.070631712,
	1.099390952,
	1.130093207,
	1.163087154,
	1.198827218,
	1.237921993,
	1.281214324,
	1.329921914,
	1.385903824,
	1.452219782,
	1.534485622,
	1.644976357,
	1.821386368
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

class NormalDistribution
{
public:
	static double getRandomPosValue(double scalar)
	{
		double prob = double(rand()) / double(RAND_MAX);
		prob *= 99;
		int probIndex = int(floor(prob));

		if (probIndex >= 99)
			return zTable[99];

		if (probIndex <= 0)
			return zTable[0];

		double value1 = zTable[probIndex] * scalar;
		double value2 = zTable[probIndex + 1] * scalar;
		
		double indexDiff = prob - double(probIndex);
		double valueDiff = value2 - value1;

		// linear interpolation
		// indexDiff is difference between actual
		//		random value and index into array
		// indexDiff == 1 pushes return value up to value2
		// indexDiff == 0 returns value1
		return value1 + valueDiff * indexDiff;
	}
	static double getRandomValue(double scalar)
	{
		double positiveValue = getRandomPosValue(scalar);
		double prob = double(rand()) / double(RAND_MAX);
		if (prob < 0.5)
			return positiveValue * -1;
		return positiveValue;
	}
	static double getRandomNormalValue(double scalar)
	{
		double unNormalized = getRandomValue(scalar);
		return unNormalized / zTable[99];
	}
};
