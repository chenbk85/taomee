CC = g++ 

WORKDIR=$(HOME)/DB
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
LIBDIR = -L/usr/lib  -L/usr/lib/mysql  -L$(WORKDIR)/lib
LIB 	= -ldb -lpub   -lmysqlclient 
INCLUDE= -I/usr/include/mysql -I$(WORKDIR)/include

INPUT_SERIAL = test  
all : $(INPUT_SERIAL) clean
OO=Cvalidationcode.o test.o

$(INPUT_SERIAL): $(OO)
	$(CC)  $(OO)  -o $@ $(LIBDIR) $(LIB)

.cpp.o:
	$(CC)  $(CFLAGS) -g -c  $< $(INCLUDE)
	echo $@

clean:
	rm -f *.o 
