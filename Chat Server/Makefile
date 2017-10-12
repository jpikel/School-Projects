# Filename: Makefile
# AUthor: Johannes Pikel
# Date: 2017.07.13
# ONID: pikelj
# Class: CS372-400
# Assignment: Project #1
# Description: makefile to compile the chat room client/server in c
#

CC = gcc
CFLAGS = -x c 
CFLAGS += -Wall
CFLAGS += -g
CFLAGS += -pedantic-errors

SRCS = pikelj_chat.c

HEADERS = pikelj_chat.h

all: ${SRCS} ${HEADERS}
	${CC} ${CLAGS} ${SRCS} -o chatroom

clean:
	rm -f *.o chatroom
