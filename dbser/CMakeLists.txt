cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_INSTALL_PREFIX "/usr/")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_USE_RELATIVE_PATHS  true )

find_package(glib REQUIRED)
find_package(mysql REQUIRED)
find_package(taomee REQUIRED)
find_package(taomee++ REQUIRED)
set (SRC_LIST 
	Citem_change_log.cpp
	Csync_user_data.cpp
	Ctable.cpp
	CtableDate_100.cpp
	CtableDate.cpp
	CtableMonth.cpp
	CtableRoute100.cpp
	CtableRoute100x10.cpp
	CtableRoute100x1.cpp
	CtableRoute10.cpp
	CtableRoute10x10.cpp
	CtableRoute.cpp
	CtableString.cpp
	CtableWithKey.cpp
	mysql_iface.cpp
	)

set (HEADER_LIST 
	benchapi.h
	Cbig_cache.h
	Ccmdmaplist.h
	Ccmdmap_private_checklen.h
	Cfunc_route_base.h
	Cfunc_route_cmd.h
	Cfunc_route_cmd_v2.h
	Cfunc_route_db_v2.h
	Csync_user_data.h
	CtableDate_100.h
	CtableDate.h
	Ctable.h
	CtableMonth.h
	CtableRoute100.h
	CtableRoute100x10.h
	CtableRoute100x1.h
	CtableRoute10.h
	CtableRoute10x10.h
	CtableRoute.h
	CtableString.h
	CtableWithKey.h
	Citem_change_log.h
	db_error_base.h
	db_macro.h
	mysql_iface.h
	proto_header.h
	proxy_dll_interface.h
	proxy_route.h
   	)

INCLUDE_DIRECTORIES( "./", ${MYSQL_INCLUDE_DIR})
ADD_LIBRARY(dbser  ${SRC_LIST} 	)
ADD_DEFINITIONS( "-g -DDEBUG -Wall  -MMD  -funroll-loops -D__USE_STRING_INLINES  -fPIC " )
INSTALL (TARGETS dbser  DESTINATION lib)
INSTALL (FILES  ${HEADER_LIST} DESTINATION  include/dbser/	)	

