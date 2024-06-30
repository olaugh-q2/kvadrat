#include "tetrominos.h"
#include "square.h"

#include <assert.h>
#include <stdio.h>

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
      piece[2][1] = v;
      break;
    case ROTATION_2:
      piece[1][0] = v; //
      piece[1][1] = v; // [][][]
      piece[1][2] = v; //     []
      piece[2][2] = v;
      break;
    case ROTATION_L:
      piece[0][1] = v; //   []
      piece[1][1] = v; //   []
      piece[2][0] = v; // [][]
      piece[2][1] = v;
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
      piece[0][1] = v; //   [][]
      piece[1][1] = v; //
      piece[1][2] = v;
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

int SpawnColumn(int piece_index) { return 3; }

void GetRotationOffsets(int piece_index, int starting_rotation,
                        int rotation_direction, int test_index, int *row_delta,
                        int *col_delta) {
  *row_delta = 0;
  *col_delta = 0;
  printf("GetRotationOffsets piece_index: %d, starting_rotation: %d, rotation_direction: %d, test_index: %d\n", piece_index, starting_rotation, rotation_direction, test_index);
  if ((test_index == 0) || (piece_index == O_PIECE)) {
    return;
  }
  switch (piece_index) {
  case I_PIECE:
    switch (starting_rotation) {
    case ROTATION_0:
      switch (rotation_direction) {
      case 1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = -2;
          return;
        case 2:
          *row_delta = 0;
          *col_delta = 1;
          return;
        case 3:
          *row_delta = -1;
          *col_delta = -2;
          return;
        case 4:
          *row_delta = -2;
          *col_delta = 1;
          return;
        }
      case -1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = -1;
          return;
        case 2:
          *row_delta = 0;
          *col_delta = 2;
          return;
        case 3:
          *row_delta = -2;
          *col_delta = -1;
          return;
        case 4:
          *row_delta = 1;
          *col_delta = 2;
          return;
        }
      }
    case ROTATION_R:
      switch (rotation_direction) {
      case 1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = -1;
          return;
        case 2:
          *row_delta = 0;
          *col_delta = 2;
          return;
        case 3:
          *row_delta = -2;
          *col_delta = -1;
          return;
        case 4:
          *row_delta = 1;
          *col_delta = 2;
          return;
        }
      case -1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = 2;
          return;
        case 2:
          *row_delta = 0;
          *col_delta = -1;
          return;
        case 3:
          *row_delta = -1;
          *col_delta = 2;
          return;
        case 4:
          *row_delta = -2;
          *col_delta = 1;
          return;
        }
      }
    case ROTATION_2:
      switch (rotation_direction) {
      case 1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = 2;
          return;
        case 2:
          *row_delta = 0;
          *col_delta = -1;
          return;
        case 3:
          *row_delta = -1;
          *col_delta = 2;
          return;
        case 4:
          *row_delta = -2;
          *col_delta = -1;
          return;
        }
        break;
      case -1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = 1;
          return;
        case 2:
          *row_delta = 0;
          *col_delta = -2;
          return;
        case 3:
          *row_delta = 2;
          *col_delta = 1;
          return;
        case 4:
          *row_delta = -1;
          *col_delta = -2;
          return;
        }
      }
    case ROTATION_L:
      switch (rotation_direction) {
      case 1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = 1;
          return;
        case 2:
          *row_delta = 0;
          *col_delta = -2;
          return;
        case 3:
          *row_delta = 2;
          *col_delta = 1;
          return;
        case 4:
          *row_delta = -1;
          *col_delta = -2;
          return;
        }
      case -1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = -2;
          return;
        case 2:
          *row_delta = 0;
          *col_delta = 1;
          return;
        case 3:
          *row_delta = 1;
          *col_delta = -2;
          return;
        case 4:
          *row_delta = -2;
          *col_delta = 1;
          return;
        }
      }
    }
  case J_PIECE:
  case L_PIECE:
  case S_PIECE:
  case T_PIECE:
  case Z_PIECE:
    switch (starting_rotation) {
    case ROTATION_0:
      switch (rotation_direction) {
      case 1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = -1;
          return;
        case 2:
          *row_delta = -1;
          *col_delta = -1;
          return;
        case 3:
          *row_delta = 2;
          *col_delta = 0;
          return;
        case 4:
          *row_delta = 2;
          *col_delta = -1;
          return;
        }
      case -1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = 1;
          return;
        case 2:
          *row_delta = -1;
          *col_delta = 1;
          return;
        case 3:
          *row_delta = 2;
          *col_delta = 0;
          return;
        case 4:
          *row_delta = 2;
          *col_delta = 1;
          return;
        }
      }
    case ROTATION_R:
      switch (rotation_direction) {
      case 1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = 1;
          return;
        case 2:
          *row_delta = 1;
          *col_delta = 1;
          return;
        case 3:
          *row_delta = -2;
          *col_delta = 0;
          return;
        case 4:
          *row_delta = -2;
          *col_delta = 1;
          return;
        }
      case -1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = 1;
          return;
        case 2:
          *row_delta = 1;
          *col_delta = 1;
          return;
        case 3:
          *row_delta = -2;
          *col_delta = 0;
          return;
        case 4:
          *row_delta = -2;
          *col_delta = 1;
          return;
        }
        break;
      }
    case ROTATION_2:
      switch (rotation_direction) {
      case 1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = 1;
          return;
        case 2:
          *row_delta = -1;
          *col_delta = -1;
          return;
        case 3:
          *row_delta = 2;
          *col_delta = 0;
          return;
        case 4:
          *row_delta = 2;
          *col_delta = 1;
          return;
        }
      case -1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = -1;
          return;
        case 2:
          *row_delta = -1;
          *col_delta = -1;
          return;
        case 3:
          *row_delta = 2;
          *col_delta = 0;
          return;
        case 4:
          *row_delta = 2;
          *col_delta = -1;
          return;
        }
      }
    case ROTATION_L:
      switch (rotation_direction) {
      case 1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = -1;
          return;
        case 2:
          *row_delta = 1;
          *col_delta = -1;
          return;
        case 3:
          *row_delta = -2;
          *col_delta = 0;
          return;
        case 4:
          *row_delta = -2;
          *col_delta = -1;
          return;
        }
      case -1:
        switch (test_index) {
        case 1:
          *row_delta = 0;
          *col_delta = -1;
          return;
        case 2:
          *row_delta = -1;
          *col_delta = -1;
          return;
        case 3:
          *row_delta = -2;
          *col_delta = 0;
          return;
        case 4:
          *row_delta = -2;
          *col_delta = -1;
          return;
        }
      }
    }
  }
  printf("unhandled case!!");
  assert(false);
}