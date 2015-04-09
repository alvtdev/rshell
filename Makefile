VPATH = src
COMPFLAGS = -ansi -pedantic -Wall -Werror
objects = $(addprefix obj/, main.o file.o)

bin/program: $(objects) | bin
				g++ -o $@ $(objects)

obj/%.o: %.cpp
				g++ $(COMPFLAGS) -c -o $@ $<
obj/main.o: file.h

$(objects): | obj

bin:
				mkdir bin

obj:
				mkdir obj

clean
				rm -rf obj bin


