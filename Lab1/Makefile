CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -g -DEXT_PROMPT
LDFLAGS = 

SRCS = main.c shell.c scanner.c
OBJS = $(SRCS:.c=.o)
DEPS = shell.h scanner.h

all: myshell

myshell: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o myshell

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) myshell
