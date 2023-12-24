/******************************************************************************
 *
 *                     uarray2.c
 *
 *     Assignment: iii
 *     Authors:    ihackm01 and nmoren01
 *     Date:       9/21/2023
 *
 *     summary:    uarray2.c contains the function definitions for the uarray2 
 *                 interface.
 *
 *****************************************************************************/

/**********UArray2_new********
 *
 * Initializes and allocates memory for a 2D Uarray of given width and height
 * 
 * Inputs: Integer values of width and height to multiple and get the length og.
 * Integer representing the size 
 *(in bytes) of the objects to bbe stored
 *
 * Return: Pointer to new UArray2 struct
 * Expects: Width and height are larger than 0
 *      
 * Notes:
 *     
 ************************/
#include <stdio.h>
#include <stdlib.h>
#include "uarray.h"
#include "uarray2.h"
#include "assert.h"
#define T UArray2_T

struct T {
        int width;
        int height;
        int size;
        UArray_T array;
};

/**********uarray2_new********
 *
 * Initializes and allocates memory for a 2D bit array of given width and 
 * height and size
 *
 * Inputs: integers of width and height to multiply and get length and integer 
 * of the size to malloc array.
 *
 * Return: Pointer to new UArray2 struct
 * Expects: Width, height and size are larger than 0
 *      
 * Notes:
 *     
 ************************/
T UArray2_new(int width, int height, int size) 
{
        assert(width > 0 && height > 0 && size > 0);
        
        T uarray2 = malloc(sizeof(*uarray2));
       
        assert(uarray2 != NULL);

        uarray2->width = width;
        uarray2->height = height;
        uarray2->size = size;

        int length = width * height;
        uarray2->array = UArray_new(length, size);

        return uarray2;
}

/**********UArray2_map_col_major********
 *
 * Applies the apply function to each element column by column.
 * 
 * Inputs: Pointer to UArray2 struct and function to apply to all elements.
 *
 * Return: Nothing
 * Expects: a real uarray2 (not NULL) and a real pointer to apply 
 *          (not NULL)
 *      
 * Notes:
 *     
 ************************/
void UArray2_map_col_major(T uarray2, void apply(int col, int row, UArray2_T a, 
                           void *p1, void *p2), void *cl)    
{
        assert(uarray2 != NULL && apply != NULL && cl != NULL);
        int width = UArray2_width(uarray2);
        int height = UArray2_height(uarray2);
        
        void *element = NULL;
        for (int i = 0; i < width; i++) {
                for (int j = 0; j < height; j++) {
                        element = UArray2_at(uarray2, i, j);
                        apply(i, j, uarray2, element, cl);
                }
        }
}

/**********UArray2_map_row_major********
 *
 * Applies the apply function to each element row by row.
 * 
 * Inputs: Pointer to UArray2 struct and function to apply to all elements.
 *
 * Return: Nothing
 * Expects: A real uarray2 (not NULL) and a real pointer to apply (not NULL).
 *      
 * Notes:
 *     
 ************************/
void UArray2_map_row_major(T uarray2, void apply(int col, int row, UArray2_T a,
                           void *p1, void *p2), void *cl)    
{
        assert(uarray2 != NULL && apply != NULL && cl != NULL);
        int width = UArray2_width(uarray2);
        int height = UArray2_height(uarray2);

        void *element = NULL;
        for (int i = 0; i < height; i++)    {
                for (int j = 0; j < width; j++)    {
                        element = UArray2_at(uarray2, j, i);
                        //printf("Element: %i\n", *(int *)element);
                        apply(j, i, uarray2, element, cl);
                }
        }
}

/**********UArray2_at********
 *
 * Retrieves a pointer to the element stored at given index.
 * 
 * Inputs: Pointer to UArray2 struct, integer representing column to get from, 
 *         integer representing row to get from.
 *
 * Return: Pointer to value at given index.
 * Expects: Provided UArray2 to be initialized (not NULL).
 *      
 * Notes:
 *     
 ************************/
void *UArray2_at(T uarray2, int col, int row) 
{
        assert(uarray2 != NULL && col >= 0 && row >= 0);
        assert(col < uarray2->width && row < uarray2->height);
        return UArray_at(uarray2->array, (row * uarray2->width) + col);
}

/**********UArray2_free********
 *
 * Frees all memory allocated for UArray2.
 * 
 * Inputs: Pointer to the pointer to the struct that will be freed.
 *
 * Return: Nothing.
 * Expects: Provided UArray2 to be initialized (not NULL).
 *      
 * Notes:
 *     
 ************************/
void UArray2_free(T *uarray2) 
{       
        T test = *(uarray2);
        assert(test->array != NULL && uarray2 != NULL);
        UArray_free(&test->array); 
        free(*uarray2);
}

/**********UArray2_width********
 *
 * Retrieve the width of the 2D array.
 * 
 * Inputs: Pointer to the UArray2 struct.
 *
 * Return: The width in an integer.
 * Expects: Provided UArray2 to be initialized (not NULL).
 *      
 * Notes:
 *     
 ************************/
int UArray2_width(T uarray2)
{
        assert(uarray2 != NULL);
        return uarray2->width;
}

/**********UArray2_height********
 *
 * Gets height of the 2D array
 * 
 * Inputs: Pointer to the UArray2 struct.
 *
 * Return: The height in an integer.
 * Expects: Provided UArray2 to be initialized (not NULL).
 *      
 * Notes: 
 *     
 ************************/
int UArray2_height(T uarray2) 
{
        assert(uarray2 != NULL);
        return uarray2->height;
}

/**********UArray2_size********
 *
 * Gets size of the 2D array
 * 
 * Inputs: Pointer to the UArray2 struct.
 *
 * Return: The size in an integer.
 * Expects: Provided UArray2 to be initialized (not NULL).
 *      
 * Notes: 
 *     
 ************************/
int UArray2_size(T uarray2)
{
        assert(uarray2 != NULL);
        return uarray2->size;
}

#undef T