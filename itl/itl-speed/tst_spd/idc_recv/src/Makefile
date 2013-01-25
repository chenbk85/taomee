CC = gcc
CFLAGS= -Wall -g3 -lpthread
CFLAGS_S= -Wall -g3 -DENCODE_
SOURCE = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SOURCE))

TARGET=idc_recv

all: $(TARGET)

$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) -o $@ $^
	mv $(TARGET) ../
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

