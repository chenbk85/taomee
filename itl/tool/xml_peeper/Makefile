CC = g++
INCLUDE = -I./ 
CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -fPIC -D_REENTRANT

LIB     = -ldl -lrt 
LDFLAGS	=  $(CFLAGS) $(INCLUDE)

OO	= data_processer.o net_client_impl.o main.o 
TARGETS = xml_peeper

all: $(OO) $(TARGETS)
	
$(TARGETS): $(OO)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OO) -o $@ $(LIBDIR) $(LIB)

$(OO): %.o: %.cpp %.h
	$(CC)  $(CFLAGS) -c  $(INCLUDE) $< -o $@
	echo $@

.PHONY : clean
clean:
	rm -f *.o
	rm -f $(TARGETS)
	rm -f *.xml
c:clean

