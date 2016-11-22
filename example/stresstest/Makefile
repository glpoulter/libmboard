# $Id: Makefile 732 2008-03-10 15:55:53Z lsc $

CC         = gcc
CFLAGS     = -std=c99 -Wall -pedantic -I../../include -pg
LDFLAGS    = -L../../lib -lmboard_s -lm -pg

P_CC       := mpicc
P_CFLAGS   := $(CFLAGS) -D_PARALLEL
P_LDFLAGS  := -L../../lib -lmboard_p -lm

SOURCES   := $(wildcard *.c)
OBJECTS    = $(SOURCES:.c=.o)
EXECUTABLE = stresstest
DEPS       = Makefile stresstest.h
RM         = rm -f

all: $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS)

$(OBJECTS): $(DEPS)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

parallel: CC      = $(P_CC)
parallel: CFLAGS  = $(P_CFLAGS)
parallel: LDFLAGS = $(P_LDFLAGS)
parallel: clean all

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)

