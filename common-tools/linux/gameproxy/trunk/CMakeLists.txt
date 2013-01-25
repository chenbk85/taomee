CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT ( extend_port )
if ("${CMAKE_BUILD_TYPE}" STREQUAL "debug" OR  "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	add_definitions(-D_DEBUG)
	add_definitions(-Ddebug)
	add_definitions(-DDebug)
	set(FLAGS   " -Wall -g -fPIC -MMD -D_REENTRANT")
else()
	add_definitions(-DNDEBUG)
	set(FLAGS   " -Wall -g -fPIC -MMD -D_REENTRANT")
endif()


set(ExtendSRCS dll_interface.cpp
				client.cpp
				clientsmgr.cpp
				memorypool.cpp
				)
set(DATA_SRCS global_data.cpp)

source_group("Src" FILES ${DATA_SRCS} ${ExtendSRCS})	

include_directories("/home/albert/async_serv/include")
include_directories(".")
include_directories("/usr/include/glib-2.0")
include_directories("/usr/lib/glib-2.0/include/")

SET( CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}")

ADD_LIBRARY(${PROJECT_NAME} MODULE ${ExtendSRCS} )
ADD_LIBRARY(data_extend MODULE ${DATA_SRCS} )
set_target_properties( ${PROJECT_NAME}
		PROPERTIES 
		COMPILE_FLAGS ${FLAGS}
    )
set_target_properties( data_extend
		PROPERTIES 
		COMPILE_FLAGS ${FLAGS}
    )
INSTALL(TARGETS ${PROJECT_NAME}
		RUNTIME DESTINATION bin        
		LIBRARY DESTINATION bin
		ARCHIVE DESTINATION bin
		)
INSTALL(TARGETS data_extend
		RUNTIME DESTINATION bin        
		LIBRARY DESTINATION bin
		ARCHIVE DESTINATION bin
		)
LINK_DIRECTORIES("/usr/lib")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} taomee)

message(STATUS "PROJECT_BINARY_DIR=${PROJECT_BINARY_DIR}")
message(STATUS "PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}")
message(STATUS "CMAKE_SHARED_LIBRARY_SUFFIX=${CMAKE_SHARED_LIBRARY_SUFFIX}")
message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_C_FLAGS=${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_C_COMPILER=${CMAKE_C_COMPILER}")


