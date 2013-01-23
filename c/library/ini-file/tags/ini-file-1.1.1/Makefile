TARGETS = libinifile.so.1.1.1
CC 	= g++
CFLAGS  = -Wall -pipe -ggdb -D_GNU_SOURCE -fPIC -shared -export-dynamic -Wno-non-virtual-dtor
LIB     = -ldl -lrt
LDFLAGS = $(CFLAGS)
OO      = c_ini_file.o

all: $(OO) $(TARGETS)

$(TARGETS): $(OO)
	$(CC) -Wl,-soname,libinifile.so.1 $(CFLAGS) $(LDFLAGS) $(OO) -o $@ $(LIBDIR) $(LIB)
	@/sbin/ldconfig -v -n . && ln -s libinifile.so.1 libinifile.so

$(OO): %.o: %.cpp %.h Makefile
	$(CC)  $(CFLAGS) -c  $< -o $@
	echo $@

.PHONY : clean
clean:
	rm -f *.o
	rm -f *.so*

