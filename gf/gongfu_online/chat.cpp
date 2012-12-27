#include <libtaomee++/inet/pdumanip.hpp>
#include "battle.hpp"

using namespace taomee;

#include "cli_proto.hpp"
#include "message.hpp"
#include "utils.hpp"
#include "player.hpp"
#include "switch.hpp"
#include "trade.hpp"
#include "home.hpp"
#include "chat.hpp"
#include "item.hpp"
#include "coupons.hpp"
#include "common_op.hpp"
#include "mcast_proto.hpp"
#include "trade.hpp"
#include "chat.hpp"

//added by cws 0529
#include<string.h> 
#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <libtaomee++/conf_parser/xmlparser.hpp>
#include "global_data.hpp"

int chat_svr_fd = -1;

/* ! save handles for handling protocol packages from chat server */
chatsvr_handle_t chat_handles[1000];



//////added by cws 0529
int load_one_answer(xmlNodePtr cur, char* ans)
{
	cur = cur->xmlChildrenNode;
	while (cur) {
		char  answer[1024]="";
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>("answer"))) {
			get_xml_prop_raw_str_def(answer, cur, "name","empty_name");
			if(strcmp(answer,"empty_name")==0) return -1;
			else strcpy(ans,answer);
			}
		cur = cur->next;
	}
	return 0;
}

int load_quesAns_config(xmlNodePtr cur)
{
	 struct QA tmp;
	cur = cur->xmlChildrenNode;
	while (cur) {
	 	int index = quesAns.size();
		if (!xmlStrcmp(cur->name, reinterpret_cast<const xmlChar*>("question"))) {
			get_xml_prop_raw_str_def( tmp.questions,  cur,  "name", "empty_name");
				 //DEBUG_LOG("quesAns %d questions is %s",index,tmp->questions);
				
			if (load_one_answer(cur, tmp.answers) == -1) {
				DEBUG_LOG("wrong qa.xml form");
				return  0;
			}
			//DEBUG_LOG("quesAns %d answers is %s \n", index, tmp->answers);
			quesAns.push_back(tmp);
		}
		//DEBUG_LOG("cws quesAnself %d answers is %s", index, quesAns[index].questions);
		//DEBUG_LOG("cws quesAnself %d answers is %s\n", index, quesAns[index].answers);
		cur = cur->next;
	}
	DEBUG_LOG("qa.xml, the end !!!");
	return  0;
}





//-----------------------------------------------------------
// public function declarations
//-----------------------------------------------------------
/**
  * @brief connect to switch and send an init package of online information when online is launched
  */
void connect_to_chat_svr()
{
	chat_svr_fd = connect_to_service(config_get_strval("chat_name"),
										0, 65535, 1);
	if (chat_svr_fd != -1) {
		KDEBUG_LOG(0, "BEGIN CONNECTING TO CHAT SVR AND SEND INIT PKG");

//		send_init_pkg();
	}
}


/**
  * @brief uniform interface to send request to switch server
  * @param p sender 
  * @param cmd command 
  * @param body_len package body length
  * @param sender_id package sender id
  * @return 0 on success, -1 on ret_valor  
  */
int send_to_chat_svr(const player_t* p, userid_t sender_id, uint16_t cmd, void* body_buf, uint32_t body_len)
{
	//Common buffer for sending package to switch
	static uint8_t chat_pkg[chat_proto_max_len];

	//Connect to switch and send init package in case of disconnection
	if (chat_svr_fd == -1) {
		connect_to_chat_svr();
	}
	
	int len = sizeof(chat_proto_t) + body_len;
	if ((chat_svr_fd == -1) || (body_len > static_cast<int>(sizeof(chat_pkg) - sizeof(chat_proto_t)))) {	
		ERROR_LOG("FAILED SENDING PACKAGE TO SWITCH\t[chat_svr_fd=%d]", chat_svr_fd);
		return -1;
	}

	//Build the package
	chat_proto_t* pkg = reinterpret_cast<chat_proto_t*>(chat_pkg);
	pkg->len = len;
	pkg->seq = (p ? ((p->fd) << 16) | (p->waitcmd) : 0); // set higher 16 bytes fd and lower 16 bytes waitcmd
	pkg->cmd = cmd;
	pkg->ret = 0;
	pkg->sender_id  = sender_id;
	//pkg->sender_role_tm  = role_tm;
	memcpy(pkg->body, body_buf, body_len);

	if (p) {
		KDEBUG_LOG(p->id, "SEND REQ TO CHAT\t[uid=%u cmd=%u waitcmd=%u]", p->id, cmd, p->waitcmd);
	}
	return net_send(chat_svr_fd, chat_pkg, len);
}




