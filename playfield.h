#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "raylib.h"

#include "constants.h"
#include "game_state.h"

typedef struct {
  int squares[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH];
} Playfield;

Playfield *CreateInitialPlayfield();

void DestroyPlayfield(Playfield *playfield);

void DisplayPlayfield(const Playfield *playfield, const GameState *game_state,
                      const Font *letter_font, const Font *ui_font);

void CopyPlacedSquaresToPlayfield(const GameState *game_state,
                                  Playfield *playfield);

void CopyActivePieceToPlayfield(const GameState *game_state,
                                Playfield *playfield);

void CopyGhostPieceToPlayfield(const GameState *game_state,
                               Playfield *playfield);

#endif // PLAYFIELD_H