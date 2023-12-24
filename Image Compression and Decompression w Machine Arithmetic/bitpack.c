/******************************************************************************
 *
 *                     bitpack.c
 *
 *     Assignment: arith
 *     Authors:    ihackm01 and wranda01
 *     Date:       10/24/2023
 *
 *     summary:    bitpack.c contains the function definitions for the bitpack.h
 *                 interface
 *
 *****************************************************************************/
#include "bitpack.h"
#include "assert.h"

const unsigned WORD_SIZE = 64;

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/********** Bitpack_fitsu ********
 *
 *  Purpose: Checks if unsigned integer 'n' can be represented in 'width' 
 *  bits without overflow.
 *
 * Parameters:
 *      n (uint64_t): The unsigned integer to check for fit.
 *      width (unsigned): Width representing the number of bits available.
 *
 * Return: 
 *      bool: Returns true if 'n' can be represented in 'width' bits without 
 *      overflow, false otherwise.
 *
 * Notes: 
 *      - If width exceedes WORD_SIZE then CRE is raised. 
 *      - assumed that width >= 0 since unsigned cannot be negative.
 * 
 ************************/
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= WORD_SIZE);

        /* get the minimum shift amount */
        uint64_t shift_mask = (1ULL << width) - 1;
        return n <= shift_mask;
}

/********** Bitpack_fitss ********
 *
 *  Purpose: Checks if signed integer 'n' can be represented in 'width' 
 *  bits without overflow.
 *
 * Parameters:
 *      n (int64_t): The nsigned integer to check for fit.
 *      width (unsigned): Width representing the number of bits available.
 *
 * Return: 
 *      bool: Returns true if 'n' can be represented in 'width' bits without 
 *      overflow, false otherwise.
 *
 * Notes: 
 *      - If width exceedes WORD_SIZE then CRE is raised. 
 *      - Checks that width is between the greatest positive and negative value.
 *      - Assumed that width >= 0 since unsigned cannot be negative.
 * 
 ************************/
bool Bitpack_fitss(int64_t n, unsigned width)
{
        assert(width <= WORD_SIZE);

        /* get the maximum and minimum shift amounts */
        int64_t max = (1LL << (width - 1)) - 1;
        int64_t min = -(1ULL << (width - 1));
        return n <= max && n >= min;
}

/********** Bitpack_getu ********
 *
 *  Purpose: Gets an unsigned integer value from a 'word' at a given 
 *           bit position.
 *
 * Parameters:
 *      word (uint64_t): The word from which to extract the value.
 *      width (unsigned): The number of bits to extract.
 *      lsb (unsigned): The least significant bit position of the value.
 *
 * Return: 
 *      uint64_t: The extracted unsigned integer value.
 *
 * Notes: 
 *      - CRE if width greater than WORD_SIZE and if width + lsb is greater 
 *        than WORD_SIZE
 ************************/
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= WORD_SIZE && width + lsb <= WORD_SIZE);

        /* handles case of shifting by 64 bits */
        if (width == WORD_SIZE && lsb == 0) {
                return word;
        }
        /* handles case if width is 0 */
        if (width == 0) {
                return 0;
        }

        /* calculate mask shifted to position of value */
        uint64_t shift_mask = ((1ULL << width) - 1) << lsb;
        return (word & shift_mask) >> lsb;
}

/********** Bitpack_gets ********
 *
 *  Purpose: Gets signed integer value from a 'word' at a given 
 *           bit position.
 *
 * Parameters:
 *      word (uint64_t): The word from which to extract the value.
 *      width (unsigned): The number of bits to extract.
 *      lsb (unsigned): The least significant bit position of the value.
 *
 * Return: 
 *      int64_t: The extracted unsigned integer value.
 *
 * Notes: 
 *      - CRE if width greater than WORD_SIZE and if width + lsb is greater 
 *        than WORD_SIZE
 *      - If size bit is one, sign extraction is performed
 ************************/
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= WORD_SIZE && width + lsb <= WORD_SIZE);

        /* handles case of shifting by 64 bits */
        if (width == WORD_SIZE && lsb == 0) {
                return word;
        }
        /* handles case if width is 0 */
        if (width == 0) {
                return 0;
        }

        uint64_t shift_mask = ((1LL << width) - 1) << lsb;
        /* get the value of word shifting back from width */
        int64_t value = (word & shift_mask) >> lsb;
        int64_t sign_bit = 1LL << (width - 1);
        
        /* sign extraction for negative numbers */
        if (value & sign_bit) {
                value |= ~((1LL << width) - 1);
        }
        return value;
}

/********** Bitpack_newu ********
 *
 *  Purpose: Inserts an unsigned integer value into 'word' at a given 
 *           bit position.
 *
 * Parameters:
 *      word (uint64_t): The word in which to insert the value.
 *      width (unsigned): The number of bits to insert.
 *      lsb (unsigned): The least significant bit position for insertion.
 *      value (uint64_t): The unsigned integer value to insert.
 *
 * Return: 
 *      uint64_t: The word with the new value inserted.
 *
 * Notes: 
 *      - An assertion failure is raised if 'width' exceeds WORD_SIZE or if 
 *       'width' + 'lsb' exceeds WORD_SIZE.
 *      - Checks for overflow when inserting the value using the 
 *        Bitpack_fitsu function. If so Bitpack_Overflow exception is raised
 * 
 ************************/
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
        assert(width <= WORD_SIZE && width + lsb <= WORD_SIZE);
        if (width == 0) {
                return word;
        }
        /* handles case of shifting by 64 bits */
        if (width == WORD_SIZE && lsb == 0) {
                return value;
        }

        /* check for overflow */
        if (!Bitpack_fitsu(value, width)) {
                RAISE(Bitpack_Overflow);
        }
        /* calculate mask of 1s at values except wher value should go */
        uint64_t mask = ~(((1ULL << width) - 1) << lsb);
        /* add value to word */
        return (word & mask) | (value << lsb);
}

/********** Bitpack_news ********
 *
 *  Purpose: Inserts an signed integer value into 'word' at a given 
 *           bit position.
 *
 * Parameters:
 *      word (uint64_t): The word in which to insert the value.
 *      width (unsigned): The number of bits to insert.
 *      lsb (unsigned): The least significant bit position for insertion.
 *      value (uint64_t): The unsigned integer value to insert.
 *
 * Return: 
 *      uint64_t: The word with the new value inserted.
 *
 * Notes: 
 *      - An assertion failure is raised if 'width' exceeds WORD_SIZE or if 
 *       'width' + 'lsb' exceeds WORD_SIZE.
 *      - Checks for overflow when inserting the value using the 
 *        Bitpack_fitss function. If so Bitpack_Overflow exception is raised
 * 
 ************************/
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                      int64_t value)
{
        assert(width <= WORD_SIZE && width + lsb <= WORD_SIZE);
        
        /* handles case if width is 0 */
        if (width == 0) {
                return word;
        }
        /* handles case of shifting by 64 bits */
        if (width == WORD_SIZE && lsb == 0) {
                return value;
        }
        
        /* check for overflow */
        if (!Bitpack_fitss(value, width)) {
                RAISE(Bitpack_Overflow);
        }
        /*  shift to last position in value*/
        value = value << (WORD_SIZE - width);
        /* cast to unsigned int and shift back */
        uint64_t new_value = (uint64_t)value;
        new_value = new_value >> (WORD_SIZE - width);
        /* calls newu to perform shift */
        return Bitpack_newu(word, width, lsb, new_value);
}
