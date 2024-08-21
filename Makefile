CC=g++
CFLAGS=-std=c++20 -g -static
SRCS=$(filter-out func_example.cpp , $(wildcard *.cpp))
OBJS=$(SRCS:.cpp=.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(CFLAGS) $(OBJS)

$(OBJS): 9cc.h

%.o : %.cpp 9cc.h
	$(CC) -c -o $@ $(CFLAGS) $<

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean