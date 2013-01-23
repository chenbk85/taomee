XX	= g++
CFLAGS	= -Wall -pipe -D_GNU_SOURCE -fPIC -shared -ggdb
LIBS	= -ldl
INCLUDE	= -I.
SOURCES	= $(wildcard *.cpp)
OBJS	= $(patsubst %.cpp,%.o,$(SOURCES))
ALL_SO  = $(patsubst %.o, %.so, $(OBJS))

all: $(ALL_SO)

$(ALL_SO): $(OBJS)
	$(XX) $(CFLAGS) $(patsubst %.so, %.o, $(notdir $@)) -o $@

$(OBJS): %.o: %.cpp  Makefile
	$(XX) $(CFLAGS) $(INCLUDE) -c $< -o $@ $(LIBS)

clean:
	rm -rf *.o $(ALL_SO)

rebuild: clean all

c: clean

r: rebuild


