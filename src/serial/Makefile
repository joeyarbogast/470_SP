
CC = gcc
CFLAGS = --std=c99 -O2 -Wall
LDFLAGS = -lm

default: shamir
shamir: main.o shamir.o shamir.h GLibFacade.o GLibFacade.h strtok.o strtok.h
	$(CC) $(CFLAGS) -o shamir main.o shamir.o strtok.o GLibFacade.o $(LDFLAGS)
	rm -f *.o;
	rm -f *.gch
main.o: main.c shamir.h strtok.h GLibFacade.h
	$(CC) -c main.c shamir.h strtok.h GLibFacade.h
shamir.o: shamir.c shamir.h
	$(CC) -c shamir.c shamir.h
GLibFacade.o: GLibFacade.c GLibFacade.h
	$(CC) -c GLibFacade.c GLibFacade.h
strtok.o: strtok.c strtok.h
	$(CC) -c strtok.c strtok.h
clean:
	rm -f shamir
