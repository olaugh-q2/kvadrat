#include "kwg.h"
#include "raylib.h"

#include "bag.h"
#include "constants.h"
#include "game_state.h"
#include "square.h"
#include "tetrominos.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

GameState *CreateInitialGameState(const char *bags_filename,
                                  const char *kwg_filename) {
  GameState *game_state = (GameState *)malloc(sizeof(GameState));

  game_state->num_lines = 0;
  game_state->num_pieces = 0;
  game_state->num_words = 0;
  game_state->sum_of_word_lengths = 0;
  game_state->total_score = 0;
  game_state->unpaused_frame_counter = 0;

  LoadKwg(game_state, kwg_filename);

  LoadBags(game_state, bags_filename);
  DrawWordsFromBag(game_state, 0);
  game_state->words_until_redraw = 1;
  for (int i = 0; i < PLAYFIELD_HEIGHT; i++) {
    for (int j = 0; j < PLAYFIELD_WIDTH; j++) {
      game_state->squares[i][j] = EMPTY_SQUARE;
    }
  }
  DrawRandomPieces(game_state->piece_queue, 0);
  game_state->active_piece_index = game_state->piece_queue[0];
  game_state->active_piece_rotation = ROTATION_0;
  int letters[4];
  for (int i = 0; i < 4; i++) {
    letters[i] = game_state->word_letters[0][i];
  }
  CreatePiece(game_state->active_piece_index, game_state->active_piece,
              ROTATION_0, letters);
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
  game_state->gravity_delay = 12;
  game_state->soft_drop_delay = 1;

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

  game_state->hard_drop_sound = LoadSound("harddrop.ogg");
  game_state->soft_drop_sound = LoadSound("floor.ogg");
  game_state->line_clear_sound = LoadSound("clearline.mp3");
  game_state->quad_clear_sound = LoadSound("clearquad.mp3");
  game_state->rotate_sound = LoadSound("rotate.ogg");
  game_state->move_wave = LoadWave("move.ogg");
  for (int i = 0; i < 10; i++) {
    game_state->move_sounds[i] = LoadSoundFromWave(game_state->move_wave);
    SetSoundVolume(game_state->move_sounds[i], 0.4f);
  }
  game_state->move_sound_index = 0;

  game_state->paused = false;
  game_state->pause_sound = LoadSound("pause.mp3");

  for (int row = 0; row < PLAYFIELD_HEIGHT; row++) {
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      game_state->horizontal_word_ids[row][col] = 0;
      game_state->vertical_word_ids[row][col] = 0;
      game_state->horizontal_word_scores[row][col] = 0;
      game_state->vertical_word_scores[row][col] = 0;
    }
  }

  game_state->num_words_formed = 0;
  game_state->checked_line_clears_this_frame = false;

  return game_state;
}

void DestroyGameState(GameState *game_state) {
  UnloadSound(game_state->hard_drop_sound);
  UnloadSound(game_state->soft_drop_sound);
  UnloadSound(game_state->line_clear_sound);
  UnloadSound(game_state->quad_clear_sound);
  UnloadSound(game_state->rotate_sound);
  UnloadWave(game_state->move_wave);
  for (int i = 0; i < 10; i++) {
    UnloadSound(game_state->move_sounds[i]);
  }
  UnloadSound(game_state->pause_sound);
  free(game_state->bags);
  free(game_state);
}

void LoadKwg(GameState *game_state, const char *filename) {
  KWG *kwg = kwg_create(".", filename);
  if (!kwg) {
    fprintf(stderr, "Failed to load KWG file %s\n", filename);
    exit(1);
  }
  game_state->kwg = kwg;
}

void LoadBags(GameState *game_state, const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Failed to open file %s\n", filename);
    exit(1);
  }
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);
  game_state->bags = (char *)malloc(file_size);
  fread(game_state->bags, 1, file_size, file);
  fclose(file);
  game_state->num_bags = file_size / (28 * 5 + 1);
}

