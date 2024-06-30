#include "raylib.h"

#include "constants.h"
#include "game_state.h"
#include "tetrominos.h"

#include <stdio.h>
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
  game_state->active_piece_col = SpawnColumn(game_state->active_piece_index);
  game_state->pieces_until_redraw = 1;

  game_state->lateral_movement_counter = 0;
  game_state->lateral_movement_direction = 0;
  game_state->lateral_movement_repeating = false;

  game_state->rotation_counter = 0;
  game_state->rotation_direction = 0;

  game_state->gravity_counter = 0;
  game_state->level = 1;
  game_state->gravity_delay = 20;
  game_state->soft_drop_delay = 2;

  game_state->hard_dropped = false;

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
    //piece_queue[start_index + i] = T_PIECE;
    piece_queue[start_index + i] = bag[i];
  }
}

bool TestPieceCollision(const GameState *game_state, const int piece[4][4],
                        int row, int col) {
  for (int piece_row = 0; piece_row < 4; piece_row++) {
    for (int piece_col = 0; piece_col < 4; piece_col++) {
      if (piece[piece_row][piece_col] != EMPTY_SQUARE) {
        //printf("piece[%d]][%d]: %d\n", piece_row, piece_col, piece[piece_row][piece_col]);
        const int playfield_row = piece_row + row;
        const int playfield_col = piece_col + col;
        if (playfield_row < 0 || playfield_row >= PLAYFIELD_HEIGHT ||
            playfield_col < 0 || playfield_col >= PLAYFIELD_WIDTH ||
            game_state->squares[playfield_row][playfield_col] != EMPTY_SQUARE) {
          if ((playfield_row < 0) || playfield_row >= PLAYFIELD_HEIGHT) {
            printf("collision: row out of bounds: %d\n", playfield_row);
          }
          if ((playfield_col < 0) || playfield_col >= PLAYFIELD_WIDTH) {
            printf("collision: col out of bounds: %d\n", playfield_col);
          }
          if ((playfield_col >= 0) && (playfield_col < PLAYFIELD_WIDTH) &&
              (playfield_row >= 0) && (playfield_row < PLAYFIELD_HEIGHT) &&
              game_state->squares[playfield_row][playfield_col] !=
                  EMPTY_SQUARE) {
            printf("collision: square not empty: %d\n", game_state->squares[playfield_row][playfield_col]);
          }
          return true;
        }
      }
    }
  }
  return false;
}

bool TestActivePieceCollision(const GameState *game_state, int row, int col) {
  return TestPieceCollision(game_state, game_state->active_piece, row, col);
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
      const int test_column =
          game_state->active_piece_col + game_state->lateral_movement_direction;
      if (!TestActivePieceCollision(game_state, game_state->active_piece_row,
                                    test_column)) {
        printf("successfully moved piece from column %d to %d\n",
               game_state->active_piece_col, test_column);
        game_state->active_piece_col = test_column;
      }
    }
  }
}

void UpdateRotationIntent(GameState *game_state) {
  game_state->rotation_counter++;
  int new_direction = 0;
  if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
    new_direction = COUNTERCLOCKWISE_DIRECTION;
  } else if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
    new_direction = CLOCKWISE_DIRECTION;
  }
  if (new_direction != game_state->rotation_direction) {
    game_state->rotation_counter = ROTATION_DELAY;
  }
  if (new_direction == 0) {
    game_state->rotation_counter = 0;
  }
  game_state->rotation_direction = new_direction;
}

void MaybeRotatePiece(GameState *game_state) {
  if (game_state->rotation_counter < ROTATION_DELAY) {
    return;
  }
  printf("MaybeRotatePiece game_state->rotation_counter: %d\n",
         game_state->rotation_counter);
  if (game_state->rotation_direction != 0) {
    printf("MaybeRotatePiece game_state->rotation_direction: %d\n",
           game_state->rotation_direction);
    const int test_rotation = (game_state->active_piece_rotation +
                               game_state->rotation_direction + 4) %
                              4;
    printf("Testing rotation from %d to %d\n",
           game_state->active_piece_rotation, test_rotation);
    int test_piece[4][4];
    CreatePiece(game_state->active_piece_index, test_piece, test_rotation);
    for (int test_index = 0; test_index <= 4; test_index++) {
      printf("MaybeRotatePiece test_index: %d\n", test_index);
      int row_delta;
      int col_delta;
      GetRotationOffsets(
          game_state->active_piece_index, game_state->active_piece_rotation,
          game_state->rotation_direction, test_index, &row_delta, &col_delta);
      printf("that would use offsets row_delta: %d, col_delta: %d\n", row_delta,
             col_delta);
      const int test_row = game_state->active_piece_row + row_delta;
      const int test_col = game_state->active_piece_col + col_delta;
      printf("test_row is %d + %d = %d\n", game_state->active_piece_row,
             row_delta, test_row);
      printf("test_col is %d + %d = %d\n", game_state->active_piece_col,
              col_delta, test_col);
      if (!TestPieceCollision(game_state, test_piece, test_row, test_col)) {
        game_state->active_piece_rotation = test_rotation;
        for (int i = 0; i < 4; i++) {
          for (int j = 0; j < 4; j++) {
            game_state->active_piece[i][j] = test_piece[i][j];
          }
        }
        game_state->active_piece_row = test_row;
        game_state->active_piece_col = test_col;
        game_state->rotation_counter = 0;
        printf("successful rotation to %d\n", test_rotation);
        return;
      }
      printf("collision!\n");
    }
  }
}

void MaybeApplyGravity(GameState *game_state) {
  game_state->gravity_counter++;
  int gravity_delay = IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)
                          ? game_state->soft_drop_delay
                          : game_state->gravity_delay;
  if (game_state->gravity_counter >= gravity_delay) {
    game_state->gravity_counter = 0;
    if (!TestActivePieceCollision(game_state, game_state->active_piece_row + 1,
                                  game_state->active_piece_col)) {
      game_state->active_piece_row++;
    } else {
      printf("gravity can't move piece down\n");
    }
  }
}

void MaybeHardDrop(GameState *game_state) {
  if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT) ||
        IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
      // Don't risk accidental hard drop with diagonal input
      return;
    }
    game_state->active_piece_row = game_state->ghost_piece_row;
    game_state->hard_dropped = true;
  } else {
    game_state->hard_dropped = false;
  }
}

void UpdateGhostPieceRow(GameState *game_state) {
  game_state->ghost_piece_row = game_state->active_piece_row;
  while (!TestActivePieceCollision(game_state, game_state->ghost_piece_row + 1,
                                   game_state->active_piece_col)) {
    game_state->ghost_piece_row++;
  }
}