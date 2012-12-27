CC = g++ 

WORKDIR=$(HOME)/del_server/DB
ifdef  NEED_MEMCACHE 
MC_FLAG= -DNEED_MEMCACHE
else
MC_FLAG=
endif

ifdef  MAKE_SEND_EMAIL 
SEND_EMAIL_FLAG= -DMAKE_SEND_EMAIL
else
SEND_EMAIL_FLAG=
endif



CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared \
		  -D_REENTRANT -D__STDC_FORMAT_MACROS -fPIC $(MC_FLAG) $(SEND_EMAIL_FLAG)  

#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT -D__STDC_FORMAT_MACROS -fPIC    
#make for SEND_EMAIL
#CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT -DMAKE_SEND_EMAIL
LIBDIR =  -L/usr/lib  -L/usr/lib/mysql  -L$(WORKDIR)/lib
LIB 	= -ldbtable -lpub -ldl  -lmysqlclient -ltaomee
INCLUDE = -I../src  -I/usr/include/mysql -I$(WORKDIR)/include
#	Cmc.o
  
OO	= service.o Croute_func.o CNetComm.o Cgf_dbproxy.o Cgf_role_deleted.o
TARGETS = libgfser.so

cur: start_make  $(OO) $(TARGETS) end_make
all: $(OO) $(TARGETS) end_make
start_make:
	mv ./.obj/*.o . || echo "--" 
end_make:
	mv  *.o ./.obj/  

$(TARGETS):
	$(CC) $(CFLAGS) $(INCLUDE) $(OO) -o $@ $(LIBDIR) $(LIB)
	rm ../bin/$(TARGETS); mv $(TARGETS) ../bin/
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

%:%.cpp
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)
	echo $@

clean:
	rm -f $(TARGETS)
