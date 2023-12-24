/*
 *     readaline.c
 *     sburch01, ihackn01
 *     HW1: filesofpix
 *
 *     Reads one line in a given file. Its size along with a pointer to the 
 *     first byte is returned. This file can read lines of variable length
 *     and reallocate memory as necessary.
 */
#include <mem.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <readaline.h>
#include <ctype.h>


/**********readaline********
 *
 * Reads in a line from a input file
 * Inputs:
 *      FILE *inputfd: pointer to the file to read from 
 *      char** datapp: Reference to the char pointer that holds the current 
 *      line
 * Return: The size of the line
 * Expects
 *      *inputfd to be opened
 * Notes:
 *      Will CRE if either argument is NULL
 *      *datapp will be ALLOCed in 
 *              Will CRE if ALLOC fails
 *      Will CRE if reading fails
 *      WILL exit if the input line exceeds 1000 chars
 ************************/
size_t readaline(FILE *inputfd, char **datapp) {

        assert(inputfd != NULL);
        assert(datapp != NULL);

        /* Counter for keeping track of the chars in a line */
        int line_length = 0; 
        int size_of_arr = 1001;

        /* Get the first char in the line and assert its not null */
        int c = fgetc(inputfd);
        assert(&c != NULL);

        /* End of file check */
        if (c == -1) {
                *datapp = NULL;
                return 0;
        }
        /* At first ALLOC 1001 bites to datapp */
        *datapp = ALLOC(size_of_arr);
        assert(*datapp != NULL);

        /* The case in which the first char in the line is a new line char */
        if (c == 10) {
                (*datapp)[line_length] = c;
                return 2;
        }

        /* Keep reading in chars from the line until the EOF or a newline char
        is reached */
        while (c != -1 && c != 10) {
                /* Resize arr if necessary */
                if (line_length == size_of_arr) {
                        char* new_datapp = *datapp;
                        *datapp = RESIZE(new_datapp, size_of_arr * 2);
                        size_of_arr *= 2;
                }
                /* Cast to char and add to datapp also increment counter */
                char actual_c = c;
                (*datapp)[line_length] = actual_c;
                line_length++; 
                /* Get next char */
                c = fgetc(inputfd);
                assert(*datapp != NULL);
        }
        /* No need to add newline char if EOF */
        if (c == -1) {
                return line_length;
        }

        /* If need to allocate one more byte for newline do so otherwise
        just add newline */
        if (line_length == size_of_arr) {
                char* new_datapp = *datapp;
                *datapp = RESIZE(new_datapp, size_of_arr + 1);
                (*datapp)[line_length] = '\n';
        }
        else if (c != -1) {
                (*datapp)[line_length] = '\n';
        }

        /* Return line_length plus one */
        return line_length + 1;
}