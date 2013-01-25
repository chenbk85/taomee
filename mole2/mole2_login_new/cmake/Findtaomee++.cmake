FIND_PATH( TAOMEEXX_INCLUDE_DIR libtaomee++/time/time.hpp
	DOC "The directory where time.hpp resides"  )
 
FIND_LIBRARY( TAOMEEXX_LIBRARY
	NAMES taomee++ 
	PATHS /usr/lib/
	DOC "The TAOMEE library")

IF (TAOMEEXX_INCLUDE_DIR )
	SET( TAOMEEXX_FOUND 1 CACHE STRING "Set to 1 if taomee is found, 0 otherwise")
ELSE (TAOMEEXX_INCLUDE_DIR)
	SET( TAOMEEXX_FOUND 0 CACHE STRING "Set to 1 if taomee is found, 0 otherwise")
ENDIF (TAOMEEXX_INCLUDE_DIR)

MARK_AS_ADVANCED( TAOMEEXX_FOUND )

IF(TAOMEEXX_FOUND)
	MESSAGE(STATUS "找到 taomee++  库")
ELSE(TAOMEEXX_FOUND)
	MESSAGE(FATAL_ERROR " taomee++ 没有安装 :没有找到libtaomee++/time/time.hpp文件,安装后删除build 重新开始 ")
ENDIF(TAOMEEXX_FOUND)


