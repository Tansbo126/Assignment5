# Makefile for the RPC Server Application (JSON Version)

# Compiler and flags
CXX = g++
# C++17 required for std::variant (if used) and recommended for modern C++
# -pthread is needed for std::thread if used (e.g., for bonus concurrency)
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread

# Include directories
INC_DIR = ../rpc_lib/cpp/include
JSON_INC_DIR = /usr/include/jsoncpp # Explicit path to jsoncpp headers
INCLUDES = -I$(INC_DIR) -I$(JSON_INC_DIR) # Combined include paths

# Source directories
APP_SRCDIR = .
LIB_SRCDIR = ../rpc_lib/cpp/src

# Source files
# Include server app sources and the necessary library sources
APP_SOURCES = $(wildcard $(APP_SRCDIR)/*.cpp)
LIB_SOURCES = $(wildcard $(LIB_SRCDIR)/rpc_connection.cpp) $(wildcard $(LIB_SRCDIR)/rpc_server_stub.cpp)

SOURCES = $(APP_SOURCES) $(LIB_SOURCES)
OBJS = $(SOURCES:.cpp=.o)

# Library flags
# Link with pthread (for std::thread, potentially used in server stub)
# Link with jsoncpp for JSON parsing/serialization
LDFLAGS = -lpthread -ljsoncpp

# Executable name
TARGET = server

# --- Rules ---

all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
# This rule handles both app sources and library sources
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	# Clean object files possibly generated in lib dir if using alternative rules
	# rm -f $(LIB_SRCDIR)/*.o

.PHONY: all clean
