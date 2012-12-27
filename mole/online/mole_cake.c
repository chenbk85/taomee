#include "item.h"
#include <statistic_agent/msglog.h>
#include "util.h"
#include "dbproxy.h"
#include "proto.h"
#include "message.h"
#include "map.h"
#include "mole_cake.h"
#include <libtaomee/list.h>

typedef struct del_tmp_type_item{
	uint32_t itemid;
	uint32_t count;
}del_tmp_type_t;

del_tmp_type_t tm_type_item[][3] = {
	{{1351333, 3},{1351334, 1},{0, 0}},//310
	{{1351335, 1},{1351336, 1},{0, 0}},//110
	{{1351337, 3},{0, 0},{0, 0}},//300
	{{1351338, 1},{1351339, 1},{0, 0}},//110
	{{1351340, 1},{1351341, 1},{1351342, 1}},//111
	{{1351343, 1},{1351344, 1},{0, 0}},//110
};
uint32_t figure_bit[][3] = {
	{3, 1, 0},
	{1, 1, 0},
	{3, 0, 0},
	{1, 1, 0},
	{1, 1, 1},
	{1, 1, 0},
};

uint32_t datetype[] = {
	2100000069,
	2100000070,
	2100000071,
	2100000072,
	2100000073,
	2100000074,
	/////
	2100000075,
};

uint32_t del_itemid_count[] = {
	200,
	200,
	180,
	100,
	100,
	200,
	////
	200
};

cake_info_t birthday_cake;

static finish_cake_timer_t cake_timer;

#define FINISH_CELEBRATE_BIRHTDAY_CAKE_TIME 10*60
#define CAL_CAKE_CREAM_MELT_TIME    10

#define IS_CAKE_MAP(mapid_)	(((uint32_t)mapid_) == MAKE_BIRTHDAY_CAKE_MAP)

#define CHECK_IN_CAKE_MAP(_p)		\
	do {\
	if (!(_p)->tiles || !IS_CAKE_MAP((_p)->tiles->id)) {\
	DEBUG_LOG("user not in make cake map: uid[%u] mapid[%lu]", (_p)->id, ((_p)->tiles->id));\
	return send_to_self_error((_p), (_p)->waitcmd, -ERR_invalid_session, 1);\
	}\
	} while(0)

int check_whether_legal_operate( int material)
{
	if((birthday_cake.cur_state == FIRST_STEP  && (material == 0 || material == 4)) ||
		(birthday_cake.cur_state == SECOND_STEP && (material == 1 )) ||
		(birthday_cake.cur_state == THIRD_STEP  && (material == 3 || material == 2))){
		
		materail_flag_t item = (materail_flag_t)material;
		//DEBUG_LOG("item: %u", item);
		switch(item)
		{
		case FLOUR:
		case WATER:
			{
				if(item == FLOUR){
					if(birthday_cake.flour < 50){
						++birthday_cake.flour;
					}	
				}
				else if(item == WATER){
					if(birthday_cake.water < 50){
						++birthday_cake.water;
					}
				}
				if(birthday_cake.flour == 50 && birthday_cake.water == 50){
					birthday_cake.cur_state = SECOND_STEP;
				}
				
				break;
			}
		case ALMOND:
		case CANDLE:
			{
				if(item == ALMOND){
					if(birthday_cake.almond < 100){
						++birthday_cake.almond;
					}
				}
				else if(item == CANDLE){
					if(birthday_cake.candle < 10){
						//DEBUG_LOG("-#####################3 item: %u", item);
						++birthday_cake.candle;
					}
					
				}
				if(birthday_cake.almond == 100 && birthday_cake.candle == 10){
					birthday_cake.cur_state = FINISH_STEP;
				    add_timer();
				}
				break;
			}
		case CREAM:
			{
				if(birthday_cake.cream < 100){
					++birthday_cake.cream;
					if(birthday_cake.cream == 100){
						birthday_cake.cur_state = THIRD_STEP;
					}//if
				}
				break;
			}

	   }//switch

		return 1;
	}
	
	return 0;
}

int add_timer()
{
	if(cake_timer.stat_first == 0){
		INIT_LIST_HEAD(&cake_timer.timer_list);
		++cake_timer.stat_first;
	}
	uint32_t cur_time = get_now_tv()->tv_sec;
	ADD_TIMER_EVENT(&cake_timer, handle_celebrate_finsh_cake_time_out, NULL, cur_time + FINISH_CELEBRATE_BIRHTDAY_CAKE_TIME );
	return 0;
}

int handle_celebrate_finsh_cake_time_out()
{
	memset(&birthday_cake.cal_cream_time, 0, sizeof(birthday_cake));	
	birthday_cake.cur_state =  FIRST_STEP; 
	birthday_cake.cal_cream_time = time(NULL);
	//DEBUG_LOG("birthday_cake.cur_state!!!!!!!!!!!!#######: %u", birthday_cake.cur_state);

	return notify_all_client_players(NULL, 1);
}

int notify_all_client_players(sprite_t* p, int boolean)
{
	int l = sizeof(protocol_t);
	uint8_t b_msg[256] = { 0 };
	uint32_t now = time(NULL);
	if(birthday_cake.cal_cream_time != 0){
		if(birthday_cake.cur_state == SECOND_STEP && birthday_cake.cream != 100){		
			uint32_t span_time = now - birthday_cake.cal_cream_time;
			uint32_t times = span_time / CAL_CAKE_CREAM_MELT_TIME;
			birthday_cake.cream = birthday_cake.cream > times ? (birthday_cake.cream - times):0;
		}//if
	}
	birthday_cake.cal_cream_time = now;
	if(boolean == 1){
		PKG_UINT32(b_msg, birthday_cake.cur_state, l);
		DEBUG_LOG("birthday_cake.cur_state [%u]",birthday_cake.cur_state);
	}
	else{
		if(p->snowball_flag == 1 && birthday_cake.cur_state == FINISH_STEP){
			PKG_UINT32(b_msg, 4, l);// for client to distinguish whether the player get award yet
		}
		else{
			PKG_UINT32(b_msg, birthday_cake.cur_state, l);
		}
	}
	PKG_UINT32(b_msg, birthday_cake.flour, l);
	PKG_UINT32(b_msg, birthday_cake.cream, l);
	PKG_UINT32(b_msg, birthday_cake.almond, l);
	PKG_UINT32(b_msg, birthday_cake.candle, l);
	PKG_UINT32(b_msg, birthday_cake.water, l);

	if(boolean == 1){
		init_proto_head(b_msg, PROTO_MAKE_BIRHTHDAY_START_CAKE_ACTIVIEY, l);
		send_to_map3(MAKE_BIRTHDAY_CAKE_MAP, b_msg, l);
	}
	else{
		init_proto_head(b_msg, p->waitcmd, l);
		send_to_self(p, b_msg, l, 1);
	}

	return 0;
}

int mole_make_birthday_cake_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_IN_CAKE_MAP(p);

	int  material = 0, j = 0;
	CHECK_BODY_LEN(len, sizeof(material));
	UNPKG_UINT32(body, material, j);
	if(material < 0 || material > 4){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	if(birthday_cake.cur_state == FIRST_STEP){
		p->snowball_flag = 0;
	}
	//DEBUG_LOG("material [%u]", material);
	int l = sizeof(protocol_t);
	int ret = check_whether_legal_operate(material);
	if(ret == 0){
		PKG_UINT32(msg, -1, l);

	}
	else{
		PKG_UINT32(msg, material, l);
		notify_all_client_players(NULL, 1);
	}
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
	
}

int get_bithday_cake_curent_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len ,0);	
	CHECK_VALID_ID(p->id);

	notify_all_client_players(p, 0);
	return 0;
}

int get_make_birthday_cake_award_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	uint32_t type = 50020;
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &type, p->id);
}

