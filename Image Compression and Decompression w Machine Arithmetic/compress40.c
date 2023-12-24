/******************************************************************************
 *
 *                     compress40.c
 *
 *     Assignment: arith
 *     Authors:    ihackm01 and wranda01
 *     Date:       10/24/2023
 *
 *     Summary:    compress40.c implements a compression algorithm for a 
 *                 given ppm image. It outputs the compressed image to stdout.
 *
 *****************************************************************************/
#include "compress40.h"
#include <stdio.h>
#include <math.h>
#include "pnm.h"
#include "dct.h"
#include "assert.h"
#include "mem.h"
#include "arith40.h"
#include "bitpack.h"

const int BLOCKSIZE = 2;
const int C_SIGNED_BITE_SIZE = 5;
const int C_CHROMA_BITE_SIZE = 4;
const int C_CODEWORD_SIZE = 32;

typedef A2Methods_UArray2 A2;


/*
 * Stores the average Pb and Pr values in a 2x2 block.
 */
typedef struct average_chroma {
        float Pb_avg;
        float Pr_avg;
} average_chroma;

/*
 * Stores scaled integers to be put in a 5-bit form in the bitpacking process
 */
typedef struct scaled_ints {
        int b;
        int c;
        int d;
} scaled_ints;


/* Helper Function Declarations */
Pnm_ppm read_and_trim(FILE *input, A2Methods_T methods);
void copy_into_even_array(int col, int row, A2 new_pixels, void *elem,
                                                                void *cl);
void bitpack_all_pixels(int col, int row, A2 pixels, void *elem, void *cl);
void RGB_to_color_space(int col, int row, A2 array, void *elem, void *cl);
average_chroma get_average_chroma(object_methods_container color_space_arr,
                                  int col, int row);

scaled_ints get_scaled_ints(DCT_space DCT);
uint32_t bitpack_codeword(float a, scaled_ints scaled, average_chroma chroma);

/********** compress40 ********
 *
 * Executes the compression sequence by setting variables, creating image arrays
 * and calling map funcitons.
 *
 * Parameters:
 *      FILE *input - pointer to ppm file 
 *
 * Return: nothing
 *
 * Notes: 
 *      - CRE if the input file in NULL 
 *      - Sets the methods suite, creates pixmap and calls mapping functions for
 *        RGB_to_color_space and bitpack_all_pixels
 * 
 ************************/
void compress40(FILE *input)
{
        assert(input != NULL);

        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);

        /* set mapping function to row-major */
        A2Methods_mapfun *map = methods->map_row_major; 
        assert(map);
        
        /* get pixmap */
        Pnm_ppm pixmap = read_and_trim(input, methods);
        assert(pixmap != NULL);

        /* create an array with same dimensions as pixmap->pixels to store 
         * color_space structs */
        A2 color_pixels_arr = methods->new(pixmap->width, 
                                           pixmap->height, 
                                           sizeof(color_space));

        object_methods_container *contain =
                                        ALLOC(sizeof(object_methods_container));
        assert(contain != NULL);

        /*  create container, swap pixels with color_pixels_arr */        
        A2 pixels = pixmap->pixels;
        pixmap->pixels = color_pixels_arr;
        contain->object = &pixmap;
        contain->methods = methods;
        
        /* convert all pixels to color space */
        map(pixels, RGB_to_color_space, contain);
        pixmap->pixels = pixels;

        /* print header */
        printf("COMP40 Compressed image format 2\n%u %u\n",
               methods->width(pixels),
               methods->height(pixels));
        
        /* convert pixels to 32-bit words */
        map(color_pixels_arr, bitpack_all_pixels, &methods);

        /* free all allocated memory */
        methods->free(&color_pixels_arr);
        FREE(contain);
        Pnm_ppmfree(&pixmap);
}

/********** read_and_trim ********
 *
 * Creates a pixmap containing data of the inputted image. Ensures even width
 * and height.
 *
 * Parameters:
 *      FILE *input - pointer to ppm file 
 *
 * Return: Pnm_ppm struct containing array and relevant data
 *
 * Notes: 
 *      - CRE if the input file in NULL
 *      - CRE if methods is NULL
 *      - CRE if pixmap is NULL
 *      - CRE if width or height is less than 2 
 * 
 ************************/
