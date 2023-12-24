/**************************************************************
 *
 *                     memory_management.c
 *
 *     Assignment: um
 *     Authors:  Will Randall (wranda01), Ian Hackman (ihackm01)
 *     Date:     11/20/2023
 *
 *     memory_management.c implements the collection of memory segments and 
 *     functions that interact with the it.
 *
 **************************************************************/
#include "memory_management.h"

#define DEFAULT_SEG_SIZE 32

/* Sentinel to halt the fetch-decode-execute loop */
#define END_OF_PROGRAM -1

#define BITS_PER_BYTE 8

/********* struct program_memory ******** 
 *
 * Each instance of this struct contains the sequence of segments. Each segment
 * is a sequence of 32-bit instructions. The struct also contains a uint32_t 
 * program counter that identifies the current position in the sequence when 
 * executing each segment. 
 * 
 * The struct also contains a sequence of unmapped identifiers: identifiers
 * which previously stored the id of a segment which has since been unmapped.
 * This will be used so that id's can be reused after segments are unmapped.
 *
 * The struct is typedefined as a pointer as it will be passed by reference. 
 *
 ************************/
struct program_memory {
        Seq_T memory_segments;
        Stack_T unmapped_ids;
        int32_t program_counter;
};

/********* new_program_memory ******************
 * 
 * Creates a new instance program_memory struct, and initializes it's values
 * to the appropriate defaults.
 *
 * Input: N/A
 *
 * Returns:
 *      - program_memory : A new struct
 *              Default values:
 *                      - Seq_T memory_segments
 * 
 * Notes:
 *      - It is responsibility of user to free the program_memory instance 
 *              using the function free_program_memory.
 *      - CREs if memory allocation fails
 *
 *********************************************/
program_memory new_program_memory()
{
        /*  create empty instance of struct then of each element in the struct,
         *  sequence, stack and progrm_counter */
        program_memory new_program_memory = ALLOC(sizeof(*new_program_memory));
        assert(new_program_memory != NULL);

        new_program_memory->memory_segments = Seq_new(DEFAULT_SEG_SIZE);
        assert(new_program_memory->memory_segments != NULL);
        
        new_program_memory->unmapped_ids = Stack_new();
        assert(new_program_memory->unmapped_ids != NULL);
        
        new_program_memory->program_counter = 0;
        
        return new_program_memory;
}

/********* get_program_counter ******************
 * 
 * Gets the value of the program_counter data member of a program_memory
 * instance.
 *
 * Input:
 *      program_memory memory : program_memory instance
 *
 * Returns:
 *      - uint32_t program_counter : counter indicating the program's location 
 *                                   in memory.
 * 
 * Notes:
 *      - CRE if program_memory struct is NULL
 *
 *********************************************/
int32_t get_program_counter(program_memory memory)
{
        assert(memory != NULL);
        return memory->program_counter;
}

/********* increment_program_counter ******************
 * 
 * Increments the value of the program_counter data member of a program_memory
 * instance.
 *
 * Input:
 *      program_memory memory : program_memory instance
 *
 * Returns: None
 * 
 * Notes:
 *      - CRE if program_memory struct is NULL
 *
 *********************************************/
void increment_program_counter(program_memory memory)
{
        assert(memory != NULL);
        (memory->program_counter)++;
}

/********* set_program_counter ***************
 * 
 * Sets the value of the program_counter data member of a program_memory
 * instance to val;
 *
 * Input:
 *      program_memory memory : program_memory instance
 *      int32_t val           : value to be placed in program counter
 *
 * Returns: None
 * 
 * Notes:
 *      - CRE if program_memory struct is NULL
 *      - CRE is val is less than -1
 *
 *********************************************/
void set_program_counter(program_memory memory, int32_t val)
{
        assert(memory != NULL);
        assert(val >= -1);

        memory->program_counter = val;
}

/********* create_segment_zero ***************
 * 
 * Reads in the instructions from the input file and places them in segment 0
 *
 * Input:
 *      program_memory memory : program_memory instance
 *      File *input           : File stream with the program instructions
 *
 * Returns: None
 * 
 * Notes:
 *      - CRE if program_memory struct is NULL
 *      - CRE if input is NULL
 *      - Note that program_counter is set to zero in new_program_memory
 *
 *********************************************/
