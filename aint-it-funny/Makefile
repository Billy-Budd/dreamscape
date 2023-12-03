PROJECT_NAME=ProjectTwo
EXEC=$(PROJECT_NAME)
CXX=g++
CXXFLAGS=-std=c++11 -Wall -pedantic -pthread -g
OBJS=$(PROJECT_NAME).cpp Output.cpp Functions.cpp

all: $(EXEC)

clean:
	rm -f $(PROJECT_NAME)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^