void DrawWordsFromBag(GameState *game_state, int start_index) {
  const int bag_index = rand() % game_state->num_bags;
  for (int i = 0; i < 28; i++) {
    for (int j = 0; j < 4; j++) {
      game_state->word_letters[start_index + i][j] =
          game_state->bags[bag_index * (28 * 5 + 1) + i * 5 + j] - 'A' + 1;
    }
  }
/*  
  char swum[5] = "SWUM";
  char rulb[5] = "RULB";
  char zoea[5] = "ZOEA";
  char abpu[5] = "ABPU";
  char quag[5] = "QUAG";
  char amen[5] = "AMEN";
  for (int i = 0; i < 4; i++) {
    game_state->word_letters[start_index + 0][i] = swum[i] - 'A' + 1;
    game_state->word_letters[start_index + 1][i] = rulb[i] - 'A' + 1;
    game_state->word_letters[start_index + 2][i] = zoea[i] - 'A' + 1;
    game_state->word_letters[start_index + 3][i] = abpu[i] - 'A' + 1;
    game_state->word_letters[start_index + 4][i] = quag[i] - 'A' + 1;
    game_state->word_letters[start_index + 5][i] = amen[i] - 'A' + 1;
  }
*/  
  // TODO: shuffle
}

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
/*  
  piece_queue[start_index + 0] = J_PIECE;
  piece_queue[start_index + 1] = J_PIECE;
  piece_queue[start_index + 2] = I_PIECE;
  piece_queue[start_index + 3] = J_PIECE;
  piece_queue[start_index + 4] = O_PIECE;
  piece_queue[start_index + 5] = J_PIECE;
*/  
}

void CheckWhetherPaused(GameState *game_state) {
  if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
    game_state->paused = !game_state->paused;
    if (game_state->paused) {
      PlaySound(game_state->pause_sound);
    }
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
            // printf("collision: row out of bounds: %d\n", playfield_row);
          }
          if ((playfield_col < 0) || playfield_col >= PLAYFIELD_WIDTH) {
            // printf("collision: col out of bounds: %d\n", playfield_col);
          }
          if ((playfield_col >= 0) && (playfield_col < PLAYFIELD_WIDTH) &&
              (playfield_row >= 0) && (playfield_row < PLAYFIELD_HEIGHT) &&
              game_state->squares[playfield_row][playfield_col] !=
                  EMPTY_SQUARE) {
            // printf("collision: square not empty: %d\n",
            //        game_state->squares[playfield_row][playfield_col]);
          }
          return true;
        }
      }
    }
  }
  return false;
}

bool TestActivePieceCollision(const GameState *game_state, int row, int col) {
  // printf("TestActivePieceCollision row: %d, col: %d\n", row, col);
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
  // printf("MaybeMovePieceLaterally game_state->lateral_movement_counter:
  // %d\n",
  //        game_state->lateral_movement_counter);
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
        // printf("successfully moved piece from column %d to %d\n",
        //        game_state->active_piece_col, test_column);
        game_state->active_piece_col = test_column;
        game_state->soft_lock_counter = 0;
        game_state->soft_locking = false;
        const float panning =
            1.0f - ((float)(game_state->active_piece_col + 1) / 9.0 - 0.5f);
        SetSoundPan(game_state->move_sounds[game_state->move_sound_index],
                    panning);
        PlaySound(game_state->move_sounds[game_state->move_sound_index++]);
        if (game_state->move_sound_index >= 10) {
          game_state->move_sound_index = 0;
        }
      }
    }
  }
}

