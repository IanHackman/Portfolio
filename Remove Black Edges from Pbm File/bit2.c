/******************************************************************************
 *
 *                     bit2.c
 *
 *     Assignment: iii
 *     Authors:    ihackm01 and nmoren01
 *     Date:       9/21/2023
 *
 *     summary:    bit2.c contains the function definitions for the bit2 
 *                 interface,
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "bit2.h"
#include "uarray.h"
#include "assert.h"
#define T Bit2_T

struct T {
        int width;
        int height;
        int size;
        UArray_T array;
};

/**********bi2_new********
 *
 * Initializes and allocates memory for a 2D bit array of given width and height
 *
 * Inputs: integers of width and height to multiply and get length
 *
 * Return: Pointer to new Bit2 struct
 * Expects: Width, height and size are larger than 0.
 *      
 * Notes:
 *     
 ************************/
T Bit2_new(int width, int height) 
{
        assert(width > 0 && height > 0);
        
        T Bit2 = malloc(sizeof(*Bit2));
       
        assert(Bit2 != NULL);

        Bit2->width = width;
        Bit2->height = height;

        int length = width * height;
        Bit2->array = UArray_new(length, sizeof(int));

        return Bit2;
}

/**********bi2_map_col_major********
 *
 * applies the apply function to each element column by column
 *
 * Inputs: Pointer to Bit2 struct to access array elements, void pointer to 
 *         apply function to apply to all elements, pointer for closure
 *
 * Return: Nothing
 * Expects: a real Bit2 array (not NULL) and a real pointer to apply (not NULL)
 *      
 * Notes:
 *     
 ************************/
void Bit2_map_col_major(T Bit2, void apply(int col, int row, Bit2_T a, 
                        int b, void *p1), void *cl)
{
        assert(Bit2 != NULL && apply != NULL);
        int width = Bit2_width(Bit2);
        int height = Bit2_height(Bit2);

        for (int i = 0; i < width; i++) {
                for (int j = 0; j < height; j++) {
                        /*get bit here*/
                        int b = Bit2_get(Bit2, i, j);
                        apply(i, j, Bit2, b, cl);
                }
        }
}

/**********bi2_map_row_major********
 *
 * applies the apply function to each element row by row
 *
 * Inputs: Pointer to Bit2 struct to access array elements, void pointer to 
 *         apply 
 *
 * Return:  Nothing
 * Expects: a real Bit2 array (not NULL) and a real pointer to apply (not NULL)
 *      
 * Notes:
 *     
 ************************/
void Bit2_map_row_major(T Bit2, void apply(int col, int row, Bit2_T a, 
                        int b, void *p1), void *cl)
{
        assert(Bit2 != NULL && apply != NULL);
        int width = Bit2_width(Bit2);
        int height = Bit2_height(Bit2);

        for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                        int b = Bit2_get(Bit2, j, i);
                        apply(j, i, Bit2, b, cl);
                }
        }
}

/**********bi2_put********
 *
 * 
 * Inputs: Pointer to Bit2 struct to access array elements, integers of the 
 *         height and width, integer with value to be put at specified index
 *
 * Return: Integer value of bit previously at given location 
 *
 * Expects: Index  to be within a valid range (Greater than zero and less width/
 *          height) and that the array has been initialized (not NULL)
 *      
 * Notes: Unfilled arrays will return 0 when "put"ing for the first time
 *     
 ************************/
int Bit2_put(T Bit2, int col,  int row, int value)
{
        assert(Bit2 != NULL && col >= 0 && row >= 0);
        assert(col < Bit2->width && row < Bit2->height);

        int prev = Bit2_get(Bit2, col, row);

        int *pos = UArray_at(Bit2->array, (row * Bit2->width) + col);
        *pos = value;

        return prev;
}

/**********bi2_get********
 *
 * Retrieve the bit at specified column and row
 * 
 * Inputs: Pointer to Bit2 struct to access array elements, integers of the 
 *         height and width.
 *
 * Return: Pointer to the bit in the specified index.
 * Expects: The column and row are greater than or equal to 0 and that the Bit2 
 *          array exists (not NULL)
 *      
 * Notes:
 *     
 ************************/
int Bit2_get(T Bit2, int col, int row)
{
        assert(Bit2 != NULL && col >= 0 && row >= 0);
        assert(col < Bit2->width && row < Bit2->height);
        return *(int *)UArray_at(Bit2->array, (row * Bit2->width) + col);
}

/**********bi2_width********
 *
 * Returns the width of the Bit2 array.
 * 
 * Inputs: Pointer to Bit2 struct to get width element.
 *
 * Return: The width in an integer.
 * Expects: The Bit2 array exists (not NULL).
 *      
 * Notes:
 *     
 ************************/
int Bit2_width(T Bit2)
{
        assert(Bit2 != NULL);
        return (Bit2->width);
}

/**********bi2_height********
 *
 * Returns the height of the Bit2 array.
 * 
 * Inputs: Pointer to Bit2 struct to get height element.
 *
 * Return: The height in an integer.
 * Expects: The Bit2 array exists (not NULL).
 *      
 * Notes:
 *     
 ************************/
int Bit2_height(T Bit2)
{
        assert(Bit2 != NULL);
        return (Bit2->height);
}

/**********Bit2_size********
 *
 * Gets size of the 2D array
 * 
 * Inputs: Pointer to the Bit2 struct.
 *
 * Return: The size of the objects stored in bit2 as an integer.
 * Expects: Bit2 to be initialized (not NULL).
 *      
 * Notes: 
 *     
 ************************/
int Bit2_size(T Bit2) 
{
        assert(Bit2 != NULL);
        return (Bit2->size);
}

/**********bi2_free********
 *
 * Frees all memory allocated for Bit2.
 * 
 * Inputs: Pointer to pointer to Bit2 struct array element to be freed.
 *
 * Return: Nothing.
 * Expects: Provided Bit2 to be initialized (not NULL).
 *      
 * Notes:
 *     
 ************************/
void Bit2_free(T *Bit2)
{
        T test = *(Bit2);
        assert(test->array != NULL && Bit2 != NULL);
        UArray_free(&test->array); 
        free(*Bit2);
}

#undef T