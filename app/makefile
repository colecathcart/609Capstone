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
           src/websocket_server.cpp

# Object files
OBJ = $(SRC:src/%.cpp=%.o)

# Executables
APP = ransomware_detector
TEST_APP = test_app
WS_APP = websocket_server_app

# Default target
all: $(APP)

# Build main application
$(APP): $(OBJ)
	$(CXX) $(OBJ) $(LDFLAGS) -o $(APP)

# Build websocket_server_app
$(WS_APP): websocket_main.o websocket_server.o
	$(CXX) websocket_main.o websocket_server.o $(LDFLAGS) -lpthread -o $(WS_APP)

# Test runner
test: $(TEST_SRC) tests/test_main.cpp tests/test_entropy_calculator.cpp tests/test_file_extension_checker.cpp \
      tests/test_event_detector.cpp tests/test_analyzer.cpp tests/test_process_killer.cpp tests/test_logger.cpp
	$(CXX) $(CXXFLAGS) $(TEST_SRC) tests/test_main.cpp tests/test_entropy_calculator.cpp \
	tests/test_file_extension_checker.cpp tests/test_event_detector.cpp \
	tests/test_analyzer.cpp tests/test_process_killer.cpp tests/test_logger.cpp $(LDFLAGS) $(LDLIBS) -o $(TEST_APP)
	sudo ./$(TEST_APP)

# Pattern rule for compiling source files
%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Special rule for websocket_main.o (not in src/)
websocket_main.o: src/websocket_main.cpp
	$(CXX) $(CXXFLAGS) -c src/websocket_main.cpp -o websocket_main.o

# Clean targets
remove:
	rm -f $(OBJ) websocket_main.o $(APP) $(TEST_APP) $(WS_APP)

remove_objects:
	rm -f $(OBJ) websocket_main.o

no_obj: all remove_objects

clean: remove