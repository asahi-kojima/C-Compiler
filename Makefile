CC=g++
CFLAGS=-std=c++17 -g -static
# SRCS=$(filter-out func_example.cpp , $(wildcard *.cpp))
# OBJS=$(SRCS:.cpp=.o)

compiler.out: main.o tokenizer.o
	g++ $(CFLAGS) -o $@ main.o tokenizer.o

main.o: main.cpp essential.h tokenizer.h
	$(CC) $(CFLAGS) -o $@ -c $<

tokenizer.o: tokenizer.cpp tokenizer.h essential.h
	$(CC) $(CFLAGS) -o $@ -c $<

test: compiler.out
	bash ./test.sh

test-on-docker:
	docker compose -f DockerEnv/docker-compose.yml up

clean:
	rm -f *.out *.o *~ tmp*

.PHONY: test clean