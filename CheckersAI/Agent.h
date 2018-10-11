#pragma once
#include "Board.h"
#include <random>

using MoveSequence = vector<pair<Position, Position>>;

class Agent
{
public:
	Agent(Board& board, Color color): m_board(board), m_color(color)	{
		srand(1234567890);
	}

	void Move()
	{
		auto allMoves = GetAllMoves();
		auto choice = rand() % allMoves.size();
		auto pieceMoves = allMoves[choice];
		for (auto move : pieceMoves)
		{
			auto test = m_board.Move(move.first, move.second, m_color);
			cout << "{{" << to_string(move.first.first) << "," << to_string(move.first.second) << "},{" << to_string(move.second.first) << "," << to_string(move.second.second) << "}},";
			if (test == false)
			{
				throw runtime_error("Invalid Move detected");
			}
		}
	}
	vector<MoveSequence> GetAllMoves()
	{
		vector<MoveSequence> allMoves;
		const auto& myPieces = m_board.getPieces(m_color);
		const auto& otherPieces = m_board.getPieces(Common::OtherColor(m_color));
		for (auto piece : myPieces)
		{
			auto pos = piece.first;
			auto moves = piece.second->getMoves();
			for (auto move : moves)
			{
				MoveSequence pieceMoves;
				auto newPos = PositionPlusMove(pos, move);
				if (otherPieces.find(newPos) == otherPieces.end() && myPieces.find(newPos) == myPieces.end() && m_board.PositionOnBoard(newPos))
				{
					pieceMoves.push_back({ pos, newPos });
					allMoves.push_back(pieceMoves);
					continue;
				}

				if (otherPieces.find(newPos) == otherPieces.end())
					continue;
				//found an opponent piece, try attack instead

				newPos = PositionPlusMove(pos, AttackFromMove(move));
				if (otherPieces.find(newPos) != otherPieces.end() || myPieces.find(newPos) != myPieces.end() || !m_board.PositionOnBoard(newPos))
					continue;

				pieceMoves.push_back({ pos, newPos });
				auto tempBoard = m_board;
				auto tempMyPieces = m_board.getPieces(m_color);
				tempBoard.Move(pos, newPos, m_color);
				GetAttackSeries(piece.second, newPos, allMoves, pieceMoves, tempBoard);
			}

		}
		return allMoves;
	}

	void GetAttackSeries(
		PieceBase* piece, Position currentPos, 
		vector<MoveSequence>& allMoves,
		MoveSequence currentMoveSeries,
		Board board)
	{
		auto myPieces = board.getPieces(m_color);
		auto otherPieces = board.getPieces(Common::OtherColor(m_color));
		for (auto move : piece->getAttackMoves())
		{
			auto moveSeriesCopy = currentMoveSeries;
			auto newPos = PositionPlusMove(currentPos, move);
			if (otherPieces.find(newPos) == otherPieces.end() 
				&& myPieces.find(newPos) == myPieces.end()
				&& m_board.PositionOnBoard(newPos) 
				&& otherPieces.find(Common::JumpedPosition(currentPos, newPos)) != otherPieces.end())
			{
				moveSeriesCopy.push_back({ currentPos, newPos });
				auto tempBoard = board;
				auto tempMyPieces = m_board.getPieces(m_color);
				tempBoard.Move(currentPos, newPos, m_color);
				GetAttackSeries(piece, newPos, allMoves, moveSeriesCopy, tempBoard);
			}
			else
			{
				if (moveSeriesCopy.size() > 0)
					allMoves.push_back(moveSeriesCopy);
			}
		}
	}

	Position AttackFromMove(Position pos)
	{
		return Position{ pos.first * 2, pos.second * 2 };
	}

	Position PositionPlusMove(Position pos, Position move)
	{
		return Position{ pos.first + move.first, pos.second + move.second };
	}

	Board& m_board;
	Color m_color;
};