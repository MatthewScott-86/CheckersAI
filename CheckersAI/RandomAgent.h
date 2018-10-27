#pragma once
#include "Board.h"

#include <random>

class RandomAgent
{
public:
	RandomAgent(Color color) :
		m_color(color), m_board(Board()) {}
	RandomAgent(Board& board, Color color) :
		m_board(board), m_color(color) {}

	void setBoard(Board& board) { m_board =board; }
	void Move()
	{
		auto allMoves = GetAllMoves();
		if (allMoves.size() <= 0)
		{
			m_board.getPieces(m_color).clear();
			cout << "Player " << ColorStrings[m_color] << " cannot move, game lost" << endl;
		}
		auto choice = rand() % allMoves.size();
		auto pieceMoves = allMoves[choice];
		for (auto move : pieceMoves)
		{
			cout << "{{" << to_string(move.first.first) << "," << to_string(move.first.second) << "},{" << to_string(move.second.first) << "," << to_string(move.second.second) << "}},";
			assert(m_board.Move(move.first, move.second, m_color));
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
				auto newPos = Common::PositionPlusMove(pos, move);
				if (otherPieces.find(newPos) == otherPieces.end() && myPieces.find(newPos) == myPieces.end() && m_board.PositionOnBoard(newPos))
				{
					pieceMoves.push_back({ pos, newPos });
					allMoves.push_back(pieceMoves);
					continue;
				}

				if (otherPieces.find(newPos) == otherPieces.end())
					continue;
				//found an opponent piece, try attack instead

				newPos = Common::PositionPlusMove(pos, Common::AttackFromMove(move));
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
			auto newPos = Common::PositionPlusMove(currentPos, move);
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
	Board& m_board;
	Color m_color;
};