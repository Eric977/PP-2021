CC = gcc
CGLAG = -std=c99

all: main-uni main-omp

main-omp: main.c matrix.c matrix.h 
	$(CC) -fopenmp $(CFLAG) main.c matrix.c -o main-omp
main-uni: main.c matrix.c matrix.h
	$(CC) $(CFLAG) main.c matrix.c -o main-uni
