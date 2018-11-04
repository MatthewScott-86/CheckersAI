#pragma once

#include "Common.h"

class IPiece
{
	virtual IPiece* Clone() = 0; 
	virtual PieceType GetType() = 0;
	virtual vector<Position> getMoves() = 0;
	virtual vector<Position> getAttackMoves() = 0;
	virtual string ToString() = 0;
	virtual bool NeedsUpgrade() = 0;
};

class PieceBase : public IPiece
{
public:
	PieceBase() : m_color(Color::NO_COLOR) {}

	PieceBase(Color col, int direction) :
		m_color(col), m_direction(direction) {}
	virtual PieceBase* Clone() override	{ throw runtime_error("Can't be here");	}
	virtual PieceType GetType() override { throw runtime_error("Can't be here"); }
	virtual vector<Position> getMoves() override { throw runtime_error("Cant be here");	}
	virtual vector<Position> getAttackMoves() override { throw runtime_error("Cant be here"); }	
	virtual string ToString() override { throw runtime_error("Cant be here"); }
	virtual bool NeedsUpgrade() override { throw runtime_error("Cant be here"); }

	Color m_color;
	int m_direction;
};

class KingPiece : public PieceBase
{
public:
	KingPiece(Color col, int direction)
		: PieceBase(col, direction) {}

	virtual PieceBase* Clone() override	{ 
		return new KingPiece(m_color, m_direction);
	}
	virtual PieceType GetType() override { return PieceType::KING; }
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
	virtual bool NeedsUpgrade() override { return false; }
	virtual string ToString() override { return PieceTypeStrings[PieceType::KING] + string(ColorStrings[m_color]); }
};

class NormalPiece : public PieceBase
{
public:
	NormalPiece(Color col, int direction)
		: PieceBase(col, direction) {}

	virtual PieceBase* Clone() override { 
		return new NormalPiece(m_color, m_direction);
	}
	virtual PieceType GetType() override { return PieceType::NORMAL; }
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
	virtual bool NeedsUpgrade() override { return true; }
	virtual string ToString() override { return PieceTypeStrings[PieceType::NORMAL] + string(ColorStrings[m_color]); }
};