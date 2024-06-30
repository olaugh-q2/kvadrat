#include "constants.h"
#include "raylib.h"

#include "game_state.h"
#include "inputs.h"
#include "next.h"
#include "playfield.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "kvadrat");

  Font ui_font = LoadFont("Futura-Medium-01.ttf");
  Font wordgame_font = LoadFont("FranklinGothic.ttf");

  SetTargetFPS(60);
  GameState *game_state = CreateInitialGameState();

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(GRAY);

    UpdateLateralMovementIntent(game_state);

    if (game_state->soft_locking) {
      printf("soft locking\n");
      game_state->soft_lock_counter++;
      if (game_state->soft_lock_counter >= SOFT_LOCK_DELAY) {
        game_state->soft_locking = false;
        game_state->soft_lock_counter = 0;
        assert(game_state->active_piece_row == game_state->ghost_piece_row);
        LockPiece(game_state);
        PlaceLockedPiece(game_state);
        CheckForLineClears(game_state);
      }
    }

    if (game_state->locking_piece) {
      printf("locking piece\n");
      UpdateLockingPiece(game_state);
      if (!game_state->locking_piece) {
        printf("done locking piece\n");
        if (!game_state->clearing_lines) {
          SpawnNewPiece(game_state);
        }
      }
    }

    if (game_state->clearing_lines) {
      printf("clearing lines\n");
      game_state->line_clear_counter++;
      printf("line clear counter: %d\n", game_state->line_clear_counter);
      if (game_state->line_clear_counter >= LINE_CLEAR_DELAY) {
        game_state->line_clear_counter = 0;
        game_state->clearing_lines = false;
        UpdateAfterClearedLines(game_state);
        SpawnNewPiece(game_state);
      }
    }

    if (!game_state->clearing_lines && !game_state->locking_piece) {
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
    if (!game_state->locking_piece && !game_state->clearing_lines) {
      CopyActivePieceToPlayfield(game_state, playfield);
      CopyGhostPieceToPlayfield(game_state, playfield);
    }

    DisplayPlayfield(playfield, game_state, &wordgame_font);
    DestroyPlayfield(playfield);

    DisplayNext(game_state, &ui_font);
    DisplayInputs(&ui_font);

    char *fpsText = NULL;
    asprintf(&fpsText, "%02d", GetFPS());
    DrawText(fpsText, 10, 10, 15, BLACK);
    free(fpsText);

    EndDrawing();
  }

  CloseWindow();

  DestroyGameState(game_state);

  return 0;
}
