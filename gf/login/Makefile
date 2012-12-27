CC = g++

#FLAGS   = -Wall -DNDEBUG -O3 -fPIC -MMD
FLAGS   = -Wall -DDEBUG -g -fPIC -MMD 
#FLAGS   = -Wall -DDEBUG -g -fPIC -MMD -DENABLE_TRACE_LOG

SOFLAGS = -shared

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I../ -I/usr/include/libxml2/
#INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libtaomee++/project/main_login -I../
#INCLUDE = -I/home/mark -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ 

#LIBPATH = -L./bin -L/home/mark/libtaomee++/build -ltaomee++
LIBPATH = -L./bin
#LIBS	= -lgd -lrt  -ltaomee -lplayer_attr -lkfcommon
#LIBS	= -lgd -lrt -ltaomee++ -ltaomee -lkfcommon
LIBS	= -lgd -lrt -lxml2 -lssl -lmainlogin -ltaomee++ -ltaomee 

ifdef VERSION_TAIWAN
    FLAGS += -DVERSION_TAIWAN
endif

ifdef VERSION_KAIXIN          
    FLAGS += -DVERSION_KAIXIN 
    LIBS += -lkfkxcommon      
else   
    LIBS += -lkfcommon        
endif 

BINS	= libgf_login.so
SRCS 	= login.cpp login_impl.cpp 

all: $(BINS)

$(BINS): $(SRCS:.cpp=.o)
	$(CC)  $(FLAGS) $(SOFLAGS) -o $@ $^ $(LIBPATH) $(LIBS)
	-mv libgf_login.so ./lib/
#-rm ./bin/$(BINS)
#	mv $(BINS) ./bin

%.o: %.cpp Makefile
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<
	@-mv -f $*.d .dep.$@

clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) core

-include /dev/null $(wildcard .dep.*)
