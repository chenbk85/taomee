FIND_PATH( GLIB_INCLUDE_DIR glib.h
	/usr/include/glib-2.0/
	DOC "The directory where glib.h resides")
MESSAGE(STATUS "Looking for glib - found:${GLIB_INCLUDE_PATH}")
#/usr/include/glib-2.0/
FIND_PATH( GLIB_CONFIG_INCLUDE_PATH glibconfig.h
	/usr/lib/glib-2.0/include/
	DOC "The directory where glib.h resides")
MESSAGE(STATUS "Looking for glib - found:${GLIB_CONFIG_INCLUDE_PATH}")

set (GLIB_INCLUDE_DIR ${GLIB_INCLUDE_DIR} ${GLIB_CONFIG_INCLUDE_PATH}	)

FIND_LIBRARY( GLIB_LIBRARY
	NAMES glib 
	PATHS
	/usr/lib/
	DOC "The GLIB library")

IF (GLIB_INCLUDE_DIR )
	SET( GLIB_FOUND 1 CACHE STRING "Set to 1 if Foo is found, 0 otherwise")
ELSE (GLIB_INCLUDE_DIR )
	SET( GLIB_FOUND 0 CACHE STRING "Set to 1 if Foo is found, 0 otherwise")
ENDIF (GLIB_INCLUDE_DIR )

MARK_AS_ADVANCED( GLIB_FOUND )

IF(GLIB_FOUND)
	MESSAGE(STATUS "找到了 glib 库")
ELSE(GLIB_FOUND)
	MESSAGE(FATAL_ERROR "没有找到 glib :请安装：sudo apt-get install  apt-get install libglib2.0-dev ,然后删除build目录:rm -rf ./build/* ,重新 安装 ")
ENDIF(GLIB_FOUND)

