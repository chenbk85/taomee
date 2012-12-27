/*报文头部<font color=red >小端字节序</font>*/
int cli_proto_header(Csprite *p, Cmessage* c_in)
{
	cli_proto_header_in * p_in = P_IN;



	return 0;
}

/*重新获取验证码*/
int login_refresh_img(Csprite *p, Cmessage* c_in)
{
	login_refresh_img_in * p_in = P_IN;
	login_refresh_img_out	cli_out ;



	return 0;
}

/*检查游戏是否激活*/
int login_check_game_activate(Csprite *p, Cmessage* c_in)
{
	login_check_game_activate_in * p_in = P_IN;
	login_check_game_activate_out	cli_out ;



	return 0;
}

/*登录*/
int login_login(Csprite *p, Cmessage* c_in)
{
	login_login_in * p_in = P_IN;
	login_login_out	cli_out ;



	return 0;
}

/*激活游戏*/
int login_activate_game(Csprite *p, Cmessage* c_in)
{
	login_activate_game_in * p_in = P_IN;
	login_activate_game_out	cli_out ;



	return 0;
}

/*得到推荐服务器列表*/
int login_get_recommeded_list(Csprite *p, Cmessage* c_in)
{
	login_get_recommeded_list_in * p_in = P_IN;
	login_get_recommeded_list_out	cli_out ;



	return 0;
}

/*得到服务器范围列表*/
int login_get_ranged_svr_list(Csprite *p, Cmessage* c_in)
{
	login_get_ranged_svr_list_in * p_in = P_IN;
	login_get_ranged_svr_list_out	cli_out ;



	return 0;
}

/*登入*/
int cli_login(Csprite *p, Cmessage* c_in)
{
	cli_login_in * p_in = P_IN;
	cli_login_out	cli_out ;



	return 0;
}

/*用户注册*/
int cli_reg(Csprite *p, Cmessage* c_in)
{
	cli_reg_in * p_in = P_IN;
	cli_reg_out	cli_out ;



	return 0;
}

/*通知用户离线*/
int noti_cli_leave(Csprite *p, Cmessage* c_in)
{
	noti_cli_leave_out	cli_out ;



	return 0;
}

/*通知用户某人岛屿完成*/
int cli_noti_island_complete(Csprite *p, Cmessage* c_in)
{
	cli_noti_island_complete_out	cli_out ;



	return 0;
}

/*完成任务节点*/
int cli_task_complete_node(Csprite *p, Cmessage* c_in)
{
	cli_task_complete_node_in * p_in = P_IN;
	cli_task_complete_node_out	cli_out ;



	return 0;
}

/*得到物品列表*/
int cli_get_item_list(Csprite *p, Cmessage* c_in)
{
	cli_get_item_list_in * p_in = P_IN;
	cli_get_item_list_out	cli_out ;



	return 0;
}

/*获取特殊物品数量*/
int cli_get_spec_item(Csprite *p, Cmessage* c_in)
{
	cli_get_spec_item_in * p_in = P_IN;
	cli_get_spec_item_out	cli_out ;



	return 0;
}

/*进入场景*/
int cli_walk(Csprite *p, Cmessage* c_in)
{
	cli_walk_in * p_in = P_IN;



	return 0;
}

/*得到用户在该岛上的任务信息*/
int cli_get_user_island_task_info(Csprite *p, Cmessage* c_in)
{
	cli_get_user_island_task_info_in * p_in = P_IN;
	cli_get_user_island_task_info_out	cli_out ;



	return 0;
}

/*增加找到的地图*/
int cli_find_map_add(Csprite *p, Cmessage* c_in)
{
	cli_find_map_add_in * p_in = P_IN;



	return 0;
}

/*设置昵称*/
int cli_set_nick(Csprite *p, Cmessage* c_in)
{
	cli_set_nick_in * p_in = P_IN;



	return 0;
}

/*设置使用物品列表*/
int cli_set_item_used_list(Csprite *p, Cmessage* c_in)
{
	cli_set_item_used_list_in * p_in = P_IN;



	return 0;
}

