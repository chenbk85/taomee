CC = g++ 

WORKDIR=$(HOME)/DB
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
LIBDIR = -L/usr/lib  -L/usr/lib/mysql  -L$(WORKDIR)/lib
LIB 	= -ldb -lproto -lpub -ldl  -lmysqlclient  -lxml2 
INCLUDE = -I../src  -I/usr/include/mysql -I$(WORKDIR)/include -I/usr/include/libxml2/ 

OO	=  	Cdamee_history.o   Cpay_history.o   service.o  \
		Croute_func.o   Cattire_conf.o Cincrement.o 
TARGETS =libtest.so
INPUT_SHM = input_shm

#all: $(OO) $(TARGETS) $(INPUT_SHM) clean
all: $(OO) $(TARGETS) clean
	
$(TARGETS): 
	$(CC) $(CFLAGS) $(INCLUDE) $(OO) -o $@ $(LIBDIR) $(LIB)
	rm ../bin/$(TARGETS); mv $(TARGETS) ../bin/

$(INPUT_SHM): input_shm.o
	$(CC)  input_shm.o  -o $@ $(LIBDIR) $(LIB)
	rm ../bin/$@; mv $@ ../bin/

.cpp.o:
	$(CC)  $(CFLAGS) -g -c $(INCLUDE) $<
	echo $@

clean:
	rm -f *.o 
