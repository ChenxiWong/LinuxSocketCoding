.PHONY:clean all
CC=gcc
CFLAGS= -Wall -g
BIN=server client getip
all:$(BIN)
%.o:%.c
	$(CC) -o $@ $(CFLAGS) -c $<
clean:
	rm -rf *.o $(BIN)
