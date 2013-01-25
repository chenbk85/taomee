PROJECT(DBSER)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_COMPILER "g++")

if  ( DEFINED MK_INSTALL ) 
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../bin ) 
MESSAGE( STATUS "安装到运行目录")
endif   ( DEFINED MK_INSTALL ) 


find_package(taomee REQUIRED)
find_package(taomee++ REQUIRED)
find_package(glib REQUIRED)

find_package(libxml2 REQUIRED)
find_package(AsncServ REQUIRED)

set(CMAKE_USE_RELATIVE_PATHS  true )



set (SRC_LIST 
	viponly.cpp
	map.cpp
	svr_proto.cpp
	homemap.cpp
	activity.cpp
	box.cpp
	beast.cpp
	mole2_online.cpp
	cli_proto.cpp
	dbproxy.cpp
	mc_proto.cpp
	sprite.cpp
	pet.cpp
	cli_login.cpp
	battle.cpp
	center.cpp
	event.cpp
	sns.cpp
	items.cpp
	util.cpp
	skill.cpp
	honor.cpp
	home.cpp
	picsrv.cpp
	mail.cpp
	task_new.cpp
	maze.cpp
	npc.cpp
	code.cpp
	version.cpp
	mall.cpp
	buff.cpp
	./proto/mole2_online.cpp
	./proto/mole2_home.cpp
	./proto/mole2.cpp
	./proto/mole2_db.cpp
	./proto/mole2_switch.cpp
	./proto/bus_server.cpp
)

set (DATA_SRC_LIST 
	global.cpp
	)



INCLUDE_DIRECTORIES(   ${GLIB_INCLUDE_DIR}   ${LIBXML2_INCLUDE_DIR}  	)
ADD_LIBRARY(m2online SHARED ${SRC_LIST}	)
ADD_LIBRARY(m2data SHARED ${DATA_SRC_LIST}	)
TARGET_LINK_LIBRARIES ( m2online   "taomee"  "taomee++" "xml2"  )
ADD_DEFINITIONS( "-g -Wall -DNDEBUG -fPIC -MMD -DUSE_TLOG -DENABLE_TRACE_LOG  -DUSE_HEX_LOG_PROTO " )


