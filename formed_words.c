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
  for (int game_word_index = 0; game_word_index < game_state->num_words_formed;
       game_word_index++) {
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
    const int word_display_time =
        game_state->unpaused_frame_counter - word_frame;
    if (word_display_time >= WORD_OPAQUE_TIME) {
      int fade_frame = word_display_time - WORD_OPAQUE_TIME;
      word_alpha = 1.0 - (fade_frame / (float)WORD_FADE_TIME);
    }
    int word_index = game_state->words_formed_indices[game_word_index];
    // Color base_word_color = RED;
    Color base_word_color = FormedWordsGetColorForScore(
        game_state->words_formed_scores[game_word_index]);
    Color word_color =
        ColorAlphaBlend(base_word_color, Fade(BLACK, 0.5), Fade(BLACK, 0.5));
    const int word_length = game_state->words_formed_lengths[game_word_index];
    char letter_text[] = "A";
    const int max_word_display_width = 195;
    const int letter_display_size = word_length == 10 ? 18 : 20;
    const int text_height = letter_display_size - 8;
    DrawRectangle(600, 22 + word_index * 50, word_length * letter_display_size,
                  letter_display_size, Fade(WHITE, word_alpha));
    for (int i = 0; i < word_length; i++) {
      letter_text[0] = word[i];
      Vector2 text_size =
          MeasureTextEx(*words_font, letter_text, text_height, 1);
      DrawTextEx(*words_font, letter_text,
                 (Vector2){600 + i * letter_display_size +
                               letter_display_size * 0.5 - text_size.x / 2,
                           22 + word_index * 50 + letter_display_size * 0.5 -
                               text_size.y / 2},
                 text_height, 1.0, Fade(word_color, word_alpha));
    }
    DrawRectangleRoundedLines((Rectangle){600, 22 + word_index * 50,
                                          word_length * letter_display_size,
                                          letter_display_size},
                              0.5, 4, 2.5, Fade(word_color, word_alpha));
    char *raw_score_text;
    const int raw_score = game_state->words_formed_scores[game_word_index] /
                          (word_length * word_length);
    asprintf(&raw_score_text, "%d ", raw_score);
    Vector2 raw_score_size =
        MeasureTextEx(*score_font, raw_score_text, 16, 1.0);
    char x_text[] = "x";
    DrawTextEx(*score_font, raw_score_text,
               (Vector2){600, 22 + word_index * 50 + 27}, 16, 1.0,
               Fade(BLACK, word_alpha));
    DrawTextEx(*score_font, x_text,
               (Vector2){600 + raw_score_size.x, 22 + word_index * 50 + 28}, 12,
               1.0, Fade(BLACK, word_alpha));
    char *length_text;
    asprintf(&length_text, " %d", word_length);
    Vector2 length_text_size = MeasureTextEx(*score_font, length_text, 16, 1.0);
    DrawTextEx(
        *score_font, length_text,
        (Vector2){600 + raw_score_size.x + 5, 22 + word_index * 50 + 27}, 16,
        1.0, Fade(BLACK, word_alpha));
    char score_expression_exponent[] = "2";
    char *score_expression_right;
    asprintf(&score_expression_right, " = %d",
             game_state->words_formed_scores[game_word_index]);
    Vector2 score_expression_right_size =
        MeasureTextEx(*score_font, score_expression_right, 16, 1.0);
    DrawTextEx(*score_font, score_expression_exponent,
               (Vector2){600 + raw_score_size.x + 6 + length_text_size.x,
                         22 + word_index * 50 + 26},
               9, 1.0, Fade(BLACK, word_alpha));
    DrawTextEx(*score_font, score_expression_right,
               (Vector2){600 + raw_score_size.x + length_text_size.x + 11,
                         22 + word_index * 50 + 27},
               16, 1.0, Fade(BLACK, word_alpha));
    free(raw_score_text);
    free(length_text);
    free(score_expression_right);
  }
}