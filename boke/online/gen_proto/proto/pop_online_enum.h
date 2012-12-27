#ifndef  POP_ONLINE_ENUM_H
#define  POP_ONLINE_ENUM_H

/*命令定义*/	
enum  enum_pop_online_cmd{
	 cli_proto_header_cmd		=	1, /*报文头部<font color=red >小端字节序</font>*/
	 login_refresh_img_cmd		=	101, /*重新获取验证码*/
	 login_check_game_activate_cmd		=	102, /*检查游戏是否激活*/
	 login_login_cmd		=	103, /*登录*/
	 login_activate_game_cmd		=	104, /*激活游戏*/
	 login_get_recommeded_list_cmd		=	105, /*得到推荐服务器列表*/
	 login_get_ranged_svr_list_cmd		=	106, /*得到服务器范围列表*/
	 cli_login_cmd		=	1001, /*登入*/
	 cli_reg_cmd		=	1002, /*用户注册*/
	 noti_cli_leave_cmd		=	1003, /*通知用户离线*/
	 cli_noti_island_complete_cmd		=	1004, /*通知用户某人岛屿完成*/
	 cli_task_complete_node_cmd		=	1011, /*完成任务节点*/
	 cli_get_item_list_cmd		=	1012, /*得到物品列表*/
	 cli_get_spec_item_cmd		=	1013, /*获取特殊物品数量*/
	 cli_walk_cmd		=	1014, /*进入场景*/
	 cli_get_user_island_task_info_cmd		=	1015, /*得到用户在该岛上的任务信息*/
	 cli_find_map_add_cmd		=	1016, /*增加找到的地图*/
	 cli_set_nick_cmd		=	1017, /*设置昵称*/
	 cli_set_item_used_list_cmd		=	1018, /*设置使用物品列表*/
	 cli_task_del_node_cmd		=	1019, /*删除任务*/
	 cli_get_user_island_find_map_info_cmd		=	1020, /*得到岛上的找到地图的信息*/
	 cli_get_card_list_by_islandid_cmd		=	1021, /**/
	 cli_talk_npc_cmd		=	1022, /*和npc聊天*/
	 cli_get_user_info_cmd		=	1023, /*得到用户信息*/
	 cli_get_user_game_stat_cmd		=	1024, /*获取用户的小游戏统计*/
	 cli_noti_user_online_time_cmd		=	1025, /*通知客户端用户在线时间（分钟）*/
	 cli_noti_first_enter_island_cmd		=	1026, /*通知用户第一次进某岛*/
	 cli_set_effect_used_cmd		=	1028, /*使用特效卡片*/
	 cli_noti_effect_used_cmd		=	1029, /*通知有人使用特效卡片*/
	 cli_noti_get_item_list_cmd		=	1030, /*通知客户端得到一些物品*/
	 cli_del_item_cmd		=	1031, /*删除一些物品*/
	 cli_get_island_time_cmd		=	1032, /*获取限时岛屿的开放时间*/
	 cli_noti_leave_island_cmd		=	1033, /*通知用户离开限时岛屿*/
	 cli_get_spec_item_list_cmd		=	1034, /*拉取范围的特殊物品*/
	 cli_set_color_cmd		=	1035, /*换肤*/
	 cli_noti_set_color_cmd		=	1036, /**/
	 cli_noti_one_user_info_cmd		=	1100, /*通知单个用户在线信息(别人进来了)*/
	 cli_get_all_user_info_from_cur_map_cmd		=	1101, /*得到当前地图的所有人信息*/
	 cli_user_move_cmd		=	1102, /*用户移动 多人模式下*/
	 cli_noti_user_move_cmd		=	1103, /*通知用户移动*/
	 cli_noti_user_left_map_cmd		=	1104, /*通知用户离开地图*/
	 cli_talk_cmd		=	1105, /*聊天*/
	 cli_noti_user_use_clothes_list_cmd		=	1106, /*通知用户更换装扮*/
	 cli_noti_talk_cmd		=	1107, /*通知聊天*/
	 cli_show_cmd		=	1108, /*发表情*/
	 cli_noti_show_cmd		=	1109, /*通知某用户做表情*/
	 cli_set_busy_state_cmd		=	1110, /*设置忙状态*/
	 cli_noti_user_level_up_cmd		=	1111, /*通知有人升级*/
	 cli_game_request_cmd		=	1201, /*请求和某人玩游戏*/
	 cli_noti_game_request_cmd		=	1202, /*通知游戏请求*/
	 cli_game_play_with_other_cmd		=	1203, /*确认请求*/
	 cli_noti_game_is_start_cmd		=	1204, /*通知请求游戏是否开始,如果is_start=1,（发给双方)，is_start=0(发给请求方)*/
	 cli_game_can_start_cmd		=	1205, /*客户端确认游戏可以开始了( 1204 返回is_start=1 时，在加载游戏完成时发)*/
	 cli_noti_game_start_cmd		=	1206, /*通知游戏开始*/
	 cli_game_opt_cmd		=	1207, /*用户游戏操作*/
	 cli_noti_game_opt_cmd		=	1208, /*通知用户游戏操作*/
	 cli_cur_game_end_cmd		=	1209, /*当前一局游戏结束*/
	 cli_noti_game_end_cmd		=	1210, /*通知游戏结束*/
	 cli_game_user_left_game_cmd		=	1211, /*离开游戏*/
	 cli_noti_game_user_left_game_cmd		=	1212, /*用户离开游戏通知*/
	 cli_buy_item_cmd		=	1213, /*购买物品*/
	 cli_noti_game_user_cmd		=	1214, /*通知场景中的所有玩家有人进入游戏*/
	 cli_get_game_user_cmd		=	1215, /*获取当前场景正在游戏的玩家*/
	 cli_buy_item_use_gamept_cmd		=	1216, /*使用游戏积分购买物品*/
	 cli_noti_game_seat_cmd		=	1217, /*获取游戏座位信息*/
	 cli_get_valid_gamept_cmd		=	1218, /*拉取有效的小游戏积分*/
	 cli_post_msg_cmd		=	1301, /*投稿投诉*/
	 cli_work_get_lottery_cmd		=	1401, /*打工获得抽奖机会*/
	 cli_draw_lottery_cmd		=	1402, /*抽奖*/
	 cli_get_lottery_count_cmd		=	1403, /*获取剩余抽奖机会*/
	 cli_hang_bell_get_item_cmd		=	1404, /*挂铃铛赢奖励*/
	 cli_click_chris_tree_cmd		=	1405, /*点击圣诞树统计*/
	 cli_click_guide_cmd		=	1406, /*点击新手指引*/
	 cli_click_stat_cmd		=	1407, /*点击统计*/
	 cli_click_game_notice_cmd		=	1408, /*点击游戏王公告*/
	 cli_noti_get_invitation_cmd		=	1409, /*通知用户得到邀请函*/
	 cli_noti_svr_time_cmd		=	1410, /*通知客户端系统时间*/
	 cli_click_pet_egg_cmd		=	1411, /*点击宠物蛋*/
	 cli_get_login_chest_cmd		=	1412, /*获取登录宝箱*/
	 cli_get_login_reward_cmd		=	1413, /*获取登录奖励*/

};

