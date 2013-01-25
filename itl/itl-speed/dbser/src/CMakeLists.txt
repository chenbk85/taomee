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
	service.cpp
	Ccdn_rate_ip.cpp
	Ccdn_rate_prov.cpp
	Ccdn_rate_detail.cpp
	Cnet_stat_ip.cpp
	Cnet_stat_prov.cpp
	Clink_stat.cpp
	Curl_stat_prov.cpp
	Curl_stat_user.cpp
    Cdb_cdn_info.cpp
	./proto/cdn.cpp
	./proto/cdn_rate.cpp
	)


INCLUDE_DIRECTORIES( "./proto" ${MYSQL_INCLUDE_DIR} ${DBSER_INCLUDE_DIR})
ADD_LIBRARY(db SHARED ${SRC_LIST}	)
TARGET_LINK_LIBRARIES (db "dbser" "mysqlclient" "taomee++" "taomee" )
ADD_DEFINITIONS( "-g  -DDB_GET_LIST_NEW  -DDEBUG -Wall  -MMD  -funroll-loops -D__USE_STRING_INLINES  -fPIC " )

