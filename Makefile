# Gerald Arocena
# CSCI E-20, Spring 2020
# 
# makefile for sttyl.c
#

GCC    = gcc
CFLAGS = -Wall -Wextra -g

sttyl:
	$(GCC) $(CFLAGS) sttyl.c -o sttyl

clean:
	rm -f sttyl

