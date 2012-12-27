CC = g++ 

WORKDIR=$(HOME)/DB
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
LIBDIR = -L/usr/lib  -L/usr/lib/mysql  -L$(WORKDIR)/lib
LIB 	= -ldbtable -lproto -lpub -ldl -ltaomee  -lmysqlclient  -lxml2  
INCLUDE=-I ~/DB/include/   -I/usr/include/mysql -I$(WORKDIR)/include -I/usr/include/libxml2/ 

INPUT_SERIAL = input_serial
all : $(INPUT_SERIAL) clean

$(INPUT_SERIAL):  input_serial.o
	$(CC)  input_serial.o  -o $@ $(LIBDIR) $(LIB)

.cpp.o:
	$(CC)  $(CFLAGS) -g -c  $< $(INCLUDE)
	echo $@
.c.o:
	gcc  $(CFLAGS) -g -c  $< $(INCLUDE)
	echo $@

clean:
	rm -f *.o 
