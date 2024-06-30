#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "constants.h"
#include "game_state.h"

typedef struct {
  int squares[PLAYFIELD_HEIGHT][PLAYFIELD_WIDTH];
} Playfield;

Playfield *CreateInitialPlayfield();

void DestroyPlayfield(Playfield *playfield);

void DisplayPlayfield(const Playfield *playfield);

void CopyActivePieceToPlayfield(const GameState *game_state, Playfield *playfield);

void CopyGhostPieceToPlayfield(const GameState *game_state, Playfield *playfield);

#endif // PLAYFIELD_H