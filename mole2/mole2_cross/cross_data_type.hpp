#ifndef __CROSS_DATA_TYPE_H__
#define __CROSS_DATA_TYPE_H__

struct online_info_t {
	uint16_t	domain_id;
	uint32_t	online_id;
	char		online_ip[16];
	in_port_t	online_port;

	fdsession_t*	fdsess;
	list_head_t     online_head; //list start point for every online server
};

struct keepalive_timer_t {
	timer_struct_t*	tmr;
	list_head_t		timer_list;
};

struct pkwait_info_t {
	userid_t		teamid;
	uint8_t			type;
	uint32_t		level;
	uint32_t		count;
	list_head_t		timer_list;
	uint16_t        online_id;
};

struct pkaccept_info_t {
	userid_t		uid;
	battle_id_t		btid;
};

#endif
