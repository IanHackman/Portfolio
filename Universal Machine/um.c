/**************************************************************
 *
 *                     um.c
 *
 *     Assignment: um
 *     Authors:  Will Randall (wranda01), Ian Hackman (ihackm01)
 *     Date:     11/20/2023
 *
 *     um.c runs the Universal Machine by initializing all memory and fetching,
 *     decoding and executing each instruction as they are inputted.
 *
 **************************************************************/
#include "instruction_set.h"

/* Sentinel to halt the fetch-decode-execute loop */
#define END_OF_PROGRAM -1


/* HELPER FUNCTION DECLARATIONS */
uint32_t fetch_instruction(program_memory pm);
void decode_and_execute(program_memory pm, uint32_t *registers,
                        uint32_t instruction);

int main(int argc, char *argv[])
{
        assert(argc == 2);
        
        /* open filee and initialize file pointer */
        FILE *input;
        input = fopen(argv[1], "r");
        assert(input != NULL);
        /* set program_memory struct and 8 registers */
        program_memory pm = new_program_memory();
        create_segment_zero(pm, input);
        
        uint32_t registers[8] = { 0 };

        /* fetch, decode, execute loop */
        while (get_program_counter(pm) != END_OF_PROGRAM) {
                
                uint32_t instruction = fetch_instruction(pm);
                decode_and_execute(pm, registers, instruction);
        }

        fclose(input);
        free_program_memory(pm);

        return 0;
}

/********* fetch_instruction ***************
 * 
 * Reads an instruction from $m[0][program_counter]
 *
 * Input:
 *      program_memory memory : program_memory instance
 *
 * Returns:
 *      uint32_t - the instruction
 * 
 * Notes:
 *      - CRE if program_memory struct is NULL
 *
 *********************************************/
uint32_t fetch_instruction(program_memory pm)
{
        assert(pm != NULL);
        /* get instruction from segment 0 */
        uint32_t instruction = read_from_mem(pm, 0, get_program_counter(pm));
        
        increment_program_counter(pm);

        return instruction;
}

/********* decode_and_execute ***************
 * 
 * Decodes an instruction and executes the appropriate instruction with the
 * appropriate registers.
 *
 * Input:
 *      program_memory memory : program_memory instance
 *
 * Returns:
 *      uint32_t - the instruction
 * 
 * Notes:
 *      - CRE if program_memory struct is NULL
 *
 *********************************************/
void decode_and_execute(program_memory pm, uint32_t *registers,
                        uint32_t instruction)
{
        assert(pm != NULL);
        uint8_t opcode = Bitpack_getu(instruction, 4, 28);
        /* if instruction is load_value assign register A and 25-bit value */
        if (opcode == 13) {
                uint8_t reg_index = Bitpack_getu(instruction, 3, 25);
                uint32_t value = Bitpack_getu(instruction, 25, 0);
                load_value(&registers[reg_index], value);
                return;
        }
        /* get first 3 registers */
        uint32_t *rA = &registers[Bitpack_getu(instruction, 3, 6)];
        uint32_t *rB = &registers[Bitpack_getu(instruction, 3, 3)];
        uint32_t *rC = &registers[Bitpack_getu(instruction, 3, 0)];
        /* switch case table to match instruction based on opcode */
        switch(opcode) {
                case 0:
                        conditional_move(rA, *rB, *rC);
                        break;
                case 1:
                        segmented_load(pm, rA, *rB, *rC);
                        break;
                case 2:
                        segmented_store(pm, *rA, *rB, *rC);
                        break;
                case 3:
                        addition(rA, *rB, *rC);
                        break;
                case 4:
                        multiplication(rA, *rB, *rC);
                        break;
                case 5:
                        division(rA, *rB, *rC);
                        break;
                case 6:
                        bitwise_NAND(rA, *rB, *rC);
                        break;
                case 7:
                        halt(pm);
                        break;
                case 8:
                        map_segment(pm, *rC, rB);
                        break;
                case 9:
                        unmap_segment(pm, *rC);
                        break;
                case 10:
                        output(*rC);
                        break;
                case 11:
                        input(rC);
                        break;
                case 12:
                        load_program(pm, *rB, *rC);
                        break;
        }
}

#undef END_OF_PROGRAM