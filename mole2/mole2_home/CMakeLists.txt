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
	dispatcher.cpp
	market.cpp
	global.cpp
	home.cpp
	mole2_home.cpp
	proto.cpp
	sprite.cpp
	pvp.cpp
	trade.cpp
	./proto/mole2_home.cpp
	./proto/mole2.cpp
)




INCLUDE_DIRECTORIES(   ${GLIB_INCLUDE_DIR}   ${LIBXML2_INCLUDE_DIR}  	)
ADD_LIBRARY(m2home SHARED ${SRC_LIST}	)
TARGET_LINK_LIBRARIES ( m2home   "xml2" "dl" "m" "taomee"  "taomee++"  )
ADD_DEFINITIONS( "-g -Wall -DNDEBUG -fPIC -MMD -DENABLE_TRACE_LOG  -DUSE_HEX_LOG_PROTO " )


