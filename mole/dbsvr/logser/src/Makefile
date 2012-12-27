CC = g++ 
WORKDIR=$(HOME)/DB
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE  -D_REENTRANT
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
LIBDIR = -L$(WORKDIR)/lib -L/usr/lib  -L/usr/lib/mysql 
LIB 	= -ldb -ldl  -lmysqlclient
INCLUDE = -I/usr/include/mysql -I$(WORKDIR)/include

OO =   Cdbdeal.o    Clogtable.o log.o 
TARGETS= collect 
TARGETSOBJ= collect.o 
STATISTICS=statistics 
STATISTICSOBJ=statistics.o 

all: $(OO) $(TARGETS) $(STATISTICS) clean
	
$(TARGETS): $(TARGETSOBJ)  $(OO) 
	$(CC) $(CFLAGS) $(TARGETSOBJ) $(OO) -o $@ $(LIBDIR) $(LIB)
	mv $(TARGETS) ~/bin/

$(STATISTICS): $(STATISTICSOBJ)  $(OO) 
	$(CC) $(CFLAGS) $(STATISTICSOBJ)  $(OO) -o $@ $(LIBDIR) $(LIB)
	mv $(STATISTICS) ~/bin/

.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@  $()

clean:
	rm -f *.o 
