/*
 *     restoration.c
 *     sburch01, ihackn01
 *     HW1: filesofpix
 *
 *     Restoration takes in a pointer to a corrupted pgm image and proceedes
 *     line by line to seperate the digit and non-digit chars. Firstly the 
 *     non-digit infusion that corresponds to original image lines is found.
 *     Then the lines with this infusion are found and aggregated to finally
 *     print out the non-corrupted image.
 *      
 *     Restoration depends on readaline.h to read in line from files. 
 */
#include "mem.h"
#include "atom.h"
#include "table.h"
#include "seq.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <readaline.h>
#include <math.h>
#include <ctype.h>

void print_raw_pgm(Seq_T* orig_img_line, int width);

static void tablefree(const void *key, void **value, void *cl);

void clean_program(int* dup, Table_T* unique_infusions, int nums_in_line, 
Seq_T* orig_image_lines, FILE* fp);

const char* find_duplicate(int** num_line, int** dup, int* nums_in_line,  
Table_T* unique_infusions, FILE *fp);

void find_orig_lines(Seq_T* orig_image_lines, int** num_line, int**dup,
int* nums_in_line, FILE *fp, const char* orig_infusion);

const char* get_key_value(int** num_line, int* nums_in_line, FILE *fp);

int infusion_length(char* image_line, int line_size);

char* strip_infusion(char* image_line, int** num_line, int* nums_in_line,
int line_size, int infusion_size);

int string_to_num(char* char_num, int num_digits);



int main (int argc, char *argv[]) {
        /* CRE more than one argument is provided */
        assert(argc < 3);

        FILE* fp = NULL;
        /* Read from stdin */
        if (argc == 1) {
                fp = stdin;
        }
        else {
                fp = fopen(argv[1], "r");
                assert(fp != NULL);
        }
        /* Intialize values and start searching for duplicates */
        int* dup = NULL;
        int* num_line = NULL;
        int nums_in_line = 0;
        Table_T unique_infusions = Table_new(0, NULL, NULL);
        const char* orig_infusion = find_duplicate(&num_line, &dup, 
        &nums_in_line, &unique_infusions, fp);

        /* Intialize our sequence and adding elements */
        Seq_T orig_image_lines = Seq_new(0);
        find_orig_lines(&orig_image_lines, &num_line, &dup,
        &nums_in_line, fp, orig_infusion);

        print_raw_pgm(&orig_image_lines, nums_in_line);
        clean_program(dup, &unique_infusions, nums_in_line, &orig_image_lines, 
        fp);

        return 0;
}

/**********print_raw_pgm********
 *
 * Prints the raw pgm to standard output
 * Inputs:
 *      Seq_T* orig_img_lines: The sequence storing all of the original 
 *      image lines 
 * 
 *      int width: The amount of integers in one integer array
 * Return: N/A
 * Expects: 
 *      orig_img_lines not to be NULL
 *      width to be 2 or bigger
 ************************/
void print_raw_pgm(Seq_T* orig_img_lines, int width) {
        /* Height of the image is the amount of int arrs in the Seq */
        int height = Seq_length(*orig_img_lines);

        /* Print the magic number, dimensions, and Maxval  */
        printf("P5\n");
        printf("%i %i\n", width, height);
        printf("255\n");

        /* Line by line in the Seq print all the nums out */
        for (int i = 0; i < Seq_length(*orig_img_lines); i++) {
                int* curr_line = Seq_get(*orig_img_lines, i);    
                for (int j = 0; j < width; j++) {
                        printf("%c", curr_line[j]);
                }
        }
}


/**********tablefree********
 *
 * Frees the value stored in the Hanson table unique_infusions
 * Inputs:
 *              const void *key: Key of the unique_infusions table
 *      void **value: Value of the unique_infusions table
 *      void* cl: In this case always null
 * Return: N/A
 * Expects: 
 *      
 * Notes:
 *      Called by Table_map to reach all the values stored in the Hanson
 *      table
 *      Frees the memory assocaited with int arrays stored in the values of the
 *      table
 ************************/
static void tablefree(const void *key, void **value, void *cl) {
        (void) key;
        (void) cl;

        FREE(*value);
}  

/**********clean_program********
 *
 * Frees multiple aspects of our program
 * Inputs:
 *      int* dup: The second instance of a original image line
 *
 *      Table_T* unique_infusions: Table that was used to find the duplicates
 *
 *      int nums_in_line: The number of nums in a line
 *
 *      Seq_T* orig_image_lines: Storing all the original image lines
 * 
 *      FILE* fp: Pointer to the file
 * Return: N/A
 * Expects
 *      None of these pointer arguments to be NULL;
 *      nums_in_line is equal to the int arr length
 * Notes
 *      Frees memory of dup
 *      Goes through and frees the values of unique_infusions
 *      Int arrays stored in orig_image_lines are freed
 *      Closes fp
 ************************/
