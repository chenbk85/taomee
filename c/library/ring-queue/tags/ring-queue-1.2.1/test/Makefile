XX       = g++
CFLAGS   = -Wall -ggdb -Wno-non-virtual-dtor
LIBS     = -L../ -lringqueue -lpthread -ltaomee 
INCLUDES = -I../
SOURCES  = $(wildcard *.cpp)
OBJS     = $(patsubst %.cpp,%.o,$(SOURCES))
TARGET   = $(patsubst %.cpp,%,$(SOURCES))
 
all: $(TARGET)

%: %.cpp
	$(XX) $(CFLAGS) $(INCLUDES) $(LIBS) $< -o $@

clean:  
	rm -rf *.o $(TARGET)            

test: $(TARGET)
	@export LD_LIBRARY_PATH=../:$LD_LIBRARY_PATH && set +x; \
	for test in $(TARGET); do \
		./$$test;  \
	done

c: clean 

r: clean all
