#include "bag.h"
#include "kwg.h"
#include "playfield.h"
#include "tetrominos.h"

#include <assert.h>
#include <stddef.h>

void generate_permuations(int alphagram_letter_counts[LAST_LETTER + 1],
                          char permutations[24][4], int *num_permutations) {
  *num_permutations = 0;
  for (int i = 0; i < LAST_LETTER + 1; i++) {
    if (alphagram_letter_counts[i] > 0) {
      alphagram_letter_counts[i]--;
      for (int j = 0; j < LAST_LETTER + 1; j++) {
        if (alphagram_letter_counts[j] > 0) {
          alphagram_letter_counts[j]--;
          for (int k = 0; k < LAST_LETTER + 1; k++) {
            if (alphagram_letter_counts[k] > 0) {
              alphagram_letter_counts[k]--;
              for (int l = 0; l < LAST_LETTER + 1; l++) {
                if (alphagram_letter_counts[l] > 0) {
                  alphagram_letter_counts[l]--;
                  permutations[*num_permutations][0] = i + 'A' - 1;
                  permutations[*num_permutations][1] = j + 'A' - 1;
                  permutations[*num_permutations][2] = k + 'A' - 1;
                  permutations[*num_permutations][3] = l + 'A' - 1;
                  (*num_permutations)++;
                  alphagram_letter_counts[l]++;
                }
              }
              alphagram_letter_counts[k]++;
            }
          }
          alphagram_letter_counts[j]++;
        }
      }
      alphagram_letter_counts[i]++;
    }
  }
}

void print_best_permutations(char *alphagram, char *bags, int num_bags) {
  int alphagram_letter_counts[LAST_LETTER + 1];
  for (int i = 0; i < LAST_LETTER + 1; i++) {
    alphagram_letter_counts[i] = 0;
  }
  for (int i = 0; i < 4; i++) {
    alphagram_letter_counts[alphagram[i] - 'A' + 1]++;
  }
  int *indices_matching_alphagram;
  int num_matching_indices = 0;
  indices_matching_alphagram = malloc(num_bags * sizeof(int));
  for (int i = 0; i < num_bags; i++) {
    for (int j = 0; j < 28; j++) {
      int word_letter_counts[LAST_LETTER + 1];
      for (int k = 0; k < LAST_LETTER + 1; k++) {
        word_letter_counts[k] = 0;
      }
      for (int k = 0; k < 4; k++) {
        word_letter_counts[bags[i * (28 * 5 + 1) + j * 5 + k] - 'A' + 1]++;
      }
      bool is_match = true;
      for (int k = 0; k < LAST_LETTER + 1; k++) {
        if (alphagram_letter_counts[k] != word_letter_counts[k]) {
          is_match = false;
          break;
        }
      }
      if (is_match) {
        indices_matching_alphagram[num_matching_indices] = i;
        num_matching_indices++;
        break;
      }
    }
  }
  int num_permutations = 0;
  char permutations[24][4];
  generate_permuations(alphagram_letter_counts, permutations, &num_permutations);
  printf("alphagram: %s bags_matching_alphagram: %d num_permutations: %d\n", alphagram,
         num_matching_indices, num_permutations);
  for (int piece_type = 1; piece_type <= 7; piece_type++) {
    int permutation_scores[24];
    for (int i = 0; i < num_permutations; i++) {
      permutation_scores[i] = -1000000;      
    }
    for (int i = 0; i < 100; i++) {
      int other_six_pieces[6];
      int index = 0;
      for (int j = 1; j <= 7; j++) {
        if (j != piece_type) {
          other_six_pieces[index] = j;
          index++;
        }
      }
      // shuffle other six pieces
      for (int j = 5; j > 0; j--) {
        int k = rand() % (j + 1);
        int temp = other_six_pieces[j];
        other_six_pieces[j] = other_six_pieces[k];
        other_six_pieces[k] = temp;
      }
      int random_matching_index = rand() % num_matching_indices;
      int bag_index = indices_matching_alphagram[random_matching_index];
      int first_matching_word_index = -1;
      for (int j = 0; j < 28; j++) {
        int word_letter_counts[LAST_LETTER + 1];
        for (int k = 0; k < LAST_LETTER + 1; k++) {
          word_letter_counts[k] = 0;
        }
        for (int k = 0; k < 4; k++) {
          word_letter_counts[bags[bag_index * (28 * 5 + 1) + j * 5 + k] - 'A' + 1]++;
        }
        bool is_match = true;
        for (int k = 0; k < LAST_LETTER + 1; k++) {
          if (alphagram_letter_counts[k] != word_letter_counts[k]) {
            is_match = false;
            break;
          }
        }
        if (is_match) {
          first_matching_word_index = j;
          break;
        }
      }
      assert(first_matching_word_index != -1);
      int other_twenty_seven_words[27];
      index = 0;
      for (int j = 0; j < 28; j++) {
        if (j != first_matching_word_index) {
          other_twenty_seven_words[index] = j;
          index++;
        }
      }
      // shuffle other twenty seven words
      for (int j = 26; j > 0; j--) {
        int k = rand() % (j + 1);
        int temp = other_twenty_seven_words[j];
        other_twenty_seven_words[j] = other_twenty_seven_words[k];
        other_twenty_seven_words[k] = temp;
      }
      Playfield *playfield = CreateInitialPlayfield();
      for (int i = 0; i < 6; i++) {
        int piece = other_six_pieces[i];
        int word_index = other_twenty_seven_words[i];
        char word[4];
        for (int j = 0; j < 4; j++) {
          word[j] = bags[bag_index * (28 * 5 + 1) + word_index * 5 + j];
        }
      }
    }
  }         
}

int main(void) {
  // load csw21-alphagrams.txt to malloc'ed char* array
  FILE *alphagrams_stream = stream_from_filename("csw21-alphagrams.txt");
  if (!alphagrams_stream) {
    printf("could not open alphagrams_stream\n");
    return 1;
  }
  fseek(alphagrams_stream, 0, SEEK_END);
  size_t alphagrams_size = ftell(alphagrams_stream);
  fseek(alphagrams_stream, 0, SEEK_SET);
  char *alphagrams = malloc(alphagrams_size);
  if (!alphagrams) {
    printf("could not malloc alphagrams\n");
    return 1;
  }
  size_t read_size = fread(alphagrams, 1, alphagrams_size, alphagrams_stream);
  if (read_size != alphagrams_size) {
    printf("could not read alphagrams\n");
    return 1;
  }
  printf("number of alphagrams: %d\n", (int)alphagrams_size / 5);
  FILE *bags_stream = stream_from_filename("csw21-bags.txt");
  if (!bags_stream) {
    printf("could not open bags_stream\n");
    return 1;
  }
  fseek(bags_stream, 0, SEEK_END);
  size_t bags_size = ftell(bags_stream);
  fseek(bags_stream, 0, SEEK_SET);
  char *bags = malloc(bags_size);
  if (!bags) {
    printf("could not malloc bags\n");
    return 1;
  }
  read_size = fread(bags, 1, bags_size, bags_stream);
  if (read_size != bags_size) {
    printf("could not read bags\n");
    return 1;
  }
  printf("number of bags: %d\n", (int)bags_size / (28 * 5 + 1));

  for (int i = 0; i < alphagrams_size / 5; i++) {
    char alphagram[5];
    memcpy(alphagram, alphagrams + i * 5, 5);
    alphagram[4] = '\0';
    // printf("%s\n", alphagram);
    print_best_permutations(alphagram, bags, bags_size / (28 * 5 + 1));
  }

  return 0;
}