void UpdateRotationIntent(GameState *game_state) {
  // printf("UpdateRotationIntent game_state->rotation_counter: %d\n",
  //        game_state->rotation_counter);
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
  // printf("MaybeRotatePiece game_state->rotation_counter: %d\n",
  //        game_state->rotation_counter);
  if (game_state->rotation_counter < ROTATION_DELAY) {
    return;
  }
  // printf("MaybeRotatePiece game_state->rotation_counter: %d\n",
  //        game_state->rotation_counter);
  if (game_state->rotation_direction != 0) {
    // printf("MaybeRotatePiece game_state->rotation_direction: %d\n",
    //        game_state->rotation_direction);
    const int test_rotation = (game_state->active_piece_rotation +
                               game_state->rotation_direction + 4) %
                              4;
    // printf("Testing rotation from %d to %d\n",
    //        game_state->active_piece_rotation, test_rotation);
    int test_piece[4][4];
    int letters[4];
    for (int i = 0; i < 4; i++) {
      letters[i] = game_state->word_letters[0][i];
    }
    CreatePiece(game_state->active_piece_index, test_piece, test_rotation,
                letters);
    for (int test_index = 0; test_index <= 4; test_index++) {
      // printf("MaybeRotatePiece test_index: %d\n", test_index);
      int row_delta;
      int col_delta;
      GetRotationOffsets(
          game_state->active_piece_index, game_state->active_piece_rotation,
          game_state->rotation_direction, test_index, &row_delta, &col_delta);
      // printf("that would use offsets row_delta: %d, col_delta: %d\n",
      // row_delta,
      //        col_delta);
      const int test_row = game_state->active_piece_row + row_delta;
      const int test_col = game_state->active_piece_col + col_delta;
      // printf("test_row is %d + %d = %d\n", game_state->active_piece_row,
      //        row_delta, test_row);
      // printf("test_col is %d + %d = %d\n", game_state->active_piece_col,
      //        col_delta, test_col);
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
        // printf("successful rotation to %d\n", test_rotation);
        game_state->soft_lock_counter = 0;
        game_state->soft_locking = false;
        PlaySound(game_state->rotate_sound);
        return;
      }
      // printf("collision!\n");
    }
  }
}

void MaybeApplyGravity(GameState *game_state) {
  // printf("MaybeApplyGravity game_state->gravity_counter: %d\n",
  //        game_state->gravity_counter);
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
      // printf("gravity can't move piece down\n");
      game_state->soft_locking = true;
    }
  }
}

void MaybeHardDrop(GameState *game_state) {
  // printf("MaybeHardDrop game_state->hard_dropped: %d\n",
  //        game_state->hard_dropped);
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
    PlaySound(game_state->hard_drop_sound);
    LockPiece(game_state);
    PlaceLockedPiece(game_state);
    if (!game_state->checked_line_clears_this_frame) {
      MarkFormedWords(game_state);
      CheckForLineClears(game_state);
      game_state->checked_line_clears_this_frame = true;
    }
  } else {
    game_state->hard_dropped = false;
  }
}

void UpdateGhostPieceRow(GameState *game_state) {
  // printf("UpdateGhostPieceRow\n");
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

  // printf("SpawnNewPiece\n");

  game_state->words_until_redraw--;
  for (int i = 0; i < 55; i++) {
    for (int j = 0; j < 4; j++) {
      game_state->word_letters[i][j] = game_state->word_letters[i + 1][j];
    }
  }
  if (game_state->words_until_redraw == 0) {
    DrawWordsFromBag(game_state, 27);
    game_state->words_until_redraw = 28;
  }
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
  int letters[4];
  for (int i = 0; i < 4; i++) {
    letters[i] = game_state->word_letters[0][i];
  }
  CreatePiece(game_state->active_piece_index, game_state->active_piece,
              ROTATION_0, letters);
  game_state->active_piece_row = 0;
  game_state->active_piece_col = SpawnColumn(game_state->active_piece_index);

  game_state->gravity_counter = 0;
  game_state->num_pieces++;
}

