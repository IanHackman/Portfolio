#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "uarray2b.h"
#include "uarray.h"
#include "mem.h"
#include "assert.h"
#define T UArray2b_T


/* Function Declarations (for private helper functions) */
void apply_to_block(T array2b, int tl_col, int tl_row,
                                                    void apply(int col, int row,
                                                    T array2b, void *elem,
                                                    void *cl),
                                                    void *cl);
int get_index(T array2b, int column, int row);
int dimension_prime(int dimension, int blocksize);

/********* struct UArray2b_T ********
 *
 * Each instance represents an instance of the UArray2b interface.
 * The 2d arrays have a width and height, a size (representing the size of an 
 * element in bytes).
 *
 * Each array also has an underlying UArray_T. The 2d arrays map to the 1d array
 * in row major order within each block, and block major order overall.
 * An equation representing conversion from 2d indices to 1d indices can be
 * found in get_index()
 *
 ************************/
struct T {
        int width;
        int height;
        int size;
        int blocksize;
        UArray_T arr;
};

/********** UArray2b_new ********
 *
 * Creates a new 2d array of a given width and height, containing elements
 * of a given size in bytes, with a given blocksize. Bytes in each element are 
 * initialized to zero.
 *
 *
 * Parameters:
 *      int width:      number of columns in the desired 2d array
 *      int height:     number of rows in the desired 2d array
 *      int size:       number of bytes in each element
 *      int blocksize:  refers to the sidelength of a single block in 2d array
 *
 * Return:
 *      Pointer to newly created UArray2b_T
 *
 * Notes:
 *      The caller of UArray2b_new is responsibile for freeing the memory
 *      allocated to the new array using UArray2b_free()
 *
 *      CRE if width or height is negative
 *      CRE if size is non-positive
 *      CRE if blocksize is non-positive
 *      CRE if UArray failed to initialize (i.e. if ALLOC throws a CRE)
 ************************/
T UArray2b_new (int width, int height, int size, int blocksize)
{
        assert(width >= 0 && height >= 0);
        assert(size > 0 && blocksize > 0);
        
        /* initialize a UArray2b_T struct */
        T uarray2b;
        NEW(uarray2b);
        assert(uarray2b != NULL);
        
        /* assign value to each element in uarray2b */
        uarray2b->width = width;
        uarray2b->height = height;
        uarray2b->size = size;
        uarray2b->blocksize = blocksize;
        
        /* allocate a UArray to represent UArray2b */
        uarray2b->arr = UArray_new(dimension_prime(width, blocksize) 
                                   * dimension_prime(height, blocksize), size);

        assert(uarray2b->arr != NULL);

        return uarray2b;
}

/********** UArray2b_new_64K_block ********
 *
 * Creates a new 2d array of a given width and height, containing elements
 * of a given size in bytes. Bytes in each element are initialized to zero.
 * Blocksize is maximized without going over 64KB.
 *
 *
 * Parameters:
 *      int width:      number of columns in the desired 2d array
 *      int height:     number of rows in the desired 2d array
 *      int size:       number of bytes in each element
 *
 * Return:
 *      Pointer to newly created UArray2b_T
 *
 * Notes:
 *      The caller is responsibile for freeing the memory
 *      allocated to the new array using UArray2b_free()
 *
 *      CRE if width or height is negative
 *      CRE if size is non-positive
 *      CRE if UArray2b_new throws an error
 ************************/
T UArray2b_new_64K_block(int width, int height, int size)
{
        assert(width >= 0 && height >= 0);
        assert(size > 0);
        
        /* get the maximum dimension of the Urray2b to be allocated */
        int max_width_height;
        if (width > height) {
                max_width_height = width;
        }
        else {
                max_width_height = height;
        }
        
        int blocksize;
        if (size > 65536) {
                /* if the size is greater than blocksize */
                blocksize = 1;
        }
        else {
                blocksize = max_width_height;
                if (blocksize * blocksize * size > 65536) {
                        /* calculate the maximun blocksize */
                        blocksize = floor(sqrt(65536 / (float)size));
                }
        }
        /* create UArray2b with maximum blocksize */
        return UArray2b_new(width, height, size, blocksize);
}

/********** UArray2b_free ********
 *
 * Frees memory allocated to a UArray2b_T 
 *
 *
 * Parameters:
 *      UArray2b_T *array2b :  Pointer to an array2b that needs to be freed
 *
 * Return: void
 *
 * Notes:
 *      The caller relinquishes ownership of array2b
 *      CRE if uarray2b is NULL;
 *      CRE if *uarray2b is NULL;
 ************************/
void UArray2b_free (T *array2b)
{
        assert(array2b != NULL);
        assert(*array2b != NULL);
        
        /* free the UArray then the UArray_T struct */
        UArray_free(&((*array2b)->arr));
        FREE(*array2b);
}

/********** UArray2b_width ********
 *
 * Gets the width of a UArray2b_T
 *
 *
 * Parameters:
 *      T array2b:  UArray2b_T struct to get the width from
 *
 * Return: (int) width of uarrayb2
 *
 * Notes:
 *      CRE if array2b NULL;
 ************************/
int UArray2b_width (T array2b)
{
        assert(array2b != NULL);
        return (array2b->width);
}

/********** UArray2b_height ********
 *
 * Gets the height of a UArray2b_T
 *
 *
 * Parameters:
 *      T array2b:  UArray2b_T struct to get the height from
 *
 * Return: (int) height of uarrayb2
 *
 * Notes:
 *      CRE if array2b is NULL;
  ************************/
int UArray2b_height (T array2b)
{
        assert(array2b != NULL);
        return (array2b->height);
}

