COMP = g++
COMPFLAGS = -ansi -pedantic -Wall -Werror

all: rshell

rshell:
	@mkdir -p bin
	$(COMP) $(COMPFLAGS) src/main.cpp -o bin/rshell
	
