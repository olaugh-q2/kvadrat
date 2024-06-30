#ifndef TETROMINOS_H
#define TETROMINOS_H

#include "raylib.h"

#define I_PIECE 1
#define J_PIECE 2
#define L_PIECE 3
#define O_PIECE 4
#define S_PIECE 5
#define T_PIECE 6
#define Z_PIECE 7

#define ROTATION_0 0
#define ROTATION_R 1
#define ROTATION_2 2
#define ROTATION_L 3

#define CLOCKWISE_DIRECTION 1;
#define COUNTERCLOCKWISE_DIRECTION (-1);

#define COLOR_ACTIVE_I_PIECE CLITERAL(Color){0, 227, 251, 255}  // light blue
#define COLOR_ACTIVE_J_PIECE CLITERAL(Color){36, 18, 247, 255}  // dark blue
#define COLOR_ACTIVE_L_PIECE CLITERAL(Color){255, 102, 34, 255} // orange
#define COLOR_ACTIVE_O_PIECE CLITERAL(Color){255, 216, 53, 255} // yellow
#define COLOR_ACTIVE_S_PIECE CLITERAL(Color){48, 253, 57, 255}  // green
#define COLOR_ACTIVE_Z_PIECE CLITERAL(Color){255, 6, 56, 255}   // red
#define COLOR_ACTIVE_T_PIECE CLITERAL(Color){186, 14, 245, 255} // magenta

void CreatePiece(int piece_index, int piece[4][4], int rotation);

int SpawnColumn(int piece_index);

void GetRotationOffsets(int piece_index, int starting_rotation,
                        int rotation_direction, int test_index, int *row_delta,
                        int *col_delta);

#endif // TETROMINOS_H