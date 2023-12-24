/**************************************************************
 *
 *                     memory_management.h
 *
 *     Assignment: um
 *     Authors:  Will Randall (wranda01), Ian Hackman (ihackm01)
 *     Date:     11/20/2023
 *
 *     memory_management.h contains the interface for memory_management module 
 *     and the struct that handles mapping segments.
 *
 **************************************************************/
#ifndef MEMORY_MANAGEMENT_H
#define MEMORY_MANAGEMENT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "seq.h"
#include "stack.h"
#include "assert.h"
#include "mem.h"
#include "bitpack.h"

typedef struct program_memory *program_memory;

program_memory new_program_memory();
int32_t get_program_counter(program_memory memory);
void increment_program_counter(program_memory memory);
void set_program_counter(program_memory memory, int32_t val);
void create_segment_zero(program_memory pm, FILE *input);
uint32_t new_segment(program_memory memory, uint32_t length);
void free_segment(program_memory memory, uint32_t identifier);
uint32_t read_from_mem(program_memory memory, uint32_t segment_id,
                       uint32_t word_offset);
void write_to_mem(program_memory memory, uint32_t segment_id,
                       uint32_t word_offset, uint32_t new_word);
void copy_segment(program_memory memory, uint32_t segment1, uint32_t segment2);
void free_program_memory(program_memory memory);

#endif

#undef END_OF_PROGRAM