FIND_PATH( TAOMEE_INCLUDE_DIR libtaomee/log.h
	DOC "The directory where taomee.h resides"  )
 
FIND_LIBRARY( TAOMEE_LIBRARY
	NAMES taomee 
	PATHS /usr/lib/
	DOC "The TAOMEE library")

IF (TAOMEE_INCLUDE_DIR )
	SET( TAOMEE_FOUND 1 CACHE STRING "Set to 1 if taomee is found, 0 otherwise")
ELSE (TAOMEE_INCLUDE_DIR)
	SET( TAOMEE_FOUND 0 CACHE STRING "Set to 1 if taomee is found, 0 otherwise")
ENDIF (TAOMEE_INCLUDE_DIR)

MARK_AS_ADVANCED( TAOMEE_FOUND )

IF(TAOMEE_FOUND)
	MESSAGE(STATUS "找到了 taomee 库")
ELSE(TAOMEE_FOUND)
	MESSAGE(FATAL_ERROR "没有找到 taomee库 :请安装它, 然后删除build目录:rm -rf ./build/* ,重新 安装 ")
ENDIF(TAOMEE_FOUND)


