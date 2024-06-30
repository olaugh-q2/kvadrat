#include "next.h"
#include "game_state.h"
#include "raylib.h"
#include "tetrominos.h"

void DisplayPiece(int piece_type, int queue_index) {
  int piece[4][4];
  CreatePiece(piece_type, piece, ROTATION_0);
  Color color;
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
  float piece_vertical_offset = 0.0f;
  if (piece_type == I_PIECE) {
    piece_vertical_offset = -0.5f;
  } else if (piece_type == O_PIECE) {
    piece_vertical_offset = -1.0f;
  }
  float piece_horizontal_offset = 0.5f;
  if (piece_type == I_PIECE) {
    piece_horizontal_offset = 0.0f;
  } else if (piece_type == O_PIECE) {
    piece_horizontal_offset = 0.0f;
  }

  int piece_box_vertical_offset = 5;
  if (queue_index >= 2) {
    piece_box_vertical_offset = 65 + (queue_index - 2) * 40;
  }

  const int mino_size = queue_index < 2 ? 20 : 10;
  const int piece_box_horizontal_offset = queue_index < 2 ? 5 : 25;

  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 4; col++) {
      if (piece[row][col]) {
        float effective_row = (float)(row) + piece_vertical_offset;
        float effective_col = (float)(col) + piece_horizontal_offset;
        int rectangle_x =
            505 + piece_box_horizontal_offset + effective_col * mino_size;
        int rectangle_y =
            40 + piece_box_vertical_offset + effective_row * mino_size;
        DrawRectangle(rectangle_x, rectangle_y, mino_size, mino_size, DARKGRAY);
        DrawRectangle(rectangle_x + 1, rectangle_y + 1, mino_size - 2,
                      mino_size - 2, color);
        DrawRectangle(rectangle_x + 1, rectangle_y + 1, mino_size - 2,
                      mino_size - 2, Fade(LIGHTGRAY, 0.4f));
        DrawRectangle(rectangle_x + 2, rectangle_y + 2, mino_size - 4,
                      mino_size - 4, Fade(WHITE, 0.1f));
      }
    }
  }
}

void DisplayNext(const GameState *game_state, const Font *font) {
  DrawTextEx(*font, "NEXT", (Vector2){510, 22}, font->baseSize * 0.5, 1.0,
             BLACK);
  DrawRectangle(505, 40, 90, 250, BLACK);

  for (int i = 1; i <= 6; i++) {
    DisplayPiece(game_state->piece_queue[i], i);
  }
}