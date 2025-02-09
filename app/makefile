all: app

app: main.o entropycalculator.o file_extension_checker.o
	g++ -Wall main.o entropycalculator.o file_extension_checker.o -o app

main.o: main.cpp
	g++ -Wall -c main.cpp

entropycalculator.o: entropycalculator.cpp
	g++ -Wall -c entropycalculator.cpp

fileextensionchecker.o: file_extension_checker.cpp
	g++ -Wall -c file_extension_checker.cpp

remove:
	rm -f *.o

clean: all remove