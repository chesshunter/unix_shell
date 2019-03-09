Main : Main.cpp Parse Helpers
	g++ -w -std=c++14 -g -c Main.cpp Parse.o Helpers.o -lpthread -lrt

Helpers : Helpers.h Helpers.cpp
	g++ -w -std=c++14 -g -c Helpers.cpp Helpers.h

Parse : Parse.h Helpers
	g++ -w -std=c++14 -g -c Parse.cpp Helpers.o Parse.h

all: Main Helpers Parse
	g++ -o shell Main.o Parse.o Helpers.o

clean :
	rm *.o
