CC=gcc
CFLAGS=  -O3 -I../include  -g -DDEBUG
OBJECTS= hello_mutex.o lockdep.o
LIBS = -lpthread


# --- targets
all:    hello_mutex
hello_mutex:   $(OBJECTS) 
	$(CC)  -o hello_mutex  $(OBJECTS) $(LIBS)
        
hello_mutex.o: hello_mutex.o
	$(CC) $(CFLAGS) -c hello_mutex.c
       
lockdep.o: ../lib/lockdep.c
	$(CC) $(CFLAGS) -c ../lib/lockdep.c


# --- remove binary and executable files
clean:
	rm -f hello_mutex $(OBJECTS)
