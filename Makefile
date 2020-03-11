# Gerald Arocena
# CSCI E-20, Spring 2020
# 
# makefile for sttyl.c
#

GCC    = gcc
CFLAGS = -Wall -Wextra -g

sttyl: sttyl.c
	$(GCC) $(CFLAGS) -o sttyl sttyl.c

clean:
	rm -f sttyl 

