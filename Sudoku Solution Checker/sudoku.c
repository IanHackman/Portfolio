/******************************************************************************
 *
 *                     sudoku.c
 *
 *     Assignment: iii
 *     Authors:    ihackm01 and nmoren01
 *     Date:       9/21/2023
 *
 *     summary:    sudoku.c uses the uarray2 interface to read in a sudoku pgm 
 *                 and analyze if it is a valid sudoku solution or not.
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "uarray2.h"
#include "assert.h"
#include "pnmrdr.h"
#include <stdbool.h>

const int MAXVAL = 9;

struct checker {
        int *arr;
        int counter;
        bool correct;
};
typedef struct checker checker;

UArray2_T readPgm(FILE *file);

bool check_sudoku(UArray2_T uarray2);
void check(int col, int row, UArray2_T a, void *element, void *sudoku_checker);
void initialize_array(int *array);
void fill_uarray2(int col, int row, UArray2_T a, void *element, void *rdr); 

void print_values(int col, int row, UArray2_T a, void *element, void *cl);

int main(int argc, char *argv[]) 
{
        assert(argc <= 2);
        FILE *file = NULL;

        /*check for valid input*/
        if (argc == 1)    {
                file = stdin;
        } else    {
                file = fopen(argv[1], "r");
                assert(file != NULL);
        }
        
        /*read sudoku tabble into 2D Uarray*/
        UArray2_T uarray2 = readPgm(file);
        fclose(file);
        if (uarray2 == NULL)    {
                return EXIT_FAILURE;
        }

        bool test = check_sudoku(uarray2);
        UArray2_free(&uarray2);
        /* returns the result of the sudoku checker */
        if (!test)    {
                return EXIT_FAILURE;
        }
        
        return EXIT_SUCCESS;
}

/**********readPgm********
 *
 * Reads in the pgm containing the sudoku puzzle into the 2D array uarray2
 *
 * Inputs: pointer to the pgm file
 *
 * Return: the 2D array uarray2 which contains the sudoku puzzle
 * Expects: the file pointer is not NULL and the pgm type is plain or graymap.
 *      
 * Notes: Pnmrdr interface is used to read in the plain P2 pgm file and row 
 *        major is used to assign each index of the uarray2 a value. 
 *     
 ************************/
UArray2_T readPgm(FILE *file)
{
        assert(file != NULL);

        Pnmrdr_T rdr = Pnmrdr_new(file);

        /* check that the type of pgm is Plain or 'P2'*/
        assert(Pnmrdr_data(rdr).type == Pnmrdr_gray);

        int height = Pnmrdr_data(rdr).height;
        int width = Pnmrdr_data(rdr).width;
        int maxVal = Pnmrdr_data(rdr).denominator;

        /* check that the pgm has the correct sudoku dimensions */
        if (height != MAXVAL || width != MAXVAL || maxVal != MAXVAL)    {
                return NULL;
        }
        
        UArray2_T uarray2 = UArray2_new(9, 9, sizeof(int));
        
        /* traverse uarray2 and assign value at each index */
        UArray2_map_row_major(uarray2, fill_uarray2, &rdr);
        
        Pnmrdr_free(&rdr);

        return uarray2;
}

/**********fill_uarray2********
 *
 * fills given element of 2D array with value from a pnmrdr
 *
 * Inputs: integers representing column and row, uarray2, pointer to element at 
 *         given index of uarray2, and pointer to the Pnmrdr struct w/ file
 *
 * Return: nothing
 * Expects: The array, element, and rdr are not NULL
 *      
 * Notes: this is to be passed into the row major map function when reading in 
 *        file
 *     
 ************************/
void fill_uarray2(int col, int row, UArray2_T a, void *element, void *rdr) 
{
        (void) col;
        (void) row;
        assert(a != NULL && element != NULL && rdr != NULL);
        int value = Pnmrdr_get(*(Pnmrdr_T *)rdr);
        (*(int *)element) = value;
}

