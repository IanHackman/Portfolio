#include <string.h>

#include <a2plain.h>
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

typedef A2Methods_UArray2 A2;

/********** new ********
 *
 * Creates a new 2d array of a given width and height, containing elements
 * of a given size in bytes.
 *
 *
 * Parameters:
 *      int width:      number of columns in the desired 2d array
 *      int height:     number of rows in the desired 2d array
 *      int size:       number of bytes in each element
 *
 * Return:
 *      Pointer to newly created UArray2_T
 *
 * Notes: all exception hndling is done in UArray2.c
 *      
 ************************/
static A2Methods_UArray2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}

/********** new_with_blocksize ********
 *
 * Creates a new 2d array of a given width and height, containing elements
 * of a given size in bytes. Since UArray2 does not support blocksize, 
 * blocksize is ignored.
 *
 *
 * Parameters:
 *      int width:      number of columns in the desired 2d array
 *      int height:     number of rows in the desired 2d array
 *      int size:       number of bytes in each element
 *      int blocksize:  refers to the sidelength of a single block in 2d array
 *
 * Return:
 *      Pointer to newly created UArray2_T
 *
 * Notes: all exception hndling is done in UArray2.c. Since blocksize is 
 * ignored, blocksize is void.
 *      
 ************************/
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size,
                                            int blocksize)
{
        (void) blocksize;
        return UArray2_new(width, height, size);
}

/********** a2_free ********
 *
 * Frees memory allocated to a UArray2_T 
 *
 *
 * Parameters:
 *      UArray2b_T *array2b :  Pointer to an A2 array that needs to be freed
 *
 * Return: void
 *
 * Notes:
 *      CRE if array2p or *array2p is NULL
 *
 ************************/
static void a2free(A2 *array2p)
{
        assert(array2p != NULL);
        assert(*array2p != NULL);

        UArray2_free((UArray2_T *) array2p);
}

/********** width ********
 *
 * Gets the width of a, A2 array2 UArray2
 *
 *
 * Parameters:
 *      A2 array2:  A2 UArray2 to get width of
 *
 * Return: (int) width of uarrayb2
 *
 * Notes: exception handling done in UArray2.c
 ************************/
static int width(A2 array2)
{
        return UArray2_width(array2);
}

/********** height ********
 *
 * Returns the height of a given 2d array.
 *
 * Parameters:
 *      A2 array2:      UArray2 2d array
 *
 * Return:
 *      int:    height of array2.
 *
 ************************/
static int height(A2 array2)
{
        return UArray2_height(array2);
}

/********** size ********
 *
 * Returns the size of the elements of a given 2d array, in bytes.
 *
 * Parameters:
 *      A2 array2:      UArray2 2d array whose element size we want
 *
 * Return:
 *      int:    size of the elements in the provided 2d array
 *
 ************************/
static int size(A2 array2)
{
        return UArray2_size(array2);
}

/********** blocksize ********
 *
 * Returns the blocksize of a given 2d array. In this case, returns 1
 * because the underlying 2d array is not blocked.
 *
 * Parameters:
 *      A2 array2:      UArray2 2d array
 *
 * Return:
 *      int:    1 (unblocked 2d arrays have a blocksize of 1)
 *
 ************************/
static int blocksize(A2 array2)
{
        (void) array2;
        return 1;
}

/********** at ********
 *
 * Returns a pointer to an element in a 2d array at a given index
 *
 *
 * Parameters:
 *      A2 array2:      2d array whose element address we are returning
 *      int i    :      column index of element accessed
 *      int j    :      row index of element accessed
 *
 *
 * Return:
 *      Void pointer to element at array2[i][j]
 *
 ************************/
static A2Methods_Object *at(A2 array2, int i, int j)
{
        return UArray2_at(array2, i, j);
}

typedef void applyfun(int i, int j, UArray2_T array2, void *elem, void *cl);

static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_row_major(uarray2, (applyfun*)apply, cl);
}

static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (applyfun*)apply, cl);
}

struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};

static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
        width,
        height,
        size,
        blocksize,
        at,
        map_row_major,         
        map_col_major,
        NULL,                   /* map_block_major */
        map_row_major,          /* map_default */
        small_map_row_major,
        small_map_col_major,
        NULL,                   /* small_map_block_major */
        small_map_row_major,    /* small map_default */
};

// finally the payoff: here is the exported pointer to the struct

A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
