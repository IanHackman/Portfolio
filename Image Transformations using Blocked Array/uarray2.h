/**************************************************************
 *
 *                     uarray2.h
 *
 *     Assignment: iii
 *     Authors:  Will Randall (wranda01), Joseph Carpenter (jcarpe04)
 *     Date:     9/28/23
 *
 *     The interface of the uarray2 program. Clients will use this interface
 *     when they need to utilize a 2 dimensional array-like structure
 *     with elements of any size.
 *
 **************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "mem.h"
#include "assert.h"
#include "uarray.h"


typedef struct UArray2_T *UArray2_T;


extern UArray2_T UArray2_new(int width, int height, int size);

extern void UArray2_free(UArray2_T *uarray2_ptr);

extern int UArray2_width(UArray2_T uarray2);

extern int UArray2_height(UArray2_T uarray2);

extern int UArray2_size(UArray2_T uarray2);

void *UArray2_at(UArray2_T uarray2, int col, int row);

void UArray2_map_row_major(UArray2_T uarray2,
                           void apply(int col, int row, UArray2_T uarray2,
                                      void *value, void *cl),
                           void *cl);

void UArray2_map_col_major(UArray2_T uarray2,
                           void apply(int col, int row, UArray2_T uarray2,
                                      void *value, void *cl),
                           void *cl);


