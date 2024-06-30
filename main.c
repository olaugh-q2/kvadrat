#include "raylib.h"

#include "game_state.h"
#include "inputs.h"
#include "next.h"
#include "playfield.h"

#include <stdio.h>
#include <stdlib.h>

int main(void) {
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "kvadrat");

  Font font = LoadFont("Futura-Medium-01.ttf");
  SetTargetFPS(60);
  GameState *game_state = CreateInitialGameState();


  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(GRAY);

    UpdateLateralMovementIntent(game_state);
    MaybeMovePieceLaterally(game_state);

    UpdateRotationIntent(game_state);
    MaybeRotatePiece(game_state);

    MaybeApplyGravity(game_state);

    UpdateGhostPieceRow(game_state);
    MaybeHardDrop(game_state);

    Playfield *playfield = CreateInitialPlayfield();
    CopyActivePieceToPlayfield(game_state, playfield);
    CopyGhostPieceToPlayfield(game_state, playfield);
    DisplayPlayfield(playfield);
    DestroyPlayfield(playfield);

    DisplayNext(&font);
    DisplayInputs(&font);

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
