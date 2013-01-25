FIND_PATH( MMSEG_INCLUDE_DIR mmseg/csr.h
	DOC "The directory where mmseg.h resides"  )
 
FIND_LIBRARY( MMSEG_LIBRARY
	NAMES mmseg 
	PATHS /usr/local/lib/
	DOC "The MMSEG library")

IF (MMSEG_INCLUDE_DIR )
	SET( MMSEG_FOUND 1 CACHE STRING "Set to 1 if taomee is found, 0 otherwise")
ELSE (MMSEG_INCLUDE_DIR)
	SET( MMSEG_FOUND 0 CACHE STRING "Set to 1 if taomee is found, 0 otherwise")
ENDIF (MMSEG_INCLUDE_DIR)

MARK_AS_ADVANCED( MMSEG_FOUND )

IF(MMSEG_FOUND)
	MESSAGE(STATUS "找到了 mmseg 库")
ELSE(MMSEG_FOUND)
	MESSAGE(FATAL_ERROR "没有找到 mmseg库 :请安装它 ")
ENDIF(MMSEG_FOUND)


