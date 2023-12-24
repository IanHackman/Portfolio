CXX      = clang++
CXXFLAGS = -g3 -Wall -Wextra -Wpedantic -Wshadow
LDFLAGS  = -g3 

zap: main.o HuffmanCoder.o BinaryIO.o HuffmanTreeNode.o 
	$(CXX) $(LDFLAGS) -o zap main.o HuffmanCoder.o BinaryIO.o HuffmanTreeNode.o

main.o: main.cpp BinaryIO.h HuffmanTreeNode.h HuffmanCoder.h
	$(CXX) $(CXXFLAGS) -c main.cpp

HuffmanCoder.o: HuffmanCoder.cpp HuffmanTreeNode.h BinaryIO.h
	$(CXX) $(CXXFLAGS) -c HuffmanCoder.cpp

clean: 
	rm !(*.o)

provide: 
    