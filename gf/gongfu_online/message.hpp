/**
 *============================================================
 *  @file      message.hpp
 *  @brief    message related functions are declared here. (talk_cmd, ...)
 * 
 *  compiler   gcc4.1.2
 *  platform   Linux
 *
 *  copyright:  TaoMee, Inc. ShangHai CN. All rights reserved.
 *
 *============================================================
 */

#ifndef KF_MESSAGE_HPP_
#define KF_MESSAGE_HPP_

#include "fwd_decl.hpp"

/**
  * @brief player talks
  * @param p the player
  * @param body protocol body
  * @param bodylen len of the protocol body
  * @return 0 on success, -1 on error
  */
int talk_cmd(player_t* p, uint8_t* body, uint32_t bodylen);

/**
 * @brief pack simple notification into buf
 * @param buf buffer to pack into 
 * @param type message type
 * @param from message sender
 * @param accept give acceptting or not information
 * @param m player's current map
 * @return the length of buf
 */
int pack_simple_notification(uint8_t* buf, uint32_t type, const player_t* from, uint32_t accept, const map_t* m);


/**
  * @brief pack realtime notification into buf
  * @param buf buffer to pack into 
  * @param type message type
  * @param from message sender
  * @param accept give acceptting or not information
  * @param 
  * @return the length of buf
  */
int pack_rltm_notification(uint8_t* buf, uint16_t cmd, uint32_t type, const player_t* from, uint32_t accept, uint32_t ex_id1 = 0, uint32_t ex_id2 = 0, uint32_t ex_id3 = 0);

/**
 * @brief send a simple notification to 'to'
 * @param to user to send a notification to
 * @param from user from whom the notification is sent
 * @param type type of the notification
 * @param accept accept or not 
 * @param m map of user "from"
 */

void send_simple_notification_with_pre_svr(userid_t to, player_t * from, uint32_t type, uint32_t server_id);

void send_simple_notification(userid_t to, const player_t* from, uint32_t type, uint32_t accept, const map_t* m);

/**
 * @brief send system notification on the end of monthly
 * @param to user to send a notification to
 * @param day 
 */
void send_monthly_timeout_notification(player_t* to, int day);

/**
  * @brief send a real time notification to 'to'
  * @param to user to send a notification to
  * @param from user from whom the notification is sent
  * @param type type of the notification
  * @param accept
  * @param m map
  */
void send_rltm_notification(uint16_t cmd, userid_t to, const player_t* from, uint32_t type, uint32_t accept, uint32_t ex_id1= 0, uint32_t ex_id2 = 0, uint32_t ex_id3 = 0);


/**                                                                           
 * @brief real to send sys-notify to all online player (include home)
 * @param npc the sys-notify sender
 * @param msg the msg to notify
 * @param msglen the length of the notify msg
 */
void sys_notify_to_all(uint32_t npc, const char* msg, int msglen);

/**                                                                           
 * @brief real to sys-notify to one player
 * @param p the player
 * @param npc the sys-notify sender
 * @param msg the msg to notify
 * @param msglen the length of the notify msg
 */
int sys_notify_to_player(player_t* p, uint32_t npc, const char* msg, int msglen);

void report_chat_to_monitor(player_t* p, uint32_t rid, uint32_t msglen, uint8_t* msg);
#endif // KF_MESSAGE_HPP_
