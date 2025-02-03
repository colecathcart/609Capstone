all: app

app: main.o
	g++ -Wall main.o -o app

main.o: main.cpp
	g++ -Wall -c main.cpp

remove:
	rm -f *.o

clean: all remove