#ifndef KF_USER_FEEDBACK_HPP_
#define KF_USER_FEEDBACK_HPP_

extern "C" {
#include <libtaomee/project/types.h>
}

#include "player.hpp"


/* title of msg player submits */
const uint32_t c_msg_title_len = 60;

/* max content length of msg player submits */
const uint32_t c_msg_max_len = 3600;


#pragma pack(1)

struct submit_msg_t {
	uint32_t	type;	//1: a message to master wugui 2:...
	uint32_t	msglen;
	uint8_t 	title[c_msg_title_len];
	uint8_t 	msg[];
};

struct db_submit_msg_t {
	uint32_t gameid;//1: mole  2: pp 3:dawanguo 4:dudu 5: hua 6: gongfupai
	uint32_t flag;	// 0: feedback  1: vote
	uint32_t type;	//1: a message to master wugui 2:...
	char	 nick[16];
	char 	 title[c_msg_title_len];
	uint32_t msglen;
	char	 msg[];
};

struct appellee_msg_t{
    uint32_t    id;
    uint32_t    role_tm;
    uint8_t     nick[16];
    uint32_t    rule_id;
};
#pragma pack()

/**
  * @brief submit message to DB
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int submit_msg_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

inline int db_submit_msg(player_t* p, submit_msg_t* precv);

/**
  * @brief user report message to DB
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int report_msg_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

#endif

