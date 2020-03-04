# Gerald Arocena
# CSCI E-20, Spring 2020
# 
# makefile for sttyl.c
#

CC     = cc
CFLAGS = -Wall -Wextra -g 

sttyl: sttyl.c
	$(CC) $(CFLAGS) -o sttyl sttyl.c

clean:
	rm -f sttyl sttyl.o 

