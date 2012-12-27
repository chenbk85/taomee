STATIC=1

CFLAGS= -Wall -fPIC 
CC=g++ -g

ifeq ($(STATIC), 1)
	ifdef VERSION_KAIXIN
		TARGET=libkfkxcommon.a
		CFLAGS+= -DVERSION_KAIXIN
	else
		TARGET=libkfcommon.a
	endif
else
	TARGET=player_calc
endif

HPP=player_attr.hpp item_impl.hpp item_manager.hpp vip_config_data_mgr.hpp  home.hpp pet_attr.hpp pack_ex.hpp memstream.hpp title_attr.hpp math.hpp singleton.hpp utils.hpp fight_team.hpp
SRC=gf_calc.cpp item_impl.cpp vip_config_data_mgr.cpp gf_attire_strengthen.cpp  home.cpp pet_attr.cpp memstream.cpp title_attr.cpp utils.cpp fight_team.cpp
OBJ=gf_calc.o item_impl.o  vip_config_data_mgr.o gf_attire_strengthen.o  home.o pet_attr.o memstream.o title_attr.o utils.o fight_team.o

INCLUDE= -I/usr/include/libxml2 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include/ -I/usr/include/libtaomee++/inet/
LIBS= -ltaomee -ltaomee++

ifeq ($(STATIC), 1)
$(TARGET): $(OBJ)
	ar -rvs $(TARGET) $(OBJ)
	cp -f $(TARGET) ../test/
	cp -f $(HPP) ../test/
	sudo cp -f $(HPP) /usr/local/include/kf/
	sudo cp -f $(TARGET) /usr/local/lib/
else
	$(CC) $(CFLAGS) -o $(OBJ)
endif
$(OBJ):$(SRC)
	$(CC) $(CFLAGS) $(INCLUDE) -c $(SRC) $(LIBS)


clean:
	rm -f $(TARGET) $(OBJ)

