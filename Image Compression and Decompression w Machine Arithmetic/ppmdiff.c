#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "pnm.h"
#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"

int main(int argc, char *argv[]) 
{
        assert(argc == 3);
        FILE *fp1 = NULL;
        FILE *fp2 = NULL;
        if (strcmp(argv[1], "-") == 0) {
                fp1 = stdin;
        } else {
                fp1 = fopen(argv[1], "r");
        }

        if (strcmp(argv[2], "-") == 0) {
                fp2 = stdin;
        } else {
                fp2 = fopen(argv[2], "r");
        }
        
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);

        Pnm_ppm pixmap1 = Pnm_ppmread(fp1, methods);
        Pnm_ppm pixmap2 = Pnm_ppmread(fp2, methods);
        assert(pixmap1 != NULL);
        assert(pixmap2 != NULL);

        A2Methods_UArray2 pixels1 = pixmap1->pixels;
        A2Methods_UArray2 pixels2 = pixmap2->pixels;
        assert(pixels1 != NULL);
        assert(pixels2 != NULL);

        if (methods->width(pixels1) - methods->width(pixels2) > 1 || 
            methods->width(pixels1) - methods->width(pixels2) < -1) {
                fprintf(stderr, 
                        "Difference in widths cannot be greater than 1.\n");
                printf("1.0\n");
                return 1;
        }
        if (methods->height(pixels1) - methods->height(pixels2) > 1 || 
            methods->height(pixels1) - methods->height(pixels2) < -1) {
                fprintf(stderr, 
                        "Difference in heights cannot be greater than 1.\n");
                printf("1.0\n");
                return 1;
        }

        int small_width;
        int small_height;

        if (methods->width(pixels1) < methods->width(pixels2)) {
                small_width = methods->width(pixels1);
        } else {
                small_width = methods->width(pixels2);
        }
        
        if (methods->height(pixels1) < methods->height(pixels2)) {
                small_height = methods->height(pixels1);
        } else {
                small_height = methods->height(pixels2);
        }

        float sum = 0;
        float denominator = 3 * small_width * small_height;

        for (int r = 0; r < small_height; r++) {
                for (int c = 0; c < small_width; c++) {
                        Pnm_rgb pixel1 = (Pnm_rgb)methods->at(pixels1, c, r);
                        assert(pixel1 != NULL);
                        Pnm_rgb pixel2 = (Pnm_rgb)methods->at(pixels2, c, r);
                        assert(pixel2 != NULL);

                        float R = ((int)pixel1->red - (int)pixel2->red)
                                / (float) pixmap1->denominator;

                        float G = ((int)pixel1->green - (int)pixel2->green)
                                / (float) pixmap1->denominator;
                        
                        float B = ((int)pixel1->blue - (int)pixel2->blue)
                                / (float) pixmap1->denominator;
                        
                        float curr_num = pow(R, 2) + pow(G, 2) + pow(B, 2);
                        
                        sum += curr_num;
                }
        }
        float E = sqrt(sum / denominator);
        
        printf("%.4f\n", E);

        Pnm_ppmfree(&pixmap1);
        Pnm_ppmfree(&pixmap2);
        fclose(fp1);
        fclose(fp2);

        return 0;
}
