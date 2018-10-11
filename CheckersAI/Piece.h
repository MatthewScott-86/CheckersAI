#pragma once

#include <vector>
#include <string>
#include "Common.h"

class IPiece
{
	virtual IPiece* Clone() = 0; 
	virtual IPiece* Upgrade() = 0;
	virtual vector<Position> getMoves() = 0;
	virtual vector<Position> getAttackMoves() = 0;
	virtual string ToString() = 0;
};

class PieceBase : public IPiece
{
public:
	PieceBase() : m_pos({ -1, -1 }), m_color(Color::NO_COLOR) {}

	PieceBase(Position pos, Color col, int direction) :
		m_pos(pos), m_color(col), m_direction(direction) {}

	PieceBase(int x, int y, Color col, int direction) :
		m_pos({ x, y }), m_color(col), m_direction(direction) {}

	virtual PieceBase* Clone() override	{ throw runtime_error("Can't be here");	}
	virtual PieceBase* Upgrade() override { throw runtime_error("Can't be here"); }
	virtual vector<Position> getMoves() override { throw runtime_error("Cant be here");	}
	virtual vector<Position> getAttackMoves() override { throw runtime_error("Cant be here"); }	
	virtual string ToString() override { throw runtime_error("Cant be here"); }

	Position m_pos;
	Color m_color;
	int m_direction;
};

class KingPiece : public PieceBase
{
public:
	KingPiece(int x, int y, Color col, int direction)
		: PieceBase(x, y, col, direction) {}

	virtual PieceBase* Clone() override	{ 
		return new KingPiece(m_pos.first, m_pos.second, m_color, m_direction);
	}
	virtual PieceBase* Upgrade() override { 
		return new KingPiece(m_pos.first, m_pos.second, m_color, m_direction); 
	}
	virtual vector<Position> getMoves() override
	{
		vector<Position> moves;
		moves.emplace_back(1, 1);
		moves.emplace_back(-1, 1);
		moves.emplace_back(1, -1);
		moves.emplace_back(-1, -1);
		return moves;
	}
	virtual vector<Position> getAttackMoves() override
	{
		vector<Position> moves;
		moves.emplace_back(2, 2);
		moves.emplace_back(-2, 2);
		moves.emplace_back(2, -2);
		moves.emplace_back(-2, -2);
		return moves;
	}
	virtual string ToString() override { return PieceTypeStrings[PieceType::KING] + string(ColorStrings[m_color]); }
};

class NormalPiece : public PieceBase
{
public:
	NormalPiece(int x, int y, Color col, int direction)
		: PieceBase(x, y, col, direction) {}

	virtual PieceBase* Clone() override { 
		return new NormalPiece(m_pos.first, m_pos.second, m_color, m_direction); 
	}
	virtual PieceBase* Upgrade() override { 
		return new KingPiece(m_pos.first, m_pos.second, m_color, m_direction); 
	}
	virtual vector<Position> getMoves() override
	{
		vector<Position> moves;
		moves.emplace_back(1, m_direction);
		moves.emplace_back(-1, m_direction);
		return moves;
	}
	virtual vector<Position> getAttackMoves() override
	{
		vector<Position> moves;
		moves.emplace_back(2, m_direction * 2);
		moves.emplace_back(-2, m_direction * 2);
		return moves;
	}
	virtual string ToString() override { return PieceTypeStrings[PieceType::NORMAL] + string(ColorStrings[m_color]); }
};