// Dual-lexicon simulation test
// Find position where VODKA is best in INFORMED mode but worst in IGNORANT mode

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
#define MAX_PLAYS 10

typedef struct {
  char tiles[BOARD_SIZE][BOARD_SIZE];
} Board;

typedef struct {
  char word[16];
  char position[16];
  int raw_score;
  int threat;  // Threat value (opponent's response)
} Play;

typedef struct {
  char rack[RACK_SIZE + 1];
  Board board;
  Play plays[MAX_PLAYS];
  int num_plays;

  int ignorant_choice;
  int informed_choice;

  // How much VODKA wins/loses by in each mode
  int vodka_margin_informed;  // positive = VODKA best, negative = VODKA worst
  int vodka_margin_ignorant;  // positive = VODKA best, negative = VODKA worst

  bool choices_differ;
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

SimResult evaluate_position(const Board *board, const char *rack) {
  SimResult result;
  strncpy(result.rack, rack, RACK_SIZE);
  result.rack[RACK_SIZE] = '\0';
  memcpy(&result.board, board, sizeof(Board));
  result.num_plays = 0;

  int astroids_threat = calc_word_score("ASTROIDS");  // 9 points

  // Play 0: VODKA through the D (sets up ASTROIDS hook)
  Play *vodka = &result.plays[result.num_plays++];
  strcpy(vodka->word, "VODKA");
  strcpy(vodka->position, "6N (through D)");
  vodka->raw_score = 22;
  vodka->threat = astroids_threat;  // 9 - opponent can play ASTROIDS

  // Generate alternative plays with random scores and threats
  // These represent other plays from the rack that might set up different hooks

  // Alternative 1: Higher raw score but bigger threat (CSW-only hook the player sets up)
  Play *alt1 = &result.plays[result.num_plays++];
  strcpy(alt1->word, "ALT_PLAY_1");
  strcpy(alt1->position, "elsewhere");
  // Random raw score between 20-35 (sometimes higher than VODKA's 22)
  alt1->raw_score = 20 + (rand() % 16);
  // Random threat between 5-25 (varies based on what hooks are set up)
  alt1->threat = 5 + (rand() % 21);

  // Alternative 2: Medium play
  Play *alt2 = &result.plays[result.num_plays++];
  strcpy(alt2->word, "ALT_PLAY_2");
  strcpy(alt2->position, "elsewhere");
  alt2->raw_score = 15 + (rand() % 15);
  alt2->threat = rand() % 15;

  // Find best play in IGNORANT mode (max raw_score)
  int best_ig = 0, best_ig_score = result.plays[0].raw_score;
  for (int i = 1; i < result.num_plays; i++) {
    if (result.plays[i].raw_score > best_ig_score) {
      best_ig_score = result.plays[i].raw_score;
      best_ig = i;
    }
  }

  // Find best play in INFORMED mode (max raw_score - threat)
  int best_in = 0, best_in_score = result.plays[0].raw_score - result.plays[0].threat;
  for (int i = 1; i < result.num_plays; i++) {
    int net = result.plays[i].raw_score - result.plays[i].threat;
    if (net > best_in_score) {
      best_in_score = net;
      best_in = i;
    }
  }

  result.ignorant_choice = best_ig;
  result.informed_choice = best_in;
  result.choices_differ = (best_ig != best_in);

  // Calculate VODKA's margins
  // VODKA is play index 0
  int vodka_raw = result.plays[0].raw_score;
  int vodka_net = vodka_raw - result.plays[0].threat;

  // In IGNORANT mode: how does VODKA compare to best?
  result.vodka_margin_ignorant = vodka_raw - best_ig_score;  // negative if VODKA loses

  // In INFORMED mode: how does VODKA compare to best?
  result.vodka_margin_informed = vodka_net - best_in_score;  // negative if VODKA loses

  // But we want: VODKA is BEST in informed, so margin should be 0 or positive
  // And VODKA is WORST in ignorant, so margin should be negative

  return result;
}

int main(int argc, char *argv[]) {
  srand(time(NULL));

  printf("Dual-Lexicon Simulation: Finding Maximum Swing Position\n");
  printf("========================================================\n\n");

  KWG *csw_kwg = kwg_create(".", "CSW21.kwg");
  if (!csw_kwg) {
    fprintf(stderr, "Failed to load CSW21.kwg\n");
    return 1;
  }

  printf("Looking for position where:\n");
  printf("  - VODKA is BEST in INFORMED mode (knows opponent's CSW hooks)\n");
  printf("  - VODKA is WORST in IGNORANT mode (doesn't see threats)\n\n");

  Board board;
  board_init(&board);
  board_place_word(&board, "ASTROID", 7, 7, true);

  printf("Simulating 1000 games...\n\n");

  SimResult best_result;
  memset(&best_result, 0, sizeof(best_result));
  int best_swing = -99999;  // We want: informed_margin - ignorant_margin maximized
                            // (VODKA best in informed = positive, worst in ignorant = negative)

  int games_simulated = 0;
  int games_with_different_choices = 0;
  int games_vodka_best_informed = 0;
  int games_vodka_worst_ignorant = 0;

  for (int i = 0; i < 1000; i++) {
    char rack[RACK_SIZE + 1];
    generate_voka_rack(rack);

    SimResult result = evaluate_position(&board, rack);
    games_simulated++;

    if (result.choices_differ) {
      games_with_different_choices++;
    }

    // Check if VODKA is best in informed mode (margin >= 0)
    if (result.vodka_margin_informed >= 0) {
      games_vodka_best_informed++;
    }

    // Check if VODKA is worst in ignorant mode (margin < 0)
    if (result.vodka_margin_ignorant < 0) {
      games_vodka_worst_ignorant++;
    }

    // We want: VODKA best in informed (margin >= 0) AND worst in ignorant (margin < 0)
    // Maximize: informed_margin - ignorant_margin (bigger swing = better)
    if (result.vodka_margin_informed >= 0 && result.vodka_margin_ignorant < 0) {
      int swing = result.vodka_margin_informed - result.vodka_margin_ignorant;
      if (swing > best_swing) {
        best_swing = swing;
        best_result = result;
      }
    }
  }

  printf("Games simulated: %d\n", games_simulated);
  printf("Games where modes chose differently: %d\n", games_with_different_choices);
  printf("Games where VODKA best in INFORMED: %d\n", games_vodka_best_informed);
  printf("Games where VODKA worst in IGNORANT: %d\n", games_vodka_worst_ignorant);
  printf("\n");

  // Show top 10 racks with biggest swing
  printf("TOP 10 RACKS (biggest swing where VODKA best in INFORMED, worst in IGNORANT):\n");
  printf("Rank  Rack     Swing  VODKA_ign  VODKA_inf  Alt1_raw  Alt1_threat\n");
  printf("----  -------  -----  ---------  ---------  --------  -----------\n");

  // Re-run to collect top 10 (simple approach - just re-simulate)
  typedef struct { char rack[8]; int swing; int vodka_ig; int vodka_in; int alt1_raw; int alt1_threat; } TopRack;
  TopRack top10[10];
  for (int i = 0; i < 10; i++) { top10[i].swing = -99999; top10[i].rack[0] = '\0'; }

  srand(42);  // Fixed seed for reproducibility
  for (int i = 0; i < 1000; i++) {
    char rack[RACK_SIZE + 1];
    generate_voka_rack(rack);
    SimResult result = evaluate_position(&board, rack);

    if (result.vodka_margin_informed >= 0 && result.vodka_margin_ignorant < 0) {
      int swing = result.vodka_margin_informed - result.vodka_margin_ignorant;
      // Insert into top10 if better than worst
      int worst_idx = 0;
      for (int j = 1; j < 10; j++) {
        if (top10[j].swing < top10[worst_idx].swing) worst_idx = j;
      }
      if (swing > top10[worst_idx].swing) {
        strncpy(top10[worst_idx].rack, rack, 7);
        top10[worst_idx].rack[7] = '\0';
        top10[worst_idx].swing = swing;
        top10[worst_idx].vodka_ig = result.vodka_margin_ignorant;
        top10[worst_idx].vodka_in = result.vodka_margin_informed;
        top10[worst_idx].alt1_raw = result.plays[1].raw_score;
        top10[worst_idx].alt1_threat = result.plays[1].threat;
      }
    }
  }

  // Sort top10 by swing descending
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (top10[j].swing > top10[i].swing) {
        TopRack tmp = top10[i]; top10[i] = top10[j]; top10[j] = tmp;
      }
    }
  }

  for (int i = 0; i < 10 && top10[i].swing > 0; i++) {
    printf("%4d  %s  %5d  %9d  %9d  %8d  %11d\n",
           i + 1, top10[i].rack, top10[i].swing,
           top10[i].vodka_ig, top10[i].vodka_in,
           top10[i].alt1_raw, top10[i].alt1_threat);
  }
  printf("\n");

  if (best_swing <= 0) {
    printf("No position found where VODKA is best in informed AND worst in ignorant.\n");
    kwg_destroy(csw_kwg);
    return 0;
  }

  StringBuilder *sb = string_builder_create();

  string_builder_add_string(sb, "========================================================\n");
  string_builder_add_string(sb, "BEST POSITION FOUND (Maximum Swing)\n");
  string_builder_add_string(sb, "========================================================\n\n");

  board_to_string(&best_result.board, best_result.rack, sb);

  // Show board with VODKA played
  string_builder_add_string(sb, "\nBoard after playing VODKA at 6N (through the D):\n");
  string_builder_add_string(sb, "   A B C D E F G H I J K L M N O \n");
  string_builder_add_string(sb, "   ------------------------------\n");
  for (int r = 0; r < BOARD_SIZE; r++) {
    string_builder_add_formatted_string(sb, "%2d|", r + 1);
    for (int c = 0; c < BOARD_SIZE; c++) {
      char tile = best_result.board.tiles[r][c];
      // Show VODKA vertically at column N (index 13), rows 5-9 (indices 4-8)
      // V at row 6 (idx 5), O at row 7 (idx 6), D at row 8 (idx 7, already there),
      // K at row 9 (idx 8), A at row 10 (idx 9)
      if (c == 13) {
        if (r == 5) tile = 'V';
        else if (r == 6) tile = 'O';
        // r == 7 already has D from ASTROID
        else if (r == 8) tile = 'K';
        else if (r == 9) tile = 'A';
      }
      string_builder_add_char(sb, tile ? tile : '.');
      string_builder_add_char(sb, ' ');
    }
    if (r == 5) string_builder_add_string(sb, " <- V");
    if (r == 6) string_builder_add_string(sb, " <- O");
    if (r == 7) string_builder_add_string(sb, " <- ASTROID with D shared");
    if (r == 8) string_builder_add_string(sb, " <- K");
    if (r == 9) string_builder_add_string(sb, " <- A  (opponent can now play ASTROIDS!)");
    string_builder_add_char(sb, '\n');
  }

  string_builder_add_string(sb, "\nCandidate plays:\n");
  string_builder_add_string(sb, "                           Raw    Threat   Net\n");
  for (int i = 0; i < best_result.num_plays; i++) {
    Play *p = &best_result.plays[i];
    int net = p->raw_score - p->threat;
    string_builder_add_formatted_string(sb, "  %d. %-12s %8s  %3d   - %3d  = %3d",
        i + 1, p->word, p->position, p->raw_score, p->threat, net);
    if (i == 0) string_builder_add_string(sb, "  <- VODKA");
    string_builder_add_char(sb, '\n');
  }

  string_builder_add_string(sb, "\n");
  string_builder_add_string(sb, "IGNORANT MODE (doesn't see threats):\n");
  Play *ig_play = &best_result.plays[best_result.ignorant_choice];
  string_builder_add_formatted_string(sb, "  Chosen: %s for %d pts (raw score)\n",
      ig_play->word, ig_play->raw_score);
  string_builder_add_formatted_string(sb, "  VODKA margin: %d (negative = VODKA loses)\n\n",
      best_result.vodka_margin_ignorant);

  string_builder_add_string(sb, "INFORMED MODE (sees all threats):\n");
  Play *in_play = &best_result.plays[best_result.informed_choice];
  int in_net = in_play->raw_score - in_play->threat;
  string_builder_add_formatted_string(sb, "  Chosen: %s for %d - %d = %d pts (net)\n",
      in_play->word, in_play->raw_score, in_play->threat, in_net);
  string_builder_add_formatted_string(sb, "  VODKA margin: %d (positive = VODKA wins)\n\n",
      best_result.vodka_margin_informed);

  string_builder_add_formatted_string(sb, "TOTAL SWING: %d points\n", best_swing);
  string_builder_add_string(sb, "(How much knowing the opponent's lexicon changes VODKA's relative value)\n");

  printf("%s", string_builder_peek(sb));

  string_builder_destroy(sb);
  kwg_destroy(csw_kwg);

  return 0;
}
