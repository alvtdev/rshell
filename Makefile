COMP = g++
COMPFLAGS = -ansi -pedantic -Wall -Werror

all: rshell

rshell:
	@mkdir -p bin
	$(COMP) $(COMPFLAGS) src/main.cpp -o bin/rshell

cp:
	@mkdir -p bin
	$(COMP) $(COMPFLAGS) src/cp.cpp -o bin/cp


	
