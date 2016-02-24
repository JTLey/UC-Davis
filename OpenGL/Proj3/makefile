main.out : main.o Sort.o Coord.h
	g++ -std=c++11 -Wall -g -o main main.o Sort.o Coord.h -lglut -lGL

main.o : main.cpp
	g++ -std=c++11 -Wall -g -c main.cpp

Shape.o : Sort.cpp Sort.h
	g++ -Wall -g -c Sort.cpp

clean :
	rm -f main main.o Sort.o
