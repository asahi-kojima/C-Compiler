CC=g++
CFLAGS=-std=c++20 -g -static
# SRCS=$(filter-out func_example.cpp , $(wildcard *.cpp))
# OBJS=$(SRCS:.cpp=.o)

compiler.out: essential.h main.cpp
	g++ -o compiler.out main.cpp

test: compiler.out
	bash ./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean