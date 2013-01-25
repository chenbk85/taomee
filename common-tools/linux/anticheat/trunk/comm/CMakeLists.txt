CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
FIND_PACKAGE(glib REQUIRED)

SET(CMAKE_USE_RELATIVE_PATHS true)

SET (SRC_LIST 
	channel.c
	shm.c
	util.c
	hash_table.c
	mysql_wrapper.c
	log.c
	svc.c
	daemon.c
	)
SET (HEADER_LIST 
	common.h
	struct.h
	shm_keys.h
	channel.h
	shm.h
	util.h
	hash_table.h
	mysql_wrapper.h
	log.h
	svc.h
	daemon.h
   	)

INCLUDE_DIRECTORIES( ${GLIB_INCLUDE_DIR} )
ADD_DEFINITIONS("-g -Wall -MMD -funroll-loops -fhosted  -fPIC")
ADD_LIBRARY(comm ${SRC_LIST})


# for install
#SET(CMAKE_INSTALL_PREFIX "./libcomm")
#INSTALL (TARGETS lib DESTINATION lib)
#FOREACH (HEADER_FILE ${HEADER_LIST})
# 	STRING( REGEX REPLACE "\\.(.*)/.*\\.h$" "\\1" DIR_NAME ${HEADER_FILE} ) 
#	INSTALL (FILES ${HEADER_FILE} DESTINATION  include/libtaomee/${DIR_NAME}	)	
#ENDFOREACH(HEADER_FILE)
