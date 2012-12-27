FIND_PATH( LIBXML2_INCLUDE_DIR libxml/tree.h
	/usr/include/libxml2/
	DOC "The directory where glib.h resides")
MESSAGE(STATUS "Looking for libxml2 - found:${LIBXML2_INCLUDE_DIR}")
#/usr/include/glib-2.0/


FIND_LIBRARY( LIBXML2_LIBRARY
	NAMES xml2 
	PATHS
	/usr/lib/
	DOC "The xml2 library")

IF (LIBXML2_INCLUDE_DIR )
	SET( LIBXML2_FOUND 1 CACHE STRING "Set to 1 if Foo is found, 0 otherwise")
ELSE (LIBXML2_INCLUDE_DIR )
	SET( LIBXML2_FOUND 0 CACHE STRING "Set to 1 if Foo is found, 0 otherwise")
ENDIF (LIBXML2_INCLUDE_DIR )

MARK_AS_ADVANCED( LIBXML2_FOUND )

IF(LIBXML2_FOUND)
	MESSAGE(STATUS "Looking for libxml2 - found")
ELSE(LIBXML2_FOUND)
	MESSAGE(FATAL_ERROR "Looking for  libxml2 - not found 运行:apt-get install libxml2-dev  ")
ENDIF(LIBXML2_FOUND)


