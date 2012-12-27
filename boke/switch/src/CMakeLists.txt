PROJECT(DBSER)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_COMPILER "g++")

#if  ( DEFINED MK_INSTALL ) 
#SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../ ) 
#MESSAGE( STATUS "安装到运行目录")
#endif   ( DEFINED MK_INSTALL ) 


find_package(taomee REQUIRED)
find_package(taomee++ REQUIRED)
find_package(glib REQUIRED)

find_package(AsncServ REQUIRED)

set(CMAKE_USE_RELATIVE_PATHS  true )



set (SRC_LIST 
dbproxy.cpp
dispatcher.cpp
online.cpp
switch.cpp
./proto/pop.cpp
./proto/pop_switch.cpp
./proto/pop_db.cpp
	)


INCLUDE_DIRECTORIES(   ${GLIB_INCLUDE_DIR}    	)
ADD_LIBRARY(switch SHARED ${SRC_LIST}	)
TARGET_LINK_LIBRARIES (switch "taomee" "taomee++" )
ADD_DEFINITIONS( " -g -DDEBUG -DUSE_TLOG -Wall  -MMD  -funroll-loops -D__USE_STRING_INLINES  -fPIC" )


