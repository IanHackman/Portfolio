#include "ppmtrans.h"

/********* struct col_row ******** 
 *
 * Each instance of this struc contains the mapped index column and row of a 
 * pixel in A2 array. This struct is created and returned by each of the 
 * transformaion functions.
 *
 ************************/
struct col_row {
        unsigned col;
        unsigned row;
};

/********* struct index_mapping ********
 *
 * Each instance of this struct contains n empty A2 array to be mapped to, the 
 * transformation function entered by the user and the methods suite. Only one 
 * instance of this struct is used in ppmtrans.
 *
 ************************/
struct index_mapping {
        A2 mapped_pixels;
        col_row(*mapping_function)(int col, int row,
                                  A2 array, A2Methods_T methods);
        A2Methods_T methods;
};

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (false)

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

int main(int argc, char *argv[]) 
{
        char *time_file_name = NULL;
        int   rotation       = 0;
        int i;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map);

        /* call function to parse through command lines and set file pointer */
        FILE *fp = parse_command_line(argc, argv, &methods,
                                      &map, &time_file_name, &rotation, &i);
        assert(fp != NULL);

        /* initialize pixmap and the A2 array element */
        Pnm_ppm pixmap = Pnm_ppmread(fp, methods);
        assert(pixmap != NULL);

        A2 pixels = pixmap->pixels;
        assert(pixels != NULL);

        /* get the transformation funciton from command line */
        trans_func transformation_function
                 = get_rotate_flip_transpose(rotation);
        assert(transformation_function != NULL);
        
        /* create empty array to be copied into */
        index_mapping *mapped_image = make_mapped_image(transformation_function,
                                                        pixels, methods);
        
        /* if -time is entered, initialize CPUTime instance and start timer */
        CPUTime_T timer = CPUTime_New();
        FILE *timing_fp = NULL;
        if (time_file_name != NULL) {
                timing_fp = fopen(time_file_name, "a");
                CPUTime_Start(timer);
        }
        
        map(pixels, map_to_new_array, (void *)mapped_image);
        
        /* if -time is entered, stop timer and write results to output file */
        if (time_file_name != NULL) {
                write_to_time_file(timer, methods, timing_fp, pixels, argc, 
                                   argv, i);
        }
        
        swap_arrays(pixmap, methods, mapped_image);

        Pnm_ppmwrite(stdout, pixmap);

        free_and_close(pixmap, fp, timing_fp, timer, mapped_image);
        
        return 0;
}

/********** parse_command_line ********
 *
 * Gets relevent file, transformation type, and time option from command line
 * arguments.
 *
 * Parameters:
 *      int argc               :   Number of command line arguments
 *      char *argv[]           :   Array of command line arguments
 *      char **time_file_name  :   Pointer to string storing timing file namee
 *      int *rotation          :   Ptr to variable which stores rotation type
 *      int *j                 :   Ptr to variable which stores integer
 *                                 corresponding to specific command line arg
 *
 * Return: File pointer to relevent input source (either from a file or stdin)
 * 
 * Error checking for the arguments and variables is done within the for loop
 *
 ************************/
