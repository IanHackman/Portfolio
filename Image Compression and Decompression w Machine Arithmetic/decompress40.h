#include <stdio.h>
#include <math.h>
#include "pnm.h"
#include "assert.h"
#include "mem.h"
#include "bitpack.h"
#include "arith40.h"
#include "dct.h"



extern void decompress40(FILE *input);  /* reads compressed image, writes PPM */