/*错误码定义*/	
enum  enum_pop_online_error{
	 clierr_system_error		=	5001, /*系统出错*/
	 clierr_user_is_banned		=	5002, /*号码被禁用了*/
	 clierr_wrong_password		=	5003, /*密码错误*/
	 clierr_userid_not_found		=	5005, /*系统用户不存在*/
	 clierr_wrong_password_too_much		=	5009, /*用户被暂时禁止登录，密码出错太多*/
	 cli_userid_nofind_err		=	11105, /*用户不存在*/
	 cli_err_system_error		=	20006, /*系统繁忙*/
	 cli_request_arg_err		=	20007, /*请求参数不合法*/
	 cli_login_other_online_err		=	20008, /*在其它地方登入了*/
	 cli_task_node_existed_err		=	20010, /*已经交过任务了*/
	 cli_task_node_nofind_err		=	20011, /*任务id不存在*/
	 cli_task_node_need_item_nofind_err		=	20012, /*需要的物品还没存在*/
	 cli_task_node_req_task_err		=	20013, /*前置任务还没有完成*/
	 cli_mapid_existed_err		=	20014, /*地图id已经存在*/
	 cli_nick_err		=	20015, /*昵称非法*/
	 cli_user_no_in_map_err		=	20016, /*用户已不在地图中*/
	 cli_user_in_game_err		=	20017, /*用户已经在游戏中了*/
	 cli_xiaomee_max_err		=	20018, /*钱到达上限*/
	 cli_xiaomee_no_enough_err		=	20019, /*钱不够*/
	 cli_itemid_existed_err		=	20020, /*物品已经存在*/
	 cli_dirty_word_err		=	20021, /*有脏词*/
	 cli_item_ever_get_err		=	20022, /*已经领过物品了*/
	 cli_work_today_err		=	20023, /*今天已经打过工了*/
	 cli_lottery_no_enough_err		=	20024, /*抽奖次数不足*/
	 cli_island_no_open_err		=	20025, /*岛屿没开放*/

};



#endif // POP_ONLINE_ENUM_H
