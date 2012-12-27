
/*报文头部<font color=red >小端字节序</font>*/
void cli_proto_header( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*重新获取验证码*/
void login_refresh_img( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	login_refresh_img_out * p_out = P_OUT;
	


}


/*检查游戏是否激活*/
void login_check_game_activate( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	login_check_game_activate_out * p_out = P_OUT;
	


}


/*登录*/
void login_login( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	login_login_out * p_out = P_OUT;
	


}


/*激活游戏*/
void login_activate_game( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	login_activate_game_out * p_out = P_OUT;
	


}


/*得到推荐服务器列表*/
void login_get_recommeded_list( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	login_get_recommeded_list_out * p_out = P_OUT;
	


}


/*得到服务器范围列表*/
void login_get_ranged_svr_list( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	login_get_ranged_svr_list_out * p_out = P_OUT;
	


}


/*登入*/
void cli_login( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_login_out * p_out = P_OUT;
	


}


/*用户注册*/
void cli_reg( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_reg_out * p_out = P_OUT;
	


}


/*通知用户离线*/
void noti_cli_leave( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	noti_cli_leave_out * p_out = P_OUT;
	


}


/*通知用户某人岛屿完成*/
void cli_noti_island_complete( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_island_complete_out * p_out = P_OUT;
	


}


/*完成任务节点*/
void cli_task_complete_node( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_task_complete_node_out * p_out = P_OUT;
	


}


/*得到物品列表*/
void cli_get_item_list( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_item_list_out * p_out = P_OUT;
	


}


/*获取特殊物品数量*/
void cli_get_spec_item( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_spec_item_out * p_out = P_OUT;
	


}


/*进入场景*/
void cli_walk( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*得到用户在该岛上的任务信息*/
void cli_get_user_island_task_info( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_user_island_task_info_out * p_out = P_OUT;
	


}


/*增加找到的地图*/
void cli_find_map_add( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*设置昵称*/
void cli_set_nick( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*设置使用物品列表*/
void cli_set_item_used_list( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*删除任务*/
void cli_task_del_node( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*得到岛上的找到地图的信息*/
void cli_get_user_island_find_map_info( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_user_island_find_map_info_out * p_out = P_OUT;
	


}


/**/
void cli_get_card_list_by_islandid( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_card_list_by_islandid_out * p_out = P_OUT;
	


}


/*和npc聊天*/
void cli_talk_npc( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*得到用户信息*/
void cli_get_user_info( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_user_info_out * p_out = P_OUT;
	


}


/*获取用户的小游戏统计*/
void cli_get_user_game_stat( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_user_game_stat_out * p_out = P_OUT;
	


}


/*通知客户端用户在线时间（分钟）*/
void cli_noti_user_online_time( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_user_online_time_out * p_out = P_OUT;
	


}


/*通知用户第一次进某岛*/
void cli_noti_first_enter_island( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_first_enter_island_out * p_out = P_OUT;
	


}


/*使用特效卡片*/
void cli_set_effect_used( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*通知有人使用特效卡片*/
void cli_noti_effect_used( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_effect_used_out * p_out = P_OUT;
	


}


/*通知客户端得到一些物品*/
void cli_noti_get_item_list( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_get_item_list_out * p_out = P_OUT;
	


}


/*删除一些物品*/
void cli_del_item( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*获取限时岛屿的开放时间*/
void cli_get_island_time( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_island_time_out * p_out = P_OUT;
	


}


/*通知用户离开限时岛屿*/
void cli_noti_leave_island( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_leave_island_out * p_out = P_OUT;
	


}


/*拉取范围的特殊物品*/
void cli_get_spec_item_list( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_spec_item_list_out * p_out = P_OUT;
	


}


/*换肤*/
void cli_set_color( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/**/
void cli_noti_set_color( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_set_color_out * p_out = P_OUT;
	


}


/*通知单个用户在线信息(别人进来了)*/
void cli_noti_one_user_info( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_one_user_info_out * p_out = P_OUT;
	


}


/*得到当前地图的所有人信息*/
void cli_get_all_user_info_from_cur_map( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_all_user_info_from_cur_map_out * p_out = P_OUT;
	


}


/*用户移动 多人模式下*/
void cli_user_move( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*通知用户移动*/
void cli_noti_user_move( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_user_move_out * p_out = P_OUT;
	


}


/*通知用户离开地图*/
void cli_noti_user_left_map( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_user_left_map_out * p_out = P_OUT;
	


}


/*聊天*/
void cli_talk( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*通知用户更换装扮*/
void cli_noti_user_use_clothes_list( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_user_use_clothes_list_out * p_out = P_OUT;
	


}


/*通知聊天*/
void cli_noti_talk( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_talk_out * p_out = P_OUT;
	


}


/*发表情*/
void cli_show( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*通知某用户做表情*/
void cli_noti_show( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_show_out * p_out = P_OUT;
	


}


/*设置忙状态*/
void cli_set_busy_state( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*通知有人升级*/
void cli_noti_user_level_up( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_user_level_up_out * p_out = P_OUT;
	


}


/*请求和某人玩游戏*/
void cli_game_request( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_game_request_out * p_out = P_OUT;
	


}


/*通知游戏请求*/
void cli_noti_game_request( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_game_request_out * p_out = P_OUT;
	


}


/*确认请求*/
void cli_game_play_with_other( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_game_play_with_other_out * p_out = P_OUT;
	


}


/*通知请求游戏是否开始,如果is_start=1,（发给双方)，is_start=0(发给请求方)*/
void cli_noti_game_is_start( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_game_is_start_out * p_out = P_OUT;
	


}


/*客户端确认游戏可以开始了( 1204 返回is_start=1 时，在加载游戏完成时发)*/
void cli_game_can_start( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*通知游戏开始*/
void cli_noti_game_start( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_game_start_out * p_out = P_OUT;
	


}


/*用户游戏操作*/
void cli_game_opt( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*通知用户游戏操作*/
void cli_noti_game_opt( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_game_opt_out * p_out = P_OUT;
	


}


/*当前一局游戏结束*/
void cli_cur_game_end( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*通知游戏结束*/
void cli_noti_game_end( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_game_end_out * p_out = P_OUT;
	


}


/*离开游戏*/
void cli_game_user_left_game( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*用户离开游戏通知*/
void cli_noti_game_user_left_game( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_game_user_left_game_out * p_out = P_OUT;
	


}


/*购买物品*/
void cli_buy_item( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_buy_item_out * p_out = P_OUT;
	


}


/*通知场景中的所有玩家有人进入游戏*/
void cli_noti_game_user( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_game_user_out * p_out = P_OUT;
	


}


/*获取当前场景正在游戏的玩家*/
void cli_get_game_user( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_game_user_out * p_out = P_OUT;
	


}


/*使用游戏积分购买物品*/
void cli_buy_item_use_gamept( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_buy_item_use_gamept_out * p_out = P_OUT;
	


}


/*获取游戏座位信息*/
void cli_noti_game_seat( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_game_seat_out * p_out = P_OUT;
	


}


/*拉取有效的小游戏积分*/
void cli_get_valid_gamept( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_valid_gamept_out * p_out = P_OUT;
	


}


/*投稿投诉*/
void cli_post_msg( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*打工获得抽奖机会*/
void cli_work_get_lottery( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_work_get_lottery_out * p_out = P_OUT;
	


}


/*抽奖*/
void cli_draw_lottery( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_draw_lottery_out * p_out = P_OUT;
	


}


/*获取剩余抽奖机会*/
void cli_get_lottery_count( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_lottery_count_out * p_out = P_OUT;
	


}


/*挂铃铛赢奖励*/
void cli_hang_bell_get_item( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_hang_bell_get_item_out * p_out = P_OUT;
	


}


/*点击圣诞树统计*/
void cli_click_chris_tree( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*点击新手指引*/
void cli_click_guide( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*点击统计*/
void cli_click_stat( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*点击游戏王公告*/
void cli_click_game_notice( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*通知用户得到邀请函*/
void cli_noti_get_invitation( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_get_invitation_out * p_out = P_OUT;
	


}


/*通知客户端系统时间*/
void cli_noti_svr_time( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_noti_svr_time_out * p_out = P_OUT;
	


}


/*点击宠物蛋*/
void cli_click_pet_egg( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	


}


/*获取登录宝箱*/
void cli_get_login_chest( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_login_chest_out * p_out = P_OUT;
	


}


/*获取登录奖励*/
void cli_get_login_reward( Csprite* p, uint32_t id,  Cmessage* c_out, uint32_t ret)
{
	if (ret!=SUCC){
		return ;
	}
	cli_get_login_reward_out * p_out = P_OUT;
	


}

