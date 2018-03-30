LIBS = -lpthread -lm
CC = gcc
CFLAGS = -O0 -Wall -std=c99
VARIABLES = -DPOND_SIZE=7

default: run

run: miniep2
	./miniep2

all: miniep2

ep2: miniep2.c
	$(CC) -o miniep2 miniep2.c $(CFLAGS) $(LIBS) $(VARIABLES)

clean:
	-rm -f miniep2
