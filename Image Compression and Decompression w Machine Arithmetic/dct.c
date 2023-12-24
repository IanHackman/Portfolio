/******************************************************************************
 *
 *                     dct.c
 *
 *     Assignment: arith
 *     Authors:    ihackm01 and wranda01
 *     Date:       10/24/2023
 *
 *     Summary:    dct.c contains implements for the dct interface, allowing
 *                 the client to execute discrete cosine transforms and 
 *                 inverse discrete cosine transforms.
 *
 *****************************************************************************/
#include "dct.h"

typedef A2Methods_UArray2 A2;


/********** pixel_to_DCT ********
 *
 * Converts luma values of each pixel into a,b,c,d coefficients
 *
 * Parameters:
 *      object_methods_container color_space_cont 
 *                    - contains Y/Pb/Pr array and methods suite
 *      int col       - col index of top left element in current pixel block
 *      int row       - row index of top left element in current pixel block
 *
 * Return: struct with DCT coefficients
 *
 * Notes: 
 *      - CRE if the color_space_cont is NULL
 *      - CRE if the color_space_arr is NULL
 *      - CRE if the methods is NULL
 *      - CRE if the col or row is out of bounds
 *
 ************************/
DCT_space pixel_to_DCT(object_methods_container color_space_cont,
                       int col, int row)
{
        A2Methods_T methods = color_space_cont.methods;
        A2 color_pixels_arr = *(A2 *)color_space_cont.object;
        assert(methods != NULL);
        assert(color_pixels_arr != NULL);
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
        /* calculate a, b, c and d */
        DCT_space coefficients;
        coefficients.a = (pixel4.Y + pixel3.Y + pixel2.Y + pixel1.Y) / 4.0;
        coefficients.b = (pixel4.Y + pixel3.Y - pixel2.Y - pixel1.Y) / 4.0;
        coefficients.c = (pixel4.Y - pixel3.Y + pixel2.Y - pixel1.Y) / 4.0;
        coefficients.d = (pixel4.Y - pixel3.Y - pixel2.Y + pixel1.Y) / 4.0;

        return coefficients;
}


/********** get_inverse_DCT ********
 *
 * Gets a set of Y values for a 2x2 pixel block using inverse a discrete cosine
 * transform.
 *
 * Parameters:
 *      unpacked_vals values - contains a,b,c,d coefficients for a 2x2 block
 *
 * Return: inverse_DCT struct containing Y values for each pixel in a 2x2 block
 *
 ************************/
inverse_DCT get_inverse_DCT(unpacked_vals values)
{
        /* get a, b, c and d values */
        inverse_DCT inverse;
        float a = values.a;
        float b = values.b;
        float c = values.c;
        float d = values.d;

        /* calculate inverse */
        inverse.Y1 = a - b - c + d;
        inverse.Y2 = a - b + c - d;
        inverse.Y3 = a + b - c - d;
        inverse.Y4 = a + b + c + d;

        return inverse;
}