/**
  *  @brief send chatting pkg to switch server
  *  @param p sender
  *  @param body package body
  *  @param package body length
  */
inline void
chat_cross_chat_svr(const player_t* p, void* body, uint32_t bodylen)
{
	if (send_to_chat_svr(0, p->id, chat_svr_cmd_chat_cross_chat_svr, body, bodylen) == -1) {
		userid_t recvid = *(reinterpret_cast<userid_t*>(body));
		ERROR_LOG("chat_across_svr send err:id=[%u]",recvid);
	}
}


//--------------------------------------------------------------------

/**
 * @brief player talks
 * @param p the player
 * @param body protocol body
 * @param bodylen len of the protocol body
 * @return 0 on success, -1 on error
 */
int chat_cmd(player_t* p, uint8_t* body, uint32_t bodylen)
{
	
	int i = 0;
	uint32_t type;
	uint32_t to_uid;
	uint32_t msg_len;
	unpack(body, type, i);
	unpack(body, to_uid, i);
	unpack(body, msg_len, i);
	TRACE_LOG("%u %u %u", type, to_uid, msg_len);
	CHECK_VAL_LE(msg_len, 512);
	CHECK_VAL_EQ(bodylen - 12, msg_len);
	body[bodylen - 1] = '\0';

	//CHECK_DIRTYWORD(p, body + 12);
	REPLACE_DIRTYWORD(p, body + 12);
	p->waitcmd = 0;
	uint8_t* msg_body = body + 12;
	report_chat_to_monitor(p, to_uid, msg_len, msg_body);
	switch (type) {
		case talk_type_fight_team:
			return fight_team_chat(p, type, to_uid, msg_body, msg_len);
			break;
		case talk_type_tmp_team:
			if (p->temporary_team_id) {
				return temp_team_chat(p, type, to_uid, msg_body, msg_len);
			}
			break;
		case talk_type_trade:
			if (p->trade_grp) {
				uint32_t item_id = 0;
				if (p->my_packs->is_item_exist(trade_mcast_item_id_1)) {
					item_id = trade_mcast_item_id_1;
				} else if (p->my_packs->is_item_exist(trade_mcast_item_id_2)) {
					item_id = trade_mcast_item_id_2;
				}
				if (item_id) {
					db_del_item_elem_t item_elem = { item_id, 1 };
					player_del_items(p, &item_elem, 1, false);
					player_gain_item(p, 2, gold_coin_item_id, get_feedback_coin_cnt(item_id), channel_string_use_tongbao_item);
					return trade_chat(p, type, to_uid, msg_body, msg_len);
				}
			}
			return 0;
			break;
		case talk_type_system:
			break;
		case talk_type_home:
			home_cur_map_chat(p, type, to_uid, msg_body, msg_len);
			break;
		case talk_type_cur_map:
			if (to_uid) {
				return online_chat(p, type, to_uid, msg_len, msg_body);
			}
			if (p->trade_grp) {
				return trade_cur_map_chat(p, type, to_uid, msg_body, msg_len);
			} else if (p->battle_grp) {
				return tmp_team_chat(p, type, to_uid, msg_body, msg_len);
			} else if (p->home_grp) {
				return home_cur_map_chat(p, type, to_uid, msg_body, msg_len);
			} else {
				return online_chat(p, type, to_uid, msg_len, msg_body);
			}
			break;
		default:
			return online_chat(p, type, to_uid, msg_len, msg_body);
			break;
	}
	return 0;
}

