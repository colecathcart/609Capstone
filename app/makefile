# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Iinclude
LDFLAGS = -lssl -lcrypto -lmagic
LDLIBS = -lgtest -pthread

# Source files
SRC = src/main.cpp src/entropy_calculator.cpp src/file_extension_checker.cpp src/event.cpp src/event_detector.cpp src/analyzer.cpp src/process_killer.cpp src/logger.cpp

# Object files (will be placed in the current directory)
OBJ = $(SRC:src/%.cpp=%.o)

# Target executables
APP = app
TEST_APP = test_app

# Default target
all: $(APP)

# Build the main application
$(APP): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(APP)

# Build the test application
test: $(SRC) tests/test_main.cpp tests/test_entropy_calculator.cpp tests/test_file_extension_checker.cpp tests/test_event_detector.cpp tests/test_analyzer.cpp tests/test_process_killer.cpp
	$(CXX) $(SRC) tests/test_main.cpp tests/test_entropy_calculator.cpp tests/test_file_extension_checker.cpp tests/test_event_detector.cpp tests/test_analyzer.cpp tests/test_process_killer.cpp $(LDFLAGS) $(LDLIBS) -o $(TEST_APP)
	sudo ./$(TEST_APP)

# General rule for compiling .cpp to .o (creates .o files in the current directory)
%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the build
remove:
	rm -f $(OBJ) $(APP) $(TEST_APP)

# Clean up everything including .o files
remove_objects:
	rm -f $(OBJ)

no_obj: all remove_objects

# Clean target
clean: remove