Pnm_ppm read_and_trim(FILE *input, A2Methods_T methods)
{
        assert(input != NULL);

        Pnm_ppm pixmap = Pnm_ppmread(input, methods);

        /* get dimensions */
        int original_width = pixmap->width;
        int original_height = pixmap->height;
        assert(original_width >= 2 && original_height >= 2);
    
        A2 original_pixels = pixmap->pixels;

        /* check and trim if dimensions are odd */
        if (original_height % 2 != 0) {
                original_height -= 1;
        }
        if (original_width % 2 != 0) {
                original_width -= 1;
        }

        pixmap->width = original_width;
        pixmap->height = original_height;

        /* allocate new array with correct dimensions */
        A2 new_pixels = methods->new(original_width, 
                                     original_height, 
                                     sizeof(struct Pnm_rgb));
        /* make container for copying old array into new even array */
        object_methods_container *container
                                      = ALLOC(sizeof(object_methods_container));
        assert(container != NULL);
        container->object = &original_pixels;
        container->methods = methods;

        methods->map_row_major(new_pixels, copy_into_even_array, container);

        /* free old array and reassign pixmap->pixels to new array */
        methods->free(&original_pixels);
        pixmap->pixels = new_pixels;

        FREE(container);
       
        return pixmap;
}

/********** copy_into_even_array ********
 *
 * Copies elements of original pixmap into a new array of even dimensions. 
 * Last col and/or row ignored if width and/or height is odd.
 * Apply function for a mapping function.
 *
 * Parameters:
 *      int col       - col index of current element of new_pixels
 *      int row       - row index of current element of new_pixels
 *      A2 new_pixels - array which old data is going to be copied into
 *      void *elem    - points to element of new_pixels at (col, row)
 *      void *cl      - contains original array and methods
 *
 * Return: None
 *
 * Notes: 
 *      - CRE if new_pixels is NULL
 *      - CRE if cl is NULL
 *      - CRE if original_pixels is NULL
 *      - CRE if methods is NULL
 *      - CRE if col or row is out of range
 * 
 ************************/
void copy_into_even_array(int col, int row, A2 new_pixels, void *elem, void *cl)
{
        assert(new_pixels != NULL);
        assert(cl != NULL);

        /* get array to be copied and methods suite */
        A2 original_pixels = *(A2 *)((object_methods_container *)cl)->object;
        A2Methods_T methods = ((object_methods_container *)cl)->methods;
        
        /* assert parameters are correct */
        assert(col >= 0 && col < methods->width(new_pixels));
        assert(row >= 0 && row < methods->height(new_pixels));
        assert(original_pixels != NULL && methods != NULL);

        /* get pixel */
        Pnm_rgb curr_pixel = (Pnm_rgb)methods->at(original_pixels, col, row);
        assert(curr_pixel != NULL);

        /* copy elements of curr_pixels into elem */
        ((Pnm_rgb)elem)->red = curr_pixel->red;
        ((Pnm_rgb)elem)->green = curr_pixel->green;
        ((Pnm_rgb)elem)->blue = curr_pixel->blue;

        (void)new_pixels;
}

/********** bitpack_all_pixels ********
 *
 * Calls functions to convert pixels into 32-bit codeword for every block
 *
 * Parameters:
 *      int col       - col index of current element of new_pixels
 *      int row       - row index of current element of new_pixels
 *      A2 array      - array which old data is going to be copied into
 *      void *elem    - points to element of new_pixels at (col, row)
 *      void *cl      - contains original methods suite
 *
 * Return: nothing 
 *
 * Notes: 
 *      - CRE if the array, elem and cl are NULL. 
 *      - CRE is col and row are less than zero or not less than height or width
 *      - Checks id pixel is in corner of block of 4 pixels. If not ignores 
 *        that pixel
 *      - Functions called:
 *              - get_average_chroma, pixel_to_DCT, get_scaled_ints and
 *                bitpack_codeword
 *      - prints codeword to standard output
 *
 ************************/
void bitpack_all_pixels(int col, int row, A2 pixels, void *elem, void *cl)
{


        /* bitpack_all_pixels acts on top left pixel in 2x2 block */
        if (col % BLOCKSIZE != 0 || row % BLOCKSIZE != 0) {
                return;
        }

        A2Methods_T methods = *(A2Methods_T *)cl;

        /* assert correct index */
        assert(pixels != NULL && elem != NULL && cl != NULL);
        assert(col >= 0 && col < methods->width(pixels));
        assert(row >= 0 && row < methods->height(pixels));

        /* create container */
        object_methods_container container;
        container.object = &pixels;
        container.methods = methods;

        /* calls each conversion function */
        average_chroma chroma = get_average_chroma(container, col, row);
        DCT_space coefficients = pixel_to_DCT(container, col, row);
        scaled_ints scaled = get_scaled_ints(coefficients);
        uint32_t codeword = bitpack_codeword(coefficients.a, scaled, chroma);
        
        /* print codeword 8 bytes at a time using putchar() */
        for (int i = 1; i <= 4; i++) {
                uint8_t byte = Bitpack_getu(codeword, 8, (32 - i * 8));
                putchar(byte);
        }
}