void CheckForLineClears(GameState *game_state) {
  printf("CheckForLineClears:");
  int num_lines_cleared = 0;
  int score_sum = 0;
  int words_this_frame = 0;
  for (int row = 0; row < PLAYFIELD_HEIGHT; row++) {
    bool line_clear = true;
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      if (game_state->squares[row][col] == EMPTY_SQUARE) {
        line_clear = false;
        break;
      }
    }
    if (line_clear) {
      printf("line clear in %d\n", row);
      for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
        const int word_score = game_state->horizontal_word_scores[row][col];
        if (word_score > 0) {
          int word_length = 1;
          int word_id = game_state->horizontal_word_ids[row][col];
          for (int col2 = col + 1; col2 < PLAYFIELD_WIDTH; col2++) {
            if (game_state->horizontal_word_ids[row][col2] == word_id) {
              word_length++;
            } else {
              break;
            }
          }
          score_sum += word_score;
          assert(game_state->num_words_formed < MAX_WORDS_PER_GAME);
          const int words_formed_offset =
              game_state->num_words_formed * (PLAYFIELD_WIDTH + 1);
          for (int i = 0; i < word_length; i++) {
            const int ml = game_state->squares[row][col + i] >> 8;
            char c = ml + 'A' - 1;
            game_state->words_formed[words_formed_offset + i] = c;
          }
          game_state->words_formed[words_formed_offset + word_length] = '\0';
          game_state->words_formed_scores[game_state->num_words_formed] =
              word_score;
          game_state->words_formed_lengths[game_state->num_words_formed] =
              word_length;
          game_state->words_formed_at_frame[game_state->num_words_formed] =
              game_state->unpaused_frame_counter;
          game_state->words_formed_indices[game_state->num_words_formed] =
              words_this_frame;
          game_state->num_words_formed++;
          words_this_frame++;
        }
      }
      game_state->cleared_lines[row] = true;
      game_state->clearing_lines = true;
      num_lines_cleared++;
    }
  }
  game_state->num_lines += num_lines_cleared;
  game_state->total_score += score_sum;

  if (num_lines_cleared > 0) {
    if (num_lines_cleared == 4) {
      PlaySound(game_state->quad_clear_sound);
    } else {
      PlaySound(game_state->line_clear_sound);
    }
  }

  // printf("\n");
}

