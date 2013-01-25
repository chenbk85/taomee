FIND_PATH( MYSQL_INCLUDE_DIR mysql.h
	/usr/include/mysql/
	DOC "The directory where mysql resides")
MESSAGE(STATUS "Looking for mysql - found:${MYSQL_INCLUDE_PATH}")

FIND_LIBRARY( MYSQL_LIBRARY
	NAMES mysqlclient
	PATHS
	/usr/lib/
	DOC "The MYSQL library")

IF (MYSQL_INCLUDE_DIR )
	SET( MYSQL_FOUND 1 CACHE STRING "Set to 1 if Foo is found, 0 otherwise")
ELSE (MYSQL_INCLUDE_DIR )
	SET( MYSQL_FOUND 0 CACHE STRING "Set to 1 if Foo is found, 0 otherwise")
ENDIF (MYSQL_INCLUDE_DIR )

MARK_AS_ADVANCED( MYSQL_FOUND )

IF(MYSQL_FOUND)
	MESSAGE(STATUS "找到了 mysql 库")
ELSE(MYSQL_FOUND)
	MESSAGE(FATAL_ERROR "没有找到mysql :请安装：sudo apt-get install libmysqlclient16-dev ,然后删除build目录:rm -rf ./build/* ,重新 安装 ")
ENDIF(MYSQL_FOUND)

