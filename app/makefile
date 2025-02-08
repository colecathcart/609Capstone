all: app

app: main.o entropycalculator.o fileextensionchecker.o
	g++ -Wall main.o entropycalculator.o fileextensionchecker.o -o app

main.o: main.cpp
	g++ -Wall -c main.cpp

entropycalculator.o: entropycalculator.cpp
	g++ -Wall -c entropycalculator.cpp

fileextensionchecker.o: fileextensionchecker.cpp
	g++ -Wall -c fileextensionchecker.cpp

remove:
	rm -f *.o

clean: all remove