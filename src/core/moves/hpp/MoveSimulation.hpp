#ifndef MOVE_SIMULATION_HPP
#define MOVE_SIMULATION_HPP

#include "../../Piece.hpp"
#include <vector> 
#include <tuple>

class Board;

namespace MoveSimulation {
    // Track last move for en passant detection (piece, original position, new position) 
    extern std::tuple<Piece, Vector2, Vector2> lastMove;

    bool SimulateMove(Piece &piece, const Vector2 &newPosition,
                      std::vector<Piece> &pieces, Board &board);
    bool SimulateMoveForOur(Piece &piece, const Vector2 &newPosition, 
                          std::vector<Piece> &pieces, Board &board);
    bool SimulateMoveAndCheck(std::vector<Piece> &pieces, Piece &piece,
                          const Vector2 &newPosition, int kingColor, Board &board);
    bool CheckKingAfterMove(Piece &king, Vector2 &newPosition,
                          std::vector<Piece> &pieces,const Vector2 &originalPosition, Board &board);

}

#endif