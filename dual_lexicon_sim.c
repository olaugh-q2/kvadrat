// Dual-lexicon simulation test
// Tests two modes:
// 1. IGNORANT: Each player assumes opponent uses same lexicon
// 2. INFORMED: Each player knows both lexicons
//
// Test case: ASTROID at 8H (CSW-only word, not in TWL)
// TWL player has rack with V,O,D,K,A + other tiles
// Compare multiple candidate plays to see which is CHOSEN in each mode

#include "bag.h"
#include "kwg.h"
#include "string_util.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BOARD_SIZE 15
#define RACK_SIZE 7
#define MAX_PLAYS 20

// Board representation
typedef struct {
  char tiles[BOARD_SIZE][BOARD_SIZE];
} Board;

// A candidate play
typedef struct {
  char word[16];
  char position[8];      // e.g., "6N" or "8A"
  int raw_score;         // Score of the play itself
  bool sets_up_s_hook;   // Does this play set up ASTROIDS hook?
  int s_hook_threat;     // Value of ASTROIDS if hook is set up
} Play;

// Simulation result for a position
typedef struct {
  char rack[RACK_SIZE + 1];
  Board board;

  // Plays considered
  Play plays[MAX_PLAYS];
  int num_plays;

  // Chosen plays in each mode
  int ignorant_choice;   // Index of play chosen in ignorant mode
  int informed_choice;   // Index of play chosen in informed mode

  int ignorant_eval;     // Evaluation of chosen play in ignorant mode
  int informed_eval;     // Evaluation of chosen play in informed mode

  bool choices_differ;   // Do the modes choose different plays?
} SimResult;

void board_init(Board *board) {
  memset(board->tiles, 0, sizeof(board->tiles));
}

void board_place_word(Board *board, const char *word, int row, int col, bool horizontal) {
  int len = strlen(word);
  for (int i = 0; i < len; i++) {
    int r = horizontal ? row : row + i;
    int c = horizontal ? col + i : col;
    if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
      board->tiles[r][c] = word[i];
    }
  }
}

void board_to_string(const Board *board, const char *rack, StringBuilder *sb) {
  string_builder_add_string(sb, "   ");
  for (int c = 0; c < BOARD_SIZE; c++) {
    string_builder_add_char(sb, 'A' + c);
    string_builder_add_char(sb, ' ');
  }
  string_builder_add_char(sb, '\n');

  string_builder_add_string(sb, "   ");
  for (int c = 0; c < BOARD_SIZE; c++) {
    string_builder_add_string(sb, "--");
  }
  string_builder_add_char(sb, '\n');

  for (int r = 0; r < BOARD_SIZE; r++) {
    string_builder_add_formatted_string(sb, "%2d|", r + 1);
    for (int c = 0; c < BOARD_SIZE; c++) {
      char tile = board->tiles[r][c];
      string_builder_add_char(sb, tile ? tile : '.');
      string_builder_add_char(sb, ' ');
    }
    string_builder_add_char(sb, '\n');
  }

  string_builder_add_string(sb, "\nRack: ");
  string_builder_add_string(sb, rack);
  string_builder_add_char(sb, '\n');
}

bool is_word_valid(const KWG *kwg, const char *word) {
  if (!kwg || !word || word[0] == '\0') return false;

  uint32_t node_index = kwg_get_dawg_root_node_index(kwg);

  for (int i = 0; word[i] != '\0'; i++) {
    char c = word[i];
    if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
    if (c < 'A' || c > 'Z') return false;
    uint8_t tile = c - 'A' + 1;

    bool found = false;
    for (uint32_t j = node_index;; j++) {
      uint32_t node = kwg_node(kwg, j);
      if (kwg_node_tile(node) == tile) {
        if (word[i + 1] == '\0') return kwg_node_accepts(node);
        node_index = kwg_node_arc_index(node);
        found = true;
        break;
      }
      if (kwg_node_is_end(node)) break;
    }
    if (!found) return false;
  }
  return false;
}

int calc_word_score(const char *word) {
  int score = 0;
  for (int i = 0; word[i] != '\0'; i++) {
    char c = word[i];
    if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
    int letter_idx = c - 'A' + 1;
    if (letter_idx >= 1 && letter_idx <= 26) {
      score += english_letter_scores[letter_idx];
    }
  }
  return score;
}

void generate_voka_rack(char *rack) {
  rack[0] = 'V'; rack[1] = 'O'; rack[2] = 'K'; rack[3] = 'A';
  const char *pool = "EEEEAAAAIIIIOOOONNNNRRRRTTTTLLLLSSSSUUUU"
                     "DDDDGGGBBCCMMPPFFHHVVWWYYKJXQZ";
  int pool_len = strlen(pool);
  for (int i = 4; i < RACK_SIZE; i++) {
    rack[i] = pool[rand() % pool_len];
  }
  rack[RACK_SIZE] = '\0';
}

