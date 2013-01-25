CC = g++

#CFLAGS  = -pipe -DDEBUG -g -D_GNU_SOURCE -shared -fPIC -D_REENTRANT -parallel -axSSE4.2 -ivdep -ansi-alias -restrict -fargument-alias -funroll-all-loops -opt-prefetch
CFLAGS  = -pipe -Wall -DDEBUG -g3 -O0 -D_GNU_SOURCE -shared -fPIC -D_REENTRANT
LIB 	= -lglib-2.0 -ltaomee -lpthread
INCLUDE = -I./include/ -I./include/iter_serv/ -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/

#OO	= dll_if.o ip_dict.o load_conf_ip_hash_mmap.o polling_files.o process_packet.o update_mmap.o send_data.o cache.o
SOURCE = $(wildcard *.cpp)
OO = $(patsubst %.cpp,%.o,$(SOURCE))
TARGETS = rebuild_database.so

all: $(OO) $(TARGETS)
	
$(TARGETS):
	$(CC) $(CFLAGS) $(OO) -o $@ $(LIBDIR) $(LIB)
#	rm ../bin/$(TARGETS); mv $(TARGETS) ../bin/
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
#	echo $@

%:%.cpp
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)
#	echo $@

clean:
	rm -f *.o
	rm -f $(TARGETS)