void clean_program(int* dup, Table_T* unique_infusions, int nums_in_line, 
Seq_T* orig_image_lines, FILE* fp) {
        /* 
        * Free dup (which after Table_put finds a duplicate and replaces the 
        * value stored there with the second original image line) is the first
        * line in the original image 
        */
        FREE(dup);
        /* Free the table and its values */
        Table_map(*unique_infusions, tablefree, &nums_in_line);
        Table_free(unique_infusions); 

        int seq_length = Seq_length(*orig_image_lines); 
        /*
        * For loop to free the values in the Sequence
        * Additonally, the loop needs to start at i = 2
        * This because the values at indexes 0 and 1 have been deleted when
        * FREE(img_line) is called and when the tables are free respectively.
        */
        for (int i = 2; i < seq_length; i++) {
                int* curr_line = Seq_get(*orig_image_lines, i);
                FREE(curr_line);
        } 

        Seq_free(orig_image_lines);
        fclose(fp); 
}

/**********find_duplicate********
 *
 * Finds which infusion lines are duplicate i.e. which lines are original
 * image lines
 * Inputs:
 *      int** num_line: Storing the line of digit chars in one image line
 *      int** dup: Stores the digit line (of a image line) from a image 
 *       line that has been found to have a duplicate infusion sequence.
 * 
 *      int* nums_in_line: Stores the size of an arr of digits
 * 
 *      Table_T* unique_infusions: Stores unique infusions sequences as keys 
 *      and digit arrs are values till a duplicate is found
 *      
 *      FILE *fp: Pointer to where we should read values in from

 * Return: The infusion sequence that is found in original image lines
 * Expects:
 *      unique_infusions: To be initialized
 * 
 * Notes:
 *      dup and num_line are updated constantly until a line with a duplicate 
 *      sequence is found, then dup is the first instance of a original image
 *      line, and num_line is the second 
 * 
 *      nums_in_line is also updated in the function strip_infusions where the
 *      number of the ints in the array is manually counted
 * 
 *      unique_infusions is also added to/edited through the execution of this
 *      function
 *      
 ************************/
const char* find_duplicate(int** num_line, int** dup, int* nums_in_line,  
Table_T* unique_infusions, FILE *fp) {

        const char* infusion_atom = get_key_value(num_line, nums_in_line, fp);
        assert(infusion_atom != NULL);

        /* Our table uses the infusion of a line (Atom string) as the key and
        the digit chars of a line as the value */
        Table_put(*unique_infusions, infusion_atom, *num_line);

        /* Table_put returns NULL if adding a key-value pair with a unique key 
        If NULL is reached, then a duplicate key has been added */
        while (*dup == NULL) {
                *num_line = NULL;
                infusion_atom = get_key_value(num_line, nums_in_line, fp);

                *dup = Table_put(*unique_infusions, infusion_atom, *num_line);
                
        }

        /* If the loop is exited the value of infusion_atom is the infusion 
        sequence found in original image lines */
        return infusion_atom;
}


/**********find_orig_lines********
 *
 * Identifies original image lines and adds them to a Sequence
 * Inputs:
 *      Seq_T* orig_image_lines: Stores the original image lines (int arrs)
 * 
 *      int** num_line: Storing the line of digit chars in one image line
 * 
 *              int** dup: First original image line to be added to seq
 * 
 *      int* nums_in_line: Stores the size of an arr of digits
 *      
 *      FILE *fp: Pointer to where we should read values in from
 * 
 *      const char* orig_infusion: The infusion that is found in the original
 *      image lines
 * Return: N/A
 * Expects:
 *      orig_image_lines: To be initalized 
 * 
 * Notes:
 *      num_line is updated constantly and either freed if the infusion of the
 *      line does not match the infusion of original image lines or added to
 *      the sequence if it does
 * 
 *      As mentioned above the Sequence is added to in this function
 *      
 ************************/
void find_orig_lines(Seq_T* orig_image_lines, int** num_line, int**dup,
int* nums_in_line, FILE *fp, const char* orig_infusion) {

        Seq_addhi(*orig_image_lines, *dup);
        /* Note when first passed into find_orig_lines, num_line holds the 
        second original image line */
        Seq_addhi(*orig_image_lines, *num_line);

        *num_line = NULL;

        const char* infusion_atom = get_key_value(num_line, nums_in_line, fp);

        /* Continue until readaline returns NULL */
        while (infusion_atom != NULL) {
                /* If infusions match add to Sequence */
                if (infusion_atom == orig_infusion) {
                        Seq_addhi(*orig_image_lines, *num_line);
                }
                /* Otherwise free the int arr */
                else {
                        assert(*num_line != NULL);
                        FREE(*num_line);
                }
                /* Grab next infusion and its associated num_line */
                *num_line = NULL;
                infusion_atom = get_key_value(num_line, nums_in_line, fp);
        } 

}


/**********get_key_value********
 *
 * Reads in lines and strips and converts them 
 * Inputs:
 *      int** num_line: Storing the line of digit chars in one image line
 * 
 *      int* nums_in_line: Stores the size of an arr of digits
 *      
 *      FILE *fp: Pointer to where we should read values in from
 * 
 * Return: The Atom string of the infusion of the current line
 * Expects:
 *      fp to be opened
 * 
 * Notes:
 *      num_line is updated to be the pointer to the int arr storing the 
 *      digit chars of the current line
 *      
 *      Additionally nums_in_line is calculated in strip_infusion and will be
 *      be updated and returned
 * 
 *      
 ************************/
