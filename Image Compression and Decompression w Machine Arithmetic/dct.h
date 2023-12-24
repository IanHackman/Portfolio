/******************************************************************************
 *
 *                     dct.h
 *
 *     Assignment: arith
 *     Authors:    ihackm01 and wranda01
 *     Date:       10/24/2023
 *
 *     Summary:    dct.h is an interface allowing the client to execute
 *                 discrete cosine transforms and inverse discrete cosine
 *                 transforms.
 *
 *****************************************************************************/
#ifndef DCT_H_
#define DCT_H_
#include <stdio.h>
#include <math.h>
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "assert.h"

/*
 * Stores data on a 2x2 pixel block after running a discrete cosine transform
 */
typedef struct DCT_space {
        float a;
        float b;
        float c;
        float d;
} DCT_space;

/*
 * Stores the Y values for four pixels in a 2x2 block obtained by running an
 * inverse discrete cosine transform
 */
typedef struct inverse_DCT {
        float Y1;
        float Y2;
        float Y3;
        float Y4;
} inverse_DCT;

/* Stores the values stored in each codework in an unpacked format */
typedef struct unpacked_vals {
        float a;
        float b;
        float c;
        float d;
        float avg_Pb;
        float avg_Pr;
} unpacked_vals;

/*
 * Stores a generic object and the methods suite for A2 arrays.
 * This abstraction is very useful because we can pass an A2 object or an 
 * object which contains an A2 object (such as a Pnm_ppm object) alongside
 * A2's associated method suite.
 */
typedef struct object_methods_container {
        void *object;
        A2Methods_T methods;
} object_methods_container;

/*
 * This struct contains Y/Pb/Pr values for a given pixel.
 * These values are convert from RGB values.
 */
typedef struct color_space {
        float Y;
        float Pb;
        float Pr;
} color_space;


DCT_space pixel_to_DCT(object_methods_container color_space_cont,
                       int col, int row);
                       
inverse_DCT get_inverse_DCT(unpacked_vals values);

#endif