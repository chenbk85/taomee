USER_MACROS=

ifdef  NEED_MEMCACHE
USER_MACROS += -DNEED_MEMCACHE
endif

ifdef TW_VER
USER_MACROS += -DTW_VER
endif

ifdef MAKE_SEND_EMAIL
USER_MACROS += -DMAKE_SEND_EMAIL
endif


target:=libser.so
WORKDIR:=../../

comlier:=g++
linker:=g++
cflags:=  -pipe -D_NEW_LIC -g -D_GNU_SOURCE \
		  -D_REENTRANT -D__STDC_FORMAT_MACROS -fPIC $(USER_MACROS)

.deps/%.d: cflags += -M -I../src  -I/usr/include/mysql -I$(WORKDIR)/include
.objs/%.o: cflags += -c -Wall -I../src  -I/usr/include/mysql -I$(WORKDIR)/include

lib_cfgs:= -L/usr/lib  -L/usr/lib/mysql  -L$(WORKDIR)/lib \
		    -lproto -ldbtable  -lpub -ldl  -lmysqlclient -ltaomee

all_srcs:=$(wildcard *.cpp)
all_objs:=$(addprefix .objs/,$(patsubst %.cpp,%.o,$(all_srcs)))
all_deps:=$(addprefix .deps/,$(patsubst %.cpp,%.d,$(all_srcs)))


$(shell mkdir -p .deps .objs)

all: $(target) $(all_deps) $(all_objs) $(all_srcs) tags

ifneq ($(MAKECMDGOALS),clean)
-include $(all_deps)
endif

.PHONY: clean send_mail tags

.deps/%.d:%.cpp
	@echo "generate $@ ..."
	@$(comlier) -M $(cflags) $< > $@.$$$$;\
	sed 's,\($*\)\.o[ :]*,.objs/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$ 

.objs/%.o:%.cpp
	@echo "generate $@ ..."
	@$(comlier) $(cflags) $< -o $@

$(target): $(all_objs)
	@echo "generate $@ ..."
	@$(linker) -shared $(cflags) $+ $(lib_cfgs) -o $@

clean:	
	@echo $(cflags)
	@echo "clean ..."
	@rm -f $(all_objs) $(all_deps) $(target) .deps/* .objs/*

#这用于专门的邮件处理服务器
send_mail: 
	@echo "make for sendmail server ..."
	make clean
	make MAKE_SEND_EMAIL=1
	mv $(target) libvipsendser.so
	make clean

mole: $(target)
	mv $(target) /home/mole/DB/ser/bin

#tags:
#	@echo "taging ..."
#	@ctags -R  --append=no --c++-kinds=+p --fields=+iaS --extra=+q --language-force=c++ --exclude=../../doc ../../

install: $(target)
	mv ../bin/libser.so ../bin/libser.so.bak
	mv -f $(target) ../bin

