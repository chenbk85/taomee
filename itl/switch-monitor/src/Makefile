XX       = g++
#CFLAGS   = -Wall -O2 -ggdb -Wno-non-virtual-dtor -D_GNU_SOURCE `net-snmp-config --cflags`
#CFLAGS   = -Wall -O2 -ggdb -Wno-non-virtual-dtor -D_GNU_SOURCE
CFLAGS   = -Wall -O -ggdb -Wno-non-virtual-dtor -D_GNU_SOURCE -DRELEASE_VERSION 
LIBS     = -lpthread -lcairo -lrrd -lcurl `mysql_config --libs` `net-snmp-config --libs` `net-snmp-config --agent-libs`
TARGET   = ../bin/switch_monitor
SOURCES  = $(wildcard ./*.cpp)
OBJS     = $(patsubst %.cpp,%.o,$(SOURCES)) 

SUBSOURCES  = $(wildcard ./*/*.cpp)
SUBOBJS  = $(patsubst %.cpp,%.o,$(SUBSOURCES))

all: $(TARGET)

$(TARGET) : $(OBJS) $(SUBOBJS)
	$(XX) $(OBJS) $(SUBOBJS) -o $(TARGET) $(LIBS)
	rm -rf *.o
	
%.o: %.cpp
	$(XX) $(CFLAGS) -c $< -o $@	
	
$(SUBOBJS):
	@make_all_sub_dir()\
	{\
		for dir in `ls`; do \
			if [ -d $$dir ]; then \
				cd $$dir;\
				make_all_sub_dir;\
				if [ -f makefile -o -f Makefile ]; then\
					make;\
				fi;\
				cd ..;\
			fi;\
		done;\
	};\
	make_all_sub_dir		

clean:
	rm -rf *.o $(TARGET)
	@make_clean_sub_dir()\
	{\
		for dir in `ls`; do \
			if [ -d $$dir ]; then \
				cd $$dir;\
				make_clean_sub_dir;\
				if [ -f makefile -o -f Makefile ]; then\
					make clean;\
				fi;\
				cd ..;\
			fi;\
		done;\
	};\
	make_clean_sub_dir
	
rebuild: clean all	
	
c: clean

r: rebuild
