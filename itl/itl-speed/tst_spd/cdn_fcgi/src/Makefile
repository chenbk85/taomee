CC = gcc
#CFLAGS= -Wall -g3 -L./ -lfcgi
CFLAGS = -Wall -g3 -O0
#LIB = -L./ -lfcgi
LIB = /usr/lib/libfcgi.a
CFLAGS_S= -Wall -g3 -O0 -DENCODE_
# -DFAKE_CLIENT_
SOURCE = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SOURCE))

TARGET=cdn_fcgi

all: $(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

%.o:%.c
	$(CC) $(CFLAGS_S) -o $@ -c $^

cleanall:
	rm -f *.o $(TARGET)
cleanobj:
	rm -f *.o
#====================================
src:obj
	@echo $(SOURCE)
obj:
	@echo $(OBJ)