const char* get_key_value(int** num_line, int* nums_in_line, FILE *fp) {
        /* Call readaline */
        char *datapp = NULL;
        int line_size = readaline(fp, &datapp);

        /* If we reached EOF  */
        if (datapp == NULL) {
                return NULL;
        }

        /* Calculate the length of the infusion in line */
        int infusions_len = infusion_length(datapp, line_size);
        /* ALLOC memory for num_line */
        *num_line = ALLOC(line_size * sizeof(int));
        assert(*num_line != NULL);

        char* infusion = strip_infusion(datapp, num_line, 
        nums_in_line, line_size, infusions_len);

        /* datapp is not needed anymore */
        FREE(datapp);

        /* Create an Atom string holding the current infusion */
        const char *infusion_atom = Atom_new(infusion, infusions_len);

        /* After being converted to Atom, char arr is not needed */
        FREE(infusion);

        return infusion_atom;
}


/**********infusion_length********
 *
 * Calculates the number of non-digit chars in a line
 * Inputs:
 *              char* image_line: The entire line of digit and non-digit chars
 *              int line_size: Size of the image_line
 * Return: The length of the non-digit chars
 * Expects
 *      image_line to be not NULL
 *      the length of image_line to be equal to line_size
 ************************/
int infusion_length(char* image_line, int line_size) {
        int infusion_length = 0; 
        for (int i = 0; i < line_size; i++) {
                /* If the curr char is not a digit and not a newline increment
                infusion_length */
                if (!(isdigit(image_line[i])) && image_line[i] != '\n') {
                        infusion_length++;
                }
        }
        return infusion_length;
} 

/**********strip_infusion********
 *
 * Seperates the non-digit infusion from the digit lines
 * Inputs:
 *              char* image_line: The entire line of digit and non-digit chars
 *      int** num_line: Reference to the int array that will store the digit 
 *      chars which will be returned
 *      int* nums_in_line: Number of digits in the num_line also returned by 
 *      reference
 *      int line_size: Size of the entire image_line
 *      int infusion_size: Number of non-digits in image_line
 * Return: The char array storing the non-digit chars in image_line
 * Expects
 *      image_line to be not NULL
 * Notes:
 *              This function returns the infusion sequence, along with the 
 *      array of numbers, and length of that array of numbers.
 ************************/
char* strip_infusion(char* image_line, int** num_line, int* nums_in_line,
int line_size, int infusion_size) {
        /* Allocate memory for lines storing the digit and non-digit chars */
        char* infusion_line = NULL;
        infusion_line = ALLOC(infusion_size);
        assert(infusion_line != NULL);
        *nums_in_line = 0;
        int infusion_counter = 0;

        /* Iterate through the entire image line */
        for (int i = 0; i < line_size; i++) {
                /* If a digit is detected find the number it represents */
                if (isdigit(image_line[i])) {
                        char char_curr_num[4];
                        int j = 0;
                        char_curr_num[j] = image_line[i];
                        /* Consecutive number chars are a part of the same 
                        number */
                        while (i < line_size - 1 && 
                        isdigit(image_line[i + 1])) {
                                i++;
                                j++;
                                char_curr_num[j] = image_line[i];
                        }
                        char_curr_num[j + 1] = '\0';
                        int num_to_add = string_to_num(char_curr_num, j + 1);
                        /* Only add nums that are less than or equal to 255 */
                        if (num_to_add <= 255) {
                                (*num_line)[(*nums_in_line)] = num_to_add;
                                (*nums_in_line)++;
                        } 
                }
                /* Otherwise add curr char to the non-digit array */
                else if (image_line[i] != '\n') {
                        infusion_line[infusion_counter] = image_line[i];
                        infusion_counter++;
                }
        }
        return infusion_line;
}


/**********string_to_num********
 *
 * Takes a string of chars (that are numbers) and returns the number as an int
 * Inputs:
 *      char* char_num: Array of chars that are nums that needs to be converted
 *      int num_digits: The numbers of digits in the char_num array
 * Return: int Number that was represented by a char array
 * Expects
 *      The char_num to not be null
 * Notes:
 *      This functions uses the pow function from the math function
 ************************/
int string_to_num(char* char_num, int num_digits) {

        int total_num = 0;
        int char_num_idx = 0;
        /* Goes through the entire char array */
        while (num_digits != 0) {
                /*
                * Calculates based on how many digits there are in the char
                * array and mutliplies a number at a index by a certain factor 
                * of 10
                * For example with the char array 192 (representing 192) we 
                * will calculate the number by doing: 
                * 1 * (10^2) + 9 * (10^1) + 2 * (10^0)
                */
                int temp = (char_num[char_num_idx] - 48) * 
                (pow(10, (num_digits - 1)));
                total_num += temp;
                char_num_idx++;
                num_digits--;
        }

        return total_num;

}