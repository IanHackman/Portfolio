/**************************************************************
 *
 *                     instruction_set.c
 *
 *     Assignment: um
 *     Authors:  Will Randall (wranda01), Ian Hackman (ihackm01)
 *     Date:     11/20/2023
 *
 *     instruction_set.c implements the 14 three-register instructions that 
 *     perform computation over 32-bit words and their registers. 
 *
 **************************************************************/
#include "instruction_set.h"

/* Sentinel to halt the fetch-decode-execute loop */
#define END_OF_PROGRAM -1

/********* conditional_move ******************
 * If $r[C] is not 0, $r[A] is set to the value in $r[B]
 *
 * Input:
 *      - uint32_t *rA : register A passed by reference
 *      - uint32_t rB  : register B passed by value
 *      - uint32_t rC  : register C passed by value
 *
 * Updates:
 *      - Updates the value stored in register A if register C does not
 *        contain zero
 *
 * Notes:
 *      - CRE if rA == NULL
 *********************************************/
void conditional_move(uint32_t *rA, uint32_t rB, uint32_t rC)
{
        assert(rA != NULL);
        if (rC != 0) {
                *rA = rB;
        }
}

/********* segmented_load ******************
 * Loads a value from memory into a register.
 *
 * Input:
 *      - program_memory memory contains the memory table of the program
 *      - uint32_t *rA : register which will be updated to contain 
 *                       value from memory
 *      - uint32_t rB  : contains segment number of relevant word in memory
 *      - uint32_t rC  : contains offset within segment of relevant word
 *
 * Updates:
 *      - Updates the value stored in register A to store $m[$r[B]][$r[C]]
 *
 * Notes:
 *      - CRE if memory == NULL
 *      - CRE if rA == NULL
 *      - CRE if rB contains the id of an unmapped segment
 *      - CRE if rC contains an offset outside the bounds of segment rB
 *********************************************/
void segmented_load(program_memory pm, uint32_t *rA, uint32_t rB, uint32_t rC)
{
        assert(pm != NULL);
        assert(rA != NULL);
        *rA = read_from_mem(pm, rB, rC);
}

/********* segmented_store ******************
 * Stores a value in a specific location in memory
 *
 * Input:
 *      - program_memory memory contains the memory table of the program
 *      - uint32_t rA  : contains segment number of memory location
 *      - uint32_t rB  : contains offset within relevant segment in memory
 *      - uint32_t rC  : stores the value that will be written to memory
 *
 * Updates:
 *      - $m[$r[A]][$r[B]] := $r[C]
 *
 * Notes:
 *      - CRE if memory == NULL
 *      - CRE if rA contains the id of an unmapped segment
 *      - CRE if rB contains an offset outside the bounds of segment rA
 *********************************************/
void segmented_store(program_memory pm, uint32_t rA, uint32_t rB, uint32_t rC)
{
        assert(pm != NULL);
        write_to_mem(pm, rA, rB, rC);     
}

/********* addition ******************
 * Adds the value of two registers and stores the result in a third register.
 *
 * Input:
 *      - uint32_t *rA : Will be updated to contain ($r[B] + $r[C]) mod 2^32
 *      - uint32_t rB  : Contains a value that will be summed with $r[C]
 *      - uint32_t rC  : Contains a value that will be summed with $r[B]
 *
 * Updates:
 *      - $r[A] := ($r[B] + $r[C]) mod 2^32
 *
 * Notes:
 *      - CRE if rA == NULL
 *********************************************/
void addition(uint32_t *rA, uint32_t rB, uint32_t rC)
{
        assert(rA != NULL);
        *rA = rB + rC;
}  

/********* multiplication ******************
 * Multiplies the value of two registers and stores the result in a third
 * register.
 *
 * Input:
 *      - uint32_t *rA : Will be updated to contain ($r[B] * $r[C]) mod 2^32
 *      - uint32_t rB  : Contains a value that will be multiplied with $r[C]
 *      - uint32_t rC  : Contains a value that will be multiplied with $r[B]
 *
 * Updates:
 *      - $r[A] := ($r[B] * $r[C]) mod 2^32
 *
 * Notes:
 *      - CRE if rA == NULL
 *********************************************/
void multiplication(uint32_t *rA, uint32_t rB, uint32_t rC)
{
        assert(rA != NULL);
        *rA = rB * rC;
}

/********* division ******************
 * Divides the value of two registers and stores the result in a third
 * register.
 *
 * Input:
 *      - uint32_t *rA : Will be updated to contain: floor($r[B] / $r[C])
 *      - uint32_t rB  : Contains a numerator
 *      - uint32_t rC  : Contains a denominator
 *
 * Updates:
 *      - $r[A] := floor($r[B] / $r[C])
 *
 * Notes:
 *      - CRE if rA == NULL
 *      - CRE if rC == 0
 *********************************************/
void division(uint32_t *rA, uint32_t rB, uint32_t rC)
{
        assert(rA != NULL);
        assert(rC != 0);
        *rA = rB / rC;
}

/********* bitwise_NAND ******************
 * Conducts a bitwise and of two integers then negates it. Stores the result.
 *
 * Input:
 *      - uint32_t *rA : Will be updated to contain: ~($r[B] & $r[C])
 *      - uint32_t rB  : Contains an integer used in the calculation
 *      - uint32_t rC  : Contains an integer used in the calculation
 *
 * Updates:
 *      - $r[A] := ~($r[B] & $r[C])
 *
 * Notes:
 *      - CRE if rA == NULL
 *********************************************/
