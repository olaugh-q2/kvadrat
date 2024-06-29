#include "raylib.h"

void DisplayNext(const Font *font) {
  DrawTextEx(*font, "NEXT", (Vector2){510, 22}, font->baseSize * 0.5, 1.0,
             BLACK);
  DrawRectangle(505, 40, 90, 200, BLACK);
}