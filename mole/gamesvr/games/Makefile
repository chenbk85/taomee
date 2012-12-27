CC = g++

FLAGS   = -Wall -pipe -DDEBUG -g -D_GNU_SOURCE  -D_REENTRANT $(SHARE_FLAG)
#SOFLAGS = -shared -fPIC

INCLUDE = -I../../ -I../

OO	= mpog.o
#TARGETS = test.out
#GAME    = test.cpp

all: $(OO) #$(TARGETS)

#$(TARGETS): $(OO) $(GAME)
#	$(CC) $(FLAGS) $(SOFLAGS) $(INCLUDE) $(OO) $(GAME) -o $@

%.o: %.cpp %.hpp
	$(CC) $(FLAGS) $(INCLUDE) -c $<

clean:
	rm -f *.o
	rm -f $(TARGETS)