/*删除任务*/
int cli_task_del_node(Csprite *p, Cmessage* c_in)
{
	cli_task_del_node_in * p_in = P_IN;



	return 0;
}

/*得到岛上的找到地图的信息*/
int cli_get_user_island_find_map_info(Csprite *p, Cmessage* c_in)
{
	cli_get_user_island_find_map_info_in * p_in = P_IN;
	cli_get_user_island_find_map_info_out	cli_out ;



	return 0;
}

/**/
int cli_get_card_list_by_islandid(Csprite *p, Cmessage* c_in)
{
	cli_get_card_list_by_islandid_in * p_in = P_IN;
	cli_get_card_list_by_islandid_out	cli_out ;



	return 0;
}

/*和npc聊天*/
int cli_talk_npc(Csprite *p, Cmessage* c_in)
{
	cli_talk_npc_in * p_in = P_IN;



	return 0;
}

/*得到用户信息*/
int cli_get_user_info(Csprite *p, Cmessage* c_in)
{
	cli_get_user_info_in * p_in = P_IN;
	cli_get_user_info_out	cli_out ;



	return 0;
}

/*获取用户的小游戏统计*/
int cli_get_user_game_stat(Csprite *p, Cmessage* c_in)
{
	cli_get_user_game_stat_in * p_in = P_IN;
	cli_get_user_game_stat_out	cli_out ;



	return 0;
}

/*通知客户端用户在线时间（分钟）*/
int cli_noti_user_online_time(Csprite *p, Cmessage* c_in)
{
	cli_noti_user_online_time_out	cli_out ;



	return 0;
}

/*通知用户第一次进某岛*/
int cli_noti_first_enter_island(Csprite *p, Cmessage* c_in)
{
	cli_noti_first_enter_island_out	cli_out ;



	return 0;
}

/*使用特效卡片*/
int cli_set_effect_used(Csprite *p, Cmessage* c_in)
{
	cli_set_effect_used_in * p_in = P_IN;



	return 0;
}

/*通知有人使用特效卡片*/
int cli_noti_effect_used(Csprite *p, Cmessage* c_in)
{
	cli_noti_effect_used_out	cli_out ;



	return 0;
}

/*通知客户端得到一些物品*/
int cli_noti_get_item_list(Csprite *p, Cmessage* c_in)
{
	cli_noti_get_item_list_out	cli_out ;



	return 0;
}

/*删除一些物品*/
int cli_del_item(Csprite *p, Cmessage* c_in)
{
	cli_del_item_in * p_in = P_IN;



	return 0;
}

/*获取限时岛屿的开放时间*/
int cli_get_island_time(Csprite *p, Cmessage* c_in)
{
	cli_get_island_time_out	cli_out ;



	return 0;
}

/*通知用户离开限时岛屿*/
int cli_noti_leave_island(Csprite *p, Cmessage* c_in)
{
	cli_noti_leave_island_out	cli_out ;



	return 0;
}

/*拉取范围的特殊物品*/
int cli_get_spec_item_list(Csprite *p, Cmessage* c_in)
{
	cli_get_spec_item_list_in * p_in = P_IN;
	cli_get_spec_item_list_out	cli_out ;



	return 0;
}

/*换肤*/
int cli_set_color(Csprite *p, Cmessage* c_in)
{
	cli_set_color_in * p_in = P_IN;



	return 0;
}

/**/
int cli_noti_set_color(Csprite *p, Cmessage* c_in)
{
	cli_noti_set_color_out	cli_out ;



	return 0;
}

/*通知单个用户在线信息(别人进来了)*/
int cli_noti_one_user_info(Csprite *p, Cmessage* c_in)
{
	cli_noti_one_user_info_out	cli_out ;



	return 0;
}

/*得到当前地图的所有人信息*/
int cli_get_all_user_info_from_cur_map(Csprite *p, Cmessage* c_in)
{
	cli_get_all_user_info_from_cur_map_out	cli_out ;



	return 0;
}

