/******************************************************************************
 *
 *                     decompress40.c
 *
 *     Assignment: arith
 *     Authors:    ihackm01 and wranda01
 *     Date:       10/24/2023
 *
 *     Summary:    decompress40.c reads in a compressed image, and decompresses
 *                 it, outputing the decompressed image to stdout
 *
 *****************************************************************************/
#include "decompress40.h"

const int DENOMINATOR = 255;
const int SIGNED_BITE_SIZE = 5;
const int CHROMA_BITE_SIZE = 4;
const int CODEWORD_SIZE = 32;

typedef A2Methods_UArray2 A2;

/* Struct Declarations */

/* Stores the width and height of a 2 dimensional object */
typedef struct dimensions {
        unsigned width;
        unsigned height;
} dimensions;

/*
 * Stores four RGB value sets in a 2x2 block of pixels.
 *
 * Data member names contain intra-block indices:
 *      i.e. pixel_00 is the top left pixel, pixel_10 is top riight pixel, etc.
 */
typedef struct RGB_block {
        Pnm_rgb pixel_00;
        Pnm_rgb pixel_10;
        Pnm_rgb pixel_01;
        Pnm_rgb pixel_11;
} RGB_block;

/* Helper Function Declarations */
dimensions read_header(FILE *input);
Pnm_ppm convert_codewords_to_image(FILE *input, 
                                   object_methods_container contain);
unpacked_vals get_unpacked_vals(uint32_t codeword);
Pnm_rgb calculate_RGB_pixel(float Y, float Pb, float Pr);
RGB_block convert_to_RGB_block(unpacked_vals values, inverse_DCT inverse);
void fill_pixels_arr(object_methods_container contain, Pnm_rgb rgb_vals,
                                                       int col,
                                                       int row);
float round_val(float val, float min, float max);

/********** decompress40 ********
 *
 * Executes the compression sequence and calls functions that execute de
 *
 * Parameters:
 *      FILE *input - input will contain compressed image 
 *
 * Return: nothing
 *
 * Notes: 
 *      - CRE if the input file in NULL
 ************************/
void decompress40(FILE *input)
{
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);
        assert(input != NULL);
        
        /* get dimensions of image to be decompressed */
        dimensions pic_dims = read_header(input);
        int width = pic_dims.width;
        int height = pic_dims.height;

        /* create array and set pixmap struct */
        A2 RGB_pixels = methods->new(width, height, sizeof(struct Pnm_rgb));
        struct Pnm_ppm pixmap_struct = { .width = width,   
                                  .height = height, 
                                  .denominator = DENOMINATOR, 
                                  .pixels = RGB_pixels, 
                                  .methods = methods };
        Pnm_ppm pixmap = &pixmap_struct;
        
        /* assign container call function to decompress all codewords */
        object_methods_container contain;
        contain.object = &pixmap;
        contain.methods = methods;
        pixmap = convert_codewords_to_image(input, contain);

        /* output image array and free all allocated memory */
        Pnm_ppmwrite(stdout, pixmap);
        methods->free(&RGB_pixels);
}

/********** read_header ********
 *
 * Reads the header of the compressed image which contains the width and
 * the height of the image.
 *
 * Parameters:
 *      FILE *input - input will contain compressed image 
 *
 * Return: dimensions struct containing width and height
 *
 * Notes: 
 *      - CRE if the input file in NULL
 ************************/
dimensions read_header(FILE *input) 
{
        assert(input != NULL);
        /* read in width and height of compressed image */
        unsigned height, width;
        int read = fscanf(input, "COMP40 Compressed image format 2\n%u %u",
                          &width, &height);
        assert(read == 2);
        int c = getc(input);
        assert(c == '\n');

        /* set dimensions of array to be allocated */
        dimensions pic_dims;
        pic_dims.width = width;
        pic_dims.height = height;

        return pic_dims;
}

/********** convert_codewords_to_image ********
 *
 * Reads the codewords from the compressed file and call functions to decompress
 * the values
 *
 * Parameters:
 *      FILE *input - input will contain compressed image 
 *      Pnm_ppm pixmap - pixmap struct of image to be decompressed
 *
 * Return: nothing
 *
 * Notes: 
 *      - CRE if the input file or pixmap are NULL
 *      - traverses the array by 2x2 blocks 
 ************************/
