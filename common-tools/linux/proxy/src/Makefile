CC = g++

CFLAGS = -Wall -pipe -DDEBUG -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT -fPIC
LIBDIR = -L/usr/lib
LIB    = -ldl -ltaomee -lxml2
INCLUDE = -I../src -I/usr/include/libxml2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/dbser
OO = dll.o CProxyRoute.o CProxyDealBase.o CProxyDealGf.o  CProxyDealSeer.o CProxyDealMole.o CProxyDealHero.o CProxyDealXhx.o CProxyClientManage.o CProxyDealMdd.o CProxyDealSeer2.o
TARGETS = libproxy_route.so

all: clean  $(OO) $(TARGETS)
    
$(TARGETS):
	$(CC) $(CFLAGS) $(INCLUDE) $(OO) -o $@ $(LIBDIR) $(LIB)
	rm ../bin/$(TARGETS); mv $(TARGETS) ../bin/
	rm -f *.o
.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<
	echo $@

clean:
	rm -f *.o
	rm -f $(TARGETS)
