#
# Makefile for the UM Assignment
# 

CC = gcc

IFLAGS  = -I/comp/40/build/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -Werror -pedantic $(IFLAGS)
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64
LDLIBS  = -lbitpack -lcii40 -lm

INCLUDES = $(shell echo *.h)

EXECS   = um test writetests

all: $(EXECS)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@

test: test.o memory_management.o instruction_set.o 
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

um: um.o memory_management.o instruction_set.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

writetests: umlabwrite.o umlab.o memory_management.o instruction_set.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f $(EXECS)  *.o