void bitwise_NAND(uint32_t *rA, uint32_t rB, uint32_t rC)
{
        assert(rA != NULL);
        *rA = ~(rB & rC);
}


/********* halt *************************
 *
 * Purpose: Stop the computation and frees program_memory struct
 *
 * Parameters: 
 *      - program_memory pm  -  pointer to program memory struct which contains
 *                               mapped segments
 * 
 * Returns: nothing
 * 
 * Notes: 
 *      - sets the program_memory counter to -1 to stop the fetch, decode, 
 *        execute cycle in module um. 
 *      - Calls free_program_memory from module memory_management to free the 
 *        program_memory table and the contents inside it. 
 * 
 *********************************************/
void halt(program_memory pm)
{
        assert(pm != NULL);
        set_program_counter(pm, END_OF_PROGRAM);
}

/********* map_segment ******************
 *
 * Purpose: create a new segment and initialize to 0
 *
 * Parameters: 
 *      - program_memory pm  -  pointer to program memory struct which contains
 *                               mapped segments
 *      - uint32_t *rB : register b passed by reference, will store segment_id
 *      - uint32_t rC  : register c, stores length of segment
 * 
 * Returns: nothing
 * 
 * Notes: 
 *      - creates a new segemnt with number of words equal to the value in 
 *        register c
 *      - Each word in unmapped segemnt initialized ot 0
 *      - A bit patterns that isn't all 0s or that doesn't match with current 
 *        mapped segments is mapped to rB
 *      - Order to adding to sequence:
 *              - If the program_memory unmapped_identifiers statck is not 
 *                empty, map_segment will map the new segment to the identifier
 *                popped from the stack.
 *              - If the stack is empty then add the new segment to the top of
 *                the sequence.
 *
 *********************************************/
void map_segment(program_memory pm, uint32_t rC, uint32_t *rB)
{
        assert(pm != NULL);
        assert(rB != NULL);
        *rB = new_segment(pm, rC);
}

/********* unmap_segment ******************
 *
 * Purpose: Unmaps segment is register C 
 *
 * Parameters: 
 *      - program_memory pm  :  pointer to program memory struct which contains
 *                               mapped segments
 *      - uint32_t rC        :  register c, stores segmemt_id of segment to
 *                               be unmapped
 * 
 * Returns: nothing
 * 
 * Notes: 
 *      - register c is unmapped and free'd from memory, its value is set to 
 *        NULL and its identifier is placed onto the program_memory struct 
 *        unmapped_identifiers stack.
 *      - If the instruction is called on $m[0] or an unmapped segment a CRE is
 *        raised
 *
 *********************************************/
void unmap_segment(program_memory pm, uint32_t rC)
{
        assert(rC != 0);
        free_segment(pm, rC);
}

/********* output ******************
 *
 * Purpose: writes register c to stdout
 *
 * Parameters: 
 *      - uint32_t rc : register c
 * 
 * Returns: nothing
 * 
 * Notes: 
 *      - If register c is not between 0 and 255 a CRE is raised
 *
 *********************************************/
void output(uint32_t rC)
{
        assert(rC < 256);
        putchar(rC);
}

/********* input ******************
 *
 * Purpose: input instruction from stdin into register c
 *
 * Parameters: 
 *      - uint32_t *rC : register c passed by reference, to store input
 * 
 * Returns: nothing
 * 
 * Notes: 
 *      - CRE is rC is NULL
 *      - Input must be a value betwene 0 and 255, if not a CRE is raised.
 *      - If input ends or an EOF has been reached, register c is loaded with a
 *        32-bit word of all 1s. 
 *
 *********************************************/
void input(uint32_t *rC)
{
        assert(rC != NULL);
        /* get first 8 bytes of each word */
        int8_t val = getchar();
        if (val == EOF) {
                /* if end of file place set value to all 1s */
                *rC = 0xffffffff;
        }
        *rC = val;
}

/********* load_program ******************
 *
 * Purpose: loads program from mapped segment
 *
 * Parameters: 
 *      - program_memory pm  : pointer to program memory struct which contains
 *                               mapped segments
 *      - uint32_t rB        : register b, stores segment to be replaced in
 *                                      segment 0
 *      - uint32_t rC        : register c - program counter will store this
 *                                      value
 * 
 * Returns: nothing
 * 
 * Notes: 
 *      - The segment already existing replaces the 0 segment
 *      - The program counter is reset to point to the segment containing rc
 *      - If the instruction tries to load an unmapped program a CRE is rais
 *
 *********************************************/
void load_program(program_memory pm, uint32_t rB, uint32_t rC)
{
        assert(pm != NULL);
        copy_segment(pm, rB, 0);
        set_program_counter(pm, rC);
}

/********* load_value ******************
 *
 * Purpose: Retrieves unsigned binary value from least significant 25 bits in
 *          the word and sets it equal to register a
 *
 * Parameters: 
 *      - uint32_t *rA   : Register in which value will be stored
 *      - uint32_t value : Value to be stored in rA
 * 
 * Returns: nothing
 * 
 * Notes: 
 *      -CRE if rA == NULL
 * 
 *********************************************/
void load_value(uint32_t *rA, uint32_t value)
{
        assert(rA != NULL);
        *rA = value;
}

#undef END_OF_PROGRAM