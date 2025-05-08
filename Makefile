CC=g++
CFLAGS=-std=c++17 -g -static
# SRCS=$(filter-out func_example.cpp , $(wildcard *.cpp))
# OBJS=$(SRCS:.cpp=.o)

compiler.out: main.o tokenizer.o parser.o code-generator.o
	$(CC) $(CFLAGS) -o $@ *.o

main.o: main.cpp essential.h tokenizer.h parser.h code-generator.h
	$(CC) $(CFLAGS) -o $@ -c $<

tokenizer.o: tokenizer.cpp tokenizer.h essential.h
	$(CC) $(CFLAGS) -o $@ -c $<

parser.o: parser.cpp parser.h tokenizer.h essential.h
	$(CC) $(CFLAGS) -o $@ -c $<

code-generator.o: code-generator.cpp code-generator.h parser.h essential.h
	$(CC) $(CFLAGS) -o $@ -c $<

test: compiler.out
	bash ./test.sh

test-on-docker:
	docker compose -f DockerEnv/docker-compose.yml up

clean:
	rm -f *.out *.o *~ tmp*

.PHONY: test clean