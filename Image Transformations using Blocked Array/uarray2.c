/**************************************************************
 *
 *                     uarray2.c
 *
 *     Assignment: iiiwc
 *     Authors:  Will Randall (wranda01), Joseph Carpenter (jcarpe04)
 *     Date:     9/28/23
 *
 *     The implementation of the uarray2 program. Clients will use this
 *     program when they need to utilize a 2 dimensional array-like structure
 *     with elements of any size.
 *
 **************************************************************/

#include "uarray2.h"


struct UArray2_T {
        int width;
        int height;
        int size;
        UArray_T uarray;
};



/********** UArray2_new ********
 *
 * Creates a new 2d array of a given width and height, containing elements
 * of a given size in bytes. Bytes in each element are initialized to zero.
 *
 *
 * Parameters:
 *      int width:      number of columns in the desired 2d array
 *      int height:     number of rows in teh desired 2d array
 *      int size:       number of bytes in each element
 *
 * Return:
 *      Pointer to newly created UArray_T
 *
 * Notes:
 *      CRE if width or height is negative
 *      CRE if size is non-positive
 *      CRE if UArray failed to initialize (i.e. if ALLOC throws a CRE)
 ************************/
extern UArray2_T UArray2_new(int width, int height, int size)
{

        assert(width >= 0 && height >= 0);
        assert(size > 0);

        UArray2_T uarray2;
        NEW(uarray2);
        assert(uarray2 != NULL);

        uarray2->width = width;
        uarray2->height = height;
        uarray2->size = size;
        uarray2->uarray = UArray_new(width * height, size);
        assert(uarray2->uarray != NULL);
        


        return uarray2;


}


/********** UArray2_free ********
 *
 * Frees memory allocated to a UArray2_T 
 *
 *
 * Parameters:
 *      UArray_T *uarray2_ptr :  Pointer to a UArray2_T that needs to be freed
 *
 * Return: void
 *
 * Notes:
 *      CRE if uarray2_ptr is NULL;
 *
 ************************/
extern void UArray2_free(UArray2_T *uarray2_ptr)
{
        assert(uarray2_ptr != NULL);
        assert(*uarray2_ptr != NULL);


        UArray_free(&((*(uarray2_ptr))->uarray));
        FREE(*uarray2_ptr);
}


/********** UArray2_width ********
 *
 * Gets the width of a UArray2_T
 *
 *
 * Parameters:
 *      UArray2_T *uarray2 :  UArray2_T ptr to get the width from
 *
 * Return: (int) width of uarray2
 *
 * Notes:
 *      CRE if uarray2 or *uarray2 is NULL;
 ************************/
extern int UArray2_width(UArray2_T uarray2)
{
        assert(uarray2 != NULL);
        assert(uarray2->uarray != NULL);

        return uarray2->width;
}

/********** UArray2_height ********
 *
 * Gets the height of a UArray2_T
 *
 *
 * Parameters:
 *      UArray2_T *uarray2 :  pointer to UArray2_T to get the height from
 *
 * Return:
 *      (int) width of uarray2
 *
 * Notes:
 *      CRE if uarray2 or *uarray2 is NULL;
 ************************/
extern int UArray2_height(UArray2_T uarray2)
{
        assert(uarray2 != NULL);
        assert(uarray2->uarray != NULL);

        return uarray2->height;
}

/********** UArray2_size ********
 *
 * Gets the size of the elements of a UArray2_T in bytes
 *
 *
 * Parameters:
 *      UArray2_T *uarray2 :  pointer to UArray2_T whose element size we 
 *                            want to return
 *
 * Return: 
 *      (int) element size of uarray2
 *
 * Notes:
 *      CRE if uarray2 or *uarray2 is NULL;
 ************************/
extern int UArray2_size(UArray2_T uarray2)
{
        assert(uarray2 != NULL);
        assert(uarray2->uarray != NULL);

        return uarray2->size;
}


