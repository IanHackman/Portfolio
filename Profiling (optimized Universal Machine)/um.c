#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "assert.h"

/* Sentinel to halt the fetch-decode-execute loop */

struct int_arrayList {
        uint32_t *arr;
        uint32_t size;
        uint32_t capacity;
};
typedef struct int_arrayList *int_arrayList;

struct outer_arrayList {
        int_arrayList *arr;
        uint32_t size;
        uint32_t capacity;
};

typedef struct outer_arrayList *outer_arrayList;

struct program_memory {
        outer_arrayList memory_segments;
        int_arrayList unmapped_ids;
        int32_t program_counter;
};
typedef struct program_memory *program_memory;

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
        
        program_memory pm = malloc(sizeof(*pm));
        assert(pm != NULL);

        outer_arrayList new_outer_arrayList = malloc(sizeof(*new_outer_arrayList));
        assert(new_outer_arrayList != NULL);

        new_outer_arrayList->arr = malloc(sizeof(int_arrayList) * 50);
        assert(new_outer_arrayList->arr != NULL);
        new_outer_arrayList->capacity = 50;
        new_outer_arrayList->size = 0;
        
        pm->memory_segments = new_outer_arrayList;
        
        int_arrayList stack = malloc(sizeof(*stack));
        assert(stack != NULL);

        stack->arr = malloc(11422);
        assert(stack->arr != NULL);
        stack->capacity = 50;
        stack->size = 0;

        pm->unmapped_ids = stack;
        assert(pm->unmapped_ids != NULL);
        
        pm->program_counter = 0;

        int_arrayList segment_zero = malloc(sizeof(*segment_zero));
        assert(segment_zero != NULL);
        
        segment_zero->arr = malloc(11422);
        assert(segment_zero->arr != NULL);
        segment_zero->capacity = 50;
        segment_zero->size = 0;

        /* read in each word from input, add to segment zero */
        uint32_t instruction2 = 0;
        while (!feof(input)) {
                /* get instrction in Big-Endian order */
                for (int i = 3; i >= 0; i--) {
                        uint32_t byte = getc(input);
                        instruction2 =  instruction2 | (byte << i * 8);
                }
                if (segment_zero->size == segment_zero->capacity) {
                        segment_zero->arr = realloc(segment_zero->arr, sizeof(uint32_t) * (segment_zero->capacity * 2 + 1));
                        assert(segment_zero->arr != NULL);
                        segment_zero->capacity = segment_zero->capacity * 2 + 1;
                }

                segment_zero->arr[segment_zero->size] = instruction2;
                
                instruction2 = 0;
                segment_zero->size++;
        }

        pm->memory_segments->arr[0] = segment_zero;
        pm->memory_segments->size++;
        
        uint32_t registers[8] = { 0 };

        /* fetch, decode, execute loop */
        uint32_t instruction = 0;
        instruction2 = 0;
        uint32_t instruction3 = 0;
        uint32_t instruction4 = 0;
        uint32_t instruction5 = 0;
        uint32_t instruction6 = 0;
        
        bool edge_case_triggered = false;

        int curr_instruction = 1;
        while (1) {
                switch(curr_instruction) {
                        case 1:
                                instruction = pm->memory_segments->arr[0]->arr[pm->program_counter];
                                uint8_t opcode = instruction >> 28;
                                if (opcode != 7 && opcode != 12) {  
                                        instruction2 = pm->memory_segments->arr[0]->arr[pm->program_counter + 1];
                                        opcode = instruction2 >> 28;
                                        if (opcode != 7 && opcode != 12) {
                                                instruction3 = pm->memory_segments->arr[0]->arr[pm->program_counter + 2];
                                                opcode = instruction3 >> 28;
                                                if (opcode != 7 && opcode != 12) {
                                                        instruction4 = pm->memory_segments->arr[0]->arr[pm->program_counter + 3];
                                                        opcode = instruction4 >> 28;
                                                        if (opcode != 7 && opcode != 12) {
                                                                instruction5 = pm->memory_segments->arr[0]->arr[pm->program_counter + 4];
                                                                opcode = instruction5 >> 28;
                                                                if (opcode != 7 && opcode != 12) {
                                                                        instruction6 = pm->memory_segments->arr[0]->arr[pm->program_counter + 5];
                                                                } else {
                                                                        edge_case_triggered = true;
                                                                }
                                                        } else {
                                                                edge_case_triggered = true;
                                                        }
                                                } else {
                                                        edge_case_triggered = true;
                                                }
                                        } else {
                                                edge_case_triggered = true;
                                        }
                                } else {
                                        edge_case_triggered = true;
                                }
                                break;
                        case 2:
                                instruction = instruction2;
                                break;
                        case 3:
                                instruction = instruction3;
                                break;
                        case 4:
                                instruction = instruction4;
                                break;
                        case 5:
                                instruction = instruction5;
                                break;
                        case 6:
                                instruction = instruction6;
                                break;
                }
                
                // INCREMENT CURR_INSTRUCTION
                if (edge_case_triggered) {
                        curr_instruction = 1;
                } else if (curr_instruction == 6) {
                        curr_instruction = 1;
                } else {
                        curr_instruction++;
                }
                edge_case_triggered = false;
                
                pm->program_counter++;
                /* if instruction is load_value assign register A and 25-bit value */

                uint8_t opcode = instruction >> 28;
                if (opcode == 13) {
                        uint8_t reg_index = (instruction << 4) >> 29;
                        uint32_t value = (instruction << 7) >> 7;
                        registers[reg_index] = value;
                        continue;
                }
                /* get first 3 registers */
                uint32_t *rA = &registers[(instruction << 23) >> 29];
                uint32_t *rB = &registers[(instruction << 26) >> 29];
                uint32_t *rC = &registers[(instruction << 29) >> 29];
                /* switch case table to match instruction based on opcode */
                switch(opcode) {
                        case 0:
                                if (*rC != 0) {
                                        *rA = *rB;
                                }
                                break;
                        case 1:
                                *rA = pm->memory_segments->arr[*rB]->arr[*rC];
                                break;
                        case 2:
                                pm->memory_segments->arr[*rA]->arr[*rB] = *rC;
                                break;
                        case 3:
                                *rA = *rB + *rC;
                                break;
                        case 4:
                                *rA = (*rB) * (*rC);
                                break;
                        case 5:
                                *rA = *rB / *rC;
                                break;
                        case 6:
                                *rA = ~(*rB & *rC);
                                break;
                        case 7:
                                fclose(input);

                                for (uint32_t i = 0; i < pm->memory_segments->size; i++) {
                                        int_arrayList curr_segment = pm->memory_segments->arr[i];
        
                                        /* If segment has already been freed, skip it */
                                        if (curr_segment != NULL) {
                                                free(curr_segment->arr);
                                                free(curr_segment);
                                                pm->memory_segments->arr[i] = NULL;
                                        }       
                                }

                                if (pm->memory_segments != NULL) {
                                        free(pm->memory_segments->arr);
                                        free(pm->memory_segments);
                                }

                                /* free the stack and struct */
                                free((pm->unmapped_ids->arr));
                                free((pm->unmapped_ids));
                                free(pm);
                                return 0;
                                break;
                        case 8: ;
                                int_arrayList new_segment = malloc(sizeof(*new_segment));
                                assert(new_segment != NULL);

                                new_segment->arr = malloc(sizeof(uint32_t) * (*rC));

                                for(uint32_t i = 0; i < *rC; i++) {
                                        new_segment->arr[i] = 0;
                                }

                                new_segment->size = *rC;
                                new_segment->capacity = *rC;

                                uint32_t seg_id;
                                if (pm->unmapped_ids->size != 0) {
                                        seg_id = pm->unmapped_ids->arr[pm->unmapped_ids->size - 1];
                                        pm->unmapped_ids->size--;
                                        pm->memory_segments->arr[seg_id] = new_segment;
                                } else {
                                        seg_id = pm->memory_segments->size;
                                        if (seg_id == pm->memory_segments->capacity) {
                                                pm->memory_segments->arr = realloc(pm->memory_segments->arr, sizeof(int_arrayList) * (pm->memory_segments->capacity * 2 + 1));
                                                assert(pm->memory_segments->arr != NULL);
                                                pm->memory_segments->capacity = pm->memory_segments->capacity * 2 + 1;

                                        }
                                        pm->memory_segments->arr[seg_id] = new_segment;
                                        pm->memory_segments->size++;
                                }

                                *rB = seg_id;
                                break;
                        case 9: ;
                                int_arrayList curr_segment = pm->memory_segments->arr[*rC];
                                free(curr_segment->arr);
                                free(curr_segment);
                                pm->memory_segments->arr[*rC] = NULL;

                                if (pm->unmapped_ids->size + 1 >= pm->unmapped_ids->capacity) {
                                        pm->unmapped_ids->arr = realloc(pm->unmapped_ids->arr, sizeof(uint32_t) * (pm->unmapped_ids->capacity * 2 + 1));
                                        assert(pm->unmapped_ids->arr != NULL);
                                        pm->unmapped_ids->capacity = pm->unmapped_ids->capacity * 2 + 1;
                                        
                                }

                                pm->unmapped_ids->size++;
                                pm->unmapped_ids->arr[pm->unmapped_ids->size - 1] = *rC;

                                break;
                        case 10:
                                putchar(*rC);
                                break;
                        case 11: ;
                                int8_t val = getchar();
                                if (val == EOF) {
                                        /* if end of file place set value to all 1s */
                                        *rC = 0xffffffff;
                                }
                                *rC = val;
                                break;
                        case 12:
                                // Putting this commented out code instead of function call made it slower

                                // tried putting this here and it seg faults
                                if (*rB != 0) {
                                        int_arrayList seg1 = pm->memory_segments->arr[*rB];
                                        int_arrayList seg2 = pm->memory_segments->arr[0];

                                        while (seg2->capacity < seg1->size) {
                                                seg2->arr = realloc(seg2->arr, sizeof(uint32_t) * (seg2->capacity * 2 + 1));
                                                assert(seg2->arr != NULL);
                                                seg2->capacity = seg2->capacity * 2 + 1;
                                        }

                                        for (uint32_t i = 0; i < seg1->size; i++) {
                                                seg2->arr[i] = seg1->arr[i];
                                        }

                                        seg2->size = seg1->size;
                                }

                                pm->program_counter = *rC;

                                break;
                }
        }

        return 0;
}
