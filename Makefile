CC ?= gcc

all:
	$(CC) -o hashtable -g -Wall hashtable.c test.c

clean:
	rm -f hashtable
