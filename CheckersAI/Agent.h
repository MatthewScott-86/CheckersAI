#pragma once
#include "Board.h"
#include <random>

using MoveSequence = vector<pair<Position, Position>>;

class Agent
{
public:
	Agent(Board& board, Color color) :
		m_board(board), m_color(color), m_maxDepth(8)
	{
		m_weights.resize(4);
		m_weights[0] = rand() / RAND_MAX * 10;
		m_weights[1] = rand() / RAND_MAX * 10;
		m_weights[2] = rand() / RAND_MAX * 10 - 10;
		m_weights[3] = rand() / RAND_MAX * 10 - 10;
	}
	void ChooseMove()
	{

	}
	MoveSequence MiniMaxDecision()
	{
		double value = INT_MIN;
		MoveSequence returnMoves;
		auto myPieces = m_board.getPieces(m_color);
		auto otherPieces = m_board.getPieces(Common::OtherColor(m_color));
		for (auto piece : myPieces)
		{
			for (auto move : piece.second->getMoves())
			{
				MoveSequence tempMoves;
				double tempValue = INT_MIN;
				auto tempBoard = m_board;

				auto pos = piece.first;
				auto newPos = Common::PositionPlusMove(piece.first, move);

				if (otherPieces.find(newPos) == otherPieces.end()
					&& myPieces.find(newPos) == myPieces.end()
					&& m_board.PositionOnBoard(newPos))
				{
					//empty board square
					tempBoard.Move(pos, newPos, m_color);
					tempMoves.push_back({ pos, newPos });
					tempValue = MaxValue(tempBoard, Common::OtherColor(m_color), INT_MIN, INT_MAX, 1);
				}
				else
				{
					if (myPieces.find(newPos) != myPieces.end() || !m_board.PositionOnBoard(newPos))
						continue;
					
					assert(otherPieces.find(newPos) != otherPieces.end());
					newPos = Common::PositionPlusMove(pos, Common::AttackFromMove(move));
					if (otherPieces.find(newPos) != otherPieces.end()
						|| myPieces.find(newPos) != myPieces.end()
						|| !m_board.PositionOnBoard(newPos))
						continue;

					tempMoves.push_back({ pos, newPos });
					tempBoard.Move(pos, newPos, m_color);
					tempValue = MaxValueAttackSeries(tempBoard, m_color, INT_MIN, INT_MAX, 1, tempMoves);					
				}

				if (tempValue > value)
				{
					value = tempValue;
					returnMoves = tempMoves;
				}
			}
		}
		return returnMoves;
	}
	/*void GetMostAttackMoves(Board& board, Color color, Position currentPos)
	{
		auto myPieces = board.getPieces(color);
		auto otherPieces = board.getPieces(Common::OtherColor(color));
		auto piece = myPieces.find(currentPos);
		assert(piece != myPieces.end());
		for (auto move : piece->second->getAttackMoves())
		{
			auto newPos = Common::PositionPlusMove(currentPos, move);
			auto jumpedPos = Common::JumpedPosition(currentPos, newPos);
			if (otherPieces.find(jumpedPos) != otherPieces.end()
				&& myPieces.find(newPos) == myPieces.end()
				&& otherPieces.find(newPos) == otherPieces.end()
				&& board.PositionOnBoard(newPos))
			{
				board.Move(currentPos, newPos, color);
				GetMostAttackMoves(board, color, newPos);
				break;
			}
		}
	}*/
	double MaxValue(Board board, Color color, double alpha, double beta, int depth)
	{
		double value = INT_MIN;
		if (!board.NoWinner() || depth == m_maxDepth)
			return Fitness(board);
	}
	double MaxValueAttackSeries(Board board, Color color, double alpha, double beta, int depth, MoveSequence& moves)
	{
		double value = INT_MIN;
		if (!board.NoWinner() || depth == m_maxDepth)
			return Fitness(board);
	}
	double MinValue(Board board, Color color, double alpha, double beta, int depth)
	{
		double value = INT_MAX;
		if (!board.NoWinner() || depth == m_maxDepth)
			return Fitness(board);
	}
	double MinValueAttackSeries(Board board, Color color, double alpha, double beta, int depth, MoveSequence& moves)
	{
		double value = INT_MAX;
		if (!board.NoWinner() || depth == m_maxDepth)
			return Fitness(board);
	}
	double Fitness(Board& board)
	{
		double fitness = 0;
		for (auto piece : board.getPieces(m_color))
		{
			if (piece.second->GetType() == PieceType::NORMAL)
				fitness += m_weights[0];
			else
				fitness += m_weights[1];
		}
		for (auto piece : board.getPieces(Common::OtherColor(m_color)))
		{
			if (piece.second->GetType() == PieceType::NORMAL)
				fitness -= m_weights[2];
			else
				fitness -= m_weights[3];
		}
		return fitness;
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
	int m_maxDepth;
	vector<double> m_weights;
	Board& m_board;
	Color m_color;
};

class RandomAgent
{
public:
	RandomAgent(Board& board, Color color) :
		m_board(board), m_color(color) {}

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