/******************************************************************************
 *
 *                     unblackedges.c
 *
 *     Assignment: iii
 *     Authors:    ihackm01 and nmoren01
 *     Date:       9/21/2023
 *
 *     summary:    unblackedges.c uses the Bit2 implementation to read in a pbm 
 *                 and remove any black edges and any black pixels neighboring 
 *                 the edges.
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "uarray.h"
#include "stack.h"
#include "assert.h"
#include "pnmrdr.h"
#include "bit2.h"

const int MARKED_BIT = 3;

Bit2_T readPbm(FILE *file);
void fill_bit2(int col, int row, Bit2_T bit2, int element, void *rdr);
void unblack(Bit2_T bit2);
void DFS(Bit2_T bit2, int col, int row, int width, int height);
void stack_and_check(Bit2_T bit2, Stack_T black_stack, int *ptr, int w, int h);
void add_neighbor_to_stack(Bit2_T bit2, Stack_T stack, int col, int row);
bool isBlack(Bit2_T bit2, int col, int row);
void turn_marked_to_white(int col, int row, Bit2_T bit2, int bit, void *cl);
void pbmWrite(int col, int row, Bit2_T bit2, int element, void *cl);
void assert_bit2_col_row(Bit2_T bit2, int col, int row);

int main(int argc, char *argv[]) {
        assert(argc <= 2);
        FILE *file = NULL;

        /*check for valid input*/
        if (argc == 1)    {
                file = stdin;
        } else    {
                file = fopen(argv[1], "r");
                assert(file != NULL);
        }
        
        Bit2_T bit2 = readPbm(file);
        assert(bit2 != NULL);
        fclose(file);
        
        /* this is where the meat happens */
        unblack(bit2);

        printf("P1\n%i %i\n", Bit2_width(bit2), Bit2_height(bit2));
        Bit2_map_row_major(bit2, pbmWrite, NULL);

        Bit2_free(&bit2);

        return 0;
}

/**********readPgm********
 *
 * Reads in the pgm containing the bitmap of the image into the 2D array bit2
 *
 * Inputs: pointer to the pbm file
 *
 * Return: the 2D array bit2 which contains the bitmap of the image. 
 * Expects: the file pointer is not NULL and the pbm type is a bitmap
 *      
 * Notes: Pnmrdr interface is used to read in the pbm file and row 
 *        major is used to assign each index of the bit2 a value. 
 *     
 ************************/
Bit2_T readPbm(FILE *file) 
{
        assert(file != NULL);

        Pnmrdr_T rdr = Pnmrdr_new(file);

        /* check that the type of pbm is a bitmap */
        assert(Pnmrdr_data(rdr).type == Pnmrdr_bit);

        int height = Pnmrdr_data(rdr).height;
        int width = Pnmrdr_data(rdr).width;

        /* check that the width and height are not 0 */
        assert(height != 0 && width != 0 );

        Bit2_T bit2 = Bit2_new(width, height);

        /* traverse uarray2 and assign value at each index */
        Bit2_map_row_major(bit2, fill_bit2, &rdr);

        Pnmrdr_free(&rdr);

        return bit2;
}

/**********fill_bit2********
 *
 * fills given element of 2D array with bit value from a pnmrdr
 *
 * Inputs: integers representing column and row, bit2, pointer to element at 
 *         given index of bit2, and pointer to the Pnmrdr struct
 *
 * Return: nothing
 * Expects: The array, element, and rdr are not NULL
 *      
 * Notes: this is to be passed into the row major map function when reading in 
 *        file
 *     
 ************************/
void fill_bit2(int col, int row, Bit2_T bit2, int element, void *rdr)
{
        (void) element;
        assert_bit2_col_row(bit2, col, row);
        int value = Pnmrdr_get(*(Pnmrdr_T *)rdr);
        Bit2_put(bit2, col, row, value);
}

/**********unblack********
 * removes all black edges from a given bitmap
 *
 * Inputs: a 2D array with the bitmap
 *
 * Return: nothing
 * Expects: bit2 has been initialized and is not NULL
 *      
 * Notes: 
 *      
 ************************/
