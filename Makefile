CXX=g++
CXXFLAGS=-std=c++17 -g -static -Wall

SRC_DIRS = .
BUILD_DIR = ./build

TARGET=compiler.out

DUMMY_MKDIR:=$(shell mkdir -p build)

SRCS=$(wildcard $(SRC_DIRS)/*.cpp)
OBJS=$(patsubst $(SRC_DIRS)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))
#OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

all: build/$(TARGET)

build/$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIRS)/%.cpp
	echo $^ Compile Start!
	$(CXX) $(CXXFLAGS) -o $@ -c $<


test: $(BUILD_DIR)/compiler.out
	bash ./test.sh

test-on-docker:
	docker compose -f DockerEnv/docker-compose.yml up

clean:
	rm -f *.out *.o *~ tmp*

.PHONY: all test clean