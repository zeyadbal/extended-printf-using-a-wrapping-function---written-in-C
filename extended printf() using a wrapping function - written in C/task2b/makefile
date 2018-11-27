all: target

target: main.o toy_printf.o toy_stdio.h
	gcc -g -Wall -o target main.o toy_printf.o toy_stdio.h
main.o: main.c toy_stdio.h
	gcc -g -Wall -c -o main.o main.c
toy_printf.o: toy_printf[2b].c
	gcc -g -Wall -c -o toy_printf.o toy_printf[2b].c
clean:
	rm -f *.o target
