CC = g++ 

WORKDIR=$(HOME)/DB
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
LIBDIR = -L/usr/lib  -L/usr/lib/mysql  -L$(WORKDIR)/lib
LIB 	= -ldb -lproto -lpub -ldl  -lmysqlclient  -lxml2  
INCLUDE = -I../../src  -I/usr/include/mysql -I$(WORKDIR)/include  -I/usr/include/libxml2/ 

TARGETS =libtest.so
INPUT_SHM = input_shm
DEAL_DAMEE = deal_month_damee
SET_DAMEE_DEALMSG = set_damee_dealmsg

all:  $(SET_DAMEE_DEALMSG) $(DEAL_DAMEE) clean 
	
$(DEAL_DAMEE): deal_month_damee.o
	$(CC)  deal_month_damee.o  -o $@ $(LIBDIR) $(LIB)
	cp $@ ../../bin/

$(SET_DAMEE_DEALMSG): set_damee_dealmsg.o
	$(CC)  set_damee_dealmsg.o  -o $@ $(LIBDIR) $(LIB)
	cp $@ ../../bin/



.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

clean:
	rm -f *.o 
