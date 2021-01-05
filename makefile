# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS  = -g -Wall

# executable files
main: main.o funciones.o aed.o
	$(CC) $(CFLAGS) -o main main.o funciones.o aed.o

main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp
funciones.o: funciones.cpp
	$(CC) $(CFLAGS) -c funciones.cpp
aed.o: aed.cpp
	$(CC) $(CFLAGS) -c aed.cpp
clean:
	rm *.o