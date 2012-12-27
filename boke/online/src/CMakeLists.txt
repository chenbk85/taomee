PROJECT(DBSER)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_COMPILER "g++")


find_package(taomee REQUIRED)
find_package(taomee++ REQUIRED)
find_package(glib REQUIRED)

find_package(libxml2 REQUIRED)
find_package(AsncServ REQUIRED)

set(CMAKE_USE_RELATIVE_PATHS  true )



set (SRC_LIST 
 	Csprite.cpp
	Cgame.cpp
	version.cpp
	Cmap_conf.cpp
	Citem_conf.cpp
	Cactivity.cpp
 	cli_proto.cpp
	online.cpp
	dbproxy.cpp
	switch.cpp
	timer_func.cpp
	load_data.cpp
	svr_proto.cpp
	./proto/pop.cpp
	./proto/pop_db.cpp
	./proto/pop_online.cpp
	./proto/pop_switch.cpp
	./proto/main.cpp
	./proto/main_login.cpp
	)
#set (SRC_LIST 
	#online.cpp
	#cli_proto.cpp
	#svr_proto.cpp
	#version.cpp

	#./proto/pop.cpp
	#./proto/pop_db.cpp
	#./proto/pop_online.cpp
#)

set (DATA_SRC_LIST 
	global.cpp
	)



INCLUDE_DIRECTORIES(   ${GLIB_INCLUDE_DIR}   ${LIBXML2_INCLUDE_DIR}  	)
ADD_LIBRARY(online SHARED ${SRC_LIST}	)
ADD_LIBRARY(online_data SHARED ${DATA_SRC_LIST}	)
TARGET_LINK_LIBRARIES ( online  "taomee"   "taomee++" "taomee"   "xml2"  )
#ADD_DEFINITIONS( "-g -Wall -DNDEBUG -fPIC -MMD -DENABLE_TRACE_LOG " )
ADD_DEFINITIONS( "-g -Wall -DNDEBUG -DUSE_TLOG -fPIC -MMD -DENABLE_TRACE_LOG  -DUSE_HEX_LOG_PROTO" )
#ADD_DEFINITIONS( "-g -Wall -DNDEBUG -fPIC -MMD " )


