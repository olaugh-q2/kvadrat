#include "raylib.h"

#include "constants.h"
#include "game_state.h"
#include "tetrominos.h"

#include <assert.h>
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

  game_state->soft_lock_counter = 0;
  game_state->soft_locking = false;
  game_state->lock_counter = 0;
  game_state->locking_piece = false;

  game_state->line_clear_counter = 0;
  game_state->clearing_lines = false;

  for (int i = 0; i < PLAYFIELD_HEIGHT; i++) {
    game_state->cleared_lines[i] = false;
  }
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
    // piece_queue[start_index + i] = T_PIECE;
    piece_queue[start_index + i] = bag[i];
  }
}

bool TestPieceCollision(const GameState *game_state, const int piece[4][4],
                        int row, int col) {
  for (int piece_row = 0; piece_row < 4; piece_row++) {
    for (int piece_col = 0; piece_col < 4; piece_col++) {
      if (piece[piece_row][piece_col] != EMPTY_SQUARE) {
        // printf("piece[%d]][%d]: %d\n", piece_row, piece_col,
        // piece[piece_row][piece_col]);
        const int playfield_row = piece_row + row;
        const int playfield_col = piece_col + col;
        if (playfield_row < 0 || playfield_row >= PLAYFIELD_HEIGHT ||
            playfield_col < 0 || playfield_col >= PLAYFIELD_WIDTH ||
            game_state->squares[playfield_row][playfield_col] != EMPTY_SQUARE) {
          if ((playfield_row < 0) || playfield_row >= PLAYFIELD_HEIGHT) {
            //printf("collision: row out of bounds: %d\n", playfield_row);
          }
          if ((playfield_col < 0) || playfield_col >= PLAYFIELD_WIDTH) {
            //printf("collision: col out of bounds: %d\n", playfield_col);
          }
          if ((playfield_col >= 0) && (playfield_col < PLAYFIELD_WIDTH) &&
              (playfield_row >= 0) && (playfield_row < PLAYFIELD_HEIGHT) &&
              game_state->squares[playfield_row][playfield_col] !=
                  EMPTY_SQUARE) {
            //printf("collision: square not empty: %d\n",
            //       game_state->squares[playfield_row][playfield_col]);
          }
          return true;
        }
      }
    }
  }
  return false;
}

bool TestActivePieceCollision(const GameState *game_state, int row, int col) {
  //printf("TestActivePieceCollision row: %d, col: %d\n", row, col);
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
  //printf("MaybeMovePieceLaterally game_state->lateral_movement_counter: %d\n",
  //       game_state->lateral_movement_counter);
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
        //printf("successfully moved piece from column %d to %d\n",
        //       game_state->active_piece_col, test_column);
        game_state->active_piece_col = test_column;
        game_state->soft_lock_counter = 0;
        game_state->soft_locking = false;
      }
    }
  }
}

void UpdateRotationIntent(GameState *game_state) {
  //printf("UpdateRotationIntent game_state->rotation_counter: %d\n",
  //       game_state->rotation_counter);
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
  //printf("MaybeRotatePiece game_state->rotation_counter: %d\n",
  //       game_state->rotation_counter);
  if (game_state->rotation_counter < ROTATION_DELAY) {
    return;
  }
  //printf("MaybeRotatePiece game_state->rotation_counter: %d\n",
  //       game_state->rotation_counter);
  if (game_state->rotation_direction != 0) {
    //printf("MaybeRotatePiece game_state->rotation_direction: %d\n",
    //       game_state->rotation_direction);
    const int test_rotation = (game_state->active_piece_rotation +
                               game_state->rotation_direction + 4) %
                              4;
    //printf("Testing rotation from %d to %d\n",
    //       game_state->active_piece_rotation, test_rotation);
    int test_piece[4][4];
    CreatePiece(game_state->active_piece_index, test_piece, test_rotation);
    for (int test_index = 0; test_index <= 4; test_index++) {
      //printf("MaybeRotatePiece test_index: %d\n", test_index);
      int row_delta;
      int col_delta;
      GetRotationOffsets(
          game_state->active_piece_index, game_state->active_piece_rotation,
          game_state->rotation_direction, test_index, &row_delta, &col_delta);
      //printf("that would use offsets row_delta: %d, col_delta: %d\n", row_delta,
      //       col_delta);
      const int test_row = game_state->active_piece_row + row_delta;
      const int test_col = game_state->active_piece_col + col_delta;
      //printf("test_row is %d + %d = %d\n", game_state->active_piece_row,
      //       row_delta, test_row);
      //printf("test_col is %d + %d = %d\n", game_state->active_piece_col,
      //       col_delta, test_col);
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
        //printf("successful rotation to %d\n", test_rotation);
        game_state->soft_lock_counter = 0;
        game_state->soft_locking = false;
        return;
      }
      //printf("collision!\n");
    }
  }
}

void MaybeApplyGravity(GameState *game_state) {
  //printf("MaybeApplyGravity game_state->gravity_counter: %d\n",
  //       game_state->gravity_counter);
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
      //printf("gravity can't move piece down\n");
      game_state->soft_locking = true;
    }
  }
}

