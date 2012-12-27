CC = g++ 

MYPATH=`pwd`
#WORKDIR=$(HOME)/DB
WORKDIR=$(MYPATH)/../../


kfcommonlib=-lkfcommon

CFLAGS  = -DGF_PROTO_HEADER  -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared \
		  -D_REENTRANT -D__STDC_FORMAT_MACROS -fPIC 

LIBDIR =  -L/usr/lib  -L/usr/lib/mysql  -L$(WORKDIR)/lib
LIB 	=  -ldbser -ldl -lmysqlclient  $(kfcommonlib) -ltaomee++ -ltaomee -lxml2 -lrt -lglib-2.0
INCLUDE = -I/usr/include/libxml2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/dbser/  -I/usr/include/mysql 
  
OO	= service.o Croute_func.o Cgf_mail.o

TARGETS = libgfser.so

all: $(OO) $(TARGETS) 

$(TARGETS):
	$(CC) $(CFLAGS) $(INCLUDE) $(OO) -o $@ $(LIBDIR) $(LIB)
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

%:%.cpp
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)
	echo $@

clean:
	rm -f $(TARGETS)
	rm -f *.o