int pack_chat_rsp_pkg(uint8_t* pkg, player_t* from_p, uint32_t to_uid, uint32_t type, uint32_t msg_len, uint8_t* msg_body, bool local_flg)
{
	int idx = 0;
	
	if (local_flg) {
		pack_h(pkg, from_p->id, idx);
		pack_h(pkg, from_p->role_tm, idx);
		pack(pkg, from_p->nick, sizeof(from_p->nick), idx);
	
		pack_h(pkg, to_uid, idx);
		pack_h(pkg, type, idx);
		pack_h(pkg, msg_len, idx);

		pack(pkg, msg_body, msg_len, idx);

	} else {
		pack(pkg, from_p->id, idx);
		pack(pkg, from_p->role_tm, idx);		
		pack(pkg, from_p->nick, sizeof(from_p->nick), idx);	
		pack(pkg, to_uid, idx);
		pack(pkg, type, idx);
		pack(pkg, msg_len, idx);

		pack(pkg, msg_body, msg_len, idx);
	}
	return idx;	
}

int trade_chat(player_t* p, uint32_t type, uint32_t to_uid, uint8_t* msg_body, uint32_t msg_len)
{
    player_mcast_t info = {0};
    memset(&info, 0x00, sizeof(player_mcast_t)); 
    info.type = 1; //:player_market 2:player_world
    info.front_id = 2;
    memcpy(&(info.info), msg_body, max_trade_mcast_size);

    notify_player_mcast_info(p, mcast_trade_message, &info);
	return 0;
}

int trade_cur_map_chat(player_t* p, uint32_t type, uint32_t to_uid, uint8_t* msg_body, uint32_t msg_len)
{
	//chat cross trade svr
	int idx = sizeof(cli_proto_t);
	idx += pack_chat_rsp_pkg(dbpkgbuf + idx, p, 0, type, msg_len, msg_body, false);
	init_cli_proto_head(dbpkgbuf, 0, cli_proto_chat, idx);

	trsvr_player_talk(p, dbpkgbuf, idx, to_uid);
	return 0;
}

int home_cur_map_chat(player_t* p, uint32_t type, uint32_t to_uid, uint8_t* msg_body, uint32_t msg_len)
{
	//chat cross trade svr
	int idx = sizeof(cli_proto_t);
	idx += pack_chat_rsp_pkg(dbpkgbuf + idx, p, 0, type, msg_len, msg_body, false);
	init_cli_proto_head(dbpkgbuf, 0, cli_proto_chat, idx);

    if (p->home_grp) {
        homesvr_player_talk(p, dbpkgbuf, idx, to_uid);
    }
	return 0;

}


int fight_team_chat(player_t* p, uint32_t type, uint32_t to_uid, uint8_t* msg_body, uint32_t msg_len)
{
	player_t* to_p = 0;
	if (to_uid) {
		to_p = get_player(to_uid);
	}

	//chat cross chat svr
	//
	int idx = sizeof(cli_proto_t);
	idx += pack_chat_rsp_pkg(pkgbuf + idx, p, to_p ? to_p->id : 0, type, msg_len, msg_body, false);
	init_cli_proto_head(pkgbuf, 0, cli_proto_chat, idx);
	
	transmit_pkg_cross_chat_svr(p, talk_type_fight_team, p->team_info.team_id, pkgbuf, idx);
	return 0;
}	

int temp_team_chat(player_t* p, uint32_t type, uint32_t to_uid, uint8_t* msg_body, uint32_t msg_len)
{
	player_t* to_p = 0;
	if (to_uid) {
		to_p = get_player(to_uid);
	}

	//chat cross chat svr
	//
	int idx = sizeof(cli_proto_t);
	idx += pack_chat_rsp_pkg(pkgbuf + idx, p, to_p ? to_p->id : 0, type, msg_len, msg_body, false);
	init_cli_proto_head(pkgbuf, 0, cli_proto_chat, idx);
	
	transmit_pkg_cross_chat_svr(p, talk_type_tmp_team, p->temporary_team_id, pkgbuf, idx);
	return 0;
}	


