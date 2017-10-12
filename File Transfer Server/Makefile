# Filename: Makefile
# AUthor: Johannes Pikel
# Date: 2017.08.02
# ONID: pikelj
# Class: CS372-400
# Assignment: Project #2
# Description: makefile to compile the ftp server
#

CC = gcc
CFLAGS = -x c 
CFLAGS += -Wall
CFLAGS += -g
CFLAGS += -pedantic-errors

SRCS = pikelj_ftserver.c

HEADERS = pikelj_ftserver.h

all: ${SRCS} ${HEADERS}
	${CC} ${CFLAGS} ${SRCS} -lm -o ftserver

clean:
	rm -f *.o ftserver
