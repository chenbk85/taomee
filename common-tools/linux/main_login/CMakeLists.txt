cmake_minimum_required(VERSION 2.6)
set(CMAKE_INSTALL_PREFIX "/usr/")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_USE_RELATIVE_PATHS true)

set(SRC_LIST
	common.cpp
	user_manager.cpp
	)
set(HEADER_LIST
	common.hpp
	user_manager.hpp
	)
set(COPY_HEADER_LIST
	common.hpp
	user_manager.hpp
	cli_proto.hpp
	login_impl.hpp
	main_login.hpp
	)

include_directories("./" "../../../" "/usr/include/glib-2.0" "/usr/lib/glib-2.0/include")
add_definitions(" -Wall -MMD -g -DDEBUG -funroll-loops -fPIC -D__USE_STRING_INLINES -fPIC ")
add_library(mainlogin ${SRC_LIST})
target_link_libraries(mainlogin taomee taomee++)
install(TARGETS mainlogin DESTINATION local/lib)
install(FILES ${COPY_HEADER_LIST} DESTINATION local/include/main_login)
