/******************************************************************************
 *
 *                     bit2.h
 *
 *     Assignment: iii
 *     Authors:    ihackm01 and nmoren01
 *     Date:       9/21/2023
 *
 *     summary:    bit2.h is the interface for a 2D array of bits that 
 *                 represents an image in pbm format (black represented as 1 
 *                 and white as 0).
 *
 *****************************************************************************/

#ifndef PRINTING_H
#define PRINTING_H

#include <stdio.h>
#include <stdlib.h>
#include "uarray.h"

#define T Bit2_T
typedef struct T *T;

T Bit2_new(int width, int height);

void Bit2_map_col_major(T Bit2, void apply(int col, int row, Bit2_T a, 
                        int b, void *p1), void *cl);

void Bit2_map_row_major(T Bit2, void apply(int col, int row, Bit2_T a, 
                        int b, void *p1), void *cl);

int Bit2_put(T Bit2, int col,  int row, int other);

int Bit2_get(T Bit2, int col, int row);

int Bit2_width(T Bit2);

int Bit2_height(T Bit2);

int Bit2_size(T Bit2);

void Bit2_free(T *Bit2);

#undef T

#endif