/********** RGB_to_color_space ********
 *
 * Converts RGB values to component video color space (Y/Rb/Pr)
 *
 * Parameters:
 *      int col       - col index of current element of new_pixels
 *      int row       - row index of current element of new_pixels
 *      A2 array      - array which old data is going to be copied into
 *      void *elem    - points to element of new_pixels at (col, row)
 *      void *cl      - contains original array and methods
 *
 * Return: nothing 
 *
 * Notes: 
 *      - CRE if the array, elem and cl are NULL. After *colored_pixel is 
 *        allocated, CRE if memory allocation fails. *colored_pixel is freed in 
 *        the end of the function.
 ************************/
void RGB_to_color_space(int col, int row, A2 array, void *elem, void *cl)
{
        assert(array != NULL && elem != NULL && cl != NULL);

        /* get methods, pixmap and pixels array */
        A2Methods_T methods =((object_methods_container *)cl)->methods;
        Pnm_ppm pixmap = *(Pnm_ppm *)(((object_methods_container *)cl)->object);
        A2 color_pixels = pixmap->pixels;
        float denominator = (float)pixmap->denominator;

        assert(col >= 0 && col < methods->width(array));
        assert(row >= 0 && row < methods->height(array));
        
        /* allocates color_space (Y/Pb/Pr) struct */
        color_space *colored_pixel = ALLOC(sizeof(color_space));
        assert(colored_pixel != NULL);

        Pnm_rgb curr_pixel = (Pnm_rgb)elem;
        
        /* get RGB values */
        float red = curr_pixel->red / denominator;
        float green = curr_pixel->green / denominator;
        float blue = curr_pixel->blue / denominator;

        /* calculate Y, Pb and Pr */
        /* TODO: check if should round Y to be btwn 0 and 1 */
        colored_pixel->Y = 0.299 * red + 0.587 * green + 0.114 * blue;
        colored_pixel->Pb = -0.168736 * red - 0.331264 * green + 0.5 * blue;
        colored_pixel->Pr = 0.5 * red - 0.418688 * green - 0.081312 * blue;

        /* assign color_space struct to color_pixels array */
        *(color_space *)methods->at(color_pixels, col, row) = *colored_pixel;
        FREE(colored_pixel);
}

/********** get_average_chroma ********
 *
 * Take the average chroma value (Pb and Pr) of 4 pixels in a block. 
 *
 * Parameters:
 *      object_methods_container color_space_cont 
 *                    - contains Y/Pb/Pr array and methods suite
 *      int col       - col index of top left element in current pixel block
 *      int row       - row index of top left element in current pixel block
 *
 * Return: struct with average chroma values, Pb_avg and Pr_avg
 *
 * Notes: 
 *      - CRE if the color_space_cont is NULL
 *      - CRE if the color_space_arr is NULL
 *      - CRE if the methods is NULL
 *      - CRE if the col or row is out of bounds
 *
 ************************/
average_chroma get_average_chroma(object_methods_container color_space_cont,
                                  int col, int row)
{       /* get methods and array, assert not NULL an dthen col and row are 
         * indexed correctly */
        A2Methods_T methods = color_space_cont.methods;
        A2 color_pixels_arr = *(A2 *)color_space_cont.object;
        assert(methods != NULL && color_pixels_arr != NULL);
        assert(col >= 0 && col + 1 < methods->width(color_pixels_arr));
        assert(row >= 0 && row + 1 < methods->height(color_pixels_arr));

        /* get 4 pixels in block */
        color_space pixel1 = *(color_space *)methods->at(color_pixels_arr,
                                                         col, row);
        color_space pixel2 = *(color_space *)methods->at(color_pixels_arr,
                                                         col + 1, row);
        color_space pixel3 = *(color_space *)methods->at(color_pixels_arr,
                                                         col, row + 1);
        color_space pixel4 = *(color_space *)methods->at(color_pixels_arr,
                                                         col + 1, row + 1);
        /* calculate avg Pb and Pr */
        float avg_Pb = (pixel1.Pb + pixel2.Pb + pixel3.Pb + pixel4.Pb) * 0.25;
        float avg_Pr = (pixel1.Pr + pixel2.Pr + pixel3.Pr + pixel4.Pr) * 0.25;

        /* create average_chroma struct, assign elements */
        average_chroma new_chroma;
        new_chroma.Pb_avg = avg_Pb;
        new_chroma.Pr_avg = avg_Pr;

        return new_chroma;
}



