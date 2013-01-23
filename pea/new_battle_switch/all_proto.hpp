#ifndef __ALL_PROTO_HPP__
#define __ALL_PROTO_HPP__


#include "common.hpp"
#include "cli_proto.hpp"



void pack_pvp_room_list_info(room_t *room, room_list_info_t *room_info);
uint8_t get_room_info_turn_lower(room_map_t *room_map,
		uint32_t bound_roomid, uint8_t nr, btlsw_pvp_room_list_out *p_out);
uint8_t get_room_info_turn_upper(room_map_t *room_map,
		uint32_t bound_roomid, uint8_t nr, btlsw_pvp_room_list_out *p_out);




#endif // __ALL_PROTO_HPP__
