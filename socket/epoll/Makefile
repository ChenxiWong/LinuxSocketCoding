.PHONY:clean all
CC=gcc
CFLAGS= -Wall -g
BIN=server client epoll_server
all:$(BIN)
%.o:%.c
	$(CC) -o $@ $(CFLAGS) -c $<
%.o:%.cpp
	g++ -o $@ $(CFLAGS) -c $<
clean:
	rm -rf *.o $(BIN)
