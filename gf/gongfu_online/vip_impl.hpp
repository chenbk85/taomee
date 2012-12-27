#ifndef VIP_IMPL_H
#define VIP_IMPL_H


#include <stdint.h>
#include <map>
#include <kf/vip_config_data_mgr.hpp>

struct player_t;
struct get_player_rsp_t;

/**
  * @brief command id for vipsvr
  */
enum vip_cmd_t {
	/* for mb */
	vip_proto_swap_vip_qualify		= 0x9701,
};


#pragma pack(1)
/**
  * @brief vipsvr protocol type
  */
struct vip_proto_t {
	/*! package length */
	uint32_t	len;
	/*! sequence number ((p->fd << 16) | p->waitcmd) */
	uint32_t	seq;
	/*! command id */
	uint16_t	cmd;
	/*! errno */
	uint32_t	ret;
	/*! user id */
	userid_t	id;
	/*! package body */
	uint8_t		body[];
};

#pragma pack()

/////////////////////////////////////////////////////////
//
int send_request_to_vipsvr_2(player_t* p, userid_t id, uint16_t cmd, const void* body, uint32_t body_len);


void handle_vipsvr_return(vip_proto_t* pkg, uint32_t pkglen);

/**
 * @brief swap vip qualify 
 */
int do_swap_vip_qualify(player_t* p, userid_t id, uint32_t day);

/**
 * @brief swap vip qualify  callback
 */
int vipsvr_swap_vip_qualify_callback(player_t* p, void* body, uint32_t bodylen, uint32_t ret);

//
bool 	  init_player_vip(player_t* p);
bool 	  final_player_vip(player_t* p);

uint32_t  add_player_vip_level(player_t* p, uint32_t level  = 1);
uint32_t  get_player_vip_point(player_t* p);
uint32_t  get_player_vip_level(player_t* p);

uint32_t  get_player_extern_warehouse_gird_count(player_t* p);
uint32_t  get_player_extern_item_bag_grid_count(player_t* p);
uint32_t  get_player_total_item_bag_grid_count(player_t* p);

bool      send_player_vip_info(player_t* p);

void      init_player_vip_info(player_t *p ,  get_player_rsp_t* rsp);
void      set_player_vip_info(player_t *p, uint32_t is_vip, uint32_t month_cnt, uint32_t vip_begin_time,  uint32_t vip_end_time);
void  	  add_player_vip_point(player_t *p,  uint32_t point);
void      set_player_vip_point(player_t *p,  uint32_t point);
void      calc_player_vip_info(player_t *p,  bool notify_client = true);


int db_set_extern_grid_count_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);











#endif
