#include "raylib.h"

#include "constants.h"
#include "game_state.h"
#include "stats.h"

void DisplayStats(const GameState *game_state, const Font *label_font,
                  const Font *stats_font) {
  DrawTextEx(*label_font, "PIECES", (Vector2){220, 100}, 16, 1.0, BLACK);

  char *pieces_per_second_text = NULL;
  if (game_state->unpaused_frame_counter == 0) {
    pieces_per_second_text = strdup("-.--/S");
  } else {
    asprintf(&pieces_per_second_text, "%.2f/S",
             game_state->num_pieces /
                 (game_state->unpaused_frame_counter / 60.0));
  }
  Vector2 pieces_per_second_text_size =
      MeasureTextEx(*stats_font, pieces_per_second_text, 16, 1);
  char *pieces_text = NULL;
  asprintf(&pieces_text, "%d,", game_state->num_pieces);
  Vector2 pieces_text_size = MeasureTextEx(*stats_font, pieces_text, 24, 1);
  const int pieces_size_y_diff =
      pieces_text_size.y - pieces_per_second_text_size.y;
  DrawTextEx(*stats_font, pieces_per_second_text,
             (Vector2){295 - pieces_per_second_text_size.x,
                       120 + pieces_size_y_diff - 2},
             16, 1.0, BLACK);
  DrawTextEx(
      *stats_font, pieces_text,
      (Vector2){295 - pieces_text_size.x - pieces_per_second_text_size.x + 6,
                120},
      24, 1.0, BLACK);
  free(pieces_per_second_text);
  free(pieces_text);

  DrawTextEx(*label_font, "LINES", (Vector2){220, 150}, 16, 1.0, BLACK);
  char *max_lines_text = NULL;
  asprintf(&max_lines_text, "/%d", MAX_LINES);
  Vector2 max_lines_text_size =
      MeasureTextEx(*stats_font, max_lines_text, 14, 1);
  char *lines_text = NULL;
  asprintf(&lines_text, "%d", game_state->num_lines);
  Vector2 lines_text_size = MeasureTextEx(*stats_font, lines_text, 24, 1);
  const int lines_size_y_diff = lines_text_size.y - max_lines_text_size.y;
  DrawTextEx(
      *stats_font, max_lines_text,
      (Vector2){295 - max_lines_text_size.x, 170 + lines_size_y_diff - 2}, 14,
      1.0, BLACK);
  DrawTextEx(
      *stats_font, lines_text,
      (Vector2){295 - lines_text_size.x - max_lines_text_size.x - 1, 170}, 24,
      1.0, BLACK);
  free(max_lines_text);
  free(lines_text);

  DrawTextEx(*label_font, "WORDS", (Vector2){220, 200}, 16, 1.0, BLACK);
  char *words_avg_len_text = NULL;
  if (game_state->num_words == 0) {
    words_avg_len_text = strdup("-.--LPW");
  } else {
    asprintf(&words_avg_len_text, "%.2fLPW",
             game_state->sum_of_word_lengths / (float)game_state->num_words);
  }
  Vector2 words_avg_len_text_size =
      MeasureTextEx(*stats_font, words_avg_len_text, 16, 1);
  char *words_text = NULL;
  asprintf(&words_text, "%d,", game_state->num_words);
  Vector2 words_text_size = MeasureTextEx(*stats_font, words_text, 24, 1);
  const int words_size_y_diff = words_text_size.y - words_avg_len_text_size.y;
  DrawTextEx(
      *stats_font, words_avg_len_text,
      (Vector2){295 - words_avg_len_text_size.x, 220 + words_size_y_diff - 2},
      16, 1.0, BLACK);
  DrawTextEx(
      *stats_font, words_text,
      (Vector2){295 - words_text_size.x - words_avg_len_text_size.x + 6, 220},
      24, 1.0, BLACK);
  free(words_avg_len_text);
  free(words_text);

  DrawTextEx(*label_font, "SCORE", (Vector2){220, 250}, 16, 1.0, BLACK);
  char *score_per_lines_text = NULL;
  if (game_state->num_lines == 0) {
    score_per_lines_text = strdup("--/L");
  } else {
    asprintf(&score_per_lines_text, "%d/L",
             (int)(game_state->total_score / (float)game_state->num_lines));
  }
  Vector2 score_per_lines_text_size =
      MeasureTextEx(*stats_font, score_per_lines_text, 16, 1);
  char *score_text = NULL;
  asprintf(&score_text, "%d,", game_state->total_score);
  Vector2 score_text_size = MeasureTextEx(*stats_font, score_text, 24, 1);
  const int score_size_y_diff = score_text_size.y - score_per_lines_text_size.y;
  DrawTextEx(
      *stats_font, score_per_lines_text,
      (Vector2){295 - score_per_lines_text_size.x, 270 + score_size_y_diff - 2},
      16, 1.0, BLACK);
  DrawTextEx(
      *stats_font, score_text,
      (Vector2){295 - score_text_size.x - score_per_lines_text_size.x + 6, 270},
      24, 1.0, BLACK);
  free(score_text);

  DrawTextEx(*label_font, "TIME", (Vector2){220, 300}, 16, 1.0, BLACK);
  int time_total_thousandths = 1000 * game_state->unpaused_frame_counter / 60.0;
  if (!game_state->paused) {
    time_total_thousandths += rand() % 3;
  }
  const int time_thousandths_part = time_total_thousandths % 1000;
  const int time_total_seconds = time_total_thousandths / 1000;
  const int time_seconds = time_total_seconds % 60;
  const int time_minutes = time_total_seconds / 60;

  char *time_thousandths_text = NULL;
  asprintf(&time_thousandths_text, ".%03d", time_thousandths_part);
  Vector2 time_thousandths_text_size =
      MeasureTextEx(*stats_font, time_thousandths_text, 16, 1);
  char *time_minutes_and_seconds_text = NULL;
  asprintf(&time_minutes_and_seconds_text, "%d:%02d", time_minutes,
           time_seconds);
  Vector2 time_minutes_and_seconds_text_size =
      MeasureTextEx(*stats_font, time_minutes_and_seconds_text, 24, 1);
  const int time_size_y_diff =
      time_minutes_and_seconds_text_size.y - time_thousandths_text_size.y;

  DrawTextEx(
      *stats_font, time_thousandths_text,
      (Vector2){295 - time_thousandths_text_size.x, 320 + time_size_y_diff - 2},
      16, 1.0, BLACK);
  DrawTextEx(*stats_font, time_minutes_and_seconds_text,
             (Vector2){295 - time_minutes_and_seconds_text_size.x -
                           time_thousandths_text_size.x,
                       320},
             24, 1.0, BLACK);
  printf("time_minutes_and_seconds_text: %s\n", time_minutes_and_seconds_text);
  printf("time_thousandths_text: %s\n", time_thousandths_text);
  printf("time_total_thousandths: %d\n", time_total_thousandths);
  printf("time_total_seconds: %d\n", time_total_seconds);
  printf("time_seconds: %d\n", time_seconds);
  printf("time_minutes: %d\n", time_minutes);
  free(time_thousandths_text);
  free(time_minutes_and_seconds_text);
}