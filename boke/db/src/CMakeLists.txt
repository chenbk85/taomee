PROJECT(DBSER)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_INSTALL_PREFIX "../")
set(CMAKE_CXX_COMPILER "g++")

set(CMAKE_USE_RELATIVE_PATHS  true )


find_package(glib REQUIRED)
find_package(mysql REQUIRED)
find_package(taomee REQUIRED)
find_package(taomee++ REQUIRED)
find_package(dbser REQUIRED)

set (SRC_LIST 
	Croute_func.cpp
	Cuser_log.cpp
	Citem.cpp
	Ctask.cpp
	Cfind_map.cpp
	service.cpp
	Cuser.cpp
	Cday.cpp
	Cgame.cpp
	./proto/pop.cpp
	./proto/pop_db.cpp
	)


INCLUDE_DIRECTORIES( "./proto" ${MYSQL_INCLUDE_DIR} ${DBSER_INCLUDE_DIR})
ADD_LIBRARY(db SHARED ${SRC_LIST}	)
TARGET_LINK_LIBRARIES (db "dbser" "mysqlclient" "taomee++" "taomee"  )
ADD_DEFINITIONS( "-g  -DDB_GET_LIST_NEW  -DDEBUG -Wall  -MMD  -funroll-loops -D__USE_STRING_INLINES  -fPIC " )

