PROJECT(DBSER)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_INSTALL_PREFIX "../")
set(CMAKE_CXX_COMPILER "g++")
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../ ) 

set(CMAKE_USE_RELATIVE_PATHS  true )


find_package(glib REQUIRED)
find_package(mysql REQUIRED)
find_package(taomee REQUIRED)
find_package(taomee++ REQUIRED)
find_package(dbser REQUIRED)
find_package(libxml2 REQUIRED)


set (SRC_LIST 
	task.cpp
	Croute_func.cpp
	extra_info.cpp
	friends.cpp
	item.cpp
	mail.cpp
	pet.cpp
	pet_gift.cpp
	service.cpp
	user.cpp
	./proto/pea.cpp
	./proto/pea_db.cpp
)


INCLUDE_DIRECTORIES( ${GLIB_INCLUDE_DIR}    "./proto" ${MYSQL_INCLUDE_DIR} ${DBSER_INCLUDE_DIR} "../../pea_common/trunk"  ${LIBXML2_INCLUDE_DIR}  ) 
ADD_LIBRARY(ser SHARED ${SRC_LIST}	)
TARGET_LINK_LIBRARIES ( ser "dbser" "mysqlclient"  "taomee++"  "-L../../../pea_common/trunk" "pea_common" "taomee" )
ADD_DEFINITIONS( "-DGF_PROTO_HEADER  -DCREATE_ROLE_WITH_DATA -Wall -pipe -D_NEW_LIC -g -D_GNU_SOURCE -shared -D_REENTRANT -D__STDC_FORMAT_MACROS -fPIC " )

