#include "raylib.h"

#include "constants.h"
#include "game_state.h"
#include "tetrominos.h"

#include <stdlib.h>

GameState *CreateInitialGameState() {
  GameState *game_state = (GameState *)malloc(sizeof(GameState));
  for (int i = 0; i < PLAYFIELD_HEIGHT; i++) {
    for (int j = 0; j < PLAYFIELD_WIDTH; j++) {
      game_state->squares[i][j] = EMPTY_SQUARE;
    }
  }
  DrawRandomPieces(game_state->piece_queue, 0);
  game_state->active_piece_index = game_state->piece_queue[0];
  game_state->active_piece_rotation = ROTATION_0;
  CreatePiece(game_state->active_piece_index, game_state->active_piece,
              ROTATION_0);
  game_state->active_piece_row = 0;
  game_state->active_piece_column = SpawnColumn(game_state->active_piece_index);
  game_state->pieces_until_redraw = 1;

  game_state->lateral_movement_counter = 0;
  game_state->lateral_movement_direction = 0;
  game_state->lateral_movement_repeating = false;

  game_state->gravity_counter = 0;
  game_state->level = 1;
  game_state->gravity_delay = 60; 

  return game_state;
}

void DestroyGameState(GameState *game_state) { free(game_state); }

void DrawRandomPieces(int piece_queue[14], int start_index) {
  int bag[] = {I_PIECE, J_PIECE, L_PIECE, O_PIECE, S_PIECE, T_PIECE, Z_PIECE};
  for (int i = 6; i > 0; i--) {
    const int j = rand() % (i + 1);
    int temp = bag[i];
    bag[i] = bag[j];
    bag[j] = temp;
  }
  for (int i = 0; i < 7; i++) {
    piece_queue[start_index + i] = bag[i];
  }
}

bool TestActivePieceCollision(const GameState *game_state, int row, int col) {
  for (int piece_row = 0; piece_row < 4; piece_row++) {
    for (int piece_col = 0; piece_col < 4; piece_col++) {
      if (game_state->active_piece[piece_row][piece_col] != EMPTY_SQUARE) {
        const int playfield_row = piece_row + row;
        const int playfield_col = piece_col + col;
        if (playfield_row < 0 || playfield_row >= PLAYFIELD_HEIGHT ||
            playfield_col < 0 || playfield_col >= PLAYFIELD_WIDTH ||
            game_state->squares[playfield_row][playfield_col] != EMPTY_SQUARE) {
          return true;
        }
      }
    }
  }
  return false;
}

void UpdateLateralMovementIntent(GameState *game_state) {
  game_state->lateral_movement_counter++;
  int new_direction = 0;
  if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
    new_direction = -1;
  } else if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
    new_direction = 1;
  }
  if (new_direction != game_state->lateral_movement_direction) {
    game_state->lateral_movement_counter = LATERAL_MOVEMENT_DELAY;
    game_state->lateral_movement_repeating = false;
  }
  game_state->lateral_movement_direction = new_direction;
}

void MaybeMovePieceLaterally(GameState *game_state) {
  if (game_state->lateral_movement_counter >= LATERAL_MOVEMENT_DELAY) {
    if (game_state->lateral_movement_repeating) {
      game_state->lateral_movement_counter =
        LATERAL_MOVEMENT_DELAY - LATERAL_MOVEMENT_REPEAT_DELAY;
    } else {
      game_state->lateral_movement_counter = 0;
      game_state->lateral_movement_repeating = true;
    }
    if (game_state->lateral_movement_direction != 0) {
      const int test_column = game_state->active_piece_column +
                              game_state->lateral_movement_direction;
      if (!TestActivePieceCollision(game_state, game_state->active_piece_row,
                                    test_column)) {
        game_state->active_piece_column = test_column;
      }
    }
  }
}