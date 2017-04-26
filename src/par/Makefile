
CC = gcc
CFLAGS = --std=c99 -O2 -Wall
LDFLAGS = -lm

default: par_shamir
par_shamir: main.o par_shamir.o par_shamir.h GLibFacade.o GLibFacade.h strtok.o strtok.h omp_timer.h
	$(CC) $(CFLAGS) -fopenmp -o par_shamir main.o par_shamir.o strtok.o GLibFacade.o $(LDFLAGS)
	rm -f *.o;
	rm -f *.gch
main.o: main.c par_shamir.h strtok.h GLibFacade.h omp_timer.h
	$(CC) -fopenmp -c main.c par_shamir.h strtok.h GLibFacade.h omp_timer.h
par_shamir.o: par_shamir.c par_shamir.h
	$(CC) -fopenmp -c par_shamir.c par_shamir.h
GLibFacade.o: GLibFacade.c GLibFacade.h
	$(CC) -c GLibFacade.c GLibFacade.h
strtok.o: strtok.c strtok.h
	$(CC) -c strtok.c strtok.h
clean:
	rm -f par_shamir