int get_make_birthday_cake_award_callback(sprite_t* p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t count = 0;
	count = *(uint32_t*)buf;
	*(uint32_t*)p->session = count;
	if(count < 200){
		if(count == 9){
			*(uint32_t*)(p->session+4) = 1;
			uint8_t db_buf[16];
			int off = 0;
			uint32_t item = 161060;
			PKG_H_UINT32( db_buf, 2, off);
			PKG_H_UINT32( db_buf, 1, off);
			PKG_H_UINT32( db_buf, item, off);
			pkg_item_kind(p,db_buf, item, &off);
			return send_request_to_db(SVR_PROTO_GET_ITEM_ARRAY, p, sizeof(db_buf), db_buf, p->id);
			
		}
		else{
			*(uint32_t*)(p->session+4) = 0;
			return get_make_birthday_cake_for_client(p, id, buf, len);
		}
	}
	else{
		*(uint32_t*)(p->session+4) = 0;
		return get_make_birthday_cake_for_client(p, id, buf, len);
	}
}

int get_make_birthday_cake_for_client(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct items_st{
		uint32_t itemid;
		uint32_t count;
	}items_t;
	uint32_t h_count = 0;
	CHECK_BODY_LEN_GE(len, sizeof(h_count));
	h_count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, sizeof(h_count)+h_count*sizeof(items_t));

	uint32_t count = *(uint32_t*)(p->session);
	uint32_t type = *(uint32_t*)(p->session+4);
	uint32_t itemid = 0;
	
	if(type == 0){
		if(count < 200){
			itemid = 180100;
		}
		else{
			itemid = 0;
		}
	}
	else{
		uint32_t count = *(uint32_t*)buf;
		items_t* head = (items_t*)(buf+sizeof(uint32_t));
		if(count > 0 && head->count > 0){
			itemid = 180100;
		}
		else{
			itemid = 161060;//mole lele birthday cake
		}
	}
	if(itemid != 0){
		db_exchange_single_item_op(p, 202, itemid, 1, 0);
		uint32_t day_buf[] = {50020, 200, 1};
		send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);
	}
	p->snowball_flag = 1;
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}



// crystal celebrate dancing ball

dancing_ball_t dancing_ball;

static dancing_ball_timer_t dance_timer; 
#define PAIR_DANCING_LIMIT_TIME    60
#define IS_CRYSTAL_DANCING_MAP(mapid_)	(((uint32_t)mapid_) == CRYSTAL_DANCING_MAP)

#define CHECK_IN_DANCING_BALL_MAP(_p)		\
	do {\
	if (!(_p)->tiles || !IS_CRYSTAL_DANCING_MAP((_p)->tiles->id)) {\
	return send_to_self_error((_p), (_p)->waitcmd, -ERR_invalid_session, 1);\
	}\
	} while(0)

dancing_state_t dance_list[] = {
	JAZZ, SAMBA, RUMBA, CHACHA, WALTZ,
};

#define GET_DACING_RAND_AWARD_SPAN  60

int notify_client_dancing_ball(sprite_t *p)
{
	int l = sizeof(protocol_t);
	uint8_t b_msg[256] = { 0 };

	PKG_UINT32(b_msg, dancing_ball.dancing_state, l);
	
	DEBUG_LOG("for client dance_type: [%u], round: %u", dance_list[dancing_ball.round % 5],dancing_ball.round);
	PKG_UINT32(b_msg, (int)dance_list[dancing_ball.round % 5], l);
	int i = 0;
	for(; i < GROUP_VS_NUMS; ++i){
		PKG_UINT32(b_msg, dancing_ball.pairs[i].position_cinderella, l);
		PKG_UINT32(b_msg, dancing_ball.pairs[i].position_prince, l);
	}
	if(p != NULL){
		init_proto_head(b_msg, p->waitcmd, l);
		send_to_self(p, b_msg, l, 1);
	}
	else{
		init_proto_head(b_msg, PROTO_CRYSTAL_WHOLE_DNANCE_SCENE, l);
		send_to_map3(CRYSTAL_DANCING_MAP, b_msg, l);
	}
	return 0;
}

