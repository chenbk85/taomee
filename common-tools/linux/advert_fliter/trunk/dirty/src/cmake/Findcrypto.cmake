FIND_PATH( CRYPTO_INCLUDE_DIR md5.h
	/usr/include/openssl/
	DOC "The directory where md5.h resides")
MESSAGE(STATUS "Looking for crypto - found:${CRYPTO_INCLUDE_PATH}")

set (CRYPTO_INCLUDE_DIR ${CRYPTO_INCLUDE_DIR} )

FIND_LIBRARY( CRYPTO_LIBRARY
	NAMES crypto 
	PATHS
	/usr/lib/
	DOC "The CRYPTO library")

IF (CRYPTO_INCLUDE_DIR )
	SET( CRYPTO_FOUND 1 CACHE STRING "Set to 1 if Foo is found, 0 otherwise")
ELSE (CRYPTO_INCLUDE_DIR )
	SET( CRYPTO_FOUND 0 CACHE STRING "Set to 1 if Foo is found, 0 otherwise")
ENDIF (CRYPTO_INCLUDE_DIR )

MARK_AS_ADVANCED( CRYPTO_FOUND )

IF(CRYPTO_FOUND)
	MESSAGE(STATUS "找到了 crypto 库")
ELSE(CRYPTO_FOUND)
	MESSAGE(FATAL_ERROR "没有找到 crypto :请安装：sudo apt-get install  libcrypto2.0-dev ,然后重新 安装 ")
ENDIF(CRYPTO_FOUND)