void MaybeHardDrop(GameState *game_state) {
  printf("MaybeHardDrop game_state->hard_dropped: %d\n",
         game_state->hard_dropped);
  if (!IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
    game_state->hard_dropped = false;
  }
  if (game_state->hard_dropped) {
    // Don't hard drop a second piece without first releasing the button
    return;
  }
  if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT) ||
        IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
      // Don't risk accidental hard drop with diagonal input
      return;
    }
    game_state->active_piece_row = game_state->ghost_piece_row;
    game_state->hard_dropped = true;
    LockPiece(game_state);
    PlaceLockedPiece(game_state);
    CheckForLineClears(game_state);
  } else {
    game_state->hard_dropped = false;
  }
}

void UpdateGhostPieceRow(GameState *game_state) {
  //printf("UpdateGhostPieceRow\n");
  game_state->ghost_piece_row = game_state->active_piece_row;
  while (!TestActivePieceCollision(game_state, game_state->ghost_piece_row + 1,
                                   game_state->active_piece_col)) {
    assert(game_state->ghost_piece_row < PLAYFIELD_HEIGHT + 5);
    game_state->ghost_piece_row++;
  }
}

void LockPiece(GameState *game_state) {
  game_state->locking_piece = true;
  game_state->lock_counter = 0;
}

void UpdateLockingPiece(GameState *game_state) {
  game_state->lock_counter++;
  if (game_state->lock_counter >= ENTRY_DELAY) {
    game_state->lock_counter = 0;
    game_state->locking_piece = false;
  }
}

void PlaceLockedPiece(GameState *game_state) {
  for (int piece_row = 0; piece_row < 4; piece_row++) {
    for (int piece_col = 0; piece_col < 4; piece_col++) {
      const int active_piece_square =
          game_state->active_piece[piece_row][piece_col];
      if (active_piece_square == EMPTY_SQUARE) {
        continue;
      }
      const int playfield_row = game_state->active_piece_row + piece_row;
      const int playfield_col = game_state->active_piece_col + piece_col;
      const int placed_square =
          (active_piece_square & ~ACTIVE_MASK) | PLACED_MASK;
      assert(playfield_row >= 0 && playfield_row < PLAYFIELD_HEIGHT);
      assert(playfield_col >= 0 && playfield_col < PLAYFIELD_WIDTH);
      game_state->squares[playfield_row][playfield_col] = placed_square;
    }
  }
}

void SpawnNewPiece(GameState *game_state) {
  // Unclear why this is needed here, main.c game loop already does this.
  game_state->soft_locking = false;
  game_state->soft_lock_counter = 0;

  printf("SpawnNewPiece\n");

  game_state->pieces_until_redraw--;
  for (int i = 0; i < 13; i++) {
    game_state->piece_queue[i] = game_state->piece_queue[i + 1];
  }
  if (game_state->pieces_until_redraw == 0) {
    DrawRandomPieces(game_state->piece_queue, 6);
    game_state->pieces_until_redraw = 7;
  }
  // We must have at least 7 known pieces to fill the next queue.
  for (int i = 0; i < 7; i++) {
    assert(game_state->piece_queue[i] >= 1 && game_state->piece_queue[i] <= 7);
  }
  game_state->active_piece_index = game_state->piece_queue[0];
  assert(game_state->active_piece_index >= 1 &&
         game_state->active_piece_index <= 7);
  game_state->active_piece_rotation = ROTATION_0;
  CreatePiece(game_state->active_piece_index, game_state->active_piece,
              ROTATION_0);
  game_state->active_piece_row = 0;
  game_state->active_piece_col = SpawnColumn(game_state->active_piece_index);

  game_state->gravity_counter = 0;
}

void CheckForLineClears(GameState *game_state) {
  printf("CheckForLineClears:");
  for (int row = 0; row < PLAYFIELD_HEIGHT; row++) {
    bool line_clear = true;
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      if (game_state->squares[row][col] == EMPTY_SQUARE) {
        line_clear = false;
        break;
      }
    }
    if (line_clear) {
      printf(" %d", row);
      game_state->cleared_lines[row] = true;
      game_state->clearing_lines = true;
    }
  }
  printf("\n");
}

void UpdateAfterClearedLines(GameState *game_state) {
  printf("UpdateAfterClearedLines\n");
  int cleared_lines = 0;
  for (int row = PLAYFIELD_HEIGHT - 1; row >= 0; row--) {
    if (game_state->cleared_lines[row]) {
      for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
        game_state->squares[row][col] = EMPTY_SQUARE;
      }
      cleared_lines++;
    } else if (cleared_lines > 0) {
      for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
        game_state->squares[row + cleared_lines][col] =
            game_state->squares[row][col];
        game_state->squares[row][col] = EMPTY_SQUARE;
      }
    }
  }
  for (int row = 0; row < PLAYFIELD_HEIGHT; row++) {
    game_state->cleared_lines[row] = false;
  }
}