void unblack(Bit2_T bit2) 
{
        assert(bit2 != NULL);
        int width = Bit2_width(bit2);
        int height = Bit2_height(bit2);
        /* go through first row w/ for loop */
        for (int i = 0; i < width; i++) {
                DFS(bit2, i, 0, width, height);
        }
        /* go through last row */
        for (int i = 0; i < width; i++) {
                DFS(bit2, i, height - 1, width, height);
        }
        /* go through first column */
        for (int i = 0; i < height; i++) {
                DFS(bit2, 0, i, width, height);
        }
        /*go through last column */
        for (int i = 0; i < height; i++) {
                DFS(bit2, width - 1, i, width, height);
        }
        
        /* go through bit2 and turned marked bits to white */
        Bit2_map_row_major(bit2, turn_marked_to_white, NULL);
}

/**********DFS********
 * This is a depth first search function that traverses and marks all black 
 * bits connected to a given edge pixel (think of neighboring pixels as a graph)
 * 
 * Inputs: Pointer to 2D array with bits, integers with column and row of given 
 *          bit and width and height of bitmap
 *
 * Return: Nothing
 *
 * Expects: bit2 has been initialized and that col/row are valid coordinates 
 *          within the bitmap and that user has given the real width and height
 *      
 * Notes: 
 *      This is a beautiful way to get around too many recursive calls on the 
 *      REAL stack
 *     
 ************************/
void DFS(Bit2_T bit2, int col, int row, int width, int height)
{
        assert_bit2_col_row(bit2, col, row);
        /* if the bit is white or it has already been marked then return */
        int first_bit = Bit2_get(bit2, col, row);
        if (first_bit == 0 || first_bit == MARKED_BIT) {
                return;
        }

        Bit2_put(bit2, col, row, MARKED_BIT);
        
        Stack_T black_stack = Stack_new();
        assert(black_stack != NULL);

        /* make int *ptr w/ col and row at index 0 and 1 */
        int *curr_bit = malloc(sizeof(int) * 2);
        assert(curr_bit != NULL);
        curr_bit[0] = col;
        curr_bit[1] = row;

        /* push ptr to stack */
        Stack_push(black_stack, curr_bit);
       
        /* continue until entire "graph" connected to edge pixel is traversed */
        while (Stack_empty(black_stack) != 1) {
                /* get index of bit at top of stack */
                curr_bit = (int *)Stack_pop(black_stack);
                /* check and stack neighbors (have pseudo-recursive faith) */
                stack_and_check(bit2, black_stack, curr_bit, width, height);

                free(curr_bit);
        }

        Stack_free(&black_stack);
}

/**********stack_and_check********
 * checks neighbors of a given bit and adds to stack if they are black
 * 
 * Inputs: bitmap, stack for neighbors that need to be traversed, int pointer 
 *         with indices of pixel, integers with width and height of bit2
 *
 * Return: Nothing
 *
 * Expects: Bit2 is real and the pointer with index is real
 *      
 * Notes:   checks the nieghbors above, below and to the left and right. 
 *     
 ************************/
void stack_and_check(Bit2_T bit2, Stack_T black_stack, int *ptr, int w, int h)
{       
        assert(black_stack != NULL && ptr != NULL);
        /* get col and row from ptr */
        int col = ptr[0];
        int row = ptr[1];
        
        assert_bit2_col_row(bit2, col, row);

        /* search bottom neighbor (if new index not out of bounds) */
        if (row + 1 < h)    {
                Bit2_get(bit2, col, row  + 1);
                if (isBlack(bit2, col, row + 1)) {
                        add_neighbor_to_stack(bit2, black_stack, col, row + 1);
                }

        }
        
        /* search top neighbor (if new index not out of bounds) */
        if (row - 1 >= 0)    {
                if (isBlack(bit2, col, row - 1)) {
                        add_neighbor_to_stack(bit2, black_stack, col, row - 1);
                }
        }

        /* search right neighbor (if new index not out of bounds) */
        if (col + 1 <= w - 1)    {
                if (isBlack(bit2, col + 1, row)) {
                        add_neighbor_to_stack(bit2, black_stack, col + 1, row);
                }
        }
        
        /* search left neighbor (if new index not out of bounds) */
        if (col - 1 >= 0)    {
                 if (isBlack(bit2, col - 1, row)) {
                        add_neighbor_to_stack(bit2, black_stack, col - 1, row);
                }
        }
}

