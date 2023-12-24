/*
 * umlab.c
 *
 * Functions to generate UM unit tests. Once complete, this module
 * should be augmented and then linked against umlabwrite.c to produce
 * a unit test writing program.
 *  
 * A unit test is a stream of UM instructions, represented as a Hanson
 * Seq_T of 32-bit words adhering to the UM's instruction format.  
 * 
 * Any additional functions and unit tests written for the lab go
 * here. 
 *  
 */


#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <seq.h>
#include <bitpack.h>


typedef uint32_t Um_instruction;
typedef enum Um_opcode {
        CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
        NAND, HALT, ACTIVATE, INACTIVATE, OUT, IN, LOADP, LV
} Um_opcode;


/* Functions that return the two instruction types */

Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        Um_instruction word = 0;
        word = Bitpack_newu(word, 4, 28, op);
        word = Bitpack_newu(word, 3, 6, ra);
        word = Bitpack_newu(word, 3, 3, rb);
        word = Bitpack_newu(word, 3, 0, rc);

        return word;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
        Um_instruction word = 0;
        word = Bitpack_newu(word, 4, 28, 13);
        word = Bitpack_newu(word, 3, 25, ra);
        word = Bitpack_newu(word, 25, 0, val);
        return word;
}


/* Wrapper functions for each of the instructions */

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}

typedef enum Um_register { r0 = 0, r1, r2, r3, r4, r5, r6, r7 } Um_register;

static inline Um_instruction add(Um_register a, Um_register b, Um_register c) 
{
        return three_register(ADD, a, b, c);
}

static inline Um_instruction output(Um_register c)
{
        return three_register(OUT, 0, 0, c);
}

static inline Um_instruction conditional_move(Um_register a, Um_register b,
                                             Um_register c)
{
    return three_register(CMOV, a, b, c);
}

static inline Um_instruction segmented_load(Um_register a, Um_register b,
                                            Um_register c)
{
    return three_register(SLOAD, a, b, c);
}

static inline Um_instruction segmented_store(Um_register a, Um_register b,
                                             Um_register c)
{
    return three_register(SSTORE, a, b, c);
}

static inline Um_instruction multiplication(Um_register a, Um_register b, 
                                            Um_register c)
{
    return three_register(MUL, a, b, c);
}

static inline Um_instruction division(Um_register a, Um_register b,
                                      Um_register c)
{
    return three_register(DIV, a, b, c);
}

static inline Um_instruction bitwise_NAND(Um_register a, Um_register b, 
                                          Um_register c)
{
    return three_register(NAND, a, b, c);
}

static inline Um_instruction map_segment(Um_register b, Um_register c)
{
    return three_register(ACTIVATE, 0, b, c);
}

static inline Um_instruction unmap_segment(Um_register c)
{
    return three_register(INACTIVATE, 0, 0, c);
}

static inline Um_instruction input(Um_register c)
{
    return three_register(IN, 0, 0, c);
}

static inline Um_instruction load_program(Um_register b, Um_register c)
{
    return three_register(LOADP, 0, b, c);
}

static inline Um_instruction load_value(Um_register a, uint32_t value)
{
    return loadval(a, value);
}

/* Functions for working with streams */

static inline void append(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}

const uint32_t Um_word_width = 32;

void Um_write_sequence(FILE *output, Seq_T stream)
{
        assert(output != NULL && stream != NULL);
        int stream_length = Seq_length(stream);
        for (int i = 0; i < stream_length; i++) {
                Um_instruction inst = (uintptr_t)Seq_remlo(stream);
                for (int lsb = Um_word_width - 8; lsb >= 0; lsb -= 8) {
                        fputc(Bitpack_getu(inst, 8, lsb), output);
                }
        }
      
}


/* Unit tests for the UM */

void build_print_six_test(Seq_T stream)
{
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));
        append(stream, add(r3, r1, r2));
        append(stream, output(r3));
        append(stream, halt());
}

void build_halt_test(Seq_T stream)
{
        append(stream, halt());
}