FILE *parse_command_line(int argc, char *argv[], A2Methods_T *methods_ptr, 
                         A2Methods_mapfun **map_ptr, char **time_file_name,
                         int *rotation, int *j)
{
        A2Methods_mapfun *map = *map_ptr;
        A2Methods_T methods = *methods_ptr;
        int i;
        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major,
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major,
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) { /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        *rotation = strtol(argv[++i], &endptr, 10);
                        if (!(*rotation == 0 || *rotation == 90
                        || *rotation == 180 || *rotation == 270)) {
                                fprintf(stderr, "Rotation must be "
                                        "0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) { /* Not a number */
                                usage(argv[0]);
                        } 
                } else if (strcmp(argv[i], "-flip") == 0) {
                        if (i == argc) {
                                /* if no argument is provided after -flip */
                                usage(argv[0]);
                        }
                        if (strcmp(argv[++i], "horizontal") == 0) {
                                *rotation = -1;
                        } else if (strcmp(argv[++i], "vertical") == 0) {
                                *rotation = -2;
                        } else {
                        /* if an invalid command is entered after -flip */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-transpose") == 0) {
                        *rotation = -3;
                } else if (strcmp(argv[i], "-time") == 0) {
                        *time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }
        
        FILE *fp = NULL;
        /* handles -transpose case */
        if (*rotation == -3 && i == argc) {
                fp = stdin;
        } else if (*rotation == -3 && i + 1 == argc) {
                if (i < argc) {
                        fp = fopen(argv[i], "r");
                }
        } else if (i == argc)   {
                fp = stdin;
        } else {
                if (i < argc) {
                        fp = fopen(argv[i], "r");
                }
        }

        *j = i;
        *map_ptr = map;
        *methods_ptr = methods;
        assert(fp != NULL);
        return fp;
}

/********** get_rotate_flip_transpose ********
 *
 * Purpose: Get the transformation function based on the specified rotation
 *          or flip/transpose input.
 *
 * Parameters:
 *     int rotation - the rotation or flip angle:
 *                90  - Rotate 90 degrees clockwise
 *                180 - Rotate 180 degrees clockwise
 *                270 - Rotate 270 degrees clockwise
 *                0   - No rotation (identity transformation)
 *                -1  - Flip horizontally
 *                -2  - Flip vertically
 *                -3  - Transpose (swap) horizontally and vertically
 *
 * Return: The address of the corresponding transformation function pointer.
 *         If an invalid `rotation` value is provided, returns NULL.
 *
 * Notes:
 *     A rotation value that is not 90, 180, 270, 0, -1, -2 or -3 is handlied in
 *     main command line parsing. In addition if the function returns NULL, 
 *     indicating an incorrect rotation value was entered, an assert statement 
 *     in main asserts if the funciton if NULL. 
 *
 ************************/
trans_func get_rotate_flip_transpose(int rotation)
{
        if (rotation == 90) {
                return &rotate_90;
        } else if (rotation == 180) {
                return &rotate_180;
        } else if (rotation == 270) {
                return &rotate_270;
        } else if (rotation == 0) {
                return &rotate_zero;
        } else if (rotation == -1) {
                return &flip_horizontal;
        } else if (rotation == -2) {
                return &flip_vertical;
        } else if (rotation == -3) {
                return &flip_transpose;
        }
        return NULL;
}

/********** map_to_new_array ********
 *
 * Purpose: Map an element from original 2D array to the new 2D array, applying 
 *          a transformation function specified in the `index_mapping` structure
 *          to get the mapped index.
 *
 * Parameters:
 *     int col      - the column of the current element
 *     int row      - the row of the current element
 *     A2 array     - the original 2D array 
 *     void *elem   - a void pointer to the element to be mapped
 *     void *cl     - a void pointer to an `index_mapping` structure containing:
 *                      - methods:           - An A2Methods_T interface for the 
 *                                             source array.
 *                      - mapped_pixels:     - The destination 2D array to map 
 *                                             the element to
 *                      - mapping_function:  - A function pointer for the 
 *                                             function to apply
 *
 * Return: void
 *
 * Notes:
 *     Error handling for col, row, array and methods is done by assert_col_row
 *     The function checks if *cl and *elem are NULL. It also checks if each 
 *     variable that is made after casting *cl to an element of index_mapping 
 *     struct is not NULL
 *
 ************************/
void map_to_new_array(int col, int row, A2 array, void *elem, void *cl)
{
        assert(cl != NULL && elem != NULL);
        /* get methods suite from index_mapping struct contained in *cl  */
        A2Methods_T methods = ((index_mapping *)cl)->methods;
        assert_col_row(col, row, array, methods);

        /* get A2 array */
        A2 mapped_array = ((index_mapping *)cl)->mapped_pixels;
        assert(mapped_array != NULL);
        
        /* get the mapped index from mapping function */
        col_row mapped_index = 
        ((index_mapping *)cl)->mapping_function(col, row, array, methods);
        
        /* get pixel from *elem */
        assert(cl != NULL && elem != NULL);
        Pnm_rgb pixel = (Pnm_rgb)elem;
        assert(pixel != NULL);
        
        /* get element at mapped index and swap it with pixel at current index*/
        *(Pnm_rgb)(methods->at(mapped_array, mapped_index.col,
                                 mapped_index.row)) = *pixel;      
}


/********** assert_col_row ********
 *
 * Purpose: Check the validity of (col, row) coordinates within a 2D array.
 *
 * Parameters:
 *     int col     - the column index to be checked
 *     int row     - the row index to be checked
 *     A2 array   - the 2D array
 *     A2Methods_T methods - the A2Methods_T interface containing operations for
 *                           A2 array
 *
 * Return: void
 *
 * Notes:
 *     This function checks that the coordinates are within the valid range, 
 *     greater than or equal to 0 or less than the total height and width
 *     for the given 2D array.
 *     Helper function for every funciton in ppmtrans that uses col, row, 
 *     array and methods. 
 *
 ************************/
void assert_col_row(int col, int row, A2 array, A2Methods_T methods)
{
        assert(array != NULL);
        assert(methods != NULL);
        assert(col >= 0 && row >= 0);
        assert(col < methods->width(array) && row < methods->height(array));
}

/********** write_to_time_file ********
 *
 * Purpose: calculate the time for each rotation and the time per pixel and then
 *          output it to a file
 *
 * Parameters: 
 *      CPUTime_T timer:        - timer instance with ttoal time per rotation
 *      FILE *timing_fp:        - file pointer to timing output file
 *      A2 pixels:              - A2 array of original image
 *      int argc                - Number of command line arguments
 *      char *argv[]            - Array of command line arguments
 *      int i                   - int that traverses through argv based on argc
 *
 * Return: nothing
 *
 * Notes: CRE if timer, timing_fp or pixels are is NULL
 *        The function stops the timer from main and calculates the time per 
 *        pixel by dividing the total time by the total number of pixels.
 *        argc and argv are used to access command line arguments
 *        This function assumes valid command line arguments have been entered 
 *        as they were checked in parse_command_line.
 *  
 *
 ************************/
void write_to_time_file(CPUTime_T timer, A2Methods_T methods, FILE *timing_fp, 
                        A2 pixels, int argc, char *argv[], int i) 
{   
        assert(timer != NULL && timing_fp != NULL && pixels != NULL);
        
        /* stops the timer */
        double time_used = CPUTime_Stop(timer);

        /* calcuates time per pixel */
        int num_pixels = methods->width(pixels)
                        * methods->height(pixels);
        double time_per_pixel = time_used / num_pixels;

        /* gets image name or stdin */
        if (i != argc) {
                fprintf(timing_fp, "Image Name: %s\n", argv[i]);
        } else {
                fprintf(timing_fp,
                        "Image Name: unknown (read from stdin)\n");
        }

        /* gets transformaion functiton and mapping function */
        fprintf(timing_fp,
                "Transformation function applied to image: ");
        if (strcmp(argv[1], "-rotate") == 0
                || strcmp(argv[1], "-flip") == 0) {
                fprintf(timing_fp, "%s, %s\n", argv[1], argv[2]);
                fprintf(timing_fp, "Mapping Operation Used: %s\n",
                        argv[3]);
        } else {
                fprintf(timing_fp, "%s\n", argv[1]);
                fprintf(timing_fp, "Mapping Operation Used: %s\n",
                        argv[2]);
        }
        
        fprintf(timing_fp, "Total Time: %f ns\n", time_used);
        fprintf(timing_fp, "Time per pixel: %f ns\n\n\n",
                time_per_pixel);
}

/********** make_mapped_image ********
 *
 * Purpose: Create an index_mapping struct containing a 2d array for the mapped
 *          image and the relevent function pointer
 *
 * Parameters: 
 *      trans_func transformation_function : ptr to correct transformation func
 *      A2 pixels                          : A2 array of original image
 *      A2Methods_T methods                : Methods suite
 *
 * Return: Pointer to an initialized index mapping struct
 *
 * Notes: CRE if any parameters are NULL.
 *        It is the responsibility of the caller to free the memory of the 
 *        returned index_mapping struct.
 ************************/
index_mapping *make_mapped_image(trans_func transformation_function,
                                 A2 pixels, A2Methods_T methods)
{
        assert(transformation_function != NULL);
        assert(pixels != NULL);
        assert(methods != NULL);

        /* create new array to be mapped into */
        A2 mapped_pixels;
        if (transformation_function == rotate_90
         || transformation_function == rotate_270
         || transformation_function == flip_transpose) {
                /* create new array with opposite dimensions as original array*/
                mapped_pixels = methods->new(methods->height(pixels),
                                        methods->width(pixels),
                                        methods->size(pixels));
         } else {
                mapped_pixels = methods->new(methods->width(pixels),
                                        methods->height(pixels),
                                        methods->size(pixels));
         }
         
        assert(mapped_pixels != NULL);
        
        /* create an instance of index_mapping and assign its elements */
        index_mapping *mapped_image = ALLOC(sizeof(index_mapping));
        assert(mapped_image != NULL);
        
        mapped_image->mapped_pixels = mapped_pixels;
        mapped_image->mapping_function = transformation_function;
        mapped_image->methods = methods;

        return mapped_image;
}

/********** swap_arrays ********
 *
 * Purpose: swap the original array with the new array 
 *
 * Parameters: 
 *      Pnm_ppm pixmap                :   pixmap to be freed
 *      A2Methods_T methods           :   Methods suite
 *      index_mapping *mapped_image   :   index_mapping struct to be freed.
 *
 * Return: nothing
 *
 * Notes: CRE if any parameters are NULL
 *        Function frees the original array and assigns the height, width and 
 *        array of the new array
 *  
 *
 ************************/
void swap_arrays(Pnm_ppm pixmap, A2Methods_T methods, 
                 index_mapping *mapped_image) 
{
        assert(pixmap != NULL && methods != NULL && mapped_image != NULL);
        
        /* frees the original array, reasigns the new array to pixmap */
        methods->free(&(pixmap->pixels));
        pixmap->pixels = mapped_image->mapped_pixels;
        pixmap->width = methods->width(pixmap->pixels);
        pixmap->height = methods->height(pixmap->pixels);
}

/********** free_and_close ********
 *
 * Purpose: Free memory allocated to the heap by the ppmtrans program.
 *
 * Parameters: 
 *      Pnm_ppm pixmap                :   pixmap to be freed
 *      FILE *fp                      :   input file pointer to be closed
 *      FILE *timing_fp               :   timing file pointer to be closed
 *      index_mapping *mapped_image   :   index_mapping struct to be freed.
 *
 * Return: Pointer to an initialized index mapping struct
 *
 * Notes: CRE if any parameters are NULL
 *  
 *
 ************************/
void free_and_close(Pnm_ppm pixmap, FILE *fp, FILE *timing_fp, CPUTime_T timer,
                    index_mapping *mapped_image)
{
        assert(pixmap != NULL && fp != NULL && mapped_image != NULL);
        
        /* frees pixmap and mapped_image struct */
        FREE(mapped_image);
        Pnm_ppmfree(&pixmap);
        
        /* closes file pointer */
        if (fp != stdin) {
                fclose(fp);
        }
        if (timing_fp != NULL) {
                fclose(timing_fp);
        }
        /* frees instance of CPUTime */
        CPUTime_Free(&timer);
}

/******************************************************************************
 *
 *                          TRANSFORMATION FUNCTIONS
 *           Functions below send an ordered pair of coordinates to a new
 *                  ordered pair under a given transformation
 *
 ******************************************************************************/

/********** rotate_90 ********
 *
 * Purpose: Rotate the given (col, row) index 90 degrees clockwise
 *          within the given 2D array.
 *
 * Parameters:
 *     int col             - the original column index
 *     int row             - the original row index
 *     A2 array            - the 2D array being rotated
 *     A2Methods_T methods - the A2Methods_T interface containing operations
 *                           for A2 array
 *
 * Return: A col_row struct map_index containing the rotated coordinates.
 *
 * Notes:
 *     Performs calculations on original index to get the mapped index. 
 *     Errer handling done in assert_col_row
 *
 ************************/
col_row rotate_90(int col, int row, A2 array, A2Methods_T methods)
{
        assert_col_row(col, row, array, methods);
        col_row map_index;

        /* calculate mapped index */
        map_index.col = methods->height(array) - row - 1;
        map_index.row = col;

        return map_index;
}

/********** rotate_180 ********
 *
 * Purpose: Rotate the given (col, row) index 180 degrees clockwise
 *          within the given 2D array.
 *
 * Parameters:
 *     int col             - the original column index
 *     int row             - the original row index
 *     A2 array            - the 2D array being rotated
 *     A2Methods_T methods - the A2Methods_T interface containing operations for
 *                           A2 array
 *
 * Return: A col_row struct map_index containing the rotated coordinates.
 *
 * Notes:
 *     Performs calculations on original index to get the mapped index. 
 *     Errer handling done in assert_col_row
 *
 ************************/
col_row rotate_180(int col, int row, A2 array, A2Methods_T methods)
{
        assert_col_row(col, row, array, methods);
        col_row map_index;

        /* calculate mapped index */
        map_index.col = methods->width(array) - col - 1;
        map_index.row = methods->height(array) - row - 1;

        return map_index;
}

/********** rotate_270 ********
 *
 * Purpose: Rotate the given (col, row) index 270 degrees clockwise
 *          within the given 2D array.
 *
 * Parameters:
 *     int col             - the original column index
 *     int row             - the original row index
 *     A2 array            - the 2D array being rotated
 *     A2Methods_T methods - the A2Methods_T interface containing operations for
 *                           A2 array
 *
 * Return: A col_row struct map_index containing the rotated coordinates.
 *
 * Notes:
 *     Performs calculations on original index to get the mapped index. 
 *     Errer handling done in assert_col_row
 *
 ************************/
col_row rotate_270(int col, int row, A2 array, A2Methods_T methods)
{
        assert_col_row(col, row, array, methods);
        col_row map_index;

        /* calculate mapped index */
        map_index.col = row;
        map_index.row = methods->width(array) - col - 1;

        return map_index;
}

/********** rotate_0 ********
 *
 * Purpose: Rotate the given (col, row) index 0 degrees clockwise
 *          within the given 2D array.
 *
 * Parameters:
 *     int col             - the original column index
 *     int row             - the original row index
 *     A2 array            - the 2D array being rotated
 *     A2Methods_T methods - the A2Methods_T interface containing operations for
 *                           A2 array
 *
 * Return: A col_row struct map_index containing the rotated coordinates.
 *
 * Notes:
 *     Since the array is not rotated, no operations are performed on the 
 *     original index
 *     Errer handling done in assert_col_row
 *
 ************************/
col_row rotate_zero(int col, int row, A2 array, A2Methods_T methods)
{
        assert_col_row(col, row, array, methods);
        col_row mapped_index;

        /* calculate mapped index */
        mapped_index.col = col;
        mapped_index.row = row;

        return mapped_index;
}

/********** flip_horizontal ********
 *
 * Purpose: Flip the given (col, row) index across the horizontal axis 
 *          within the given 2D array.
 *
 * Parameters:
 *     int col             - the original column index
 *     int row             - the original row index
 *     A2 array            - the 2D array being rotated
 *     A2Methods_T methods - the A2Methods_T interface containing operations for
 *                           A2 array
 *
 * Return: A col_row struct map_index containing the rotated coordinates.
 *
 * Notes:
 *     Performs calculations on original index to get the mapped index. 
 *     Errer handling done in assert_col_row
 *
 ************************/
col_row flip_horizontal(int col, int row, A2 array, A2Methods_T methods)
{
        assert_col_row(col, row, array, methods);
        col_row mapped_index;

        /* calculate mapped index */
        mapped_index.col = methods->width(array) - 1 - col;
        mapped_index.row = row;

        return mapped_index;
}

/********** flip_vertical ********
 *
 * Purpose: Flip the given (col, row) index across the vertical axis 
 *          within the given 2D array.
 *
 * Parameters:
 *     int col             - the original column index
 *     int row             - the original row index
 *     A2 array            - the 2D array being rotated
 *     A2Methods_T methods - the A2Methods_T interface containing operations for
 *                           A2 array
 *
 * Return: A col_row struct map_index containing the rotated coordinates.
 *
 * Notes:
 *     Performs calculations on original index to get the mapped index. 
 *     Errer handling done in assert_col_row
 *
 ************************/
col_row flip_vertical(int col, int row, A2 array, A2Methods_T methods)
{
        assert_col_row(col, row, array, methods);
        col_row mapped_index;

        /* calculate mapped index */
        mapped_index.col = col;
        mapped_index.row = methods->height(array) - 1 - row;

        return mapped_index;
}

/********** flip_horizontal ********
 *
 * Purpose: Transpose the given (col, row) index within the given 2D array.
 *
 * Parameters:
 *     int col             - the original column index
 *     int row             - the original row index
 *     A2 array            - the 2D array being rotated
 *     A2Methods_T methods - the A2Methods_T interface containing operations for
 *                           A2 array
 *
 * Return: A col_row struct map_index containing the rotated coordinates.
 *
 * Notes:
 *     Swaps the col and row index.
 *     Errer handling done in assert_col_row
 *
 ************************/
col_row flip_transpose(int col, int row, A2 array, A2Methods_T methods)
{
        assert_col_row(col, row, array, methods);
        col_row mapped_index;

        mapped_index.col = row;
        mapped_index.row = col;

        return mapped_index;
}
