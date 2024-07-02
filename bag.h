#ifndef BAG_H
#define BAG_H

#include <stdbool.h>

#define FIRST_LETTER 1
#define LAST_LETTER 26

static const int english_letter_scores[LAST_LETTER + 1] = {
    0,  // unused
    1,  // A
    3,  // B
    3,  // C
    2,  // D
    1,  // E
    4,  // F
    2,  // G
    4,  // H
    1,  // I
    8,  // J
    5,  // K
    1,  // L
    3,  // M
    1,  // N
    1,  // O
    3,  // P
    10, // Q
    1,  // R
    1,  // S
    1,  // T
    1,  // U
    4,  // V
    4,  // W
    8,  // X
    4,  // Y
    10, // Z
};

static const int english_letter_counts[LAST_LETTER + 1] = {
    0,  // unused
    11, // A
    2,  // B
    3,  // C
    5,  // D
    14, // E
    2,  // F
    3,  // G
    2,  // H
    10, // I
    1,  // J
    1,  // K
    4,  // L
    2,  // M
    7,  // N
    9,  // O
    2,  // P
    1,  // Q
    7,  // R
    6,  // S
    7,  // T
    4,  // U
    2,  // V
    2,  // W
    1,  // X
    2,  // Y
    2,  // Z
};

static const bool is_vowel[LAST_LETTER + 1] = {
    false, // unused
    true,  // A
    false, // B
    false, // C
    false, // D
    true,  // E
    false, // F
    false, // G
    false, // H
    true,  // I
    false, // J
    false, // K
    false, // L
    false, // M
    false, // N
    true,  // O
    false, // P
    false, // Q
    false, // R
    false, // S
    false, // T
    true,  // U
    false, // V
    false, // W
    false, // X
    false, // Y
    false, // Z
};

static const char priorities[] = "QXZJKVBPYGFWMUCLDRHSNIOATE";

#endif // BAG_H