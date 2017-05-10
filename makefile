vpath %.c src/
vpath %.h include/
vpath %.o obj/
vpath pc bin/

.PHONY: all clean

CC = gcc 
CFLAGS = -Wall -Werror -Wextra -pedantic -std=c11 -g -O3
O_REP = obj/
B_REP = bin/
L_REP = lib/
D_REP = doc/

all: clean pc

pc: pc.o abin.o
	$(CC) $(CFLAGS) $(patsubst %,$(O_REP)%,$^) -lm -o $(B_REP)$@

matrice.o: abin.c abin.h
pc.o: pc.c abin.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -I include -o $(O_REP)$@

clean:
	rm -f $(O_REP)*.o $(B_REP)pc
