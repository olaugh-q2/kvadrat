#include "raylib.h"

#include "constants.h"
#include "formed_words.h"
#include "game_state.h"

Color FormedWordsGetColorForScore(int word_score) {
  if (word_score < 100) {
    return Fade(DARKPURPLE, 1.0f);
  }
  if (word_score <= 400) {
    return Fade(DARKBLUE, 1.0f);
  }
  if (word_score <= 1000) {
    return Fade(DARKGREEN, 1.0f);
  }
  return Fade(RED, 1.0f);
}

void DisplayFormedWords(const GameState *game_state, const Font *words_font,
                        const Font *score_font) {
  const int last_frame_needing_display =
      game_state->unpaused_frame_counter - WORD_DISPLAY_TIME;
  int total_height = 0;
  for (int game_word_index = 0; game_word_index < game_state->num_words_formed; game_word_index++) {
    const int word_frame = game_state->words_formed_at_frame[game_word_index];
    if (word_frame < last_frame_needing_display) {
      continue;
    }
    char word[PLAYFIELD_WIDTH + 1];
    const int formed_word_offset = game_word_index * (PLAYFIELD_WIDTH + 1);
    for (int col = 0; col < PLAYFIELD_WIDTH; col++) {
      word[col] = game_state->words_formed[formed_word_offset + col];
    }
    float word_alpha = 1.0;
    const int word_display_time = game_state->unpaused_frame_counter - word_frame;
    if (word_display_time >= WORD_OPAQUE_TIME) {
      int fade_frame = word_display_time - WORD_OPAQUE_TIME;
      word_alpha = 1.0 - (fade_frame / (float)WORD_FADE_TIME);
    }
    int word_index = game_state->words_formed_indices[game_word_index];
    //Color base_word_color = RED;
    Color base_word_color = FormedWordsGetColorForScore(game_state->words_formed_scores[game_word_index]);
    Color word_color = ColorAlphaBlend(base_word_color, Fade(BLACK, 0.5), Fade(BLACK, 0.5));
    DrawTextEx(*words_font, word, (Vector2){600, 22 + word_index * 64}, 32, 1.0,
               Fade(word_color, word_alpha));

  }
}