/********** UArray2_at ********
 *
 * Returns a pointer to a particular element in the uarray2
 *
 *
 * Parameters:
 *      UArray2_T uarray2 :  pointer to UArray2_T which we want an element from
 *      int col           :  col index of element we want
 *      int row           :  row index of element we want 
 * 
 * Return: 
 *       pointer to element at given index
 * 
 * Notes:
 *      CRE if indices out of range
 *      CRE if pointer to uarray2 is null 
 *      
 ************************/
void *UArray2_at(UArray2_T uarray2, int col, int row)
{
        /* Used this formula to convert between 2d array and an under-the-hood 
         * representation as a 1d array
         */
        assert(uarray2 != NULL);
        int index = uarray2->width * row + col;

        return UArray_at(uarray2->uarray, index);
}


/********** UArray2_map_row_major ********
 *
 * Calls an apply function on every element of a UArray2, in row major order
 * (column indices vary more rapidly than row indices)
 *
 * Parameters:
 *      UArray2_T uarray2      :  UArray2_T on which apply is called
 *      void apply(...)        :  apply function which will be called on each
 *                                     element of uarray2 (more info below)
 *      void *cl               :  supplied by the client to potentially be 
 *                                utilized by apply
 * 
 * Return: 
 *       None
 * 
 * 
 * apply parameters
 *      int col           :  col index of element on which apply is called
 *      int row           :  row index of element on which apply is called
 *      UArray2_T uarray2 :  UArray2_T on which apply is called
 *      void *value       :  value of element at uarray2[col][row]
 *      void *cl          :  an application-specifc parameter supplied by the
 *                           client, which we supply to apply()
 * 
 * 
 * Notes:
 *      CRE if uarray2 is null 
 *      CRE if apply is NULL
 *      
 ************************/
void UArray2_map_row_major(UArray2_T uarray2,
                           void apply(int col, int row, UArray2_T uarray2,
                                      void *value, void *cl),
                           void *cl)
{
        assert(uarray2 != NULL);
        assert(apply != NULL);

        /*
         * Calls apply function on every element in uarray2 in row-major order
         */
        for (int r = 0; r < UArray2_height(uarray2); r++) {
                for (int c = 0; c < UArray2_width(uarray2); c++) {
                        apply(c, r, uarray2, UArray2_at(uarray2, c, r), cl);
                }
        }
        
}

/********** UArray2_map_col_major ********
 *
 * Calls an apply function on every element of a UArray2, in column major order
 * (row indices vary more rapidly than column indices)
 *`
 * Parameters:
 *      UArray2 uarray2      :  uarray2 on which apply is called
 *      void apply(...)      :  apply function which will be called on each 
 *                              element of uarray2 (more info below)
 *      void *cl             :  supplied by the client to potentially be 
 *                              utilized by apply
 * 
 * Return: 
 *       None
 * 
 * 
 * apply parameters
 *      int width         :  width index of element on which apply is called
 *      int height        :  height index of element on which apply is called
 *      UArray2_T uarray2 :  UArray2_T on which apply is called
 *      void *value       :  value of element at uarray2[width][height]
 *      void *cl          :  an application-specifc parameter supplied by the
 *                           client, which we supply to apply()
 * 
 * 
 * Notes:
 *      CRE if indices out of range
 *      CRE if uarray2 is NULL 
 *      CRE if apply is NULL
 *      
 ************************/
void UArray2_map_col_major(UArray2_T uarray2,
                           void apply(int col, int row, UArray2_T uarray2,
                                      void *value, void *cl),
                           void *cl)
{
        assert(uarray2 != NULL);
        assert(apply != NULL);

        /*
         * Calls apply function on every element in uarray2 in col-major order
         */
        for (int c = 0; c < UArray2_width(uarray2); c++) {
                for (int r = 0; r < UArray2_height(uarray2); r++) {
                        apply(c, r, uarray2, UArray2_at(uarray2, c, r), cl);
                }
        }
        
}