#include "raylib.h"

#include "inputs.h"

void DisplayInputs(const Font *font) {
  DrawTextEx(*font, "INPUTS", (Vector2){180, 22}, 16, 1.0, BLACK);
  DrawRectangle(175, 40, 120, 45, BLACK);

  // Draw outline of NES D-pad
  DrawLine(190, 50, 200, 50, GRAY);
  DrawLine(200, 50, 200, 60, GRAY);
  DrawLine(200, 60, 210, 60, GRAY);
  DrawLine(210, 60, 210, 70, GRAY);
  DrawLine(210, 70, 200, 70, GRAY);
  DrawLine(200, 70, 200, 80, GRAY);
  DrawLine(200, 80, 190, 80, GRAY);
  DrawLine(190, 80, 190, 70, GRAY);
  DrawLine(190, 70, 180, 70, GRAY);
  DrawLine(180, 70, 180, 60, GRAY);
  DrawLine(180, 60, 190, 60, GRAY);
  DrawLine(190, 60, 190, 50, GRAY);

  // Select button
  DrawLine(220, 72, 228, 72, GRAY);
  DrawLine(220, 76, 228, 76, GRAY);
  DrawLine(220, 72, 218, 74, GRAY);
  DrawLine(218, 74, 220, 76, GRAY);
  DrawLine(228, 72, 230, 74, GRAY);
  DrawLine(230, 74, 228, 76, GRAY);

  // Start button
  DrawLine(236, 72, 244, 72, GRAY);
  DrawLine(236, 76, 244, 76, GRAY);
  DrawLine(236, 72, 234, 74, GRAY);
  DrawLine(234, 74, 236, 76, GRAY);
  DrawLine(244, 72, 246, 74, GRAY);
  DrawLine(246, 74, 244, 76, GRAY);

  // B button
  DrawCircle(262, 74, 6, GRAY);

  // A button
  DrawCircle(278, 74, 6, GRAY);

  if (IsGamepadAvailable(0)) {
    IsGamepadButtonDown(0, GAMEPAD_BUTTON_UNKNOWN);
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
      DrawRectangle(179, 59, 11, 12, RED);
    }
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
      DrawRectangle(189, 49, 12, 11, RED);
    }
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
      DrawRectangle(189, 70, 12, 11, RED);
    }
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
      DrawRectangle(200, 59, 11, 12, RED);
    }
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_LEFT)) {
      DrawRectangle(220, 71, 8, 6, RED);
      DrawTriangle((Vector2){220, 71}, (Vector2){217, 74}, (Vector2){220, 77},
                   RED);
      DrawTriangle((Vector2){228, 71}, (Vector2){228, 77}, (Vector2){231, 74},
                   RED);
    }
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) {
      DrawRectangle(236, 71, 8, 6, RED);
      DrawTriangle((Vector2){236, 71}, (Vector2){233, 74}, (Vector2){236, 77},
                   RED);
      DrawTriangle((Vector2){244, 71}, (Vector2){244, 77}, (Vector2){247, 74},
                   RED);
    }

    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
      DrawCircle(262, 74, 7, RED);
    }
    if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) {
      DrawCircle(278, 74, 7, RED);
    }
  }
}