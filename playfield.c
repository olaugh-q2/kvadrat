#include "constants.h"
#include "raylib.h"

#include "playfield.h"
#include "square.h"
#include "tetrominos.h"

#include <assert.h>
#include <stdlib.h>

Playfield *CreateInitialPlayfield() {
  Playfield *playfield = (Playfield *)malloc(sizeof(Playfield));

  for (int i = 0; i < PLAYFIELD_WIDTH; i++) {
    for (int j = 0; j < PLAYFIELD_HEIGHT; j++) {
      playfield->squares[i][j] = EMPTY_SQUARE;
    }
  }

  return playfield;
}

void DestroyPlayfield(Playfield *playfield) { free(playfield); }

void CopyActivePieceToPlayfield(const GameState *game_state,
                                Playfield *playfield) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (game_state->active_piece[i][j] != EMPTY_SQUARE) {
        assert(playfield->squares[game_state->active_piece_row + i]
                                 [game_state->active_piece_column + j] ==
               EMPTY_SQUARE);
        playfield->squares[game_state->active_piece_row + i]
                          [game_state->active_piece_column + j] =
            game_state->active_piece[i][j];
      }
    }
  }
}

void DisplayPlayfield(const Playfield *playfield) {
  DrawRectangle(300, 25, 200, 400, DARKGRAY);

  for (int row = 0; row < PLAYFIELD_HEIGHT; row++) {
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      Color color = BLACK;
      int square = playfield->squares[row][col];
      if (square != EMPTY_SQUARE) {
        const int piece_type = square & PIECE_MASK;
        switch (piece_type) {
        case I_PIECE:
          color = COLOR_ACTIVE_I_PIECE;
          break;
        case J_PIECE:
          color = COLOR_ACTIVE_J_PIECE;
          break;
        case L_PIECE:
          color = COLOR_ACTIVE_L_PIECE;
          break;
        case O_PIECE:
          color = COLOR_ACTIVE_O_PIECE;
          break;
        case S_PIECE:
          color = COLOR_ACTIVE_S_PIECE;
          break;
        case Z_PIECE:
          color = COLOR_ACTIVE_Z_PIECE;
          break;
        case T_PIECE:
          color = COLOR_ACTIVE_T_PIECE;
          break;
        }
      }

      DrawRectangle(301 + col * 20, 26 + row * 20, 18, 18, color);
    }
  }
}