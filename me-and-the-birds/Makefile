PROJECT_NAME=ProjectThree
EXEC=$(PROJECT_NAME)
CXX=g++
CXXFLAGS=-std=c++11 -Wall -pedantic -g
OBJS=$(PROJECT_NAME).cpp DiskDrive.cpp FileSystem.cpp
HEADER=DiskDrive.h FileSystem.h ProjectThree.h

all: $(EXEC)

clean:
	rm -f $(PROJECT_NAME)

$(EXEC): $(OBJS) $(HEADER)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS)