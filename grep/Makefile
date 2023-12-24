
CXX      = clang++
CXXFLAGS = -g3 -O2 -Wall -Wextra -Wpedantic -Wshadow
LDFLAGS  = -g3 

gerp: main.o Gerp.o gerpHash.o DirNode.o FSTree.o
	$(CXX) $(LDFLAGS) -o gerp main.o Gerp.o gerpHash.o DirNode.o FSTree.o

main.o: main.cpp Gerp.h gerpHash.h DirNode.h FSTree.h
	$(CXX) $(CXXFLAGS) -c main.cpp

Gerp.o: Gerp.cpp Gerp.h gerpHash.h DirNode.h FSTree.h
	$(CXX) $(CXXFLAGS) -c Gerp.cpp

gerpHash.o: gerpHash.cpp 
	$(CXX) $(CXXFLAGS) -c gerpHash.cpp

clean: 
	rm !(*.o)

provide: 
    