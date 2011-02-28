CFLAGS=-g -Wall -Wextra -ansi -std=c99
OBJ=open_range.o finished.o test.o ppio.o readanyv.o

all: $(OBJ)

suite: $(OBJ)
	$(CC) -o $@ $^ -lcheck

check: $(OBJ) suite
	./suite

clean:
	rm -f $(OBJ) suite

finished.o: ppio.h ppio_internal.h
open_range.o: ppio.h ppio_internal.h