int tmp_team_chat(player_t* p, uint32_t type, uint32_t to_uid, uint8_t* msg_body, uint32_t msg_len)
{
	player_t* to_p = 0;
	if (to_uid) {
		to_p = get_player(to_uid);
	}

	//chat cross btl svr
	int idx = sizeof(cli_proto_t);
	idx += pack_chat_rsp_pkg(dbpkgbuf + idx, p, to_p ? to_p->id : 0, type, msg_len, msg_body, false);
	init_cli_proto_head(dbpkgbuf, 0, cli_proto_chat, idx);

	btlsvr_player_talk(p, dbpkgbuf, idx, to_uid);
	return 0;
}	

///added by cws0529
int find_answer(uint8_t* msg_body, char * answer_msg_body)
{
	//save question
	INFO_LOG("QA:%s",msg_body);
	char* tmp_q_user =(char*) msg_body;
	int lenQus = strlen(tmp_q_user);
	if(lenQus >= 1024)
		return -2;
	DEBUG_LOG("cws tmp_q_user is %s,length is %d\n", tmp_q_user, lenQus);
	int tag = quesAns.size();
	for(int i=0; i < tag ; i++){
		regex_t oRegex;
		int nErrCode = 0;
		char szErrMsg[1024] = {0};
		size_t unErrMsgLen = 0;
		if ((nErrCode = regcomp(&oRegex,quesAns[i].questions,REG_EXTENDED)) == 0)
			{
			if ((nErrCode = regexec(&oRegex,tmp_q_user, 0, NULL, 0)) == 0){
				//int lenAns = strlen(quesAns[i].questions);
				DEBUG_LOG("cws %s matches %s",  tmp_q_user, 
					quesAns[i].questions);
				strcpy(answer_msg_body , quesAns[i].answers);
				regfree(&oRegex);
				return 0;
				}
			}
		unErrMsgLen = regerror(nErrCode, &oRegex, szErrMsg, sizeof(szErrMsg));
		unErrMsgLen = unErrMsgLen < sizeof (szErrMsg) ? unErrMsgLen : sizeof(szErrMsg) - 1;
		szErrMsg[unErrMsgLen] = '\0';
		DEBUG_LOG("cws ErrMsg: %s\n", szErrMsg);
		regfree(&oRegex);
		}
	return -1;   
}
    
int online_chat_knowall(player_t* p, uint32_t type, uint32_t to_uid, uint32_t msg_len, uint8_t* msg_body)
{
	int idx = sizeof(cli_proto_t);
	idx += pack_chat_rsp_pkg(pkgbuf + idx, p, to_uid, type, msg_len, msg_body);
	init_cli_proto_head(pkgbuf, p, cli_proto_chat, idx);
	if (to_uid ) {
		send_to_player(p, pkgbuf, idx, 0);
		DEBUG_LOG("CWS to sender ");
	}
	char* answer_msg_body = new char[1024];
	uint32_t  answer_msg_len = 0;
	if(find_answer( msg_body,answer_msg_body ) == -1)
		strcpy(answer_msg_body ,"no answer"); 
	else  if(find_answer( msg_body,answer_msg_body ) == -2)
		strcpy(answer_msg_body ,"question is too long"); 
	answer_msg_len =strlen(answer_msg_body);
	DEBUG_LOG("CWS answer_msg_len is %u", answer_msg_len);
	int idx2 = sizeof(cli_proto_t);
//info of npc knowall
	static player_t wst_npc;
	wst_npc.id = 1000;

	idx2 += pack_chat_rsp_pkg(pkgbuf + idx2, &wst_npc, p->id, 
		type, answer_msg_len, (uint8_t*)answer_msg_body);
	init_cli_proto_head(pkgbuf, p, cli_proto_chat, idx2);
	send_to_player(p, pkgbuf, idx2, 0);
	delete[] answer_msg_body;
	answer_msg_body = NULL;
	return 0;
}
    
    
int online_chat(player_t* p, uint32_t type, uint32_t to_uid, uint32_t msg_len, uint8_t* msg_body)
 {      
 //added by cws 0528
 	if(to_uid == 1000){
		DEBUG_LOG("ask knowall--------");
		online_chat_knowall(p,type,to_uid,msg_len, msg_body);
		return 0;
	}

	player_t* to_p = 0;
	if (to_uid) {
		to_p = get_player(to_uid);
	}
	int idx = sizeof(cli_proto_t);
	idx += pack_chat_rsp_pkg(pkgbuf + idx, p, to_uid, type, msg_len, msg_body);
	init_cli_proto_head(pkgbuf, p, cli_proto_chat, idx);
	if (to_uid && to_p) {
	//在同一个服务器
		send_to_player(p, pkgbuf, idx, 0);
		return send_to_player(to_p, pkgbuf, idx, 0);
	}

	if (!to_uid) {
	//发向online上本地图玩家
		send_to_map(p, pkgbuf, idx, 0);
		return 0;
	} else {
		send_to_player(p, pkgbuf, idx, 0);
	}

	//chat cross chat svr
	//
	idx = 4;
	idx += sizeof(cli_proto_t);
	idx += pack_chat_rsp_pkg(pkgbuf + idx, p, to_uid, type, msg_len, msg_body, false);
	init_cli_proto_head(pkgbuf + 4, p, cli_proto_chat, idx - 4);
	if (to_uid && !to_p) {
	//在不同的服务器
		//chat_cross_chat_svr(p, pkgbuf, idx);
		*(reinterpret_cast<userid_t*>(pkgbuf)) = to_uid;
		chat_across_svr_rltm(p, p->id, pkgbuf,  idx);
		return 0;	
	}
	return 0;
}	

