CC=gcc
CFLAGS=  -Wall -Werror -O3 -I../include -g -Wno-format
OBJECTS= hello_mutex.o lockdep.o
LIBS = -lpthread


# --- targets
all:    lockdep_test
lockdep_test:   $(OBJECTS)
	$(CC) $(CFLAGS) -o lockdep_test  $(OBJECTS) $(LIBS)

hello_mutex.o: hello_mutex.c
	$(CC) $(CFLAGS) -c hello_mutex.c

lockdep.o: ../lib/lockdep.c
	$(CC) $(CFLAGS) -c ../lib/lockdep.c


# --- remove binary and executable files
clean:
	rm -f lockdep_test $(OBJECTS)
