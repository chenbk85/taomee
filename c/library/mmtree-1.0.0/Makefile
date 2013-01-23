TARGETS = libmmtree.so
VERSION	= libmmtree.so.1.0.0
CC 	= g++ 
CFLAGS  = -Wall -pipe -ggdb -D_GNU_SOURCE -fPIC -shared -export-dynamic -Wno-non-virtual-dtor 
LIB     = -ldl -lrt 
LDFLAGS = $(CFLAGS) 
OO      = mmtree.o

all: $(OO) $(VERSION)
        
$(VERSION): $(OO)
	$(CC) -shared -Wl,-soname,libmmtree.so.1.0 $(CFLAGS) $(LDFLAGS) $(OO) -o $@ $(LIBDIR) $(LIB)
	@/sbin/ldconfig -n -v .
	ln -fs $(VERSION) $(TARGETS)

$(OO): %.o: %.cpp %.h Makefile
	$(CC)  $(CFLAGS) -c  $< -o $@
	echo $@

.PHONY : clean
clean:
	rm -f *.o
	rm -f *.so*
