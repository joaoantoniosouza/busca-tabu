all: pab

pab: pab.o
	g++ -o pab pab.o
	mkdir -p objects
	mv *.o objects/

pab.o: ./source/pab.cpp
	g++ -o pab.o -c ./source/pab.cpp -Wall

clear:
	rm -rf *.o *~ pab ./objects