int mole_join_in_dacing_cmd(sprite_t* p, const uint8_t* body, int len)
{
	typedef struct{
		uint32_t seat;
		uint32_t type; //join or leave, 1: join, 2:leave
	} choice_dance_t;

	CHECK_BODY_LEN(len, sizeof(choice_dance_t));
	CHECK_IN_DANCING_BALL_MAP(p);

	choice_dance_t dance = {0};
	int offset = 0;
	UNPKG_UINT32(body, dance.seat, offset);
	UNPKG_UINT32(body, dance.type, offset);
/* 
#ifndef TW_VER
	uint32_t START_DAY = 20120427;
	uint32_t END_DAY  =  20120607;
#else
	uint32_t START_DAY = 20120504;
	uint32_t END_DAY = 20120614;
#endif
*/
	if( (get_now_tm()->tm_hour < 19 || get_now_tm()->tm_hour > 20)){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	} 
	DEBUG_LOG("check_player : dancing_state: %u, dance_seat: %u, dance.type: %u", 
		dancing_ball.dancing_state, dance.seat, dance.type);
	if((dance.type != 1 && dance.type != 2) || (dance.seat < 1 || dance.seat > 10)){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	uint32_t is_broadcast = check_player_on_seat(dance.seat, dance.type ,p->id);
	//notify to all players
	if(is_broadcast == 1){
		notify_client_dancing_ball(NULL);
		if(dance.type == 1){
			uint32_t tmp_itemd = 1351333;
			db_exchange_single_item_op(p, 202, tmp_itemd, 1, 0);
		}
	}
	
	{
		uint32_t msgbuff[2]= {p->id, 1};
		uint32_t msg_id = 0x0409C388;
		msglog(statistic_logfile, msg_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
	}

	int lens =  sizeof(protocol_t);
	PKG_UINT32(msg, is_broadcast, lens);
	PKG_UINT32(msg, dance.seat, lens);
	init_proto_head(msg, p->waitcmd, lens);
	return send_to_self(p, msg, lens, 1);
}

int check_player_on_seat(uint32_t seat, uint32_t type,  uint32_t uid)
{
	//DEBUG_LOG("!!!!!!!!!!!!!!check_player : dancing_state: %u", dancing_ball.dancing_state);
	uint32_t pos = (seat-1)/2;
	if(dancing_ball.dancing_state != JOINING_DANCE){
		return 0;
	}
	if(type == 1){
		if(seat%2 == 0){
			if(dancing_ball.pairs[pos].position_prince != 0){
				return 0;
			}
			else{
				dancing_ball.pairs[pos].position_prince = uid;
			}
		}
		else{
			if(dancing_ball.pairs[pos].position_cinderella != 0){
				return 0;
			}
			else{
				dancing_ball.pairs[pos].position_cinderella = uid;
			}
		}

		return 1;
	}
	else{
		if(dancing_ball.pairs[pos].position_cinderella == uid){
			dancing_ball.pairs[pos].position_cinderella = 0;
		}
		else if(dancing_ball.pairs[pos].position_prince == uid){
			dancing_ball.pairs[pos].position_prince = 0;
		}
		else{
			return 0;
		}
		return 1;
	}

}

int get_whole_dancing_scene_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_IN_DANCING_BALL_MAP(p);

	int today = get_today();

	//if( (today < START_DAY || today > END_DAY) || (get_now_tm()->tm_hour < 13 || get_now_tm()->tm_hour > 14)){
		if(dance_timer.today == 0 ){
			dance_timer.today = today;
			add_dacing_timer( 60 - get_now_tm()->tm_sec );
			if(get_now_tm()->tm_min % 2 == 1){
				dancing_ball.dancing_state = NOW_DANCING;
			}
		}
	//}
	
	return notify_client_dancing_ball(p);
}

int add_dacing_timer(uint32_t span)
{
  if(dance_timer.stat_first == 0){
	  ++dance_timer.stat_first; 
	  INIT_LIST_HEAD(&dance_timer.timer_list);
  }
  uint32_t cur_time = get_now_tv()->tv_sec;
  if(span == 0){
	  ADD_TIMER_EVENT(&dance_timer, handle_pair_vs_pair_time_out, NULL, cur_time + PAIR_DANCING_LIMIT_TIME);
  }
  else{
	   ADD_TIMER_EVENT(&dance_timer, handle_pair_vs_pair_time_out, NULL, cur_time + span);
  }
  
  return 0;
}

int kick_off_dacing_user(sprite_t *p)
{
	int i = 0, right_user = 0;
	//check if pair vs pair start now
	if(dancing_ball.dancing_state == JOINING_DANCE){
		for(; i < 5; ++i){
			if(dancing_ball.pairs[i].position_cinderella == p->id){
				dancing_ball.pairs[i].position_cinderella = 0;
				right_user = 1;
				break;
			}
			else if(dancing_ball.pairs[i].position_prince == p->id){
				dancing_ball.pairs[i].position_prince = 0;
				right_user = 1;
				break;
			}
		}
	}//if

	if(right_user == 1){
		//if(dancing_ball.dancing_state == JOINING_DANCE){
			notify_client_dancing_ball(NULL);
		//}
	}
	return 0;
}

int handle_pair_vs_pair_time_out()
{
 DEBUG_LOG("time out now begin : dancing_state: %u", dancing_ball.dancing_state);

 if(dancing_ball.dancing_state == JOINING_DANCE){
	 uint32_t i = 0, already_pairs = 0;
	 for(; i < GROUP_VS_NUMS; ++i){
		 if(dancing_ball.pairs[i].position_cinderella != 0 && dancing_ball.pairs[i].position_prince != 0){
			 ++already_pairs;
		 }
		 else{
			 if(dancing_ball.pairs[i].position_cinderella != 0){
				 dancing_ball.pairs[i].position_cinderella  = 0;
			 }
			 if(dancing_ball.pairs[i].position_prince != 0){
				 dancing_ball.pairs[i].position_prince  = 0;
			 }
		 }
	 }//for

	 dancing_ball.dancing_state = NOW_DANCING;
	 DEBUG_LOG("time out now continue : dancing_state: %u, already_pairs: %u", dancing_ball.dancing_state, already_pairs);

	 notify_client_dancing_ball(NULL);
	 return add_dacing_timer(0);
 }
 else{
	 dancing_ball.dancing_state = DANCING_OVER;//

	 uint32_t rank[GROUP_VS_NUMS] = {0};
	 int i = 0, group_count = 0;
	 for(; i < GROUP_VS_NUMS; ++i){
		 if(dancing_ball.pairs[i].position_cinderella != 0 || dancing_ball.pairs[i].position_prince != 0){
			 rank[group_count] = i+1;
			 ++group_count;
		 }//if
	 }
	  int champion = -1;
	  random_arrage(rank, group_count, &champion);
	  //uint32_t m = 0;
	 //send to all players which who are champions
	  if( champion != -1 && champion != 0){
		  sprite_t* cham_player_1 = NULL;
		  sprite_t* cham_player_2 = NULL;

		  if(dancing_ball.pairs[champion-1].position_cinderella != 0 || dancing_ball.pairs[champion-1].position_prince != 0){
			  if(dancing_ball.pairs[champion-1].position_cinderella != 0){
				  cham_player_1 = get_sprite(dancing_ball.pairs[champion-1].position_cinderella);
			  }
			  if(dancing_ball.pairs[champion-1].position_prince != 0){
				  cham_player_2 = get_sprite(dancing_ball.pairs[champion-1].position_prince);
			  }
		  }
		  if(cham_player_1 != NULL && cham_player_2 != NULL){
				 uint8_t s_msg[256] = {0};
				 char txt[256] = {0};
				 #ifndef TW_VER
				 int msg_len = sprintf(txt, "本轮舞会的冠军是：%s 和 %s !",  cham_player_1->nick,cham_player_2->nick);
				 #else
				 int msg_len = sprintf(txt, "本輪舞會冠軍是：%s 和 %s !",  cham_player_1->nick,cham_player_2->nick);
				 #endif
				 int j = sizeof(protocol_t);
				 PKG_UINT32(s_msg, msg_len, j);
				 PKG_STR( s_msg, txt, j, msg_len);
				 init_proto_head( s_msg, PROTO_TELL_ALL_PLAYERS_ROUND_CHAMPION_DANCE, j );
				 send_to_all_players( NULL, s_msg, j, 0 );
				 
		  }
		  else{
			  sprite_t* one_cham_player = NULL;
			  if(cham_player_1 != NULL){
				  one_cham_player = cham_player_1;
			  }
			  if(cham_player_2 != NULL){
				  one_cham_player = cham_player_2;
			  }
			  if(one_cham_player != NULL){  	  
				 uint8_t s_msg[256] = {0};
				 char txt[256] = {0};
			#ifndef TW_VER
				int msg_len = sprintf(txt, "本轮舞会的冠军是：%s !", one_cham_player->nick);
			#else
				 int msg_len = sprintf(txt, "本輪舞會冠軍是：%s !", one_cham_player->nick);
			#endif
				int j = sizeof(protocol_t);
				PKG_UINT32(s_msg, msg_len, j);
				PKG_STR( s_msg, txt, j, msg_len);
				init_proto_head( s_msg, PROTO_TELL_ALL_PLAYERS_ROUND_CHAMPION_DANCE, j );
				send_to_all_players( NULL, s_msg, j, 0 );
			  }
		  }//else
	  }

	  
	 //for(i = 0; i < GROUP_VS_NUMS; ++i){
	 for(i = 0; i < group_count; ++i){
		 uint32_t team = rank[i]-1;
		sprite_t  *sigle_player = NULL;
		if(dancing_ball.pairs[team].position_cinderella != 0){
			//DEBUG_LOG(" left team: %u, rank: %u", team+1, i+1);
			sigle_player = get_sprite(dancing_ball.pairs[team].position_cinderella);
			if(sigle_player != NULL){
				int l = sizeof(protocol_t);
				uint8_t n_msg[64] = {0};
				PKG_UINT32(n_msg, i+1, l);
				init_proto_head(n_msg, PROTO_TELL_CLIENT_TO_GET_DANCE_AWARD, l);
				send_to_self(sigle_player, n_msg, l, 1);

			}
		}
		sprite_t* another_player = NULL;
		if(dancing_ball.pairs[team].position_prince != 0){
			//DEBUG_LOG(" right team: %u, rank: %u", team+1, i+1);
			another_player = get_sprite(dancing_ball.pairs[team].position_prince);
			if(another_player != NULL){
				int l = sizeof(protocol_t);
				uint8_t n_msg[64] = {0};
				PKG_UINT32(n_msg, i+1, l);
				init_proto_head(n_msg, PROTO_TELL_CLIENT_TO_GET_DANCE_AWARD, l);
				send_to_self(another_player, n_msg, l, 1);
			} 
		}
		 

	 }
	 
	//new dancing round begin
	memset(dancing_ball.pairs, 0, sizeof(dancing_ball) - sizeof(dancing_ball.dancing_state));
	dancing_ball.round = get_now_tm()->tm_min / 2;
	notify_client_dancing_ball(NULL);
	dancing_ball.dancing_state = JOINING_DANCE;
	DEBUG_LOG("time out now end : dancing_state: %u", dancing_ball.dancing_state);

	return add_dacing_timer(0);
 }

}

int random_arrage(uint32_t *rank, uint32_t count, int *top)
{
	*top = -1;
	if(count == 0 ){
		return 0;
	}
	else if(count == 1){
		*top = rank[0];
		return 0;
	}
	int k = 0;
	for( k = count; k > 0; --k){
		uint32_t data = rank[0]; 
		uint32_t middle = rand()%count;
		rank[0] = rank[middle];
		rank[middle] = data;
	}
	*top = rank[0];
	return 0;
}

int get_dancing_random_item_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_VALID_ID(p->id);
	CHECK_BODY_LEN(len, 0);
	CHECK_IN_DANCING_BALL_MAP(p);
/* 
#ifndef TW_VER
	uint32_t START_DAY = 20120427;
	uint32_t END_DAY  =  20120607;
#else
	uint32_t START_DAY = 20120504;
	uint32_t END_DAY = 20120614;
#endif
	int today = get_today();
*/

	if((get_now_tm()->tm_hour >= 19 && get_now_tm()->tm_hour <= 20)){
		if(dancing_ball.dancing_state == NOW_DANCING){
			if(get_now_tv()->tv_sec - p->water_bomb_time >= GET_DACING_RAND_AWARD_SPAN){
				uint32_t type = 50021;
				*(uint32_t*)p->session = 1;
				return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &type, p->id);
			}
		}
	} 
	
	
	*(uint32_t*)p->session = 0;
	return give_award_crystal_dancing_ball(p, p->id, NULL, 0);
}

