HEADERS = program.h
OBJECTS = program.o

default: program clear run

%.o: %.c $(HEADERS)
	gcc -c $< -o $@

program: $(OBJECTS)
	gcc $(OBJECTS) -o $@

clear:
	clear

run: program
	./program
