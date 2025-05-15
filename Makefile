CC=clang++
CFLAGS=-Wall -Werror -pedantic -Ilib/json/include -D_FORTIFY_SOURCE=2 -O1 -lcurl
DEBUGCFLAGS=-Wall -Werror -pedantic -Ilib/json/include -g -lcurl
SRC=src
BIN=bin
PROGRAM_NAME=jaguar
SRCS=$(wildcard $(SRC)/*.cpp)

all:	$(SRCS)
	$(CC) $(CFLAGS) $< -o $(BIN)/$(PROGRAM_NAME)

debug:	$(SRCS)
	@mkdir -p $(BIN)/debug
	$(CC) $(DEBUGCFLAGS) $< -o $(BIN)/debug/$(PROGRAM_NAME)-debug
clean:
	$(RM) -rf $(BIN)/*

.PHONY=all