int give_award_crystal_dancing_ball(sprite_t *p, uint32_t id, char* buf, int len)
{
	
	int lens =  sizeof(protocol_t);
	int itemid_1 = 1351328;
	int itemid_2 = 0;

	uint32_t is_from_db = *(uint32_t*)p->session;
	if(is_from_db == 0){
		PKG_UINT32(msg, 0, lens);
		PKG_UINT32(msg, 0, lens);
		PKG_UINT32(msg, 0, lens);
		PKG_UINT32(msg, 0, lens);
		PKG_UINT32(msg, 0, lens);
	}
	else{
		CHECK_BODY_LEN(len, 4);
		uint32_t day_limit = *(uint32_t*)buf;
		if(day_limit < 200){
			PKG_UINT32(msg, 1, lens);
			PKG_UINT32(msg, itemid_1, lens);
			PKG_UINT32(msg, 2, lens);
			db_exchange_single_item_op(p, 202, itemid_1, 2, 0);
			PKG_UINT32(msg, itemid_2, lens);
			PKG_UINT32(msg, 2, lens);
			db_exchange_single_item_op(p, 202, itemid_2, 2, 0);
			uint32_t day_buf[] = {50021, 200, 1};
			send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);
			p->water_bomb_time = get_now_tv()->tv_sec;
		}
		else{
			PKG_UINT32(msg, 0, lens);
			PKG_UINT32(msg, 0, lens);
			PKG_UINT32(msg, 0, lens);
			PKG_UINT32(msg, 0, lens);
			PKG_UINT32(msg, 0, lens);
		}
	}

	init_proto_head(msg, p->waitcmd, lens);
	return send_to_self(p, msg, lens, 1);
}

int check_dancing_three_times_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len ,8);
	uint32_t type = 0, flag = 0;
	int i = 0;
	UNPKG_UINT32(body, type, i);
	UNPKG_UINT32(body, flag, i);//flag:1,2,3,4,5,6,7
	

	*(uint32_t*)p->session = type;
	*(uint32_t*)(p->session + 4) = flag;
	//type == 0 represent querying, else exchanging
	DEBUG_LOG("type: %u, flag: %u", type, flag);
	if(flag < 1 || type > 1 || (type == 0 && flag == 7) || type >= 8){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	
	int off = 0, incr_off = 8;
	uint32_t total = 1, k = 0;
	uint32_t tmp_flag = flag;
	if(flag == 7){
		tmp_flag = 1;
	}
	for(; k < 3; ++k){
		if(tm_type_item[tmp_flag - 1][k].itemid != 0){
			++total;
		}
	}//for
	uint8_t db_buf[128];
	memset(db_buf, 0, sizeof(db_buf));
	PKG_H_UINT32( db_buf, 1351328, incr_off);
	pkg_item_kind(p,db_buf, 1351328, &incr_off);
	
	for(k = 0; k < 3; ++k){
		if(tm_type_item[tmp_flag - 1][k].itemid != 0){
			PKG_H_UINT32( db_buf, tm_type_item[tmp_flag - 1][k].itemid, incr_off);
			pkg_item_kind(p,db_buf, tm_type_item[tmp_flag - 1][k].itemid, &incr_off);
			//++total;
		}
	}//for
	PKG_H_UINT32( db_buf, 2, off);
	PKG_H_UINT32( db_buf, total, off);
	
	return send_request_to_db(SVR_PROTO_GET_ITEM_ARRAY, p, incr_off, db_buf, p->id);

}

int get_crystal_dancing_times(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct items_st{
		uint32_t itemid;
		uint32_t count;
	}items_t;

	uint32_t count = 0;
	CHECK_BODY_LEN_GE(len, sizeof(count));
	count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, sizeof(count)+ count*sizeof(items_t));

	items_t* head = (items_t*)(buf+sizeof(uint32_t));

	uint32_t type = *(uint32_t*)p->session;
	uint32_t flag = *(uint32_t*)(p->session+4);
	uint32_t final_use_flag = 0;
	if(flag == 7){
		flag = 1;
		final_use_flag = 7;
	}
	uint32_t ret_count = 0;
	int k = 0;
	if(count > 1){
		items_t* pointer = head+1;
		uint32_t f1 = 0, f2 = 0, f3 = 0;
		for(; k < 3; ++k){
			if(tm_type_item[flag - 1][k].itemid != 0){	
				if(pointer->itemid == tm_type_item[flag - 1][k].itemid){
					uint32_t middle_cnt = pointer->count;
					if(pointer->count > tm_type_item[flag - 1][k].count){
						middle_cnt = tm_type_item[flag - 1][k].count;
					}
					if(k == 0){
						f1 = middle_cnt *100;
					}
					else if(k == 1){
						f2 = middle_cnt *10;
					}
					else{
						f3 = middle_cnt;
					}
				}

			}//if
			++pointer;
		}//for
		ret_count = f1 + f2 + f3;
	}//if


	if(type == 0){
		int l = sizeof(protocol_t);
		PKG_UINT32(msg, type, l);
		PKG_UINT32(msg, flag, l);
		 PKG_UINT32(msg, ret_count, l);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
		//PKG_UINT32(msg, 0, l);
	}
	else{
		DEBUG_LOG("itemid: %u, count: %u, count_2: %u", head->itemid, head->count, del_itemid_count[flag-1]);
		if(head->itemid == 1351328 && head->count >= del_itemid_count[flag-1]){
			*(uint32_t*)(p->session+8) = count;
			*(uint32_t*)(p->session+12) = ret_count;
			if(final_use_flag == 7){
				return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &datetype[final_use_flag-1], p->id);
			}
			else{
				return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &datetype[flag-1], p->id);
			}
			
		}
		else{
			int l = sizeof(protocol_t);
			PKG_UINT32(msg, type, l);
			PKG_UINT32(msg, 0, l);
			init_proto_head(msg, p->waitcmd, l);
			return send_to_self(p, msg, l, 1);
		}
	}

}

int get_part_in_three_times_dancing(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t type;
		uint32_t flag;
		uint32_t head_count;
		uint32_t particle_count;
	}tm_session_t;

	CHECK_BODY_LEN(len, 4);
	
	uint32_t max_limit = *(uint32_t*)buf;
	tm_session_t *cur = (tm_session_t *)p->session;
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cur->type, l);
	//PKG_UINT32(msg, cur->flag, l);
	uint32_t final_user_flag = cur->flag;
	if(cur->flag == 7){
		cur->flag = 1;
	}
	uint32_t comp_base_cnt = 0;
	uint32_t boundary = 1;
	if(cur->flag == 3 || cur->flag == 5){
		boundary = 99999999;
	}
	if(max_limit >= boundary){
		//int rt = -1
		PKG_UINT32(msg, 1, l);
		PKG_UINT32(msg, 0, l);
	}
	else{
		
		switch(cur->flag){
		case 1:
			{
				comp_base_cnt = 310;
				break;
			}
		case 2:
			{
				comp_base_cnt = 110;
				break;
			}
		case 3:
			{
				comp_base_cnt = 300;
				break;
			}
		case 4:
			{
				comp_base_cnt = 110;

				break;
			}
		case 5:
			{
				comp_base_cnt = 111;
				break;
			}
		case 6:
			{
				comp_base_cnt = 110;
				break;
			}
		}
		cur->flag = final_user_flag; 
		if( cur->head_count > 0 && cur->particle_count >= comp_base_cnt ){
			//db_exchange_items()
			int sendlen = 0;
			uint8_t buff[128];
			uint32_t del_count = 1;
			PKG_H_UINT32( buff, del_count, sendlen );

			uint32_t add_itemid[][4] = {
				{14239, 14240, 0, 0},
				{14235, 14236, 14237, 14238},
				{1353321, 0, 0, 0},
				{14234, 0, 0, 0},
				{1270134, 0, 0, 0},
				{14241, 14242, 14243, 0},
				////
				{14244, 14245, 14246, 14247},
			};
			uint32_t i = 0, total = 0;
			for(; i < 4; ++i){
				if(add_itemid[cur->flag - 1][i] != 0){
					++total;
				}
	
			}

			uint32_t add_count = total;
			PKG_H_UINT32( buff, add_count, sendlen );

			PKG_H_UINT32( buff, 202, sendlen );
			PKG_H_UINT32( buff, 0, sendlen );

			uint32_t del_itemid = 1351328;
			pkg_item_kind(p, buff, del_itemid, &sendlen);
			PKG_H_UINT32( buff, del_itemid, sendlen);
			PKG_H_UINT32( buff, del_itemid_count[cur->flag - 1], sendlen);

			PKG_UINT32(msg, total, l);

			for(i = 0; i < 4; ++i){
				if(add_itemid[cur->flag-1][i] != 0){
					item_kind_t* ik = find_kind_of_item(add_itemid[cur->flag-1][i]);
					item_t* it = get_item( ik, add_itemid[cur->flag-1][i] );
					pkg_item_kind( p, buff, add_itemid[cur->flag-1][i], &sendlen);
					PKG_H_UINT32( buff, add_itemid[cur->flag-1][i], sendlen);
					uint32_t add_itemid_count = 1;
					PKG_H_UINT32( buff, add_itemid_count, sendlen);
					PKG_H_UINT32( buff, it->max, sendlen);

					PKG_UINT32(msg, add_itemid[cur->flag-1][i], l);	
				}
			}//for

			send_request_to_db( SVR_PROTO_EXCHG_ITEM, p, sendlen, buff, p->id );
			if(cur->flag == 3){
				uint32_t day_buf[] = {datetype[cur->flag-1], 99999999, 1};
				send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);
			}
			else{
				uint32_t day_buf[] = {datetype[cur->flag-1], 1, 1};
				send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);
			}
			
			
		}
		else{
			PKG_UINT32(msg, 0, l);
			//PKG_UINT32(msg, 0, l);
		}
	}
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int check_four_celebrate_book_light_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);

	int i = 0, k = 0 ;
	uint32_t total = 0;


	uint8_t db_buf[128];
	memset(db_buf, 0, sizeof(db_buf));

	int off = 0, inc_off = 8;


	for(i = 0; i < 6; ++i){
		for(k = 0; k < 3; ++k){
			if(tm_type_item[i][k].itemid != 0){
				//DEBUG_LOG("itemid: %u", tm_type_item[i][k].itemid);
				PKG_H_UINT32( db_buf, tm_type_item[i][k].itemid, inc_off);
				pkg_item_kind(p,db_buf, tm_type_item[i][k].itemid, &inc_off);
				++total;
			}
		}//for
	}
	PKG_H_UINT32( db_buf, 2, off);
	PKG_H_UINT32( db_buf, total, off);
	//DEBUG_LOG("other total: %u", total);
	return send_request_to_db(SVR_PROTO_GET_ITEM_ARRAY, p, inc_off, db_buf, p->id);
}

