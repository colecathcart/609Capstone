all: app

app: main.o entropy_calculator.o file_extension_checker.o event.o event_detector.o analyzer.o process_killer.o logger.o
	g++ -Wall main.o entropy_calculator.o file_extension_checker.o event.o event_detector.o analyzer.o process_killer.o logger.o -o app

main.o: src/main.cpp 
	g++ -Wall -Iinclude -c src/main.cpp

entropy_calculator.o: src/entropy_calculator.cpp
	g++ -Wall -Iinclude -c src/entropy_calculator.cpp

file_extension_checker.o: src/file_extension_checker.cpp
	g++ -Wall -Iinclude -c src/file_extension_checker.cpp

event.o: src/event.cpp
	g++ -Wall -Iinclude -c src/event.cpp

event_detector.o: src/event_detector.cpp
	g++ -Wall -Iinclude -c src/event_detector.cpp

analyzer.o: src/analyzer.cpp
	g++ -Wall -Iinclude -c src/analyzer.cpp

process_killer.o: src/process_killer.cpp
	g++ -Wall -Iinclude -c src/process_killer.cpp

logger.o: src/logger.cpp
	g++ -Wall -Iinclude -c src/logger.cpp

test: tests/test_main.cpp tests/test_entropy_calculator.cpp tests/test_file_extension_checker.cpp tests/test_event_detector.cpp tests/test_analyzer.cpp tests/test_process_killer.cpp src/entropy_calculator.cpp src/file_extension_checker.cpp src/event_detector.cpp src/event.cpp src/analyzer.cpp src/process_killer.cpp src/logger.cpp
	g++ -Wall -Iinclude tests/test_main.cpp tests/test_entropy_calculator.cpp tests/test_file_extension_checker.cpp tests/test_event_detector.cpp tests/test_analyzer.cpp tests/test_process_killer.cpp src/entropy_calculator.cpp src/file_extension_checker.cpp src/event_detector.cpp src/event.cpp src/analyzer.cpp src/process_killer.cpp src/logger.cpp -lgtest -pthread -o test_app
	sudo ./test_app

remove:
	rm -f *.o app test_app

without_objects:
	rm -f *.o

no_obj: all without_objects

clean: remove