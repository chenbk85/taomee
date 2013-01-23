PROJECT(DBSER)
cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_CXX_COMPILER "g++")


find_package(taomee REQUIRED)
find_package(taomee++ REQUIRED)
find_package(glib REQUIRED)

find_package(libxml2 REQUIRED)
find_package(AsncServ REQUIRED)

set(CMAKE_USE_RELATIVE_PATHS  true )



set (SRC_LIST 
		battle.cpp
		battle_switch.cpp
		chat.cpp
		cli_proto.cpp
		db_friends.cpp
		db_item.cpp
		db_mail.cpp
		db_pet.cpp
		db_player.cpp
		dbproxy.cpp
		db_task.cpp
		dll_interface.cpp
		friends.cpp
		fwd_decl.cpp
		item_bag.cpp
		item.cpp
		login.cpp
		mail.cpp
		map.cpp
		map_data.cpp
		npc_shop.cpp
		online.cpp
		pet.cpp
        prize.cpp
		player_attribute.cpp
		player.cpp
		player_equip.cpp
		player_extra_info.cpp
		proto.cpp
		./proto/pea_btlsw.cpp
		./proto/pea.cpp
		./proto/pea_db.cpp
		./proto/pea_online.cpp
		room.cpp
		session.cpp
		task.cpp
		task_funcs.cpp
	)
#set (SRC_LIST 
	#online.cpp
	#cli_proto.cpp
	#svr_proto.cpp
	#version.cpp

	#./proto/pop.cpp
	#./proto/pop_db.cpp
	#./proto/pop_online.cpp
#)

#set (DATA_SRC_LIST 
#	global.cpp
#	)



INCLUDE_DIRECTORIES(   ${GLIB_INCLUDE_DIR}   ${LIBXML2_INCLUDE_DIR} "../../pea_common/trunk"  	)
ADD_LIBRARY(online SHARED ${SRC_LIST}	)
#ADD_LIBRARY(online_data SHARED ${DATA_SRC_LIST}	)
TARGET_LINK_LIBRARIES ( online  "taomee"   "taomee++" "taomee"   "xml2" "-L ../../../pea_common/trunk"  pea_common  )
#ADD_DEFINITIONS( "-g -Wall -DNDEBUG -fPIC -MMD -DENABLE_TRACE_LOG " )
#ADD_DEFINITIONS( "-Wall -DDEBUG  -g -fPIC -MMD -DUSE_TLOG  -DENABLE_TRACE_LOG -DUSE_HEX_LOG_PROTO  " )
ADD_DEFINITIONS( "-Wall -DDEBUG  -g -fPIC -MMD -DUSE_TLOG  -DENABLE_TRACE_LOG" )
#ADD_DEFINITIONS( "-g -Wall -DNDEBUG -fPIC -MMD " )


