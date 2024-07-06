#include "constants.h"
#include "game_state.h"
#include "raylib.h"

#include "playfield.h"
#include "square.h"
#include "tetrominos.h"

#include <assert.h>
#include <math.h>
#include <stdlib.h>

Playfield *CreateInitialPlayfield() {
  Playfield *playfield = (Playfield *)malloc(sizeof(Playfield));

  for (int row = 0; row < PLAYFIELD_HEIGHT; row++) {
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      playfield->squares[row][col] = EMPTY_SQUARE;
    }
  }

  return playfield;
}

void DestroyPlayfield(Playfield *playfield) { free(playfield); }

void CopyPlacedSquaresToPlayfield(const GameState *game_state,
                                  Playfield *playfield) {
  for (int i = 0; i < PLAYFIELD_HEIGHT; i++) {
    for (int j = 0; j < PLAYFIELD_WIDTH; j++) {
      if (game_state->squares[i][j] != EMPTY_SQUARE) {
        assert(playfield->squares[i][j] == EMPTY_SQUARE);
        assert((game_state->squares[i][j] & ACTIVE_MASK) == 0);
        assert((game_state->squares[i][j] & GHOST_MASK) == 0);
        assert(game_state->squares[i][j] & PLACED_MASK);
        playfield->squares[i][j] = game_state->squares[i][j];
      }
    }
  }
}

void CopyActivePieceToPlayfield(const GameState *game_state,
                                Playfield *playfield) {
  // assert(TestActivePieceCollision(game_state, game_state->active_piece_row,
  //                                 game_state->active_piece_col) == false);

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (game_state->active_piece[i][j] != EMPTY_SQUARE) {
        assert(playfield->squares[game_state->active_piece_row + i]
                                 [game_state->active_piece_col + j] ==
               EMPTY_SQUARE);
        playfield->squares[game_state->active_piece_row + i]
                          [game_state->active_piece_col + j] =
            game_state->active_piece[i][j] | ACTIVE_MASK;
      }
    }
  }
}

void CopyGhostPieceToPlayfield(const GameState *game_state,
                               Playfield *playfield) {
  int ghost_piece_row = game_state->ghost_piece_row;
  int ghost_piece_col = game_state->active_piece_col;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (game_state->active_piece[i][j] != EMPTY_SQUARE) {
        const int current_square =
            playfield->squares[ghost_piece_row + i][ghost_piece_col + j];
        assert(current_square == EMPTY_SQUARE ||
               (current_square & ACTIVE_MASK));
        if (current_square != EMPTY_SQUARE) {
          continue;
        }
        playfield->squares[ghost_piece_row + i][ghost_piece_col + j] =
            (game_state->active_piece[i][j] | GHOST_MASK) & (~ACTIVE_MASK);
      }
    }
  }
}

Color PlayfieldGetColorForScore(int word_score) {
  if (word_score < 100) {
    return Fade(DARKPURPLE, 1.0f);
  }
  if (word_score <= 400) {
    return Fade(DARKBLUE, 1.0f);
  }
  if (word_score <= 1000) {
    return Fade(DARKGREEN, 1.0f);
  }
  return Fade(RED, 1.0f);
}

