PROJECT(SWSER)
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
	dbproxy.cpp
	dispatcher.cpp
	online.cpp
	switch.cpp
	./proto/mole2_switch.cpp
)

link_directories(${CMAKE_SOURCE_DIR})

INCLUDE_DIRECTORIES(   ${GLIB_INCLUDE_DIR}   ${LIBXML2_INCLUDE_DIR} "./include")
ADD_LIBRARY(Mole2Switch SHARED ${SRC_LIST}	)
TARGET_LINK_LIBRARIES ( Mole2Switch "taomee" "dl"  "taomee++" "xml2" "leveldb" "pthread" )
ADD_DEFINITIONS( "-Wall -DNDEBUG -g -fPIC -MMD -Wno-pmf-conversions -lpthread" )

