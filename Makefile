CFLAGS=-O2 
CC=clang

all: mmul

mmul: mmul.c
	$(CC) $(CFLAGS) mmul.c -o mmul

.PHONY: clean

clean:
	rm -f mmul
