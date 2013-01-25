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
	Cwarrior_state.cpp
	bt_online.cpp
	batrserv.cpp
	beast_ai_api.cpp
	bt_bat_check.cpp
	bt_beastai.cpp
	bt_dispatcher.cpp
	bt_pet_skill.cpp
	bt_skill.cpp
	version.cpp
	global_not_reload.cpp
	./proto/mole2_btl.cpp
	./proto/mole2.cpp
	)

set (DATA_SRC_LIST 
	global.cpp
	)



INCLUDE_DIRECTORIES(   ${GLIB_INCLUDE_DIR}   ${LIBXML2_INCLUDE_DIR}  	)
ADD_LIBRARY(batrserv SHARED ${SRC_LIST}	)
ADD_LIBRARY(batrserv_data SHARED ${DATA_SRC_LIST}	)
TARGET_LINK_LIBRARIES ( batrserv "taomee" "taomee++" "xml2" "lua" )
ADD_DEFINITIONS( "-DUSE_HEX_LOG_PROTO  -g -DDEBUG -Wall  -MMD  -funroll-loops -D__USE_STRING_INLINES  -fPIC " )


