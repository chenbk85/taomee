CC = g++ 

WORKDIR=$(HOME)/DB
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
LIBDIR = -L/usr/lib    -L$(WORKDIR)/lib
LIB 	=  -lproto -lpub -ldl   
INCLUDE= -I$(WORKDIR)/include

INPUT_SERIAL = hope_sendmsg
all : $(INPUT_SERIAL) clean

$(INPUT_SERIAL):  hope_sendmsg.o
	$(CC)    hope_sendmsg.o  -o $@ $(LIBDIR) $(LIB)

.cpp.o:
	$(CC)  $(CFLAGS) -g -c  $< $(INCLUDE)
	echo $@
.c.o:
	gcc  $(CFLAGS) -g -c  $< $(INCLUDE)
	echo $@

clean:
	rm -f *.o 
