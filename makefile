CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic

wiretap: wiretap.c format.h format.o
	mkdir -p bin
	$(CC) $(CFLAGS) src/wiretap.c bin/format.o -o bin/wiretap -lm

format.o: format.c
	mkdir -p bin
	$(CC) $(CFLAGS) -c src/format.c -o bin/format.o

%.c %.h:
	touch src/$@

clean:
	rm -rf bin