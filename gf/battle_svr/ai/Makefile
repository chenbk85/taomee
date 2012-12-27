CC = g++

ifndef RELEASE_O2
FLAGS  = -Wall -DDEBUG -g -fPIC -MMD
else
FLAGS  = -Wall -DDEBUG -O2 -fPIC -MMD
endif

ifndef RELEASE
FLAGS += -DENABLE_TRACE_LOG 
else
FLAGS += -DRELEASE_VER
endif

INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2 -I/usr/include/lua5.1
#INCLUDE = -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libxml2
LIBPATH = 
LIBS	= -ltaomee -ltaomee++ -lxml2 -lkfcommon
#LIBS	= -ltaomee -ltaomee++ -lxml2 -lkfcommon -llua5.1 -lluabind
#LIBS	= -ltaomee -ltaomee++ -lxml2 -lkfcommon

SRCS 	= AI_interface.cpp MonsterAI.cpp LowLvAI.cpp MiddleLvAI.cpp HighLvAI.cpp \
				LongRangeAtkAI.cpp SpecialMonAI.cpp GuardAI.cpp SummonMonAI.cpp BossAI.cpp \
				MoveAI.cpp

all: $(SRCS:.cpp=.o)
	-cp *.o ../

%.o: %.cpp Makefile
	$(CC) $(FLAGS) $(INCLUDE) -c -o $@ $<
	@-mv -f $*.d .dep.$@

clean:
	@-rm -f *.o *.oxx *.po *.so *.d .dep.* $(BINS) core

-include /dev/null $(wildcard .dep.*)
