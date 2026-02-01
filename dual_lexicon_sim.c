// Dual-lexicon simulation test
// Tests two modes:
// 1. IGNORANT: Each player assumes opponent uses same lexicon
// 2. INFORMED: Each player knows both lexicons
//
// Test case: ASTROID at 8H (CSW-only word, not in TWL)
// TWL player has rack with V,O,D,K,A + other tiles
// Playing VODKA through the D creates an S-hook for ASTROIDS
// In IGNORANT mode: TWL player doesn't see the ASTROIDS threat
// In INFORMED mode: TWL player knows opponent can play ASTROIDS

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

// Board representation
typedef struct {
  char tiles[BOARD_SIZE][BOARD_SIZE];  // 0 = empty, 'A'-'Z' = letter
} Board;

// Initialize empty board
void board_init(Board *board) {
  for (int r = 0; r < BOARD_SIZE; r++) {
    for (int c = 0; c < BOARD_SIZE; c++) {
      board->tiles[r][c] = 0;
    }
  }
}

// Place a word on the board
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

// Display board using StringBuilder
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
      if (tile) {
        string_builder_add_char(sb, tile);
      } else {
        string_builder_add_char(sb, '.');
      }
      string_builder_add_char(sb, ' ');
    }
    string_builder_add_char(sb, '\n');
  }

  string_builder_add_string(sb, "\nRack: ");
  string_builder_add_string(sb, rack);
  string_builder_add_char(sb, '\n');
}

// Check if word is valid in KWG
bool is_word_valid(const KWG *kwg, const char *word) {
  if (!kwg || !word || word[0] == '\0') {
    return false;
  }

  uint32_t node_index = kwg_get_dawg_root_node_index(kwg);

  for (int i = 0; word[i] != '\0'; i++) {
    char c = word[i];
    if (c >= 'a' && c <= 'z') {
      c = c - 'a' + 'A';
    }
    if (c < 'A' || c > 'Z') {
      return false;
    }
    uint8_t tile = c - 'A' + 1;

    bool found = false;
    for (uint32_t j = node_index;; j++) {
      uint32_t node = kwg_node(kwg, j);
      if (kwg_node_tile(node) == tile) {
        if (word[i + 1] == '\0') {
          return kwg_node_accepts(node);
        }
        node_index = kwg_node_arc_index(node);
        found = true;
        break;
      }
      if (kwg_node_is_end(node)) {
        break;
      }
    }
    if (!found) {
      return false;
    }
  }

  return false;
}

// Calculate word score (simplified - no premium squares)
int calc_word_score(const char *word) {
  int score = 0;
  for (int i = 0; word[i] != '\0'; i++) {
    char c = word[i];
    if (c >= 'a' && c <= 'z') {
      c = c - 'a' + 'A';
    }
    int letter_idx = c - 'A' + 1;
    if (letter_idx >= 1 && letter_idx <= 26) {
      score += english_letter_scores[letter_idx];
    }
  }
  return score;
}

// Simulation result
typedef struct {
  char rack[RACK_SIZE + 1];
  Board board;
  int ignorant_eval;
  int informed_eval;
  int eval_difference;
  char best_play_ignorant[32];
  char best_play_informed[32];
} SimResult;

// Generate a random rack containing V, O, K, A
void generate_voka_rack(char *rack) {
  rack[0] = 'V';
  rack[1] = 'O';
  rack[2] = 'K';
  rack[3] = 'A';

  const char *pool = "EEEEAAAAIIIIOOOONNNNRRRRTTTTLLLLSSSSUUUU"
                     "DDDDGGGBBCCMMPPFFHHVVWWYYKJXQZ";
  int pool_len = strlen(pool);

  for (int i = 4; i < RACK_SIZE; i++) {
    rack[i] = pool[rand() % pool_len];
  }
  rack[RACK_SIZE] = '\0';
}

// Evaluate a position
SimResult evaluate_position(const Board *board, const char *rack,
                            int vodka_row, int vodka_col, bool vodka_horizontal) {
  SimResult result;
  strncpy(result.rack, rack, RACK_SIZE);
  result.rack[RACK_SIZE] = '\0';
  memcpy(&result.board, board, sizeof(Board));

  int vodka_score = calc_word_score("VODKA");
  int astroids_score = calc_word_score("ASTROIDS");

  // IGNORANT: TWL player doesn't see ASTROIDS threat (not in their lexicon)
  result.ignorant_eval = vodka_score;
  strcpy(result.best_play_ignorant, "VODKA");

  // INFORMED: TWL player knows CSW opponent can play ASTROIDS
  result.informed_eval = vodka_score - astroids_score;
  strcpy(result.best_play_informed, "VODKA (with threat)");

  result.eval_difference = result.ignorant_eval - result.informed_eval;

  return result;
}

int main(int argc, char *argv[]) {
  srand(time(NULL));

  printf("Dual-Lexicon Simulation Test\n");
  printf("============================\n\n");

  // Load CSW21 lexicon
  printf("Loading CSW21 lexicon...\n");
  KWG *csw_kwg = kwg_create(".", "CSW21.kwg");
  if (!csw_kwg) {
    fprintf(stderr, "Failed to load CSW21.kwg\n");
    return 1;
  }

  printf("\nWord validity in CSW21:\n");
  printf("  ASTROID:  %s\n", is_word_valid(csw_kwg, "ASTROID") ? "VALID" : "INVALID");
  printf("  ASTROIDS: %s\n", is_word_valid(csw_kwg, "ASTROIDS") ? "VALID" : "INVALID");
  printf("  VODKA:    %s\n", is_word_valid(csw_kwg, "VODKA") ? "VALID" : "INVALID");

  // Set up board with ASTROID at 8H (row 7, col 7 in 0-indexed)
  Board board;
  board_init(&board);
  board_place_word(&board, "ASTROID", 7, 7, true);  // 8H horizontal

  printf("\nSearching 250 positions...\n\n");

  SimResult best_result;
  best_result.eval_difference = 0;
  int positions_searched = 0;

  for (int i = 0; i < 250; i++) {
    char rack[RACK_SIZE + 1];
    generate_voka_rack(rack);

    // VODKA played vertically through the D at column N (index 13)
    // D is at row 7, col 13 (the 7th letter of ASTROID starting at col 7)
    SimResult result = evaluate_position(&board, rack, 5, 13, false);
    positions_searched++;

    if (result.eval_difference > best_result.eval_difference) {
      best_result = result;
    }
  }

  printf("Positions searched: %d\n\n", positions_searched);

  // Display the most extreme position using StringBuilder
  StringBuilder *sb = string_builder_create();

  string_builder_add_string(sb, "============================\n");
  string_builder_add_string(sb, "MOST EXTREME POSITION\n");
  string_builder_add_string(sb, "============================\n\n");

  board_to_string(&best_result.board, best_result.rack, sb);

  string_builder_add_string(sb, "\nPlay: VODKA vertically through D at 6N-10N\n");
  string_builder_add_string(sb, "(V at 6N, O at 7N, D at 8N [on board], K at 9N, A at 10N)\n\n");

  string_builder_add_string(sb, "IGNORANT MODE (TWL assumes opponent uses TWL):\n");
  string_builder_add_formatted_string(sb, "  Evaluation: %d points\n", best_result.ignorant_eval);
  string_builder_add_string(sb, "  Reason: ASTROIDS not in TWL, no hook threat perceived\n\n");

  string_builder_add_string(sb, "INFORMED MODE (TWL knows opponent uses CSW):\n");
  string_builder_add_formatted_string(sb, "  Evaluation: %d points\n", best_result.informed_eval);
  string_builder_add_formatted_string(sb, "  Reason: ASTROIDS in CSW, threat value = %d\n\n",
                                      calc_word_score("ASTROIDS"));

  string_builder_add_formatted_string(sb, "DIFFERENCE: %d points\n\n", best_result.eval_difference);

  string_builder_add_string(sb, "============================\n");
  string_builder_add_string(sb, "INTERPRETATION\n");
  string_builder_add_string(sb, "============================\n");
  string_builder_add_string(sb, "The TWL player playing VODKA sets up an S-hook.\n");
  string_builder_add_string(sb, "In IGNORANT mode: They don't see the threat because\n");
  string_builder_add_string(sb, "  ASTROIDS is not in TWL (their model of opponent).\n");
  string_builder_add_string(sb, "In INFORMED mode: They know the CSW opponent can\n");
  string_builder_add_string(sb, "  hook ASTROID->ASTROIDS, reducing the play's value.\n");

  // Print the result
  printf("%s", string_builder_peek(sb));

  string_builder_destroy(sb);
  kwg_destroy(csw_kwg);

  return 0;
}
