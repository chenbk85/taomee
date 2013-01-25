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
		login.cpp
		login_impl.cpp
)




INCLUDE_DIRECTORIES(   ${GLIB_INCLUDE_DIR}   ${LIBXML2_INCLUDE_DIR}  	)
ADD_LIBRARY(mole2_login SHARED ${SRC_LIST}	)
TARGET_LINK_LIBRARIES (mole2_login "mainlogin" "taomee++" "taomee"   "ssl" "rt" "gd"  )
ADD_DEFINITIONS( "-g -Wall -DNDEBUG -fPIC -MMD -DENABLE_TRACE_LOG  -DUSE_HEX_LOG_PROTO " )