Pnm_ppm convert_codewords_to_image(FILE *input,
                                   object_methods_container contain)
{
        Pnm_ppm pixmap = *(Pnm_ppm *)contain.object;
        assert(input != NULL && pixmap != NULL);

        /* traverse empty array block by block*/
        for(unsigned row = 0; row < pixmap->height; row += 2) {
                for(unsigned col = 0; col < pixmap->width; col += 2) {
                        
                        /* get codeword from compressed file */
                        uint32_t codeword = 0;
                        int byte;
                        for (int i = 0; i < 4; i++) {
                                byte = fgetc(input);
                                assert(byte != EOF);
                                codeword = (codeword << 8) | (byte & 0xFF);
                        }

                        /* function calls to decompress codeworsd values */
                        unpacked_vals vals = get_unpacked_vals(codeword);
                        inverse_DCT inv = get_inverse_DCT(vals);
                        RGB_block block = convert_to_RGB_block(vals, inv);

                        /* get 4 pixels in 2x2 block */
                        Pnm_rgb rgb_vals_00 = block.pixel_00;
                        Pnm_rgb rgb_vals_10 = block.pixel_10;
                        Pnm_rgb rgb_vals_01 = block.pixel_01;
                        Pnm_rgb rgb_vals_11 = block.pixel_11;

                        /* add pixels to array */
                        fill_pixels_arr(contain, rgb_vals_00, col, row);
                        fill_pixels_arr(contain, rgb_vals_10, col + 1, row);
                        fill_pixels_arr(contain, rgb_vals_01, col, row + 1);
                        fill_pixels_arr(contain, rgb_vals_11, col + 1, row + 1);
                }
        }
        return pixmap;
}

/********** fill_pixel_arr ********
 *
 * Fills a slot in a pixel array with a given RGB value at a given index
 *
 * Parameters:
 *      object_methods_container contain - holds pixmap and methods suite
 *      Pnm_pgb rgb_vals                 - RGB value to be placed in array
 *      int col                          - Col index
 *      int row                           *
 *
 * Return: nothing
 *
 * Notes: 
 *      - CRE if pixmap, pixels, methods, or rgb_vals is NULL
 *      - Frees rgb_vals
 ************************/
void fill_pixels_arr(object_methods_container contain, Pnm_rgb rgb_vals,
                                                       int col,
                                                       int row)
{
        /* get array and methods  */
        Pnm_ppm pixmap = *(Pnm_ppm *)(contain.object);
        A2 pixels = pixmap->pixels;
        A2Methods_T methods = contain.methods;
        assert(pixmap != NULL && methods != NULL && 
               pixels != NULL && rgb_vals != NULL);

        /* get location of pixel in array and assign its values */
        Pnm_rgb curr_pix = (Pnm_rgb)methods->at(pixels, col, row);
        curr_pix->red = rgb_vals->red;
        curr_pix->green = rgb_vals->green;
        curr_pix->blue = rgb_vals->blue;

        /* FREE RGB pixel alloc'd in calculate_RGB_pixel */
        FREE(rgb_vals);
}

/********** get_unpacked_vals ********
 *
 * Takes in a 32-bit codeword and unpacks its values
 *
 * Parameters:
 *      uint32_t codeword - 32-bit codeword
 *
 * Return: unpacked_vals struct containing unpacked values of a,b,c,d and 
 *         average values of Pb and Pr.
 *
 ************************/
