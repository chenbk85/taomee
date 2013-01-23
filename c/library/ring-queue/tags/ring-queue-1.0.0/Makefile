TARGETS = libringqueue.so.1.0.0
CC 	= g++ 
CFLAGS  = -Wall -ggdb -shared -D_GNU_SOURCE -fPIC -export-dynamic -Wno-non-virtual-dtor
LIB     = -ldl -lrt 
LDFLAGS = $(CFLAGS) 
OO      = ring_queue_impl.o fixed_queue_impl.o variable_queue_impl.o

all: $(OO) $(TARGETS)
        
$(TARGETS): $(OO)
	$(CC) -Wl,-soname,libringqueue.so.1 $(CFLAGS) $(LDFLAGS) $(OO) -o $@ $(LIBDIR) $(LIB)
	/sbin/ldconfig -n -v .

$(OO): %.o: %.cpp %.h Makefile
	$(CC)  $(CFLAGS) -c  $< -o $@
	echo $@

.PHONY : clean
clean:
	rm -f *.o
	rm *.so.*
