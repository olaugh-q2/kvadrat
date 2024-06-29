#include "raylib.h"

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

  while (!WindowShouldClose()) {
    BeginDrawing();

    ClearBackground(GRAY);

    DisplayPlayfield();
    DisplayNext(&font);
    DisplayInputs(&font);

    char *fpsText = NULL;
    asprintf(&fpsText, "%02d", GetFPS());
    DrawText(fpsText, 10, 10, 15, BLACK);
    free(fpsText);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