/**********initialize_array********
 *
 * Reset all the elements of the checker array to 0.
 *
 * Inputs: pointer to int array
 *
 * Return: nothing
 * Expects: The array is not NULL
 *      
 * Notes: This is called every time after a row or column has been checked to 
 *        clear he array to the next row or column can be checked. 
 *     
 ************************/
void initialize_array(int *array) 
{
        assert(array != NULL);
        /*change all values in array to 0*/
        for (int i = 0; i < MAXVAL; i++)    {
                array[i] = 0;
        }
}

/**********check_sudoku********
 *
 * create the checker struct, call the uarray2 2D array containing the sudoku 
 * and call row and column major functions with the check function passed in.
 *          
 * Inputs: 2D array uarray2 representing sudoku puzzle
 *
 * Return: bool indicating if the sudoku is correct 
 * Expects: The array is not NULL
 *      
 * Notes: The function runs row major first and, if the sudoku is wrong, returns
 *        false before column major is ran. If both are run without the 
 *        checker.counter element being fakse, than true is returned. 
 *     
 ************************/
bool check_sudoku(UArray2_T uarray2)  
{       /*initialize a checker struct and fill*/
        assert(uarray2 != NULL);
        checker sudoku_checker;
        sudoku_checker.counter = 0;
        sudoku_checker.arr = malloc(MAXVAL * sizeof(int));
        sudoku_checker.correct = true;
        initialize_array(sudoku_checker.arr);
        
        /*check each row for duplicate digits*/
        UArray2_map_row_major(uarray2, check, &sudoku_checker);
        if (sudoku_checker.correct == false)    {
                free(sudoku_checker.arr);
                return false;
        }

        /* reset the checker for column major*/
        sudoku_checker.counter = 0;
        initialize_array(sudoku_checker.arr);
        
        /*check each column for duplicate digits*/
        UArray2_map_col_major(uarray2, check, &sudoku_checker);
        if (sudoku_checker.correct == false) {
                free(sudoku_checker.arr);
                return false;
        }
        
        free(sudoku_checker.arr);
        return true;
}

/**********check********
 *
 * apply function to be passed to row and col major, checks for sudoku with 
 * valid solution
 *
 * Inputs: int representing column index,int representing row index, 2D array 
 *         containing sudoku board, pointer to element at given index, struct 
 *         containing counter, array of size 9, and bool to check if sudoku 
 *         board is invalid
 *
 * Return: Nothing
 * Expects: sudoku_checker struct to be initialized and not a NULL pointer,
 *          the element pointer is not NULL and the 2D array is 9x9.
 *          value pointed to by element is greater than 0 and less than 9
 * Notes: check checks if the counter element in the checker struct is 9. If it
 *        is 9, meaning the function has gone through the entire row or column, 
 *        then initialize_array() is called to clear the array so the next row 
 *        or column can be checked.
 *     
 ************************/
void check(int col, int row, UArray2_T a, void *element, void *sudoku_checker) 
{
        (void) col;
        (void) row;
        assert(sudoku_checker != NULL);
        assert(element != NULL);
        assert((UArray2_width(a) == 9) && (UArray2_height(a) == 9));

        /*reset counter if it is full*/
        if (((checker *)sudoku_checker)->counter == 9)    {
                initialize_array(((checker *)sudoku_checker)->arr);
                ((checker *)sudoku_checker)->counter = 0;
        }
        
        /*use value of element to get index in the checker array*/
        int index = (*(int *)element) - 1;

        /* make sure value is with range of the checker array to prevent seg 
         * fault */
        if (index < 0 || index >= 9) {
                ((checker *)sudoku_checker)->correct = false;
                ((checker *)sudoku_checker)->counter++;
                return;
        }

        /*check if element has already been seen in current row/col*/
        if (((checker *)sudoku_checker)->arr[index] == 0)    {
                ((checker *)sudoku_checker)->arr[index] = 1;
                ((checker *)sudoku_checker)->counter++;
        } else    {
                ((checker *)sudoku_checker)->correct = false;
                ((checker *)sudoku_checker)->counter++;
        }
}