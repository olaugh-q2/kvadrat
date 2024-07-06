#ifndef STATS_H
#define STATS_H

#include "raylib.h"

#include "game_state.h"
#include "session_state.h"

void DisplayStats(const GameState *game_state,
                  const SessionState *session_state, const Font *label_font,
                  const Font *stats_font);

#endif // STATS_H