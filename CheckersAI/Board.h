#pragma once
#include <map>
#include <unordered_map>
#include <iostream>
#include <assert.h>

#include "Piece.h"

using PieceCollection = map<Position, PieceBase*>;
class Board
{
public:
	Board() : m_size(8), m_moveCount(0), m_noMoves(false)
	{
		for (int i = 0; i < 4; i++)
		{
			int x = i * 2;
			for (int y = 0; y < 3; y++)
			{
				int blackY = 7 - y;
				int blackX = y % 2 == 0 ? x + 1 : x;
				int redX = y % 2 == 0 ? x : x + 1;
				m_redPieces[{ redX, y }]		= new NormalPiece(Color::RED, 1);
				m_blackPieces[{blackX, blackY}] = new NormalPiece(Color::BLACK, -1);
			}
		}
	}
	Board(const Board& other)
	{
		for (auto it : other.m_blackPieces)
			m_blackPieces[it.first] = it.second->Clone();
		for (auto it : other.m_redPieces)
			m_redPieces[it.first] = it.second->Clone();
		m_size = other.m_size;
		m_moveCount = other.m_moveCount;
		m_noMoves = other.m_noMoves;
	}
	PieceCollection& getPieces(Color col)
	{
		if (Color::RED == col)
			return m_redPieces;
		if (Color::BLACK == col)
			return m_blackPieces;
		throw runtime_error("Invalid Color Piece Requested");
	}

	bool Upgrade(PieceBase* piece, Position pos)
	{
		if (!piece->NeedsUpgrade())
			return false;

		if (piece->m_color == Color::RED && pos.second == 7)
			return true;
		if (piece->m_color == Color::BLACK && pos.second == 0)
			return true;

		return false;
	}
	bool AttackOnBoard(Color color)
	{
		auto playerPieces = getPieces(color);

		for (auto piece : playerPieces)
			for (auto move : piece.second->getAttackMoves())
				if (ValidAttack(color, piece.first, Common::PositionPlusMove(piece.first, move)))
					return true;

		return false;
	}
	bool ValidAttack(Color color, Position start, Position end)
	{
		auto playerPieces = getPieces(color);
		auto otherPieces = getPieces(Common::OtherColor(color));
		auto jumpedPos = Common::JumpedPosition(start, end);

		if (otherPieces.find(jumpedPos) == otherPieces.end())
			return false;

		if (otherPieces.find(end) != otherPieces.end())
			return false;

		if (playerPieces.find(end) != playerPieces.end())
			return false;

		return PositionOnBoard(end);
	}
	bool ValidMove(Color color, Position end)
	{
		auto playerPieces = getPieces(color);
		auto otherPieces = getPieces(Common::OtherColor(color));

		if (otherPieces.find(end) != otherPieces.end())
			return false;

		if (playerPieces.find(end) != playerPieces.end())
			return false;
		
		return PositionOnBoard(end);
	}
	void MovePiece(PieceCollection& pieces, Position start, Position end)
	{
		auto piece = pieces.find(start);
		assert(piece != pieces.end());

		pieces[end] = move(pieces[start]);
		if (Upgrade(pieces[end], end))
		{
			PieceBase* oldPiece = pieces[end];
			pieces[end] = new KingPiece(oldPiece->m_color, oldPiece->m_direction);
			delete oldPiece;
		}
		pieces.erase(start);
		m_moveCount++;
	}

	bool Move(Position start, Position end, Color col)
	{
		auto& pieces = getPieces(col);
		auto& otherPieces = getPieces(Common::OtherColor(col));
		auto piece = pieces.find(start);
		auto otherPiece = otherPieces.find(end);
		if (piece == pieces.end())
			return false;

		if (otherPiece != otherPieces.end())
			return false;

		if (abs(start.first - end.first) == 1)
		{
			MovePiece(pieces, start, end);
			return true;
		}
		
		if (abs(start.first - end.first) == 2)
		{
			Position otherPos = Common::JumpedPosition(start, end);
			otherPiece = otherPieces.find(otherPos);
			if (otherPiece == otherPieces.end())
				return false;

			delete otherPiece->second;
			otherPieces.erase(otherPiece->first);
			MovePiece(pieces, start, end);
			return true;
		}

		return false;
	}

	bool PositionOnBoard(Position pos)
	{
		return pos.first >= 0 && pos.first < m_size && pos.second >= 0 && pos.second < m_size;
	}

	bool NoWinner()
	{
		return m_redPieces.size() > 0 && m_blackPieces.size() > 0;
	}
	void Print() 
	{
		for (int i = 0; i < m_size; i++)
		{
			cout << "|";
			for (int j = 0; j < m_size; j++)
			{
				pair<int, int> currentPos{ i, j };
				auto redPiece = m_redPieces.find(currentPos);
				auto blackPiece = m_blackPieces.find(currentPos);
				if (redPiece != m_redPieces.end())
				{
					cout << m_redPieces[currentPos]->ToString() << "|";
					continue;
				}
				if (blackPiece != m_blackPieces.end())
				{
					cout << m_blackPieces[currentPos]->ToString() << "|";
					continue;
				}
				cout << "  " << "|";
			}
			cout << endl;
		}
		cout << endl;
		cout << endl;
	}

	void recordNoMoves(Color color)
	{
		getPieces(color).clear();
		m_noMoves = true;
	}

	Color getWinner()
	{
		if (m_redPieces.size() == 0)
			return Color::BLACK;
		if (m_blackPieces.size() == 0)
			return Color::RED;
		return Color::NO_COLOR;
	}
	~Board()
	{
		for (auto it : m_blackPieces)
			delete it.second;
		for (auto it : m_redPieces)
			delete it.second;
		m_blackPieces.clear();
		m_redPieces.clear();
	}
	PieceCollection m_redPieces;
	PieceCollection m_blackPieces;
	int m_size;
	int m_moveCount;
	bool m_noMoves;
};
