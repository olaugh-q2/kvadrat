#include "constants.h"
#include "formed_words.h"
#include "raylib.h"

#include "game_state.h"
#include "inputs.h"
#include "next.h"
#include "playfield.h"
#include "stats.h"

#include <assert.h>
#include <mach/mach_time.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;

  // seed rand() based on timer
  srand(mach_absolute_time());

  SetConfigFlags(FLAG_WINDOW_HIGHDPI);
  InitWindow(screenWidth, screenHeight, "kvadrat");
  InitAudioDevice();

  int codepoints[512] = {0};
  for (int i = 0; i < 96; i++) {
    codepoints[i] = 32 + i; // ASCII
  }
  for (int i = 0; i < 256; i++) {
    codepoints[96 + i] = 0x400 + i; // Cyrillic
  }
  Font ui_font = LoadFontEx("FuturaCyrillicMedium.ttf", 64, codepoints, 512);
  Font wordgame_font = LoadFontEx("FranklinGothic.ttf", 96, NULL, 0);
  Font stats_font = LoadFontEx("HunDIN1451.ttf", 96, NULL, 0);

  SetTargetFPS(60);
  GameState *game_state = CreateInitialGameState("csw21-bags.txt", "CSW21.kwg");

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(GRAY);

    game_state->checked_line_clears_this_frame = false;
    UpdateLateralMovementIntent(game_state);

    if (!game_state->topped_out && !game_state->reached_line_cap) {
      CheckWhetherPaused(game_state);
    } else {
      MaybeRestartGame(game_state);
    }
    if (!game_state->paused && !game_state->topped_out && !game_state->reached_line_cap) {
      game_state->unpaused_frame_counter++;
    }

    if (!game_state->paused && !game_state->topped_out && !game_state->reached_line_cap &&
        game_state->soft_locking) {
      // printf("soft locking\n");
      game_state->soft_lock_counter++;
      if (game_state->soft_lock_counter >= SOFT_LOCK_DELAY) {
        game_state->soft_locking = false;
        game_state->soft_lock_counter = 0;
        assert(game_state->active_piece_row == game_state->ghost_piece_row);
        PlaySound(game_state->soft_drop_sound);
        LockPiece(game_state);
        PlaceLockedPiece(game_state);
        if (!game_state->checked_line_clears_this_frame) {
          MarkFormedWords(game_state);
          CheckForLineClears(game_state);
          game_state->checked_line_clears_this_frame = true;
        }
      }
    }

    if (!game_state->paused && !game_state->topped_out && !game_state->reached_line_cap &&
        game_state->locking_piece) {
      // printf("locking piece\n");
      UpdateLockingPiece(game_state);
      if (!game_state->locking_piece) {
        // printf("done locking piece\n");
        if (!game_state->clearing_lines) {
          SpawnNewPiece(game_state);
        }
      }
    }

    if (!game_state->paused && !game_state->topped_out && !game_state->reached_line_cap &&
        game_state->clearing_lines) {
      // printf("clearing lines\n");
      game_state->line_clear_counter++;
      // printf("line clear counter: %d\n", game_state->line_clear_counter);
      if (game_state->line_clear_counter >= LINE_CLEAR_DELAY) {
        game_state->line_clear_counter = 0;
        game_state->clearing_lines = false;
        UpdateAfterClearedLines(game_state);
        MarkFormedWords(game_state);
        SpawnNewPiece(game_state);
      }
    }

    if (!game_state->paused && !game_state->topped_out &&
        !game_state->clearing_lines && !game_state->locking_piece) {
      MaybeMovePieceLaterally(game_state);

      UpdateRotationIntent(game_state);
      MaybeRotatePiece(game_state);

      UpdateGhostPieceRow(game_state);
      MaybeApplyGravity(game_state);

      MaybeHardDrop(game_state);
    }

    // printf("ready to update playfield for display\n");
    Playfield *playfield = CreateInitialPlayfield();
    CopyPlacedSquaresToPlayfield(game_state, playfield);

    // There is no active piece while clearing lines: the locked piece is now
    // placed and there is a delay before spawning a new piece.
    if (!game_state->locking_piece && !game_state->clearing_lines &&
        !game_state->topped_out && !game_state->reached_line_cap) {
      CopyActivePieceToPlayfield(game_state, playfield);
      CopyGhostPieceToPlayfield(game_state, playfield);
    }

    DisplayPlayfield(playfield, game_state, &ui_font, &wordgame_font);
    DestroyPlayfield(playfield);

    DisplayNext(game_state, &ui_font, &wordgame_font);
    DisplayInputs(&ui_font);

    DisplayStats(game_state, &ui_font, &stats_font);

    DisplayFormedWords(game_state, &wordgame_font, &stats_font);

    char *fpsText = NULL;
    asprintf(&fpsText, "%02d", GetFPS());
    DrawText(fpsText, 10, 10, 15, BLACK);
    free(fpsText);

    EndDrawing();
  }

  CloseWindow();
  CloseAudioDevice();

  UnloadFont(ui_font);
  UnloadFont(wordgame_font);
  UnloadFont(stats_font);

  DestroyGameState(game_state);

  return 0;
}
