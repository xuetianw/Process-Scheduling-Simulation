
output: asdf
	gcc main.o list.o -o output

asdf: main.c list.h
	gcc -c main.c

clean:
	rm output