int generate_book_light_flag(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct items_st{
		uint32_t itemid;
		uint32_t count;
	}items_t;
	
	uint32_t count = 0;
	CHECK_BODY_LEN_GE(len, sizeof(count));
	count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, sizeof(count)+ count*sizeof(items_t));
	items_t* head = (items_t*)(buf+4);
	//int n = 0;
	//for(; n < count; ++n){
	//	DEBUG_LOG("itemid: %u, n: %u, count: %u", (head+n)->itemid, n, (head+n)->count);
	//}
	int l = sizeof(protocol_t);
	int i = 0, j = 0;
	for(; i < 6; ++i){
		uint32_t light_flag = 0;
		uint32_t f1 = 0, f2 =0, f3 = 0;
		for(j = 0; j < 3; ++j){
			int k = 0;
			for(; k < count; ++k){
				//DEBUG_LOG("itemid: %u, i: %u, j: %u, k: %u", (head+k)->itemid, i, j, k);
				if(tm_type_item[i][j].itemid == (head+k)->itemid){
					//DEBUG_LOG("22222itemid: %u, i: %u, j: %u, k: %u", (head+k)->itemid, i, j, k);
					break;
				}
			}
			if(k < count){
				int tm_cnt = 0;
				if( (head+k)->count > figure_bit[i][j]){
					tm_cnt = figure_bit[i][j];
				}
				else{
					tm_cnt = (head+k)->count;
				}
				if(j == 0){
					f1 = tm_cnt;
				}
				else if(j == 1){
					f2 = tm_cnt;
				}
				else{
					f3 = tm_cnt;
				}
			}

		}//for
		light_flag = f1*100 + f2*10 + f3;
		uint32_t lit = 0;
		if(i == 0){
			if(light_flag == 310){
				lit = 1;
			}
		}
		else if(i == 1){
			if(light_flag == 110){
				lit = 1;
			}
		}
		else if(i == 2){
			if(light_flag == 300){
				lit = 1;
			}
		}
		else if(i == 3){
			if(light_flag == 110){
				lit = 1;
			}
		}
		else if( i== 4){
			if(light_flag == 111){
				lit = 1;
			}
		}
		else if( i== 5){
			if(light_flag == 110){
				lit = 1;
			}
		}
	   
		PKG_UINT32(msg, lit, l);
		//DEBUG_LOG("light: %u, i: %u, light_flag: %u", lit, i, light_flag);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int mole_set_miss_note_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 8);

	uint32_t type = 0, flag = 0;
	int l = 0;
	UNPKG_UINT32(body, type, l);
	UNPKG_UINT32(body, flag, l);

	if(type != 1 && type != 2){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	if((type == 1 && flag != 0) || (type == 2 && (flag < 1 || flag > 5))){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	*(uint32_t*)p->session = type;
	*(uint32_t*)(p->session+4) = flag;
	uint32_t db_type = 0;
	if(type == 1){
		db_type = 50022;
		{
			uint32_t msgbuff[2]= {p->id, 1};
			uint32_t msg_id = 0x0409C3A9;
			msglog(statistic_logfile, msg_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}
	else{
		db_type = 50023;
		{
			uint32_t msgbuff[2]= {p->id, 1};
			uint32_t msg_id = 0x0409C3AA;
			msglog(statistic_logfile, msg_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));
		}
	}
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &db_type, p->id);
}

int get_mole_miss_note_day_times(sprite_t *p, uint32_t id, char* buf, int len)
{

	CHECK_BODY_LEN(len, 4);
	
	uint32_t day_max = *(uint32_t*)buf;
	if(day_max >= 3){
		return send_to_self_error(p, p->waitcmd, -ERR_miss_note_acitvity_max_day, 1);
	}
	uint32_t db_buf[] = {*(uint32_t*)p->session, *(uint32_t*)(p->session+4)};//

	return send_request_to_db(SVR_PROTO_SET_MISS_NOTE, p, sizeof(db_buf), db_buf, p->id);

}

int mole_set_miss_note_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 20);
	
	uint32_t state = *(uint32_t*)buf;
	uint32_t round = *(uint32_t*)(buf+4);
	uint32_t item = *(uint32_t*)(buf+8);
	uint32_t item3 = *(uint32_t*)(buf+12);
	uint32_t award = *(uint32_t*)(buf+16);

	DEBUG_LOG("state: %u, round: %u, item: %u, item3: %u, award: %u", state, round, item, item3, award);
	uint32_t type = *(uint32_t*)p->session;
	if(state == 1){
		if(type == 1){
			uint32_t day_buf[] = {50022, 3, 1};
			send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);
		}
		else if(round == 1){
			uint32_t day_buf[] = {50023, 3, 1};
			send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);
		}
	}
	else{
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, round, l);
	PKG_UINT32(msg, type, l);
	if(type == 1){
		uint32_t items[] = {1, item3, award};
		uint32_t total = 0;
		int k = 0;
		for(; k < sizeof(items)/sizeof(uint32_t); ++k){
			if(items[k] != 0){
				++total;
			}
		}
		PKG_UINT32(msg, total, l);
		for(k = 0; k < sizeof(items)/sizeof(uint32_t); ++k){
			if(items[k] != 0){
				if(k == 0){
					PKG_UINT32(msg, 0,  l);
					PKG_UINT32(msg, 200, l);
					uint32_t xiaomee = 0;
					db_exchange_single_item_op(p, 202, xiaomee, 200, 0);
				}
				else{
					PKG_UINT32(msg, items[k], l);
					PKG_UINT32(msg, 1, l);
				}
			}
		}
		DEBUG_LOG("total_1: [%u]", total);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}
	else{
		if(item == 0){
			PKG_UINT32(msg, 0, l);
			init_proto_head(msg, p->waitcmd, l);
			return send_to_self(p, msg, l, 1);
		}
		else{
			uint32_t items[] = {item, item3, award};
			uint32_t total = 0;
			int k = 0;
			for(; k < sizeof(items)/sizeof(uint32_t); ++k){
				if(items[k] != 0){
					++total;
				}
			}
			PKG_UINT32(msg, total, l);
			for(k = 0; k < sizeof(items)/sizeof(uint32_t); ++k){
				if(items[k] != 0){
					PKG_UINT32(msg, items[k], l);
					PKG_UINT32(msg, 1, l);
				}
			}
			DEBUG_LOG("total_2: [%u]", total);
			init_proto_head(msg, p->waitcmd, l);
			return send_to_self(p, msg, l, 1);
		}

	}
	return 0;
}

