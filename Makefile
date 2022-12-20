cc = g++


all: 
	g++ src/main.cc utils/mygraph.cc inc/mygraph.hpp -o test

clean: 
	rm test