void UpdateAfterClearedLines(GameState *game_state) {
  printf("UpdateAfterClearedLines\n");
  int cleared_lines = 0;
  for (int row = PLAYFIELD_HEIGHT - 1; row >= 0; row--) {
    if (game_state->cleared_lines[row]) {
      for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
        game_state->total_score += game_state->horizontal_word_scores[row][col];
        if (game_state->horizontal_word_scores[row][col] != 0) {
          game_state->num_words++;
          int word_length = 1;
          for (int col2 = col + 1; col2 < PLAYFIELD_WIDTH; col2++) {
            if (game_state->horizontal_word_ids[row][col2] ==
                game_state->horizontal_word_ids[row][col]) {
              word_length++;
            } else {
              break;
            }
          }
          game_state->sum_of_word_lengths += word_length;
        }
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

int ScoreHorizontalWord(GameState *game_state, int row, int start_col,
                        int end_col) {
  int score_sum = 0;
  int length = end_col - start_col + 1;
  for (int col = start_col; col <= end_col; col++) {
    const int square = game_state->squares[row][col];
    const int ml = square >> 8;
    assert(ml >= 1 && ml <= 26);
    const int score = english_letter_scores[ml];
    score_sum += score;
  }
  const int score = score_sum * length * length;
  /*
    printf("ScoreHorizontalWord row: %d, start_col: %d, end_col: %d, score: %d
    [", row, start_col, end_col, score); for (int col = start_col; col <=
    end_col; col++) { const int square = game_state->squares[row][col]; const
    int ml = square >> 8; printf("%c", ml + 'A' - 1);
    }
    printf("]\n");
  */
  return score;
}

void MarkBestHorizontalWords(GameState *game_state, uint32_t dawg_root,
                             uint32_t node_index, bool accepts, int row,
                             int current_col, int word_start_col,
                             int next_word_id, bool color_changed,
                             int previous_color,
                             int best_word_marking[PLAYFIELD_WIDTH],
                             int best_word_scores[PLAYFIELD_WIDTH],
                             int word_marking[PLAYFIELD_WIDTH],
                             int word_scores[PLAYFIELD_WIDTH]) {
  assert(word_marking != NULL);
  assert(word_scores != NULL);

  bool ended_word = false;
  /*
    printf("MarkBestHorizontalWords row: %d, current_col: %d, word_start_col: "
           "%d, next_word_id: %d node_index: %d accepts: %d\n",
           row, current_col, word_start_col, next_word_id, node_index, accepts);
    printf("word_marking ");
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      printf(" %d ", word_marking[col]);
    }
    printf("\nscores ");
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      printf("%d ", word_scores[col]);
    }
    printf("\n");
  */
  if (color_changed && accepts) {
    assert(word_start_col >= 0 && word_start_col < PLAYFIELD_WIDTH);
    const int score =
        ScoreHorizontalWord(game_state, row, word_start_col, current_col - 1);
    if (score >= MINIMUM_WORD_SCORE) {
      word_scores[word_start_col] = score;
      for (int col = word_start_col + 1; col <= current_col - 1; col++) {
        word_scores[col] = 0;
      }
      for (int col = word_start_col; col <= current_col - 1; col++) {
        word_marking[col] = next_word_id;
      }
      /*
          printf("-> word_marking ");
      for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
        printf(" %d ", word_marking[col]);
      }
      printf("\n-> scores ");
      for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
        printf("%d ", word_scores[col]);
      }
      printf("\n");
      */
      ended_word = true;
      next_word_id++;
    }
  }
  if (current_col >= PLAYFIELD_WIDTH) {
    int best_score_sum = 0;
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      best_score_sum += best_word_scores[col];
    }
    int this_score_sum = 0;
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      this_score_sum += word_scores[col];
    }
    if (this_score_sum > 0) {
      printf("row %d: ", row);
      for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
        if (word_marking[col] > 0) {
          printf("%d ", word_marking[col]);
        } else {
          printf(". ");
        }
      }
      printf(" ");
      for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
        if (word_marking[col] > 0) {
          printf("%c", (game_state->squares[row][col] >> 8) + 'A' - 1);
        } else {
          printf(".");
        }
      }
      printf(" ");
      for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
        const int score = word_scores[col];
        if (score > 0) {
          printf(" %d@%d", score, col);
        }
      }
      printf("\n");
    }
    // printf("this_score_sum: %d, best_score_sum: %d\n", this_score_sum,
    //        best_score_sum);
    if (this_score_sum > best_score_sum) {
      for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
        best_word_marking[col] = word_marking[col];
        best_word_scores[col] = word_scores[col];
      }
    }

    return;
  }
  const int square = game_state->squares[row][current_col];
  // printf("square: %d\n", square);
  int new_word_marking[PLAYFIELD_WIDTH];
  int new_word_scores[PLAYFIELD_WIDTH];
  for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
    new_word_marking[col] = word_marking[col];
    new_word_scores[col] = word_scores[col];
  }
  if (square == EMPTY_SQUARE) {
    MarkBestHorizontalWords(game_state, dawg_root, dawg_root, false, row,
                            current_col + 1, -1, next_word_id, false, 0,
                            best_word_marking, best_word_scores,
                            new_word_marking, new_word_scores);
    return;
  }
  const int color = square & PIECE_MASK;
  if (ended_word) {
    color_changed = false;
  } else {
    color_changed |= color && previous_color && (color != previous_color);
  }
  if (ended_word) {
    MarkBestHorizontalWords(game_state, dawg_root, dawg_root, false, row,
                            current_col, -1, next_word_id, false, 0,
                            best_word_marking, best_word_scores,
                            new_word_marking, new_word_scores);
    return;
  }
  const int ml = square >> 8;
  // printf("ml: %d\n", ml);
  uint32_t next_node_index = 0;
  accepts = false;
  for (uint32_t i = node_index;; i++) {
    const uint32_t node = kwg_node(game_state->kwg, i);
    if (kwg_node_tile(node) == ml) {
      next_node_index = kwg_node_arc_index_prefetch(node, game_state->kwg);
      accepts = kwg_node_accepts(node);
      break;
    }
    if (kwg_node_is_end(node)) {
      break;
    }
  }
  // printf("next_node_index: %d\n", next_node_index);
  // printf("accepts: %d\n", accepts);
  if (word_start_col < 0) {
    word_start_col = current_col;
  }

  // end the word with this letter;
  if (color_changed && accepts) {
    int new_word_marking2[PLAYFIELD_WIDTH];
    int new_word_scores2[PLAYFIELD_WIDTH];

    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      new_word_marking2[col] = word_marking[col];
      new_word_scores2[col] = word_scores[col];
    }
    MarkBestHorizontalWords(
        game_state, dawg_root, next_node_index, accepts, row, current_col + 1,
        word_start_col, next_word_id, color_changed, color, best_word_marking,
        best_word_scores, new_word_marking2, new_word_scores2);
  }

  int new_word_marking3[PLAYFIELD_WIDTH];
  int new_word_scores3[PLAYFIELD_WIDTH];
  for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
    new_word_marking3[col] = word_marking[col];
    new_word_scores3[col] = word_scores[col];
  }
  MarkBestHorizontalWords(
      game_state, dawg_root, next_node_index, false, row, current_col + 1,
      word_start_col, next_word_id, color_changed, color, best_word_marking,
      best_word_scores, new_word_marking3, new_word_scores3);

  // printf("start a new word at %d\n", current_col + 1);
  int new_word_marking4[PLAYFIELD_WIDTH];
  int new_word_scores4[PLAYFIELD_WIDTH];
  for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
    new_word_marking4[col] = word_marking[col];
    new_word_scores4[col] = word_scores[col];
  }
  MarkBestHorizontalWords(game_state, dawg_root, dawg_root, false, row,
                          current_col + 1, -1, next_word_id, false, 0,
                          best_word_marking, best_word_scores,
                          new_word_marking4, new_word_scores4);
}

