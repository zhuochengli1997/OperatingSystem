CC = gcc
CFLAGS = -Wall -Wextra -g

all: shell

shell: main.o scanner.o shell.o
	$(CC) $(CFLAGS) -o shell main.o scanner.o shell.o

main.o: main.c shell.h
	$(CC) $(CFLAGS) -c main.c

scanner.o: scanner.c scanner.h
	$(CC) $(CFLAGS) -c scanner.c

shell.o: shell.c shell.h
	$(CC) $(CFLAGS) -c shell.c

clean:
	rm -f *.o shell
