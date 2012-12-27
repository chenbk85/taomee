FIND_PATH( ASYNSERV_INCLUDE_DIR async_serv/net_if.h
	DOC "The directory where aync_serv.h resides"  )
 

IF (ASYNSERV_INCLUDE_DIR )
	SET( ASYNSERV_FOUND 1 CACHE STRING "Set to 1 if aync_serv is found, 0 otherwise")
ELSE (ASYNSERV_INCLUDE_DIR)
	SET( ASYNSERV_FOUND 0 CACHE STRING "Set to 1 if aync_serv is found, 0 otherwise")
ENDIF (ASYNSERV_INCLUDE_DIR)

MARK_AS_ADVANCED( ASYNSERV_FOUND )

IF(ASYNSERV_FOUND)
	MESSAGE(STATUS "Looking for aync_serv - found")
ELSE(ASYNSERV_FOUND)
	MESSAGE(FATAL_ERROR "Looking for aync_serv - not found : async_serv/net_if.h ")
ENDIF(ASYNSERV_FOUND)