void create_segment_zero(program_memory memory, FILE *input)
{
        assert(memory != NULL && input != NULL);

        /* create empty segment */
        Seq_T new_segment = Seq_new(0);
        assert(new_segment != NULL);

        /* read in each word from input, add to segment zero */
        uint32_t instruction = 0;
        while (!feof(input)) {
                /* get instrction in Big-Endian order */
                for (int i = 3; i >= 0; i--) {
                        uint8_t byte = getc(input);

                        instruction = Bitpack_newu(instruction, BITS_PER_BYTE,
                                                   i * BITS_PER_BYTE, byte);
                }
                Seq_addhi(new_segment, (void *)(uintptr_t)instruction);
                instruction = 0;
        }
        /* add segment zero to program memory */
        (void)Seq_addhi(memory->memory_segments, new_segment);  
}

/********* new_segment ******************
 * 
 * Creates a new segment in memory and returns its identifier.
 *
 * Input:
 *      - program_memory memory : Sequence of memory segments
 *      - uint32_t length       : the length of the segment to be initialized
 *
 * Updates:
 *      - program_memory struct (passed by reference) with new segment
 *
 * Returns:
 *      - Identifier of new segment
 *
 * Choosing an identifier:
 *      - Identifier, if possible, is popped from unmapped_ids stack,
 *        so that old ids can be resued. If the stack is empty, a new id
 *        segment is created at the end of the sequence of segments.
 * 
 * Notes:
 *      - Each word in new segment is initialized to 0
 *      - In the case of mapping a new segment to an existing one whose address
 *        is on the stack, the existing segment is free'd here and not in 
 *        free_segment.
 *
 *********************************************/
uint32_t new_segment(program_memory memory, uint32_t length)
{
        assert(memory != NULL);
        assert(memory->memory_segments != NULL);

        /* create an empty segment and fill with number of empty words equal to
         * its value, length */
        Seq_T new_segment = Seq_new(length);
        assert(new_segment != NULL);
        uint32_t zero_word = 0;
        for (uint32_t i = 0; i < length; i++) {
                Seq_addhi(new_segment, (void *)(uintptr_t)zero_word);
        }
        /* if there is an empty identifier use its address, if not add to top*/
        uint32_t seq_id;
        if (!Stack_empty(memory->unmapped_ids)) {
                seq_id = (uint32_t)(uintptr_t)Stack_pop(memory->unmapped_ids); 
                (void)Seq_put(memory->memory_segments, seq_id, new_segment); 
        } else {
                seq_id = Seq_length(memory->memory_segments);

                /*
                 * Expand the length of the sequence by one, and add the
                 * new segment to the sequence
                 */
                (void)Seq_addhi(memory->memory_segments, new_segment);
        }

        return seq_id;
}

/********* free_segment ******************
*
* Unmaps segment from memory
*
* Input:
*      - program_memory memory : Sequence of memory segments
*      - uint32_t identifier   : location of sequence to be unmapped
*
* Updates:
*      - program_memory struct (passed by reference) with new segment
*
* Notes:
*      - Removes value at identifier (sets to NULL)
*      - After the segment is unmapped, its id is pushed to the
*        unmapped_ids stack, so that it can be later reused
*
*********************************************/
void free_segment(program_memory memory, uint32_t identifier)
{
        assert(memory != NULL);

        /* get the segment and free it, set its value to NULL in memory */
        Seq_T curr_segment = Seq_get(memory->memory_segments, identifier);
        Seq_free(&curr_segment);
        (void)Seq_put(memory->memory_segments, identifier, NULL);

        Stack_push(memory->unmapped_ids, (void *)(uintptr_t)identifier);
}

/********* read_from_mem ******************
 * Returns the value of a word at a specific location in memory
 *
 * Input:
 *      - program_memory memory : Sequence of memory segments
 *      - uint32_t segment_id   : id of segment which contains relevant word
 *      - uint32_t word_offset  : offset of relevant word within its segment
 *
 * Updates:
 *      - None
 *
 * Returns:
 *      - uint32_t : The word stored at the given memory location
 *
 * Notes:
 *      - CRE if memory == NULL
 *      - CRE if segment_id is not valid
 *      - CRE if word_offset is not valid for the given segment_id
 *
 *********************************************/