int mole_get_enery_rock_stones_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	return send_request_to_db(SVR_PROTO_GET_ENERGY_STONE_ROUND, p, 0, NULL, p->id);
}

int mole_get_enery_rock_stones_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	uint32_t state = 0, times = 0;

	state = *(uint32_t*)buf;
	times = *(uint32_t*)(buf+4);
	switch(p->waitcmd){
		case PROTO_MOLE_GET_ENERGY_STONE:
			{
				int l = sizeof(protocol_t);
				PKG_UINT32(msg, state, l);
				init_proto_head(msg, p->waitcmd, l);
				return send_to_self(p, msg, l, 1);
			}
		case PROTO_MOLE_GET_MISS_NOTE:
			{
				*(uint32_t*)p->session = times;
				uint8_t db_buf[128];
				memset(db_buf, 0, sizeof(db_buf));

				uint32_t flag_item[] = {1351337, 1353306 };
				int off = 0;
				PKG_H_UINT32( db_buf, 2, off);
				PKG_H_UINT32( db_buf, sizeof(flag_item)/sizeof(uint32_t), off);
				uint32_t k = 0;
				for(k = 0; k < sizeof(flag_item)/sizeof(uint32_t); ++k){
					PKG_H_UINT32( db_buf, flag_item[k], off);
					pkg_item_kind(p,db_buf, flag_item[k], &off);
				}//for
				return send_request_to_db(SVR_PROTO_GET_ITEM_ARRAY, p, off, db_buf, p->id);
			}
	}
	return 0;
	
}

int mole_get_miss_note_cmd(sprite_t* p, const uint8_t* body, int len)
{

	CHECK_BODY_LEN(len, 0);

	return send_request_to_db(SVR_PROTO_GET_ENERGY_STONE_ROUND, p, 0, NULL, p->id);

}

int mole_check_expedition_lock_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t flag = 0;
	int l = 0;
	UNPKG_UINT32(body, flag, l);
	return send_request_to_db(SVR_PROTO_CHECK_EXPEDITION_LOCK, p, 4, &flag, p->id);

}

int mole_check_expedition_lock_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t already_unlock = 0;
	int offset = 0;
	UNPKG_H_UINT32(buf, already_unlock, offset);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, already_unlock, l);
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

int mole_get_chapter_state_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t type;
		uint32_t state;
	}chapter_t;
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t type_count = 0;
	int offset = 0;
	UNPKG_H_UINT32(buf, type_count, offset);
	CHECK_BODY_LEN(len, 4+type_count*sizeof(chapter_t));
	//DEBUG_LOG("=======type_count: %u", type_count);
	//for chapter 1, 2
	typedef struct items_st{
		uint32_t itemid;
		uint32_t count;
	}items_t;
	uint32_t count = *(uint32_t*)(p->session+4);
	items_t* head = (items_t*)(p->session+sizeof(uint32_t)*2);
	uint32_t flag_item[] = {1351337, 1353306 };
	uint32_t flag_count[] = {3, 1};

	uint32_t finish = 0, index = 0;
	int l = sizeof(protocol_t);
	int i = 0, j = 0;
	for(; i < 2; ++i){
		for(j = 0; j < count; ++j){
			if((head+j)->itemid == flag_item[i] && (head+j)->count >= flag_count[i]){
				//if(i == 1){
				//	PKG_UINT32(msg, 1, l);
				//}
				//else{
					PKG_UINT32(msg, 2, l);
					finish = finish | ( 1<<index);
					++index;
				//}
				
				break;
			}
		}
		if(j >= count){
			PKG_UINT32(msg, 1, l);
		}
		//for the chapter now step
		if(i == 0){
			int k = 0;
			for(; k < count; ++k){
				if((head+k)->itemid == flag_item[i]){
					break;
				}
			}
			if(k < count){
				PKG_UINT32(msg, (head+k)->count, l);
			}
			else{
				PKG_UINT32(msg, 0, l);
			}

		}
		else{
			PKG_UINT32(msg, *(uint32_t*)p->session, l);
		}
	}//for

	//for chapter 3...
	//type includes 38,39,40....
	chapter_t* other = (chapter_t*)(buf+sizeof(uint32_t));
	uint32_t already_step = 0, k = 0;
	if(type_count == 0){
		PKG_UINT32(msg, 1, l);
		PKG_UINT32(msg, 0, l);
		PKG_UINT32(msg, 1, l);
		PKG_UINT32(msg, 0, l);
		PKG_UINT32(msg, 1, l);
		PKG_UINT32(msg, 0, l);
	}
	else{
		uint32_t exist[3] = {0, 0, 0};
		uint32_t ret_state[3] = {0, 0, 0};
		for(; k < type_count; ++k){
			if((other+k)->type == 38){
				exist[0] = 1;
				ret_state[0] = (other+k)->state;
			}
			else if((other+k)->type == 39){
			
				exist[1] = 1;
				ret_state[1] = (other+k)->state;
			}
			else if((other+k)->type == 40){
				exist[2] = 1;
				ret_state[2] = (other+k)->state;
			}
		}//for
		for(k = 0; k < sizeof(exist)/sizeof(uint32_t); ++k){
			if(exist[k] == 1 ){
				if(k == 0){
					if(ret_state[k] == 255){
						PKG_UINT32(msg, 2, l);
						finish = finish | ( 1<<index);
						++index;
					}
					else{
						PKG_UINT32(msg, 1, l);
					}
					uint32_t m = 0;
					for(; m < 8; ++m){
						uint32_t bit = 1 << m;
						if((ret_state[k] & bit) != 0){
							++already_step;
						}
					}//for
					
					PKG_UINT32(msg, already_step, l);
				}
				else if(k == 1){
					if(ret_state[k] >= 3){
						PKG_UINT32(msg, 2, l);
						finish = finish | ( 1<<index);
						++index;
					}
					else{
						PKG_UINT32(msg, 1, l);
					}
					PKG_UINT32(msg, ret_state[k], l);
					DEBUG_LOG("(other+k)->state: [ %u ]", ret_state[k]);
				}
				else if(k == 2){
					if(ret_state[k] >= 10){
						PKG_UINT32(msg, 2, l);
						finish = finish | ( 1<<index);
						++index;
					}
					else{
						PKG_UINT32(msg, 1, l);
					}
					PKG_UINT32(msg, ret_state[k], l);
				}

			}
			else{
				PKG_UINT32(msg, 1, l);
				PKG_UINT32(msg, 0, l);
			}
		}
	}
	DEBUG_LOG("finish=====: %u", finish);
	if(finish == 31){
		db_exchange_single_item_op(p, 202, 1351370, 1, 0);
	}
	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

int mole_get_make_ship_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	uint32_t db_type = 50025;
	DEBUG_LOG("db_type: %u", db_type);
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &db_type, p->id);
}

int mole_get_make_ship_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t state;
		uint32_t count;
	}ret_head_t;
	typedef struct{
		uint32_t itemid;
	} ret_tms_t;
	
	CHECK_BODY_LEN_GE(len, sizeof(ret_head_t));
	
	ret_head_t *head = (ret_head_t*)buf;

	DEBUG_LOG("count: %u, state: %u", head->count, head->state);
	CHECK_BODY_LEN(len, sizeof(ret_tms_t)*head->count+sizeof(ret_head_t));
	ret_tms_t *cur = (ret_tms_t *)(buf+sizeof(ret_head_t));

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, head->state, l);
	PKG_UINT32(msg, head->count, l);
	uint32_t k = 0;
	for(; k < head->count; ++k){
		PKG_UINT32(msg, (cur+k)->itemid, l);
	}
	uint32_t day_buf[] = {50025, 10, 1};
	send_request_to_db(SVR_PROTO_SET_STH_DONE, NULL, sizeof(day_buf), day_buf, p->id);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int mole_funfly_seahorse_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t direction = 0;
	int l = 0;
	UNPKG_UINT32(body, direction, l);

	uint32_t db_buff[] = {40, direction};
	return send_request_to_db(SVR_PROTO_SET_CHAPTER_STATE, p, sizeof(db_buff), db_buff, p->id);

}


