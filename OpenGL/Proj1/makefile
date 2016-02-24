main.out : main.o Line.o Poly.o
	g++ -std=c++11 -Wall -g -o main main.o Line.o Poly.o -lglut -lGL

main.o : main.cpp
	g++ -std=c++11 -Wall -g -c main.cpp

Line.o : Line.cpp Line.h
	g++ -Wall -g -c Line.cpp

Poly.o : Poly.cpp Poly.h
	g++ -Wall -g -c Poly.cpp

clean :
	rm -f main main.o Line.o Poly.o
