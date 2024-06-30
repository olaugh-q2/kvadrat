#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "constants.h"
#include "square.h"

#include <stdbool.h>

typedef struct {
  int squares[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH];
  int active_piece[4][4];
  int piece_queue[14];
  int active_piece_row;
  int active_piece_col;
  int ghost_piece_row;
  int active_piece_rotation;
  int active_piece_index;
  int pieces_until_redraw;

  int lateral_movement_counter;
  int lateral_movement_direction;
  bool lateral_movement_repeating;

  int rotation_counter;
  int rotation_direction;

  int gravity_counter;
  int level;
  int soft_drop_delay;
  int gravity_delay;

  bool hard_dropped;

} GameState;

GameState *CreateInitialGameState();

void DestroyGameState(GameState *game_state);

void DrawRandomPieces(int piece_queue[14], int start_index);

void UpdateLateralMovementIntent(GameState *game_state);

bool TestActivePieceCollision(const GameState *game_state, int row, int col);

void MaybeMovePieceLaterally(GameState *game_state);

void UpdateRotationIntent(GameState *game_state);

void MaybeRotatePiece(GameState *game_state);

void MaybeApplyGravity(GameState *game_state);

void MaybeHardDrop(GameState *game_state);

void UpdateGhostPieceRow(GameState *game_state);

#endif // GAME_STATE_H