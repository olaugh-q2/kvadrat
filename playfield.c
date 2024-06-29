#include "raylib.h"

void DisplayPlayfield() {
  DrawRectangle(300, 25, 200, 400, DARKGRAY);

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 20; j++) {
      DrawRectangle(301 + i * 20, 26 + j * 20, 18, 18, BLACK);
    }
  }
}