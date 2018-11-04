#pragma once
#include "Board.h"
#include "Weights.h"
#include <algorithm>
#include <random>

#define FLOAT_EPSILON 1e-4f

using MoveSequence = vector<pair<Position, Position>>;

class Agent
{
public:

	Agent(Board& board, Color color) :
		m_board(board), m_color(color), m_maxDepth(4), m_weights(4, 4)
	{}
	Agent(Board& board, Color color, Weights weights) :
		m_board(board), m_color(color), m_maxDepth(weights.m_depth), m_weights(weights)
	{}
	Agent(const Agent& other) : 
		m_board(other.m_board), m_color(other.m_color), m_weights(other.m_weights)
	{}
	
	MoveSequence MiniMaxDecision()
	{
		double value = INT_MIN;
		MoveSequence returnMoves;
		auto myPieces = m_board.getPieces(m_color);
		auto otherPieces = m_board.getPieces(Common::OtherColor(m_color));
		bool first = true;
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
					auto moveTest = tempBoard.Move(pos, newPos, m_color);
					assert(moveTest);
					tempMoves.push_back({ pos, newPos });
					tempValue = MinValue(tempBoard, Common::OtherColor(m_color), INT_MIN, INT_MAX, 1);
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
					auto moveTest = tempBoard.Move(pos, newPos, m_color);
					assert(moveTest);
					tempValue = MaxValueAttackSeries(tempBoard, m_color, INT_MIN, INT_MAX, 1, tempMoves);					
				}

