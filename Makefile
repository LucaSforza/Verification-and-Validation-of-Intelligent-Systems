all: main

main: main.c
	gcc -O3 main.c -DHAVE_INLINE -o main -I Utils/ -I/opt/homebrew/include -L/opt/homebrew/lib  -lgsl -lgslcblas -lm