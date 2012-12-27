target:=db-debuger
WORKDIR:=../../

comlier:=g++
linker:=g++
cflags:= -pipe  -I$(WORKDIR)/include

.deps/%.d: cflags += -M
#-I$(HOME)/include
.objs/%.o: cflags += -c -g3 -O0 -Wall -Wextra
#-I$(HOME)/include

lib_cfgs:= -ldl -lpthread -lboost_program_options

all_srcs:=$(wildcard *.cpp)
all_objs:=$(addprefix .objs/,$(patsubst %.cpp,%.o,$(all_srcs)))
all_deps:=$(addprefix .deps/,$(patsubst %.cpp,%.d,$(all_srcs)))

$(shell mkdir -p .deps .objs)

all: $(target) $(all_deps) $(all_objs) $(all_srcs)

ifneq ($(MAKECMDGOALS),clean)
-include $(all_deps)
endif

.PHONY: clean send_mail

.deps/%.d:%.cpp
	@echo "generate $@ ..."
	@$(comlier) -M $(cflags) $< > $@.$$$$;\
	sed 's,\($*\)\.o[ :]*,.objs/\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$ 

.objs/%.o:%.cpp
	@echo "generate $@ ..."
	$(comlier) $(cflags) $< -o $@

$(target): $(all_objs)
	@echo "generate $@ ..."
	@$(linker) $(cflags) $+ $(lib_cfgs) -o $@

clean:
	@echo "clean ..."
	@rm -f $(all_objs) $(all_deps) $(target) .deps/* .objs/*

tag:
	@echo "taging ..."
	@ctags -R --languages=c++,c --exclude=../../doc ../../

