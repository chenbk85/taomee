/**
 *============================================================
 *  @file      login.hpp
 *  @brief    login related functions are declared here
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_LOGIN_HPP_
#define KF_LOGIN_HPP_


/**
  * @brief session for loggin in
  */
struct login_session_t {
	uint32_t	ip;
	uint32_t	tm1;
	uint32_t	uid;
	uint32_t	tm2;
};

enum user_progress_t{
	user_progress_login = 1,
};


int get_sess_id_cmd(player_t* p, uint8_t* body, uint32_t bodylen);
void encrypt_session_id_24(char* buf, int& idx, userid_t uid, uint32_t ip);
void encrypt_session_id_16(char* buf, int& idx, userid_t uid, uint32_t ip);
uint16_t get_award_cnt(player_t* p);

/**
 * @brief send login response to the given player
 * @param player
 * @return 0 on success, -1 on error
 */
int send_login_rsp(player_t* player);

/**
  * @brief set up a timer for kick all the players offline at 12 o'clock
  * @param owner
  * @param data 0: set up a timer; 1: set to kick all the players offline after 30 secs;
  *                     2: kick all the players offline
  * @return 0
  */
int kick_all_users_offline(void* owner, void* data);

int pack_player_secondary_pro_info(player_t* p, void* buf);

//int pack_player_mail_head_list(player_t* p, void* buf);
/**
 * @brief pack the player's fumo info
 * @param p the player
 * @param pkgbuf
 * @return the idx
 */
int pack_player_fumo_info(player_t* p, void* buf);

/**  
 * @brief check session
 * @param p   				player_t*
 * @param gameid			项目类型:
 * @param session			计算得到的session值
 * @param session_len		session长度
 * @param del_flag			验证后是否要删除session(0:不是,1:是)
 * @return 0 on success, -1 on error
**/
int db_check_session(player_t* p, uint32_t gameid, uint8_t* session, 
	uint32_t session_len, uint32_t del_flag);


//------------------------------------------------------------------
// request from client (XXX_cmd)
//------------------------------------------------------------------

/**
  * @brief handle login protocol
  * @param p the player that is logging in
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int login_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

//------------------------------------------------------------------
// callback for handling pkg return from dbproxy (db_XXX_callback)
//------------------------------------------------------------------
int db_check_session_callback(player_t* p, userid_t id, void* body, uint32_t bodylen, uint32_t ret);



#endif // KF_LOGIN_HPP_

