/**************************************************************
 *
 *                     instruction_set.h
 *
 *     Assignment: um
 *     Authors:  Will Randall (wranda01), Ian Hackman (ihackm01)
 *     Date:     11/20/2023
 *
 *     instruction_set.h contains the interface for the instruction set module.
 *
 **************************************************************/
#ifndef INSTRUCTION_SET_H
#define INSTRUCTION_SET_H

#include "memory_management.h"

void conditional_move(uint32_t *rA, uint32_t rB, uint32_t rC);
void segmented_load(program_memory pm, uint32_t *rA, uint32_t rB, uint32_t rC);
void segmented_store(program_memory pm, uint32_t rA, uint32_t rB, uint32_t rC);
void addition(uint32_t *rA, uint32_t rB, uint32_t rC);
void multiplication(uint32_t *rA, uint32_t rB, uint32_t rC);
void division(uint32_t *rA, uint32_t rB, uint32_t rC);
void bitwise_NAND(uint32_t *rA, uint32_t rB, uint32_t rC);
void halt(program_memory pm);
void map_segment(program_memory pm, uint32_t rC, uint32_t *rB);
void unmap_segment(program_memory pm, uint32_t rC);
void output(uint32_t rC);
void input(uint32_t *rC);
void load_program(program_memory pm, uint32_t rB, uint32_t rC);
void load_value(uint32_t *rA, uint32_t value);

#endif
