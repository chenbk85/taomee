CC = g++

CFLAGS  = -Wall -g -shared -fPIC
LIB 	= -ldl -lrt 
INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/

OO1	= reload.o
TARGET1 = reload.so
OO2	= data.o
TARGET2 = data.so
OO = $(OO1) $(OO2)

all: $(OO) $(TARGET1) $(TARGET2)

$(TARGET1): $(OO1)
	rm -f $@
	$(CC) $(CFLAGS) $(OO1) -o $@ $(LIBDIR) $(LIB)

$(TARGET2): $(OO2)
	rm -f $@
	$(CC) $(CFLAGS) $(OO2) -o $@ $(LIBDIR) $(LIB)

.cpp.o:
	$(CC)  $(CFLAGS) -c $(INCLUDE) $<

%:%.c
	$(CC) $(CFLAGS) -o $@ $< $(OO) $(LDFLAGS)

clean:
	rm -f *.o
	rm -f $(TARGETS)
