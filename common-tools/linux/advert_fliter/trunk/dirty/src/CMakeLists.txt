PROJECT(DBSER)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_COMPILER "g++")


find_package(taomee REQUIRED)
find_package(mmseg REQUIRED)
find_package(taomee++ REQUIRED)
find_package(glib REQUIRED)
find_package(crypto REQUIRED)
find_package(dbser REQUIRED)
find_package(mysql REQUIRED)

find_package(libxml2 REQUIRED)
find_package(AsncServ REQUIRED)

set(CMAKE_USE_RELATIVE_PATHS  true )



set (SRC_LIST 
	dispatcher.cpp
	dll.cpp
	dirty_word.cpp
	global.cpp
	timer_func.cpp
	)

INCLUDE_DIRECTORIES(   ${GLIB_INCLUDE_DIR} ${LIBXML2_INCLUDE_DIR} ${MYSQL_INCLUDE_DIR} ${MMSEG_INCLUDE_DIR} bayes	)
ADD_LIBRARY(dirty SHARED ${SRC_LIST}	)
TARGET_LINK_LIBRARIES (dirty "dbser" "mysqlclient"  "taomee++" "taomee"   "xml2" "crypto" "mmseg" )
ADD_DEFINITIONS( "-g -Wall -DNDEBUG -fPIC -MMD -DENABLE_TRACE_LOG  -DUSE_HEX_LOG_PROTO" )


