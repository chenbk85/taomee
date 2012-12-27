/*得到版本信息*/
int get_server_version(Csprite *p, Cmessage* c_in)
{
	get_server_version_out	cli_out ;



	return 0;
}

/*登入*/
int pop_login(Csprite *p, Cmessage* c_in)
{
	pop_login_out	cli_out ;



	return 0;
}

/*注册*/
int pop_reg(Csprite *p, Cmessage* c_in)
{
	pop_reg_in * p_in = P_IN;



	return 0;
}

/*设置昵称*/
int pop_set_nick(Csprite *p, Cmessage* c_in)
{
	pop_set_nick_in * p_in = P_IN;



	return 0;
}

/*复制用户*/
int pop_copy_user(Csprite *p, Cmessage* c_in)
{
	pop_copy_user_in * p_in = P_IN;



	return 0;
}

/**/
int pop_get_base_info(Csprite *p, Cmessage* c_in)
{
	pop_get_base_info_out	cli_out ;



	return 0;
}

/*设置用户标识*/
int pop_set_flag(Csprite *p, Cmessage* c_in)
{
	pop_set_flag_in * p_in = P_IN;



	return 0;
}

/**/
int pop_set_color(Csprite *p, Cmessage* c_in)
{
	pop_set_color_in * p_in = P_IN;



	return 0;
}

/*交换协议，物品，任务，钱*/
int pop_exchange(Csprite *p, Cmessage* c_in)
{
	pop_exchange_in * p_in = P_IN;



	return 0;
}

/*设置使用列表，会将原有使用的设置为未使用*/
int pop_set_item_used_list(Csprite *p, Cmessage* c_in)
{
	pop_set_item_used_list_in * p_in = P_IN;



	return 0;
}

/*完成子节点任务*/
int pop_task_complete_node(Csprite *p, Cmessage* c_in)
{
	pop_task_complete_node_in * p_in = P_IN;



	return 0;
}

/*删除任务*/
int pop_task_del(Csprite *p, Cmessage* c_in)
{
	pop_task_del_in * p_in = P_IN;



	return 0;
}

/*找到哪个地图*/
int pop_find_map_add(Csprite *p, Cmessage* c_in)
{
	pop_find_map_add_in * p_in = P_IN;



	return 0;
}

/*退出*/
int pop_logout(Csprite *p, Cmessage* c_in)
{
	pop_logout_in * p_in = P_IN;



	return 0;
}

/*用户足迹*/
int pop_user_log_add(Csprite *p, Cmessage* c_in)
{
	pop_user_log_add_in * p_in = P_IN;



	return 0;
}

/*得到用户的所有信息*/
int pop_get_all_info(Csprite *p, Cmessage* c_in)
{
	pop_get_all_info_out	cli_out ;



	return 0;
}

/*设置user表某一字段的值*/
int pop_user_set_field_value(Csprite *p, Cmessage* c_in)
{
	pop_user_set_field_value_in * p_in = P_IN;



	return 0;
}

/*设置相关的值*/
int pop_opt(Csprite *p, Cmessage* c_in)
{
	pop_opt_in * p_in = P_IN;



	return 0;
}

/*增加小游戏统计*/
int pop_add_game_info(Csprite *p, Cmessage* c_in)
{
	pop_add_game_info_in * p_in = P_IN;



	return 0;
}

/*udp 投稿*/
int udp_post_msg(Csprite *p, Cmessage* c_in)
{
	udp_post_msg_in * p_in = P_IN;



	return 0;
}