void build_verbose_halt_test(Seq_T stream)
{
        append(stream, halt());
        append(stream, loadval(r1, 'B'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'a'));
        append(stream, output(r1));
        append(stream, loadval(r1, 'd'));
        append(stream, output(r1));
        append(stream, loadval(r1, '!'));
        append(stream, output(r1));
        append(stream, loadval(r1, '\n'));
        append(stream, output(r1));
}

void build_test_without_halt(Seq_T stream)
{
        append(stream, loadval(r1, 48));
        append(stream, loadval(r2, 6));
        append(stream, add(r3, r1, r2));
        append(stream, output(r3));
}

void build_conditional_move_test(Seq_T stream)
{
    append(stream, load_value(r1, 65));
    append(stream, load_value(r2, 66));
    append(stream, load_value(r3, 67));
    append(stream, conditional_move(r1, r2, r3));
    append(stream, output(r1));
    append(stream, halt());
}

void build_segmented_store_and_load_test(Seq_T stream)
{
    append(stream, load_value(r2, 10));
    append(stream, map_segment(r3, r2));
    append(stream, load_value(r7, 50));
    append(stream, load_value(r2, 9));
    append(stream, segmented_store(r3, r2, r7));
    append(stream, segmented_load(r6, r3, r2));
    append(stream, output(r6));
    append(stream, halt());
}


void build_addition_test(Seq_T stream)
{
    append(stream, load_value(r1, 15));
    append(stream, load_value(r2, 27));
    append(stream, load_value(r3, 40));
    append(stream, add(r1, r2, r3));
    append(stream, output(r1));
    append(stream, halt());
}

void build_multiplication_test(Seq_T stream)
{
        append(stream, load_value(r1, 5));
        append(stream, load_value(r2, 10));
        append(stream, load_value(r3, 5));
        append(stream, multiplication(r1, r2, r3));
        append(stream, output(r1));
        append(stream, halt());
}

void build_division_test(Seq_T stream)
{
    append(stream, load_value(r1, 20));
    append(stream, load_value(r2, 201));
    append(stream, load_value(r3, 4));
    append(stream, division(r1, r2, r3));
    append(stream, output(r1));
    append(stream, load_value(r1, 20));
    append(stream, load_value(r2, 204));
    append(stream, load_value(r3, 4));
    append(stream, division(r1, r2, r3));
    append(stream, output(r1));
    append(stream, halt());
}

void build_bitwise_NAND_test(Seq_T stream)
{
    append(stream, load_value(r1, 0));
    append(stream, load_value(r2, 0x40));
    append(stream, load_value(r3, 0x1));
    append(stream, bitwise_NAND(r1, r2, r2));
    append(stream, bitwise_NAND(r7, r3, r3));
    append(stream, bitwise_NAND(r6, r1, r7));
    append(stream, output(r6));
    append(stream, halt());
}

void build_map_segment_test(Seq_T stream)
{
    append(stream, load_value(r2, 10));
    append(stream, map_segment(r2, r3));
    append(stream, halt());
}

void build_unmap_segment_test(Seq_T stream)
{
    append(stream, load_value(r2, 10));
    append(stream, map_segment(r3, r2));
    append(stream, unmap_segment(r3));
    append(stream, halt());
}

void build_input_output1_test(Seq_T stream)
{
    append(stream, input(r3));
    append(stream, output(r3));
    append(stream, halt());
}

void build_input_test(Seq_T stream)
{
    append(stream, input(r3));
    append(stream, halt());
}

void build_load_program_test(Seq_T stream)
{
    append(stream, load_value(r2, 10));
    append(stream, map_segment(r3, r2));
    append(stream, load_value(r7, 0));
    append(stream, load_value(r0, 0));
    append(stream, load_value(r5, 8));
    append(stream, segmented_load(r6, 0, r5));
    append(stream, segmented_store(r3, 0, r6));
    append(stream, load_program(r3, r7));
    append(stream, halt());
}

void build_input_eof_test(Seq_T stream)
{
    append(stream, input(r3));
    append(stream, output(r3));
    append(stream, halt());
}

void build_load_value_test(Seq_T stream)
{
    append(stream, load_value(r7, 65));
    append(stream, output(r7));
    append(stream, halt());
}
