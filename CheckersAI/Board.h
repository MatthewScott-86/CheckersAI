#pragma once
#include <map>
#include <unordered_map>
#include <iostream>

#include "Piece.h"

using PieceCollection = map<Position, PieceBase*>;
class Board
{
public:
	Board() : m_size(8) 
	{
		for (int i = 0; i < 4; i++)
		{
			int x = i * 2;
			for (int y = 0; y < 3; y++)
			{
				int blackY = 7 - y;
				int blackX = y % 2 == 0 ? x + 1 : x;
				int redX = y % 2 == 0 ? x : x + 1;
				m_redPieces[{ redX, y }]		= new NormalPiece(redX, y, Color::RED, 1);
				m_blackPieces[{blackX, blackY}] = new NormalPiece(blackX, blackY, Color::BLACK, -1);
			}
		}
	}
	
	PieceCollection& getPieces(Color col)
	{
		if (Color::RED == col)
			return m_redPieces;
		if (Color::BLACK == col)
			return m_blackPieces;
		throw runtime_error("Invalid Color Piece Requested");
	}
	bool Upgrade(Color color, Position pos)
	{
		if (color == Color::RED && pos.second == 7)
			return true;
		if (color == Color::BLACK && pos.second == 0)
			return true;
		return false;
	}
	void MovePiece(PieceCollection& pieces, Position start, Position end)
	{
		auto piece = pieces.find(start);
		if (piece == pieces.end())
			throw runtime_error("Invalid Piece Selection");


		auto newPiece = piece->second->Clone();
		if (Upgrade(newPiece->m_color, end))
		{
			newPiece = piece->second->Upgrade();
		}
		newPiece->m_pos = end;
		pieces[end] = newPiece;
		pieces.erase(piece);

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

			otherPieces.erase(otherPiece);
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
	}
	PieceCollection m_redPieces;
	PieceCollection m_blackPieces;
	int m_size;
};
