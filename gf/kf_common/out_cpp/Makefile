CC=g++
CXXFLAGS=-g -Wall -g3
TAGEXE=out_calc
OBJECT=out_cpp.o
INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2
LIBPATH = 
LIBS	= -ltaomee -ltaomee++ -lxml2

$(TAGEXE):$(OBJECT)
	$(CC) $(INCLUDE) $(CXXFLAGS) -o $(TAGEXE) $(OBJECT) $(LIBS)

out_cpp.o:out_cpp.cpp
	$(CC) $(CXXFLAGS) -c $(INCLUDE) out_cpp.cpp

.PHONY:clean
clean:
	-rm $(TAGEXE) $(OBJECT)
