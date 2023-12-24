#ifndef PPMTRANS_H
#define PPMTRANS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "mem.h"
#include "cputiming.h"

typedef struct col_row col_row;
typedef struct index_mapping index_mapping;
typedef A2Methods_UArray2 A2;
typedef col_row (*trans_func)(int col, int row, A2 array,
                  A2Methods_T methods);

FILE *parse_command_line(int argc, char *argv[], A2Methods_T *methods_ptr, 
                         A2Methods_mapfun **map, char **time_file_name,
                         int *rotation, int *j);

trans_func get_rotate_flip_transpose(int rotation);

void map_to_new_array(int col, int row, A2 array, void *elem, void *cl);

col_row rotate_90(int col, int row, A2 array, A2Methods_T methods);

col_row rotate_180(int col, int row, A2 array, A2Methods_T methods);

col_row rotate_270(int col, int row, A2 array, A2Methods_T methods);

col_row rotate_zero(int col, int row, A2 array, A2Methods_T methods);

col_row flip_horizontal(int col, int row, A2 array, A2Methods_T methods);

col_row flip_vertical(int col, int row, A2 array, A2Methods_T methods);

col_row flip_transpose(int col, int row, A2 array, A2Methods_T methods);

void assert_col_row(int col, int row, A2 array, A2Methods_T methods);

void write_to_time_file(CPUTime_T timer, A2Methods_T methods, FILE *timing_fp, 
                        A2 pixels, int argc, char *argv[], int i);

index_mapping *make_mapped_image(trans_func transformation_function,
                                 A2 pixels, A2Methods_T methods);

void swap_arrays(Pnm_ppm pixmap, A2Methods_T methods, 
                 index_mapping *mapped_image);

void free_and_close(Pnm_ppm pixmap, FILE *fp, FILE *timing_fp, CPUTime_T timer,
                    index_mapping *mapped_image);

#endif /* PNMTRANS_H */