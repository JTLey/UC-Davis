main : main.o Sphere.o Plane.o Light.o
	g++ -std=c++11 -Wall -g -o main main.o Light.o Sphere.o Plane.o -lglut -lGL

main.o : main.cpp
	g++ -std=c++11 -Wall -g -c main.cpp

Sphere.o : Sphere.cpp Sphere.h
	g++ -Wall -g -c Sphere.cpp

Light.o : Light.cpp Light.h
	g++ -Wall -g -c Light.cpp

Plane.o : Plane.cpp Plane.h
	g++ -Wall -g -c Plane.cpp

clean :
	rm -f main main.o Light.o Plane.o Sphere.o