void MarkFormedWords(GameState *game_state) {
  printf("MarkFormedWords\n");
  const uint32_t dawg_root = kwg_get_dawg_root_node_index(game_state->kwg);
  // printf("dawg_root: %d\n", dawg_root);
  int best_word_marking[PLAYFIELD_WIDTH];
  int best_word_scores[PLAYFIELD_WIDTH];
  int word_marking[PLAYFIELD_WIDTH];
  int word_scores[PLAYFIELD_WIDTH];
  for (int row = 0; row < PLAYFIELD_HEIGHT; row++) {
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      best_word_marking[col] = 0;
      best_word_scores[col] = 0;
      word_marking[col] = 0;
      word_scores[col] = 0;
    }
    MarkBestHorizontalWords(game_state, dawg_root, dawg_root, false, row, 0, -1,
                            1, false, 0, best_word_marking, best_word_scores,
                            word_marking, word_scores);
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      game_state->horizontal_word_ids[row][col] = best_word_marking[col];
      game_state->horizontal_word_scores[row][col] = best_word_scores[col];
    }
    int best_score_sum = 0;
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      best_score_sum += best_word_scores[col];
    }
    if (best_score_sum > 0) {
      printf("row %d: ", row);
      for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
        if (best_word_marking[col] > 0) {
          printf("%d ", best_word_marking[col]);
        } else {
          printf(". ");
        }
      }
      printf(" ");
      for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
        if (best_word_marking[col] > 0) {
          printf("%c", (game_state->squares[row][col] >> 8) + 'A' - 1);
        } else {
          printf(".");
        }
      }
      printf(" ");
      for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
        const int score = best_word_scores[col];
        if (score > 0) {
          printf(" %d@%d", score, col);
        }
      }
      printf("\n");
    }
  }
}
