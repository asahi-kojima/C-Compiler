CC=g++
CFLAGS=-std=c++11 -g -static
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean