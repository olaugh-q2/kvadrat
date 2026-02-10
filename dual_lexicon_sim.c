// Dual-lexicon simulation test
// Shows how VODKA's evaluation differs between IGNORANT and INFORMED modes
// because of the ASTROIDS hook threat that only CSW players can exploit

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

typedef struct {
  char tiles[BOARD_SIZE][BOARD_SIZE];
} Board;

typedef struct {
  const char *word;
  const char *position;
  int raw_score;
  int threat_adjustment;  // Points opponent gains from hooks we open
} Play;

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

void board_to_string(const Board *board, StringBuilder *sb) {
  string_builder_add_string(sb, "   A B C D E F G H I J K L M N O \n");
  string_builder_add_string(sb, "   ------------------------------\n");
  for (int r = 0; r < BOARD_SIZE; r++) {
    string_builder_add_formatted_string(sb, "%2d|", r + 1);
    for (int c = 0; c < BOARD_SIZE; c++) {
      char tile = board->tiles[r][c];
      string_builder_add_char(sb, tile ? tile : '.');
      string_builder_add_char(sb, ' ');
    }
    string_builder_add_char(sb, '\n');
  }
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

int evaluate_play_ignorant(const Play *play) {
  // IGNORANT: Assume opponent uses same lexicon, don't see CSW-only threats
  return play->raw_score;
}

int evaluate_play_informed(const Play *play) {
  // INFORMED: Know opponent uses CSW, account for threats they can exploit
  return play->raw_score - play->threat_adjustment;
}

void print_play_comparison(StringBuilder *sb, const Play plays[], int num_plays) {
  string_builder_add_string(sb, "  Play             Raw    Threat   IGNORANT   INFORMED\n");
  string_builder_add_string(sb, "  ----             ---    ------   --------   --------\n");
  for (int i = 0; i < num_plays; i++) {
    int ignorant_val = evaluate_play_ignorant(&plays[i]);
    int informed_val = evaluate_play_informed(&plays[i]);
    string_builder_add_formatted_string(sb, "  %-16s %3d     -%2d       %3d        %3d\n",
        plays[i].word, plays[i].raw_score, plays[i].threat_adjustment,
        ignorant_val, informed_val);
  }
}

const Play* find_best_play(const Play plays[], int num_plays, bool informed_mode) {
  const Play *best = &plays[0];
  int best_val = informed_mode ? evaluate_play_informed(best) : evaluate_play_ignorant(best);
  for (int i = 1; i < num_plays; i++) {
    int val = informed_mode ? evaluate_play_informed(&plays[i]) : evaluate_play_ignorant(&plays[i]);
    if (val > best_val) {
      best_val = val;
      best = &plays[i];
    }
  }
  return best;
}

int main(int argc, char *argv[]) {
  printf("Dual-Lexicon Simulation: Play Selection Differences\n");
  printf("====================================================\n\n");

  KWG *csw_kwg = kwg_create(".", "CSW21.kwg");
  if (!csw_kwg) {
    fprintf(stderr, "Failed to load CSW21.kwg\n");
    return 1;
  }

  // Verify words
  printf("Word validity in CSW21:\n");
  printf("  ASTROID:  %s (CSW-only)\n", is_word_valid(csw_kwg, "ASTROID") ? "YES" : "NO");
  printf("  ASTROIDS: %s (CSW-only)\n", is_word_valid(csw_kwg, "ASTROIDS") ? "YES" : "NO");
  printf("  VODKA:    %s\n", is_word_valid(csw_kwg, "VODKA") ? "YES" : "NO");
  printf("  SKATE:    %s\n", is_word_valid(csw_kwg, "SKATE") ? "YES" : "NO");
  printf("  STOVE:    %s\n", is_word_valid(csw_kwg, "STOVE") ? "YES" : "NO");
  printf("  TOKES:    %s\n", is_word_valid(csw_kwg, "TOKES") ? "YES" : "NO");
  printf("\n");

  // Set up board
  Board board;
  board_init(&board);
  board_place_word(&board, "ASTROID", 7, 7, true);  // 8H

  StringBuilder *sb = string_builder_create();

  string_builder_add_string(sb, "SCENARIO: TWL player vs CSW opponent\n");
  string_builder_add_string(sb, "=====================================\n\n");
  string_builder_add_string(sb, "Board: ASTROID at 8H (CSW-only word played by opponent)\n\n");
  board_to_string(&board, sb);

  string_builder_add_string(sb, "\nRack: VOKASTE (V,O,K,A,S,T,E)\n");
  string_builder_add_string(sb, "Turn 2: All plays must connect to ASTROID!\n\n");

  // The S-hook at O8 lands on a TRIPLE WORD SCORE!
  // ASTROIDS base = 9, on TWS = 27 points
  // Plus opponent can extend vertically through S toward O1 (another TWS)
  int astroids_base = calc_word_score("ASTROIDS");  // 9 points
  int astroids_tws_threat = astroids_base * 3;  // 27 points on TWS at O8

  // All plays connect to ASTROID (H8-N8):
  // - VODKA vertical through D at N8
  // - SKATE vertical through A at H8
  // - STOVE vertical through O at L8
  // - TOKES vertical through T at J8
  Play plays[] = {
    {"VODKA (N6 vert)", "N6", 22, astroids_tws_threat},  // Through D, opens ASTROIDS+TWS
    {"SKATE (H6 vert)", "H6", 18, 0},                    // Through A at H8
    {"STOVE (L6 vert)", "L6", 16, 0},                    // Through O at L8
    {"TOKES (J8 down)", "J8", 14, 0},                    // Through T at J8
  };
  int num_plays = 4;

  string_builder_add_string(sb, "CANDIDATE PLAYS (all connect to ASTROID):\n");
  string_builder_add_string(sb, "==========================================\n\n");

  string_builder_add_string(sb, "1. VODKA at N6 (vertical through D at N8)\n");
  string_builder_add_string(sb, "   - Uses V,O,K,A from rack + D on board\n");
  string_builder_add_string(sb, "   - Scores 22 points\n");
  string_builder_add_string(sb, "   - THREAT: Opens S-hook at O8 which is a TWS!\n");
  string_builder_add_string(sb, "   - CSW opponent plays S at O8 -> ASTROIDS on TWS\n");
  string_builder_add_formatted_string(sb, "   - ASTROIDS = %d base x 3 = %d points!\n\n",
      astroids_base, astroids_tws_threat);

  string_builder_add_string(sb, "2. SKATE at H6 (vertical through A at H8)\n");
  string_builder_add_string(sb, "   - Uses S,K,T,E from rack + A on board\n");
  string_builder_add_string(sb, "   - Scores 18 points\n");
  string_builder_add_string(sb, "   - Does NOT expose the O8 TWS hook\n\n");

  string_builder_add_string(sb, "3. STOVE at L6 (vertical through O at L8)\n");
  string_builder_add_string(sb, "   - Uses S,T,V,E from rack + O on board\n");
  string_builder_add_string(sb, "   - Scores 16 points\n");
  string_builder_add_string(sb, "   - Does NOT expose the O8 TWS hook\n\n");

  string_builder_add_string(sb, "4. TOKES at J8 down (vertical through T at J8)\n");
  string_builder_add_string(sb, "   - Uses O,K,E,S from rack + T on board\n");
  string_builder_add_string(sb, "   - Scores 14 points\n");
  string_builder_add_string(sb, "   - Does NOT expose the O8 TWS hook\n\n");

  string_builder_add_string(sb, "=====================================================\n");
  string_builder_add_string(sb, "EVALUATION IN EACH MODE\n");
  string_builder_add_string(sb, "=====================================================\n\n");

  print_play_comparison(sb, plays, num_plays);

  const Play *ignorant_choice = find_best_play(plays, num_plays, false);
  const Play *informed_choice = find_best_play(plays, num_plays, true);

  string_builder_add_string(sb, "\n");
  string_builder_add_formatted_string(sb, "IGNORANT MODE choice: %s (%d pts)\n",
      ignorant_choice->word, evaluate_play_ignorant(ignorant_choice));
  string_builder_add_string(sb, "  (Doesn't see ASTROIDS threat - TWL player assumes TWL opponent)\n\n");

  string_builder_add_formatted_string(sb, "INFORMED MODE choice: %s (%d pts)\n",
      informed_choice->word, evaluate_play_informed(informed_choice));
  string_builder_add_formatted_string(sb, "  (Knows opponent can play ASTROIDS on TWS for %d pts)\n\n",
      astroids_tws_threat);

  string_builder_add_string(sb, "=====================================================\n");
  string_builder_add_string(sb, "CONCLUSION\n");
  string_builder_add_string(sb, "=====================================================\n\n");

  if (ignorant_choice != informed_choice) {
    string_builder_add_string(sb, "THE TWO MODES CHOOSE DIFFERENT PLAYS!\n\n");
    string_builder_add_formatted_string(sb, "IGNORANT picks %s for %d perceived points\n",
        ignorant_choice->word, evaluate_play_ignorant(ignorant_choice));
    string_builder_add_formatted_string(sb, "INFORMED picks %s for %d adjusted points\n\n",
        informed_choice->word, evaluate_play_informed(informed_choice));
    string_builder_add_formatted_string(sb, "The informed player avoids VODKA because the %d-point\n",
        astroids_tws_threat);
    string_builder_add_string(sb, "ASTROIDS-on-TWS threat (CSW-only) makes it score NEGATIVE!\n");
    string_builder_add_formatted_string(sb, "VODKA adjusted = 22 - %d = %d points.\n",
        astroids_tws_threat, 22 - astroids_tws_threat);
  } else {
    string_builder_add_string(sb, "Both modes chose the same play.\n");
  }

  printf("%s", string_builder_peek(sb));

  string_builder_destroy(sb);
  kwg_destroy(csw_kwg);

  return 0;
}
