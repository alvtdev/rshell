COMP = g++
COMPFLAGS = -ansi -pedantic -Wall -Werror

all: rshell ls cp

rshell:
	@mkdir -p bin
	$(COMP) $(COMPFLAGS) src/main.cpp -o bin/rshell

ls: 
	@mkdir -p bin
	$(COMP) $(COMPFLAGS) src/ls.cpp -o bin/ls

cp:
	@mkdir -p bin
	$(COMP) $(COMPFLAGS) src/cp.cpp -o bin/cp


	