int mole_funfly_seahorse_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t state;
		uint32_t fly_cnt;
		uint32_t item_cnt;
		uint32_t itemid;
	} fly_seahorse_t;

	CHECK_BODY_LEN(len, sizeof(fly_seahorse_t));
	fly_seahorse_t *cur = (fly_seahorse_t *)buf;
	if(p->waitcmd == PROTO_MOLE_FUNFLY_SEAHORSE){
		int l = sizeof(protocol_t);
		PKG_UINT32(msg, cur->state, l);
		PKG_UINT32(msg, cur->fly_cnt, l);
		PKG_UINT32(msg, cur->item_cnt, l);
		PKG_UINT32(msg, cur->itemid, l);

		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}
	else if(p->waitcmd == PROTO_MOLE_ATTACK_INVADE_MONSTER){
		DEBUG_LOG("state: %u, itemid; %u", cur->state, cur->itemid);
		if(cur->state == 3){
			int offset = sizeof(protocol_t);
			PKG_UINT32(msg, cur->state, offset);
			PKG_UINT32(msg, p->invade_monster, offset);
			PKG_UINT32(msg, p->monster_hp, offset);
			PKG_UINT32(msg, 0, offset);
			init_proto_head(msg, p->waitcmd, offset);
			return send_to_self(p, msg, offset, 1);
		}
		uint32_t ocean_tear = *(uint32_t*)p->session;
		p->monster_hp = p->monster_hp >= 5*ocean_tear ? (p->monster_hp - 5*ocean_tear): 0;
		db_delete_single_item_op(p, 99, 1351390, ocean_tear, 0);
		DEBUG_LOG("state: %u, itemid; %u, monster_hp: %u", cur->state, cur->itemid, p->monster_hp);
		if(p->monster_hp != 0){
			int offset = sizeof(protocol_t);
			PKG_UINT32(msg, cur->state, offset);
			PKG_UINT32(msg, p->invade_monster, offset);
			PKG_UINT32(msg, p->monster_hp, offset);
			PKG_UINT32(msg, 0, offset);
			init_proto_head(msg, p->waitcmd, offset);
			return send_to_self(p, msg, offset, 1);
		}

		uint32_t msgbuff[2]= {p->id, 1};
		uint32_t msg_id = 0x0409C443 + p->invade_monster - 1;
		msglog(statistic_logfile, msg_id, get_now_tv()->tv_sec, msgbuff, sizeof(msgbuff));

		uint32_t monster_award[3] = {5, 8, 10};
		int l = sizeof(protocol_t);
		PKG_UINT32(msg, cur->state, l);
		PKG_UINT32(msg, p->invade_monster, l);
		PKG_UINT32(msg, p->monster_hp, l);
		DEBUG_LOG("--------state: %u, itemid; %u, monster_hp: %u", cur->state, cur->itemid, p->monster_hp);
		uint32_t cnt = 0;
		if(cur->itemid != 0){
			++cnt;
		}
		if(p->invade_monster == 4){
			if((cur->state&0x01) == 0){
				++cnt;
			}
			if((cur->state&0x02) == 0){
				++cnt;
			}
			PKG_UINT32(msg, cnt, l);
			if((cur->state&0x01) == 0){
				//++cnt;
				PKG_UINT32(msg, 1351389, l);
				PKG_UINT32(msg, 10, l);
				db_exchange_single_item_op(p, 99, 1351389, 10, 0);
				uint32_t db_buff[2]= {49, 10};
				send_request_to_db(SVR_PROTO_SET_CHAPTER_STATE, NULL, sizeof(db_buff), db_buff, p->id);
			}
			if((cur->state&0x02) == 0){
				//++cnt;
				PKG_UINT32(msg, 1623003, l);
				PKG_UINT32(msg, 1, l);
				db_exchange_single_item_op(p, 0, 1623002, 1, 0);
				uint32_t db_buff[2]= {48, 1};
				send_request_to_db(SVR_PROTO_SET_CHAPTER_STATE, NULL, sizeof(db_buff), db_buff, p->id);
			}

		}
		else{
			if((cur->state&0x01) == 0){
				++cnt;
				PKG_UINT32(msg, cnt, l);
				PKG_UINT32(msg, 1351389, l);
				PKG_UINT32(msg, monster_award[p->invade_monster - 1], l);
				db_exchange_single_item_op(p, 99, 1351389, 10, 0);
				uint32_t db_buff[2]= {49, monster_award[p->invade_monster - 1]};
				send_request_to_db(SVR_PROTO_SET_CHAPTER_STATE, NULL, sizeof(db_buff), db_buff, p->id);
			}
		}
		if(cur->itemid != 0){
			PKG_UINT32(msg, cur->itemid, l);
			PKG_UINT32(msg, cur->item_cnt, l);
		}
		p->invade_monster = 0;

		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}
	else if(p->waitcmd == PROTO_MOLE_DO_WORK_FOR_OTHERS ){
		int l = sizeof(protocol_t);
		uint32_t flag = 0;
		if(cur->itemid != 0 && cur->item_cnt != 0){
			flag = 1;
		}
		PKG_UINT32(msg, flag, l);

		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}
	else if(p->waitcmd == PROTO_MOLE_MEET_MERMAID_AGAIN){
		int l = sizeof(protocol_t);
		PKG_UINT32(msg, cur->state, l);
		PKG_UINT32(msg, cur->itemid, l);
		PKG_UINT32(msg, cur->item_cnt,l);
		init_proto_head(msg, p->waitcmd, l);
		return send_to_self(p, msg, l, 1);
	}
	return 0;
}

//
//Mermaid kingdom expedition

int mole_merman_kingdom_expedition_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);
	DEBUG_LOG("id: %u merman_kingdom_expedition",p->id);
	return send_request_to_db(SVR_PROTO_MERMAN_KINGDOM_EXPEDITION, p, 0, NULL, p->id);
}
int mole_merman_kingdom_expedition_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t index;
		uint32_t state;
		uint32_t schedule;
	}merman_real_t;
	DEBUG_LOG("id: %u merman_kingdom_expedition",p->id);
	CHECK_BODY_LEN_GE(len, 4);
	uint32_t count = 0;

	count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, sizeof(count)+ count*sizeof(merman_real_t));
	DEBUG_LOG("id: %u merman_kingdom_expedition",p->id);
	merman_real_t* root = (merman_real_t*)(buf+sizeof(count));
	
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, count, l);
	uint32_t k = 0;
	for(; k < count; ++k){
		DEBUG_LOG("index: %u, state: %u, count: %u", (root+k)->index, (root+k)->state, (root+k)->schedule);
		PKG_UINT32(msg, (root+k)->index, l);
		PKG_UINT32(msg, (root+k)->state, l);
		PKG_UINT32(msg, (root+k)->schedule, l);
	}

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);


}

uint32_t prayers[2];

#define IS_PRAY_ACTIVITY_MAP(mapid_)	(((uint32_t)mapid_) == PRAY_ACTIVITY_MAP)

#define CHECK_IN_PRAY_MAP(_p)		\
	do {\
	if (!(_p)->tiles || !IS_PRAY_ACTIVITY_MAP((_p)->tiles->id)) {\
	return send_to_self_error((_p), (_p)->waitcmd, -ERR_invalid_session, 1);\
	}\
	} while(0)

int notify_client_pray_status(sprite_t *p)
{
    int l = sizeof(protocol_t);
	uint8_t n_msg[256] = {0};
	PKG_UINT32(n_msg, prayers[0], l);
	PKG_UINT32(n_msg, prayers[1], l);
	if(p == NULL){
		init_proto_head(n_msg, PROTO_CRYSTAL_WHOLE_DNANCE_SCENE, l);
		send_to_map3(PRAY_ACTIVITY_MAP, n_msg, l);
		return 0;
	}
	else{
		init_proto_head(n_msg, p->waitcmd, l);
		return send_to_self(p, n_msg, l, 1);
	}
}
int mole_get_pray_info_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	return notify_client_pray_status(p);
}

