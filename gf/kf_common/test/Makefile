CC=g++
CXXFLAGS=-Wall
TAGEXE=test
OBJECT=main.o
LIB=libkfcommon.a

$(TAGEXE):$(OBJECT)
	$(CC) $(CXXFLAGS) -o $(TAGEXE) $(OBJECT) $(LIB)

main.o:main.cpp
	$(CC) $(CXXFLAGS) -c main.cpp

.PHONY:clean
clean:
	-rm $(TAGEXE) $(OBJECT)