/********** get_scaled_ints ********
 *
 * Converts DCT b,c,d values to integers between -15 and 15 representing 
 * 5-bit signed scaled integers
 *
 * Parameters:
 *      DCT_space DCT   - Contains a,b,c,d coefficients from DCT transformation
 *
 * Return: scaled_ints struct with values between -15 and 15
 *
 * Notes: 
 *      - CRE if b, c or d is out of range, less than -0.5 or greater than 0.5.
 *      - If the values for b, c or d is less than -0.3 or greater 0.3 they are 
 *         changed to -0.3 or 0.3.
 *      - After the integers are scaled, they must be within -15 to 15 range. 
 *
 ************************/
scaled_ints get_scaled_ints(DCT_space DCT)
{
        assert(DCT.b >= -0.5 && DCT.b <= 0.5);
        assert(DCT.c >= -0.5 && DCT.c <= 0.5);
        assert(DCT.d >= -0.5 && DCT.d <= 0.5);

        /* check b, c, d are within -0.3 to 0.3, if not change to 0.3 or -0.3 */
        if (DCT.b <= -0.3) {
                DCT.b = -0.3;
        } else if (DCT.b >= 0.3) {
                DCT.b = 0.3;
        }
        if (DCT.c <= -0.3) {
                DCT.c = -0.3;
        } else if (DCT.c >= 0.3) {
                DCT.c = 0.3;
        }
        if (DCT.d <= -0.3) {
                DCT.d = -0.3;
        } else if (DCT.d >= 0.3) {
                DCT.d = 0.3;
        }
        
        /* scale b, c and d. Check they are in -15 to 15 range */
        scaled_ints scaled;
        int scalar = 15 / 0.3;
        scaled.b = round(DCT.b * scalar);
        assert(scaled.b >= -15 && scaled.b <= 15);
        scaled.c = round(DCT.c * scalar);
        assert(scaled.c >= -15 && scaled.c <= 15);
        scaled.d = round(DCT.d * scalar);
        assert(scaled.d >= -15 && scaled.d <= 15);

        return scaled;
}

/********** bitpack_codeword ********
 *
 * Bitpacks float a, scaled_ints b,c,d, and average chroma values into a 32-bit
 * codeword
 *
 * Parameters:
 *      float a                - Contains a value from DCT transform
 *      scaled_ints scaled     - b,c,d scaled integers on range [-15, 15]
 *      average_chroma chroma  - avg Pb and avg Pr
 *
 * Return: 32-bit codeword
 *
 ************************/
uint32_t bitpack_codeword(float a, scaled_ints scaled, average_chroma chroma)
{
        uint64_t codeword = 0;
        int size = C_CODEWORD_SIZE;

        /* Pack a. Converts a to nine-bit scaled integer*/
        uint64_t scaled_a = round(a * (pow(2, 9) - 1));
        size -= 9;
        codeword = Bitpack_newu(codeword, 9, size, scaled_a);
        
        /* Pack b */
        int64_t five_bit_b = scaled.b;
        size -= C_SIGNED_BITE_SIZE;
        codeword = Bitpack_news(codeword, C_SIGNED_BITE_SIZE, size, five_bit_b);

        /* Pack c */
        int64_t five_bit_c = scaled.c;
        size -= C_SIGNED_BITE_SIZE;
        codeword = Bitpack_news(codeword, C_SIGNED_BITE_SIZE, size, five_bit_c);

        /* Pack d */
        int64_t five_bit_d = scaled.d;
        size -= C_SIGNED_BITE_SIZE;
        codeword = Bitpack_news(codeword, C_SIGNED_BITE_SIZE, size, five_bit_d);

        /* Pack Pb_avg */
        uint64_t Pb_index = Arith40_index_of_chroma(chroma.Pb_avg);
        size -= C_CHROMA_BITE_SIZE;
        codeword = Bitpack_newu(codeword, C_CHROMA_BITE_SIZE, size, Pb_index);
        
        /* Pack Pr_avg */
        uint64_t Pr_index = Arith40_index_of_chroma(chroma.Pr_avg);
        size -= C_CHROMA_BITE_SIZE;
        codeword = Bitpack_newu(codeword, C_CHROMA_BITE_SIZE, size, Pr_index);
        
        return (uint32_t)codeword;
}
