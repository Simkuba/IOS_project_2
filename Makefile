#Makefile pro IOS projekt 2
#Jakub Čoček (xcocek00), FIT

CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -pedantic -pthread -Werror
#CFLAGS += -g

all: proj2

############### SPUSTITELNE SOUBORY ###############
proj2: proj2.o
	$(CC) $(CFLAGS) $^ -o $@

############### .o SOUBORY ###############
proj2.o: proj2.c proj2.h
	$(CC) $(CFLAGS) -c proj2.c

############### CLEAN ###############
clean:
	$(RM) *.o proj2 proj2.out