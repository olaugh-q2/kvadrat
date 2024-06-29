#include "tetrominos.h"
#include "square.h"

void CreatePiece(int piece_index, int piece[4][4], int rotation) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      piece[i][j] = EMPTY_SQUARE;
    }
  }
  const int v = ACTIVE_MASK | piece_index;
  switch (piece_index) {
  case I_PIECE:
    switch (rotation) {
    case ROTATION_0:
      piece[1][0] = v; //
      piece[1][1] = v; // [][][][]
      piece[1][2] = v; //
      piece[1][3] = v; //
      break;
    case ROTATION_R:
      piece[0][2] = v; //     []
      piece[1][2] = v; //     []
      piece[2][2] = v; //     []
      piece[3][2] = v; //     []
      break;
    case ROTATION_2:
      piece[2][0] = v; //
      piece[2][1] = v; //
      piece[2][2] = v; // [][][][]
      piece[2][3] = v; //
      break;
    case ROTATION_L:
      piece[0][1] = v; //   []
      piece[1][1] = v; //   []
      piece[2][1] = v; //   []
      piece[3][1] = v; //   []
      break;
    }
    break;
  case J_PIECE:
    switch (rotation) {
    case ROTATION_0:
      piece[0][0] = v; // []
      piece[1][0] = v; // [][][]
      piece[1][1] = v; //
      piece[1][2] = v;
      break;
    case ROTATION_R:
      piece[0][1] = v; //   [][]
      piece[0][2] = v; //   []
      piece[1][1] = v; //   []
      piece[2][2] = v;
      break;
    case ROTATION_2:
      piece[1][0] = v; //
      piece[1][1] = v; // [][][]
      piece[1][2] = v; //     []
      piece[2][2] = v;
      break;
    case ROTATION_L:
      piece[0][2] = v; //   []
      piece[1][2] = v; //   []
      piece[2][1] = v; // [][]
      piece[2][2] = v;
      break;
    }
    break;
  case L_PIECE:
    switch (rotation) {
    case ROTATION_0:
      piece[0][2] = v; //     []
      piece[1][0] = v; // [][][]
      piece[1][1] = v; //
      piece[1][2] = v;
      break;
    case ROTATION_R:
      piece[0][1] = v; //   []
      piece[1][1] = v; //   []
      piece[2][1] = v; //   [][]
      piece[2][2] = v;
      break;
    case ROTATION_2:
      piece[1][0] = v; //
      piece[1][1] = v; // [][][]
      piece[1][2] = v; // []
      piece[2][0] = v;
      break;
    case ROTATION_L:
      piece[0][0] = v; // [][]
      piece[0][1] = v; //   []
      piece[1][1] = v; //   []
      piece[2][1] = v;
      break;
    }
    break;
  case O_PIECE:
    piece[1][1] = v; //
    piece[1][2] = v; //   [][]
    piece[2][1] = v; //   [][]
    piece[2][2] = v;
    break;
  case S_PIECE:
    switch (rotation) {
    case ROTATION_0:
      piece[0][1] = v; //   [][]
      piece[0][2] = v; // [][]
      piece[1][0] = v; //
      piece[1][1] = v;
      break;
    case ROTATION_R:
      piece[0][1] = v; //   []
      piece[1][1] = v; //   [][]
      piece[1][2] = v; //     []
      piece[2][2] = v;
      break;
    case ROTATION_2:
      piece[1][1] = v; //
      piece[1][2] = v; //   [][]
      piece[2][0] = v; // [][]
      piece[2][1] = v;
      break;
    case ROTATION_L:
      piece[0][0] = v; // []
      piece[1][0] = v; // [][]
      piece[1][1] = v; //   []
      piece[2][1] = v;
      break;
    }
    break;
  case T_PIECE:
    switch (rotation) {
    case ROTATION_0:
      piece[0][1] = v; //   []
      piece[1][0] = v; // [][][]
      piece[1][1] = v; //
      piece[1][2] = v;
      break;
    case ROTATION_R:
      piece[0][1] = v; //   []
      piece[1][1] = v; //   [][]
      piece[1][2] = v; //   []
      piece[2][1] = v;
      break;
    case ROTATION_2:
      piece[1][0] = v; //
      piece[1][1] = v; // [][][]
      piece[1][2] = v; //   []
      piece[2][1] = v;
      break;
    case ROTATION_L:
      piece[0][1] = v; //   []
      piece[1][0] = v; // [][]
      piece[1][1] = v; //   []
      piece[2][1] = v;
      break;
    }
    break;
  case Z_PIECE:
    switch (rotation) {
    case ROTATION_0:
      piece[0][0] = v; // [][]
      piece[1][0] = v; //   [][]
      piece[1][1] = v; //
      piece[2][1] = v;
      break;
    case ROTATION_R:
      piece[0][2] = v; //     []
      piece[1][1] = v; //   [][]
      piece[1][2] = v; //   []
      piece[2][1] = v;
      break;
    case ROTATION_2:
      piece[1][0] = v; //
      piece[1][1] = v; // [][]
      piece[2][1] = v; //   [][]
      piece[2][2] = v;
      break;
    case ROTATION_L:
      piece[0][1] = v; //   []
      piece[1][0] = v; // [][]
      piece[1][1] = v; // []
      piece[2][0] = v;
      break;
    }
    break;
  }
}

int SpawnColumn(int piece_index) {
  return 3;
}