/********** UArray2b_size ********
 *
 * Gets the size of the elements in a UArray2b_T
 *
 *
 * Parameters:
 *      T array2b :  2d array to the get the element size from
 *
 * Return: (int) size of elems in uarray2b
 *
 * Notes:
 *      CRE if array2b is NULL;
 ************************/
int UArray2b_size (T array2b)
{
        assert(array2b != NULL);
        return (array2b->size);
}

/********** UArray2b_blocksize ********
 *
 * Gets the blocksize of a UArray2b_T
 *
 *
 * Parameters:
 *      T array2b :  2d array to the get blocksize from
 *
 * Return: (int) blocksize of uarray2b
 *
 * Notes:
 *      CRE if array2b is NULL;
 ************************/
int UArray2b_blocksize(T array2b)
{
        assert(array2b != NULL);
        return (array2b->blocksize);
}

/********** UArray2b_at ********
 *
 * Returns a pointer to an element of the 2d array
 *
 *
 * Parameters:
 *      T array2b  :  2d array to get element access from
 *      int column :  column index of relevent element
 *      int row    :  row index of relevent element
 *
 * Return: void pointer to element at index (column, row) of array2b
 *
 * Notes:
 *      CRE if array2b is NULL;
 *      CRE if index is out of range;
 ************************/
void *UArray2b_at(T array2b, int column, int row)
{
        assert(array2b != NULL);
        assert(column >= 0 && row >= 0);
        assert(column < array2b->width && row < array2b->height);

        /* get index and return it */
        int index = get_index(array2b, column, row);

        return UArray_at(array2b->arr, index);
}

/********** UArray2b_map ********
 *
 * Calls apply function on every element of the 2d array in block major order
 *
 *
 * Parameters:
 *      T array2b  :  2d array to traverse
 *      void apply :  function pointer of apply function which will be called 
 *                    on each element
 *      void *cl   :  void pointer to something possibly used by apply
 *
 * Return: None
 *
 * Notes:
 *      CRE if array2b is NULL;
 *      CRE if apply is NULL;
 *      Utilizes apply_to_block helper function
 ************************/
void UArray2b_map(T array2b, void apply(int col, int row, T array2b, 
                                               void *elem, void *cl),
                                               void *cl)
{
        assert(array2b != NULL && apply != NULL);
        
        /* traverse through UArray2b row by row in each block then block by 
         * block through each row of blocks 
         */
        for (int row = 0; row < array2b->height; row += array2b->blocksize) {
                for (int col = 0; col < array2b->width; col +=
                                                        array2b->blocksize) {
                        apply_to_block(array2b, col, row, apply, cl);
                }
        }
}

/********** apply_to_block ********
 *
 * Calls apply function on every element within one block of 2d array
 *
 *
 * Parameters:
 *      T array2b  :  2d array to traverse
 *      int tl_col :  top left column index of block
 *      int tl_row :  top left row index of block
 *      void apply :  function pointer of apply function which will be called 
 *                    on each element
 *      void *cl   :  void pointer to something possibly used by apply
 *
 * Return: None
 *
 * Notes:
 *      CRE if array2b is NULL;
 *      CRE if apply is NULL;
 *      Helper function to UArray2b_map()
 ************************/
void apply_to_block(T array2b, int tl_col, int tl_row,
                    void apply(int col, int row, T array2b,
                               void *elem, void *cl),
                    void *cl)
{
        assert(array2b != NULL && apply != NULL);

        /*
         * Loops through elements within given block. Stops at end of block or
         * or end of initialized portion of array
         */
        for (int r = tl_row; 
                             r < array2b->height && 
                             r < (tl_row + array2b->blocksize); r++) {
                if (tl_row == 0) {

                }
                for (int c = tl_col; c < (tl_col + array2b->blocksize)
                                  && c < array2b->width; c++) {
                        void *element = UArray2b_at(array2b, c, r);
                        assert(element != NULL);
                        apply(c, r, array2b, element, cl);
                }                                              
        }
}

/********** get_index ********
 *
 * Gets the index for an element in UArray2b given its column and row index in 
 * the 2d representation
 *
 *
 * Parameters:
 *      T array2b:     2d array to traverse
 *      int column :  column index of relevent element
 *      int row    :  row index of relevent element
 *
 * Return: the index of the element in the UArray for UArray2b 
 *
 * Notes:
 *      CRE if the array is NULL and if the col and row are either less thn 
 *      zero or greater than or equal to the width and or height
 ************************/
int get_index(T array2b, int column, int row)
{
        assert(array2b != NULL);
        assert(column >= 0 && row >= 0);
        assert(column < array2b->width && row < array2b->height);
        
        int blocksize = array2b->blocksize;
        /*
         * Gets the index within the 1-dimensional array, given the 2d array
         * column and row indices, and the blocksize.
         */
        int index = ((row + 1) / blocksize)
                  * dimension_prime(array2b->width, blocksize) * blocksize;
        index += ((column + 1) / blocksize) * blocksize * blocksize;
        index += ((row % blocksize) * blocksize) + (column % blocksize);

        return index;
}

/********** dimension_prime ********
 *
 * Gets full length, width or length, of a dimension of UArray2b including the 
 * uninitialized height and width elements.
 *
 *
 * Parameters:
 *      int dimension:    the length of the dimension of UArray2b
 *      int blocksize:    refers to the sidelength of a single block in 2d array
 *
 * Return: (int) the total length of the UArray2b including unitinialized 
 *         elements
 *
 * Notes:
 *      CRE is dimension is zero or blocksize is zero.
 ************************/
int dimension_prime(int dimension, int blocksize)
{
        assert(dimension > 0 && blocksize > 0);
        /* calculates total size of array with uninitialized elements */
        return (dimension + (blocksize - (dimension % blocksize)));
}

#undef T