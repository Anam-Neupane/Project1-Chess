#ifndef MOVE_GENERATION_HPP
#define MOVE_GENERATION_HPP

#include "../../Piece.hpp"
#include <vector>

class Board;

namespace MoveGeneration {

    std::vector<Vector2> GetAllPossibleMoves(Piece &piece, std::vector<Piece> &pieces, Board &board);
    std::vector<Vector2> GetPawnMoves(const Piece &piece, const std::vector<Piece> &pieces, const Board &board);
    std::vector<Vector2> GetRookMoves(const Piece &piece, const std::vector<Piece> &pieces, const Board &board);
    std::vector<Vector2> GetBishopMoves(const Piece &piece, const std::vector<Piece> &pieces, const Board &board);
    std::vector<Vector2> GetQueenMoves(const Piece &piece, const std::vector<Piece> &pieces, const Board &board);
    std::vector<Vector2> GetKnightMoves(const Piece &piece, const std::vector<Piece> &pieces, const Board &board);
    std::vector<Vector2> GetKingMoves(Piece &piece, const std::vector<Piece> &pieces, const Board &board);

}

#endif 