/*用户移动 多人模式下*/
int cli_user_move(Csprite *p, Cmessage* c_in)
{
	cli_user_move_in * p_in = P_IN;



	return 0;
}

/*通知用户移动*/
int cli_noti_user_move(Csprite *p, Cmessage* c_in)
{
	cli_noti_user_move_out	cli_out ;



	return 0;
}

/*通知用户离开地图*/
int cli_noti_user_left_map(Csprite *p, Cmessage* c_in)
{
	cli_noti_user_left_map_out	cli_out ;



	return 0;
}

/*聊天*/
int cli_talk(Csprite *p, Cmessage* c_in)
{
	cli_talk_in * p_in = P_IN;



	return 0;
}

/*通知用户更换装扮*/
int cli_noti_user_use_clothes_list(Csprite *p, Cmessage* c_in)
{
	cli_noti_user_use_clothes_list_out	cli_out ;



	return 0;
}

/*通知聊天*/
int cli_noti_talk(Csprite *p, Cmessage* c_in)
{
	cli_noti_talk_out	cli_out ;



	return 0;
}

/*发表情*/
int cli_show(Csprite *p, Cmessage* c_in)
{
	cli_show_in * p_in = P_IN;



	return 0;
}

/*通知某用户做表情*/
int cli_noti_show(Csprite *p, Cmessage* c_in)
{
	cli_noti_show_out	cli_out ;



	return 0;
}

/*设置忙状态*/
int cli_set_busy_state(Csprite *p, Cmessage* c_in)
{
	cli_set_busy_state_in * p_in = P_IN;



	return 0;
}

/*通知有人升级*/
int cli_noti_user_level_up(Csprite *p, Cmessage* c_in)
{
	cli_noti_user_level_up_out	cli_out ;



	return 0;
}

/*请求和某人玩游戏*/
int cli_game_request(Csprite *p, Cmessage* c_in)
{
	cli_game_request_in * p_in = P_IN;
	cli_game_request_out	cli_out ;



	return 0;
}

/*通知游戏请求*/
int cli_noti_game_request(Csprite *p, Cmessage* c_in)
{
	cli_noti_game_request_out	cli_out ;



	return 0;
}

/*确认请求*/
int cli_game_play_with_other(Csprite *p, Cmessage* c_in)
{
	cli_game_play_with_other_in * p_in = P_IN;
	cli_game_play_with_other_out	cli_out ;



	return 0;
}

/*通知请求游戏是否开始,如果is_start=1,（发给双方)，is_start=0(发给请求方)*/
int cli_noti_game_is_start(Csprite *p, Cmessage* c_in)
{
	cli_noti_game_is_start_out	cli_out ;



	return 0;
}

/*客户端确认游戏可以开始了( 1204 返回is_start=1 时，在加载游戏完成时发)*/
int cli_game_can_start(Csprite *p, Cmessage* c_in)
{



	return 0;
}

/*通知游戏开始*/
int cli_noti_game_start(Csprite *p, Cmessage* c_in)
{
	cli_noti_game_start_out	cli_out ;



	return 0;
}

/*用户游戏操作*/
int cli_game_opt(Csprite *p, Cmessage* c_in)
{
	cli_game_opt_in * p_in = P_IN;



	return 0;
}

/*通知用户游戏操作*/
int cli_noti_game_opt(Csprite *p, Cmessage* c_in)
{
	cli_noti_game_opt_out	cli_out ;



	return 0;
}

/*当前一局游戏结束*/
int cli_cur_game_end(Csprite *p, Cmessage* c_in)
{
	cli_cur_game_end_in * p_in = P_IN;



	return 0;
}

/*通知游戏结束*/
int cli_noti_game_end(Csprite *p, Cmessage* c_in)
{
	cli_noti_game_end_out	cli_out ;



	return 0;
}

/*离开游戏*/
int cli_game_user_left_game(Csprite *p, Cmessage* c_in)
{



	return 0;
}

