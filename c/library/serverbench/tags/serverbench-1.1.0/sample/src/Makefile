CC = g++
INCLUDE = -I./ -I../../../serverbench/src/

CFLAGS  = -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -fPIC -shared -export-dynamic -D_REENTRANT
LIB 		= -lpthread -ldl -lrt -lmysqlclient -lssl
LDFLAGS	=  $(CFLAGS) $(INCLUDE)

OO	= collector_server.o collector_config.o serverbench_interface.o mysql_res_auto_ptr.o db_operator.o
TARGETS = liblogininfocollector.so 

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

