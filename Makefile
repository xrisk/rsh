CFLAGS=-O0 -g -Wall -Wextra -Wpedantic
CC=cc

all: shell

prompt.o: prompt.c main.o
	$(CC) $(CFLAGS) -c prompt.c

parse.o: parse.c main.o
	$(CC) $(CFLAGS) -c parse.c

main.o: main.c 
	$(CC) $(CFLAGS) -c main.c

interpret.o: interpret.c
	$(CC) $(CFLAGS) -c interpret.c

builtin.o: builtin.c
	$(CC) $(CFLAGS) -c builtin.c

ls.o: ls.c
	$(CC) $(CFLAGS) -c ls.c

external.o: external.c
	$(CC) $(CFLAGS) -c external.c

pinfo.o: pinfo.c
	$(CC) $(CFLAGS) -c pinfo.c

nightswatch.o: nightswatch.c
	$(CC) $(CFLAGS) -c nightswatch.c


shell: main.o prompt.o parse.o interpret.o builtin.o ls.o external.o pinfo.o nightswatch.o
	$(CC) $(CLAGS) main.o prompt.o parse.o builtin.o interpret.o external.o pinfo.o ls.o nightswatch.o -o shell

clean:
	rm *.o shell
