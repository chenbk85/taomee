CC = g++
WORKDIR=../../
CFLAGS  =  -g3 -O0 -Wall -pipe -DDEBUG -DLOG_SQL_DEBUG  -D_NEW_LIC  -D_GNU_SOURCE -shared -D_REENTRANT -fPIC
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
LIBDIR = -L/usr/lib -L/usr/lib/mysql  -L$(WORKDIR)/lib
LIB 	= -ldl    -lmysqlclient
INCLUDE =  -I$(WORKDIR)/include -I/usr/include/mysql

OO	=  CtableRoute100x100_ex.o CtableRoute10.o  CtableRoute100x10.o CtableRoute100.o CtableRoute10x10.o CtableString.o CtableDate.o CtableMonth.o Ctable.o CtableWithKey.o  CtableRoute.o  mysql_iface.o

TARGETS = libdbtable.a

all: $(OO) $(TARGETS)
	
$(TARGETS):
	ar -rv  $@ $(OO) 
	cp $(TARGETS) $(WORKDIR)/lib/
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

clean:
	rm -f $(TARGETS)  $(OO)  
