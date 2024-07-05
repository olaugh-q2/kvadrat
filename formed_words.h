#ifndef FORMED_WORDS_H
#define FORMED_WORDS_H

#include "raylib.h"

#include "game_state.h"

void DisplayFormedWords(const GameState *game_state, const Font *words_font,
                        const Font *score_font);

#endif // FORMED_WORDS_H