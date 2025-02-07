all: app

app: main.o entropycalculator.o
	g++ -Wall main.o entropycalculator.o -o app

main.o: main.cpp
	g++ -Wall -c main.cpp

entropycalculator.o: entropycalculator.cpp
	g++ -Wall -c entropycalculator.cpp

remove:
	rm -f *.o

clean: all remove