int mole_contend_for_seat_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t pos = 0;
	int i = 0;
	UNPKG_UINT32(body, pos, i);
	if(pos >= 2){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	uint32_t state = 0;
	if(prayers[pos] == 0){
		state = 1;
		prayers[pos] = p->id;
		notify_client_pray_status(NULL);
	}
	else{
		state = 0;
	}

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, state, l);
	PKG_UINT32(msg, prayers[0], l);
	PKG_UINT32(msg, prayers[1], l);
	init_proto_head(msg, p->waitcmd, l);
	send_to_self(p, msg, l, 1);

	if(prayers[0] != 0 && prayers[1] != 0){
		kick_off_pray_user(NULL);
	}
	return 0;
}

int kick_off_pray_user(sprite_t *p)
{
	if(p != NULL){
		int k = 0;
		for(; k < 2; ++k){
			if(prayers[k] == p->id){
				prayers[k] = 0;
				return notify_client_pray_status(NULL);
			}
		}
	}
	else{
		prayers[0] = 0;
		prayers[1] = 0;
		return notify_client_pray_status(NULL);
	}
	
	return 0;	
}

int mole_get_pray_activity_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);
	CHECK_VALID_ID(p->id);

	return send_request_to_db(SVR_PROTO_GET_PRAY_ACITIVITY, p, 0, NULL, p->id);
	return 0;
	
}
int mole_get_pray_activity_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);

	uint32_t all = *(uint32_t*)buf;
	uint32_t first_step = all % 10;
	uint32_t second_step = (all /10)% 10;
	uint32_t third_step = all / 100;
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, first_step, l);
	PKG_UINT32(msg, second_step, l);
	PKG_UINT32(msg, third_step, l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
	
}

int mole_set_pray_activity_cmd(sprite_t* p, const uint8_t* body, int len)
{

	CHECK_BODY_LEN(len, 4);
	uint32_t step = 0;
	int i = 0;
	UNPKG_UINT32(body, step, i);
	if(step < 1 || step > 3){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	return send_request_to_db(SVR_PROTO_SET_PRAY_ACITIVITY, p, 4, &step, p->id);
}

int mole_set_pray_activity_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t itemid = *(uint32_t*)buf;

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, itemid, l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int mole_get_invade_monster_cmd(sprite_t* p, const uint8_t* body, int len)
{
	// 1, 2, 3, 4 represent monster's index
	CHECK_BODY_LEN(len, 0);

	uint32_t db_type = 50026;
	return send_request_to_db(SVR_PROTO_CHK_IF_STH_DONE, p, 4, &db_type, p->id);

}

int mole_get_invade_monster_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	//uint32_t today_cnt = *(uint32_t*)buf;
	/*
	if(today_cnt >= 150){
		return send_to_self_error(p, p->waitcmd, -ERR_have_got_day_limit, 1);
	}
	*/

	if(p->invade_monster == 0){
		uint32_t random = rand()%100;
		if(random < 50){
			p->invade_monster = 1;
			p->monster_hp = 100;
		}
		else if( random < 70){
			p->invade_monster = 2;
			p->monster_hp = 200;
		}
		else if( random < 90){
			p->invade_monster = 3;
			p->monster_hp = 300;
		}
		else if(random < 100){
			p->invade_monster = 4;
			p->monster_hp = 400;
		}

	}
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, p->invade_monster, l); 
	PKG_UINT32(msg, p->monster_hp, l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);
}

int mole_attack_invade_monster_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t ocean_tear = 0;
	int j = 0;
	UNPKG_UINT32(body, ocean_tear, j);
	*(uint32_t*)p->session = ocean_tear;
	if(ocean_tear > 20){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	DEBUG_LOG("ocean_tear: %u", ocean_tear);
	uint8_t db_buf[128];
	memset(db_buf, 0, sizeof(db_buf));
	uint32_t flag_item[] = {1351390};
	int off = 0;
	PKG_H_UINT32( db_buf, 2, off);
	PKG_H_UINT32( db_buf, sizeof(flag_item)/sizeof(uint32_t), off);
	uint32_t k = 0;
	for(k = 0; k < sizeof(flag_item)/sizeof(uint32_t); ++k){
		PKG_H_UINT32( db_buf, flag_item[k], off);
		pkg_item_kind(p,db_buf, flag_item[k], &off);
	}//for
	
	return send_request_to_db(SVR_PROTO_GET_ITEM_ARRAY, p, off, db_buf, p->id);
}

int mole_attack_invade_monster_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	
	typedef struct items_st{
		uint32_t itemid;
		uint32_t count;
	}items_t;

	uint32_t count = 0;
	CHECK_BODY_LEN_GE(len, sizeof(count));
	count = *(uint32_t*)buf;
	CHECK_BODY_LEN(len, sizeof(count)+ count*sizeof(items_t));
	items_t* head = (items_t*)(buf+sizeof(uint32_t));
	uint32_t ocean_tear = *(uint32_t*)p->session;
	DEBUG_LOG("head->count: %u, count: %u, invade_monster: %u, monster_hp: %u",
		head->count, count, p->invade_monster, p->monster_hp );
	if(head->count < ocean_tear){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	if(p->invade_monster == 0 || p->monster_hp == 0){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}
	uint32_t monster_hp = p->monster_hp >= 5*ocean_tear ? (p->monster_hp - 5*ocean_tear): 0;
	uint32_t db_buff[] = {44, monster_hp};
	return send_request_to_db(SVR_PROTO_SET_CHAPTER_STATE, p, sizeof(db_buff), db_buff, p->id);
	
}



int mole_get_scroll_fragment_cmd(sprite_t* p, const uint8_t* body, int len)
{

	CHECK_BODY_LEN(len, 4);
	uint32_t mapid = 0;
	int j = 0;
	UNPKG_UINT32(body, mapid, j);
	if(mapid < 1 || mapid > 4){
		return send_to_self_error(p, p->waitcmd, -ERR_invalid_session, 1);
	}

	return send_request_to_db(SVR_PROTO_SCROLL_FRAGMENT, p, 4, &mapid, p->id);
}
int mole_get_scroll_fragment_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	typedef struct{
		uint32_t state;
		uint32_t itemid;
	}scroll_frag_t;

	CHECK_BODY_LEN(len, sizeof(scroll_frag_t));
	scroll_frag_t *cur = (scroll_frag_t*)buf;
	int l = sizeof(protocol_t);
	PKG_UINT32(msg, cur->state, l);
	PKG_UINT32(msg, cur->itemid, l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);


}

int get_srcoll_map_state_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);

	return send_request_to_db(SVR_PROTO_SCROLL_MAP_STAT, p, 0, NULL, p->id);

}
int get_srcoll_map_state_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 8);
	
	uint32_t flag = *(uint32_t*)(buf);
	uint32_t state = 0;
	state = *(uint32_t*)(buf+4);

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, flag, l);
	PKG_UINT32(msg, state, l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}

int mole_do_work_for_others_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);

	uint32_t db_buff[] = {251, 0};
	return send_request_to_db(SVR_PROTO_SET_CHAPTER_STATE, p, sizeof(db_buff), db_buff, p->id);
}

int mole_mee_mermaid_again_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t index = 0;
	int j = 0;
	UNPKG_UINT32(body, index, j);
	uint32_t db_buff[] = {46, index};

	return send_request_to_db(SVR_PROTO_SET_CHAPTER_STATE, p, sizeof(db_buff), db_buff, p->id);
}

int mole_finish_merman_kingdom_expedition_cmd(sprite_t* p, const uint8_t* body, int len)
{
	CHECK_BODY_LEN(len, 0);

	return send_request_to_db(SVR_PROTO_FINISH_KINGDOM_EXPEDITION, p, 0, NULL, p->id);
}
int mole_finish_merman_kingdom_expedition_callback(sprite_t *p, uint32_t id, char* buf, int len)
{
	CHECK_BODY_LEN(len, 4);
	uint32_t state = 0;
	state = *(uint32_t*)buf;

	int l = sizeof(protocol_t);
	PKG_UINT32(msg, state, l);

	init_proto_head(msg, p->waitcmd, l);
	return send_to_self(p, msg, l, 1);

}
