INC_DIR = -I/usr/local/include
LIB_DIR =
LIBS = -lconfuse -levent_core -levent_extra
FLAG = -g -Wall


CC=gcc

BINARY=show_libevent_info

all: $(BINARY)

show_libevent_info: show_libevent_info.c
	$(CC) $(FLAG) $(INC_DIR) -o $@ $^ $(LIB_DIR) $(LIBS)

clean:
	rm -rf *.o $(BINARY)
