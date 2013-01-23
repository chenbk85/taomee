XX	= g++
CFLAGS	= -Wall -O3 -ggdb -D_GNU_SOURCE
LIBS	= -ldl -lrt -lpthread
INCLUDE	= -I. -I../library/base -I../library/net
SOURCES	= $(wildcard *.cpp)
OBJS	= $(patsubst %.cpp, %.o, $(SOURCES))
SUBOBJS	= ../library/base/*.o ../library/net/*.o ../library/net/reactor_impl/*.o ../library/net/http/*.o


all: $(OBJS) $(SUBOBJS)

static: $(OBJS) $(SUBOBJS)
	ar -r libnet_io.a.0.1 $(OBJS) $(SUBOBJS)
%.o: %.cpp
	$(XX) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(SUBOBJS):
	@make_all_sub_dir()\
	{\
		for dir in `ls -F | grep "/"`; do \
			cd $$dir;\
			make_all_sub_dir;\
			if [ -f makefile -o -f Makefile ]; then\
				make;\
			fi;\
			cd ..;\
		done;\
	};\
	make_all_sub_dir

clean:
	rm -rf *.o $(SUBOBJS)
	@make_clean_sub_dir()\
	{\
		for dir in `ls -F  | grep "/"`; do \
			cd $$dir;\
			make_clean_sub_dir;\
			if [ -f makefile -o -f Makefile ]; then\
				make clean;\
			fi;\
			cd ..;\
		done;\
	};\
	make_clean_sub_dir

rebuild: clean all

c: clean

r: rebuild


