CC = g++ 

WORKDIR=$(HOME)/register
CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT -z defs -fpermissive -fPIC
#CFLAGS  = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE -DTW_VER -shared -D_REENTRANT -z defs -fpermissive
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT
LIBDIR = -L/usr/lib   -L$(WORKDIR)/pkg -L$(HOME)/libsrc
LIB 	= -ldl -lproto -lpub -ltaomee -lmsglog -lxml2
INCLUDE = -I$(WORKDIR)/pkg    -I$(WORKDIR)/inc -I$(WORKDIR)/dirtyFilter

OO	= service.o Cregdeal.o    CNetComm.o 
TARGETS =libregister.so

all: $(OO) $(TARGETS)
	
$(TARGETS): 
	$(CC) $(CFLAGS) $(INCLUDE) $(OO) -o $@ $(LIBDIR) $(LIB)
	mv $(TARGETS)  $(WORKDIR)/bin
	rm -f *.o
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

%:%.cpp
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS) $(LIB)
	echo $@

clean:
	rm -rf	$(WORKDIR)/src/*.o
	rm -f $(WORKDIR)/bin/$(TARGETS)