uint32_t read_from_mem(program_memory memory, uint32_t segment_id,
                       uint32_t word_offset)
{
        assert(memory != NULL);

        /* get the segment and return its value */
        assert(segment_id < (uint32_t)Seq_length(memory->memory_segments));
        Seq_T segment = Seq_get(memory->memory_segments, segment_id);
        assert(segment != NULL);
        /*  ensure vaue is not out of bounds */
        assert(word_offset < (uint32_t)Seq_length(segment));

        return (uint32_t)(uintptr_t)Seq_get(segment, word_offset);
}

/********* write_to_mem ******************
 * Updates a given memory location with a given 32-bit word
 *
 * Input:
 *      - program_memory memory : Sequence of memory segments
 *      - uint32_t segment_id   : id of segment that will be updated
 *      - uint32_t word_offset  : offset within segment which will be updated
 *      - uint32_t new_word     : placed at $memory[segment_id][word_offset]
 *
 * Updates:
 *      - program_memory memory
 *
 * Returns:
 *      - uint32_t : The word stored at the given memory location
 *
 * Notes:
 *      - CRE if memory == NULL
 *      - CRE if segment_id is not valid
 *      - CRE if word_offset is not valid for the given segment_id
 *
 *********************************************/
void write_to_mem(program_memory memory, uint32_t segment_id,
                       uint32_t word_offset, uint32_t new_word)
{
        assert(memory != NULL);
        /* get segment, replace its value with new_word */
        assert(segment_id < (uint32_t)Seq_length(memory->memory_segments));
        Seq_T segment = Seq_get(memory->memory_segments, segment_id);
        assert(segment != NULL);
        /*  ensure value is not out of bounds */
        assert(word_offset < (uint32_t)Seq_length(segment));
        (void)Seq_put(segment, word_offset, (void *)(uintptr_t)new_word);
}

/********* copy_segment ******************
 * Copies the contents of one segment into another
 *
 * Input:
 *      - program_memory memory : Sequence of memory segments
 *      - uint32_t segment1     : Contents will be copied, and placed into
 *                                      segment2
 *      - uint32_t segment2     : Contents will be updated to contain what is
 *                                      stored in segment1
 *
 * Updates:
 *      - program_memory memory to reflect copied segment
 *
 * Notes:
 *      - If segment1 == segment2, this will run very quickly
 *      - Note: This function is more abstract than necessary for the um
 *              implementation, making this module more abstract!
 *********************************************/
void copy_segment(program_memory memory, uint32_t segment1, uint32_t segment2)
{
        assert(memory != NULL);
        if (segment1 == segment2) {
                return;
        }
        /*  get both segments */
        Seq_T seg1 = Seq_get(memory->memory_segments, segment1);
        Seq_T seg2 = Seq_get(memory->memory_segments, segment2);
        Seq_free(&seg2);
        /* copy contents of segment 1 into segment 2 */
        Seq_T new_seg2 = Seq_new(Seq_length(seg1));
        for (int i = 0; i < Seq_length(seg1); i++) {
                uint32_t curr_word = (uint32_t)(uintptr_t)Seq_get(seg1, i);
                Seq_addhi(new_seg2, (void *)(uintptr_t)curr_word);
        }
        (void)Seq_put(memory->memory_segments, segment2, new_seg2);
}

/********* free_program_memory ******************
 * 
 * Free all memory allocated by program_memory struct, and the program_memory
 * struct itself
 *
 * Input:
 *      - program_memory memory : Sequence of memory segments
 *
 * Notes:
 *      - The contents of the segments, stored as sequences, are free'd, then 
 *        the sequence with the segments is free'd. 
 *      - memory pointer set to NULL
 * 
 *********************************************/
void free_program_memory(program_memory memory)
{
        assert(memory != NULL);
        /* traverse through sequence and free all segments */
        for (int i = 0; i < Seq_length(memory->memory_segments); i++) {
                Seq_T curr_segment = Seq_get(memory->memory_segments, i);
                /* if segment has already been free'd skip it */
                if (curr_segment != NULL) {
                        Seq_free(&curr_segment);
                }
        }
        if (memory->memory_segments != NULL) {
                Seq_free(&memory->memory_segments);
        }
        /* free the stack and struct */
        Stack_free(&memory->unmapped_ids);
        FREE(memory);
        memory = NULL;
}

#undef DEFAULT_SEG_SIZE
#undef END_OF_PROGRAM
#undef BITS_PER_BYTE