# Install script for directory: /home/smyang/workspace/dbser

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/smyang/workspace/dbser/libdbser.a")
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/dbser" TYPE FILE FILES
    "/home/smyang/workspace/dbser/benchapi.h"
    "/home/smyang/workspace/dbser/Cbig_cache.h"
    "/home/smyang/workspace/dbser/Ccmdmaplist.h"
    "/home/smyang/workspace/dbser/Ccmdmap_private_checklen.h"
    "/home/smyang/workspace/dbser/Cfunc_route_base.h"
    "/home/smyang/workspace/dbser/Cfunc_route_cmd.h"
    "/home/smyang/workspace/dbser/Cfunc_route_cmd_v2.h"
    "/home/smyang/workspace/dbser/Cfunc_route_db_v2.h"
    "/home/smyang/workspace/dbser/Csync_user_data.h"
    "/home/smyang/workspace/dbser/CtableDate_100.h"
    "/home/smyang/workspace/dbser/CtableDate.h"
    "/home/smyang/workspace/dbser/Ctable.h"
    "/home/smyang/workspace/dbser/CtableMonth.h"
    "/home/smyang/workspace/dbser/CtableRoute100.h"
    "/home/smyang/workspace/dbser/CtableRoute100x10.h"
    "/home/smyang/workspace/dbser/CtableRoute100x1.h"
    "/home/smyang/workspace/dbser/CtableRoute10.h"
    "/home/smyang/workspace/dbser/CtableRoute10x10.h"
    "/home/smyang/workspace/dbser/CtableRoute.h"
    "/home/smyang/workspace/dbser/CtableString.h"
    "/home/smyang/workspace/dbser/CtableWithKey.h"
    "/home/smyang/workspace/dbser/Citem_change_log.h"
    "/home/smyang/workspace/dbser/db_error_base.h"
    "/home/smyang/workspace/dbser/db_macro.h"
    "/home/smyang/workspace/dbser/mysql_iface.h"
    "/home/smyang/workspace/dbser/proto_header.h"
    "/home/smyang/workspace/dbser/proxy_dll_interface.h"
    "/home/smyang/workspace/dbser/proxy_route.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
ELSE(CMAKE_INSTALL_COMPONENT)
  SET(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
ENDIF(CMAKE_INSTALL_COMPONENT)

FILE(WRITE "/home/smyang/workspace/dbser/${CMAKE_INSTALL_MANIFEST}" "")
FOREACH(file ${CMAKE_INSTALL_MANIFEST_FILES})
  FILE(APPEND "/home/smyang/workspace/dbser/${CMAKE_INSTALL_MANIFEST}" "${file}\n")
ENDFOREACH(file)
