#ifndef STATS_H
#define STATS_H

#include "raylib.h"

#include "game_state.h"

void DisplayStats(const GameState *game_state, const Font *label_font,
                  const Font *stats_font);

#endif // STATS_H