unpacked_vals get_unpacked_vals(uint32_t codeword)
{
        unpacked_vals values;
        int size = CODEWORD_SIZE;
        
        int scalar = 15 / 0.3;

        /* unpack a, b, c, d, Pb and Pr from codeword, scale each */
        size -= 9;
        float a = Bitpack_getu(codeword, 9, size) / (float)(pow(2, 9) - 1);
        values.a = round_val(a, 0, 1);
        
        size -= SIGNED_BITE_SIZE;
        float b = Bitpack_gets(codeword, SIGNED_BITE_SIZE, size) 
                                                                / (float)scalar;
        values.b = round_val(b, -0.3, 0.3);
        
        size -= SIGNED_BITE_SIZE;
        float c = Bitpack_gets(codeword, SIGNED_BITE_SIZE, size) 
                                                                / (float)scalar;
        values.c = round_val(c, -0.3, 0.3);
        
        size -= SIGNED_BITE_SIZE;
        float d = Bitpack_gets(codeword, SIGNED_BITE_SIZE, size) 
                                                                / (float)scalar;
        values.d = round_val(d, -0.3, 0.3);

        /* get chroma index for Pb and Pr */
        size -= CHROMA_BITE_SIZE;
        values.avg_Pb = Arith40_chroma_of_index
                               (Bitpack_getu(codeword, CHROMA_BITE_SIZE, size));
        values.avg_Pb = round_val(values.avg_Pb, -0.5, 0.5);
        size -= CHROMA_BITE_SIZE;
        values.avg_Pr = Arith40_chroma_of_index
                               (Bitpack_getu(codeword, CHROMA_BITE_SIZE, size));
        values.avg_Pr = round_val(values.avg_Pr, -0.5, 0.5);

        return values;
}

/********** calculate_RGB_pixel ********
 *
 * Calculates RGB value for each pixel 
 *
 * Parameters:
 *      float Y - represents brightness of a color
 *      float Pb - chroma element of pixel
 *      float Pr - chroma element of pixel
 *
 * Return: Pnm_rgb struct representing 1 pixel
 *
 * Notes: If red, blue or green are negative they are corrected to 0. 
 *      - Pnm_rgb pixel is alloc'd here and free'd fill_pixels_arr
 *
 ************************/
RGB_block convert_to_RGB_block(unpacked_vals values, inverse_DCT inverse)
{
        float Pb = values.avg_Pb;
        float Pr = values.avg_Pr;

        /* get RGB pixel values for each pixel in 2x2 block */
        RGB_block block;
        block.pixel_00 = calculate_RGB_pixel(inverse.Y1, Pb, Pr);
        block.pixel_10 = calculate_RGB_pixel(inverse.Y2, Pb, Pr);
        block.pixel_01 = calculate_RGB_pixel(inverse.Y3, Pb, Pr);
        block.pixel_11 = calculate_RGB_pixel(inverse.Y4, Pb, Pr);

        return block; 
}

/********** calculate_RGB_pixel ********
 *
 * Calculates RGB value for each pixel 
 *
 * Parameters:
 *      float Y - represents brightness of a color
 *      float Pb - chroma element of pixel
 *      float Pr - chroma element of pixel
 *
 * Return: Pnm_rgb struct representing 1 pixel
 *
 * Notes: If red, blue or green are negative they are corrected to 0. 
 *      - Pnm_rgb pixel is alloc'd here and free'd fill_pixels_arr
 *
 ************************/
Pnm_rgb calculate_RGB_pixel(float Y, float Pb, float Pr)
{
        /* ALLOC new pixel */
        Pnm_rgb pixel = ALLOC(sizeof(struct Pnm_rgb));

        /* calcuate red, green and blue values, round up if negative or greater 
         * than DENOMINATOR */
        float red = (1.0 * Y + 0.0 * Pb + 1.402 * Pr) * DENOMINATOR;
        pixel->red = round_val(red, 0, DENOMINATOR);

        float green = (1.0 * Y - 0.344136 * Pb - 0.714136 * Pr) * DENOMINATOR;
        pixel->green = round_val(green, 0, DENOMINATOR);

        float blue = (1.0 * Y + 1.772 * Pb + 0.0 * Pr) * DENOMINATOR;
        pixel->blue = round_val(blue, 0, DENOMINATOR);
        
        return pixel;
}

/********** round_vals ********
 *
 * Rounds a given value to either a minimum or maximum values. 
 *
 * Parameters:
 *      float val - value to be rounded
 *      float min - minimum value to be rounded to
 *      float max - maximum value to be rounded to
 *
 * Return: rounded val
 *
 * Notes: If val is in range it is unchnaged. 
 *
 ************************/
float round_val(float val, float min, float max)
{
        /* check is val is less than min or greater than max */
        if (val < min) {
                val = min;
        } else if (val > max) {
                val = max;
        }

        return val;
}