/**********add_neighbor_to_stack********
 *
 * Adds the neighboring black pixel of an edge to the stack of edge pixels.
 *
 * Inputs: the bit2 array, the edge stack, and the current row and column
 *
 * Return: nothing
 * Expects: The array is not NULL, the stack is not NULL and the row and col 
 *          are between 0 and the total width and height.
 *      
 * Notes: Marks the neighbors and pushes it onto the stack. 
 *     
 ************************/
void add_neighbor_to_stack(Bit2_T bit2, Stack_T stack, int col, int row)
{
        assert(stack != NULL);
        assert_bit2_col_row(bit2, col, row);
        /* mark neighbor as visited*/
        Bit2_put(bit2, col, row, MARKED_BIT);

        /* push neighbor to stack */
        int *add_to_stack = malloc(sizeof(int) * 2);
        assert(add_to_stack != NULL);
        add_to_stack[0] = col;
        add_to_stack[1] = row;
        Stack_push(stack, add_to_stack);
}

/**********isBlack********
 *
 * Check if the current bit is black or 1
 *
 * Inputs: the bit2 array and the current row and column
 *
 * Return: nothing
 * Expects: The array is not NULL and row and column are between 0 and the 
 *          total height and width.
 *      
 * Notes: Helper funciton used in stack_and_check
 *     
 ************************/
bool isBlack(Bit2_T bit2, int col, int row) 
{
        assert_bit2_col_row(bit2, col, row);
        /* returns true is the pixel is black */
        return (Bit2_get(bit2, col, row) == 1);
}


/**********turn_marked_to_white********
 *
 * traverse the array and convert each parked pixel with value 3 to white, 0. 
 *
 * Inputs: integer of the column, row, the bit2 array, the current element and 
 *         closure pointer.
 *
 * Return: nothing
 * Expects: The array is not NULL, the row and column is between 0 and the 
 *          total height and width and the element is 0 or 1.
 *      
 * Notes: This function runs after all edge pixels and their neighbors have 
 *        been traversed and identified. 
 *     
 ************************/
void turn_marked_to_white(int col, int row, Bit2_T bit2, int bit, void *cl)    
{
        assert_bit2_col_row(bit2, col, row);
        (void) cl;
        if (bit == MARKED_BIT)    {
                Bit2_put(bit2, col, row, 0);
        }
}

/**********pbmWrite********
 *
 * print the elements of the bitmap to standard output
 *
 * Inputs: integer of the column, row, the bit2 array, th ecurrent element and 
 *         closure pointer.
 *
 * Return: nothing
 * Expects: The array is not NULL, the row and column is between 0 and the 
 *          total height and width and the element is 0 or 1.
 *      
 * Notes: pbmWrite outputs the bitmap to standard output with no whitespaces 
 *        between the bits and a newline after each row. 
 *     
 ************************/
void pbmWrite (int col, int row, Bit2_T bit2, int element, void *cl)
{
        assert(bit2 != NULL);
        /* assert no elements are still marked as 3 */
        assert(element == 0 || element == 1);
        (void) row;
        (void) cl;
        /* check if reached the end of a row */
        if (col == Bit2_width(bit2) - 1) {
                printf("%i\n", element);
        } 
        else {
                printf("%i", element);
        }
}

/**********assert_bit2_col_row********
 *
 * asserts that the bit2 array is not null and that the col and row are between
 * 0 and the height and width.
 *
 * Inputs: The bit2 array and the column and row number.
 *
 * Return: nothing
 * Expects: The array is not NULL and the row and column is between 0 and the 
 *          total height and width.
 *      
 * Notes: Since most functions in unblackedges have bit2, row and col as 
 *        parameters, this helper function is called for modularity.
 *     
 ************************/
void assert_bit2_col_row(Bit2_T bit2, int col, int row)
{
        /* check that the array is not empty */
        assert(bit2 != NULL);
        /* checks that the column is not negative and not equal to or greater 
           than the total width and height */
        assert(col >= 0 && row >= 0);
        assert(col < Bit2_width(bit2) && row < Bit2_height(bit2));
}
