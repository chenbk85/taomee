# vim: set noexpandtab tabstop=4 softtabstop=4 shiftwidth=4:
XX       = g++
CFLAGS   = -Wall -ggdb -Wno-non-virtual-dtor
LIBS     =  
INCLUDES = -I../../../storage-server/src/ 
SOURCES  = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp, %.o, $(SOURCES))
TARGET   = test_client_1
         
all: $(TARGET)

$(TARGET): $(OBJS)
	$(XX) $(OBJS) -o $(TARGET) $(LIBS)
	rm -rf *.o

                  
%.o: %.cpp  
	$(XX) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:  
	rm -rf *.o $(TARGET)            

c: clean 

r: clean all 