/**
 * @brief chat cmd callback  from chat svr
 */
int chatsvr_chat_cross_chat_svr_callback(player_t* p, chat_proto_t* hpkg)
{
	int idx = sizeof(cli_proto_t);
	pack(pkgbuf, 0, idx);
	init_cli_proto_head(pkgbuf, p, p->waitcmd, idx);
	return send_to_player(p, pkgbuf, idx, 1);
}


int report_user_onoff_to_chat_svr(player_t* p, uint32_t onoff)
{
	int idx = 0;//= sizeof(chat_proto_t);
	pack_h(pkgbuf, onoff, idx);
	pack_h(pkgbuf, p->role_tm, idx);
	pack(pkgbuf, p->nick, sizeof(p->nick), idx);
	pack_h(pkgbuf, (uint32_t)(talk_type_fight_team), idx);
	pack_h(pkgbuf, p->team_info.team_id, idx);
//	init_chat_proto_head(p, p->id, pkgbuf, cli_proto_chat, idx);
	return send_to_chat_svr(0, p->id, chat_svr_cmd_report_user_onoff, pkgbuf, idx);
}	

int report_user_base_info_to_chat_svr(player_t* p, uint32_t talk_group, uint32_t talk_id)
{
	int idx = 0;
	pack_h(pkgbuf, p->role_tm, idx);
	pack(pkgbuf, p->nick, sizeof(p->nick), idx);
	pack_h(pkgbuf, talk_group, idx);
	pack_h(pkgbuf, talk_id, idx);
	return send_to_chat_svr(0, p->id, chat_svr_cmd_report_user_base_info, pkgbuf, idx);
}	


int pack_transmit_pkg(uint8_t* pkg, uint32_t talk_type, uint32_t group_id, void* body, uint32_t len)
{
	int idx = 0;
	pack_h(pkg, talk_type, idx);
	pack_h(pkg, group_id, idx);
	pack_h(pkg, len, idx);
	pack(pkg, body, len, idx);
	return idx;	
}


int transmit_pkg_cross_chat_svr(player_t* p, talk_type_enum talk_type, uint32_t group_id, void* pkg, uint32_t len)
{
	//chat cross chat svr
	//
	static uint8_t chat_tmp_pkg[chat_proto_max_len];
	int idx = 0;
	idx += pack_transmit_pkg(chat_tmp_pkg, talk_type, group_id, pkg, len);
	return send_to_chat_svr(0, p->id, chat_svr_cmd_transmit_pkg_cross_chat_svr, chat_tmp_pkg, idx);
}	

/**
 * @brief transmit pkg cmd callback  from chat svr
 */
