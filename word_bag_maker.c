#include "bag.h"
#include "kwg.h"

#include <assert.h>

void add_possible_words(const KWG *kwg, uint32_t node_index,
                        int letter_counts[28], int prefix[4], int *word_index,
                        int letters_used, bool accepts,
                        int possible_words[40000][4]) {
  // printf("add_possible_words: node_index: %d, letters_used: %d\n",
  // node_index, letters_used);
  if (accepts && (letters_used == 4)) {
    for (int i = 0; i < 4; i++) {
      possible_words[*word_index][i] = prefix[i];
    }
    (*word_index)++;
  }
  if (letters_used == 4) {
    return;
  }
  if (node_index == 0) {
    return;
  }
  for (uint32_t i = node_index;; i++) {
    const uint32_t node = kwg_node(kwg, i);
    const uint8_t ml = kwg_node_tile(node);
    const int new_node_index = kwg_node_arc_index_prefetch(node, kwg);
    if (letter_counts[ml] > 0) {
      const bool node_accepts = kwg_node_accepts(node);
      prefix[letters_used] = ml;
      letter_counts[ml]--;
      add_possible_words(kwg, new_node_index, letter_counts, prefix, word_index,
                         letters_used + 1, node_accepts, possible_words);
      letter_counts[ml]++;
    }
    if (kwg_node_is_end(node)) {
      break;
    }
  }
}

bool find_words_for_quadrant(const KWG *kwg, int quadrants[28][4],
                             int quadrant_index, int words[4][7][4]) {
  // printf("finding words for quadrant %d\n", quadrant_index);
  int letter_counts[LAST_LETTER + 1];
  for (int i = 0; i <= LAST_LETTER; i++) {
    letter_counts[i] = 0;
  }
  for (int i = 0; i < 28; i++) {
    int letter = quadrants[i][quadrant_index];
    letter_counts[letter]++;
  }
  int prefix[4];
  int possible_words[40000][4];

  int chosen_words[7][4];
  int original_letter_counts[28];
  for (int i = 0; i < 28; i++) {
    original_letter_counts[i] = letter_counts[i];
  }
  for (int i = 0; i < 100; i++) {
    int num_chosen_words = 0;
    for (int j = 0; j < 28; j++) {
      letter_counts[j] = original_letter_counts[j];
    }
    for (; num_chosen_words < 7; num_chosen_words++) {
      const uint32_t node_index = kwg_get_dawg_root_node_index(kwg);
      int num_words = 0;
      add_possible_words(kwg, node_index, letter_counts, prefix, &num_words, 0,
                         false, possible_words);
      // printf("num words: %d\n", num_words);
      /*
      for (int i = 0; i < num_words; i++) {
        for (int j = 0; j < 4; j++) {
          printf("%c", possible_words[i][j] + 'A' - 1);
        }
        printf("\n");
      }
      */
      int priority_letter;
      for (int j = 0; j < 27; j++) {
        int priority_list_ml = priorities[j] - 'A' + 1;
        if (letter_counts[priority_list_ml] > 0) {
          priority_letter = priority_list_ml;
          break;
        }
      }
      // printf("priority letter: %c\n", priority_letter + 'A' - 1);
      int num_words_with_priority_letter = 0;
      int words_with_prioritized_letter[40000][4];
      for (int i = 0; i < num_words; i++) {
        for (int j = 0; j < 4; j++) {
          int ml = possible_words[i][j];
          if (ml == priority_letter) {
            for (int k = 0; k < 4; k++) {
              words_with_prioritized_letter[num_words_with_priority_letter][k] =
                  possible_words[i][k];
            }
            num_words_with_priority_letter++;
            break;
          }
        }
      }
      // printf("num words with priority letter: %d\n",
      // num_words_with_priority_letter);
      if (num_words_with_priority_letter == 0) {
        break;
      }
      const int chosen_word_index = rand() % num_words_with_priority_letter;
      for (int j = 0; j < 4; j++) {
        const int ml = words_with_prioritized_letter[chosen_word_index][j];
        letter_counts[ml]--;
        chosen_words[num_chosen_words][j] = ml;
      }
    }
    // printf("num chosen words: %d\n", num_chosen_words);
    if (num_chosen_words == 7) {
      for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 4; j++) {
          // printf("%c", chosen_words[i][j] + 'A' - 1);
          words[quadrant_index][i][j] = chosen_words[i][j];
        }
        // printf("\n");
      }
      return true;
    }
  }
  return false;
}

void print_bag_words(const KWG *kwg) {
  int quadrants[28][4];
  for (int quadrant_index = 0; quadrant_index < 4; quadrant_index++) {
    for (int i = 0; i < 28; i++) {
      quadrants[i][quadrant_index] = 0;
    }
  }
  int vowels[28 * 4];
  int num_vowels = 0;
  for (int letter = FIRST_LETTER; letter <= LAST_LETTER; letter++) {
    if (!is_vowel[letter]) {
      continue;
    }
    for (int i = 0; i < english_letter_counts[letter]; i++) {
      vowels[num_vowels++] = letter;
    }
  }
  int consonants[28 * 4];
  int num_consonants = 0;
  for (int letter = FIRST_LETTER; letter <= LAST_LETTER; letter++) {
    if (is_vowel[letter]) {
      continue;
    }
    for (int i = 0; i < english_letter_counts[letter]; i++) {
      consonants[num_consonants++] = letter;
    }
  }
  // shuffle the vowels
  for (int i = 0; i < num_vowels; i++) {
    int j = rand() % num_vowels;
    int temp = vowels[i];
    vowels[i] = vowels[j];
    vowels[j] = temp;
  }
  // shuffle the consonants
  for (int i = 0; i < num_consonants; i++) {
    int j = rand() % num_consonants;
    int temp = consonants[i];
    consonants[i] = consonants[j];
    consonants[j] = temp;
  }
  assert(num_vowels == 48);
  assert(num_consonants == 64);
  for (int i = 0; i < num_vowels; i++) {
    int quadrant_index = i % 4;
    quadrants[i / 4][quadrant_index] = vowels[i];
  }
  for (int i = 0; i < num_consonants; i++) {
    int quadrant_index = i % 4;
    int offset = num_vowels / 4;
    quadrants[offset + i / 4][quadrant_index] = consonants[i];
  }
  /*
  for (int i = 0; i < 4; i++) {
    printf("quadrant %d: ", i);
    for (int j = 0; j < 28; j++) {
      printf("%c", quadrants[j][i] + 'A' - 1);
    }
    printf("\n");
  }
*/
  int words[4][7][4];
  for (int i = 0; i < 4; i++) {
    bool usable_quadrant = find_words_for_quadrant(kwg, quadrants, i, words);
    if (!usable_quadrant) {
      // printf("could not find words for quadrant %d\n", i);
      return;
    }
  }
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 7; j++) {
      for (int k = 0; k < 4; k++) {
        printf("%c", words[i][j][k] + 'A' - 1);
      }
      printf(" ");
    }
  }
  printf("\n");
}

int main(void) {
  KWG *kwg = kwg_create(".", "CSW21.kwg");
  // printf("kwg size: %d\n", kwg->number_of_nodes);
  for (int i = 0; i < 10000; i++) {
    print_bag_words(kwg);
  }
  return 0;
}