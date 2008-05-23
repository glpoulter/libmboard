# $Id$

CFLAGS_BASE  = -Wall -pedantic -I../../include -O3 -g
LDFLAGS_BASE = -L../../lib -lexpat -lm -O3

CC      = gcc
CFLAGS  = $(CFLAGS_BASE)
LDFLAGS = $(LDFLAGS_BASE) -lmboard_s

P_CC      = mpicc
P_LDFLAGS = $(LDFLAGS_BASE) -lmboard_p

SOURCES   := $(wildcard *.c)
OBJECTS    = $(SOURCES:.c=.o)
EXECUTABLE = circles
DEPS       = Makefile circles.h
RM         = rm -f

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJECTS): $(DEPS)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

parallel: CC      = $(P_CC) -D_PARALLEL
parallel: CFLAGS += -std=c99
parallel: LDFLAGS = $(P_LDFLAGS)
parallel: clean all

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE) gmon.out

