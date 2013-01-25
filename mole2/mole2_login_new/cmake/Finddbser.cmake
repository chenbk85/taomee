FIND_PATH( DBSER_INCLUDE_DIR   mysql_iface.h
		/usr/include/dbser/
	DOC "The directory where  .h resides"  )
 
FIND_LIBRARY( DBSER_LIBRARY
	NAMES dbser 
	PATHS /usr/lib/
	DOC "The DBSER library")

IF (DBSER_INCLUDE_DIR )
	SET( DBSER_FOUND 1 CACHE STRING "Set to 1 if dbser is found, 0 otherwise")
ELSE (DBSER_INCLUDE_DIR)
	SET( DBSER_FOUND 0 CACHE STRING "Set to 1 if dbser is found, 0 otherwise")
ENDIF (DBSER_INCLUDE_DIR)

MARK_AS_ADVANCED( DBSER_FOUND )

IF(DBSER_FOUND)
	MESSAGE(STATUS "找到了 dbser 库")
ELSE(DBSER_FOUND)
	MESSAGE(FATAL_ERROR "没有找到 dbser库 :请安装它, 然后删除build目录:rm -rf ./build/* ,重新 安装 ")
ENDIF(DBSER_FOUND)


