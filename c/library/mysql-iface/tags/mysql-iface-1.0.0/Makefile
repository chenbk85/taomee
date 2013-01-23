TARGETS = libsafemysql.so.1.0.0
CC 	= g++ 
CFLAGS  = -Wall -shared -pipe -ggdb -D_GNU_SOURCE -fPIC -Wno-non-virtual-dtor `mysql_config --cflags`
LIB     = -ldl -lrt `mysql_config --libs`
LDFLAGS = $(CFLAGS) 
OO      = c_mysql_iface.o

all: $(OO) $(TARGETS)
        
$(TARGETS): $(OO)
	$(CC) -Wl,-soname,libsafemysql.so.1.0 $(CFLAGS) $(LDFLAGS) $(OO) -o $@ $(LIBDIR) $(LIB)
	@/sbin/ldconfig -n -v .

$(OO): %.o: %.cpp %.h Makefile
	$(CC)  $(CFLAGS) -c  $< -o $@
	echo $@

.PHONY : clean
clean:
	rm -f *.o
	rm -f *.so.*
	
