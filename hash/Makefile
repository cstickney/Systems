#Chris Stickney's makefile for HW1

LIBDIR = -L$(GET_WORD)/lib
INCLUDEDIR = -I$(GET_WORD)/include

words: main.c hash.c hash.h
	gcc -std=c99 $(LIBDIR) $(INCLUDEDIR) -lm -g main.c hash.c -o wordpairs -lget
	