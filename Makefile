CFLAGS=-O0 -g -Wall -Wextra -Wpedantic

all: shell

prompt.o: prompt.c main.o
	cc $(CFLAGS) -c prompt.c

parse.o: parse.c main.o
	cc $(CFLAGS) -c parse.c

main.o: main.c 
	cc $(CFLAGS) -c main.c

interpret.o: interpret.c
	cc $(CFLAGS) -c interpret.c

builtin.o: builtin.c
	cc $(CFLAGS) -c builtin.c

ls.o: ls.c
	cc $(CFLAGS) -c ls.c

shell: main.o prompt.o parse.o interpret.o builtin.o ls.o
	cc $(CLAGS) main.o prompt.o parse.o builtin.o interpret.o ls.o -o shell

