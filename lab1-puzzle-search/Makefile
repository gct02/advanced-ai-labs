# Builds the `main` executable required by the assignment: just run `make`.
CXX      ?= g++
CXXFLAGS ?= -std=c++17 -O3 -march=native -Wall -Wextra

SRCS := src/main.cpp src/puzzle.cpp src/search.cpp
OBJS := $(SRCS:src/%.cpp=build/%.o)

main: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp src/puzzle.h src/search.h | build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build:
	mkdir -p build

test: main
	python3 tests/check_examples.py

clean:
	rm -rf build main

.PHONY: test clean
