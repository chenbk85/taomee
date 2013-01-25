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

set (SRC_LIST 

util.cpp
Cuser2.cpp
Cactivation.cpp
Chero_cup.cpp
Csurvey.cpp
Cval.cpp
Crank.cpp
Crelation.cpp
Cargs.cpp
Cattire.cpp
Ccli_buff.cpp
Ccommon.cpp
Cday.cpp
Chome.cpp
Cinvited.cpp
Citem.cpp
Citem_handbook.cpp
Cmail.cpp
Cmax.cpp
Cmonster_handbook.cpp
Cpet.cpp
Cpet_skill.cpp
Cpet_fight.cpp
Cpresent.cpp
Crand_item.cpp
Croute_func.cpp
Cskill.cpp
Ctask.cpp
Ctask_handbook.cpp
Cuser_battle.cpp
Cuser.cpp
Cuser_map.cpp
Cuser_note.cpp
Cuser_title.cpp
Cvip_buff.cpp
Cvip_item_log.cpp
./proto/mole2_db.cpp
./proto/mole2.cpp
service.cpp
	)


INCLUDE_DIRECTORIES( "./proto" ${MYSQL_INCLUDE_DIR} ${DBSER_INCLUDE_DIR})
ADD_LIBRARY(pubser SHARED ${SRC_LIST}	)
TARGET_LINK_LIBRARIES ( pubser "dbser" "mysqlclient" "taomee++" "taomee"  )
ADD_DEFINITIONS( "-g  -DDEBUG -Wall  -MMD  -funroll-loops -D__USE_STRING_INLINES  -fPIC -DUSE_TLOG  " )
INSTALL (TARGETS pubser DESTINATION ../bin)

