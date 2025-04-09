# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Iinclude -Ithird_party/websocketpp
LDFLAGS = -lssl -lcrypto -lmagic -lsqlite3
LDLIBS = -lgtest -pthread

# Source files
SRC = src/main.cpp src/entropy_calculator.cpp src/file_extension_checker.cpp src/event.cpp \
      src/event_detector.cpp src/analyzer.cpp src/process_killer.cpp src/logger.cpp \
      src/websocket_server.cpp src/thread_pool.cpp

TEST_SRC = src/entropy_calculator.cpp src/file_extension_checker.cpp src/event.cpp \
           src/event_detector.cpp src/analyzer.cpp src/process_killer.cpp src/logger.cpp \
           src/websocket_server.cpp src/thread_pool.cpp

# Object files (store in build/)
OBJ = $(SRC:src/%.cpp=build/%.o)

# Executables
APP = ransomware_detector
TEST_APP = test_app
WS_APP = websocket_server_app

# Default target
all: $(APP)

# Ensure build directory exists
$(shell mkdir -p build)

# Build main application
$(APP): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(APP)

# Build websocket_server_app
$(WS_APP): build/websocket_main.o build/websocket_server.o
	$(CXX) build/websocket_main.o build/websocket_server.o $(LDFLAGS) -lpthread -o $(WS_APP)

# Test runner
test: $(TEST_SRC) tests/test_main.cpp tests/test_entropy_calculator.cpp tests/test_file_extension_checker.cpp \
      tests/test_event_detector.cpp tests/test_analyzer.cpp tests/test_process_killer.cpp tests/test_logger.cpp tests/test_event.cpp \
	  tests/test_thread_pool.cpp 
	$(CXX) $(CXXFLAGS) $(TEST_SRC) tests/test_main.cpp tests/test_entropy_calculator.cpp \
	tests/test_file_extension_checker.cpp tests/test_event_detector.cpp \
	tests/test_analyzer.cpp tests/test_process_killer.cpp tests/test_logger.cpp tests/test_event.cpp tests/test_thread_pool.cpp \
	 $(LDFLAGS) $(LDLIBS) -o $(TEST_APP)
	sudo ./$(TEST_APP)

# Pattern rule for compiling source files
build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Special rule for websocket_main.o (not in src/)
build/websocket_main.o: src/websocket_main.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c src/websocket_main.cpp -o build/websocket_main.o

# Clean targets
remove:
	rm -f build/*.o $(APP) $(TEST_APP) $(WS_APP)

remove_objects:
	rm -f build/*.o

no_obj: all remove_objects

clean: remove