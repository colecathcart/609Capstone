all: app

app: main.o entropy_calculator.o file_extension_checker.o
	g++ -Wall main.o entropy_calculator.o file_extension_checker.o -o app

main.o: src/main.cpp 
	g++ -Wall -Iinclude -c src/main.cpp

entropy_calculator.o: src/entropy_calculator.cpp
	g++ -Wall -Iinclude -c src/entropy_calculator.cpp

file_extension_checker.o: src/file_extension_checker.cpp
	g++ -Wall -Iinclude -c src/file_extension_checker.cpp

remove:
	rm -f *.o app

clean: all remove