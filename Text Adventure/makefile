CC=gcc
CFLAGS=-Wall -x c

all: prog

rooms: pikelj.buildrooms.c
	    gcc -Wall -g -x c pikelj.buildrooms.c -o rooms

game: pikelj.adventure.c
		gcc -Wall -g -x c pikelj.adventure.c -o adventure -lpthread

prog:
	    gcc -Wall -g -x c pikelj.buildrooms.c -o rooms
		gcc -Wall -g -x c pikelj.adventure.c -o adventure -lpthread