int chatsvr_transmit_pkg_cross_chat_svr_callback(player_t* p, chat_proto_t* hpkg)
{
	cli_proto_t* p_rsp = (cli_proto_t*)hpkg->body;
	uint16_t cmd = taomee::bswap(p_rsp->cmd);
	uint16_t len = taomee::bswap(p_rsp->len);
	TRACE_LOG("%u %u %u", p->id, cmd, len);
	switch (cmd) {
		case cli_proto_chat:
			{
				talk_msg_out_head_t* p_head = (talk_msg_out_head_t*)(p_rsp->body);
				TRACE_LOG("%u %u %s %u %u %u", taomee::bswap(p_head->from_id), taomee::bswap(p_head->from_tm), p_head->from_nick, taomee::bswap(p_head->to_id), taomee::bswap(p_head->type), taomee::bswap(p_head->msg_len));
				return send_to_player(p, hpkg->body, len, 0);
			}
			break;
		default:
			{
				return send_to_player(p, hpkg->body, len, 0);
			}
			break;
	};
	return 0;
}




//------------------ other set ---------------------//

/**
  * @brief set home server protocol handle
  */
#define SET_CHAT_HANDLE(op_, len_, cmp_) \
		do { \
			if (chat_handles[chat_svr_cmd_ ## op_ - chat_svr_cmd_start].hdlr != 0) { \
				ERROR_LOG("duplicate cmd=%u name=%s", chat_svr_cmd_ ## op_, #op_); \
				return false; \
			} \
			chat_handles[chat_svr_cmd_ ## op_ - chat_svr_cmd_start] = chatsvr_handle_t(chatsvr_ ## op_ ## _callback, len_, cmp_); \
			DEBUG_LOG("cmd %u ", chat_svr_cmd_ ## op_ - chat_svr_cmd_start);\
		} while (0)

/**
  * @brief init handles to handle protocol packages from home server
  * @return true if all handles are inited successfully, false otherwise
  */
bool init_chat_proto_handles()
{
	/* operation, bodylen, compare method */
	SET_CHAT_HANDLE(chat_cross_chat_svr, sizeof(talk_msg_out_head_t), cmp_must_eq);
	SET_CHAT_HANDLE(transmit_pkg_cross_chat_svr, sizeof(transmit_pkg_rsp_t), cmp_must_ge);
	return true;
}



/**
  * @brief uniform handler to process packages from home server
  * @param data package data
  * @param len package length
  * @param chat_fd home server fd
  */
void handle_chat_svr_return(chat_proto_t* data, uint32_t len, int chat_fd)
{
	player_t* p = get_player(data->sender_id);
	if( p == NULL ){
		ERROR_LOG("chat return error: uid=%u cmd=%u fd=%u",data->sender_id, data->cmd, chat_fd);
		return ;
	}


	if (data->ret) {
		// some error code needs to be handled specially
		send_header_to_player(p, p->waitcmd, data->ret, 0);
		return;
	}

	int err = -1;
	int cmd = data->cmd - chat_svr_cmd_start;
	uint32_t bodylen = data->len - sizeof(chat_proto_t);

	if( !(cmd >= 0 && cmd<(int)(sizeof(chat_handles)/sizeof(chat_handles[0]))))
	{
		err = 1;
		ERROR_LOG("cmd %u", cmd);
		goto ret;
	}

	if(chat_handles[cmd].hdlr == NULL)
	{
		err = 1;
		ERROR_LOG("cmd %u %p", cmd, chat_handles[cmd].hdlr);
		goto ret;
	}

	if ( ((chat_handles[cmd].cmp_method == cmp_must_eq) && (bodylen != chat_handles[cmd].len))
			|| ((chat_handles[cmd].cmp_method == cmp_must_ge) && (bodylen < chat_handles[cmd].len)) ) {
		if ((bodylen != 0) || (data->ret == 0)) {
			ERROR_LOG("invalid package len=%u %u cmpmethod=%d ret=%u uid=%u %u",
						bodylen, chat_handles[cmd].len,
						chat_handles[cmd].cmp_method, data->ret, data->sender_id, cmd);
			goto ret;
		}
	}

	err = chat_handles[cmd].hdlr(p, data);

ret:
	if (err) {
		close_client_conn(p->fd);
	}
}