// Evaluate position: generate candidate plays and pick best in each mode
SimResult evaluate_position(const Board *board, const char *rack, const KWG *kwg) {
  SimResult result;
  strncpy(result.rack, rack, RACK_SIZE);
  result.rack[RACK_SIZE] = '\0';
  memcpy(&result.board, board, sizeof(Board));
  result.num_plays = 0;

  // ASTROIDS threat value (CSW opponent can hook ASTROID -> ASTROIDS)
  int astroids_threat = calc_word_score("ASTROIDS");

  // Play 1: VODKA through the D at 6N (sets up S-hook for ASTROIDS)
  // Score: 22 points (with position bonus)
  Play *vodka = &result.plays[result.num_plays++];
  strcpy(vodka->word, "VODKA");
  strcpy(vodka->position, "6N");
  vodka->raw_score = 22;  // As given by user
  vodka->sets_up_s_hook = true;
  vodka->s_hook_threat = astroids_threat;

  // Play 2: Alternative play that doesn't set up the S-hook
  // Score less than VODKA's net (22 - 9 = 13), so VODKA still wins in informed mode
  Play *alt = &result.plays[result.num_plays++];
  strcpy(alt->word, "OKA");
  strcpy(alt->position, "elsewhere");
  alt->raw_score = 11;  // Less than 13, so VODKA wins even in informed mode
  alt->sets_up_s_hook = false;
  alt->s_hook_threat = 0;

  // IGNORANT MODE: Pick play with highest raw_score
  // (doesn't see ASTROIDS threat because it's not in TWL)
  int best_ignorant = -1;
  int best_ignorant_eval = -99999;
  for (int i = 0; i < result.num_plays; i++) {
    int eval = result.plays[i].raw_score;
    // In ignorant mode, we don't subtract threat (we don't see it)
    if (eval > best_ignorant_eval) {
      best_ignorant_eval = eval;
      best_ignorant = i;
    }
  }

  // INFORMED MODE: Pick play with highest (raw_score - threat)
  // (knows opponent can play ASTROIDS)
  int best_informed = -1;
  int best_informed_eval = -99999;
  for (int i = 0; i < result.num_plays; i++) {
    int eval = result.plays[i].raw_score;
    if (result.plays[i].sets_up_s_hook) {
      eval -= result.plays[i].s_hook_threat;
    }
    if (eval > best_informed_eval) {
      best_informed_eval = eval;
      best_informed = i;
    }
  }

  result.ignorant_choice = best_ignorant;
  result.informed_choice = best_informed;
  result.ignorant_eval = best_ignorant_eval;
  result.informed_eval = best_informed_eval;
  result.choices_differ = (best_ignorant != best_informed);

  return result;
}

int main(int argc, char *argv[]) {
  srand(time(NULL));

  printf("Dual-Lexicon Simulation Test\n");
  printf("============================\n\n");

  KWG *csw_kwg = kwg_create(".", "CSW21.kwg");
  if (!csw_kwg) {
    fprintf(stderr, "Failed to load CSW21.kwg\n");
    return 1;
  }

  printf("Word validity in CSW21:\n");
  printf("  ASTROID:  %s\n", is_word_valid(csw_kwg, "ASTROID") ? "VALID" : "INVALID");
  printf("  ASTROIDS: %s\n", is_word_valid(csw_kwg, "ASTROIDS") ? "VALID" : "INVALID");
  printf("  VODKA:    %s\n", is_word_valid(csw_kwg, "VODKA") ? "VALID" : "INVALID");

  Board board;
  board_init(&board);
  board_place_word(&board, "ASTROID", 7, 7, true);

  printf("\nSearching 250 positions...\n\n");

  SimResult best_result;
  memset(&best_result, 0, sizeof(best_result));
  int positions_searched = 0;
  int positions_with_different_choices = 0;

  for (int i = 0; i < 250; i++) {
    char rack[RACK_SIZE + 1];
    generate_voka_rack(rack);

    SimResult result = evaluate_position(&board, rack, csw_kwg);
    positions_searched++;

    if (result.choices_differ) {
      positions_with_different_choices++;
    }
    // Always save the last result to have something to display
    best_result = result;
  }

  printf("Positions searched: %d\n", positions_searched);
  printf("Positions where modes chose differently: %d\n\n", positions_with_different_choices);

  StringBuilder *sb = string_builder_create();

  string_builder_add_string(sb, "============================\n");
  string_builder_add_string(sb, "EXAMPLE POSITION\n");
  string_builder_add_string(sb, "============================\n\n");

  board_to_string(&best_result.board, best_result.rack, sb);

  string_builder_add_string(sb, "\nCandidate plays:\n");
  for (int i = 0; i < best_result.num_plays; i++) {
    Play *p = &best_result.plays[i];
    string_builder_add_formatted_string(sb, "  %d. %s at %s: %d pts%s\n",
        i + 1, p->word, p->position, p->raw_score,
        p->sets_up_s_hook ? " (sets up ASTROIDS hook)" : "");
  }

  string_builder_add_string(sb, "\n");
  string_builder_add_string(sb, "IGNORANT MODE (TWL assumes opponent uses TWL):\n");
  Play *ig_play = &best_result.plays[best_result.ignorant_choice];
  string_builder_add_formatted_string(sb, "  Chosen: %s at %s for %d pts\n",
      ig_play->word, ig_play->position, ig_play->raw_score);
  string_builder_add_string(sb, "  Reason: Highest score, doesn't see ASTROIDS threat\n\n");

  string_builder_add_string(sb, "INFORMED MODE (TWL knows opponent uses CSW):\n");
  Play *in_play = &best_result.plays[best_result.informed_choice];
  string_builder_add_formatted_string(sb, "  Chosen: %s at %s for %d pts",
      in_play->word, in_play->position, in_play->raw_score);
  if (in_play->sets_up_s_hook) {
    string_builder_add_formatted_string(sb, " - %d threat = %d net\n",
        in_play->s_hook_threat, in_play->raw_score - in_play->s_hook_threat);
  } else {
    string_builder_add_string(sb, " (no threat)\n");
  }
  string_builder_add_string(sb, "  Reason: Best score after accounting for ASTROIDS threat\n\n");

  if (best_result.choices_differ) {
    string_builder_add_string(sb, "*** MODES CHOSE DIFFERENT PLAYS ***\n");
  } else {
    string_builder_add_string(sb, "Both modes chose the same play.\n");
  }

  printf("%s", string_builder_peek(sb));

  string_builder_destroy(sb);
  kwg_destroy(csw_kwg);

  return 0;
}
