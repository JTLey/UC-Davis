main.out : main.o Shape.o
	g++ -std=c++11 -Wall -g -o main main.o Shape.o -lglut -lGL

main.o : main.cpp
	g++ -std=c++11 -Wall -g -c main.cpp

Shape.o : Shape.cpp Shape.h
	g++ -Wall -g -c Shape.cpp

clean :
	rm -f main main.o Shape.o
