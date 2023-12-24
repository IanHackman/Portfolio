#include "bitpack.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "except.h"
#include "assert.h"



int main()
{
        fprintf(stderr, "\n********* Commence bitpack_test.c *********\n\n");

        uint64_t a = Bitpack_getu(0xffffffffab362bd2, 64, 0);
        fprintf(stderr, "word = 0xffffffffab362bd2\nResult = %lx\n", a);
        assert(a == 0xffffffffab362bd2);

        int64_t b = Bitpack_gets(0xab362bd2, 64, 0);
        fprintf(stderr, "word = 0xab362bd2\nResult = %lx\n", b);
        assert(b == 0xab362bd2);

        uint64_t c = Bitpack_newu(0xffffffffab362bd2, 64, 0, 0x0);
        fprintf(stderr, "Result c  = %lx\n", c);
        assert(c == 0);


        /*
        uint64_t codeword = Bitpack_newu(0, 9, 23, 0x8f);
        fprintf(stderr, "codeword: %lx\n", codeword);
        codeword = Bitpack_news(codeword, 5, 18, 1);
        fprintf(stderr, "codeword: %lx\n", codeword);
        int64_t five_bit_c = -5;
        codeword = Bitpack_news(codeword, 5, 13, five_bit_c);
        codeword = Bitpack_news(codeword, 5, 8, 4);
        codeword = Bitpack_newu(codeword, 4, 4, 6);
        codeword = Bitpack_newu(codeword, 4, 0, 7);

        fprintf(stderr, "FINAL CODEWORD AFTER COMPRESS: %lx\n", codeword);


        uint64_t a = Bitpack_getu(codeword, 9, 23);
        int64_t b = Bitpack_gets(codeword, 5, 18);
        int64_t c = Bitpack_gets(codeword, 5, 13);
        int64_t d = Bitpack_gets(codeword, 5, 8);
        uint64_t Pb = Bitpack_getu(codeword, 4, 4);
        uint64_t Pr = Bitpack_getu(codeword, 4, 0);

        fprintf(stderr, "a: %lx\n", a);
        fprintf(stderr, "b: %lx\n", b);
        fprintf(stderr, "c: %ld\n", c);
        fprintf(stderr, "d: %lx\n", d);
        fprintf(stderr, "Pb: %lx\n", Pb);
        fprintf(stderr, "Pr: %lx\n", Pr);
        */

        fprintf(stderr, "Commence Bitpack_fitsu testing:  ");
        assert(!Bitpack_fitsu(-1, 3));
        assert(Bitpack_fitsu(0, 3));
        assert(Bitpack_fitsu(1, 3));
        assert(Bitpack_fitsu(2, 3));
        assert(Bitpack_fitsu(3, 3));
        assert(Bitpack_fitsu(4, 3));
        assert(Bitpack_fitsu(5, 3));
        assert(Bitpack_fitsu(6, 3));
        assert(Bitpack_fitsu(7, 3));
        assert(!Bitpack_fitsu(8, 3));
        assert(!Bitpack_fitsu(9, 3));
        assert(!Bitpack_fitsu(10, 3));
        assert(!Bitpack_fitsu(11, 3));
        fprintf(stderr, "SUCCESS\n");

        fprintf(stderr, "Commence Bitpack_fitss testing:  ");
        assert(!Bitpack_fitss(-5, 3));
        assert(Bitpack_fitss(-4, 3));
        assert(Bitpack_fitss(-3, 3));
        assert(Bitpack_fitss(-2, 3));
        assert(Bitpack_fitss(-1, 3));
        assert(Bitpack_fitss(0, 3));
        assert(Bitpack_fitss(1, 3));
        assert(Bitpack_fitss(2, 3));
        assert(Bitpack_fitss(3, 3));
        assert(!Bitpack_fitss(4, 3));
        assert(!Bitpack_fitss(5, 3));
        assert(!Bitpack_fitss(6, 3));
        assert(!Bitpack_fitss(7, 3));
        assert(!Bitpack_fitss(8, 3));
        fprintf(stderr, "SUCCESS\n");

        fprintf(stderr, "Commence Bitpack_getu  testing:  ");
        assert(Bitpack_getu(0x3f4, 6, 2) == 61);
        assert(Bitpack_getu(0x3f4, 4, 3) == 14);
        assert(Bitpack_getu(0x3f4, 5, 7) == 7);
        fprintf(stderr, "SUCCESS\n");

        fprintf(stderr, "Commence Bitpack_gets  testing:  ");
        assert(Bitpack_gets(0x3f4, 6, 2) == -3);
        assert(Bitpack_gets(0x3f4, 5, 4) == -1);
        assert(Bitpack_gets(0x3f4, 6, 0) == -12);
        assert(Bitpack_gets(0x3f4, 0, 0) == 0);
        assert(Bitpack_gets(0x3f4, 1, 0) == 0);
        assert(Bitpack_gets(0x3f4, 3, 0) == -4);
        assert(Bitpack_gets(0x3f4, 3, 1) == 2);
        fprintf(stderr, "SUCCESS\n");

        fprintf(stderr, "Commence Bitpack_newu  testing:  ");
        assert(Bitpack_newu(0x3f4, 4, 12, 0xf) == 62452);
        assert(Bitpack_newu(0x3f4, 4, 4, 0) == 772);
        assert(Bitpack_newu(0x3f4, 1, 2, 0) == 0x3f0);
        fprintf(stderr, "SUCCESS\n");

        fprintf(stderr, "Commence Bitpack_news  testing:  ");
        assert(Bitpack_news(0x3f4, 4, 4, -8) == 0x384);
        assert(Bitpack_news(0x3f4, 4, 12, -8) == 0x83f4);
        assert(Bitpack_news(0x3f4, 4, 12, 7) == 0x73f4);
        fprintf(stderr, "SUCCESS\n");
        

        return 0;
}