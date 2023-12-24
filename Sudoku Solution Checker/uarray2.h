/******************************************************************************
 *
 *                     uarray2.h
 *
 *     Assignment: iii
 *     Authors:    ihackm01 and nmoren01
 *     Date:       9/21/2023
 *
 *     summary:    uarray2.h is the interface for a 2D array that represents a 
 *                 portable graymap for a plain pgm or P2.
 *
 *****************************************************************************/
 
#ifndef UARRAY2_H
#define UARRAY2_H

#include <stdio.h>
#include <stdlib.h>
#include "uarray.h"

#define T UArray2_T
typedef struct T *T;

T UArray2_new(int width, int height, int size);

void UArray2_map_col_major(T uarray2, void apply(int col, int row, UArray2_T a, 
                           void *p1, void *p2), void *cl);

void UArray2_map_row_major(T uarray2, void apply(int col, int row, UArray2_T a, 
                           void *p1, void *p2), void *cl);

void *UArray2_at(T uarray2, int col, int row);
void UArray2_free(T *uarray2);

int UArray2_width(T uarray2);

int UArray2_height(T uarray2);

int UArray2_size(T uarray2);

#undef T
#endif