/*用户离开游戏通知*/
int cli_noti_game_user_left_game(Csprite *p, Cmessage* c_in)
{
	cli_noti_game_user_left_game_out	cli_out ;



	return 0;
}

/*购买物品*/
int cli_buy_item(Csprite *p, Cmessage* c_in)
{
	cli_buy_item_in * p_in = P_IN;
	cli_buy_item_out	cli_out ;



	return 0;
}

/*通知场景中的所有玩家有人进入游戏*/
int cli_noti_game_user(Csprite *p, Cmessage* c_in)
{
	cli_noti_game_user_out	cli_out ;



	return 0;
}

/*获取当前场景正在游戏的玩家*/
int cli_get_game_user(Csprite *p, Cmessage* c_in)
{
	cli_get_game_user_out	cli_out ;



	return 0;
}

/*使用游戏积分购买物品*/
int cli_buy_item_use_gamept(Csprite *p, Cmessage* c_in)
{
	cli_buy_item_use_gamept_in * p_in = P_IN;
	cli_buy_item_use_gamept_out	cli_out ;



	return 0;
}

/*获取游戏座位信息*/
int cli_noti_game_seat(Csprite *p, Cmessage* c_in)
{
	cli_noti_game_seat_out	cli_out ;



	return 0;
}

/*拉取有效的小游戏积分*/
int cli_get_valid_gamept(Csprite *p, Cmessage* c_in)
{
	cli_get_valid_gamept_out	cli_out ;



	return 0;
}

/*投稿投诉*/
int cli_post_msg(Csprite *p, Cmessage* c_in)
{
	cli_post_msg_in * p_in = P_IN;



	return 0;
}

/*打工获得抽奖机会*/
int cli_work_get_lottery(Csprite *p, Cmessage* c_in)
{
	cli_work_get_lottery_out	cli_out ;



	return 0;
}

/*抽奖*/
int cli_draw_lottery(Csprite *p, Cmessage* c_in)
{
	cli_draw_lottery_out	cli_out ;



	return 0;
}

/*获取剩余抽奖机会*/
int cli_get_lottery_count(Csprite *p, Cmessage* c_in)
{
	cli_get_lottery_count_out	cli_out ;



	return 0;
}

/*挂铃铛赢奖励*/
int cli_hang_bell_get_item(Csprite *p, Cmessage* c_in)
{
	cli_hang_bell_get_item_in * p_in = P_IN;
	cli_hang_bell_get_item_out	cli_out ;



	return 0;
}

/*点击圣诞树统计*/
int cli_click_chris_tree(Csprite *p, Cmessage* c_in)
{



	return 0;
}

/*点击新手指引*/
int cli_click_guide(Csprite *p, Cmessage* c_in)
{



	return 0;
}

/*点击统计*/
int cli_click_stat(Csprite *p, Cmessage* c_in)
{
	cli_click_stat_in * p_in = P_IN;



	return 0;
}

/*点击游戏王公告*/
int cli_click_game_notice(Csprite *p, Cmessage* c_in)
{



	return 0;
}

/*通知用户得到邀请函*/
int cli_noti_get_invitation(Csprite *p, Cmessage* c_in)
{
	cli_noti_get_invitation_out	cli_out ;



	return 0;
}

/*通知客户端系统时间*/
int cli_noti_svr_time(Csprite *p, Cmessage* c_in)
{
	cli_noti_svr_time_out	cli_out ;



	return 0;
}

/*点击宠物蛋*/
int cli_click_pet_egg(Csprite *p, Cmessage* c_in)
{



	return 0;
}

/*获取登录宝箱*/
int cli_get_login_chest(Csprite *p, Cmessage* c_in)
{
	cli_get_login_chest_out	cli_out ;



	return 0;
}

/*获取登录奖励*/
int cli_get_login_reward(Csprite *p, Cmessage* c_in)
{
	cli_get_login_reward_in * p_in = P_IN;
	cli_get_login_reward_out	cli_out ;



	return 0;
}

