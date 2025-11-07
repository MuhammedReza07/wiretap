CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic

wiretap: wiretap.c format.h format.o
	$(CC) $(CFLAGS) src/wiretap.c bin/format.o -o bin/wiretap -lm

format.o: format.c
	$(CC) $(CFLAGS) -c src/format.c -o bin/format.o

%.c %.h:
	touch src/$@

clean:
	rm bin/*