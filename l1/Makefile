CXX = g++
CXXFLAGS = -std=c++11 -Wall -O3

all: main

main: main.o heuristics.o node.o utils.o
	$(CXX) $(CXXFLAGS) -o main main.o heuristics.o node.o utils.o

main.o: src/main.cpp heuristics.o
	$(CXX) $(CXXFLAGS) -c src/main.cpp

heuristics.o: src/heuristics.cpp node.o utils.o
	$(CXX) $(CXXFLAGS) -c src/heuristics.cpp

utils.o: src/utils.cpp src/utils.h node.o
	$(CXX) $(CXXFLAGS) -c src/utils.cpp

node.o: src/node.cpp src/node.h
	$(CXX) $(CXXFLAGS) -c src/node.cpp

clean:
	rm -f main *.o