void DisplayPlayfield(const Playfield *playfield, const GameState *game_state,
                      const Font *ui_font, const Font *letter_font) {
  DrawRectangle(300, 25, 200, 400, DARKGRAY);

  const int first_visible_row = PLAYFIELD_HEIGHT - VISIBLE_PLAYFIELD_HEIGHT;
  for (int row = first_visible_row; row < PLAYFIELD_HEIGHT; row++) {
    int display_row = row - first_visible_row;
    bool cleared_line = game_state->cleared_lines[row];
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      Color color = BLACK;
      int square = playfield->squares[row][col];
      int ml = square >> 8;
      bool line_clear_after_flash = false;
      if (game_state->cleared_lines[row] &&
          game_state->line_clear_counter >= LINE_CLEAR_FLASH_DELAY) {
        line_clear_after_flash = true;
      }
      if (cleared_line) {
        if (game_state->line_clear_counter >= LINE_CLEAR_FLASH_DELAY) {
          const int frames_since_flash =
              game_state->line_clear_counter - LINE_CLEAR_FLASH_DELAY;
          square = EMPTY_SQUARE;
        }
      }
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
      DrawRectangle(301 + col * 20, 26 + display_row * 20, 18, 18, color);
      if (square & GHOST_MASK) {
        assert((square & ACTIVE_MASK) == 0);
        assert((square & PLACED_MASK) == 0);
        DrawRectangle(301 + col * 20, 26 + display_row * 20, 18, 18,
                      Fade(WHITE, 0.4f));
        DrawRectangle(301 + col * 20 + 2, 26 + display_row * 20 + 2, 14, 14,
                      Fade(BLACK, 0.9f));
        const int ghost_distance =
            game_state->ghost_piece_row - game_state->active_piece_row;
        float ghost_alpha =
            0.3f + (float)(ghost_distance) / (float)PLAYFIELD_HEIGHT;
        if (ghost_alpha > 1.0f) {
          ghost_alpha = 1.0f;
        }
        DrawRectangle(301 + col * 20, 26 + display_row * 20, 18, 18,
                      Fade(BLACK, 1.0 - ghost_alpha));
      }
      if (square & ACTIVE_MASK) {
        DrawRectangle(301 + col * 20, 26 + display_row * 20, 18, 18,
                      Fade(LIGHTGRAY, 0.4f));
        DrawRectangle(301 + col * 20 + 2, 26 + display_row * 20 + 2, 14, 14,
                      Fade(WHITE, 0.1f));
      }
      if (square & PLACED_MASK) {
        DrawRectangle(301 + col * 20, 26 + display_row * 20, 18, 18,
                      Fade(LIGHTGRAY, 0.5f));
        DrawRectangle(301 + col * 20 + 2, 26 + display_row * 20 + 2, 14, 14,
                      Fade(BLACK, 0.1f));
      }
      const int word_id = game_state->horizontal_word_ids[row][col];
      float score_alpha = 0.0f;
      int word_score = 0;
      if (word_id == 0 && line_clear_after_flash) {
        ml = 0;
      }
      bool is_word_start_col = false;
      bool is_word_end_col = false;
      if (word_id != 0) {
        int word_start_col = col;
        while (word_start_col > 0 &&
               game_state->horizontal_word_ids[row][word_start_col - 1] ==
                   word_id) {
          word_start_col--;
        }
        int word_end_col = col;
        while (word_end_col < PLAYFIELD_WIDTH - 1 &&
               game_state->horizontal_word_ids[row][word_end_col + 1] ==
                   word_id) {
          word_end_col++;
        }
        is_word_start_col = word_start_col == col;
        is_word_end_col = word_end_col == col;
        word_score = game_state->horizontal_word_scores[row][word_start_col];
        // printf("row: %d, col: %d, word_id: %d, word_score: %d\n", row, col,
        //        word_id, word_score);
        /*
        const float log_score = log(word_score);

        float score_alpha = 0.1 * log_score + 0.5;
        if (score_alpha > 1.0) {
          score_alpha = 1.0f;
        }
        if (score_alpha < 0.5f) {
          score_alpha = 0.5f;
        }
        */
        if (!cleared_line) {
          DrawRectangle(300 + col * 20, 25 + display_row * 20, 20, 20, WHITE);
        }
      }
      Color word_color = BLACK;
      if (ml != 0) {

        char text[2] = "A";
        text[0] += ml - 1;
        Vector2 text_size = MeasureTextEx(*letter_font, text, 12, 1);
        word_color = Fade(BLACK, 0.9);
        if (square & GHOST_MASK) {
          const int ghost_distance =
              game_state->ghost_piece_row - game_state->active_piece_row;
          float ghost_alpha =
              0.3f + (float)(ghost_distance) / (float)PLAYFIELD_HEIGHT;
          if (ghost_alpha > 1.0f) {
            ghost_alpha = 1.0f;
          }
          word_color = Fade(color, ghost_alpha);
        }
        if (word_score) {
          // word_color = RED;
          word_color = PlayfieldGetColorForScore(word_score);
        }
        if (line_clear_after_flash) {
          DrawRectangle(301 + col * 20, 25 + display_row * 20, 20, 20,
                        word_color);
        }
        Color text_color = line_clear_after_flash ? WHITE : word_color;
        DrawTextEx(*letter_font, text,
                   (Vector2){301 + col * 20 + 9 - text_size.x / 2,
                             26 + display_row * 20 + 9 - text_size.y / 2},
                   12, 1, text_color);
      }
    }
    if (game_state->cleared_lines[row]) {
      if (game_state->line_clear_counter < LINE_CLEAR_FLASH_DELAY) {
        const float flash_alpha =
            1.0f * game_state->line_clear_counter / LINE_CLEAR_FLASH_DELAY;
        DrawRectangle(300, 26 + display_row * 20, 200, 20,
                      Fade(WHITE, flash_alpha));
      }
    }
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      if (game_state->horizontal_word_scores[row][col] != 0) {
        const int word_id = game_state->horizontal_word_ids[row][col];
        int end_col = col;
        while (end_col < PLAYFIELD_WIDTH - 1 &&
               game_state->horizontal_word_ids[row][end_col + 1] == word_id) {
          end_col++;
        }
        Color word_color = PlayfieldGetColorForScore(
            game_state->horizontal_word_scores[row][col]);
        if (game_state->cleared_lines[row] &&
            game_state->line_clear_counter <
                LINE_CLEAR_FLASH_DELAY + LINE_CLEAR_COLLAPSE_DELAY) {
          word_color = WHITE;
        }
        const int word_length = end_col - col + 1;
        // Draw a box with rounded corners
        DrawRectangleRoundedLines((Rectangle){300 + 2.5 + col * 20,
                                              25 + 2.5 + display_row * 20,
                                              20 * word_length - 5, 20 - 5},
                                  0.5, 4, 2.5, word_color);
      }
    }
  }

  if (game_state->paused) {
    DrawRectangle(300, 25, 200, 400, Fade(BLACK, 0.85f));
    const char *paused_text = "PAUSED";
    Vector2 text_size = MeasureTextEx(*ui_font, paused_text, 24, 1);
    DrawTextEx(*ui_font, paused_text, (Vector2){400 - text_size.x / 2, 180}, 24,
               1.0, GREEN);
    DrawLine(375, 210, 425, 210, LIGHTGRAY);
    const char *resumed_text = "PRESS START TO RESUME";
    Vector2 resumed_text_size = MeasureTextEx(*ui_font, resumed_text, 16, 1);
    DrawTextEx(*ui_font, resumed_text,
               (Vector2){400 - resumed_text_size.x / 2, 220}, 16, 1.0,
               LIGHTGRAY);
  }
  if (game_state->topped_out || game_state->reached_line_cap) {
    DrawRectangle(300, 25, 200, 400, Fade(BLACK, 0.85f));
    const char *game_over_text = "GAME OVER";
    Vector2 text_size = MeasureTextEx(*ui_font, game_over_text, 24, 1);
    DrawTextEx(*ui_font, game_over_text, (Vector2){400 - text_size.x / 2, 160},
               24, 1.0, RED);

    if (game_state->topped_out) {
      const char *topped_out_text = "TOPPED OUT";
      Vector2 topped_out_text_size =
          MeasureTextEx(*ui_font, topped_out_text, 16, 1);
      DrawTextEx(*ui_font, topped_out_text,
                 (Vector2){400 - topped_out_text_size.x / 2, 185}, 16, 1.0,
                 LIGHTGRAY);
    } else {
      char *reached_line_cap_text;
      asprintf(&reached_line_cap_text, "%d LINES", MAX_LINES);
      Vector2 reached_line_cap_text_size =
          MeasureTextEx(*ui_font, reached_line_cap_text, 16, 1);
      DrawTextEx(*ui_font, reached_line_cap_text,
                 (Vector2){400 - reached_line_cap_text_size.x / 2, 185}, 16,
                 1.0, LIGHTGRAY);
      free(reached_line_cap_text);
    }

    DrawLine(375, 210, 425, 210, LIGHTGRAY);

    const char *restart_text = "PRESS START TO PLAY AGAIN";
    Vector2 restart_text_size = MeasureTextEx(*ui_font, restart_text, 16, 1);
    DrawTextEx(*ui_font, restart_text,
               (Vector2){400 - restart_text_size.x / 2, 220}, 16, 1.0,
               LIGHTGRAY);
  }

  // Draw DARKGREY border around the playfield
  DrawRectangleLines(300, 25, 200, 400, DARKGRAY);
}