				if (tempValue > value || first)
				{
					value = tempValue;
					returnMoves = tempMoves;
					first = false;
				}
				else
				{
					auto randNum = float(rand()) / float(RAND_MAX);
					if (tempValue + FLOAT_EPSILON > value && randNum > 0.5)
					{
						value = tempValue;
						returnMoves = tempMoves;
					}
				}
			}
		}
		return returnMoves;
	}

	double MaxValue(Board board, Color color, double alpha, double beta, int depth)
	{
		double value = INT_MIN;
		if (!board.NoWinner() || depth >= m_maxDepth)
			return Fitness(board, m_color);

		auto myPieces = board.getPieces(color);
		auto otherPieces = board.getPieces(Common::OtherColor(color));
		
		for (auto piece : myPieces)
		{
			for (auto move : piece.second->getMoves())
			{
				MoveSequence tempMoves;
				double tempValue = INT_MIN;
				auto tempBoard = board;

				auto pos = piece.first;
				auto newPos = Common::PositionPlusMove(piece.first, move);

				if (otherPieces.find(newPos) == otherPieces.end()
					&& myPieces.find(newPos) == myPieces.end()
					&& board.PositionOnBoard(newPos))
				{
					//empty board square
					auto moveTest = tempBoard.Move(pos, newPos, color);
					assert(moveTest);
					//tempBoard.Print();
					tempValue = MinValue(tempBoard, Common::OtherColor(color), INT_MIN, INT_MAX, depth + 1);
				}
				else
				{
					if (myPieces.find(newPos) != myPieces.end() || !board.PositionOnBoard(newPos))
						continue;

					assert(otherPieces.find(newPos) != otherPieces.end());
					newPos = Common::PositionPlusMove(pos, Common::AttackFromMove(move));
					if (otherPieces.find(newPos) != otherPieces.end()
						|| myPieces.find(newPos) != myPieces.end()
						|| !board.PositionOnBoard(newPos))
						continue;

					tempMoves.push_back({ pos, newPos });
					auto moveTest = tempBoard.Move(pos, newPos, color);
					assert(moveTest);
					//tempBoard.Print();
					tempValue = MaxValueAttackSeries(tempBoard, color, INT_MIN, INT_MAX, depth, tempMoves);
				}

				value = max(value, tempValue);
				if (value >= beta)
					return value;
				alpha = max(alpha, value);
			}
		}
		return value;
	}
	double MaxValueAttackSeries(Board board, Color color, double alpha, double beta, int depth, MoveSequence& moves)
	{
		double value = INT_MIN;
		if (!board.NoWinner() || depth >= m_maxDepth)
			return Fitness(board, m_color);

		auto currentPos = moves.back().second;
		auto myPieces = board.getPieces(color);
		auto otherPieces = board.getPieces(Common::OtherColor(color));
		auto piece = myPieces.find(currentPos);
		auto attackMoves = piece->second->getAttackMoves();
		MoveSequence bestMoves;

		bool attacked = false;
		for (auto move : attackMoves)
		{
			auto tempBoard = board;
			MoveSequence tempMoves;
			auto newPos = Common::PositionPlusMove(currentPos, move);
			auto jumpedPos = Common::JumpedPosition(currentPos, newPos);
			if (!tempBoard.PositionOnBoard(newPos)
				|| otherPieces.find(newPos) != otherPieces.end()
				|| myPieces.find(newPos) != myPieces.end()
				|| otherPieces.find(jumpedPos) == otherPieces.end())
				continue;

			attacked = true;
			auto moveTest = tempBoard.Move(currentPos, newPos, color);
			assert(moveTest);
			//tempBoard.Print();
			tempMoves.push_back({ currentPos, newPos });
			auto tempValue = MaxValueAttackSeries(tempBoard, color, alpha, beta, depth, tempMoves);
			if (tempValue > value)
			{
				bestMoves = tempMoves;
				value = tempValue;
			}
		}
		if (!attacked)
		{
			auto tempValue = MinValue(board, Common::OtherColor(color), alpha, beta, depth + 1);
			if (tempValue > value)
				value = tempValue;
		}

		for (auto move : bestMoves)
			moves.push_back(move);
		return value;
	}
	double MinValue(Board board, Color color, double alpha, double beta, int depth)
	{
		double value = INT_MAX;
		if (!board.NoWinner() || depth >= m_maxDepth)
			return Fitness(board, m_color);

		auto myPieces = board.getPieces(color);
		auto otherPieces = board.getPieces(Common::OtherColor(color));
		for (auto piece : myPieces)
		{
			for (auto move : piece.second->getMoves())
			{
				MoveSequence tempMoves;
				double tempValue = INT_MAX;
				auto tempBoard = board;

				auto pos = piece.first;
				auto newPos = Common::PositionPlusMove(piece.first, move);

				if (otherPieces.find(newPos) == otherPieces.end()
					&& myPieces.find(newPos) == myPieces.end()
					&& board.PositionOnBoard(newPos))
				{
					//empty board square
					auto moveTest = tempBoard.Move(pos, newPos, color);
					assert(moveTest);
					//tempBoard.Print();
					tempValue = MaxValue(tempBoard, Common::OtherColor(color), INT_MIN, INT_MAX, depth + 1);
				}
				else
				{
					if (myPieces.find(newPos) != myPieces.end() || !board.PositionOnBoard(newPos))
						continue;

					assert(otherPieces.find(newPos) != otherPieces.end());
					newPos = Common::PositionPlusMove(pos, Common::AttackFromMove(move));
					if (otherPieces.find(newPos) != otherPieces.end()
						|| myPieces.find(newPos) != myPieces.end()
						|| !board.PositionOnBoard(newPos))
						continue;

					tempMoves.push_back({ pos, newPos });
					auto moveTest = tempBoard.Move(pos, newPos, color);
					assert(moveTest);
					//tempBoard.Print();
					tempValue = MinValueAttackSeries(tempBoard, color, INT_MIN, INT_MAX, depth, tempMoves);
				}

				value = min(tempValue, value);
				if (value <= alpha)
					return value;

				beta = min(beta, value);
			}
		}
		return value;
	}
	double MinValueAttackSeries(Board board, Color color, double alpha, double beta, int depth, MoveSequence& moves)
	{
		double value = INT_MAX;
		if (!board.NoWinner() || depth >= m_maxDepth)
			return Fitness(board, m_color);

		auto currentPos = moves.back().second;
		auto myPieces = board.getPieces(color);
		auto otherPieces = board.getPieces(Common::OtherColor(color));
		auto piece = myPieces.find(currentPos);
		auto attackMoves = piece->second->getAttackMoves();
		MoveSequence bestMoves;

		bool attacked = false;
		for (auto move : attackMoves)
		{
			auto tempBoard = board;
			MoveSequence tempMoves;
			auto newPos = Common::PositionPlusMove(currentPos, move);
			auto jumpedPos = Common::JumpedPosition(currentPos, newPos);
			if (!tempBoard.PositionOnBoard(newPos)
				|| otherPieces.find(newPos) != otherPieces.end()
				|| myPieces.find(newPos) != myPieces.end()
				|| otherPieces.find(jumpedPos) == otherPieces.end())
				continue;

			attacked = true;
			auto moveTest = tempBoard.Move(currentPos, newPos, color);
			assert(moveTest);
			//tempBoard.Print();
			tempMoves.push_back({ currentPos, newPos });
			auto tempValue = MinValueAttackSeries(tempBoard, color, alpha, beta, depth, tempMoves);
			if (tempValue < value)
			{
				bestMoves = tempMoves;
				value = tempValue;
			}
		}
		if (!attacked)
		{
			auto tempValue = MaxValue(board, Common::OtherColor(color), alpha, beta, depth + 1);
			if (tempValue < value)
				value = tempValue;
		}

		for (auto move : bestMoves)
			moves.push_back(move);
		return value;
	}
	double Fitness(Board& board, Color color)
	{
		double fitness = 0;
		for (auto piece : board.getPieces(color))
		{
			if (piece.second->GetType() == PieceType::NORMAL)
				fitness += m_weights.m_weights[Common::WeightStart(color) % 4];
			else
				fitness += m_weights.m_weights[(Common::WeightStart(color) + 1) % 4];
		}

		for (auto piece : board.getPieces(Common::OtherColor(color)))
		{
			if (piece.second->GetType() == PieceType::NORMAL)
				fitness -= m_weights.m_weights[(Common::WeightStart(color) + 2) % 4];
			else
				fitness -= m_weights.m_weights[(Common::WeightStart(color) + 3) % 4];
		}
		return fitness;
	}
	void Move()
	{
		auto moves = MiniMaxDecision();
		if (moves.size() == 0)
			m_board.recordNoMoves(m_color);

		for (auto move : moves)
		{
			//cout << "{{" << to_string(move.first.first) << "," << to_string(move.first.second) << "},{" << to_string(move.second.first) << "," << to_string(move.second.second) << "}},";
			m_board.Move(move.first, move.second, m_color);
		}
	}
	
	int m_maxDepth;
	Weights m_weights;
	Board& m_board;
	Color m_color;
};
