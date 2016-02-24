main : main.o Bezier.o
	g++ -std=c++11 -Wall -g -o main main.o Bezier.o -lglut -lGL

main.o : main.cpp
	g++ -std=c++11 -Wall -g -c main.cpp

Bezier.o : Bezier.cpp Bezier.h
	g++ -Wall -g -c Bezier.cpp

clean :
	rm -f main main.o Bezier.o
